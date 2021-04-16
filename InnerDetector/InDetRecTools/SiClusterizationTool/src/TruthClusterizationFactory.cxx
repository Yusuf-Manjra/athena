/////////////////////////////////////////////////////////////////////////////////////////////////////

/// Name    : TruthClusterizationFactory.cxx

/// Package : SiClusterizationTool 

/// Author  : Roland Jansky & Felix Cormier

/// Created : April 2016

///

/// DESCRIPTION: Emulates NN evaluation from truth (for ITK studies)

///////////////////////////////////////////////////////////////////////////////////////////////////////




#include <TMath.h>
#include "SiClusterizationTool/TruthClusterizationFactory.h"

//for position estimate and clustering

#include "InDetIdentifier/PixelID.h"
#include "InDetPrepRawData/PixelCluster.h"

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/Incident.h"
#include "InDetSimData/InDetSimData.h"
#include "HepMC/GenParticle.h"

#include "TrkEventPrimitives/ParamDefs.h"

#include "CLHEP/Random/RandFlat.h"


namespace InDet {

  TruthClusterizationFactory::TruthClusterizationFactory(const std::string& name,
							 const std::string& n, const IInterface* p):
    AthAlgTool(name, n,p),
    m_incidentSvc("IncidentSvc", n),
    m_simDataCollectionName("PixelSDO_Map"),
    m_rndmSvc("AtDSFMTGenSvc",name),
    m_rndmEngineName("TruthClustering"),
    m_usePUHits(false)
  {
    // further properties
    declareProperty("IncidentService", m_incidentSvc );
    declareProperty("RndmSvc", m_rndmSvc, "Random Number Service used in TruthClusterizationFactory");
    declareProperty("RndmEngine",m_rndmEngineName,"Random generator engine used to emualate pixel NN");
    declareProperty("InputSDOMap",  m_simDataCollectionName, "sim data collection name");
    declareProperty("usePUHits", m_usePUHits, "Take into account PU hits as contributing particles for NN emulation");
    
    declareInterface<TruthClusterizationFactory>(this);
  } 

  
/////////////////////////////////////////////////////////////////////////////////////

/// Destructor - check up memory allocation

/// delete any memory allocation on the heap

  
  TruthClusterizationFactory::~TruthClusterizationFactory() {}
  
  StatusCode TruthClusterizationFactory::initialize() {
    if (m_incidentSvc.retrieve().isFailure()){
        ATH_MSG_WARNING("Can not retrieve " << m_incidentSvc << ". Exiting.");
        return StatusCode::FAILURE;
    }
  
    // register to the incident service

    m_incidentSvc->addListener( this, "BeginEvent");

 // random svc
	CHECK(m_rndmSvc.retrieve());
 	
	// get the random stream
	ATH_MSG_DEBUG ( "Getting random number engine : <" << m_rndmEngineName << ">" );
	m_rndmEngine = m_rndmSvc->GetEngine(m_rndmEngineName);
	if (!m_rndmEngine) {
	  ATH_MSG_ERROR("Could not find RndmEngine : " << m_rndmEngineName);
	  return StatusCode::FAILURE;
	}
	else {
	  ATH_MSG_DEBUG("Found RndmEngine : " << m_rndmEngineName);
	}
	
	ATH_CHECK( m_simDataCollectionName.initialize() );
	
	
	msg(MSG::INFO) << "initialize() successful in " << name() << endreq;
	return StatusCode::SUCCESS;
  }
  
  void TruthClusterizationFactory::handle(const Incident& inc) 
  { 
   if ( inc.type() == IncidentType::BeginEvent ){
    // record the SDO collection
     
     SG::ReadHandle<InDetSimDataCollection> pixSdoColl(m_simDataCollectionName);
     
     if (!pixSdoColl.isValid()){
       ATH_MSG_WARNING("Could not retrieve the  InDetSimDataCollection with name "   << m_simDataCollectionName.key());
       m_simDataCollection = 0;
     } else {
       ATH_MSG_VERBOSE("Successfully retrieved the InDetSimDataCollection with name " << m_simDataCollectionName.key());
       m_simDataCollection = &(*pixSdoColl);  
     }
     
   }  
  }

  std::vector<double> TruthClusterizationFactory::estimateNumberOfParticles(const InDet::PixelCluster& pCluster)
  {
    std::vector<double> probabilities(3,0.);
    auto rdos = pCluster.rdoList();
    unsigned int nPartContributing = 0;
    //Initialize vector for a list of UNIQUE barcodes for the cluster

    std::vector<int> barcodes;
    //Loop over all elements (pixels/strips) in the cluster

    for (auto rdoIter :  rdos){
        if (m_simDataCollection){
            auto simDataIter = m_simDataCollection->find(rdoIter);
            if (simDataIter != m_simDataCollection->end()){
                // get the SimData and count the individual contributions

                auto simData = (simDataIter->second);

                for( auto deposit : simData.getdeposits() ){
                    //If deposit exists

                    if (deposit.first){
                        //Now iterate over all barcodes

                        std::vector<int>::iterator barcodeIterator;
                        //Look for the barcode of the specific particle depositing energy in the barcodes vector

                        barcodeIterator  = find(barcodes.begin(), barcodes.end(), deposit.first->barcode());
                        //If this barcode is not found

                        if (!(barcodeIterator != barcodes.end())){

                            //Only count deposits from HS to ensure consistency between full truth and standard truth PU configurations
                            if(m_usePUHits || deposit.first.eventIndex()==0) barcodes.push_back(deposit.first->barcode());

                        }
                    }
                    else ATH_MSG_WARNING("No deposits found");
                }

            }
        }
    }
    //Barcodes vector is then a list of the total number of UNIQUE

    //barcodes in the cluster, each corresponding to a truth particle

    nPartContributing = barcodes.size();
    ATH_MSG_VERBOSE("n Part Contributing: " << nPartContributing);
    //Set random seed for smearing NN efficiency

    ATH_MSG_VERBOSE("Smearing TruthClusterizationFactory probability output for TIDE studies");
    //If only 1 truth particles found

    if (nPartContributing<=1) {
        //NN will always return 100% chance of there being only 1 particle
        //For pure PU case nPartContributing=0, assume that there is a single particle contributing as well
        probabilities[0] = 1.0;
    }
    //If two unique truth particles found in cluster

    else if (nPartContributing==2) {
        //90% chance NN returns high probability of there being 2 particles

       if (CLHEP::RandFlat::shoot( m_rndmEngine, 0, 1 ) < 0.9) probabilities[1] = 1.0;
        //Other 10% NN returns high probability of there being 1 particle

        else probabilities[0] = 1.0;
    }
    //If greater than 2 unique truth particles in cluster

    else if (nPartContributing>2) {
        //90% chance NN returns high probability of there being >2 particles

        if (CLHEP::RandFlat::shoot( m_rndmEngine, 0, 1 ) < 0.9) probabilities[2] = 1.0;
        //Other 10% NN returns high probability of there being 1 particle

        else probabilities[0] = 1.0;
    }
    
    return probabilities;
    
  }

   std::vector<Amg::Vector2D> TruthClusterizationFactory::estimatePositions(const InDet::PixelCluster& )
  { 
    ATH_MSG_ERROR("TruthClusterizationFactory::estimatePositions called for ITk ambiguity setup, should never happen! Digital clustering should be run for positions & errors.");
    return std::vector<Amg::Vector2D>(2,Amg::Vector2D (2,0.));
  }
  
}//end InDet namespace
