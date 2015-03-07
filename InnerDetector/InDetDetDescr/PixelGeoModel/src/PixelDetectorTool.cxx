/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "PixelGeoModel/PixelDetectorTool.h"
#include "PixelGeoModel/PixelDetectorFactory.h" 
#include "PixelGeoModel/PixelDetectorFactorySR1.h" 
#include "PixelGeoModel/PixelDetectorFactoryDC2.h" 
#include "PixelGeoModel/PixelGeometryManager.h" 
#include "PixelGeoModel/PixelSwitches.h" 
#include "PixelGeoModel/IBLParameterSvc.h"
#include "InDetReadoutGeometry/PixelDetectorManager.h" 
#include "DetDescrConditions/AlignableTransformContainer.h"
#include "InDetCondServices/ISiLorentzAngleSvc.h"
#include "PixelGeoModel/PixelGeoModelAthenaComps.h"
#include "GeoModelUtilities/GeoModelExperiment.h"
#include "GaudiKernel/ServiceHandle.h"
#include "StoreGate/StoreGateSvc.h"
#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "GeoModelUtilities/DecodeVersionKey.h"

#include "GeometryDBSvc/IGeometryDBSvc.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "GeoModelInterfaces/IGeoSubDetTool.h"
#include "InDetGeoModelUtils/IInDetServMatBuilderTool.h"

#include "CLIDSvc/tools/ClassID_traits.h"
#include "SGTools/DataProxy.h"

using InDetDD::PixelDetectorManager; 
using InDetDD::SiDetectorManager; 

/**
 ** Constructor(s)
 **/
PixelDetectorTool::PixelDetectorTool( const std::string& type, const std::string& name, const IInterface* parent )
  : GeoModelTool( type, name, parent ),
    m_detectorName("PixelDetector"),
    m_IBLParameterSvc("IBLParameterSvc",name),
    m_buildDBM(0),
    m_bcmTool(""),
    m_blmTool(""),
    m_serviceBuilderTool(""),
    m_geoModelSvc("GeoModelSvc",name),
    m_rdbAccessSvc("RDBAccessSvc",name),
    m_geometryDBSvc("InDetGeometryDBSvc",name),
    m_lorentzAngleSvc("PixelLorentzAngleSvc", name),
    m_manager(0),
    m_athenaComps(0)
{
  declareProperty("Services",m_services=true);
  declareProperty("ServicesOnLadder",m_servicesOnLadder=true); ///JBdV
  declareProperty("Alignable", m_alignable=true);
  declareProperty("DC1Geometry",m_dc1Geometry=false); 
  declareProperty("InitialLayout",m_initialLayout=false);
  declareProperty("DevVersion", m_devVersion=false);
  declareProperty("BCM_Tool", m_bcmTool);
  declareProperty("BLM_Tool", m_blmTool);
  declareProperty("ServiceBuilderTool", m_serviceBuilderTool);
  declareProperty("RDBAccessSvc", m_rdbAccessSvc);
  declareProperty("GeometryDBSvc", m_geometryDBSvc);
  declareProperty("GeoModelSvc", m_geoModelSvc);
  declareProperty("LorentzAngleSvc", m_lorentzAngleSvc);
  declareProperty("OverrideVersionName", m_overrideVersionName);
}
/**
 ** Destructor
 **/
PixelDetectorTool::~PixelDetectorTool()
{
  // This will need to be modified once we register the Pixel DetectorNode in
  // the Transient Detector Store
  delete m_detector; // Needs checking if this is really needed or not.
  delete m_athenaComps;
}

/**
 ** Create the Detector Node corresponding to this tool
 **/
StatusCode PixelDetectorTool::create( StoreGateSvc* detStore )
{ 
  StatusCode result = StatusCode::SUCCESS;


  if (m_devVersion) {
    msg(MSG::WARNING) << "You are using a development version. There are no guarantees of stability"
	<< endreq;
  }
   

  // Get the detector configuration.
  StatusCode sc = m_geoModelSvc.retrieve();
  if (sc.isFailure()) {
    msg(MSG::FATAL) << "Could not locate GeoModelSvc" << endreq;
    return (StatusCode::FAILURE); 
  }  
 
  DecodeVersionKey versionKey(&*m_geoModelSvc, "Pixel");

  msg(MSG::INFO) << "Building Pixel Detector with Version Tag: " << versionKey.tag() << " at Node: " 
		 << versionKey.node() << endreq;

  std::string pixelVersionTag;

  sc = m_rdbAccessSvc.retrieve();
  if (sc.isFailure()) {
    msg(MSG::FATAL) << "Could not locate RDBAccessSvc" << endreq;
    return (StatusCode::FAILURE); 
  }  

  // Print the version tag:
  pixelVersionTag = m_rdbAccessSvc->getChildTag("Pixel", versionKey.tag(), versionKey.node(), false);
  msg(MSG::INFO) << "Pixel Version: " << pixelVersionTag << "  Package Version: " << PACKAGE_VERSION << endreq;
  
  
  // Check if version is empty. If so, then the SCT cannot be built. This may or may not be intentional. We
  // just issue an INFO message. 
  if (pixelVersionTag.empty()) { 
    msg(MSG::INFO) << "No Pixel Version. Pixel Detector will not be built." << endreq;
     
  } else {
  
    // Unless we are using custom pixel, the switch positions are going to
    // come from the database:
    
    std::string versionName;
    std::string descrName="noDescr";

    if (versionKey.custom()) {

      msg(MSG::WARNING) << "PixelDetectorTool:  Detector Information coming from a custom configuration!!" << endreq;
 
    } else {

      if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "PixelDetectorTool:  Detector Information coming from the database and job options IGNORED." << endreq;
      if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Keys for Pixel Switches are "  << versionKey.tag()  << "  " << versionKey.node() << endreq;
      IRDBRecordset_ptr switchSet = m_rdbAccessSvc->getRecordsetPtr("PixelSwitches", versionKey.tag(), versionKey.node());
      const IRDBRecord    *switchTable   = (*switchSet)[0];
      
      //m_services           = switchTable->getInt("BUILDSERVICES");
      //m_alignable          = switcheTable->getInt("ALIGNABLE");
      m_dc1Geometry        = switchTable->getInt("DC1GEOMETRY");
      m_initialLayout      = switchTable->getInt("INITIALLAYOUT");
      if (!switchTable->isFieldNull("VERSIONNAME")) {
	versionName        = switchTable->getString("VERSIONNAME");
      }
      if (!switchTable->isFieldNull("DESCRIPTION")) {
	descrName        = switchTable->getString("DESCRIPTION");
      }
      m_buildDBM        = switchTable->getInt("BUILDDBM");
   }

   if (versionName.empty()) {
      if (m_dc1Geometry) {
	versionName = "DC1"; 
      } else {
	versionName = "DC2"; 
      } 
   }

   if (!m_overrideVersionName.empty()) {
     versionName = m_overrideVersionName;
     msg(MSG::INFO) << "Overriding version name: " << versionName << endreq;
   }

   if(msgLvl(MSG::DEBUG)) {  
     msg(MSG::DEBUG)  << "Creating the Pixel " << endreq;
     msg(MSG::DEBUG)  << "Pixel Geometry Options:" << endreq;
     msg(MSG::DEBUG)  << "  Services           = " << (m_services ? "true" : "false") << endreq;
     msg(MSG::DEBUG)  << "  Alignable          = " << (m_alignable ? "true" : "false") <<endreq;
     msg(MSG::DEBUG)  << "  DC1Geometry        = " << (m_dc1Geometry ? "true" : "false") <<endreq;
     msg(MSG::DEBUG)  << "  InitialLayout      = " << (m_initialLayout ? "true" : "false") <<endreq;
     msg(MSG::DEBUG)  << "  VersioName         = " << versionName  << endreq;
   }
    if (m_IBLParameterSvc.retrieve().isFailure()) {
       ATH_MSG_WARNING( "Could not retrieve IBLParameterSvc");
    }
    else {
	m_IBLParameterSvc->setBoolParameters(m_alignable,"alignable");
    }

    //
    // Initialize the geometry manager
    //

    // Initialize switches
    PixelSwitches switches;
    
    switches.setServices(m_services);
    switches.setDC1Geometry(m_dc1Geometry);
    switches.setAlignable(m_alignable);
    switches.setInitialLayout(m_initialLayout);
    if (versionName == "SLHC") switches.setSLHC();
    if (versionName == "IBL") switches.setIBL();
    switches.setDBM(m_buildDBM); //DBM flag

    //JBdV
    switches.setServicesOnLadder(m_servicesOnLadder);
    switches.setServices(m_services); //Overwrite there for the time being.

    const PixelID * idHelper = 0;
    if (detStore->retrieve(idHelper, "PixelID").isFailure()) {
      msg(MSG::FATAL) << "Could not get Pixel ID helper" << endreq;
      return StatusCode::FAILURE;
    }


    // Retrieve the Geometry DB Interface
    sc = m_geometryDBSvc.retrieve();
    if (sc.isFailure()) {
      msg(MSG::FATAL) << "Could not locate Geometry DB Interface: " << m_geometryDBSvc.name() << endreq;
      return (StatusCode::FAILURE); 
    }  

    // Pass athena services to factory, etc
    m_athenaComps = new PixelGeoModelAthenaComps;
    m_athenaComps->setDetStore(detStore);
    m_athenaComps->setGeoModelSvc(&*m_geoModelSvc);
    m_athenaComps->setRDBAccessSvc(&*m_rdbAccessSvc);
    m_athenaComps->setLorentzAngleSvc(m_lorentzAngleSvc);
    m_athenaComps->setGeometryDBSvc(&*m_geometryDBSvc);
    m_athenaComps->setIdHelper(idHelper);

    //
    // LorentzAngleService
    //
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Lorentz angle service passed to InDetReadoutGeometry with name: " 
					   << m_lorentzAngleSvc.name() << endreq;
     

    // BCM Tool.
    if (!m_bcmTool.empty()) {
      sc = m_bcmTool.retrieve();
      if (!sc.isFailure()) {
	msg(MSG::INFO) << "BCM_GeoModel tool retrieved: " << m_bcmTool << endreq;
      } else {
	msg(MSG::INFO) << "Could not retrieve " << m_bcmTool << " -  BCM will not be built" << endreq;
      }
      m_athenaComps->setBCM(&*m_bcmTool);
      //IGeoSubDetTool* tt = m_bcmTool;

    } else {
      msg(MSG::INFO) << "BCM not requested." << endreq;
    }


   // BLM Tool.
    if (!m_blmTool.empty()) {
      sc = m_blmTool.retrieve();
      if (!sc.isFailure()) {
	msg(MSG::INFO) << "BLM_GeoModel tool retrieved: " << m_blmTool << endreq;
      } else {
	msg(MSG::INFO) << "Could not retrieve " << m_blmTool << " -  BLM will not be built" << endreq;
      }
      m_athenaComps->setBLM(&*m_blmTool);

    } else {
      msg(MSG::INFO) << "BLM not requested." << endreq;
    }

    // Service builder tool
    if (!m_serviceBuilderTool.empty()) {
      sc = m_serviceBuilderTool.retrieve(); 
      if (!sc.isFailure()) {
	msg(MSG::INFO) << "Service builder tool retrieved: " << m_serviceBuilderTool << endreq;
	m_athenaComps->setServiceBuilderTool(&*m_serviceBuilderTool);
      } else {
	msg(MSG::ERROR) << "Could not retrieve " <<  m_serviceBuilderTool << ",  some services will not be built." << endreq;
      }
    } else {
      if (versionName == "SLHC") {
	// This will become an error once the tool is ready.
	//msg(MSG::ERROR) << "Service builder tool not specified. Some services will not be built" << endreq;
	msg(MSG::INFO) << "Service builder tool not specified." << endreq; 
      } else {
	msg(MSG::INFO) << "Service builder tool not specified." << endreq; 
      }	
    }


    // 
    // Locate the top level experiment node 
    // 
    GeoModelExperiment * theExpt; 
    if (StatusCode::SUCCESS != detStore->retrieve( theExpt, "ATLAS" )) { 
      msg(MSG::ERROR) 
	<< "Could not find GeoModelExperiment ATLAS" 
	<< endreq; 
      return (StatusCode::FAILURE); 
    } 
    
    GeoPhysVol *world=&*theExpt->getPhysVol();
    m_manager = 0;
 
    if (!m_devVersion) {
      
      if(versionName == "DC1" || versionName == "DC2") {	
        // DC1/DC2 version
        PixelDetectorFactoryDC2 thePixel(m_athenaComps, switches);
        thePixel.create(world);      
	m_manager  = thePixel.getDetectorManager();
      } else if (versionName == "SR1") {
	// SR1. Same a DC3 but only 1 part (barrel, ec A or ec C) built
	PixelDetectorFactorySR1 thePixel(m_athenaComps, switches);
        thePixel.create(world);      
        m_manager  = thePixel.getDetectorManager();
      } else {
	// DC3, SLHC, IBL
        PixelDetectorFactory thePixel(m_athenaComps, switches);
	if(descrName.compare("TrackingGeometry")!=0)
	  thePixel.create(world);      
	else
	  msg(MSG::INFO) << "Pixel - TrackingGeometry tag - no geometry built" << endreq; 
        m_manager  = thePixel.getDetectorManager();
      }	  
      


    } else {
      //
      // DEVELOPMENT VERSIONS
      //
      PixelDetectorFactory thePixel(m_athenaComps, switches);
      thePixel.create(world);      
      m_manager  = thePixel.getDetectorManager();
    }

    // Register the manager to the Det Store    
    if (StatusCode::FAILURE == detStore->record(m_manager, m_manager->getName()) ) {
      msg(MSG::ERROR) << "Could not register Pixel detector manager" << endreq;
      return( StatusCode::FAILURE );
    }
    // Add the manager to the experiment 
    theExpt->addManager(m_manager);
    
    // Symlink the manager
    const SiDetectorManager * siDetManager = m_manager;
    if (StatusCode::FAILURE == detStore->symLink(m_manager, siDetManager) ) { 
      msg(MSG::ERROR) << "Could not make link between PixelDetectorManager and SiDetectorManager" << endreq;
      return( StatusCode::FAILURE );
    }

   
    //
    // LorentzAngleService
    // We retrieve it to make sure it is initialized during geometry initialization.
    //
    if (!m_lorentzAngleSvc.empty()) {
      sc = m_lorentzAngleSvc.retrieve();
      if (!sc.isFailure()) {
	msg(MSG::INFO) << "Lorentz angle service retrieved: " << m_lorentzAngleSvc << endreq;
      } else {
	msg(MSG::INFO) << "Could not retrieve Lorentz angle service:" <<  m_lorentzAngleSvc << endreq;
      }
    } else {
      msg(MSG::INFO) << "Lorentz angle service not requested." << endreq;
    }
   
  } 

  return result;
}

StatusCode PixelDetectorTool::clear(StoreGateSvc* detStore)
{
  SG::DataProxy* _proxy = detStore->proxy(ClassID_traits<InDetDD::PixelDetectorManager>::ID(),m_manager->getName());
  if(_proxy) {
    _proxy->reset();
    m_manager = 0;
  }
  return StatusCode::SUCCESS;
}
  
StatusCode   
PixelDetectorTool::registerCallback( StoreGateSvc* detStore)
{

  if (m_alignable) {
    std::string folderName = "/Indet/Align";
    if (detStore->contains<AlignableTransformContainer>(folderName)) {
      if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Registering callback on AlignableTransformContainer with folder " << folderName << endreq;
      const DataHandle<AlignableTransformContainer> atc;
      return detStore->regFcn(&IGeoModelTool::align, dynamic_cast<IGeoModelTool *>(this), atc, folderName);
    } else {
      msg(MSG::ERROR) << "Unable to register callback on AlignableTransformContainer with folder " 
		      << folderName << ", Alignment disabled!" << endreq;
      return StatusCode::FAILURE;
    }
  } else {
    msg(MSG::INFO) << "Alignment disabled. No callback registered" << endreq;
    // We return failure otherwise it will try and register
    // a GeoModelSvc callback associated with this callback.

    return StatusCode::FAILURE;
  }
}
  
StatusCode 
PixelDetectorTool::align(IOVSVC_CALLBACK_ARGS_P(I,keys))
{
  if (!m_manager) { 
    msg(MSG::WARNING) << "Manager does not exist" << endreq;
    return StatusCode::FAILURE;
  }    
  if (m_alignable) {     
    return m_manager->align(I,keys);
  } else {
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Alignment disabled. No alignments applied" << endreq;
    return StatusCode::SUCCESS;
  }
}
