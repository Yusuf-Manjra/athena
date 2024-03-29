/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class IParticleCollHandle_CaloCluster           //
//                                                            //
//  Author: Riccardo-Maria BIANCHI (rbianchi@cern.ch)         //
//  Initial version: May 2014                                 //
//                                                            //
////////////////////////////////////////////////////////////////

//Local
#include "IParticleCollHandle_CaloCluster.h"
#include "IParticleHandle_CaloCluster.h"
#include "VP1AODSystems/CaloClusterCollectionSettingsButton.h"
#include "AODSysCommonData.h"

//VP1
#include "VP1Base/IVP1System.h"
#include "VP1Base/VP1Msg.h"
#include "VP1Base/VP1Serialise.h"
#include "VP1Base/VP1Deserialise.h"
#ifndef BUILDVP1LIGHT
  #include "VP1Utils/VP1SGAccessHelper.h"
  #include "VP1Utils/VP1SGContentsHelper.h"
#endif

//Qt
#include <QStringList>

//xAOD
#include "xAODCaloEvent/CaloClusterContainer.h"

// Coin3D
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoTexture2.h>

// Athena
#include "PathResolver/PathResolver.h"

#ifdef BUILDVP1LIGHT
	#include <QSettings>
	#include "xAODRootAccess/Init.h"
	#include "xAODRootAccess/TEvent.h"
	#include "GeoModelKernel/Units.h"
	#define SYSTEM_OF_UNITS GeoModelKernelUnits // so we will get, e.g., 'GeoModelKernelUnits::cm'
#else
	#include "GaudiKernel/SystemOfUnits.h"
	#define SYSTEM_OF_UNITS Gaudi::Units // so we will get, e.g., 'Gaudi::Units::cm'
#endif // BUILDVP1LIGHT


//____________________________________________________________________
#if defined BUILDVP1LIGHT
  QStringList IParticleCollHandle_CaloCluster::availableCollections( IVP1System*sys )
  {
    return sys->getObjectList(xAOD::Type::CaloCluster);
  }
#else
  QStringList IParticleCollHandle_CaloCluster::availableCollections( IVP1System*sys )
  {
    return VP1SGContentsHelper(sys).getKeys<xAOD::CaloClusterContainer>();

  }
#endif // BUILDVP1LIGHT

//____________________________________________________________________
class IParticleCollHandle_CaloCluster::Imp {

public:

  Imp () : theclass(nullptr), updateGUICounter(0), collSettingsButton(nullptr), sephelper(nullptr),
  considerTransverseEnergies{},showOutlines{},last_highestEnergy(0.0){}

  IParticleCollHandle_CaloCluster * theclass;
  int updateGUICounter;
  CaloClusterCollectionSettingsButton* collSettingsButton;

  VP1ExtraSepLayerHelper * sephelper;

  // settings
  bool considerTransverseEnergies;
  QPair<bool,double> scale;
  bool showOutlines;
  VP1Interval allowedEnergies;
  VP1Interval allowedEta;
  QList<VP1Interval> allowedPhi;
  double last_highestEnergy;
//  double calculateHighestVisibleClusterEnergy() const;

  #ifdef BUILDVP1LIGHT
    QStringList caloClusterList;
  #endif // BUILDVP1LIGHT

  double calo_start_r = theclass->calo_start_r;
  double calo_start_z = theclass-> calo_start_z;
  double calo_crack_eta = theclass->calo_crack_eta;

  void possiblyUpdateGUI() {//Fixme: to IParticleCollHandleBase
    if (!((updateGUICounter++)%50)) {
      theclass->systemBase()->updateGUI();
    }
  }
};

/*
 * DEFINITIONS
 */
//Fixme: Just some approximate values for now:
const double IParticleCollHandle_CaloCluster::calo_start_r = 1.1*SYSTEM_OF_UNITS::m + 0.05*SYSTEM_OF_UNITS::m;
const double IParticleCollHandle_CaloCluster::calo_start_z = 3.671*SYSTEM_OF_UNITS::m + 0.05*SYSTEM_OF_UNITS::m;
const double IParticleCollHandle_CaloCluster::calo_crack_eta = fabs(log(tan(0.5*atan(calo_start_r/calo_start_z))));



//____________________________________________________________________
IParticleCollHandle_CaloCluster::IParticleCollHandle_CaloCluster(AODSysCommonData * cd,
const QString& name, xAOD::Type::ObjectType type)
  : IParticleCollHandleBase(cd,name,type), m_d(new Imp)
{
  m_d->theclass = this;
  m_d->updateGUICounter = 0;
  m_d->collSettingsButton=0;


//  m_d->controller = controller;
//  m_d->key = key;
  m_d->sephelper = 0;
  //We start out with no clusters visible:
  m_d->last_highestEnergy = 0;
  m_d->showOutlines = false;
  m_d->considerTransverseEnergies = true;

//  //==========
//  // b-tagging
//  m_d->bTaggingSwitch=new SoSwitch;
//  m_d->bTaggingSwitch->whichChild=SO_SWITCH_ALL;
//  // new b-tagged jets and material
//  m_d->bTaggingTexture = new SoTexture2;
//  m_d->bTaggingMaterial = new SoMaterial;


  //The object names should not contain all sorts of funky chars (mat button style sheets wont work for instance):
  QString safetext(text());
  safetext.replace(' ','_');
  safetext.replace('[','_');
  safetext.replace(']','_');
  safetext.replace('/','_');
  safetext.replace('.','_');
  safetext.replace(',','_');
  safetext.replace('<','_');
  safetext.replace('>','_');
  safetext.replace('&','_');

  //  m_d->defaultParametersMaterial = new SoMaterial;
  //  m_d->defaultParametersMaterial->setName(("IParticleCollHandle_CaloCluster"+safetext).toStdString().c_str());
  //  m_d->defaultParametersMaterial->ref();
  // m_d->matButton->setDefaultParameterMaterial(m_d->defaultParametersMaterial); FIXME
}

//____________________________________________________________________
IParticleCollHandle_CaloCluster::~IParticleCollHandle_CaloCluster()
{
  //	m_d->defaultParametersMaterial->unref();

//  if (m_d->bTaggingSwitch) m_d->bTaggingSwitch->unref();

  delete m_d;
}

//____________________________________________________________________
void IParticleCollHandle_CaloCluster::init(VP1MaterialButtonBase*)
{
  // std::cout<<"IParticleCollHandle_CaloCluster::init 1"<<std::endl;


  //	m_d->matButton = new CaloClusterCollectionSettingsButton;
  //	m_d->matButton->setMaterialText(name());

  // INIT THE COLLECTION 'MATERIAL' BUTTON
  m_d->collSettingsButton = new CaloClusterCollectionSettingsButton;
  m_d->collSettingsButton->setMaterialText(name());

  // pass a collection pointer to the button
  m_d->collSettingsButton->setCollHandle(this);

  // std::cout<<"Calling VP1StdCollection::init with m_d->matButton (CaloClusterCollectionSettingsButton)="<<m_d->matButton<<std::endl;
  VP1StdCollection::init(m_d->collSettingsButton);//this call is required. Passing in m_d->collSettingsButton means we have the more complex button.
  setupSettingsFromController(common()->controller());
  connect(this,SIGNAL(visibilityChanged(bool)),this,SLOT(collVisibilityChanged(bool)));


  // std::cout<<"IParticleCollHandle_CaloCluster::init 2"<<std::endl;
  // std::cout<<"swi: "<<collSwitch()<<std::endl;
  // std::cout<<"sep: "<<collSep()<<std::endl;
  // std::cout<<"mat: "<<material()<<std::endl;

  //	collSwitch()->addChild(m_d->collSettingsButton->trackLightModel()); // TODO: update for jets
  //	collSwitch()->addChild(m_d->collSettingsButton->trackDrawStyle()); // TODO: update for jets

//  //==========
//  // b-tagging
//  if(m_d->collSettingsButton->is_bTaggingSkinEnabled()) {
//    std::cout << "switch texture" << std::endl;
//    setBTaggingSkin(m_d->collSettingsButton->bTaggingSkin());
//    //	  m_d->bTaggingSwitch->addChild(m_d->bTaggingTexture);
//  }
//  else if (m_d->collSettingsButton->is_bTaggingMaterialEnabled()) {
//    std::cout << "switch material" << std::endl;
//    setBTaggingMaterial();
//    //	  m_d->bTaggingMaterial = controller->bTaggingMaterial();
//    //	  m_d->bTaggingSwitch->addChild(m_d->bTaggingMaterial);
//  }
//  else {
//    messageVerbose("Info - No b-tag rendering selected.");
//  }
//
//  // we want these nodes to stay around even when removed from nodes, thus we increment the ref count by one
//  m_d->bTaggingSwitch->ref();
//  //  m_d->bTaggingTexture->ref();
//  //  m_d->bTaggingMaterial->ref();
//  //==========


}



//____________________________________________________________________
double IParticleCollHandle_CaloCluster::energyToLength(const double&energy) const
{
	VP1Msg::messageDebug("IParticleCollHandle_CaloCluster::energyToLength()");

	return std::max(1*SYSTEM_OF_UNITS::mm, m_d->scale.second*(m_d->scale.first?log(1+fabs(energy)):energy));
}

//____________________________________________________________________
bool IParticleCollHandle_CaloCluster::showOutlines() const
{
	VP1Msg::messageDebug("IParticleCollHandle_CaloCluster::showOutlines()");

	return m_d->showOutlines;
}

//____________________________________________________________________
void IParticleCollHandle_CaloCluster::setScale()
{
	this->setScale( collSettingsButton().scale() );
}

#include <Inventor/nodes/SoMaterial.h>//Fixme: just for hack below
//____________________________________________________________________
void IParticleCollHandle_CaloCluster::setScale(const QPair<bool,double>& s)
{
//  std::cout << "s: " << s.first << std::endl;

  VP1Msg::messageDebug("IParticleCollHandle_CaloCluster::setScale()");


  if (m_d->scale==s) {
	  VP1Msg::messageDebug("Scale is the same. Returning...");
	  return;
  }

  VP1Msg::messageDebug("setting scale: (" + QString::number(s.first) + ", " + QString::number(s.second) + " )");
  m_d->scale=s;

  if (!isLoaded()) {
	  VP1Msg::messageDebug("Collection not loaded/shown. Returning...");
	  return;
  }
//  largeChangesBegin();
//  foreach(Imp::ClusterHandle*cluster,m_d->clusters)
//    if (cluster->attached())
//      cluster->updateShapePars(m_d);
//  largeChangesEnd();

  largeChangesBegin();
  handleIterationBegin();
  AODHandleBase* handle=0;
  while ((handle=getNextHandle()))
  {
	  IParticleHandle_CaloCluster* cluster = dynamic_cast<IParticleHandle_CaloCluster*>(handle);
	  if (cluster && cluster->has3DObjects()) {
//		  cluster->setScale(m_d->scale);
		  cluster->updateShape(this);
//		  calo->updateHeight();
	  } else {
		  message("ERROR Handle of wrong type!");
	  }
  }
  largeChangesEnd();



  //   if (verbose())
  //     static_cast<IVP13DSystemSimple *>(systemBase())->warnOnDisabledNotifications();

  material()->touch();//FIXME: This is needed (and nothing else seems to matter). BUT WHY?????? Where is the bug?!?!?
}


//____________________________________________________________________
QPair<bool,double> IParticleCollHandle_CaloCluster::scale() const
{
	VP1Msg::messageDebug("IParticleCollHandle_CaloCluster::scale()");
  return m_d->scale;
}


//____________________________________________________________________
void IParticleCollHandle_CaloCluster::setupSettingsFromControllerSpecific(const AODSystemController*) {

	VP1Msg::messageDebug("IParticleCollHandle_CaloCluster::setupSettingsFromControllerSpecific()");

  CaloClusterCollectionSettingsButton* controller = m_d->collSettingsButton;

  // *** common cuts ***
  connect(controller,SIGNAL(cutAllowedPtChanged(const VP1Interval&)),this,SLOT(setCutAllowedPt(const VP1Interval&)));
  setCutAllowedPt(controller->cutAllowedPt());

  connect(controller,SIGNAL(energyTypeChanged()),this,SLOT(rebuildAllObjects()));

  connect(controller,SIGNAL(cutAllowedEtaChanged(const VP1Interval&)),this,SLOT(setCutAllowedEta(const VP1Interval&)));
  setCutAllowedEta(controller->cutAllowedEta());

  connect(controller,SIGNAL(cutAllowedPhiChanged(const QList<VP1Interval>&)),this,SLOT(setCutAllowedPhi(const QList<VP1Interval>&)));
  setCutAllowedPhi(controller->cutAllowedPhi());

  // *** specific cuts and settings ***

  // scale
  connect(controller,SIGNAL(scaleChanged(const QPair<bool,double>&)),this,SLOT(setScale(const QPair<bool,double>&)));
  setScale( controller->scale() ); // FIXME:

  // after completed 'recheckHighestVisibleClusterEnergy()', we call 'scale()' in the controller
  connect(this,SIGNAL(highestVisibleClusterEnergyChanged()),controller,SLOT(possibleChange_scale()));


//  // random jet colors
//  connect(controller,SIGNAL(rerandomise()),this,SLOT(rerandomise()));
//  connect(controller,SIGNAL(randomJetColoursChanged(const bool&)),this,SLOT(setRandomJetColours(const bool&)));
//  setRandomJetColours(controller->randomJetColours());

  // maxR
//  connect(controller, SIGNAL(maxRChanged(const double&)), this, SLOT(setMaxR(const double&)));
//  connect(controller, SIGNAL(signalMaxR(bool)), this, SLOT(setIsMaxR(bool)));

}

void IParticleCollHandle_CaloCluster::resetCachedValuesCuts()
{
	// kinetic cuts
	setCutAllowedPt(m_d->collSettingsButton->cutAllowedPt());
	setCutAllowedEta(m_d->collSettingsButton->cutAllowedEta());
	setCutAllowedPhi(m_d->collSettingsButton->cutAllowedPhi());
	// other settings
	setScale( m_d->collSettingsButton->scale() ); // FIXME:

}

//____________________________________________________________________
//SoMaterial* IParticleCollHandle_CaloCluster::defaultParameterMaterial() const {
//	return m_d->defaultParametersMaterial;
//}


//____________________________________________________________________
const CaloClusterCollectionSettingsButton& IParticleCollHandle_CaloCluster::collSettingsButton() const {
  if (!m_d->collSettingsButton){
    messageVerbose("CaloCluster - No collSettingsButton set! Can't call init(), so crash is imminent...");
  }
  return *m_d->collSettingsButton;
}



//____________________________________________________________________
void IParticleCollHandle_CaloCluster::rebuildAllObjects()
{
	VP1Msg::messageVerbose("IParticleCollHandle_CaloCluster::rebuildAllObjects()");

	recheckCutStatusOfAllHandles();
	recheckCutStatusOfAllNotVisibleHandles(); // Redundant, but needed! TODO: remove this when new 'contains' is ready
}


//____________________________________________________________________
bool IParticleCollHandle_CaloCluster::isConsiderTransverseEnergy() const
{
  VP1Msg::messageVerbose("IParticleCollHandle_CaloCluster::isConsiderTransverseEnergy()");
  return m_d->collSettingsButton->isTransverseEnergy();
}

//____________________________________________________________________
void IParticleCollHandle_CaloCluster::setShowVolumeOutLines(bool b)
{
	VP1Msg::messageDebug("IParticleCollHandle_CaloCluster::setShowVolumeOutLines()");

  if (m_d->showOutlines==b) return;

  m_d->showOutlines=b;

  if (!isLoaded()) return
  largeChangesBegin();
  handleIterationBegin();
  AODHandleBase* handle=0;
  while ((handle=getNextHandle()))
  {
	  IParticleHandle_CaloCluster* cluster = dynamic_cast<IParticleHandle_CaloCluster*>(handle);
	  if (cluster && cluster->has3DObjects()) {
		  if (cluster->genericBox())
		        cluster->genericBox()->drawEdgeLines = b;
	  } else {
		  message("ERROR Handle of wrong type!");
	  }
  }
  largeChangesEnd();


}


//____________________________________________________________________
bool IParticleCollHandle_CaloCluster::load()
{
  messageVerbose("loading CaloCluster collection");

  //Get collection:
  const xAOD::CaloClusterContainer * coll(nullptr);

  #if defined BUILDVP1LIGHT
    // // Get the name of the application:
    // const char* appName = "VP1Light";

    // // Initialize the environment:
    // if( !xAOD::Init( appName ).isSuccess() ) {
    //    message("Failed to execute xAOD::Init");
    //    return false;
    // }

    // Retrieve objects from the event
    if( !(systemBase()->getEvent())->retrieve( coll, name().toStdString()).isSuccess() ) {
      QString errMsg = "Failed to retrieve " + name();
      message("Error: Could not retrieve collection with key="+name());
       return false;
    }
  #else
    if (!VP1SGAccessHelper(systemBase()).retrieve(coll, name())) {
      message("Error: Could not retrieve collection with key="+name());
      return false;
    }
  #endif // BUILDVP1LIGHT

  // // Retrieve the xAOD particles:
  //  const xAOD::JetContainer* xaod = evtStore()->retrieve<const xAOD::JetContainer>( m_JetCollection );

  // Check that the auxiliary store association was made successfully:
  if( ! coll->hasStore() ) {
    message("No auxiliary store got associated to the CaloCluster container with key: " +name());
  }

  // This is needed for now, until the issues around the DV code are
  // sorted out...
  const_cast< xAOD::CaloClusterContainer* >( coll )->setStore(
    ( SG::IAuxStore* ) coll->getConstStore() );


  xAOD::CaloClusterContainer::const_iterator it, itEnd = coll->end();
  for ( it = coll->begin() ; it != itEnd; ++it) {
    m_d->possiblyUpdateGUI();
    if (!*it) {
      messageDebug("WARNING: Ignoring null CaloCluster pointer.");
      continue;
    }
   
    addHandle(new IParticleHandle_CaloCluster(this,*it));
  }

  // get handles list and update locally
  // m_d->jets = this->getHandlesList();

  return true;
}

//Cuts:
//____________________________________________________________________
bool IParticleCollHandle_CaloCluster::cut(AODHandleBase* c) {
  messageVerbose("IParticleCollHandle_CaloCluster::cut()");

  if (IParticleHandle_CaloCluster* handle = dynamic_cast<IParticleHandle_CaloCluster*>(c))
  {

	  message("CaloCluster::cut() --- to be implemented......");


    //------ ENERGY CUTS
    double jEnergy = handle->energyForCuts();
    double jEnergySqr = jEnergy * jEnergy; // in IParticleCollHandleBase the allowedPt is stored as squared, to avoid square-root operations
    messageVerbose("object's energy, object's energy^2, allowedPt^2, isPtAll: " + QString::number(jEnergy) + " - " + QString::number(jEnergySqr) + " - " + getCutAllowedPt().toString() + " - " + QString::number(int(getPtAllowall()))  );
    if (!getPtAllowall() && !getCutAllowedPt().contains(jEnergySqr) ) {
      messageVerbose("Energy cut not passed");
      return false;
    }

    //------ ETA CUTS
    messageVerbose("object's eta, etaCut, EtaAll: " + QString::number(handle->eta()) + " - " + getCutAllowedEta().toString() + " - " + QString::number(int(getEtaAllowall())) );
    if (!getEtaAllowall() && !getCutAllowedEta().contains(handle->eta())) {
      messageVerbose("Eta cut not passed");
      return false;
    }

    //------ PHI CUTS
    if (!getPhiAllowall() ) {
      double phi = handle->phi();
      bool ok(false);
      foreach (VP1Interval phirange, getCutAllowedPhi() ) {
        messageVerbose("object's phi, phiCut, PhiAll: " + QString::number(phi)  + " - " + phirange.toString() + " - " + QString::number(int(getPhiAllowall())) );
        if (phirange.contains(phi)||phirange.contains(phi+2*M_PI)||phirange.contains(phi-2*M_PI)) {
          ok = true;
          break;
        }
      }
      if (!ok) {
        messageVerbose("Phi cut not passed");
        return false;
      }
    }
//
//    //------ OTHER CUTS
//    messageVerbose("Other cuts??? Calling base method...");
//    if (!IParticleCollHandleBase::cut(handle))
//      return false;

  } // if(dynamic_cast)
  else{
    messageVerbose("Not a IParticleCollHandle_CaloCluster. Calling base method...");
    if (!IParticleCollHandleBase::cut(handle))
      return false;
  }

  return true; //default

}



//____________________________________________________________________
void IParticleCollHandle_CaloCluster::showParametersChanged(bool val) {
	messageDebug("IParticleCollHandle_CaloCluster::showParametersChanged to "+str(val));
  update3DObjectsOfAllHandles();
}


//____________________________________________________________________
double IParticleCollHandle_CaloCluster::highestVisibleClusterEnergy() const
{
	messageDebug("IParticleCollHandle_CaloCluster::highestVisibleClusterEnergy()");
  return m_d->last_highestEnergy;
}


//____________________________________________________________________
double IParticleCollHandle_CaloCluster::calculateHighestVisibleClusterEnergy()
{
	messageDebug("IParticleCollHandle_CaloCluster::Imp::calculateHighestVisibleClusterEnergy()");
	 if (!isLoaded()){
		 messageDebug("Not loaded. Returning...");
	   return 0;
    }
  double e = 0;

	    handleIterationBegin();
	    AODHandleBase* handle=0;
	    while ((handle=getNextHandle()))
	    {
	  	  IParticleHandle_CaloCluster* cluster = dynamic_cast<IParticleHandle_CaloCluster*>(handle);
	  	  if (cluster && cluster->has3DObjects()) {
//	  		if ( e < cluster->energyForLengthAndCuts(this) )
//	  			e = cluster->energyForLengthAndCuts(this);
	  		if ( e < cluster->energyForCuts() )
	  			e = cluster->energyForCuts();
	  	  } else {
	  		  message("ERROR Handle of wrong type!");
	  	  }
	    }

  return e;
}

//____________________________________________________________________
void IParticleCollHandle_CaloCluster::recheckHighestVisibleClusterEnergy()
{
  //When visibility or cut changes:
//  double newhighest = m_d->calculateHighestVisibleClusterEnergy();
  double newhighest = calculateHighestVisibleClusterEnergy();
  if (m_d->last_highestEnergy == newhighest)
    return;
  m_d->last_highestEnergy = newhighest;
  emit highestVisibleClusterEnergyChanged();
}



//____________________________________________________________________
QByteArray IParticleCollHandle_CaloCluster::persistifiableState() const
{
  messageDebug("IParticleCollHandle_CaloCluster::persistifiableState() - start...");

  // if (!m_d->matButton) {
  //   message("ERROR: persistifiableState() called before init()");
  //   return QByteArray();
  // }


  VP1Serialise serialise(1/*version*/);

  // settings
  serialise.disableUnsavedChecks();

  // SAVE THE CHECKED/UNCHECKED STATUS OF THE COLLECTION
  serialise.save(visible());

  // SAVE THE MATERIAL BUTTON
  //Q_ASSERT(m_d->matButton&&"Did you forget to call init() on this VP1StdCollection?");
  //serialise.save(m_d->matButton->saveState());

  // SAVE THE EXTRA-STATES
  serialise.save(extraWidgetsState());//version 1+

  // SAVE MATERIAL SETTINGS / CUTS
  serialise.save(m_d->collSettingsButton->saveState());

  messageDebug("IParticleCollHandle_CaloCluster::persistifiableState() - end.");
  return serialise.result();
}

//____________________________________________________________________
void IParticleCollHandle_CaloCluster::setState(const QByteArray&state)
{
  VP1Deserialise des(state);
  des.disableUnrestoredChecks();
  if (des.version()!=0&&des.version()!=1) {
    messageDebug("Warning: Ignoring state with wrong version");
    return;
  }

  // save state

  bool vis = des.restoreBool();

  //	QByteArray matState = des.restoreByteArray();
  // m_d->matButton->restoreFromState(matState);
  QByteArray extraWidgetState = des.version()>=1 ? des.restoreByteArray() : QByteArray();
  setVisible(vis);

  if (extraWidgetState!=QByteArray())
    setExtraWidgetsState(extraWidgetState);

  // MATERIAL SETTINGS / CUTS
  messageDebug("restoring material collection button...");
  des.restore(m_d->collSettingsButton);

  messageDebug("reset all caches storing values for cuts...");
  resetCachedValuesCuts();

  messageDebug("recheck all handles...");
  recheckCutStatusOfAllVisibleHandles();
}

void IParticleCollHandle_CaloCluster::dumpToJSON( std::ofstream& str) const {
  str << "\""<<name().toLatin1().data()<<"\":{";
  
  unsigned int num=0;
  for (auto handle : getHandlesList() ) {
    if (handle->visible()) {
      if (num) str <<",\n";
      str << "\"Clus "<<num++<< "\":{";
      handle->dumpToJSON(str);
      str << "}";
    }
  }
  
  str << "}";
}

