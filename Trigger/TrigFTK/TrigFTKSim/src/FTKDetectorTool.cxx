/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigFTKSim/FTKDetectorTool.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

#include <fstream>
#include <iostream>

using namespace std;

FTKDetectorTool::FTKDetectorTool(const std::string &algname,const std::string &name, const IInterface *ifc)
  : AthAlgTool(algname,name,ifc)
  , m_log( msgSvc() , name )
  , m_dumppath("deadmap.dat")
  , m_storeGate( 0 )
  , m_detStore( 0 )
  , m_evtStore( 0 )
  , m_PIX_mgr( 0 )
  , m_SCT_mgr( 0 )
  , m_pixelContainer( 0 )
  , m_sctContainer( 0 )
  , m_pixelCondSummarySvc("PixelConditionsSummarySvc",name)
  , m_sctCondSummarySvc("InDetSCT_ConditionsSummarySvc",name)
  , m_pixelId( 0 )
  , m_sctId( 0 )
  , m_pixelClustersName( "PixelClusters" )
  , m_sctClustersName( "SCT_Clusters" )
  , m_FTK_BadModuleMapPath( "badModulemap_FTK.bmap" )
  , m_ATLAS_BadModuleMapPath( "badModulemap_ATLAS.bmap" )
  , m_FTK_ModuleIDMapPath("ftk.moduleidmap")
  , m_doBadModuleMap(true)
  , m_pmap_path()
  , m_bad_module_map(0x0),
  m_global2local_path("global-to-local-map.moduleidmap"),
  m_rmap_path(""),
  m_rmap(0x0)
{
  declareInterface<FTKDetectorToolI>(this);
  
  declareProperty("DumpPath",                 m_dumppath);
  declareProperty("pixelClustersName",        m_pixelClustersName);
  declareProperty("SCT_ClustersName",         m_sctClustersName);
  declareProperty("PixelSummarySvc" ,         m_pixelCondSummarySvc);
  declareProperty("SctSummarySvc" ,           m_sctCondSummarySvc);
  declareProperty("FTK_BadModuleMapPath" ,    m_FTK_BadModuleMapPath );
  declareProperty("ATLAS_BadModuleMapPath" ,  m_ATLAS_BadModuleMapPath );
  declareProperty("pmap_path",                m_pmap_path);

  declareProperty("GlobalToLocalMapPath",m_global2local_path);
  declareProperty("rmap_path",m_rmap_path);
}

FTKDetectorTool::~FTKDetectorTool()
{
  if (m_bad_module_map) delete m_bad_module_map;
}


StatusCode FTKDetectorTool::initialize()
{
  if( service("StoreGateSvc", m_storeGate).isFailure() ) {
    m_log << MSG::FATAL << "StoreGate service not found" << endreq;
    return StatusCode::FAILURE;
  }
  if( service("DetectorStore",m_detStore).isFailure() ) {
    m_log << MSG::FATAL <<"DetectorStore service not found" << endreq;
    return StatusCode::FAILURE;
  }
  if( m_detStore->retrieve(m_PIX_mgr, "Pixel").isFailure() ) {
    m_log << MSG::ERROR << "Unable to retrieve Pixel manager from DetectorStore" << endreq;
    return StatusCode::FAILURE;
  }
  if( m_detStore->retrieve(m_pixelId, "PixelID").isFailure() ) {
    m_log << MSG::ERROR << "Unable to retrieve Pixel helper from DetectorStore" << endreq;
    return StatusCode::FAILURE;
  }
  if( m_detStore->retrieve(m_SCT_mgr, "SCT").isFailure() ) {
    m_log << MSG::ERROR << "Unable to retrieve SCT manager from DetectorStore" << endreq;
    return StatusCode::FAILURE;
  }
  if( m_detStore->retrieve(m_sctId, "SCT_ID").isFailure() ) {
    m_log << MSG::ERROR << "Unable to retrieve SCT helper from DetectorStore" << endreq;
    return StatusCode::FAILURE;
  }
  if ( m_pixelCondSummarySvc.retrieve().isFailure() ) {
    m_log << MSG::FATAL << "Failed to retrieve tool " << m_pixelCondSummarySvc << endreq;
    return StatusCode::FAILURE;
  } 
  if ( m_sctCondSummarySvc.retrieve().isFailure() ) {
    m_log << MSG::FATAL << "Failed to retrieve tool " << m_sctCondSummarySvc << endreq;
    return StatusCode::FAILURE;
  } 

  m_log << MSG::INFO << "Read the logical layer definitions" << endreq;
  // Look for the main plane-map
  if (m_pmap_path.empty()) {
    m_log << MSG::FATAL << "Main plane map definition missing" << endreq;
    return StatusCode::FAILURE;
  }
  else {
    m_pmap = new FTKPlaneMap(m_pmap_path.c_str());
    if (!(*m_pmap)) {
      m_log << MSG::FATAL << "Error using plane map: " << m_pmap_path << endreq;
      return StatusCode::FAILURE;
    }
    m_log << MSG::INFO << "Number of logical layers = " << m_pmap->getNPlanes() << endreq;
  }

  if (m_rmap_path.empty()) {
	  m_log << MSG::INFO << "Tower/Region map not set, module maps cannot be created" << endreq;
  }
  else {
	  m_log << MSG::INFO << "Tower/Region map path: " << m_rmap_path << endreq;
	  m_rmap = new FTKRegionMap(m_pmap, m_rmap_path.c_str());
	  if (!(*m_rmap)) {
		m_log << MSG::FATAL << "Error creating region map from: " << m_rmap_path.c_str() << endreq;
		return StatusCode::FAILURE;
	  }
  }
  return StatusCode::SUCCESS;
}

StatusCode FTKDetectorTool::finalize()
{
  return StatusCode::SUCCESS;
}

void FTKDetectorTool::makeBadModuleMap(){
  if (!m_doBadModuleMap) return;

  // list of the dead module, using the ftkrawhit format
  list<FTKRawHit> badmodule_rawlist;   
  
  // take the list of dead pixels
  for( InDetDD::SiDetectorElementCollection::const_iterator i=m_PIX_mgr->getDetectorElementBegin(), f=m_PIX_mgr->getDetectorElementEnd() ; i!=f; ++i ) {
    const InDetDD::SiDetectorElement* sielement( *i );
    Identifier id = sielement->identify();
    IdentifierHash idhash = sielement->identifyHash();
    const bool is_bad = !(m_pixelCondSummarySvc->isGood( idhash ));
    if(is_bad){
      FTKRawHit tmpmodraw;
      
      tmpmodraw.setHitType(ftk::PIXEL);
      tmpmodraw.setBarrelEC(m_pixelId->barrel_ec(id));
      tmpmodraw.setLayer(m_pixelId->layer_disk(id));
      tmpmodraw.setPhiModule(m_pixelId->phi_module(id));
      tmpmodraw.setEtaModule(m_pixelId->eta_module(id));
      tmpmodraw.setPhiSide(m_pixelId->phi_index(id));
      tmpmodraw.setEtaStrip(m_pixelId->eta_index(id));
      tmpmodraw.setIdentifierHash(idhash);
      tmpmodraw.normalizeLayerID();
      
      badmodule_rawlist.push_back(tmpmodraw);      
    }
  }
  
  // take the list of the dead SCT modules
  for( InDetDD::SiDetectorElementCollection::const_iterator i=m_SCT_mgr->getDetectorElementBegin(), f=m_SCT_mgr->getDetectorElementEnd() ; i!=f; ++i ) {
    const InDetDD::SiDetectorElement* sielement( *i );
    Identifier id = sielement->identify();
    IdentifierHash idhash = sielement->identifyHash();
    const bool is_bad = !(m_sctCondSummarySvc->isGood( idhash ));
    if(is_bad){
      FTKRawHit tmpmodraw;
      
      tmpmodraw.setHitType(ftk::SCT);
      tmpmodraw.setBarrelEC(m_sctId->barrel_ec(id));
      tmpmodraw.setLayer(m_sctId->layer_disk(id));
      tmpmodraw.setPhiModule(m_sctId->phi_module(id));
      tmpmodraw.setEtaModule(m_sctId->eta_module(id));
      tmpmodraw.setPhiSide(m_sctId->side(id));
      tmpmodraw.setEtaStrip(m_sctId->strip(id));
      tmpmodraw.setIdentifierHash(idhash);
      tmpmodraw.normalizeLayerID();
      
      badmodule_rawlist.push_back(tmpmodraw);     
    }
  }
  
  
  m_log << MSG::INFO << "Total number of dead modules find: " << badmodule_rawlist.size() << endl;
  
  // prepare the map of the bad module, the size is equal to the map of used layers
  m_bad_module_map = new list<FTKHit>[m_pmap->getNPlanes()];
  /* convert the dead module list in he FTKHit format, taking into account
     the plane-map configuration */
  list<FTKRawHit>::const_iterator ibadmod = badmodule_rawlist.begin();
  unsigned nbadmod_effective(0);
  for (;ibadmod!=badmodule_rawlist.end();++ibadmod) {
    const FTKRawHit &badmodraw = *ibadmod;
    // the module in the unused layers are skipped 
    if (m_pmap->getMap(badmodraw.getHitType(),badmodraw.getBarrelEC()!=0,badmodraw.getLayer()).getPlane() != -1) {
      // the bad module is used by FTK
      FTKHit tmpmodhit = badmodraw.getFTKHit(m_pmap);
      m_bad_module_map[tmpmodhit.getPlane()].push_back(tmpmodhit);
      nbadmod_effective += 1; // increment the counter
    }
  }
}




void FTKDetectorTool::dumpDeadModuleSummary()  
{
  if (!m_doBadModuleMap) return;
 
  ofstream mapfile_ATLAS_BadModuleMap(m_ATLAS_BadModuleMapPath.c_str());
  for( InDetDD::SiDetectorElementCollection::const_iterator i=m_PIX_mgr->getDetectorElementBegin(), f=m_PIX_mgr->getDetectorElementEnd(); i!=f; ++i ) {
    const InDetDD::SiDetectorElement* sielement( *i );
    Identifier id = sielement->identify();
    IdentifierHash idhash = sielement->identifyHash();
    const bool is_bad = !(m_pixelCondSummarySvc->isGood( idhash ));
    if(is_bad){
      
      mapfile_ATLAS_BadModuleMap << "B\t"
				 << 1  << '\t' // 1  pixel 0 sct
				 << m_pixelId->barrel_ec(id) << '\t'
				 << m_pixelId->layer_disk(id) << '\t'
				 << m_pixelId->phi_module(id) << '\t'
				 << m_pixelId->eta_module(id) << '\t'
				 << 0 //it means m_pixelId don't have side(id)
				 << idhash << '\t'
				 << std::endl;
    }
  }
  for( InDetDD::SiDetectorElementCollection::const_iterator i=m_SCT_mgr->getDetectorElementBegin(), f=m_SCT_mgr->getDetectorElementEnd(); i!=f; ++i ) {
    const InDetDD::SiDetectorElement* sielement( *i );
    Identifier id = sielement->identify();
    IdentifierHash idhash = sielement->identifyHash();
    const bool is_bad = !(m_sctCondSummarySvc->isGood( idhash ));
    if(is_bad){
      mapfile_ATLAS_BadModuleMap  << "B\t"
				 << 0  << '\t'  // 1  pixel 0 sct
				 << m_sctId->barrel_ec(id) << '\t'
				 << m_sctId->layer_disk(id) << '\t'
				 << m_sctId->phi_module(id) << '\t'
				 << m_sctId->eta_module(id) << '\t'
				 << m_sctId->side(id) << '\t'
				 << idhash << '\t'
				 << std::endl;
    }
  }
  mapfile_ATLAS_BadModuleMap.close();
  
  ofstream mapfile_FTK_BadModuleMap(m_FTK_BadModuleMapPath.c_str());
  for(int ip=0;ip!=m_pmap->getNPlanes();++ip){
    list<FTKHit> &layer_map = m_bad_module_map[ip];
    for( list<FTKHit>::iterator ite_list = layer_map.begin();
	 ite_list != layer_map.end(); ++ite_list ){
      int BEC(0);
      if((*ite_list).getIsBarrel()) BEC = 0;
      else if((*ite_list).getCSide()) BEC = 2;
      else if((*ite_list).getASide()) BEC = -2;
      mapfile_FTK_BadModuleMap <<m_pmap->isPixel((*ite_list).getPlane()) <<'\t' //SCT = 0 or pixel = 1
          <<  BEC <<'\t'  // Barrel = 0 or EndCap A = 2 or EndCap C = -2
          << (*ite_list).getSector() <<'\t'
          << (*ite_list).getPlane() <<'\t'
          << (*ite_list).getEtaModule() <<'\t'
          << (*ite_list).getPhiModule() <<'\t'
          << (*ite_list).getSection() <<'\t'
          << (*ite_list).getIdentifierHash() << '\t'
          <<  std::endl;
    }
  }
  mapfile_FTK_BadModuleMap.close();

  m_doBadModuleMap = false;
}

void FTKDetectorTool::dumpModuleIDMap()
{
#if 0 // excluded from compilation to avoid warnings
  for( InDetDD::SiDetectorElementCollection::const_iterator i=m_PIX_mgr->getDetectorElementBegin(), f=m_PIX_mgr->getDetectorElementEnd(); i!=f; ++i ) {
    const InDetDD::SiDetectorElement* sielement( *i );
    Identifier id = sielement->identify();
    IdentifierHash idhash = sielement->identifyHash();
  }
  for( InDetDD::SiDetectorElementCollection::const_iterator i=m_SCT_mgr->getDetectorElementBegin(), f=m_SCT_mgr->getDetectorElementEnd(); i!=f; ++i ) {
    const InDetDD::SiDetectorElement* sielement( *i );
    Identifier id = sielement->identify();
    IdentifierHash idhash = sielement->identifyHash();
  }
#endif
}

/* this method loops over all the ID modules, pixel and SCT, with the goal
 * to create the online and offline map used to describe the modules within
 * the FTK system. In particular can create the list of the modules in FTK
 * towers, organized by layer, as used to calcualte the SS
 */
void FTKDetectorTool::dumpGlobalToLocalModuleMap() {
  /* All the modules are collected as FTKRawHit, allowing to interact efficiently
   * with FTK elements as PMAP or RMAP
   */
  list<FTKRawHit> CompleteIDModuleList;

  // take the list of dead pixels
  for( InDetDD::SiDetectorElementCollection::const_iterator i=m_PIX_mgr->getDetectorElementBegin(), f=m_PIX_mgr->getDetectorElementEnd() ; i!=f; ++i ) {
    const InDetDD::SiDetectorElement* sielement( *i );
    Identifier id = sielement->identify();
    IdentifierHash idhash = sielement->identifyHash();

    FTKRawHit tmpmodraw;

    tmpmodraw.setHitType(ftk::PIXEL);
    tmpmodraw.setBarrelEC(m_pixelId->barrel_ec(id));
    tmpmodraw.setLayer(m_pixelId->layer_disk(id));
    tmpmodraw.setPhiModule(m_pixelId->phi_module(id));
    tmpmodraw.setEtaModule(m_pixelId->eta_module(id));
    tmpmodraw.setPhiSide(m_pixelId->phi_index(id));
    tmpmodraw.setEtaStrip(m_pixelId->eta_index(id));
    tmpmodraw.setIdentifierHash(idhash);
    tmpmodraw.normalizeLayerID();

    CompleteIDModuleList.push_back(tmpmodraw);
  }

  // take the list of the dead SCT modules
  for( InDetDD::SiDetectorElementCollection::const_iterator i=m_SCT_mgr->getDetectorElementBegin(), f=m_SCT_mgr->getDetectorElementEnd() ; i!=f; ++i ) {
    const InDetDD::SiDetectorElement* sielement( *i );
    Identifier id = sielement->identify();
    IdentifierHash idhash = sielement->identifyHash();

    FTKRawHit tmpmodraw;

    tmpmodraw.setHitType(ftk::SCT);
    tmpmodraw.setBarrelEC(m_sctId->barrel_ec(id));
    tmpmodraw.setLayer(m_sctId->layer_disk(id));
    tmpmodraw.setPhiModule(m_sctId->phi_module(id));
    tmpmodraw.setEtaModule(m_sctId->eta_module(id));
    tmpmodraw.setPhiSide(m_sctId->side(id));
    tmpmodraw.setEtaStrip(m_sctId->strip(id));
    tmpmodraw.setIdentifierHash(idhash);
    tmpmodraw.normalizeLayerID();

    CompleteIDModuleList.push_back(tmpmodraw);
  }

  /* The modules are store by tower and by logical layer */
  int nregions(m_rmap->getNumRegions()); // get the number of towers
  int nplanes(m_pmap->getNPlanes());
  set<unsigned int> **grouped_modules = new set<unsigned int>*[nregions];
  for (int ireg=0;ireg!=nregions;++ireg) grouped_modules[ireg] = new set<unsigned int>[nplanes];

  unsigned int nskipped(0);
  for (auto curmodrawhit: CompleteIDModuleList) { // loop over the modules, represente as raw hits
      // verify if accoring the current pmap this is module that has to be mapped
      if (m_pmap->getMap(curmodrawhit.getHitType(),curmodrawhit.getBarrelEC()!=0,curmodrawhit.getLayer()).getPlane() == -1) {
          nskipped += 1;
          continue;
      }

      // convert the FTKRawHit representation in FTKHit to interact with the PMAP
      FTKHit curmodhit = curmodrawhit.getFTKHit(m_pmap);

      bool hasOneRegion(false); // it will become true if at least 1 tower is associated with the module
      for (int ireg=0;ireg!=nregions;++ireg) { // loop over the regions
          if (m_rmap->isHitInRegion(curmodhit,ireg)) {
              hasOneRegion = true;
              // the module is compatible with the current
              grouped_modules[ireg][curmodhit.getPlane()].insert(curmodhit.getIdentifierHash());
          }
      } // end loop over the regions

      if (!hasOneRegion) m_log << MSG::WARNING << "The module with hash " << curmodhit.getIdentifierHash() << " and FTK ID (" << curmodhit.getPlane() << "," << curmodhit.getSector() << ") is not associated to a tower" << endreq;
  } // end loop over the modules

  // Save the map into the output file and print at screen a small message
  ofstream fout(m_global2local_path.c_str());
  for (int ireg=0;ireg!=nregions;++ireg) { // loop over the regions
      for (int ip=0;ip!=nplanes;++ip) { // loop over the regions
          m_log << MSG::INFO << "Region " << ireg << ", layer" << ip << " has " << grouped_modules[ireg][ip].size() << " modules" << endreq;
          unsigned int modnumber(0);
          for (auto curhash: grouped_modules[ireg][ip]) {
              fout << ireg << '\t' << ip << '\t' << curhash << '\t' << modnumber++ << endl;
          }
      }
  }
  fout.close();
}
