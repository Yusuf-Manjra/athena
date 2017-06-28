/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

     
///////////////////////////////////////////////////////////////////
//   Implementation file for class SiSpacePointsSeedMaker_ITK
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// AlgTool used for TRT_DriftCircleOnTrack object production
///////////////////////////////////////////////////////////////////
// Version 1.0 21/04/2004 I.Gavrilenko
///////////////////////////////////////////////////////////////////

#include <iostream>
#include <iomanip>

#include "TrkToolInterfaces/IPRD_AssociationTool.h"
#include "SiSpacePointsSeedTool_xk/SiSpacePointsSeedMaker_ITK.h"

///////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////

InDet::SiSpacePointsSeedMaker_ITK::SiSpacePointsSeedMaker_ITK
(const std::string& t,const std::string& n,const IInterface* p)
  : AthAlgTool(t,n,p),
    m_fieldServiceHandle("AtlasFieldSvc",n),    
    m_spacepointsSCT("SCT_SpacePoints"),
    m_spacepointsPixel("PixelSpacePoints"),
    m_spacepointsOverlap("OverlapSpacePoints"),
    m_assoTool("InDet::InDetPRD_AssociationToolGangedPixels")
{

  m_useassoTool = false ;
  m_useOverlap= true    ;
  m_state     = 0       ;
  m_pixel     = true    ;
  m_sct       = true    ;
  m_trigger   = false   ;
  m_checketa  = false   ;
  m_dbm       = false   ;
  m_state     = 0       ;
  m_nspoint   = 2       ;
  m_mode      = 0       ;
  m_nlist     = 0       ;
  m_endlist   = true    ;
  m_maxsize   = 10000   ;
  m_ptmin     =  500.   ;
  m_etamin    = 0.      ; m_etamax     = 2.7 ;
  m_r1min     = 0.      ; m_r1minv     = 0.  ; 
  m_r1max     = 600.    ; m_r1maxv     = 60. ;
  m_r2min     = 0.      ; m_r2minv     = 70. ;
  m_r2max     = 600.    ; m_r2maxv     = 200.;
  m_r3min     = 0.      ;
  m_r3max     = 600.    ;
  m_drmin     = 5.      ; m_drminv     = 20. ;    
  m_drmax     = 300.    ;    
  m_rapcut    = 2.7     ;
  m_zmin      = -250.   ;
  m_zmax      = +250.   ;
  m_dzver     = 5.      ;
  m_dzdrver   = .02     ;
  m_diver     = 10.     ;
  m_diverpps  = 5.      ;
  m_diversss  =  20     ;
  m_divermax  =  20.    ;
  m_dazmax    = .02     ;
  r_rmax      = 1100.   ;
  r_rmin      = 0.      ;
  m_umax      = 0.      ;
  r_rstep     =  2.     ;
  m_dzmaxPPP  = 600.    ; 
  r_Sorted    = 0       ;
  r_index     = 0       ;
  r_map       = 0       ;    
  m_maxsizeSP = 5000    ;
  m_maxOneSize= 5       ;
  m_SP        = 0       ;
  m_R         = 0       ;
  m_Tz        = 0       ;
  m_Er        = 0       ;
  m_U         = 0       ;
  m_V         = 0       ;
  m_X         = 0       ;
  m_Y         = 0       ;
  m_Zo        = 0       ;
  m_L         = 0       ;
  m_Im        = 0       ;
  m_OneSeeds  = 0       ;
  m_seedOutput= 0       ;
  m_maxNumberVertices = 99;
  
  m_radiusPPSmin = 200. ;
  m_radiusPPSmax = 600. ;
  m_rapydityPPSmax = 2.6;
  m_iminPPS        = 0  ;
  m_imaxPPS        = 1  ;

  m_xbeam[0]  = 0.      ; m_xbeam[1]= 1.; m_xbeam[2]=0.; m_xbeam[3]=0.;
  m_ybeam[0]  = 0.      ; m_ybeam[1]= 0.; m_ybeam[2]=1.; m_ybeam[3]=0.;
  m_zbeam[0]  = 0.      ; m_zbeam[1]= 0.; m_zbeam[2]=0.; m_zbeam[3]=1.;
  m_beamconditions         = "BeamCondSvc"       ;


  ///////////////////////////////////////////////////////////////////
  // Switches and cuts for ITK extended barrel: all turned OFF by default
  ///////////////////////////////////////////////////////////////////
  m_usePixelClusterCleanUp=false;              // switch to reject pixel clusters before seed search
  m_usePixelClusterCleanUp_sizeZcutsB=false;   // sizeZ cuts for barrel
  m_usePixelClusterCleanUp_sizePhicutsB=false; // size-phi cuts for barrel
  m_usePixelClusterCleanUp_sizeZcutsE=false;   // sizeZ cuts for end-cap
  m_usePixelClusterCleanUp_sizePhicutsE=false; // size-phi cuts for end-cap

  m_pix_sizePhiCut=10; // cut on the size-phi of pixel clusters
  m_pix_sizePhi2sizeZCut_p0B=3.5; // cut on size-phi/sizeZ of pixel clusters in barrel: p0/sizeZ-p1 
  m_pix_sizePhi2sizeZCut_p1B=0.5; // cut on size-phi/sizeZ of pixel clusters in barrel: p0/sizeZ-p1 
  m_pix_sizePhi2sizeZCut_p0E=3.0; // cut on size-phi/sizeZ of pixel clusters in end-cap: p0/sizeZ-p1 
  m_pix_sizePhi2sizeZCut_p1E=0.5; // cut on size-phi/sizeZ of pixel clusters in end-cap: p0/sizeZ-p1 

  m_useITKseedCuts=false;          // global switch to use special ITK cuts
  m_useITKseedCuts_dR=false;       // cut to reject seeds with both links in the same barrel pixel layer
  m_useITKseedCuts_PixHole=false;  // cut to reject seeds with Pixel hole
  m_useITKseedCuts_SctHole=false;  // cut to reject seeds with SCT hole
  m_useITKseedCuts_hit=false;      // cut to require bottom hit in barrel layer-0,1 if middle link is in layer-0 of endcap rings
  m_useITKseedCuts_dSize=false;    // cut to reject pixel barrel hits if |sizeZ-predicted_sizeZ| is too large
  m_useITKseedCuts_sizeDiff=false; // cut to reject pixel barrel hits if |sizeZ(layer-i)-sizeZ(layer-j)| is too large

  m_Nsigma_clSizeZcut=4.0;         // size of the cut on the cluster sizeZ at cluster clean-up stage: |sizeZ-predicted|<m_Nsigma_clSizeZcut*sigma

  parR_clSizeZ0cut[0][0]=2.62; parR_clSizeZ0cut[0][1]=1.82; parR_clSizeZ0cut[0][2]=1.42; parR_clSizeZ0cut[0][3]=1.31; parR_clSizeZ0cut[0][4]=1.27;    
  parR_clSizeZ0cut[1][0]=0.26; parR_clSizeZ0cut[1][1]=0.17; parR_clSizeZ0cut[1][2]=0.096; parR_clSizeZ0cut[1][3]=0.054; parR_clSizeZ0cut[1][4]=0.015;    
  parR_clSizeZ0cut[2][0]=0.094; parR_clSizeZ0cut[2][1]=0.060; parR_clSizeZ0cut[2][2]=0.0; parR_clSizeZ0cut[2][3]=0.0; parR_clSizeZ0cut[2][4]=0.0;    

  parL_clSizeZ0cut[0]=3.2; parL_clSizeZ0cut[1]=1.9; parL_clSizeZ0cut[2]=1.2; parL_clSizeZ0cut[3]=1.1; parL_clSizeZ0cut[4]=1.1;    

  parR_clSizeZcut[0][0]=1.25; parR_clSizeZcut[0][1]=1.25; parR_clSizeZcut[0][2]=1.25; parR_clSizeZcut[0][3]=1.25; parR_clSizeZcut[0][4]=1.25;
  parR_clSizeZcut[1][0]=0.021; parR_clSizeZcut[1][1]=0.006; parR_clSizeZcut[1][2]=0.0; parR_clSizeZcut[1][3]=0.0; parR_clSizeZcut[1][4]=0.0;

  parL_clSizeZcut[0]=1.5; parL_clSizeZcut[1]=1.25; parL_clSizeZcut[2]=1.25;  parL_clSizeZcut[3]=1.25; parL_clSizeZcut[4]=1.25;


  declareInterface<ISiSpacePointsSeedMaker>(this);

  declareProperty("AssociationTool"       ,m_assoTool              );
  declareProperty("usePixel"              ,m_pixel                 );
  declareProperty("useSCT"                ,m_sct                   );
  declareProperty("checkEta"              ,m_checketa              );
  declareProperty("useDBM"                ,m_dbm                   );
  declareProperty("etaMin"                ,m_etamin                );
  declareProperty("etaMax"                ,m_etamax                );  
  declareProperty("pTmin"                 ,m_ptmin                 );
  declareProperty("radMax"                ,r_rmax                  );
  declareProperty("radMin"                ,r_rmin                  );
  declareProperty("radStep"               ,r_rstep                 );
  declareProperty("maxSize"               ,m_maxsize               );
  declareProperty("maxSizeSP"             ,m_maxsizeSP             );
  declareProperty("minZ"                  ,m_zmin                  );
  declareProperty("maxZ"                  ,m_zmax                  );
  declareProperty("minRadius1"            ,m_r1min                 );
  declareProperty("minRadius2"            ,m_r2min                 );
  declareProperty("minRadius3"            ,m_r3min                 );
  declareProperty("maxRadius1"            ,m_r1max                 );
  declareProperty("maxRadius2"            ,m_r2max                 );
  declareProperty("maxRadius3"            ,m_r3max                 );
  declareProperty("mindRadius"            ,m_drmin                 );
  declareProperty("maxdRadius"            ,m_drmax                 );
  declareProperty("minVRadius1"           ,m_r1minv                );
  declareProperty("maxVRadius1"           ,m_r1maxv                );
  declareProperty("minVRadius2"           ,m_r2minv                );
  declareProperty("maxVRadius2"           ,m_r2maxv                );
  declareProperty("RapidityCut"           ,m_rapcut                );
  declareProperty("maxdZver"              ,m_dzver                 );
  declareProperty("maxdZdRver"            ,m_dzdrver               );
  declareProperty("maxdImpact"            ,m_diver                 );
  declareProperty("maxdImpactPPS"         ,m_diverpps              );
  declareProperty("maxdImpactSSS"         ,m_diversss              );
  declareProperty("maxdImpactForDecays"   ,m_divermax              );
  declareProperty("minSeedsQuality"       ,m_umax                  );
  declareProperty("dZmaxForPPPSeeds"      ,m_dzmaxPPP              );
  declareProperty("maxSeedsForSpacePoint" ,m_maxOneSize            );
  declareProperty("maxNumberVertices"     ,m_maxNumberVertices     );
  declareProperty("SpacePointsSCTName"    ,m_spacepointsSCT        );
  declareProperty("SpacePointsPixelName"  ,m_spacepointsPixel      );
  declareProperty("SpacePointsOverlapName",m_spacepointsOverlap    );
  declareProperty("BeamConditionsService" ,m_beamconditions        ); 
  declareProperty("useOverlapSpCollection",m_useOverlap            );
  declareProperty("UseAssociationTool"    ,m_useassoTool           ); 
  declareProperty("MagFieldSvc"           ,m_fieldServiceHandle    );

  ///////////////////////////////////////////////////////////////////
  // Properties for special ITK extended barrel cuts
  ///////////////////////////////////////////////////////////////////
  declareProperty("usePixelClusterCleanUp",            m_usePixelClusterCleanUp);             
  declareProperty("usePixelClusterCleanUpSizeZcutsB",  m_usePixelClusterCleanUp_sizeZcutsB);  
  declareProperty("usePixelClusterCleanUpSizePhicutsB",m_usePixelClusterCleanUp_sizePhicutsB);
  declareProperty("usePixelClusterCleanUpSizeZcutsE",  m_usePixelClusterCleanUp_sizeZcutsE);  
  declareProperty("usePixelClusterCleanUpSizePhicutsE",m_usePixelClusterCleanUp_sizePhicutsE);
  
  declareProperty("PixSizePhiCut", m_pix_sizePhiCut); 
  declareProperty("PixSizePhi2sizeZCutBp0", m_pix_sizePhi2sizeZCut_p0B); 
  declareProperty("PixSizePhi2sizeZCutBp1", m_pix_sizePhi2sizeZCut_p1B); 
  declareProperty("PixSizePhi2sizeZCutEp0", m_pix_sizePhi2sizeZCut_p0E); 
  declareProperty("PixSizePhi2sizeZCutEp1", m_pix_sizePhi2sizeZCut_p1E); 

  declareProperty("useITKseedCuts",          m_useITKseedCuts);          
  declareProperty("useITKseedCutsDeltaR",    m_useITKseedCuts_dR);       
  declareProperty("useITKseedCutsPixHole",   m_useITKseedCuts_PixHole);  
  declareProperty("useITKseedCutsSctHole",   m_useITKseedCuts_SctHole);  
  declareProperty("useITKseedCutsL01hit",    m_useITKseedCuts_hit);      
  declareProperty("useITKseedCutsDeltaSize", m_useITKseedCuts_dSize);    
  declareProperty("useITKseedCutsSizeDiff",  m_useITKseedCuts_sizeDiff); 

  declareProperty("NsigmaClusSizeZcut", m_Nsigma_clSizeZcut);

}

///////////////////////////////////////////////////////////////////
// Destructor  
///////////////////////////////////////////////////////////////////

InDet::SiSpacePointsSeedMaker_ITK::~SiSpacePointsSeedMaker_ITK()
{
  if(r_index ) delete [] r_index ;
  if(r_map   ) delete [] r_map   ; 
  if(r_Sorted) delete [] r_Sorted;

  // Delete seeds
  //
  for(i_seed=l_seeds.begin(); i_seed!=l_seeds.end (); ++i_seed) {
    delete *i_seed;
  }
  // Delete space points for reconstruction
  //
  i_spforseed=l_spforseed.begin();
  for(; i_spforseed!=l_spforseed.end(); ++i_spforseed) {
    delete *i_spforseed;
  } 
  if(m_seedOutput) delete m_seedOutput; 

  if(m_SP) delete [] m_SP;
  if(m_R ) delete [] m_R ;
  if(m_X ) delete [] m_X ;
  if(m_Y ) delete [] m_Y ;
  if(m_Tz) delete [] m_Tz;
  if(m_Er) delete [] m_Er;
  if(m_U ) delete [] m_U ;
  if(m_V ) delete [] m_V ;
  if(m_Zo) delete [] m_Zo;
  if(m_L ) delete [] m_L ;
  if(m_Im) delete [] m_Im ;
  if(m_OneSeeds) delete [] m_OneSeeds;
 }

///////////////////////////////////////////////////////////////////
// Initialisation
///////////////////////////////////////////////////////////////////

StatusCode InDet::SiSpacePointsSeedMaker_ITK::initialize()
{
  StatusCode sc = AlgTool::initialize(); 

  // Get beam geometry
  //
  p_beam = 0;
  if(m_beamconditions!="") {
    sc = service(m_beamconditions,p_beam);
  }

  // Get magnetic field service
  //
  if( !m_fieldServiceHandle.retrieve() ){
    ATH_MSG_FATAL("Failed to retrieve " << m_fieldServiceHandle );
    return StatusCode::FAILURE;
  }    
  ATH_MSG_DEBUG("Retrieved " << m_fieldServiceHandle );
  m_fieldService = &*m_fieldServiceHandle;

  // Get tool for track-prd association
  //
  if( m_useassoTool ) {
    if( m_assoTool.retrieve().isFailure()) {
      msg(MSG::FATAL)<<"Failed to retrieve tool "<< m_assoTool<<endmsg; 
      return StatusCode::FAILURE;
    } else {
      msg(MSG::INFO) << "Retrieved tool " << m_assoTool << endmsg;
    }
  }

  if(r_rmax < 1100.) r_rmax = 1100.; 

  // Build framework
  //
  buildFrameWork();
  m_CmSp .reserve(500); 
  m_CmSpn.reserve(500); 

  // Get output print level
  //
  m_outputlevel = msg().level()-MSG::DEBUG;
  if(m_outputlevel<=0) {
    m_nprint=0; msg(MSG::DEBUG)<<(*this)<<endmsg;
  }
  m_umax = 100.-fabs(m_umax)*300.;
  return sc;
}

///////////////////////////////////////////////////////////////////
// Finalize
///////////////////////////////////////////////////////////////////

StatusCode InDet::SiSpacePointsSeedMaker_ITK::finalize()
{
   StatusCode sc = AlgTool::finalize(); return sc;
}

///////////////////////////////////////////////////////////////////
// Initialize tool for new event 
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ITK::newEvent(int iteration) 
{
  m_iteration0 = iteration;
  m_trigger = false;
  if(!m_pixel && !m_sct) return; 

  iteration <=0 ? m_iteration = 0 : m_iteration = iteration;
  erase();
  m_dzdrmin =  m_dzdrmin0;
  m_dzdrmax =  m_dzdrmax0;

  if(!m_iteration) {
    buildBeamFrameWork();

    double f[3], gP[3] ={10.,10.,0.}; 
    if(m_fieldService->solenoidOn()) {
      m_fieldService->getFieldZR(gP,f); m_K = 2./(300.*f[2]);
    }
    else m_K = 2./(300.* 5. );

    m_ipt2K     = m_ipt2/(m_K*m_K);
    m_ipt2C     = m_ipt2*m_COF    ;
    m_COFK      = m_COF*(m_K*m_K) ;  
    i_spforseed = l_spforseed.begin();
  }
  else {
    r_first = 0; fillLists(); return;
  }

  m_checketa = m_dzdrmin > 1.;

  float irstep = 1./r_rstep;
  int   irmax  = r_size-1  ;
  for(int i=0; i!=m_nr; ++i) {int n = r_index[i]; r_map[n] = 0; r_Sorted[n].clear();}
  m_ns = m_nr = 0;

  // Get pixels space points containers from store gate 
  //
  r_first = 0;
  if(m_pixel) {

    if(m_spacepointsPixel.isValid()) {

      SpacePointContainer::const_iterator spc  =  m_spacepointsPixel->begin();
      SpacePointContainer::const_iterator spce =  m_spacepointsPixel->end  ();

      for(; spc != spce; ++spc) {

	SpacePointCollection::const_iterator sp  = (*spc)->begin();
	SpacePointCollection::const_iterator spe = (*spc)->end  ();
    
	for(; sp != spe; ++sp) {

	  if ((m_useassoTool &&  isUsed(*sp)) || (*sp)->r() > r_rmax || (*sp)->r() < r_rmin ) continue;
	  InDet::SiSpacePointForSeedITK* sps = newSpacePoint((*sp)); if(!sps) continue;
	  int   ir = int(sps->radius()*irstep); if(ir>irmax) continue;
	  r_Sorted[ir].push_back(sps); ++r_map[ir];
	  if(r_map[ir]==1) r_index[m_nr++] = ir;
	  if(ir > r_first) r_first = ir;
	  ++m_ns;
	}
      }
    }
    ++r_first;
  }

  // Get sct space points containers from store gate 
  //
  if(m_sct) {

    if(m_spacepointsSCT.isValid()) {

      SpacePointContainer::const_iterator spc  =  m_spacepointsSCT->begin();
      SpacePointContainer::const_iterator spce =  m_spacepointsSCT->end  ();

      for(; spc != spce; ++spc) {

	SpacePointCollection::const_iterator sp  = (*spc)->begin();
	SpacePointCollection::const_iterator spe = (*spc)->end  ();
    
	for(; sp != spe; ++sp) {

	  if ((m_useassoTool &&  isUsed(*sp)) || (*sp)->r() > r_rmax || (*sp)->r() < r_rmin ) continue;

	  InDet::SiSpacePointForSeedITK* sps = newSpacePoint((*sp)); if(!sps) continue;

	  int   ir = int(sps->radius()*irstep); if(ir>irmax) continue;
	  r_Sorted[ir].push_back(sps); ++r_map[ir];
	  if(r_map[ir]==1) r_index[m_nr++] = ir;
	  ++m_ns;
	}
      }
    }

    // Get sct overlap space points containers from store gate 
    //
    if(m_useOverlap) {

      if(m_spacepointsOverlap.isValid()) {
	
	SpacePointOverlapCollection::const_iterator sp  = m_spacepointsOverlap->begin();
	SpacePointOverlapCollection::const_iterator spe = m_spacepointsOverlap->end  ();
	
	for (; sp!=spe; ++sp) {

	  if ((m_useassoTool &&  isUsed(*sp)) || (*sp)->r() > r_rmax || (*sp)->r() < r_rmin) continue;

	  InDet::SiSpacePointForSeedITK* sps = newSpacePoint((*sp)); if(!sps) continue;

	  int   ir = int(sps->radius()*irstep); if(ir>irmax) continue;
	  r_Sorted[ir].push_back(sps); ++r_map[ir];
	  if(r_map[ir]==1) r_index[m_nr++] = ir;
	  ++m_ns;
	}
      }
    }
  }

  if(iteration < 0) r_first = 0;
  fillLists();
}

///////////////////////////////////////////////////////////////////
// Initialize tool for new region
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ITK::newRegion
(const std::vector<IdentifierHash>& vPixel, const std::vector<IdentifierHash>& vSCT)
{
  m_iteration  = 0    ;
  m_trigger    = false;
  erase();
  if(!m_pixel && !m_sct) return;

  m_dzdrmin =  m_dzdrmin0;
  m_dzdrmax =  m_dzdrmax0;

  buildBeamFrameWork();

  double f[3], gP[3] ={10.,10.,0.}; 

  if(m_fieldService->solenoidOn()) {
      m_fieldService->getFieldZR(gP,f); m_K = 2./(300.*f[2]);
    }
  else m_K = 2./(300.* 5. );

  m_ipt2K     = m_ipt2/(m_K*m_K);
  m_ipt2C     = m_ipt2*m_COF    ;
  m_COFK      = m_COF*(m_K*m_K) ;  

  i_spforseed = l_spforseed.begin();

  float irstep = 1./r_rstep;
  int   irmax  = r_size-1  ;

  r_first      = 0         ;
  m_checketa   = false     ;

  for(int i=0; i!=m_nr; ++i) {int n = r_index[i]; r_map[n] = 0; r_Sorted[n].clear();}
  m_ns = m_nr = 0;

  // Get pixels space points containers from store gate 
  //
  if(m_pixel && vPixel.size()) {
    
    if(m_spacepointsPixel.isValid()) {

      SpacePointContainer::const_iterator spce =  m_spacepointsPixel->end  ();

      std::vector<IdentifierHash>::const_iterator l = vPixel.begin(), le = vPixel.end();

      // Loop through all trigger collections
      //
      for(; l!=le; ++l) {
	
	SpacePointContainer::const_iterator  w =  m_spacepointsPixel->indexFind((*l));
	if(w==spce) continue;
	SpacePointCollection::const_iterator sp = (*w)->begin(), spe = (*w)->end();

	for(; sp != spe; ++sp) {

	  float r = (*sp)->r(); if(r > r_rmax || r < r_rmin) continue;
	  InDet::SiSpacePointForSeedITK* sps = newSpacePoint((*sp)); 
	  int   ir = int(sps->radius()*irstep); if(ir>irmax) ir = irmax;
	  r_Sorted[ir].push_back(sps); ++r_map[ir];
	  if(r_map[ir]==1) r_index[m_nr++] = ir;
	  ++m_ns;
	}
      }
    }
  }

  // Get sct space points containers from store gate 
  //
  if(m_sct && vSCT.size()) {

    if(m_spacepointsSCT.isValid()) {

      SpacePointContainer::const_iterator spce =  m_spacepointsSCT->end  ();

      std::vector<IdentifierHash>::const_iterator l = vSCT.begin(), le = vSCT.end();

      // Loop through all trigger collections
      //
      for(; l!=le; ++l) {

	SpacePointContainer::const_iterator  w =  m_spacepointsSCT->indexFind((*l));
	if(w==spce) continue;
	SpacePointCollection::const_iterator sp = (*w)->begin(), spe = (*w)->end();

	for(; sp != spe; ++sp) {

	  float r = (*sp)->r(); if(r > r_rmax || r < r_rmin) continue;
	  InDet::SiSpacePointForSeedITK* sps = newSpacePoint((*sp));
	  int   ir = int(sps->radius()*irstep); if(ir>irmax) ir = irmax;
	  r_Sorted[ir].push_back(sps); ++r_map[ir];
	  if(r_map[ir]==1) r_index[m_nr++] = ir;
	  ++m_ns;
	}
      }
    }
  }
  fillLists();
}

///////////////////////////////////////////////////////////////////
// Initialize tool for new region
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ITK::newRegion
(const std::vector<IdentifierHash>& vPixel, const std::vector<IdentifierHash>& vSCT,const IRoiDescriptor& IRD)
{
  newRegion(vPixel,vSCT);
  m_trigger = true;

  double dzdrmin = 1./tan(2.*atan(exp(-IRD.etaMinus())));
  double dzdrmax = 1./tan(2.*atan(exp(-IRD.etaPlus ())));
 
  m_zminB        = IRD.zedMinus()-m_zbeam[0];    // min bottom Z
  m_zmaxB        = IRD.zedPlus ()-m_zbeam[0];    // max bottom Z
  m_zminU        = m_zminB+550.*dzdrmin;
  m_zmaxU        = m_zmaxB+550.*dzdrmax;
  double fmax    = IRD.phiPlus ();
  double fmin    = IRD.phiMinus();
  if(fmin > fmax) fmin-=(2.*M_PI);
  m_ftrig        = (fmin+fmax)*.5;
  m_ftrigW       = (fmax-fmin)*.5;
}

///////////////////////////////////////////////////////////////////
// Methods to initilize different strategies of seeds production
// with two space points with or without vertex constraint
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ITK::find2Sp(const std::list<Trk::Vertex>& lv) 
{
  m_zminU     = m_zmin;
  m_zmaxU     = m_zmax;

  int mode; lv.begin()!=lv.end() ?  mode = 1 : mode = 0;
  bool newv = newVertices(lv);
  
  if(newv || !m_state || m_nspoint!=2 || m_mode!=mode || m_nlist) {

    i_seede   = l_seeds.begin();
    m_state   = 1   ;
    m_nspoint = 2   ;
    m_nlist   = 0   ;
    m_mode    = mode;
    m_endlist = true;
    m_fvNmin  = 0   ;
    m_fNmin   = 0   ;
    m_zMin    = 0   ;
    production2Sp ();
  }
  i_seed  = l_seeds.begin();
  
  if(m_outputlevel<=0) {
    m_nprint=1; msg(MSG::DEBUG)<<(*this)<<endmsg;
  }
}

///////////////////////////////////////////////////////////////////
// Methods to initilize different strategies of seeds production
// with three space points with or without vertex constraint
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ITK::find3Sp(const std::list<Trk::Vertex>& lv) 
{
  m_zminU     = m_zmin;
  m_zmaxU     = m_zmax;

  int mode; lv.begin()!=lv.end() ? mode = 3 : mode = 2; 
  bool newv = newVertices(lv);

  if(newv || !m_state || m_nspoint!=3 || m_mode!=mode || m_nlist) {
    i_seede   = l_seeds.begin() ;
    m_state   = 1               ;
    m_nspoint = 3               ;
    m_nlist   = 0               ;
    m_mode    = mode            ;
    m_endlist = true            ;
    m_fvNmin  = 0               ;
    m_fNmin   = 0               ;
    m_zMin    = 0               ;
    production3Sp();
  }
  i_seed  = l_seeds.begin();
  m_seed  = m_seeds.begin();

  if(m_outputlevel<=0) {
    m_nprint=1; msg(MSG::DEBUG)<<(*this)<<endmsg;
  }
}

///////////////////////////////////////////////////////////////////
// Methods to initilize different strategies of seeds production
// with three space points with or without vertex constraint
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ITK::find3Sp(const std::list<Trk::Vertex>& lv,const double* ZVertex) 
{
  m_zminU     = ZVertex[0]; if(m_zminU < m_zmin) m_zminU = m_zmin; 
  m_zmaxU     = ZVertex[1]; if(m_zmaxU > m_zmax) m_zmaxU = m_zmax;

  int mode; lv.begin()!=lv.end() ? mode = 3 : mode = 2; 
  bool newv = newVertices(lv);

  if(newv || !m_state || m_nspoint!=3 || m_mode!=mode || m_nlist) {
    i_seede   = l_seeds.begin() ;
    m_state   = 1               ;
    m_nspoint = 3               ;
    m_nlist   = 0               ;
    m_mode    = mode            ;
    m_endlist = true            ;
    m_fvNmin  = 0               ;
    m_fNmin   = 0               ;
    m_zMin    = 0               ;
    production3Sp();
  }
  i_seed  = l_seeds.begin();
  m_seed  = m_seeds.begin();

  if(m_outputlevel<=0) {
    m_nprint=1; msg(MSG::DEBUG)<<(*this)<<endmsg;
  }
}

///////////////////////////////////////////////////////////////////
// Methods to initilize different strategies of seeds production
// with variable number space points with or without vertex constraint
// Variable means (2,3,4,....) any number space points
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ITK::findVSp (const std::list<Trk::Vertex>& lv)
{
  m_zminU     = m_zmin;
  m_zmaxU     = m_zmax;

  int mode; lv.begin()!=lv.end() ? mode = 6 : mode = 5; 
  bool newv = newVertices(lv);
  
  if(newv || !m_state || m_nspoint!=4 || m_mode!=mode || m_nlist) {

    i_seede   = l_seeds.begin() ;
    m_state   = 1               ;
    m_nspoint = 4               ;
    m_nlist   = 0               ;
    m_mode    = mode            ;
    m_endlist = true            ;
    m_fvNmin  = 0               ;
    m_fNmin   = 0               ;
    m_zMin    = 0               ;
    production3Sp();
  }
  i_seed  = l_seeds.begin();
  m_seed  = m_seeds.begin();

  if(m_outputlevel<=0) {
    m_nprint=1; msg(MSG::DEBUG)<<(*this)<<endmsg;
  }
}

///////////////////////////////////////////////////////////////////
// Dumps relevant information into the MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::SiSpacePointsSeedMaker_ITK::dump( MsgStream& out ) const
{
  if(m_nprint)  return dumpEvent(out); return dumpConditions(out);
}

///////////////////////////////////////////////////////////////////
// Dumps conditions information into the MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::SiSpacePointsSeedMaker_ITK::dumpConditions( MsgStream& out ) const
{
  int n = 42-m_spacepointsPixel.name().size();
  std::string s2; for(int i=0; i<n; ++i) s2.append(" "); s2.append("|");
  n     = 42-m_spacepointsSCT.name().size();
  std::string s3; for(int i=0; i<n; ++i) s3.append(" "); s3.append("|");
  n     = 42-m_spacepointsOverlap.name().size();
  std::string s4; for(int i=0; i<n; ++i) s4.append(" "); s4.append("|");
  n     = 42-m_beamconditions.size();
  std::string s5; for(int i=0; i<n; ++i) s5.append(" "); s5.append("|");


  out<<"|---------------------------------------------------------------------|"
     <<std::endl;
  out<<"| Pixel    space points   | "<<m_spacepointsPixel.name() <<s2
     <<std::endl;
  out<<"| SCT      space points   | "<<m_spacepointsSCT.name()<<s3
     <<std::endl;
  out<<"| Overlap  space points   | "<<m_spacepointsOverlap.name()<<s4
     <<std::endl;
  out<<"| BeamConditionsService   | "<<m_beamconditions<<s5
     <<std::endl;
  out<<"| usePixel                | "
     <<std::setw(12)<<m_pixel 
     <<"                              |"<<std::endl;
  out<<"| useSCT                  | "
     <<std::setw(12)<<m_sct 
     <<"                              |"<<std::endl;
  out<<"| maxSize                 | "
     <<std::setw(12)<<m_maxsize 
     <<"                              |"<<std::endl;
  out<<"| maxSizeSP               | "
     <<std::setw(12)<<m_maxsizeSP
     <<"                              |"<<std::endl;
  out<<"| pTmin  (mev)            | "
     <<std::setw(12)<<std::setprecision(5)<<m_ptmin
     <<"                              |"<<std::endl;
  out<<"| |rapidity|          <=  | " 
     <<std::setw(12)<<std::setprecision(5)<<m_rapcut
     <<"                              |"<<std::endl;
  out<<"| max radius SP           | "
     <<std::setw(12)<<std::setprecision(5)<<r_rmax 
     <<"                              |"<<std::endl;
  out<<"| min radius SP           | "
     <<std::setw(12)<<std::setprecision(5)<<r_rmin 
     <<"                              |"<<std::endl;
  out<<"| radius step             | "
     <<std::setw(12)<<std::setprecision(5)<<r_rstep
     <<"                              |"<<std::endl;
  out<<"| min Z-vertex position   | "
     <<std::setw(12)<<std::setprecision(5)<<m_zmin
     <<"                              |"<<std::endl;
  out<<"| max Z-vertex position   | "
     <<std::setw(12)<<std::setprecision(5)<<m_zmax
     <<"                              |"<<std::endl;
  out<<"| min radius first  SP(3) | "
     <<std::setw(12)<<std::setprecision(5)<<m_r1min
     <<"                              |"<<std::endl;
  out<<"| min radius second SP(3) | "
     <<std::setw(12)<<std::setprecision(5)<<m_r2min
     <<"                              |"<<std::endl;
  out<<"| min radius last   SP(3) | "
     <<std::setw(12)<<std::setprecision(5)<<m_r3min
     <<"                              |"<<std::endl;
  out<<"| max radius first  SP(3) | "
     <<std::setw(12)<<std::setprecision(4)<<m_r1max
     <<"                              |"<<std::endl;
  out<<"| max radius second SP(3) | "
     <<std::setw(12)<<std::setprecision(5)<<m_r2max
     <<"                              |"<<std::endl;
  out<<"| max radius last   SP(3) | "
     <<std::setw(12)<<std::setprecision(5)<<m_r3max
     <<"                              |"<<std::endl;
  out<<"| min radius first  SP(2) | "
     <<std::setw(12)<<std::setprecision(5)<<m_r1minv
     <<"                              |"<<std::endl;
  out<<"| min radius second SP(2) | "
     <<std::setw(12)<<std::setprecision(5)<<m_r2minv
     <<"                              |"<<std::endl;
  out<<"| max radius first  SP(2) | "
     <<std::setw(12)<<std::setprecision(5)<<m_r1maxv
     <<"                              |"<<std::endl;
  out<<"| max radius second SP(2) | "
     <<std::setw(12)<<std::setprecision(5)<<m_r2maxv
     <<"                              |"<<std::endl;
  out<<"| min space points dR     | "
     <<std::setw(12)<<std::setprecision(5)<<m_drmin
     <<"                              |"<<std::endl;
  out<<"| max space points dR     | "
     <<std::setw(12)<<std::setprecision(5)<<m_drmax
     <<"                              |"<<std::endl;
  out<<"| max dZ    impact        | "
     <<std::setw(12)<<std::setprecision(5)<<m_dzver 
     <<"                              |"<<std::endl;
  out<<"| max dZ/dR impact        | "
     <<std::setw(12)<<std::setprecision(5)<<m_dzdrver 
     <<"                              |"<<std::endl;
  out<<"| max       impact        | "
     <<std::setw(12)<<std::setprecision(5)<<m_diver
     <<"                              |"<<std::endl;
  out<<"| max       impact pps    | "
     <<std::setw(12)<<std::setprecision(5)<<m_diverpps
     <<"                              |"<<std::endl;
  out<<"| max       impact sss    | "
    <<std::setw(12)<<std::setprecision(5)<<m_diversss
    <<"                              |"<<std::endl;
  out<<"|---------------------------------------------------------------------|"
     <<std::endl;
  out<<"| Beam X center           | "
     <<std::setw(12)<<std::setprecision(5)<<m_xbeam[0]
     <<"                              |"<<std::endl;
  out<<"| Beam Y center           | "
     <<std::setw(12)<<std::setprecision(5)<<m_ybeam[0]
     <<"                              |"<<std::endl;
  out<<"| Beam Z center           | "
     <<std::setw(12)<<std::setprecision(5)<<m_zbeam[0]
     <<"                              |"<<std::endl;
  out<<"| Beam X-axis direction   | "
     <<std::setw(12)<<std::setprecision(5)<<m_xbeam[1]
     <<std::setw(12)<<std::setprecision(5)<<m_xbeam[2]
     <<std::setw(12)<<std::setprecision(5)<<m_xbeam[3]
     <<"      |"<<std::endl;
  out<<"| Beam Y-axis direction   | "
     <<std::setw(12)<<std::setprecision(5)<<m_ybeam[1]
     <<std::setw(12)<<std::setprecision(5)<<m_ybeam[2]
     <<std::setw(12)<<std::setprecision(5)<<m_ybeam[3]
     <<"      |"<<std::endl;
  out<<"| Beam Z-axis direction   | "
     <<std::setw(12)<<std::setprecision(5)<<m_zbeam[1]
     <<std::setw(12)<<std::setprecision(5)<<m_zbeam[2]
     <<std::setw(12)<<std::setprecision(5)<<m_zbeam[3]
     <<"      |"<<std::endl;
  out<<"|---------------------------------------------------------------------|"
     <<std::endl;
  return out;
}

///////////////////////////////////////////////////////////////////
// Dumps event information into the MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::SiSpacePointsSeedMaker_ITK::dumpEvent( MsgStream& out ) const
{
  //const float pi2    = 2.*M_PI;
  out<<"|---------------------------------------------------------------------|"
     <<std::endl;
  out<<"| m_ns                    | "
     <<std::setw(12)<<m_ns
     <<"                              |"<<std::endl;
  out<<"| m_nsaz                  | "
     <<std::setw(12)<<m_nsaz
     <<"                              |"<<std::endl;
  out<<"| m_nsazv                 | "
     <<std::setw(12)<<m_nsazv
     <<"                              |"<<std::endl;
  out<<"| seeds                   | "
     <<std::setw(12)<<l_seeds.size()
     <<"                              |"<<std::endl;
  out<<"|---------------------------------------------------------------------|"
     <<std::endl;
  /*
  if(m_outputlevel==0) return out; 

  out<<"|-------------|--------|-------|-------|-------|-------|-------|";
  out<<"-------|-------|-------|-------|-------|-------|"
     <<std::endl;

  out<<"|  Azimuthal  |    n   | z[ 0] | z[ 1] | z[ 2] | z[ 3] | z[4]  |";
  out<<" z[ 5] | z[ 6] | z[ 7] | z[ 8] | z[ 9] | z[10] |"
     <<std::endl;
  out<<"|-------------|--------|-------|-------|-------|-------|-------|";
  out<<"-------|-------|-------|-------|-------|-------|"
     <<std::endl;
  
  float sF1 = pi2/float(m_fNmax+1);
  
  
  for(int f=0; f<=m_fNmax; ++f) {
    out<<"|  "
       <<std::setw(10)<<std::setprecision(4)<<sF1*float(f)<<" | "
       <<std::setw(6)<<rf_map[f]<<" |";
    for(int z=0; z!=11; ++z) {
      out<<std::setw(6)<<rfz_map[(f*11+z)]<<" |";
    }
    out<<std::endl;
  } 
  out<<"|-------------|--------|-------|-------|-------|-------|-------|";
  out<<"-------|-------|-------|-------|-------|-------|"
     <<std::endl;
  out<<std::endl;
  */
  return out;
}

///////////////////////////////////////////////////////////////////
// Dumps relevant information into the ostream
///////////////////////////////////////////////////////////////////

std::ostream& InDet::SiSpacePointsSeedMaker_ITK::dump( std::ostream& out ) const
{
  return out;
}

///////////////////////////////////////////////////////////////////
// Overload of << operator MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::operator    << 
  (MsgStream& sl,const InDet::SiSpacePointsSeedMaker_ITK& se)
{ 
  return se.dump(sl); 
}

///////////////////////////////////////////////////////////////////
// Overload of << operator std::ostream
///////////////////////////////////////////////////////////////////

std::ostream& InDet::operator << 
  (std::ostream& sl,const InDet::SiSpacePointsSeedMaker_ITK& se)
{ 
  return se.dump(sl); 
}   

///////////////////////////////////////////////////////////////////
// Find next set space points
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ITK::findNext () 
{
  if(m_endlist) return;

  i_seede = l_seeds.begin();

  if     (m_mode==0 || m_mode==1) production2Sp ();
  else if(m_mode==2 || m_mode==3) production3Sp ();
  else if(m_mode==5 || m_mode==6) production3Sp ();

  i_seed  = l_seeds.begin();
  m_seed  = m_seeds.begin(); 
  ++m_nlist;
}                       

///////////////////////////////////////////////////////////////////
// New and old list vertices comparison
///////////////////////////////////////////////////////////////////

bool InDet::SiSpacePointsSeedMaker_ITK::newVertices(const std::list<Trk::Vertex>& lV)
{
  unsigned int s1 = l_vertex.size(); 
  unsigned int s2 = lV      .size(); 

  m_isvertex = false;
  if(s1==0 && s2==0) return false;

  std::list<Trk::Vertex>::const_iterator v;
  l_vertex.clear();
  if(s2 == 0) return false;

  m_isvertex = true;
  for(v=lV.begin(); v!=lV.end(); ++v) {l_vertex.insert(float((*v).position().z()));}

  m_zminU = (*l_vertex. begin())-20.; if( m_zminU < m_zmin) m_zminU = m_zmin;
  m_zmaxU = (*l_vertex.rbegin())+20.; if( m_zmaxU > m_zmax) m_zmaxU = m_zmax;

  return false;
}

///////////////////////////////////////////////////////////////////
// Initiate frame work for seed generator
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ITK::buildFrameWork() 
{
  m_ptmin     = fabs(m_ptmin);  
  
  if(m_ptmin < 100.) m_ptmin = 100.;

  if(m_diversss < m_diver   ) m_diversss = m_diver   ; 
  if(m_diverpps < m_diver   ) m_diverpps = m_diver   ;
  if(m_divermax < m_diversss) m_divermax = m_diversss;

  if(fabs(m_etamin) < .1) m_etamin = -m_etamax ;
  m_dzdrmax0  = 1./tan(2.*atan(exp(-m_etamax)));
  m_dzdrmin0  = 1./tan(2.*atan(exp(-m_etamin)));

  m_dzdrmaxPPS= 1./tan(2.*atan(exp(-m_rapydityPPSmax)));
  
  m_COF       =  134*.05*9.                    ;
  m_ipt       = 1./fabs(m_ptmin)               ;
  m_ipt2      = m_ipt*m_ipt                    ;
  m_K         = 0.                             ;

  m_ns = m_nsaz = m_nsazv = m_nr = m_nrfz = m_nrfzv = 0;

  // Build radius sorted containers
  //
  r_size = int((r_rmax+.1)/r_rstep);
  r_Sorted = new std::list<InDet::SiSpacePointForSeedITK*>[r_size];
  r_index  = new int[r_size];
  r_map    = new int[r_size];  
  m_nr   = 0; for(int i=0; i!=r_size; ++i) {r_index[i]=0; r_map[i]=0;}

  m_Bot.reserve(500);
  m_Top.reserve(500);

  // Build radius-azimuthal sorted containers
  //
  const float pi2     = 2.*M_PI            ;
  float ptm = 400.; if(m_ptmin < ptm) ptm = m_ptmin;

  int   NFmax    = 200            ;

  // Build radius-azimuthal-Z sorted containers
  //
  m_nrfz  = 0; for(int i=0; i!=2211; ++i) {rfz_index [i]=0; rfz_map [i]=0;}

  m_fNmax[0] = int(pi2/AzimuthalStep(m_ptmin,m_diversss,400.,1000.)); 
  if(m_fNmax[0] > NFmax) m_fNmax[0] = NFmax;  
  if(m_fNmax[0] < 10   ) m_fNmax[0] = 10   ;  
  m_sF[0] = float((m_fNmax[0]+1))/pi2;
  
  // Build maps for radius-azimuthal-Z sorted collections for SSS seeds
  //
  for(int f=0; f<=m_fNmax[0]; ++f) {

    int fb = f-1; if(fb<0         ) fb=m_fNmax[0]; 
    int ft = f+1; if(ft>m_fNmax[0]) ft=0; 
    
    // For each azimuthal region loop through all Z regions
    //
    for(int z=0; z!=11; ++z) {
 
      int a        = f *11+z;
      int b        = fb*11+z;
      int c        = ft*11+z;
      rfz_b [0][a]    = 3; rfz_t [0][a]    = 3;
      rfz_ib[0][a][0] = a; rfz_it[0][a][0] = a; 
      rfz_ib[0][a][1] = b; rfz_it[0][a][1] = b; 
      rfz_ib[0][a][2] = c; rfz_it[0][a][2] = c; 
      if     (z==5) {

	rfz_t [0][a]    = 9 ;
	rfz_it[0][a][3] = a+1; 
	rfz_it[0][a][4] = b+1; 
	rfz_it[0][a][5] = c+1; 
	rfz_it[0][a][6] = a-1; 
	rfz_it[0][a][7] = b-1; 
	rfz_it[0][a][8] = c-1; 
      }
      else if(z> 5) {

	rfz_b [0][a]    = 6 ;
	rfz_ib[0][a][3] = a-1; 
	rfz_ib[0][a][4] = b-1; 
	rfz_ib[0][a][5] = c-1; 

	if(z<10) {

	  rfz_t [0][a]    = 6 ;
	  rfz_it[0][a][3] = a+1; 
	  rfz_it[0][a][4] = b+1; 
	  rfz_it[0][a][5] = c+1; 
	}
      }
      else {

	rfz_b [0][a]    = 6 ;
	rfz_ib[0][a][3] = a+1; 
	rfz_ib[0][a][4] = b+1; 
	rfz_ib[0][a][5] = c+1; 

	if(z>0) {

	  rfz_t [0][a]    = 6 ;
	  rfz_it[0][a][3] = a-1; 
	  rfz_it[0][a][4] = b-1; 
	  rfz_it[0][a][5] = c-1; 
	}
      }
      
      if     (z==3) {
	rfz_b [0][a]    = 9;
	rfz_ib[0][a][6] = a+2; 
	rfz_ib[0][a][7] = b+2; 
	rfz_ib[0][a][8] = c+2; 
      }
      else if(z==7) {
	rfz_b [0][a]      = 9;
	rfz_ib[0][a][6] = a-2; 
	rfz_ib[0][a][7] = b-2; 
	rfz_ib[0][a][8] = c-2; 
      }
    }
  }
  m_fNmax[1] = int(pi2/AzimuthalStep(m_ptmin,m_diver,40.,320.));
  if(m_fNmax[1] > NFmax) m_fNmax[1] = NFmax; 
  if(m_fNmax[1] < 10   ) m_fNmax[1] = 10   ;  
  m_sF[1] = float(m_fNmax[1]+1)/pi2;

  // Build maps for radius-azimuthal-Z sorted collections for PPP seeds
  //
  for(int f=0; f<=m_fNmax[1]; ++f) {

    int fb = f-1; if(fb<0         ) fb=m_fNmax[1]; 
    int ft = f+1; if(ft>m_fNmax[1]) ft=0; 
    
    // For each azimuthal region loop through all Z regions
    //
    for(int z=0; z!=11; ++z) {
 
      int a        = f *11+z;
      int b        = fb*11+z;
      int c        = ft*11+z;
      rfz_b [1][a]    = 3; rfz_t [1][a]    = 3;
      rfz_ib[1][a][0] = a; rfz_it[1][a][0] = a; 
      rfz_ib[1][a][1] = b; rfz_it[1][a][1] = b; 
      rfz_ib[1][a][2] = c; rfz_it[1][a][2] = c; 
      if     (z==5) {

	rfz_t [1][a]    = 9 ;
	rfz_it[1][a][3] = a+1; 
	rfz_it[1][a][4] = b+1; 
	rfz_it[1][a][5] = c+1; 
	rfz_it[1][a][6] = a-1; 
	rfz_it[1][a][7] = b-1; 
	rfz_it[1][a][8] = c-1; 
      }
      else if(z> 5) {

	rfz_b [1][a]    = 6 ;
	rfz_ib[1][a][3] = a-1; 
	rfz_ib[1][a][4] = b-1; 
	rfz_ib[1][a][5] = c-1; 

	if(z<10) {

	  rfz_t [1][a]    = 6 ;
	  rfz_it[1][a][3] = a+1; 
	  rfz_it[1][a][4] = b+1; 
	  rfz_it[1][a][5] = c+1; 
	}
      }
      else {

	rfz_b [1][a]    = 6 ;
	rfz_ib[1][a][3] = a+1; 
	rfz_ib[1][a][4] = b+1; 
	rfz_ib[1][a][5] = c+1; 

	if(z>0) {

	  rfz_t [1][a]    = 6 ;
	  rfz_it[1][a][3] = a-1; 
	  rfz_it[1][a][4] = b-1; 
	  rfz_it[1][a][5] = c-1; 
	}
      }
      
      if     (z==3) {
	rfz_b [1][a]    = 9;
	rfz_ib[1][a][6] = a+2; 
	rfz_ib[1][a][7] = b+2; 
	rfz_ib[1][a][8] = c+2; 
      }
      else if(z==7) {
	rfz_b [1][a]      = 9;
	rfz_ib[1][a][6] = a-2; 
	rfz_ib[1][a][7] = b-2; 
	rfz_ib[1][a][8] = c-2; 
      }
    }
  }

  // Build maps for radius-azimuthal-Z sorted collections for PPS seeds
  //
  m_iminPPS = int(m_radiusPPSmin/r_rstep);
  m_imaxPPS = int(m_radiusPPSmax/r_rstep);

  m_fNmax[2] = int(pi2/AzimuthalStep(m_ptmin,m_diverpps,m_radiusPPSmin,m_radiusPPSmax));
  if(m_fNmax[2] > NFmax) m_fNmax[2] = NFmax; 
  if(m_fNmax[2] < 10   ) m_fNmax[2] = 10   ;  
  m_sF[2] = float(m_fNmax[2]+1)/pi2;

  for(int f=0; f<=m_fNmax[2]; ++f) {

    int fb = f-1; if(fb<0         ) fb=m_fNmax[2]; 
    int ft = f+1; if(ft>m_fNmax[2]) ft=0; 
    
    // For each azimuthal region loop through all Z regions
    //
    for(int z=0; z!=11; ++z) {
 
      int a        = f *11+z;
      int b        = fb*11+z;
      int c        = ft*11+z;
      rfz_b [2][a]    = 3; rfz_t [2][a]    = 3;
      rfz_ib[2][a][0] = a; rfz_it[2][a][0] = a; 
      rfz_ib[2][a][1] = b; rfz_it[2][a][1] = b; 
      rfz_ib[2][a][2] = c; rfz_it[2][a][2] = c; 
      if     (z==5) {

	rfz_t [2][a]    = 9 ;
	rfz_it[2][a][3] = a+1; 
	rfz_it[2][a][4] = b+1; 
	rfz_it[2][a][5] = c+1; 
	rfz_it[2][a][6] = a-1; 
	rfz_it[2][a][7] = b-1; 
	rfz_it[2][a][8] = c-1; 
      }
      else if(z> 5) {

	rfz_b [2][a]    = 6 ;
	rfz_ib[2][a][3] = a-1; 
	rfz_ib[2][a][4] = b-1; 
	rfz_ib[2][a][5] = c-1; 

	if(z<10) {

	  rfz_t [2][a]    = 6 ;
	  rfz_it[2][a][3] = a+1; 
	  rfz_it[2][a][4] = b+1; 
	  rfz_it[2][a][5] = c+1; 
	}
      }
      else {

	rfz_b [2][a]    = 6 ;
	rfz_ib[2][a][3] = a+1; 
	rfz_ib[2][a][4] = b+1; 
	rfz_ib[2][a][5] = c+1; 

	if(z>0) {

	  rfz_t [2][a]    = 6 ;
	  rfz_it[2][a][3] = a-1; 
	  rfz_it[2][a][4] = b-1; 
	  rfz_it[2][a][5] = c-1; 
	}
      }
      
      if     (z==3) {
	rfz_b [2][a]    = 9;
	rfz_ib[2][a][6] = a+2; 
	rfz_ib[2][a][7] = b+2; 
	rfz_ib[2][a][8] = c+2; 
      }
      else if(z==7) {
	rfz_b [2][a]      = 9;
	rfz_ib[2][a][6] = a-2; 
	rfz_ib[2][a][7] = b-2; 
	rfz_ib[2][a][8] = c-2; 
      }
    }
  }

  // Build radius-azimuthal-Z sorted containers for Z-vertices
  //
  const int   NFtmax  = 100               ;
  const float sFvmax = float(NFtmax)/pi2;
  m_sFv       = m_ptmin/120.  ; if(m_sFv   >sFvmax)  m_sFv    = sFvmax; 
  m_fvNmax    = int(pi2*m_sFv); if(m_fvNmax>=NFtmax) m_fvNmax = NFtmax-1;
  m_nrfzv = 0; for(int i=0; i!=300; ++i) {rfzv_index[i]=0; rfzv_map[i]=0;}

  // Build maps for radius-azimuthal-Z sorted collections for Z
  //
  for(int f=0; f<=m_fvNmax; ++f) {

    int fb = f-1; if(fb<0       ) fb=m_fvNmax; 
    int ft = f+1; if(ft>m_fvNmax) ft=0; 
    
    // For each azimuthal region loop through central Z regions
    //
    for(int z=0; z!=3; ++z) {
      
      int a  = f *3+z; 
      int b  = fb*3+z;
      int c  = ft*3+z;
      rfzv_n[a]    = 3;
      rfzv_i[a][0] = a;
      rfzv_i[a][1] = b;
      rfzv_i[a][2] = c;
      if     (z>1) {
	rfzv_n[a]    = 6;
	rfzv_i[a][3] = a-1;
	rfzv_i[a][4] = b-1;
	rfzv_i[a][5] = c-1;
      }
      else if(z<1) {
	rfzv_n[a]    = 6;
	rfzv_i[a][3] = a+1;
	rfzv_i[a][4] = b+1;
	rfzv_i[a][5] = c+1;
      }
    }
  }
  
  if(!m_SP) m_SP   = new InDet::SiSpacePointForSeedITK*[m_maxsizeSP];
  if(!m_R ) m_R    = new                          float[m_maxsizeSP];
  if(!m_X ) m_X    = new                          float[m_maxsizeSP];
  if(!m_Y ) m_Y    = new                          float[m_maxsizeSP];
  if(!m_Tz) m_Tz   = new                          float[m_maxsizeSP];
  if(!m_Er) m_Er   = new                          float[m_maxsizeSP];
  if(!m_U ) m_U    = new                          float[m_maxsizeSP]; 
  if(!m_V ) m_V    = new                          float[m_maxsizeSP];
  if(!m_Zo) m_Zo   = new                          float[m_maxsizeSP];
  if(!m_L ) m_L    = new                          float[m_maxsizeSP];
  if(!m_Im) m_Im   = new                          float[m_maxsizeSP];
  if(!m_OneSeeds) m_OneSeeds  = new InDet::SiSpacePointsProSeedITK [m_maxOneSize];  

  if(!m_seedOutput) m_seedOutput = new InDet::SiSpacePointsSeed();

  i_seed  = l_seeds.begin();
  i_seede = l_seeds.end  ();
}

////////////////////////////////////////////////////////////////////
// Claculation azimuthal angle step as function of
// pTmin(MeV),Imax,Rbegin(R1),Rstop(R2)(mm)
///////////////////////////////////////////////////////////////////

float InDet::SiSpacePointsSeedMaker_ITK::AzimuthalStep(float pTmin,float Imax,float R1,float R2)
{
  float Rm = pTmin/.6;
  float sI = fabs(asin(Imax/R1)-asin(Imax/R2));
  float sF = fabs(asin(R2/(2.*Rm))-asin(R1/(2.*Rm)));
  return ((sI+sF)/3.);
}

//////////////////////////////////////////////////////////////////
// Initiate beam frame work for seed generator
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ITK::buildBeamFrameWork() 
{ 
  if(!p_beam) return;

  Amg::Vector3D cb =     p_beam->beamPos();
  double     tx = tan(p_beam->beamTilt(0));
  double     ty = tan(p_beam->beamTilt(1));

  double ph   = atan2(ty,tx);
  double th   = acos(1./sqrt(1.+tx*tx+ty*ty));
  double sint = sin(th);
  double cost = cos(th);
  double sinp = sin(ph);
  double cosp = cos(ph);
  
  m_xbeam[0] = float(cb.x())                  ; 
  m_xbeam[1] = float(cost*cosp*cosp+sinp*sinp);
  m_xbeam[2] = float(cost*sinp*cosp-sinp*cosp);
  m_xbeam[3] =-float(sint*cosp               );
  
  m_ybeam[0] = float(cb.y())                  ; 
  m_ybeam[1] = float(cost*cosp*sinp-sinp*cosp);
  m_ybeam[2] = float(cost*sinp*sinp+cosp*cosp);
  m_ybeam[3] =-float(sint*sinp               );
  
  m_zbeam[0] = float(cb.z())                  ; 
  m_zbeam[1] = float(sint*cosp)               ;
  m_zbeam[2] = float(sint*sinp)               ;
  m_zbeam[3] = float(cost)                    ;
}

///////////////////////////////////////////////////////////////////
// Initiate beam frame work for seed generator
///////////////////////////////////////////////////////////////////
void  InDet::SiSpacePointsSeedMaker_ITK::convertToBeamFrameWork
(Trk::SpacePoint*const& sp,float* r) 
{
  r[0] = float(sp->globalPosition().x())-m_xbeam[0];
  r[1] = float(sp->globalPosition().y())-m_ybeam[0];
  r[2] = float(sp->globalPosition().z())-m_zbeam[0];
}
   
///////////////////////////////////////////////////////////////////
// Initiate space points seed maker
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ITK::fillLists() 
{
  if      (m_iteration == 0) fillListsSSS();
  else if (m_iteration == 1) fillListsPPP();
  else if (m_iteration == 2) fillListsPPS();
  m_state = 0;
}

///////////////////////////////////////////////////////////////////
// Initiate SSS space points seed maker
///////////////////////////////////////////////////////////////////

 void InDet::SiSpacePointsSeedMaker_ITK::fillListsSSS() 
 {
  const float pi2 = 2.*M_PI;
  std::list<InDet::SiSpacePointForSeedITK*>::iterator r,re;

  int  ir0 = 0;
  int  irm = 0;
  for(int i=r_first; i!=r_size;  ++i) {

    if(!r_map[i]) continue; r = r_Sorted[i].begin(); re = r_Sorted[i].end();

    if(!ir0) ir0 = i; irm = i; 

    for(; r!=re; ++r) {
      
      // Azimuthal angle sort
      //
      float F = (*r)->phi(); if(F<0.) F+=pi2;

      int   f = int(F*m_sF[0]); f<0 ? f = m_fNmax[0] : f>m_fNmax[0] ? f = 0 : f=f;

      int z; float Z = (*r)->z();

      // Azimuthal angle and Z-coordinate sort
      //
      if(Z>0.) {
	Z< 250.?z=5:Z< 450.?z=6:Z< 925.?z=7:Z< 1400.?z=8:Z< 2500.?z=9:z=10;
      }
      else     {
	Z>-250.?z=5:Z>-450.?z=4:Z>-925.?z=3:Z>-1400.?z=2:Z>-2500.?z=1:z= 0;
      }
      int n = f*11+z; ++m_nsaz;
      rfz_Sorted[n].push_back(*r); if(!rfz_map[n]++) rfz_index[m_nrfz++] = n;
    }
  }
  m_RTmin = r_rstep*ir0+30. ;
  m_RTmax = r_rstep*irm-150.;
 }

///////////////////////////////////////////////////////////////////
// Initiate PPP space points seed maker
///////////////////////////////////////////////////////////////////

 void InDet::SiSpacePointsSeedMaker_ITK::fillListsPPP() 
 {
  const float pi2 = 2.*M_PI;
  std::list<InDet::SiSpacePointForSeedITK*>::iterator r,re;

  int  ir0 = 0;
  int  irm = 0;
  for(int i=r_first; i!=r_size;  ++i) {

    if(!r_map[i]) continue; r = r_Sorted[i].begin(); re = r_Sorted[i].end();
  
    if((*r)->spacepoint->clusterList().second) break;

    if(!ir0) ir0 = i; irm = i; 

    for(; r!=re; ++r) {
      
      // Azimuthal angle sort
      //
      float F = (*r)->phi(); if(F<0.) F+=pi2;

      int   f = int(F*m_sF[1]); f<0 ? f = m_fNmax[1] : f>m_fNmax[1] ? f = 0 : f=f;

      int z; float Z = (*r)->z();

      // Azimuthal angle and Z-coordinate sort
      //
      if(Z>0.) {
	Z< 250.?z=5:Z< 450.?z=6:Z< 925.?z=7:Z< 1400.?z=8:Z< 2500.?z=9:z=10;
      }
      else     {
	Z>-250.?z=5:Z>-450.?z=4:Z>-925.?z=3:Z>-1400.?z=2:Z>-2500.?z=1:z= 0;
      }
      int n = f*11+z; ++m_nsaz;
      rfz_Sorted[n].push_back(*r); if(!rfz_map[n]++) rfz_index[m_nrfz++] = n;
    }
  }
  m_RTmin = r_rstep*ir0+10. ;
  m_RTmax = r_rstep*irm-10.;
 }

///////////////////////////////////////////////////////////////////
// Initiate PPS space points seed maker
///////////////////////////////////////////////////////////////////

 void InDet::SiSpacePointsSeedMaker_ITK::fillListsPPS() 
 {
  const float pi2 = 2.*M_PI;

  int   ir0   = 0                ;
  int   irm   = 0                ;
  float Tpps  = 1./m_dzdrmaxPPS  ;

  for(int i=m_iminPPS; i!=m_imaxPPS;  ++i) {

    if(!r_map[i]) continue;
    std::list<InDet::SiSpacePointForSeedITK*>::iterator r = r_Sorted[i].begin();
    
    if(!ir0) ir0 = i; irm = i; 

    std::list<InDet::SiSpacePointForSeedITK*>::iterator re = r_Sorted[i].end();

    for(; r!=re; ++r) {
      
      // Azimuthal angle sort
      //
      float F = (*r)->phi(); if(F<0.) F+=pi2;

      int   f = int(F*m_sF[2]); f<0 ? f = m_fNmax[2] : f>m_fNmax[2] ? f = 0 : f=f;

      int z; float Z = (*r)->z(); 
      
      if((*r)->radius() < (fabs(Z)-m_zmaxU)*Tpps) continue;

      // Azimuthal angle and Z-coordinate sort
      //
      if(Z>0.) {
	Z< 250.?z=5:Z< 450.?z=6:Z< 925.?z=7:Z< 1400.?z=8:Z< 2500.?z=9:z=10;
      }
      else     {
	Z>-250.?z=5:Z>-450.?z=4:Z>-925.?z=3:Z>-1400.?z=2:Z>-2500.?z=1:z= 0;
      }
      int n = f*11+z; ++m_nsaz;
      rfz_Sorted[n].push_back(*r); if(!rfz_map[n]++) rfz_index[m_nrfz++] = n;
    }
  }
  m_RTmin = r_rstep*ir0+10.;
  m_RTmax = r_rstep*irm-10.;
}
///////////////////////////////////////////////////////////////////
// Erase space point information
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ITK::erase()
{
  for(int i=0; i!=m_nrfz;  ++i) {
    int n = rfz_index[i]; rfz_map[n] = 0;
    rfz_Sorted[n].clear();
  }
  
  for(int i=0; i!=m_nrfzv; ++i) {
    int n = rfzv_index[i]; rfzv_map[n] = 0;
    rfzv_Sorted[n].clear();
  }
  m_state = 0;
  m_nsaz  = 0;
  m_nsazv = 0;
  m_nrfz  = 0;
  m_nrfzv = 0;
}

///////////////////////////////////////////////////////////////////
// Test is space point used
///////////////////////////////////////////////////////////////////

bool InDet::SiSpacePointsSeedMaker_ITK::isUsed(const Trk::SpacePoint* sp)
{
  const Trk::PrepRawData* d = sp->clusterList().first ; 
  if(!d || !m_assoTool->isUsed(*d)) return false;

  d = sp->clusterList().second;
  if(!d || m_assoTool->isUsed(*d)) return true;

  return false;
}

///////////////////////////////////////////////////////////////////
// 2 space points seeds production
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ITK::production2Sp()
{
  if(m_nsazv<2) return;
  /*
  std::list<InDet::SiSpacePointForSeedITK*>::iterator r0,r0e,r,re;
  int nseed = 0; 

  // Loop thorugh all azimuthal regions
  //
  for(int f=m_fvNmin; f<=m_fvNmax; ++f) {

    // For each azimuthal region loop through Z regions
    //
    int z = 0; if(!m_endlist) z = m_zMin;
    for(; z!=3; ++z) {
      
      int a  = f*3+z;  if(!rfzv_map[a]) continue; 
      r0  = rfzv_Sorted[a].begin(); 
      r0e = rfzv_Sorted[a].end  (); 

      if(!m_endlist) {r0 = m_rMin; m_endlist = true;}

      // Loop through trigger space points
      //
      for(; r0!=r0e; ++r0) {

	float X  = (*r0)->x();
	float Y  = (*r0)->y();
	float R  = (*r0)->radius();
	if(R<m_r2minv) continue; if(R>m_r2maxv) break;
	float Z  = (*r0)->z();
	float ax = X/R;
	float ay = Y/R;

	// Bottom links production
	//
	int NB = rfzv_n[a];
	for(int i=0; i!=NB; ++i) {
	  
	  int an = rfzv_i[a][i];
	  if(!rfzv_map[an]) continue; 

	  r  =  rfzv_Sorted[an].begin();
	  re =  rfzv_Sorted[an].end  ();
	  
	  for(; r!=re; ++r) {
	    
	    float Rb =(*r)->radius();
	    if(Rb<m_r1minv) continue; if(Rb>m_r1maxv) break;
	    float dR = R-Rb; 
	    if(dR<m_drminv) break; if(dR>m_drmax) continue;
	    float dZ = Z-(*r)->z();
	    float Tz = dZ/dR; if(Tz<m_dzdrmin || Tz>m_dzdrmax) continue;
	    float Zo = Z-R*Tz;	          

	    // Comparison with vertices Z coordinates
	    //
	    if(!isZCompatible(Zo,Rb,Tz)) continue;

	    // Momentum cut
	    //
	    float dx =(*r)->x()-X; 
	    float dy =(*r)->y()-Y; 
	    float x  = dx*ax+dy*ay          ;
	    float y  =-dx*ay+dy*ax          ;
	    float xy = x*x+y*y              ; if(xy == 0.) continue;
	    float r2 = 1./xy                ;
	    float Ut = x*r2                 ;
	    float Vt = y*r2                 ;
	    float UR = Ut*R+1.              ; if(UR == 0.) continue;
	    float A  = Vt*R/UR              ;
	    float B  = Vt-A*Ut              ;
	    if(fabs(B*m_K) > m_ipt*sqrt(1.+A*A)) continue; ++nseed;
	    newSeed((*r),(*r0),Zo);
	  }
	}
	if(nseed < m_maxsize) continue; 
	m_endlist=false; m_rMin = (++r0); m_fvNmin=f; m_zMin=z; 
	return;
      }
    }
  }
  m_endlist = true;
  */
}

///////////////////////////////////////////////////////////////////
// Production 3 space points seeds 
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ITK::production3Sp()
{ 
  if(m_nsaz<3) return; 
  m_seeds.clear();

  if(m_iteration0 >= 0) {
    if     (m_iteration == 0                     ) production3SpSSS    ();
    else if(m_iteration == 1 && !m_useITKseedCuts) production3SpPPPold ();
    else if(m_iteration == 1 &&  m_useITKseedCuts) production3SpPPP    ();
    else if(m_iteration == 2                     ) production3SpPPS    ();
    else return;
  }
  else {
    
    if(m_useITKseedCuts) production3SpMIXED   ();
    else                 production3SpMIXEDold();
  }
}

///////////////////////////////////////////////////////////////////
// Production 3 space points seeds SSS 
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ITK::production3SpSSS()
{ 
  const int   ZI[11]= {5,6,4,7,3,8,2,9,1,10,0};

  std::vector<InDet::SiSpacePointForSeedITK*>::iterator rt[9],rte[9],rb[9],rbe[9];
  int nseed = 0; 

  // Loop thorugh all azimuthal regions
  //
  for(int f=m_fNmin; f<=m_fNmax[0]; ++f) {
    
    // For each azimuthal region loop through all Z regions
    //
    int z = 0; if(!m_endlist) z = m_zMin;

    for(; z!=11; ++z) {

      int a  = f *11+ZI[z];  if(!rfz_map[a]) continue;
      int NB = 0, NT = 0;
      for(int i=0; i!=rfz_b[0][a]; ++i) {
	
	int an =  rfz_ib[0][a][i];
	if(!rfz_map[an]) continue;
	rb [NB] = rfz_Sorted[an].begin(); rbe[NB++] = rfz_Sorted[an].end();
      } 
      for(int i=0; i!=rfz_t[0][a]; ++i) {
	
	int an =  rfz_it[0][a][i];
	if(!rfz_map[an]) continue; 
	rt [NT] = rfz_Sorted[an].begin(); rte[NT++] = rfz_Sorted[an].end();
      } 
      production3SpSSS    (rb,rbe,rt,rte,NB,NT,nseed);
      if(!m_endlist) {m_fNmin=f; m_zMin = z; return;} 
    }
  }
  m_endlist = true;
}

///////////////////////////////////////////////////////////////////
// Production 3 space points seeds PPPold
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ITK::production3SpPPPold()
{ 
  const int   ZI[11]= {0,1,2,3,10,9,8,7,5,4,6};

  std::vector<InDet::SiSpacePointForSeedITK*>::iterator rt[9],rte[9],rb[9],rbe[9];
  int nseed = 0; 

  // Loop thorugh all azimuthal regions
  //
  for(int f=m_fNmin; f<=m_fNmax[1]; ++f) {
    
    // For each azimuthal region loop through all Z regions
    //
    int z = 0; if(!m_endlist) z = m_zMin;

    for(; z!=11; ++z) {

      int a  = f *11+ZI[z];  if(!rfz_map[a]) continue;
      int NB = 0, NT = 0;
      for(int i=0; i!=rfz_b[1][a]; ++i) {
	
	int an =  rfz_ib[1][a][i];
	if(!rfz_map[an]) continue;
	rb [NB] = rfz_Sorted[an].begin(); rbe[NB++] = rfz_Sorted[an].end();
      } 
      for(int i=0; i!=rfz_t[1][a]; ++i) {
	
	int an =  rfz_it[1][a][i];
	if(!rfz_map[an]) continue; 
	rt [NT] = rfz_Sorted[an].begin(); rte[NT++] = rfz_Sorted[an].end();
      } 
      production3SpPPPold (rb,rbe,rt,rte,NB,NT,nseed);
      if(!m_endlist) {m_fNmin=f; m_zMin = z; return;} 
    }
  }
  m_endlist = true;
}

///////////////////////////////////////////////////////////////////
// Production 3 space points seeds PPP
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ITK::production3SpPPP()
{ 
  const int   ZI[11]= {5,6,7,8,9,10,4,3,2,1,0};
  std::vector<InDet::SiSpacePointForSeedITK*>::iterator rt[9],rte[9],rb[9],rbe[9];
  int nseed = 0; 

  // Loop thorugh all azimuthal regions
  //
  for(int f=m_fNmin; f<=m_fNmax[1]; ++f) {
    
    // For each azimuthal region loop through all Z regions
    //
    int z = 0; if(!m_endlist) z = m_zMin;

    for(; z!=11; ++z) {

      int a  = f *11+ZI[z];  if(!rfz_map[a]) continue;
      int NB = 0, NT = 0;
      for(int i=0; i!=rfz_b[1][a]; ++i) {
	
	int an =  rfz_ib[1][a][i];
	if(!rfz_map[an]) continue;
	rb [NB] = rfz_Sorted[an].begin(); rbe[NB++] = rfz_Sorted[an].end();
      } 
      for(int i=0; i!=rfz_t[1][a]; ++i) {
	
	int an =  rfz_it[1][a][i];
	if(!rfz_map[an]) continue; 
	rt [NT] = rfz_Sorted[an].begin(); rte[NT++] = rfz_Sorted[an].end();
      } 
      production3SpPPP(rb,rbe,rt,rte,NB,NT,nseed);
      if(!m_endlist) {m_fNmin=f; m_zMin = z; return;} 
    }
  }
  m_endlist = true;
}

///////////////////////////////////////////////////////////////////
// Production 3 space points seeds MIXED
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ITK::production3SpMIXED()
{ 
  const int   ZI[11]= {5,6,7,8,9,10,4,3,2,1,0};
  std::vector<InDet::SiSpacePointForSeedITK*>::iterator rt[9],rte[9],rb[9],rbe[9];
  int nseed = 0; 

  // Loop thorugh all azimuthal regions
  //
  for(int f=m_fNmin; f<=m_fNmax[0]; ++f) {
    
    // For each azimuthal region loop through all Z regions
    //
    int z = 0; if(!m_endlist) z = m_zMin;

    for(; z!=11; ++z) {

      int a  = f *11+ZI[z];  if(!rfz_map[a]) continue;
      int NB = 0, NT = 0;
      for(int i=0; i!=rfz_b[0][a]; ++i) {
	
	int an =  rfz_ib[0][a][i];
	if(!rfz_map[an]) continue;
	rb [NB] = rfz_Sorted[an].begin(); rbe[NB++] = rfz_Sorted[an].end();
      } 
      for(int i=0; i!=rfz_t[0][a]; ++i) {
	
	int an =  rfz_it[0][a][i];
	if(!rfz_map[an]) continue; 
	rt [NT] = rfz_Sorted[an].begin(); rte[NT++] = rfz_Sorted[an].end();
      } 
      production3SpMIXED    (rb,rbe,rt,rte,NB,NT,nseed);
      if(!m_endlist) {m_fNmin=f; m_zMin = z; return;} 
    }
  }
  m_endlist = true;
}

///////////////////////////////////////////////////////////////////
// Production 3 space points seeds MIXED
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ITK::production3SpMIXEDold()
{ 
  const int   ZI[11]= {5,6,7,8,9,10,4,3,2,1,0};
  std::vector<InDet::SiSpacePointForSeedITK*>::iterator rt[9],rte[9],rb[9],rbe[9];
  int nseed = 0; 

  // Loop thorugh all azimuthal regions
  //
  for(int f=m_fNmin; f<=m_fNmax[0]; ++f) {
    
    // For each azimuthal region loop through all Z regions
    //
    int z = 0; if(!m_endlist) z = m_zMin;

    for(; z!=11; ++z) {

      int a  = f *11+ZI[z];  if(!rfz_map[a]) continue;
      int NB = 0, NT = 0;
      for(int i=0; i!=rfz_b[0][a]; ++i) {
	
	int an =  rfz_ib[0][a][i];
	if(!rfz_map[an]) continue;
	rb [NB] = rfz_Sorted[an].begin(); rbe[NB++] = rfz_Sorted[an].end();
      } 
      for(int i=0; i!=rfz_t[0][a]; ++i) {
	
	int an =  rfz_it[0][a][i];
	if(!rfz_map[an]) continue; 
	rt [NT] = rfz_Sorted[an].begin(); rte[NT++] = rfz_Sorted[an].end();
      } 
      production3SpMIXEDold(rb,rbe,rt,rte,NB,NT,nseed);
      if(!m_endlist) {m_fNmin=f; m_zMin = z; return;} 
    }
  }
  m_endlist = true;
}

///////////////////////////////////////////////////////////////////
// Production 3 space points seeds PSS
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ITK::production3SpPPS()
{ 
  const int   ZI[11]= {5,6,7,8,9,10,4,3,2,1,0};
  std::vector<InDet::SiSpacePointForSeedITK*>::iterator rt[9],rte[9],rb[9],rbe[9];
  int nseed = 0; 

  // Loop thorugh all azimuthal regions
  //
  for(int f=m_fNmin; f<=m_fNmax[2]; ++f) {
    
    // For each azimuthal region loop through all Z regions
    //
    int z = 0; if(!m_endlist) z = m_zMin;

    for(; z!=11; ++z) {

      int a  = f *11+ZI[z];  if(!rfz_map[a]) continue;
      int NB = 0, NT = 0;
      for(int i=0; i!=rfz_b[2][a]; ++i) {
	
	int an =  rfz_ib[2][a][i];
	if(!rfz_map[an]) continue;
	rb [NB] = rfz_Sorted[an].begin(); rbe[NB++] = rfz_Sorted[an].end();
      } 
      for(int i=0; i!=rfz_t[2][a]; ++i) {
	
	int an =  rfz_it[2][a][i];
	if(!rfz_map[an]) continue; 
	rt [NT] = rfz_Sorted[an].begin(); rte[NT++] = rfz_Sorted[an].end();
      } 
      production3SpPPS(rb,rbe,rt,rte,NB,NT,nseed);
      if(!m_endlist) {m_fNmin=f; m_zMin = z; return;} 
    }
  }
  m_endlist = true;
}

///////////////////////////////////////////////////////////////////
// Production 3 pixel space points seeds for full scan
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ITK::production3SpPPP
( std::vector<InDet::SiSpacePointForSeedITK*>::iterator* rb ,
  std::vector<InDet::SiSpacePointForSeedITK*>::iterator* rbe,
  std::vector<InDet::SiSpacePointForSeedITK*>::iterator* rt ,
  std::vector<InDet::SiSpacePointForSeedITK*>::iterator* rte,
  int NB, int NT, int& nseed) 
{
  std::vector<InDet::SiSpacePointForSeedITK*>::iterator r0=rb[0],r;
  if(!m_endlist) {r0 = m_rMin; m_endlist = true;}

  float ipt2K = m_ipt2K   ;
  float ipt2C = m_ipt2C   ;
  float COFK  = m_COFK    ; 
  float imaxp = m_diver   ;

  for(; r0!=rbe[0]; ++r0) {if((*r0)->radius() > m_RTmin) break;}
  m_CmSp.clear();

  // Loop through all trigger space points
  //
  for(; r0!=rbe[0]; ++r0) {

    m_nOneSeeds = 0;
    m_mapOneSeeds.clear();

    const Trk::Surface* sur0 = (*r0)->sur();

    float R     =(*r0)->radius(); if(R > m_RTmax) break;
    float Ri    = 1./R           ;
    float X     = (*r0)->x()     ;
    float Y     = (*r0)->y()     ;
    float Z     = (*r0)->z()     ;
    float ax    = X*Ri           ;
    float ay    = Y*Ri           ;
    float U0    =  -Ri           ;
    float VR    = imaxp*(Ri*Ri)  ;
    float covr0 = (*r0)->covr () ;
    float covz0 = (*r0)->covz () ;
    int   Nb    = 0              ;
    bool  BA    = (*r0)->barrel();
    int   LA    = (*r0)->layer ();

    // Bottom links production
    //
    for(int i=0; i!=NB; ++i) {

      for(r=rb[i]; r!=rbe[i]; ++r) {
	
	float Rb =(*r)->radius();  
	float dR = R-Rb; 

	if(dR > m_drmax) {rb[i]=r; continue;}   
	if(dR < m_drmin) break;
	if((*r)->sur()==sur0) continue;

	// Comparison with vertices Z coordinates
	//
	float dz = Z-(*r)->z();
	if(fabs(Z*dR-R*dz) > m_zmaxU*dR) continue;
	float Tz = dz/dR;
	float Zo = Z-R*Tz; 

	float dx = (*r)->x()-X ;
	float dy = (*r)->y()-Y ;
	float x  = dx*ax+dy*ay ; if(x >= 0.) continue;
	float y  = dy*ax-dx*ay ;
	float r2 = 1./(x*x+y*y);
	float u  = x*r2        ;
	float v  = y*r2        ;

	if(fabs(R*y) > -imaxp*x) {

	  float V0  = VR;   if(y < 0. )  V0=-V0  ; 
	  float dU  = u-U0; if(dU == 0.) continue; 
	  float A   = (v-V0)/dU                  ;
	  float B   = V0-A*U0                    ;
	  if((B*B) > (ipt2K*(1.+A*A))) continue  ;
	}

	if(m_useITKseedCuts_hit     && !BA && LA==0 && (*r)->barrel() && (*r)->layer() > 1) continue;
	if(m_useITKseedCuts_dR      &&  BA && (*r)->barrel() && LA-(*r)->layer()       < 1) continue;
	if(m_useITKseedCuts_PixHole &&  BA && (*r)->barrel() && LA-(*r)->layer()       > 1) continue;
	if(ClusterSizeCuts((*r),(*r0),Zo)!=true) continue;

	float dr  = sqrt(r2)                                               ;
	float tz  = dz*dr                                                  ; if(fabs(tz) < m_dzdrmin || fabs(tz) > m_dzdrmax) continue;
	m_Tz[Nb]   = tz                                                    ;
	m_Zo[Nb]   = Z-R*tz                                                ;
	m_R [Nb]   = dr                                                    ;
	m_U [Nb]   = u                                                     ;
	m_V [Nb]   = v                                                     ;
	m_Er[Nb]   = ((covz0+(*r)->covz())+(tz*tz)*(covr0+(*r)->covr()))*r2;
	m_SP[Nb]  = (*r); if(++Nb==m_maxsizeSP) goto breakb;
      }
    }
  breakb:
    if(!Nb || Nb==m_maxsizeSP) continue; 

    int Nt = Nb;
    
    // Top   links production
    //
    for(int i=0; i!=NT; ++i) {
      
      for(r=rt[i]; r!=rte[i]; ++r) {
	
	float Rt =(*r)->radius();
	float dR = Rt-R; 
	
	if(dR<m_drmin) {rt[i]=r; continue;}
	if(dR>m_drmax) break;
	if((*r)->sur()==sur0) continue;

	// Comparison with vertices Z coordinates
	//
	float dz = (*r)->z()-Z;
	if(fabs(Z*dR-R*dz) > m_zmaxU*dR) continue;
	float Tz = dz/dR;
	float Zo = Z-R*Tz; 

	float dx = (*r)->x()-X ;
	float dy = (*r)->y()-Y ;
	float x  = dx*ax+dy*ay ; if(x<=0.) continue;
	float y  = dy*ax-dx*ay ;
	float r2 = 1./(x*x+y*y);
	float u  = x*r2        ;
	float v  = y*r2        ;

	if(fabs(R*y) > imaxp*x) {

	  float V0  = VR;   if(y > 0. )  V0=-V0  ; 
	  float dU  = u-U0; if(dU == 0.) continue; 
	  float A   = (v-V0)/dU                  ;
	  float B   = V0-A*U0                    ;
	  if((B*B) > (ipt2K*(1.+A*A))) continue  ;
	}

	if(m_useITKseedCuts_dR      && BA && (*r)->barrel() && (*r)->layer()-LA < 1) continue;
	if(m_useITKseedCuts_PixHole && BA == (*r)->barrel() && (*r)->layer()-LA > 1) continue;
	if(ClusterSizeCuts((*r0),(*r),Zo)!=true) continue;
	
	float dr  = sqrt(r2)                                               ;
	float tz  = dz*dr                                                  ; if(fabs(tz) < m_dzdrmin || fabs(tz) > m_dzdrmax) continue; 
	m_Tz[Nt]   = tz                                                    ;
	m_Zo[Nt]   = Z-R*tz                                                ;
	m_R [Nt]   = dr                                                    ;
	m_U [Nt]   = u                                                     ;
	m_V [Nt]   = v                                                     ;
	m_Er[Nt]   = ((covz0+(*r)->covz())+(tz*tz)*(covr0+(*r)->covr()))*r2;
  	m_SP[Nt] = (*r); if(++Nt==m_maxsizeSP) goto breakt;
      }
    }
    
  breakt:
    if(!(Nt-Nb)) continue;
    
    covr0      *= .5;
    covz0      *= 2.;
   
    // Three space points comparison
    //
    for(int b=0; b!=Nb; ++b) {
    
      float  Zob  = m_Zo[b]      ;
      float  Tzb  = m_Tz[b]      ;
      float  Rb2r = m_R [b]*covr0;
      float  Rb2z = m_R [b]*covz0;
      float  Erb  = m_Er[b]      ;
      float  Vb   = m_V [b]      ;
      float  Ub   = m_U [b]      ;
      float  Tzb2 = (1.+Tzb*Tzb) ;
      float sTzb2 = sqrt(Tzb2)   ;
      float  CSA  = Tzb2*COFK    ;
      float ICSA  = Tzb2*ipt2C   ;

      for(int t=Nb;  t!=Nt; ++t) {
	
	float dT  = ((Tzb-m_Tz[t])*(Tzb-m_Tz[t])-m_R[t]*Rb2z-(Erb+m_Er[t]))-(m_R[t]*Rb2r)*((Tzb+m_Tz[t])*(Tzb+m_Tz[t]));
	if( dT > ICSA) continue;

	float dU  = m_U[t]-Ub; if(dU == 0.) continue ; 
	float A   = (m_V[t]-Vb)/dU                   ;
	float S2  = 1.+A*A                           ;
	float B   = Vb-A*Ub                          ;
	float B2  = B*B                              ;
	if(B2  > ipt2K*S2 || dT*S2 > B2*CSA) continue;


	// >>>>>>>> New checks of compatibility of a cluster size for bottom cluster with a position 
        // >>>>>>>> of top cluster
	
	if(m_useITKseedCuts_dSize || m_useITKseedCuts_sizeDiff)
	  {
	    double dR= m_SP[t]->radius() - m_SP[b]->radius();
	    double dz= m_SP[t]->z() - m_SP[b]->z();
	    double Tz= fabs(dz)>0.0 ? dR/dz : 1000000.0;
	    double Zo= m_SP[b]->z() - m_SP[b]->radius()/Tz;
		
	    if(ClusterSizeCuts(m_SP[b],m_SP[t],Zo)!=true) continue;
	  }
	
	//------------------ end of cluster size checks

	float Im  = fabs((A-B*R)*R)                  ; 

	if(Im <= imaxp) {
	  float dr; m_R[t] < m_R[b] ? dr = m_R[t] : dr = m_R[b]; Im+=fabs((Tzb-m_Tz[t])/(dr*sTzb2));
	  m_CmSp.push_back(std::make_pair(B/sqrt(S2),m_SP[t])); m_SP[t]->setParam(Im);
	}
      }

      if(!m_CmSp.empty()) newOneSeedWithCurvaturesComparison(m_SP[b],(*r0),Zob);
    }
    fillSeeds();  nseed += m_fillOneSeeds;
    if(nseed>=m_maxsize) {
      m_endlist=false; ++r0; m_rMin = r0;  return;
    } 
  }
}

///////////////////////////////////////////////////////////////////
// Production 3 pixel space points seeds for full scan
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ITK::production3SpPPPold
( std::vector<InDet::SiSpacePointForSeedITK*>::iterator* rb ,
  std::vector<InDet::SiSpacePointForSeedITK*>::iterator* rbe,
  std::vector<InDet::SiSpacePointForSeedITK*>::iterator* rt ,
  std::vector<InDet::SiSpacePointForSeedITK*>::iterator* rte,
  int NB, int NT, int& nseed) 
{
  std::vector<InDet::SiSpacePointForSeedITK*>::iterator r0=rb[0],r;
  if(!m_endlist) {r0 = m_rMin; m_endlist = true;}

  float ipt2K = m_ipt2K   ;
  float ipt2C = m_ipt2C   ;
  float COFK  = m_COFK    ; 
  float imaxp = m_diver   ;


  m_CmSpn.clear();

  for(; r0!=rbe[0]; ++r0) {if((*r0)->radius() > m_RTmin) break;}

  // Loop through all trigger space points
  //
  for(; r0!=rbe[0]; ++r0) {

    m_nOneSeeds = 0;
    m_mapOneSeeds.clear();

    const Trk::Surface* sur0 = (*r0)->sur();

    float R     =(*r0)->radius(); if(R       > m_RTmax) break;
    float Z    = (*r0)->     z(); if(fabs(Z) > 2700.  ) continue;
    int   Nb    = 0             ;

    m_Bot.clear(); 
    m_Top.clear();


    // Bottom links production
    //
    for(int i=0; i!=NB; ++i) {

      for(r=rb[i]; r!=rbe[i]; ++r) {
	
	float Rb =(*r)->radius();  
	float dR = R-Rb; 

	if(dR > m_drmax) {rb[i]=r; continue;}   
	if(dR < m_drmin) break;
	if((*r)->sur()==sur0) continue;

	// Comparison with vertices Z coordinates
	//
 	if(fabs(Z*dR-R*(Z-(*r)->z()))> m_zmaxU*dR) continue;
	m_SP[Nb]  = (*r); if(++Nb==m_maxsizeSP) goto breakb;
      }
    }
  breakb:
    if(!Nb || Nb==m_maxsizeSP) continue;
 
    int Nt = Nb;
    
    // Top   links production
    //
    for(int i=0; i!=NT; ++i) {
      
      for(r=rt[i]; r!=rte[i]; ++r) {
	
	float Rt =(*r)->radius();
	float dR = Rt-R; 
	
	if(dR<m_drmin) {rt[i]=r; continue;}
	if(dR>m_drmax) break;
	if((*r)->sur()==sur0) continue;

	// Comparison with vertices Z coordinates
	//
 	if(fabs(Z*dR-R*((*r)->z()-Z))> m_zmaxU*dR) continue;
  	m_SP[Nt] = (*r); if(++Nt==m_maxsizeSP) goto breakt;
      }
    }
    
  breakt:
    if(!(Nt-Nb)) continue;

    float X     = (*r0)->    x();
    float Y     = (*r0)->    y();
    float covr0 = (*r0)->covr ();
    float covz0 = (*r0)->covz ();
    float Ri    = 1./R          ;
    float ax    = X*Ri          ;
    float ay    = Y*Ri          ;
    float Ri2   = Ri*Ri         ;
    float VR    = imaxp*Ri2       ;

    // Top space points information production
    //
    int n = Nb;
    for(int i=Nb; i!=Nt; ++i) {

      InDet::SiSpacePointForSeedITK* sp = m_SP[i];  

      float dx  = sp->x()-X   ;
      float dy  = sp->y()-Y   ;
      float dz  = sp->z()-Z   ;
      float x   = dx*ax+dy*ay ;
      float y   = dy*ax-dx*ay ;
      float dxy = x*x+y*y     ;
      float r2  = 1./dxy      ;
      float u   = x*r2        ;
      float v   = y*r2        ;
      
      if(fabs(R*y) > imaxp*x) {

	float V0  = VR; if(y > 0.) V0 =-VR   ;
	float A   = (v-V0)/(u+Ri)            ;
	float B   = V0+A*Ri                  ;
	if((B*B) > (ipt2K*(1.+A*A))) continue;
      }
      float dr  = sqrt(r2)    ;
      float tz  = dz*dr       ; if(fabs(tz) < m_dzdrmin || fabs(tz) > m_dzdrmax) continue;
      m_SP[n  ] = sp          ;
      m_Zo[n  ] = Z-R*tz      ;
      m_R [n  ] = dr          ;
      m_U [n  ] = u           ;
      m_V [n  ] = v           ;
      m_Er[n  ] = ((covz0+sp->covz())+(tz*tz)*(covr0+sp->covr()))*r2;
      m_L [n  ] = sqrt(dxy+dz*dz); 
     m_Top.push_back(std::make_pair(tz,n)); ++n;
    }
    if(n < Nb+1) continue; Nt = n;

    
    // Bottom space points information production
    //
    n=0;
    for(int i=0; i!=Nb; ++i) {

      InDet::SiSpacePointForSeedITK* sp = m_SP[i];  

      float dx  = sp->x()-X   ; 
      float dy  = sp->y()-Y   ;
      float dz  = Z-sp->z()   ;
      float x   = dx*ax+dy*ay ;
      float y   = dy*ax-dx*ay ;
      float r2  = 1./(x*x+y*y);
      float u   = x*r2        ;
      float v   = y*r2        ;

      if(fabs(R*y) > -imaxp*x) {

	float V0  = VR; if(y < 0.) V0 =-V0;
	float A   = (v-V0)/(u+Ri)              ;
	float B   = V0+A*Ri                    ;
	if((B*B) > (ipt2K*(1.+A*A))) continue  ;
      }
      float dr  = sqrt(r2)    ;
      float tz  = dz*dr       ; if(fabs(tz) < m_dzdrmin || fabs(tz) > m_dzdrmax) continue;
      m_SP[n  ] = sp          ;
      m_Zo[n  ] = Z-R*tz      ;
      m_R [n  ] = dr          ;
      m_U [n  ] = u           ;
      m_V [n  ] = v           ;
      m_Er[n  ] = ((covz0+sp->covz())+(tz*tz)*(covr0+sp->covr()))*r2;
      m_Bot.push_back(std::make_pair(tz,n)); ++n;
    }
    if(!n) continue; Nt-=Nb; Nb = n; 

    std::sort(m_Bot.begin(),m_Bot.end(),comDzDrITK());
    std::sort(m_Top.begin(),m_Top.end(),comDzDrITK());

    covr0      *= .5;
    covz0      *= 2.;
    
    // Three space points comparison
    //
    int it0 = 0;
    for(int ib = 0; ib!=Nb; ++ib) {

      if(it0==Nt) break;

      int    b    = m_Bot[ib].second; 

      unsigned int Nc; m_SP[b]->radius() > 140. ? Nc = 0 : Nc = 1;
      
      float  Tzb  = m_Bot[ib].first ;
      float  Zob  = m_Zo[b]      ;
      float  Rb2r = m_R [b]*covr0;
      float  Rb2z = m_R [b]*covz0;
      float  Erb  = m_Er[b]      ;
      float  Vb   = m_V [b]      ;
      float  Ub   = m_U [b]      ;
      float  Tzb2 = (1.+Tzb*Tzb) ;
      float  CSA  = Tzb2*COFK    ;
      float ICSA  = Tzb2*ipt2C   ;

      for(int it = it0;  it!=Nt; ++it) {

	int  t    = m_Top[it].second;

	float DT  = Tzb-m_Top[it].first;
	float ST  = Tzb+m_Top[it].first;
	float dT  = (DT*DT-m_R[t]*Rb2z-(Erb+m_Er[t]))-(m_R[t]*Rb2r)*(ST*ST);
	if( dT > ICSA) {if(DT < 0.) break; it0 = it+1; continue;}

	float dU  = m_U[t]-Ub; if(dU == 0.) continue ; 
	float A   = (m_V[t]-Vb)/dU                   ;
	float S2  = 1.+A*A                           ;
	float B   = Vb-A*Ub                          ;
	float B2  = B*B                              ;
	if(B2  > ipt2K*S2) continue;
	if(dT*S2 > B2*CSA) {if(DT < 0.) break; it0 = it; continue;}

	float Im  = fabs((A-B*R)*R)                  ; 

	if(Im <= imaxp) {
	  m_Im[t] = Im; m_CmSpn.push_back(std::make_pair(B/sqrt(S2),t));
	}
      }
      if(m_CmSpn.size() > Nc) newOneSeedWithCurvaturesComparisonPPP(m_SP[b],(*r0),Zob); m_CmSpn.clear();
    }
    fillSeeds();  nseed += m_fillOneSeeds;
    if(nseed>=m_maxsize) {
      m_endlist=false; ++r0; m_rMin = r0;  return;
    } 
  }
}

///////////////////////////////////////////////////////////////////
// Production PPP+PPS+PSS space points seeds for full scan
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ITK::production3SpMIXED
( std::vector<InDet::SiSpacePointForSeedITK*>::iterator* rb ,
  std::vector<InDet::SiSpacePointForSeedITK*>::iterator* rbe,
  std::vector<InDet::SiSpacePointForSeedITK*>::iterator* rt ,
  std::vector<InDet::SiSpacePointForSeedITK*>::iterator* rte,
  int NB, int NT, int& nseed) 
{
  std::vector<InDet::SiSpacePointForSeedITK*>::iterator r0=rb[0],r;
  if(!m_endlist) {r0 = m_rMin; m_endlist = true;}
  float ipt2K = m_ipt2K   ;
  float ipt2C = m_ipt2C   ;
  float COFK  = m_COFK    ; 
  float imaxp = m_diver   ;
  float imaxs = m_divermax;

  m_CmSp.clear();

  // Loop through all trigger space points
  //
  for(; r0!=rbe[0]; ++r0) {

    m_nOneSeeds = 0;
    m_mapOneSeeds.clear();

    bool  pix   = !(*r0)->spacepoint->clusterList().second;
    float R     = (*r0)->radius();
    float Ri    = 1./R           ;
    float X     = (*r0)->x()     ;
    float Y     = (*r0)->y()     ;
    float Z     = (*r0)->z()     ;
    float ax    = X*Ri           ;
    float ay    = Y*Ri           ;
    float U0    =  -Ri           ;
    float VR    = imaxp*(Ri*Ri)  ;
    float covr0 = (*r0)->covr () ;
    float covz0 = (*r0)->covz () ;
    int   Nb    = 0              ;
    bool  BA    = (*r0)->barrel();
    int   LA    = (*r0)->layer ();

    // Bottom links production
    //
    for(int i=0; i!=NB; ++i) {

      for(r=rb[i]; r!=rbe[i]; ++r) {
	
	float Rb =(*r)->radius();  
	float dR = R-Rb; 

	if(dR > m_drmax) {rb[i]=r; continue;}   
	if(dR < m_drmin) break;

	float dz = (*r)->z()-Z ;

	// Comparison with vertices Z coordinates
	//
	float Tz = -dz/dR, aTz = fabs(Tz);
	if(aTz < m_dzdrmin      ||     aTz > m_dzdrmax) continue;
	float Zo = Z-R*Tz; if(!isZCompatible(Zo,Rb,Tz)) continue;

	float dx = (*r)->x()-X ;
	float dy = (*r)->y()-Y ;
	float x  = dx*ax+dy*ay ; if(x >= 0.) continue;
	float y  = dy*ax-dx*ay ;
	float r2 = 1./(x*x+y*y);
	float u  = x*r2        ;
	float v  = y*r2        ;

	if(pix && fabs(R*y) > -imaxp*x) {

	  float V0  = VR;   if(y < 0. )  V0=-V0  ; 
	  float dU  = u-U0; if(dU == 0.) continue; 
	  float A   = (v-V0)/dU                  ;
	  float B   = V0-A*U0                    ;
	  if((B*B) > (ipt2K*(1.+A*A))) continue  ;
	}

	if(m_useITKseedCuts_hit     && !BA && LA==0 && (*r)->barrel() && (*r)->layer() > 1) continue;
	if(m_useITKseedCuts_dR      &&  BA && (*r)->barrel() && LA-(*r)->layer()       < 1) continue;
	if(m_useITKseedCuts_PixHole &&  BA && (*r)->barrel() && LA-(*r)->layer()       > 1) continue;
	if(ClusterSizeCuts((*r),(*r0),Zo)!=true) continue;

	float dr  = sqrt(r2)                                              ;
	float tz  =-dz*dr                                                 ; 
	m_X [Nb]  = x                                                     ;
	m_Y [Nb]  = y                                                     ;
	m_Tz[Nb]  = tz                                                    ;
	m_Zo[Nb]  = Z-R*tz                                                ;
	m_R [Nb]  = dr                                                    ;
	m_U [Nb]  = u                                                     ;
	m_V [Nb]  = v                                                     ;
	m_Er[Nb]  = ((covz0+(*r)->covz())+(tz*tz)*(covr0+(*r)->covr()))*r2;
	m_SP[Nb]  = (*r); if(++Nb==m_maxsizeSP) goto breakb;
      }
    }
  breakb:
    if(!Nb || Nb==m_maxsizeSP) continue; 

    int Nt = Nb;
    
    // Top   links production
    //
    for(int i=0; i!=NT; ++i) {
      
      for(r=rt[i]; r!=rte[i]; ++r) {
	
	float Rt =(*r)->radius();
	float dR = Rt-R; 
	
	if(dR<m_drmin) {rt[i]=r; continue;}
	if(dR>m_drmax) break;

	float dz = (*r)->z()-Z ;

	// Comparison with vertices Z coordinates
	//
	float Tz = dz/dR, aTz =fabs(Tz);  
	if(aTz < m_dzdrmin || aTz > m_dzdrmax) continue;
	float Zo = Z-R*Tz; if(!isZCompatible(Zo,R ,Tz)) continue;

	float dx = (*r)->x()-X ;
	float dy = (*r)->y()-Y ;
	float x  = dx*ax+dy*ay ; if(x<=0.) continue;
	float y  = dy*ax-dx*ay ;
	float r2 = 1./(x*x+y*y);
	float u  = x*r2        ;
	float v  = y*r2        ;

	if(pix && !(*r)->spacepoint->clusterList().second && fabs(R*y) > imaxp*x) {

	  float V0  = VR;   if(y > 0. )  V0=-V0  ; 
	  float dU  = u-U0; if(dU == 0.) continue; 
	  float A   = (v-V0)/dU                  ;
	  float B   = V0-A*U0                    ;
	  if((B*B) > (ipt2K*(1.+A*A))) continue  ;
	}

 	if( !(*r)->spacepoint->clusterList().second)
	  {
	    if(m_useITKseedCuts_dR      && BA && (*r)->barrel() && (*r)->layer()-LA < 1) continue;
	    if(m_useITKseedCuts_PixHole && BA == (*r)->barrel() && (*r)->layer()-LA > 1) continue;
	    if(ClusterSizeCuts((*r0),(*r),Zo)!=true) continue;
	  }
      
	float dr  = sqrt(r2)                                              ;
	float tz  = dz*dr                                                 ; 
	m_X [Nt]  = x                                                     ;
	m_Y [Nt]  = y                                                     ;
	m_Tz[Nt]  = tz                                                    ;
	m_Zo[Nt]  = Z-R*tz                                                ;
	m_R [Nt]  = dr                                                    ;
	m_U [Nt]  = u                                                     ;
	m_V [Nt]  = v                                                     ;
	m_Er[Nt]  = ((covz0+(*r)->covz())+(tz*tz)*(covr0+(*r)->covr()))*r2;
  	m_SP[Nt]  = (*r); if(++Nt==m_maxsizeSP) goto breakt;
      }
    }
    
  breakt:
    if(!(Nt-Nb)) continue;
    covr0      *= .5;
    covz0      *= 2.;
   
    // Three space points comparison
    //
    for(int b=0; b!=Nb; ++b) {
    
      bool  pixb  =!m_SP[b]->spacepoint->clusterList().second;
      float  Zob  = m_Zo[b]      ;
      float  Tzb  = m_Tz[b]      ;
      float  Rb2r = m_R [b]*covr0;
      float  Rb2z = m_R [b]*covz0;
      float  Erb  = m_Er[b]      ;
      float  Vb   = m_V [b]      ;
      float  Ub   = m_U [b]      ;
      float  Tzb2 = (1.+Tzb*Tzb) ;
      float sTzb2 = sqrt(Tzb2)   ;
      float  CSA  = Tzb2*COFK    ;
      float ICSA  = Tzb2*ipt2C   ;
      float imax  = imaxs        ; if(pixb) imax = imaxp;

      float Se    = 1./sTzb2     ;
      float Ce    = Se*Tzb       ;
      float Sx    = Se*ax        ;
      float Sy    = Se*ay        ;

      for(int t=Nb;  t!=Nt; ++t) {
       
	if(pix && !m_SP[t]->spacepoint->clusterList().second) {
	  
	  float dT  = ((Tzb-m_Tz[t])*(Tzb-m_Tz[t])-m_R[t]*Rb2z-(Erb+m_Er[t]))-(m_R[t]*Rb2r)*((Tzb+m_Tz[t])*(Tzb+m_Tz[t]));
	  if( dT > ICSA) continue;

	  float dU  = m_U[t]-Ub; if(dU == 0.) continue ; 
	  float A   = (m_V[t]-Vb)/dU                   ;
	  float S2  = 1.+A*A                           ;
	  float B   = Vb-A*Ub                          ;
	  float B2  = B*B                              ;
	  if(B2  > ipt2K*S2 || dT*S2 > B2*CSA) continue;

	  if(m_useITKseedCuts_dSize || m_useITKseedCuts_sizeDiff)
	    {
	      double dR= m_SP[t]->radius() - m_SP[b]->radius();
	      double dz= m_SP[t]->z() - m_SP[b]->z();
	      double Tz= fabs(dz)>0.0 ? dR/dz : 1000000.0;
	      double Zo= m_SP[b]->z() - m_SP[b]->radius()/Tz;
	      
	      if(ClusterSizeCuts(m_SP[b],m_SP[t],Zo)!=true) continue;
	    }

	  float Im  = fabs((A-B*R)*R)                  ; 
	  
	  if(Im <= imax) {
	    float dr; m_R[t] < m_R[b] ? dr = m_R[t] : dr = m_R[b]; Im+=fabs((Tzb-m_Tz[t])/(dr*sTzb2));
	    m_CmSp.push_back(std::make_pair(B/sqrt(S2),m_SP[t])); m_SP[t]->setParam(Im);
	  }
	}
	else   {

	  // Trigger point
	  //	
	  float dU0   =  m_U[t]-Ub       ;  if(dU0 == 0.) continue; 
	  float A0    = (m_V[t]-Vb)/dU0  ;
	  float C0    = 1./sqrt(1.+A0*A0); 
	  float rn[3];
	  if(pix) {
	    rn[0] = X; rn[1] = Y; rn[2] = Z;
	  }
	  else  {
	    float S0    = A0*C0            ;
	    float d0[3] = {Sx*C0-Sy*S0,Sx*S0+Sy*C0,Ce};  
	    if(!(*r0)->coordinates(d0,rn)) continue;
	  }
	  
	  // Bottom  point
	  //
	  float B0    = 2.*(Vb-A0*Ub);
	  float rb[3];
	  if(pixb) {
	    rb[0] = m_SP[b]->x(); rb[1] = m_SP[b]->y(); rb[2] = m_SP[b]->z();
	  }
	  else {
	    float Cb    = (1.-B0*m_Y[b])*C0;
	    float Sb    = (A0+B0*m_X[b])*C0;
	    float db[3] = {Sx*Cb-Sy*Sb,Sx*Sb+Sy*Cb,Ce};  
	    if(!m_SP[b]->coordinates(db,rb)) continue;
	  }
	  
	  // Top     point
	  //
	  float Ct    = (1.-B0*m_Y[t])*C0;
	  float St    = (A0+B0*m_X[t])*C0;
	  float dt[3] = {Sx*Ct-Sy*St,Sx*St+Sy*Ct,Ce};  
	  float rt[3];  if(!m_SP[t]->coordinates(dt,rt)) continue;

	  float xb    = rb[0]-rn[0];
	  float yb    = rb[1]-rn[1];
	  float xt    = rt[0]-rn[0];
	  float yt    = rt[1]-rn[1];

	  float rb2   = 1./(xb*xb+yb*yb);
	  float rt2   = 1./(xt*xt+yt*yt);
	
	  float tb    =  (rn[2]-rb[2])*sqrt(rb2);
	  float tz    =  (rt[2]-rn[2])*sqrt(rt2);

	  float dT  = ((tb-tz)*(tb-tz)-m_R[t]*Rb2z-(Erb+m_Er[t]))-(m_R[t]*Rb2r)*((tb+tz)*(tb+tz));
	  if( dT > ICSA) continue;

	  float Rn    = sqrt(rn[0]*rn[0]+rn[1]*rn[1]);
	  float Ax    = rn[0]/Rn;
	  float Ay    = rn[1]/Rn;

	  float ub    = (xb*Ax+yb*Ay)*rb2;
	  float vb    = (yb*Ax-xb*Ay)*rb2;
	  float ut    = (xt*Ax+yt*Ay)*rt2;
	  float vt    = (yt*Ax-xt*Ay)*rt2;
	
	  float dU  = ut-ub; if(dU == 0.) continue;	
	  float A   = (vt-vb)/dU;
	  float S2  = 1.+A*A                           ;
	  float B   = vb-A*ub                          ;
	  float B2  = B*B                              ;
	  if(B2  > ipt2K*S2 || dT*S2 > B2*CSA) continue;

	  float Im  = fabs((A-B*Rn)*Rn)                ; 

	  if(Im <= imax) {
	    float dr; m_R[t] < m_R[b] ? dr = m_R[t] : dr = m_R[b]; Im+=fabs((tb-tz)/(dr*sTzb2));
	    m_CmSp.push_back(std::make_pair(B/sqrt(S2),m_SP[t])); m_SP[t]->setParam(Im);
	  }
	}
      }

      if(!m_CmSp.empty()) newOneSeedWithCurvaturesComparisonMIXED(m_SP[b],(*r0),Zob);
    }
    fillSeeds();  nseed += m_fillOneSeeds;
    if(nseed>=m_maxsize) {
      m_endlist=false; ++r0; m_rMin = r0;  return;
    } 
  }
}

///////////////////////////////////////////////////////////////////
// Production PPP+PPS+PSS  space points seeds for full scan
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ITK::production3SpMIXEDold
( std::vector<InDet::SiSpacePointForSeedITK*>::iterator* rb ,
  std::vector<InDet::SiSpacePointForSeedITK*>::iterator* rbe,
  std::vector<InDet::SiSpacePointForSeedITK*>::iterator* rt ,
  std::vector<InDet::SiSpacePointForSeedITK*>::iterator* rte,
  int NB, int NT, int& nseed) 
{
  std::vector<InDet::SiSpacePointForSeedITK*>::iterator r0=rb[0],r;
  if(!m_endlist) {r0 = m_rMin; m_endlist = true;}

  float ipt2K = m_ipt2K   ;
  float ipt2C = m_ipt2C   ;
  float COFK  = m_COFK    ; 
  float imaxp = m_diver   ;
  float imaxs = m_divermax;

  m_CmSp.clear();

  // Loop through all trigger space points
  //
  for(; r0!=rbe[0]; ++r0) {

    m_nOneSeeds = 0;
    m_mapOneSeeds.clear();
    bool  pix   = !(*r0)->spacepoint->clusterList().second;
    float R     = (*r0)->radius();
    float Ri    = 1./R           ;
    float X     = (*r0)->x()     ;
    float Y     = (*r0)->y()     ;
    float Z     = (*r0)->z()     ;
    float ax    = X*Ri           ;
    float ay    = Y*Ri           ;
    float U0    =  -Ri           ;
    float VR    = imaxp*(Ri*Ri)  ;
    float covr0 = (*r0)->covr () ;
    float covz0 = (*r0)->covz () ;
    int   Nb    = 0              ;

    // Bottom links production
    //
    for(int i=0; i!=NB; ++i) {

      for(r=rb[i]; r!=rbe[i]; ++r) {
	
	float Rb =(*r)->radius();  
	float dR = R-Rb; 

	if(dR > m_drmax) {rb[i]=r; continue;}   
	if(dR < m_drmin) break;

	float dz = (*r)->z()-Z ;

	// Comparison with vertices Z coordinates
	//
	float Tz = -dz/dR, aTz = fabs(Tz);
	if(aTz < m_dzdrmin      ||     aTz > m_dzdrmax) continue;
	float Zo = Z-R*Tz; if(!isZCompatible(Zo,Rb,Tz)) continue;
	
	float dx = (*r)->x()-X ;
	float dy = (*r)->y()-Y ;
	float x  = dx*ax+dy*ay ; if(x >= 0.) continue;
	float y  = dy*ax-dx*ay ;
	float r2 = 1./(x*x+y*y);
	float u  = x*r2        ;
	float v  = y*r2        ;

	if(pix && fabs(R*y) > -imaxp*x) {

	  float V0  = VR;   if(y < 0. )  V0=-V0  ; 
	  float dU  = u-U0; if(dU == 0.) continue; 
	  float A   = (v-V0)/dU                  ;
	  float B   = V0-A*U0                    ;
	  if((B*B) > (ipt2K*(1.+A*A))) continue  ;
	}

	float dr  = sqrt(r2)                                              ;
	float tz  =-dz*dr                                                 ; 
	m_X [Nb]  = x                                                     ;
	m_Y [Nb]  = y                                                     ;
	m_Tz[Nb]  = tz                                                    ;
	m_Zo[Nb]  = Z-R*tz                                                ;
	m_R [Nb]  = dr                                                    ;
	m_U [Nb]  = u                                                     ;
	m_V [Nb]  = v                                                     ;
	m_Er[Nb]  = ((covz0+(*r)->covz())+(tz*tz)*(covr0+(*r)->covr()))*r2;
	m_SP[Nb]  = (*r); if(++Nb==m_maxsizeSP) goto breakb;
      }
    }
  breakb:
    if(!Nb || Nb==m_maxsizeSP) continue;
 
    int Nt = Nb;
    
    // Top   links production
    //
    for(int i=0; i!=NT; ++i) {
      
      for(r=rt[i]; r!=rte[i]; ++r) {
	
	float Rt =(*r)->radius();
	float dR = Rt-R; 
	
	if(dR<m_drmin) {rt[i]=r; continue;}
	if(dR>m_drmax) break;

	float dz = (*r)->z()-Z ;

	// Comparison with vertices Z coordinates
	//
	float Tz = dz/dR, aTz =fabs(Tz);  
	if(aTz < m_dzdrmin || aTz > m_dzdrmax) continue;
	float Zo = Z-R*Tz; if(!isZCompatible(Zo,R ,Tz)) continue;

	float dx = (*r)->x()-X ;
	float dy = (*r)->y()-Y ;
	float x  = dx*ax+dy*ay ; if(x<=0.) continue;
	float y  = dy*ax-dx*ay ;
	float r2 = 1./(x*x+y*y);
	float u  = x*r2        ;
	float v  = y*r2        ;

	if(!(*r)->spacepoint->clusterList().second && fabs(R*y) > imaxp*x) {

	  float V0  = VR;   if(y > 0. )  V0=-V0  ; 
	  float dU  = u-U0; if(dU == 0.) continue; 
	  float A   = (v-V0)/dU                  ;
	  float B   = V0-A*U0                    ;
	  if((B*B) > (ipt2K*(1.+A*A))) continue  ;
	}
       
	float dr  = sqrt(r2)                                              ;
	float tz  = dz*dr                                                 ; 
	m_X [Nt]  = x                                                     ;
	m_Y [Nt]  = y                                                     ;
	m_Tz[Nt]  = tz                                                    ;
	m_Zo[Nt]  = Z-R*tz                                                ;
	m_R [Nt]  = dr                                                    ;
	m_U [Nt]  = u                                                     ;
	m_V [Nt]  = v                                                     ;
	m_Er[Nt]  = ((covz0+(*r)->covz())+(tz*tz)*(covr0+(*r)->covr()))*r2;
  	m_SP[Nt]  = (*r); if(++Nt==m_maxsizeSP) goto breakt;
      }
    }
    
  breakt:
    if(!(Nt-Nb)) continue;

    covr0      *= .5;
    covz0      *= 2.;
   
    // Three space points comparison
    //
    for(int b=0; b!=Nb; ++b) {
    
      bool  pixb  =!m_SP[b]->spacepoint->clusterList().second;
      float  Zob  = m_Zo[b]      ;
      float  Tzb  = m_Tz[b]      ;
      float  Rb2r = m_R [b]*covr0;
      float  Rb2z = m_R [b]*covz0;
      float  Erb  = m_Er[b]      ;
      float  Vb   = m_V [b]      ;
      float  Ub   = m_U [b]      ;
      float  Tzb2 = (1.+Tzb*Tzb) ;
      float sTzb2 = sqrt(Tzb2)   ;
      float  CSA  = Tzb2*COFK    ;
      float ICSA  = Tzb2*ipt2C   ;
      float imax  = imaxs        ; if(pixb) imax = imaxp;

      float Se    = 1./sTzb2     ;
      float Ce    = Se*Tzb       ;
      float Sx    = Se*ax        ;
      float Sy    = Se*ay        ;

      for(int t=Nb;  t!=Nt; ++t) {

	if(pix && !m_SP[t]->spacepoint->clusterList().second) {
	
	  float dT  = ((Tzb-m_Tz[t])*(Tzb-m_Tz[t])-m_R[t]*Rb2z-(Erb+m_Er[t]))-(m_R[t]*Rb2r)*((Tzb+m_Tz[t])*(Tzb+m_Tz[t]));
	  if( dT > ICSA) continue;

	  float dU  = m_U[t]-Ub; if(dU == 0.) continue ; 
	  float A   = (m_V[t]-Vb)/dU                   ;
	  float S2  = 1.+A*A                           ;
	  float B   = Vb-A*Ub                          ;
	  float B2  = B*B                              ;
	  if(B2  > ipt2K*S2 || dT*S2 > B2*CSA) continue;
	  
	  float Im  = fabs((A-B*R)*R)                  ; 
	  
	  if(Im <= imax) {
	    float dr; m_R[t] < m_R[b] ? dr = m_R[t] : dr = m_R[b]; Im+=fabs((Tzb-m_Tz[t])/(dr*sTzb2));
	    m_CmSp.push_back(std::make_pair(B/sqrt(S2),m_SP[t])); m_SP[t]->setParam(Im);
	  }
	}
	else   {

	  // Trigger point
	  //	
	  float dU0   =  m_U[t]-Ub       ;  if(dU0 == 0.) continue; 
	  float A0    = (m_V[t]-Vb)/dU0  ;
	  float C0    = 1./sqrt(1.+A0*A0); 
	  float rn[3];
	  if(pix) {
	    rn[0] = X; rn[1] = Y; rn[2] = Z;
	  }
	  else  {
	    float S0    = A0*C0            ;
	    float d0[3] = {Sx*C0-Sy*S0,Sx*S0+Sy*C0,Ce};  
	    if(!(*r0)->coordinates(d0,rn)) continue;
	  }
	  
	  // Bottom  point
	  //
	  float B0    = 2.*(Vb-A0*Ub);
	  float rb[3];
	  if(pixb) {
	    rb[0] = m_SP[b]->x(); rb[1] = m_SP[b]->y(); rb[2] = m_SP[b]->z();
	  }
	  else {
	    float Cb    = (1.-B0*m_Y[b])*C0;
	    float Sb    = (A0+B0*m_X[b])*C0;
	    float db[3] = {Sx*Cb-Sy*Sb,Sx*Sb+Sy*Cb,Ce};  
	    if(!m_SP[b]->coordinates(db,rb)) continue;
	  }
	  
	  // Top     point
	  //
	  float Ct    = (1.-B0*m_Y[t])*C0;
	  float St    = (A0+B0*m_X[t])*C0;
	  float dt[3] = {Sx*Ct-Sy*St,Sx*St+Sy*Ct,Ce};  
	  float rt[3];  if(!m_SP[t]->coordinates(dt,rt)) continue;

	  float xb    = rb[0]-rn[0];
	  float yb    = rb[1]-rn[1];
	  float xt    = rt[0]-rn[0];
	  float yt    = rt[1]-rn[1];

	  float rb2   = 1./(xb*xb+yb*yb);
	  float rt2   = 1./(xt*xt+yt*yt);
	
	  float tb    =  (rn[2]-rb[2])*sqrt(rb2);
	  float tz    =  (rt[2]-rn[2])*sqrt(rt2);

	  float dT  = ((tb-tz)*(tb-tz)-m_R[t]*Rb2z-(Erb+m_Er[t]))-(m_R[t]*Rb2r)*((tb+tz)*(tb+tz));
	  if( dT > ICSA) continue;

	  float Rn    = sqrt(rn[0]*rn[0]+rn[1]*rn[1]);
	  float Ax    = rn[0]/Rn;
	  float Ay    = rn[1]/Rn;

	  float ub    = (xb*Ax+yb*Ay)*rb2;
	  float vb    = (yb*Ax-xb*Ay)*rb2;
	  float ut    = (xt*Ax+yt*Ay)*rt2;
	  float vt    = (yt*Ax-xt*Ay)*rt2;
	
	  float dU  = ut-ub; if(dU == 0.) continue;	
	  float A   = (vt-vb)/dU;
	  float S2  = 1.+A*A                           ;
	  float B   = vb-A*ub                          ;
	  float B2  = B*B                              ;
	  if(B2  > ipt2K*S2 || dT*S2 > B2*CSA) continue;

	  float Im  = fabs((A-B*Rn)*Rn)                ; 

	  if(Im <= imax) {
	    float dr; m_R[t] < m_R[b] ? dr = m_R[t] : dr = m_R[b]; Im+=fabs((tb-tz)/(dr*sTzb2));
	    m_CmSp.push_back(std::make_pair(B/sqrt(S2),m_SP[t])); m_SP[t]->setParam(Im);
	  }
	}
      }
      if(!m_CmSp.empty()) newOneSeedWithCurvaturesComparisonMIXED(m_SP[b],(*r0),Zob);
    }
    fillSeeds();  nseed += m_fillOneSeeds;
    if(nseed>=m_maxsize) {
      m_endlist=false; ++r0; m_rMin = r0;  return;
    } 
  }
}

///////////////////////////////////////////////////////////////////
// Production 3 SCT space points seeds for full scan
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ITK::production3SpSSS
( std::vector<InDet::SiSpacePointForSeedITK*>::iterator* rb ,
  std::vector<InDet::SiSpacePointForSeedITK*>::iterator* rbe,
  std::vector<InDet::SiSpacePointForSeedITK*>::iterator* rt ,
  std::vector<InDet::SiSpacePointForSeedITK*>::iterator* rte,
  int NB, int NT, int& nseed) 
{
  std::vector<InDet::SiSpacePointForSeedITK*>::iterator r0=rb[0],r;
  if(!m_endlist) {r0 = m_rMin; m_endlist = true;}

  float ipt2K = m_ipt2K     ;
  float ipt2C = m_ipt2C     ;
  float COFK  = m_COFK      ; 
  float imaxs = m_diversss  ;

  m_CmSpn.clear();

  for(; r0!=rbe[0]; ++r0) {if((*r0)->radius() > m_RTmin) break;}

  // Loop through all trigger space points
  //
  for(; r0!=rbe[0]; ++r0) {

    m_nOneSeeds = 0;
    m_mapOneSeeds.clear();

    float               R    = (*r0)->radius(); if(R       > m_RTmax) break;
    float               Z    = (*r0)->z()     ; if(fabs(Z) >  2700. ) continue;
    float               X    = (*r0)->x()     ;
    float               Y    = (*r0)->y()     ;
    int                 Nb   = 0              ;

    // Bottom links production
    //
    for(int i=0; i!=NB; ++i) {

      for(r=rb[i]; r!=rbe[i]; ++r) {
	
	float Rb =(*r)->radius();  
	float dR = R-Rb; 

	if(dR > m_drmax) {rb[i]=r; continue;} 
	if(dR <   20.  ) break;

	// Comparison with vertices Z coordinates
	//
	float dz = Z-(*r)->z(); 
	if(fabs(dz) > 900. || fabs(Z*dR-R*dz) > m_zmaxU*dR) continue;
	m_SP[Nb] = (*r); if(++Nb==m_maxsizeSP) goto breakb;
      }
    }
  breakb:
    if(!Nb || Nb==m_maxsizeSP) continue;  
    int Nt = Nb;
    
    // Top   links production
    //
    for(int i=0; i!=NT; ++i) {
      
      for(r=rt[i]; r!=rte[i]; ++r) {
	
	float Rt =(*r)->radius();
	float dR = Rt-R; 
	
	if(dR <  20. ) {rt[i]=r; continue;}
	if(dR>m_drmax) break;

	// Comparison with vertices Z coordinates
	//
	float dz = (*r)->z()-Z; 

	if(fabs(dz) > 900. || fabs(Z*dR-R*dz) > m_zmaxU*dR) continue;
  	m_SP[Nt] = (*r); if(++Nt==m_maxsizeSP) goto breakt;
      }
    }
    
  breakt:
    if(!(Nt-Nb)) continue;

    float covr0 = (*r0)->covr ();
    float covz0 = (*r0)->covz ();
    float ax    = X/R           ;
    float ay    = Y/R           ;

    for(int i=0; i!=Nt; ++i) {

      InDet::SiSpacePointForSeedITK* sp = m_SP[i];  

      float dx  = sp->x()-X   ;
      float dy  = sp->y()-Y   ;
      float dz  = sp->z()-Z   ;
      float x   = dx*ax+dy*ay ;
      float y   = dy*ax-dx*ay ;
      float r2  = 1./(x*x+y*y);
      float dr  = sqrt(r2)    ;
      float tz  = dz*dr       ; if(i < Nb) tz = -tz;

      m_X [i]   = x                                             ;
      m_Y [i]   = y                                             ;
      m_Tz[i]   = tz                                            ;
      m_Zo[i]   = Z-R*tz                                        ;
      m_R [i]   = dr                                            ;
      m_U [i]   = x*r2                                          ;
      m_V [i]   = y*r2                                          ;
      m_Er[i]   = ((covz0+sp->covz())+(tz*tz)*(covr0+sp->covr()))*r2;
    }
    covr0      *= .5;
    covz0      *= 2.;
   
    // Three space points comparison
    //
    for(int b=0; b!=Nb; ++b) {
    
      float  Zob  = m_Zo[b]      ;
      float  Tzb  = m_Tz[b]      ;
      float  Rb2r = m_R [b]*covr0;
      float  Rb2z = m_R [b]*covz0;
      float  Erb  = m_Er[b]      ;
      float  Vb   = m_V [b]      ;
      float  Ub   = m_U [b]      ;
      float  Tzb2 = (1.+Tzb*Tzb) ;
      float sTzb2 = sqrt(Tzb2)   ;
      float  CSA  = Tzb2*COFK    ;
      float ICSA  = Tzb2*ipt2C   ;
      float imax  = imaxs        ; 
      float Se    = 1./sTzb2     ;
      float Ce    = Se*Tzb       ;
      float Sx    = Se*ax        ;
      float Sy    = Se*ay        ;

      for(int t=Nb;  t!=Nt; ++t) {

	// Trigger point
	//	
	float dU0   =  m_U[t]-Ub       ;  if(dU0 == 0.) continue; 
	float A0    = (m_V[t]-Vb)/dU0  ;
	float C0    = 1./sqrt(1.+A0*A0); 
	float S0    = A0*C0            ;
	float d0[3] = {Sx*C0-Sy*S0,Sx*S0+Sy*C0,Ce};  
	float rn[3]; if(!   (*r0)->coordinates(d0,rn)) continue;

	// Bottom  point
	//
	float B0    = 2.*(Vb-A0*Ub);
	float Cb    = (1.-B0*m_Y[b])*C0;
	float Sb    = (A0+B0*m_X[b])*C0;
	float db[3] = {Sx*Cb-Sy*Sb,Sx*Sb+Sy*Cb,Ce};  
	float rb[3];  if(!m_SP[b]->coordinates(db,rb)) continue;

	// Top     point
	//
	float Ct    = (1.-B0*m_Y[t])*C0;
	float St    = (A0+B0*m_X[t])*C0;
	float dt[3] = {Sx*Ct-Sy*St,Sx*St+Sy*Ct,Ce};  
	float rt[3];  if(!m_SP[t]->coordinates(dt,rt)) continue;

	float xb    = rb[0]-rn[0];
	float yb    = rb[1]-rn[1];
	float xt    = rt[0]-rn[0];
	float yt    = rt[1]-rn[1];
	float dxy   = xt*xt+yt*yt;
	float dz    = rt[2]-rn[2];

	float rb2   = 1./(xb*xb+yb*yb);
	float rt2   = 1./dxy          ;
	
	float tb    = (rn[2]-rb[2])*sqrt(rb2);
	float tz    = dz           *sqrt(rt2);

	float dT  = ((tb-tz)*(tb-tz)-m_R[t]*Rb2z-(Erb+m_Er[t]))-(m_R[t]*Rb2r)*((tb+tz)*(tb+tz));
	if( dT > ICSA) continue;

	float Rn    = sqrt(rn[0]*rn[0]+rn[1]*rn[1]);
	float Ax    = rn[0]/Rn;
	float Ay    = rn[1]/Rn;

	float ub    = (xb*Ax+yb*Ay)*rb2;
	float vb    = (yb*Ax-xb*Ay)*rb2;
	float ut    = (xt*Ax+yt*Ay)*rt2;
	float vt    = (yt*Ax-xt*Ay)*rt2;
	
	float dU  = ut-ub; if(dU == 0.) continue;	
	float A   = (vt-vb)/dU;
	float S2  = 1.+A*A                           ;
	float B   = vb-A*ub                          ;
	float B2  = B*B                              ;
	if(B2  > ipt2K*S2 || dT*S2 > B2*CSA) continue;

	float Im  = fabs((A-B*Rn)*Rn)                ; 

	if(Im <= imax) {
	  m_Im[t] = Im;
	  m_L [t] = sqrt(dxy+dz*dz); 
	  m_CmSpn.push_back(std::make_pair(B/sqrt(S2),t));
	}
      }
      if(!m_CmSpn.empty()) newOneSeedWithCurvaturesComparisonSSS(m_SP[b],(*r0),Zob); m_CmSpn.clear();
   }
    fillSeeds();  nseed += m_fillOneSeeds;
    if(nseed>=m_maxsize) {m_endlist=false; ++r0; m_rMin = r0;  return;} 
 } 
}
///////////////////////////////////////////////////////////////////
// Production PPS space points seeds for full scan
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ITK::production3SpPPS
( std::vector<InDet::SiSpacePointForSeedITK*>::iterator* rb ,
  std::vector<InDet::SiSpacePointForSeedITK*>::iterator* rbe,
  std::vector<InDet::SiSpacePointForSeedITK*>::iterator* rt ,
  std::vector<InDet::SiSpacePointForSeedITK*>::iterator* rte,
  int NB, int NT, int& nseed) 
{
  std::vector<InDet::SiSpacePointForSeedITK*>::iterator r0=rb[0],r;
  if(!m_endlist) {r0 = m_rMin; m_endlist = true;}

  float ipt2K = m_ipt2K   ;
  float ipt2C = m_ipt2C   ;
  float COFK  = m_COFK    ; 
  float imaxp = m_diverpps;

  m_CmSpn.clear();

  for(; r0!=rbe[0]; ++r0) {if((*r0)->radius() > m_RTmin) break;}

  // Loop through all trigger space points
  //
  for(; r0!=rbe[0]; ++r0) {

    if((*r0)->spacepoint->clusterList().second) break;

    m_nOneSeeds = 0;
    m_mapOneSeeds.clear();

    float R     = (*r0)->radius();
    float Ri    = 1./R           ;
    float X     = (*r0)->x()     ;
    float Y     = (*r0)->y()     ;
    float Z     = (*r0)->z()     ;
    float ax    = X*Ri           ;
    float ay    = Y*Ri           ;
    float U0    =  -Ri           ;
    float VR    = imaxp*(Ri*Ri)  ;
    float covr0 = (*r0)->covr () ;
    float covz0 = (*r0)->covz () ;
    int   Nb    = 0              ;

    // Bottom links production
    //
    for(int i=0; i!=NB; ++i) {

      for(r=rb[i]; r!=rbe[i]; ++r) {
	
	float Rb =(*r)->radius();  
	float dR = R-Rb; 

	if(dR < m_drmin) break;

	float dz = Z-(*r)->z();
	if(fabs(Z*dR-R*dz) > m_zmaxU*dR || fabs(dz) > m_dzdrmaxPPS*dR) continue;
	
	float dx = (*r)->x()-X ;
	float dy = (*r)->y()-Y ;
	float x  = dx*ax+dy*ay ; if(x >= 0.) continue;
	float y  = dy*ax-dx*ay ;
	float r2 = 1./(x*x+y*y);
	float u  = x*r2        ;
	float v  = y*r2        ;

	if(fabs(R*y) > -imaxp*x) {

	  float V0  = VR;   if(y < 0. )  V0=-V0  ; 
	  float dU  = u-U0; if(dU == 0.) continue; 
	  float A   = (v-V0)/dU                  ;
	  float B   = V0-A*U0                    ;
	  if((B*B) > (ipt2K*(1.+A*A))) continue  ;
	}

	float dr  = sqrt(r2)                                              ;
	float tz  = dz*dr                                                 ; 
	m_X [Nb]  = x                                                     ;
	m_Y [Nb]  = y                                                     ;
	m_Tz[Nb]  = tz                                                    ;
	m_Zo[Nb]  = Z-R*tz                                                ;
	m_R [Nb]  = dr                                                    ;
	m_U [Nb]  = u                                                     ;
	m_V [Nb]  = v                                                     ;
	m_Er[Nb]  = ((covz0+(*r)->covz())+(tz*tz)*(covr0+(*r)->covr()))*r2;
	m_SP[Nb]  = (*r); if(++Nb==m_maxsizeSP) goto breakb;
      }
    }
  breakb:
    if(!Nb || Nb==m_maxsizeSP) continue;
 
    int Nt = Nb;
    
    // Top   links production
    //
    for(int i=0; i!=NT; ++i) {
      
      for(r=rt[i]; r!=rte[i]; ++r) {
	
	if(!(*r)->spacepoint->clusterList().second) {rt[i]=r; continue;}

	float Rt =(*r)->radius();
	float dR = Rt-R; 
	float dz = (*r)->z()-Z ;
	if(fabs(Z*dR-R*dz)> m_zmaxU*dR) continue;

	float dx  = (*r)->x()-X ;
	float dy  = (*r)->y()-Y ;
	float x   = dx*ax+dy*ay ; if(x<=0.) continue;
	float y   = dy*ax-dx*ay ;
	float r2  = 1./(x*x+y*y);
	float u   = x*r2        ;
	float v   = y*r2        ;
	float dr  = sqrt(r2)                                              ;
	float tz  = dz*dr                                                 ; 
	m_X [Nt]  = x                                                     ;
	m_Y [Nt]  = y                                                     ;
	m_Tz[Nt]  = tz                                                    ;
	m_Zo[Nt]  = Z-R*tz                                                ;
	m_R [Nt]  = dr                                                    ;
	m_U [Nt]  = u                                                     ;
	m_V [Nt]  = v                                                     ;
	m_Er[Nt]  = ((covz0+(*r)->covz())+(tz*tz)*(covr0+(*r)->covr()))*r2;
  	m_SP[Nt]  = (*r); if(++Nt==m_maxsizeSP) goto breakt;
      }
    }
    
  breakt:
    if(!(Nt-Nb) || Nt< 3) continue;

    covr0      *= .5;
    covz0      *= 2.;
   
    // Three space points comparison
    //
    for(int b=0; b!=Nb; ++b) {
    
      float  Zob  = m_Zo[b]      ;
      float  Tzb  = m_Tz[b]      ;
      float  Rb2r = m_R [b]*covr0;
      float  Rb2z = m_R [b]*covz0;
      float  Erb  = m_Er[b]      ;
      float  Vb   = m_V [b]      ;
      float  Ub   = m_U [b]      ;
      float  Tzb2 = (1.+Tzb*Tzb) ;
      float sTzb2 = sqrt(Tzb2)   ;
      float  CSA  = Tzb2*COFK    ;
      float ICSA  = Tzb2*ipt2C   ;
      float imax  = imaxp        ; 
      float Se    = 1./sTzb2     ;
      float Ce    = Se*Tzb       ;
      float Sx    = Se*ax        ;
      float Sy    = Se*ay        ;

      for(int t=Nb;  t!=Nt; ++t) {


	// Trigger point
	//	
	float dU0   =  m_U[t]-Ub       ;  if(dU0 == 0.) continue; 
	float A0    = (m_V[t]-Vb)/dU0  ;
	float C0    = 1./sqrt(1.+A0*A0); 
	float rn[3] = {X,Y,Z};
	  
	// Bottom  point
	//
	float B0    = 2.*(Vb-A0*Ub);
	  
	// Top     point
	//
	float Ct    = (1.-B0*m_Y[t])*C0;
	float St    = (A0+B0*m_X[t])*C0;
	float dt[3] = {Sx*Ct-Sy*St,Sx*St+Sy*Ct,Ce};  
	float rt[3];  if(!m_SP[t]->coordinates(dt,rt)) continue;

	float xt    = rt[0]-rn[0];
	float yt    = rt[1]-rn[1];
	float zt    = rt[2]-rn[2];
	float dxy   = xt*xt+yt*yt;
	float rt2   = 1./dxy     ;
	float tz    = zt*sqrt(rt2);
	
	float dT  = ((Tzb-tz)*(Tzb-tz)-m_R[t]*Rb2z-(Erb+m_Er[t]))-(m_R[t]*Rb2r)*((Tzb+tz)*(Tzb+tz));
	if( dT > ICSA) continue;
	
	float Rn    = sqrt(rn[0]*rn[0]+rn[1]*rn[1]);
	float Ax    = rn[0]/Rn;
	float Ay    = rn[1]/Rn;
	float ut    = (xt*Ax+yt*Ay)*rt2;
	float vt    = (yt*Ax-xt*Ay)*rt2;
	float dU  = ut-Ub; if(dU == 0.) continue;	
	float A   = (vt-Vb)/dU;
	float S2  = 1.+A*A                           ;
	float B   = Vb-A*Ub                          ;
	float B2  = B*B                              ;
	if(B2  > ipt2K*S2 || dT*S2 > B2*CSA) continue;
	
	float Im  = fabs((A-B*Rn)*Rn)                ; 

	if(Im <= imax) {

	  m_Im[t] = Im;
	  m_L [t] = sqrt(dxy+zt*zt);
	  m_CmSpn.push_back(std::make_pair(B/sqrt(S2),t));
	}
       
      }
      if(m_CmSpn.size() > 1) newOneSeedWithCurvaturesComparisonPPS(m_SP[b],(*r0),Zob);
      m_CmSpn.clear();
    }
    fillSeeds();  nseed += m_fillOneSeeds;
    if(nseed>=m_maxsize) {
      m_endlist=false; ++r0; m_rMin = r0;  return;
    } 
  }
}

///////////////////////////////////////////////////////////////////
// Production 3 space points seeds in ROI
///////////////////////////////////////////////////////////////////

 
void InDet::SiSpacePointsSeedMaker_ITK::production3SpTrigger
( std::vector<InDet::SiSpacePointForSeedITK*>::iterator* rb ,
  std::vector<InDet::SiSpacePointForSeedITK*>::iterator* rbe,
  std::vector<InDet::SiSpacePointForSeedITK*>::iterator* rt ,
  std::vector<InDet::SiSpacePointForSeedITK*>::iterator* rte,
  int NB, int NT, int& nseed) 
{
  std::vector<InDet::SiSpacePointForSeedITK*>::iterator r0=rb[0],r;
  if(!m_endlist) {r0 = m_rMin; m_endlist = true;}

  const float pi = M_PI, pi2 = 2.*pi; 

  float ipt2K = m_ipt2K   ;
  float ipt2C = m_ipt2C   ;
  float COFK  = m_COFK    ; 
  float imaxp = m_diver   ;
  float imaxs = m_diversss;

  m_CmSp.clear();

  // Loop through all trigger space points
  //
  for(; r0!=rbe[0]; ++r0) {

    m_nOneSeeds = 0;
    m_mapOneSeeds.clear();
	
    float R  = (*r0)->radius(); 

    const Trk::Surface* sur0 = (*r0)->sur();
    float               X    = (*r0)->x()  ;
    float               Y    = (*r0)->y()  ;
    float               Z    = (*r0)->z()  ;
    int                 Nb   = 0           ;

    // Bottom links production
    //
    for(int i=0; i!=NB; ++i) {

      for(r=rb[i]; r!=rbe[i]; ++r) {
	
	float Rb =(*r)->radius();  

	float dR = R-Rb; 
	if(dR < m_drmin || (m_iteration && (*r)->spacepoint->clusterList().second)) break;
	if(dR > m_drmax || (*r)->sur()==sur0) continue;

	// Comparison with  bottom and top Z 
	//
	float Tz = (Z-(*r)->z())/dR;
	float Zo = Z-R*Tz          ; if(Zo < m_zminB || Zo > m_zmaxB) continue;
	float Zu = Z+(550.-R)*Tz   ; if(Zu < m_zminU || Zu > m_zmaxU) continue;
	m_SP[Nb] = (*r); if(++Nb==m_maxsizeSP) goto breakb;
      }
    }
  breakb:
    if(!Nb || Nb==m_maxsizeSP) continue;  
    int Nt = Nb;
    
    // Top   links production
    //
    for(int i=0; i!=NT; ++i) {
      
      for(r=rt[i]; r!=rte[i]; ++r) {
	
	float Rt =(*r)->radius();
	float dR = Rt-R; 
	
	if(dR<m_drmin) {rt[i]=r; continue;}
	if(dR>m_drmax) break;

	if( (*r)->sur()==sur0) continue;

	// Comparison with  bottom and top Z 
	//
	float Tz = ((*r)->z()-Z)/dR;  
	float Zo = Z-R*Tz          ; if(Zo < m_zminB || Zo > m_zmaxB) continue;
	float Zu = Z+(550.-R)*Tz   ; if(Zu < m_zminU || Zu > m_zmaxU) continue;
  	m_SP[Nt] = (*r); if(++Nt==m_maxsizeSP) goto breakt;
      }
    }
    
  breakt:
    if(!(Nt-Nb)) continue;
    float covr0 = (*r0)->covr ();
    float covz0 = (*r0)->covz ();

    float ax   = X/R;
    float ay   = Y/R;
    
    for(int i=0; i!=Nt; ++i) {

      InDet::SiSpacePointForSeedITK* sp = m_SP[i];  

      float dx  = sp->x()-X   ;
      float dy  = sp->y()-Y   ;
      float dz  = sp->z()-Z   ;
      float x   = dx*ax+dy*ay ;
      float y   = dy*ax-dx*ay ;
      float r2  = 1./(x*x+y*y);
      float dr  = sqrt(r2)    ;
      float tz  = dz*dr       ; if(i < Nb) tz = -tz;

      m_X [i]   = x                                             ;
      m_Y [i]   = y                                             ;
      m_Tz[i]   = tz                                            ;
      m_Zo[i]   = Z-R*tz                                        ;
      m_R [i]   = dr                                            ;
      m_U [i]   = x*r2                                          ;
      m_V [i]   = y*r2                                          ;
      m_Er[i]   = ((covz0+sp->covz())+(tz*tz)*(covr0+sp->covr()))*r2;
    }
    covr0      *= .5;
    covz0      *= 2.;
   
    // Three space points comparison
    //
    for(int b=0; b!=Nb; ++b) {
    
      float  Zob  = m_Zo[b]      ;
      float  Tzb  = m_Tz[b]      ;
      float  Rb2r = m_R [b]*covr0;
      float  Rb2z = m_R [b]*covz0;
      float  Erb  = m_Er[b]      ;
      float  Vb   = m_V [b]      ;
      float  Ub   = m_U [b]      ;
      float  Tzb2 = (1.+Tzb*Tzb) ;
      float  CSA  = Tzb2*COFK    ;
      float ICSA  = Tzb2*ipt2C   ;
      float imax  = imaxp        ; if(m_SP[b]->spacepoint->clusterList().second) imax = imaxs;
      
      for(int t=Nb;  t!=Nt; ++t) {

	float dT  = ((Tzb-m_Tz[t])*(Tzb-m_Tz[t])-m_R[t]*Rb2z-(Erb+m_Er[t]))-(m_R[t]*Rb2r)*((Tzb+m_Tz[t])*(Tzb+m_Tz[t]));
	if( dT > ICSA) continue;
	float dU  = m_U[t]-Ub; if(dU == 0.) continue ; 
	float A   = (m_V[t]-Vb)/dU                   ;
	float S2  = 1.+A*A                           ;
	float B   = Vb-A*Ub                          ;
	float B2  = B*B                              ;
	if(B2  > ipt2K*S2 || dT*S2 > B2*CSA) continue;

	float Im  = fabs((A-B*R)*R)                  ; 
	if(Im > imax) continue;

	// Azimuthal angle test
	//
	float y  = 1.;
	float x  = 2.*B*R-A;
	float df = fabs(atan2(ay*y-ax*x,ax*y+ay*x)-m_ftrig);
	if(df > pi      ) df=pi2-df;
	if(df > m_ftrigW) continue;
	m_CmSp.push_back(std::make_pair(B/sqrt(S2),m_SP[t])); m_SP[t]->setParam(Im);
      }
      if(!m_CmSp.empty()) newOneSeedWithCurvaturesComparison(m_SP[b],(*r0),Zob);
    }
    fillSeeds();  nseed += m_fillOneSeeds;
    if(nseed>=m_maxsize) {m_endlist=false; ++r0; m_rMin = r0;  return;} 
  }
}

///////////////////////////////////////////////////////////////////
// New 3 space points pro seeds 
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ITK::newOneSeed
(InDet::SiSpacePointForSeedITK*& p1, InDet::SiSpacePointForSeedITK*& p2,
 InDet::SiSpacePointForSeedITK*& p3,float z,float q)
{
  if(m_nOneSeeds < m_maxOneSize) {

    m_OneSeeds[m_nOneSeeds].set(p1,p2,p3,z); 
    m_mapOneSeeds.insert(std::make_pair(q,m_OneSeeds+m_nOneSeeds));
    ++m_nOneSeeds;
  }
  else                     {
    std::multimap<float,InDet::SiSpacePointsProSeedITK*>::reverse_iterator 
      l = m_mapOneSeeds.rbegin();

    if((*l).first <= q) return;
    
    InDet::SiSpacePointsProSeedITK* s = (*l).second; s->set(p1,p2,p3,z);

    std::multimap<float,InDet::SiSpacePointsProSeedITK*>::iterator 
      i = m_mapOneSeeds.insert(std::make_pair(q,s));
	
    for(++i; i!=m_mapOneSeeds.end(); ++i) {
      if((*i).second==s) {m_mapOneSeeds.erase(i); return;}
    }
  }
}

///////////////////////////////////////////////////////////////////
// New 3 space points pro seeds production for all seeds
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ITK::newOneSeedWithCurvaturesComparisonMIXED
	(SiSpacePointForSeedITK*& SPb,SiSpacePointForSeedITK*& SP0,float Zob)
{
  const float dC = .00003;

  bool  pixb = !SPb->spacepoint->clusterList().second;

  float ub   = SPb->quality()                        ;
  float u0   = SP0->quality()                        ;

  std::sort(m_CmSp.begin(),m_CmSp.end(),comCurvatureITK());
  std::vector<std::pair<float,InDet::SiSpacePointForSeedITK*>>::iterator j,jn,i = m_CmSp.begin(),ie = m_CmSp.end(); jn=i; 
      
  for(; i!=ie; ++i) {

    bool                pixt = !(*i).second->spacepoint->clusterList().second;

    if(pixt && fabs(SPb->z() -(*i).second->z()) > m_dzmaxPPP) continue;  

    float u    = (*i).second->param(); 
    
    const Trk::Surface* Sui  = (*i).second->sur   ();
    float               Ri   = (*i).second->radius();  
    float               Ci1  =(*i).first-dC         ;
    float               Ci2  =(*i).first+dC         ;
    float               Rmi  = 0.                   ;
    float               Rma  = 0.                   ;
    bool                in   = false;
    
    if     (!pixb) u-=400.;
    else if( pixt) u-=200.;

    for(j=jn;  j!=ie; ++j) {
      
      if(       j == i           ) continue;
      if( (*j).first < Ci1       ) {jn=j; ++jn; continue;}
      if( (*j).first > Ci2       ) break;
      if( (*j).second->sur()==Sui) continue;
      
      float Rj = (*j).second->radius(); if(fabs(Rj-Ri) < m_drmin) continue;

      if(in) {
	if     (Rj > Rma) Rma = Rj;
	else if(Rj < Rmi) Rmi = Rj;
	else continue;
	if( (Rma-Rmi) > 20.) {u-=200.; break;}
      }
      else {
	in=true; Rma=Rmi=Rj; u-=200.;
      }
    }
    if(u > m_umax) continue;
    if(pixb) {
      if(u > ub && u > u0 && u > (*i).second->quality()) continue;
    }
    newOneSeed(SPb,SP0,(*i).second,Zob,u);
  }
  m_CmSp.clear();
}

///////////////////////////////////////////////////////////////////
// New 3 space points pro seeds production PPP seeds
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ITK::newOneSeedWithCurvaturesComparison
	(SiSpacePointForSeedITK*& SPb,SiSpacePointForSeedITK*& SP0,float Zob)
{
  const float dC = .00003;
  std::sort(m_CmSp.begin(),m_CmSp.end(),comCurvatureITK());
  std::vector<std::pair<float,InDet::SiSpacePointForSeedITK*>>::iterator j,jn,i = m_CmSp.begin(),ie = m_CmSp.end(); jn=i; 
      
  for(; i!=ie; ++i) {

    if(fabs(SPb->z() -(*i).second->z()) > m_dzmaxPPP) continue;  

    float u    = (*i).second->param()-200.; 

    const Trk::Surface* Sui  = (*i).second->sur   ();
    float               Ri   = (*i).second->radius();
    float               Ci1  =(*i).first-dC         ;
    float               Ci2  =(*i).first+dC         ;
    float               Rmi  = 0.                   ;
    float               Rma  = 0.                   ;
    bool                in   = false;

    for(j=jn;  j!=ie; ++j) {
      
      if(       j == i           ) continue;
      if( (*j).first < Ci1       ) {jn=j; ++jn; continue;}
      if( (*j).first > Ci2       ) break;
      if( (*j).second->sur()==Sui) continue;
      
      float Rj = (*j).second->radius(); if(fabs(Rj-Ri) < m_drmin) continue;

      if(in) {
	if     (Rj > Rma) Rma = Rj;
	else if(Rj < Rmi) Rmi = Rj;
	else continue;
	if( (Rma-Rmi) > 20.) {u-=200.; break;}
      }
      else {
	in=true; Rma=Rmi=Rj; u-=200.;
      }
    }
    if(u > m_umax || SPb->radius() < 50. || u > -200.) continue;
    if(u > SPb->quality() && u > SP0->quality() && u > (*i).second->quality()) continue;

    newOneSeed(SPb,SP0,(*i).second,Zob,u);
  }
  m_CmSp.clear();
}

///////////////////////////////////////////////////////////////////
// New 3 space points pro seeds production for SSS seeds
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ITK::newOneSeedWithCurvaturesComparisonSSS
(SiSpacePointForSeedITK*& SPb,SiSpacePointForSeedITK*& SP0,float Zob)
{
  const float dC = .00003;

  std::sort(m_CmSpn.begin(),m_CmSpn.end(),comCurvatureITKn());
  std::vector<std::pair<float,int>>::iterator j,jn,i = m_CmSpn.begin(),ie = m_CmSpn.end(); jn=i;

  const Trk::Surface*     SURt[4];
  float                   Lt  [4];

  for(; i!=ie; ++i) {

    int NT     = 1             ;
    int n      = (*i).second   ;
    SURt[0]    = m_SP[n]->sur();
    Lt  [0]    = m_L [n]       ;
    float C    = (*i).first    ;
    float Cmin = C-dC          ;
    float Cmax = C+dC          ;
    
    for(j=jn;  j!=ie; ++j) {

      if(      j == i       ) continue;

      if((*j).first < Cmin ) {jn=j; ++jn; continue;}
      if((*j).first > Cmax ) break;
      
      int m = (*j).second;

      const Trk::Surface*      SUR = m_SP[m]->sur();
      float                      L = m_L [m]       ;

      int k=0;
      for(; k!=NT; ++k) {
	if(  SUR  == SURt[k]  ) break;
	if(fabs(L-Lt[k]) < 20.) break;
      }

      if( k==NT ) {
	SURt[NT]= SUR    ;
	Lt  [NT]= L      ;
	if(++NT==4) break;
      }
    }
    float Q = m_Im[n]-float(NT)*100.; if(NT > 2) Q-=100000.;
    newOneSeed(SPb,SP0,m_SP[n],Zob,Q);
  }
}

///////////////////////////////////////////////////////////////////
// New 3 space points pro seeds production for PPP seeds
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ITK::newOneSeedWithCurvaturesComparisonPPP
(SiSpacePointForSeedITK*& SPb,SiSpacePointForSeedITK*& SP0,float Zob)
{
  const float dC = .00003;

  std::sort(m_CmSpn.begin(),m_CmSpn.end(),comCurvatureITKn());
  std::vector<std::pair<float,int>>::iterator j,jn,i = m_CmSpn.begin(),ie = m_CmSpn.end(); jn=i;


  const Trk::Surface*     SURt[4];
  float                   Lt  [4];

  float Qmin = 1.e20;
  float Rb   = 2.*SPb->radius();
  int   NTc; Rb > 280.? NTc = 1 : NTc = 2;
  int   nmin = -1   ;

  for(; i!=ie; ++i) {

    int NT     = 1             ;
    int n      = (*i).second   ;

    SiSpacePointForSeedITK* SPt = m_SP[n];

    SURt[0]    = SPt->sur()    ;
    Lt  [0]    = m_L [n]       ;
    float C    = (*i).first    ;
    float Cmin = C-dC          ;
    float Cmax = C+dC          ;

    for(j=jn;  j!=ie; ++j) {

      if(      j == i       ) continue;

      if((*j).first < Cmin ) {jn=j; ++jn; continue;}
      if((*j).first > Cmax ) break;
      
      int m = (*j).second;

      const Trk::Surface* SUR = m_SP[m]->sur();
      float               L   = m_L [m]       ;

      int k=0;
      for(; k!=NT; ++k) {
	if(  SUR  == SURt[k]  ) break;
	if(fabs(L-Lt[k]) < 20.) break;
      }

      if( k==NT ) {
	SURt[NT]= SUR    ;
	Lt  [NT]= L      ;
	if(++NT==4) break;
      }
    }
    if(NT < NTc) continue;

    float Q = 100.*m_Im[n]+((SPt->radius()-Rb)-float(NT)*100.);
    
    if(Q > SPb->quality() && Q > SP0->quality() && Q > SPt->quality()) continue;
    if     (NT-NTc > 0   )  newOneSeed(SPb,SP0,SPt,Zob,Q-100000.);
    else if(    Q  < Qmin) {Qmin=Q; nmin=n;}
  }
  if(nmin >=0) newOneSeed(SPb,SP0,m_SP[nmin],Zob,Qmin);
}

///////////////////////////////////////////////////////////////////
// New 3 space points pro seeds production for PPS seeds
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ITK::newOneSeedWithCurvaturesComparisonPPS
(SiSpacePointForSeedITK*& SPb,SiSpacePointForSeedITK*& SP0,float Zob)
{
  const float dC = .00003;

  std::sort(m_CmSpn.begin(),m_CmSpn.end(),comCurvatureITKn());
  std::vector<std::pair<float,int>>::iterator j,jn,i = m_CmSpn.begin(),ie = m_CmSpn.end(); jn=i;

  const Trk::Surface*     SURt[4];
  float                   Lt  [4];

  for(; i!=ie; ++i) {

    int NT     = 1             ;
    int n      = (*i).second   ;
    SURt[0]    = m_SP[n]->sur();
    Lt  [0]    = m_L [n]       ;
    float C    = (*i).first    ;
    float Cmin = C-dC          ;
    float Cmax = C+dC          ;

    for(j=jn;  j!=ie; ++j) {

      if(      j == i       ) continue;

      if((*j).first < Cmin ) {jn=j; ++jn; continue;}
      if((*j).first > Cmax ) break;
      
      int m = (*j).second;

      const Trk::Surface*      SUR = m_SP[m]->sur();
      float                      L = m_L [m]       ;

      int k=0;
      for(; k!=NT; ++k) {
	if(  SUR  == SURt[k]  ) break;
	if(fabs(L-Lt[k]) < 20.) break;
      }

      if( k==NT ) {
	SURt[NT]= SUR    ;
	Lt  [NT]= L      ;
	if(++NT==4) break;
      }
    }
    if(NT < 2) continue;
    float Q = m_Im[n]-float(NT)*100.;
    newOneSeed(SPb,SP0,m_SP[n],Zob,Q);
  }
}

///////////////////////////////////////////////////////////////////
// Fill seeds
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ITK::fillSeeds ()
{
  m_fillOneSeeds = 0;

  std::multimap<float,InDet::SiSpacePointsProSeedITK*>::iterator 
    lf = m_mapOneSeeds.begin(),
    l  = m_mapOneSeeds.begin(),
    le = m_mapOneSeeds.end  ();
  
  if(l==le) return;

  SiSpacePointsProSeedITK* s;

  bool Q = (*l).first < -90000.;

  for(; l!=le; ++l) {

    float w = (*l).first ; if(Q && w > -90000. ) break;

    s       = (*l).second;
    if(!s->setQuality(w)) continue;
     
    if(i_seede!=l_seeds.end()) {
      s  = (*i_seede++);
      *s = *(*l).second;
    }
    else                  {
      s = new SiSpacePointsProSeedITK(*(*l).second);
      l_seeds.push_back(s);
      i_seede = l_seeds.end(); 
    }
    ++m_fillOneSeeds;
  }
}

///////////////////////////////////////////////////////////////////
// Pixels information
///////////////////////////////////////////////////////////////////

bool InDet::SiSpacePointsSeedMaker_ITK::pixInform(Trk::SpacePoint*const& sp,bool& barrel,int& layer,float* r)
{
  if(m_usePixelClusterCleanUp && !PixelSpacePointFilter(sp)) return false;

  const InDet::SiCluster*           cl = static_cast<const InDet::SiCluster*>(sp->clusterList().first); 
  const InDetDD::SiDetectorElement* de = cl->detectorElement(); 
  const PixelID* pixId = static_cast<const PixelID*>(de->getIdHelper());
  barrel = de->isBarrel();
  layer  = pixId->layer_disk(de->identify());
  const Amg::Transform3D& Tp = de->surface().transform();
  r[3] = float(Tp(0,2));
  r[4] = float(Tp(1,2));
  r[5] = float(Tp(2,2)); 
  return true;
}

///////////////////////////////////////////////////////////////////
// SCT information
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_ITK::sctInform(Trk::SpacePoint*const& sp,bool& barrel,int& layer,float* r) 
{
  const InDet::SiCluster* c0 = static_cast<const InDet::SiCluster*>(sp->clusterList().first );
  const InDet::SiCluster* c1 = static_cast<const InDet::SiCluster*>(sp->clusterList().second);
  const InDetDD::SiDetectorElement* d0 = c0->detectorElement(); 
  const InDetDD::SiDetectorElement* d1 = c1->detectorElement(); 

  const SCT_ID* sctId = static_cast<const SCT_ID*>(d0->getIdHelper());
  barrel  = d0->isBarrel();
  layer   = sctId->layer_disk(d0->identify());

  Amg::Vector2D lc0 = c0->localPosition();  
  Amg::Vector2D lc1 = c1->localPosition();  
  
  std::pair<Amg::Vector3D, Amg::Vector3D > e0 =
    (d0->endsOfStrip(InDetDD::SiLocalPosition(lc0.y(),lc0.x(),0.))); 
  std::pair<Amg::Vector3D, Amg::Vector3D > e1 =
    (d1->endsOfStrip(InDetDD::SiLocalPosition(lc1.y(),lc1.x(),0.))); 

  Amg::Vector3D b0 (e0.second-e0.first);
  Amg::Vector3D b1 (e1.second-e1.first);
  Amg::Vector3D d02(e0.first -e1.first);

  // b0
  r[ 3] = float(b0[0]);
  r[ 4] = float(b0[1]);
  r[ 5] = float(b0[2]);
  
  // b1
  r[ 6] = float(b1[0]);
  r[ 7] = float(b1[1]);
  r[ 8] = float(b1[2]);

  // r0-r2
  r[ 9] = float(d02[0]);
  r[10] = float(d02[1]);
  r[11] = float(d02[2]);

  // r0
  r[12] = float(e0.first[0])-m_xbeam[0];
  r[13] = float(e0.first[1])-m_ybeam[0];
  r[14] = float(e0.first[2])-m_zbeam[0];
}

///////////////////////////////////////////////////////////////////
// SCT information
///////////////////////////////////////////////////////////////////

bool InDet::SiSpacePointsSeedMaker_ITK::PixelSpacePointFilter(const Trk::SpacePoint* sp)
{

  const InDet::SiCluster*       clust  = dynamic_cast<const InDet::SiCluster*>   (sp->clusterList().first);
  const InDet::PixelCluster* pixclust  = dynamic_cast<const InDet::PixelCluster*>(sp->clusterList().first);
  
  // apply cuts only to non-split and non-ganged clusters

  if(pixclust->isSplit()!=true && clust->gangedPixel()!=true) 
    {
      const InDetDD::SiDetectorElement* de = clust ->detectorElement();
      const InDetDD::PixelModuleDesign* module(dynamic_cast<const InDetDD::PixelModuleDesign*>(&de->design()));
      double size_z= clust->width().z(); // in mm
      double size_phi= clust->width().phiR(); // in mm
      const double pitch_z= module->etaPitch(); 
      const double pitch_phi=module->phiPitch();
      if(de->isEndcap() && (m_usePixelClusterCleanUp_sizeZcutsE || m_usePixelClusterCleanUp_sizePhicutsE)) // cuts for disks go here
	{
	  // removing obviously anomalous clusters
	  if(m_usePixelClusterCleanUp_sizeZcutsE && size_z>m_pix_sizePhiCut*pitch_z) return false; // for end-cap use the same cut for sizeZ and size-Phi
	  if(m_usePixelClusterCleanUp_sizePhicutsE)
	    {
	      if(size_phi>m_pix_sizePhiCut*pitch_phi) return false;
	      if(size_phi/size_z>m_pix_sizePhi2sizeZCut_p0E/size_z+m_pix_sizePhi2sizeZCut_p1E) return false;
	    }
	}
      if(de->isBarrel() && (m_usePixelClusterCleanUp_sizeZcutsB || m_usePixelClusterCleanUp_sizePhicutsB)) // cuts for barrel go here
	{
	  if(m_usePixelClusterCleanUp_sizeZcutsB) 
	    {
	      const PixelID* p_pixelId = static_cast<const PixelID*> (de->getIdHelper());
	      Identifier id=de->identify();
	      int layer_num=p_pixelId->layer_disk(id);
	      
	      if(ClusterCleanupSizeZCuts(sp,size_phi,size_z,0.0,pitch_phi,pitch_z,de->thickness(),layer_num,m_Nsigma_clSizeZcut)==false) return false;
	    }

	  // removing obviously anomalous clusters
	  if(m_usePixelClusterCleanUp_sizePhicutsB)
	    {
	      if(size_phi>m_pix_sizePhiCut*pitch_phi) return false;
	      if(size_phi/size_z>m_pix_sizePhi2sizeZCut_p0B/size_z+m_pix_sizePhi2sizeZCut_p1B) return false;
	    }
	}
    }
  return true;
}

//---------------------------------------------------------------------------
// New methods to check cluster size compatibility; needed for ITK long barrel 
double InDet::SiSpacePointsSeedMaker_ITK::predictedClusterLength(const InDet::SiSpacePointForSeedITK* sp, double thickness, float seed_zvx) {

  double length=0.0;
  double tanTheta= fabs(sp->z()-seed_zvx) > 0.0 ? sp->radius()/fabs(sp->z()-seed_zvx) : 1000000.0;

  if(fabs(tanTheta)>0.0) length=thickness/fabs(tanTheta);

  return length;
}

double InDet::SiSpacePointsSeedMaker_ITK::predictedClusterLength(const Trk::SpacePoint* sp, double thickness, float seed_zvx) {
  double length=0.0;
  const double z_cl=(sp->globalPosition()).z();
  double tanTheta= fabs(z_cl-seed_zvx) > 0.0 ? sp->r()/fabs(z_cl-seed_zvx) : 1000000.0;

  if(fabs(tanTheta)>0.0) length=thickness/fabs(tanTheta);
  return length;
}

// this function checks is a particular cluster is close to chip boundary
bool InDet::SiSpacePointsSeedMaker_ITK::isAwayFromChipBoundary(const InDet::SiSpacePointForSeedITK* sp, double size_phi, double size_z, double pitch_phi, double pitch_z) {
  bool awayFromBoundary=true;

  const InDet::SiCluster* clust  = dynamic_cast<const InDet::SiCluster*>(sp->spacepoint->clusterList().first);
  const InDetDD::SiDetectorElement* de = clust->detectorElement();  
  const PixelID* p_pixelId = static_cast<const PixelID*>(de->getIdHelper());
  const InDet::PixelCluster* pixclust  = dynamic_cast<const InDet::PixelCluster*>(sp->spacepoint->clusterList().first);
  Identifier id_cl=pixclust->identify();
  const int cl_col_min=p_pixelId->eta_index(id_cl); // first pixel in the cluster: lower left corner of the box around cluster
  const int cl_row_min=p_pixelId->phi_index(id_cl); // first pixel in the cluster: lower left corner of the box around cluster

  if(cl_col_min==0) return false; // cluster startes at module boundary
  const int cl_size_z=(int)rint(size_z/pitch_z);
  const int cl_size_phi=(int)rint(size_phi/pitch_phi);
  const InDetDD::PixelModuleDesign* module(dynamic_cast<const InDetDD::PixelModuleDesign*>(&de->design()));
  const int n_cols=module->columns(); // this works
  const int n_rows=module->rows();    // this works

  if(cl_col_min+cl_size_z-1>=n_cols-1) return false;
  if(cl_row_min+cl_size_phi-1>=n_rows-1) return false;

  return awayFromBoundary;
}

bool InDet::SiSpacePointsSeedMaker_ITK::isAwayFromChipBoundary(const Trk::SpacePoint* sp, double size_phi, double size_z, double pitch_phi, double pitch_z) {
  bool awayFromBoundary=true;

  const InDet::SiCluster* clust  = dynamic_cast<const InDet::SiCluster*>(sp->clusterList().first);
  const InDetDD::SiDetectorElement* de = clust->detectorElement();  
  const PixelID* p_pixelId = static_cast<const PixelID*>(de->getIdHelper());
  const InDet::PixelCluster* pixclust  = dynamic_cast<const InDet::PixelCluster*>(sp->clusterList().first);
  Identifier id_cl=pixclust->identify();
  const int cl_col_min=p_pixelId->eta_index(id_cl); // first pixel in the cluster: lower left corner of the box around cluster
  const int cl_row_min=p_pixelId->phi_index(id_cl); // first pixel in the cluster: lower left corner of the box around cluster

  if(cl_col_min==0) return false; // cluster startes at module boundary
  const int cl_size_z=(int)rint(size_z/pitch_z);
  const int cl_size_phi=(int)rint(size_phi/pitch_phi);
  const InDetDD::PixelModuleDesign* module(dynamic_cast<const InDetDD::PixelModuleDesign*>(&de->design()));
  const int n_cols=module->columns(); // this works
  const int n_rows=module->rows();    // this works

  if(cl_col_min+cl_size_z-1>=n_cols-1) return false;
  if(cl_row_min+cl_size_phi-1>=n_rows-1) return false;

  return awayFromBoundary;
}



int InDet::SiSpacePointsSeedMaker_ITK::deltaSize(double sizeZ, double predictedSize, double pixel_pitch)
{
  return (int)rint((sizeZ-predictedSize)/pixel_pitch);
}


// this function applies cluster size cuts to seed links
bool InDet::SiSpacePointsSeedMaker_ITK::ClusterSizeCuts(const InDet::SiSpacePointForSeedITK* sp_low,const InDet::SiSpacePointForSeedITK* sp_high, const double z_seed)
{
  bool pass_cuts=true;

  if(m_useITKseedCuts_dSize || m_useITKseedCuts_dSize)
    {

      const InDet::SiCluster* clust_lo  = dynamic_cast<const InDet::SiCluster*>(sp_low->spacepoint->clusterList().first);
      const InDet::SiCluster* clust_up  = dynamic_cast<const InDet::SiCluster*>(sp_high->spacepoint->clusterList().first);
      const InDetDD::SiDetectorElement* de_lo = clust_lo->detectorElement();   // lower link
      const InDetDD::SiDetectorElement* de_up = clust_up->detectorElement(); // upper link

      if(de_lo->isPixel() && de_lo->isBarrel() && clust_lo->gangedPixel()!=true) // lower link is in pixel barrel
	{
	  // obtain pitch for both lower and upper clusters
	  // it might happen that different layers have different pitch
	  const InDetDD::PixelModuleDesign* module_lo(dynamic_cast<const InDetDD::PixelModuleDesign*>(&de_lo->design()));
	  const double pitchZ_lo= module_lo->etaPitch(); 
	  const double pitchPhi_lo= module_lo->phiPitch(); 
	  double sizeZ_lo= clust_lo->width().z(); // in mm
	  double sizePhi_lo= clust_lo->width().phiR(); // in mm
	  // applying dSize cuts to a lower cluster
	  if(m_useITKseedCuts_dSize)  
	    {
	      int delta=deltaSize(sizeZ_lo,predictedClusterLength(sp_low,de_lo->thickness(),z_seed),pitchZ_lo);
	      if(isAwayFromChipBoundary(sp_low,sizePhi_lo,sizeZ_lo,pitchPhi_lo,pitchZ_lo)) // lower cluster is away from chip boundary
		{
		  const PixelID* p_pixelId = static_cast<const PixelID*> (de_lo->getIdHelper());
		  Identifier id=de_lo->identify();
		  int layer_num=p_pixelId->layer_disk(id);
		  double eta= fabs(sp_low->z()-z_seed) > 0.0 ? -log(atan(sp_low->radius()/fabs(sp_low->z()-z_seed))/2.0) : 0.0;		  
		  double right_rms=parR_clSizeZcut[0][layer_num] + parR_clSizeZcut[1][layer_num]*fabs(eta);		  
		  double left_rms= parL_clSizeZcut[layer_num];
		  if(layer_num<2 && fabs(eta)>2.5) right_rms=(3-layer_num)*right_rms;
		  if(delta<0 && fabs(1.0*delta) > m_Nsigma_clSizeZcut*left_rms) return false;
		  if(delta>0 && fabs(1.0*delta) > m_Nsigma_clSizeZcut*right_rms) return false;
		}
	      else // bottom cluster is close to chip boundary
		{
		  if(delta>4) return false; // bottom cluster is too large for this seed (to be tuned)
		}
	    }
	  
	  // only makes sense to check upper link if lower link is in Pixel barrel
	  if(de_up->isBarrel() && de_up->isPixel() && clust_up->gangedPixel()!=true) // upper link is a barrel cluster
	    {

	      const InDetDD::PixelModuleDesign* module_up(dynamic_cast<const InDetDD::PixelModuleDesign*>(&de_up->design()));
	      const double pitchZ_up= module_up->etaPitch(); 
	      const double pitchPhi_up= module_up->phiPitch(); 
	      double sizeZ_up= clust_up->width().z(); // in mm
	      double sizePhi_up= clust_up->width().phiR(); // in mm

	      if(m_useITKseedCuts_dSize)
		{
		  int delta0=deltaSize(sizeZ_up,predictedClusterLength(sp_high,de_up->thickness(),z_seed),pitchZ_up);
		  
		  if(isAwayFromChipBoundary(sp_high,sizePhi_up,sizeZ_up,pitchPhi_up,pitchZ_up)) // middle cluster is away from chip boundary
		    {

		      const PixelID* p_pixelId = static_cast<const PixelID*> (de_up->getIdHelper());
		      Identifier id=de_up->identify();
		      int layer_num=p_pixelId->layer_disk(id);
		      double eta= fabs(sp_high->z()-z_seed) > 0.0 ? -log(atan(sp_high->radius()/fabs(sp_high->z()-z_seed))/2.0) : 0.0;		  
		      double right_rms=parR_clSizeZcut[0][layer_num] + parR_clSizeZcut[1][layer_num]*fabs(eta);		  
		      double left_rms= parL_clSizeZcut[layer_num];
		      if(layer_num<2 && fabs(eta)>2.5) right_rms=(3-layer_num)*right_rms;
		      if(delta0<0 && fabs(1.0*delta0) > m_Nsigma_clSizeZcut*left_rms) return false;
		      if(delta0>0 && fabs(1.0*delta0) > m_Nsigma_clSizeZcut*right_rms) return false;
		    }
		  else
		    {
		      if(delta0>4) return false; // middle cluster is too large for this seed (to be tuned)
		    }
		}

	      if(m_useITKseedCuts_sizeDiff)
		{
		  //both clusters are away from chip boundaries
		  if(isAwayFromChipBoundary(sp_low,sizePhi_lo,sizeZ_lo,pitchPhi_lo,pitchZ_lo) 
		     && isAwayFromChipBoundary(sp_high,sizePhi_up,sizeZ_up,pitchPhi_up,pitchZ_up))
		    {
		      const float pitch=std::min(pitchZ_lo,pitchZ_up); // for now, pick smaller pitch
		      const double delta_size=sizeZ_lo-sizeZ_up;
		      if(de_lo->isInnermostPixelLayer()) 
			{
			  if(delta_size<-15.0*pitch || delta_size>20.0*pitch) return false;
			}
		      else if(de_lo->isNextToInnermostPixelLayer()) 
			{
			  if(delta_size<-5.0*pitch || delta_size>10.0*pitch) return false;
			}
		      else if(fabs(delta_size)>5.0*pitch) return false;
		    }
		}
	    }	
	}
    }

  return pass_cuts;
}


// this function applies cluster size cuts during the space point formation stage.
bool InDet::SiSpacePointsSeedMaker_ITK::ClusterCleanupSizeZCuts(const Trk::SpacePoint* sp, const double sizePhi, const double sizeZ, const double zvx, 
								const double pitch_phi, const double pitch_z, const double thickness, const int layerNum, const float Nsigma_clus) 
{
  bool pass_cuts=true;
  // apply cuts only to clusters away from chip boundaries
  if(isAwayFromChipBoundary(sp,sizePhi,sizeZ,pitch_phi,pitch_z) && layerNum>-1 && layerNum<5) 
    {
      int delta=deltaSize(sizeZ,predictedClusterLength(sp,thickness,zvx),pitch_z);
      if(abs(delta)<5) return pass_cuts; // don't apply cuts if |dSize|<5 
// these cuts are tuned on ITK step-1 samples (single muon sample)
      double eta=fabs(sp->eta(zvx));
      // double left_rms= eta>1.8 ? 3.0 : std::max(-0.4+1.88*eta,1.0); // These numbers need to be re-tuned/checked for the particular layout
      float left_rms=parL_clSizeZ0cut[layerNum];
      if(layerNum==0)
	{
	  const float p_left=std::max(-1.15+2.77*eta,1.0);
	  left_rms= p_left<parL_clSizeZ0cut[layerNum] ? p_left : parL_clSizeZ0cut[layerNum];
	}
      if(layerNum==1)
	{
	  const float p_left=std::max(0.53+0.95*eta,1.0);
	  left_rms= p_left<parL_clSizeZ0cut[layerNum] ? p_left : parL_clSizeZ0cut[layerNum];
	}
      float right_rms=parR_clSizeZ0cut[0][layerNum]+parR_clSizeZ0cut[1][layerNum]*eta+parR_clSizeZ0cut[2][layerNum]*eta*eta;
      if(delta<0 && fabs(1.0*delta)>Nsigma_clus*left_rms) return false; // cluster is too small, reject
      else if(delta>0 && 1.0*delta>Nsigma_clus*right_rms) return false; // clsuter is too large, reject
    }
  return pass_cuts;
}
