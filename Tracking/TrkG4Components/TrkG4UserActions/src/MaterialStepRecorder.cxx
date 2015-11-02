/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MaterialStepRecorder.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "TrkG4UserActions/MaterialStepRecorder.h"
#include "TrkGeometry/MaterialStep.h"
#include <iostream>
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/INTupleSvc.h" 
#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/MsgStream.h"
#include "SimHelpers/ServiceAccessor.h"
#include "StoreGate/StoreGateSvc.h"
#include "G4Step.hh"
#include "G4Material.hh"
#include "G4Element.hh"
#include "G4StepPoint.hh"
#include "G4TouchableHistory.hh"
#include "G4LogicalVolume.hh"
#include <climits>
#include <cmath>

static MaterialStepRecorder sntp("MaterialStepRecorder");

MaterialStepRecorder::MaterialStepRecorder(std::string s)
: FADS::UserAction(s),
  m_storeGate(0),
  m_matStepCollection(0),
  m_matStepCollectionName("MaterialStepRecords"),
  m_recordComposition(true),
  m_totalNbOfAtoms(0),
  m_totalSteps(0),
  m_eventID(0),
  m_verboseLevel(0),
  m_elementTable(0),
  m_elementTableName("ElementTable"),
  m_runElementTable(0)
{}


void MaterialStepRecorder::BeginOfEventAction(const G4Event*)
{

    if (m_verboseLevel) std::cout << "[MaterialStepRecorder]   BeginOfEventAction" << std::endl;

    if (!m_storeGate) m_storeGate = storeGateSvc();

    // create a new Collection
    m_matStepCollection = new Trk::MaterialStepCollection;

}


void MaterialStepRecorder::EndOfEventAction(const G4Event*)
{

    if (m_verboseLevel) std::cout << "[MaterialStepRecorder]   EndOfEventAction" << std::endl;

    ++m_eventID;

    // write the Collection to StoreGate
    if ( m_storeGate->record(m_matStepCollection, m_matStepCollectionName, false).isFailure() ) {
        std::cout << "[MaterialStepRecorder]   EndOfEventAction : recording MaterialStep collection in StoreGate FAILED" << std::endl;
    }

    // write out the ElementTable of this event
    if ( m_storeGate->record(m_elementTable, m_elementTableName, false).isFailure() ){
        std::cout << "[MaterialStepRecorder]   EndOfEventAction : recording ElementTable in StoreGate FAILED" << std::endl;
        delete m_elementTable;
    }
    m_elementTable = 0;

}


void MaterialStepRecorder::BeginOfRunAction(const G4Run*)
{

   std::cout << "[MaterialStepRecorder]   BeginOfRunAction" << std::endl;
   m_storeGate = storeGateSvc();

   ParseProperties();

   // initialize
   m_totalSteps = 0;
   m_eventID = 0;

}


void MaterialStepRecorder::EndOfRunAction(const G4Run*)
{

    std::cout << "[MaterialStepRecorder]   EndOfRunAction" << std::endl;
    if (m_runElementTable) { 
        std::cout << "[MaterialStepRecorder] Printing final ElementTable " << std::endl;
        std::cout << *m_runElementTable << std::endl;
    }
    delete m_runElementTable;
    

}


void MaterialStepRecorder::SteppingAction(const G4Step* aStep)
{

   

    // kill secondaries
    if (aStep->GetTrack()->GetParentID()) {
        aStep->GetTrack()->SetTrackStatus(fStopAndKill);
        return;
    }

    // ElementTable preparation
    if (m_recordComposition && !m_elementTable) {
        m_elementTable = new Trk::ElementTable();
        m_runElementTable = new Trk::ElementTable();
    }
    
    // the material information
    G4TouchableHistory* touchHist = (G4TouchableHistory*)aStep->GetPreStepPoint()->GetTouchable();
    // G4LogicalVolume
    G4LogicalVolume *lv= touchHist ? touchHist->GetVolume()->GetLogicalVolume() : 0;
    G4Material *mat    = lv ? lv->GetMaterial() : 0;
    
	std::vector<unsigned char> elements;
	std::vector<unsigned char> fractions;
	
    // log the information // cut off air
    if (mat && mat->GetRadlen() < 200000.) {

        ++m_totalSteps;

        // the step information
        double steplength     = aStep->GetStepLength();

        // the position information
        G4ThreeVector pos     = aStep->GetPreStepPoint()->GetPosition();

        double X0             = mat->GetRadlen();
        double L0             = mat->GetNuclearInterLength();
        double A              = 0.;
        double Z              = 0.;
//        double totAtoms       = 0.;

        double rho            = mat->GetDensity()*CLHEP::mm3/CLHEP::gram;

        // get the number of Elements
        size_t                 elNumber = mat->GetNumberOfElements();
        const G4ElementVector* elVector = mat->GetElementVector();
        double                 totAtoms = mat->GetTotNbOfAtomsPerVolume();

        // reserve the right number of elements
		elements.reserve(elNumber);
        fractions.reserve(elNumber);		

        if (1 == elNumber) {

            A   = mat->GetA()/CLHEP::gram;
            Z   = mat->GetZ();
			
            unsigned int Zint = (unsigned int)Z;
			// the element and fraction vector
			elements.push_back(static_cast<unsigned char>(Z));
			fractions.push_back(UCHAR_MAX);
            // record the Element 
            if (m_recordComposition && !m_elementTable->contains(Zint)){
                // the element Material
                Trk::Material elMat(X0,L0,A,Z,rho);
                G4String      elName =  (*elVector)[0]->GetName();
                // add it to the table 
                m_elementTable->addElement(elMat, elName);
                m_runElementTable->addElement(elMat,elName);
            }
            

        } else {

            const G4double*   atVector = mat->GetVecNbOfAtomsPerVolume();
            double totalFrac           = 0.;
            double totalFracChar       = 0.;

            for (size_t iel = 0; iel < elNumber; ++iel) {

                G4Element*  currentEl  = (*elVector)[iel];
                double      currentNum = atVector ? atVector[iel] : 1.;
                double      relNbAtoms = currentNum/totAtoms;
                                
                double currentZ = currentEl->GetZ();

                A += relNbAtoms*currentEl->GetA()/CLHEP::gram;
                Z += relNbAtoms*currentEl->GetZ();
                unsigned int Zint = (unsigned int)(currentEl->GetZ());
                
                // the element and fraction vector
                elements.push_back(int(currentZ));
                // calculate the fraction with a accuracy of 1/256.
                unsigned int relNbAtomsChar = (unsigned int)(relNbAtoms*(UCHAR_MAX));
                relNbAtomsChar = relNbAtomsChar > UCHAR_MAX ? UCHAR_MAX : relNbAtomsChar;
                
                // smaller components than 0.5 % are automatically ignored
                totalFrac     += relNbAtoms;
                if (relNbAtomsChar) {
                    totalFracChar += double(relNbAtomsChar)/double(1.*UCHAR_MAX);
                    fractions.push_back(relNbAtomsChar);
                    // record composition
                    if (m_recordComposition && !m_elementTable->contains(Zint)){
                        double curA          =  currentEl->GetA()/CLHEP::gram;
                        double curZ          =  currentEl->GetZ();
                        // approximate formulas for X0 and L0
                        // X0 from : PH-EP-Tech-Note-2010-013 g/cm3 -> g/mm3
                        // L0 from :
                        double curX0         =  1432.8*curA/(curZ*(curZ+1)*(11.319-std::log(curZ)));
                        double curL0         =  0.;
                        double curRho        =  rho*relNbAtoms;
                        Trk::Material elMat(curX0,curL0,curA,curZ,curRho);
                        G4String      elName =  currentEl->GetName();
                        // add it to the table 
                        m_elementTable->addElement(elMat, elName);
                        m_runElementTable->addElement(elMat,elName);
                    }
                }
            }
           if ((totalFrac-1.)*(totalFrac-1.) > 10e-4 ) 
               std::cout << "[MaterialStepRecorder]   Total fractions do not add up to one at INPUT (" << totalFrac << ") !" << std::endl;
           
        }

        // is it a Geantino?
        if (aStep->GetTrack()->GetParticleDefinition()->GetPDGEncoding() == 0) {
            if (m_recordComposition)
            	m_matStepCollection->push_back(new Trk::MaterialStep(pos.x(), pos.y(), pos.z(), steplength, X0, L0, A, Z, rho, elements, fractions));
             else 
                m_matStepCollection->push_back(new Trk::MaterialStep(pos.x(), pos.y(), pos.z(), steplength, X0, L0, A, Z, rho));
        }
    }
}


void MaterialStepRecorder::ParseProperties()
{
    // here we read the property map and set the needed variables
    // (many thanks to Zachary Marshall for this idea)
    // first fill missing properties and issue warnings
    if (theProperties.find("verboseLevel")==theProperties.end()) {
        std::cout << "[MaterialStepRecorder]   ParseProperties : no verboseLevel specified, setting to default = "
                << m_verboseLevel << std::endl;
    } else {
        m_verboseLevel = strtol(theProperties["verboseLevel"].c_str(),NULL,0);
    }
    
    // first fill missing properties and issue warnings
    if (theProperties.find("recordComposition")==theProperties.end()) {
        std::cout << "[MaterialStepRecorder]   ParseProperties : no recordComposition specified, setting to default = "
                << m_recordComposition << std::endl;
    } else {
        m_recordComposition = bool(strtol(theProperties["recordComposition"].c_str(),NULL,0));
        std::cout << "[MaterialStepRecorder]   ParseProperties : setting recordComposition to " << m_recordComposition << std::endl;
    }
    

}
