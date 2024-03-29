/*
  Copyright (C) 2002-2023 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//   Implementation file for class SiSpacePointsSeedMaker_Trigger
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// AlgTool used for TRT_DriftCircleOnTrack object production
///////////////////////////////////////////////////////////////////
// Version 1.0 21/04/2004 I.Gavrilenko
///////////////////////////////////////////////////////////////////

#include "SiSpacePointsSeedTool_xk/SiSpacePointsSeedMaker_Trigger.h"

#include <cmath>

#include <iomanip>
#include <ostream>

///////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////

InDet::SiSpacePointsSeedMaker_Trigger::SiSpacePointsSeedMaker_Trigger
(const std::string& t, const std::string& n, const IInterface* p)
  : base_class(t, n, p)
{
}

///////////////////////////////////////////////////////////////////
// Initialisation
///////////////////////////////////////////////////////////////////

StatusCode InDet::SiSpacePointsSeedMaker_Trigger::initialize()
{
  StatusCode sc = AlgTool::initialize();

  ATH_CHECK(m_spacepointsPixel.initialize(m_pixel));
  ATH_CHECK(m_spacepointsSCT.initialize(m_sct));
  ATH_CHECK(m_spacepointsOverlap.initialize(m_useOverlap));

  // Get beam geometry
  //
  ATH_CHECK(m_beamSpotKey.initialize());

  ATH_CHECK(m_fieldCondObjInputKey.initialize());

  // Build framework
  //
  buildFrameWork();

  // Get output print level
  //
  m_outputlevel = msg().level()-MSG::DEBUG;
  if (m_outputlevel<=0) {
    EventData data;
    initializeEventData(data);
    data.nprint=0;
    dump(data, msg(MSG::DEBUG));
  }

  m_initialized = true;

  return sc;
}

///////////////////////////////////////////////////////////////////
// Finalize
///////////////////////////////////////////////////////////////////

StatusCode InDet::SiSpacePointsSeedMaker_Trigger::finalize()
{
  return AlgTool::finalize();
}

///////////////////////////////////////////////////////////////////
// Initialize tool for new event 
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_Trigger::newEvent(const EventContext& ctx, EventData& data, int) const
{
  if (not data.initialized) initializeEventData(data);

  data.trigger = false;
  if (!m_pixel && !m_sct) return;
  erase(data);
  buildBeamFrameWork(data);

  double f[3], gP[3] ={10.,10.,0.};

  MagField::AtlasFieldCache    fieldCache;

  // Get field cache object
  SG::ReadCondHandle<AtlasFieldCacheCondObj> readHandle{m_fieldCondObjInputKey, ctx};
  const AtlasFieldCacheCondObj* fieldCondObj{*readHandle};
  if (fieldCondObj == nullptr) {
    ATH_MSG_ERROR("SiSpacePointsSeedMaker_Trigger: Failed to retrieve AtlasFieldCacheCondObj with key " << m_fieldCondObjInputKey.key());
    return; 
  }
  fieldCondObj->getInitializedCache (fieldCache);

  if (fieldCache.solenoidOn()) {
    fieldCache.getFieldZR(gP, f);

    data.K = 2./(300.*f[2]);
  } else {
    data.K = 2./(300.* 5.);
  }

  data.i_spforseed = data.l_spforseed.begin();

  float irstep = 1.f/m_r_rstep;
  int   irmax  = m_r_size-1;

  // Get pixels space points containers from store gate 
  //
  if (m_pixel) {

    SG::ReadHandle<SpacePointContainer> spacepointsPixel{m_spacepointsPixel, ctx};
    if (spacepointsPixel.isValid()) {

      for (const SpacePointCollection* spc: *spacepointsPixel) {
        for (const Trk::SpacePoint* sp: *spc) {

          float r = sp->r();
          if (r<0. || r>=m_r_rmax) continue;

          InDet::SiSpacePointForSeed* sps = newSpacePoint(data, sp);

          int ir = static_cast<int>(sps->radius()*irstep);
          if (ir>irmax) ir = irmax;
          data.r_Sorted[ir].push_back(sps);
          ++data.r_map[ir];
          if (data.r_map[ir]==1) data.r_index[data.nr++] = ir;
          ++data.ns;
        }
      }
    }
  }

  // Get sct space points containers from store gate 
  //
  if (m_sct) {

    SG::ReadHandle<SpacePointContainer> spacepointsSCT{m_spacepointsSCT, ctx};
    if (spacepointsSCT.isValid()) {

      for (const SpacePointCollection* spc: *spacepointsSCT) {
        for (const Trk::SpacePoint* sp: *spc) {

          float r = sp->r();
          if (r<0. || r>=m_r_rmax) continue;

          InDet::SiSpacePointForSeed* sps = newSpacePoint(data, sp);

          int   ir = static_cast<int>(sps->radius()*irstep);
          if (ir>irmax) ir = irmax;
          data.r_Sorted[ir].push_back(sps);
          ++data.r_map[ir];
          if (data.r_map[ir]==1) data.r_index[data.nr++] = ir;
          ++data.ns;
        }
      }
    }

    // Get sct overlap space points containers from store gate 
    //
    if (m_useOverlap) {

      SG::ReadHandle<SpacePointOverlapCollection> spacepointsOverlap{m_spacepointsOverlap, ctx};
      if (spacepointsOverlap.isValid()) {
 
        for (const Trk::SpacePoint* sp: *spacepointsOverlap) {

          float r = sp->r();
          if (r<0. || r>=m_r_rmax) continue;

          InDet::SiSpacePointForSeed* sps = newSpacePoint(data, sp);

          int ir = static_cast<int>(sps->radius()*irstep);
          if (ir>irmax) ir = irmax;
          data.r_Sorted[ir].push_back(sps);
          ++data.r_map[ir];
          if (data.r_map[ir]==1) data.r_index[data.nr++] = ir;
          ++data.ns;
        }
      }
    }
  }
  fillLists(data);
}

///////////////////////////////////////////////////////////////////
// Initialize tool for new region
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_Trigger::newRegion
(const EventContext& ctx, EventData& data, const std::vector<IdentifierHash>& vPixel, const std::vector<IdentifierHash>& vSCT) const
{
  if (not data.initialized) initializeEventData(data);

  data.trigger = false;
  if (!m_pixel && !m_sct) return;
  erase(data);

  buildBeamFrameWork(data);

  double f[3], gP[3] ={10.,10.,0.};

  MagField::AtlasFieldCache    fieldCache;

  // Get field cache object
  SG::ReadCondHandle<AtlasFieldCacheCondObj> readHandle{m_fieldCondObjInputKey, ctx};
  const AtlasFieldCacheCondObj* fieldCondObj{*readHandle};
  if (fieldCondObj == nullptr) {
    ATH_MSG_ERROR("SiSpacePointsSeedMaker_Trigger: Failed to retrieve AtlasFieldCacheCondObj with key " << m_fieldCondObjInputKey.key());
    return;
  }
  fieldCondObj->getInitializedCache (fieldCache);

  if (fieldCache.solenoidOn()) {
    fieldCache.getFieldZR(gP, f);

    data.K = 2./(300.*f[2]);
  } else {
    data.K = 2./(300.* 5.);
  }

  data.i_spforseed = data.l_spforseed.begin();

  float irstep = 1.f/m_r_rstep;
  int   irmax  = m_r_size-1;

  // Get pixels space points containers from store gate 
  //
  if (m_pixel && !vPixel.empty()) {

    SG::ReadHandle<SpacePointContainer> spacepointsPixel{m_spacepointsPixel, ctx};
    if (spacepointsPixel.isValid()) {

      // Loop through all trigger collections
      //
      for (const IdentifierHash& l: vPixel) {
        const auto *w =  spacepointsPixel->indexFindPtr(l);
        if (w==nullptr) continue;
        for (const Trk::SpacePoint* sp: *w) {
          float r = sp->r();
          if (r<0. || r>=m_r_rmax) continue;
          InDet::SiSpacePointForSeed* sps = newSpacePoint(data, sp);
          int ir = static_cast<int>(sps->radius()*irstep);
          if (ir>irmax) ir = irmax;
          data.r_Sorted[ir].push_back(sps);
          ++data.r_map[ir];
          if (data.r_map[ir]==1) data.r_index[data.nr++] = ir;
          ++data.ns;
        }
      }
    }
  }

  // Get sct space points containers from store gate 
  //
  if (m_sct && !vSCT.empty()) {

    SG::ReadHandle<SpacePointContainer> spacepointsSCT{m_spacepointsSCT, ctx};
    if (spacepointsSCT.isValid()) {

      // Loop through all trigger collections
      //
      for (const IdentifierHash& l: vSCT) {
        const auto *w = spacepointsSCT->indexFindPtr(l);
        if (w==nullptr) continue;
        for (const Trk::SpacePoint* sp: *w) {
          float r = sp->r();
          if (r<0. || r>=m_r_rmax) continue;
          InDet::SiSpacePointForSeed* sps = newSpacePoint(data, sp);
          int ir = static_cast<int>(sps->radius()*irstep);
          if (ir>irmax) ir = irmax;
          data.r_Sorted[ir].push_back(sps);
          ++data.r_map[ir];
          if (data.r_map[ir]==1) data.r_index[data.nr++] = ir;
          ++data.ns;
        }
      }
    }
  }
  fillLists(data);
}

///////////////////////////////////////////////////////////////////
// Initialize tool for new region
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_Trigger::newRegion
(const EventContext& ctx, EventData& data,
 const std::vector<IdentifierHash>& vPixel, const std::vector<IdentifierHash>& vSCT,
 const IRoiDescriptor& IRD) const
{
  if (not data.initialized) initializeEventData(data);

  newRegion(ctx, data, vPixel, vSCT);

  data.trigger = true;

  double dzdrmin = 1./tan(2.*atan(exp(-IRD.etaMinus())));
  double dzdrmax = 1./tan(2.*atan(exp(-IRD.etaPlus ())));
 
  data.zminB        = IRD.zedMinus()-data.zbeam[0]; // min bottom Z
  data.zmaxB        = IRD.zedPlus ()-data.zbeam[0]; // max bottom Z
  data.zminU        = data.zminB+550.f*dzdrmin;
  data.zmaxU        = data.zmaxB+550.f*dzdrmax;
  double fmax    = IRD.phiPlus ();
  double fmin    = IRD.phiMinus();
  if (fmin > fmax) fmin-=(2.*M_PI);
  data.ftrig        = (fmin+fmax)*.5;
  data.ftrigW       = (fmax-fmin)*.5;
}

///////////////////////////////////////////////////////////////////
// Methods to initilize different strategies of seeds production
// with two space points with or without vertex constraint
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_Trigger::find2Sp(EventData& data, const std::list<Trk::Vertex>& lv) const
{
  if (not data.initialized) initializeEventData(data);

  int mode = 0;
  if (lv.begin()!=lv.end()) mode = 1;
  bool newv = newVertices(data, lv);
  
  if (newv || !data.state || data.nspoint!=2 || data.mode!=mode || data.nlist) {

    data.i_seede = data.l_seeds.begin();
    data.state   = 1;
    data.nspoint = 2;
    data.nlist   = 0;
    data.mode    = mode;
    data.endlist = true;
    data.fvNmin  = 0;
    data.fNmin   = 0;
    data.zMin    = 0;
    production2Sp(data);
  }
  data.i_seed = data.l_seeds.begin();
  
  if (m_outputlevel<=0) {
    data.nprint=1;
    dump(data, msg(MSG::DEBUG));
  }
}

///////////////////////////////////////////////////////////////////
// Methods to initilize different strategies of seeds production
// with three space points with or without vertex constraint
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_Trigger::find3Sp(const EventContext&, EventData& data, const std::list<Trk::Vertex>& lv) const
{
  if (not data.initialized) initializeEventData(data);

  int mode = 2;
  if (lv.begin()!=lv.end()) mode = 3;
  bool newv = newVertices(data, lv);

  if (newv || !data.state || data.nspoint!=3 || data.mode!=mode || data.nlist) {

    data.i_seede = data.l_seeds.begin();
    data.state   = 1;
    data.nspoint = 3;
    data.nlist   = 0;
    data.mode    = mode;
    data.endlist = true;
    data.fvNmin  = 0;
    data.fNmin   = 0;
    data.zMin    = 0;
    production3Sp(data);
  }
  data.i_seed = data.l_seeds.begin();
  data.seed  = data.mapSeeds.begin();
  data.seede = data.mapSeeds.end  ();

  if (m_outputlevel<=0) {
    data.nprint=1;
    dump(data, msg(MSG::DEBUG));
  }
}

void InDet::SiSpacePointsSeedMaker_Trigger::find3Sp(const EventContext& ctx, EventData& data, const std::list<Trk::Vertex>& lv, const double*) const
{
  find3Sp(ctx, data, lv);
}

///////////////////////////////////////////////////////////////////
// Methods to initilize different strategies of seeds production
// with variable number space points with or without vertex constraint
// Variable means (2,3,4,....) any number space points
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_Trigger::findVSp(const EventContext&, EventData& data, const std::list<Trk::Vertex>& lv) const
{
  if (not data.initialized) initializeEventData(data);

  int mode = 5;
  if (lv.begin()!=lv.end()) mode = 6;
  bool newv = newVertices(data, lv);
  
  if (newv || !data.state || data.nspoint!=4 || data.mode!=mode || data.nlist) {

    data.i_seede = data.l_seeds.begin();
    data.state   = 1;
    data.nspoint = 4;
    data.nlist   = 0;
    data.mode    = mode;
    data.endlist = true;
    data.fvNmin  = 0;
    data.fNmin   = 0;
    data.zMin    = 0;
    production3Sp(data);
  }
  data.i_seed = data.l_seeds.begin();
  data.seed  = data.mapSeeds.begin();
  data.seede = data.mapSeeds.end  ();

  if (m_outputlevel<=0) {
    data.nprint=1;
    dump(data, msg(MSG::DEBUG));
  }
}

///////////////////////////////////////////////////////////////////
// Dumps relevant information into the MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::SiSpacePointsSeedMaker_Trigger::dump(EventData& data, MsgStream& out) const
{
  if (not data.initialized) initializeEventData(data);

  if (data.nprint) return dumpEvent(data, out);
  return dumpConditions(data, out);
}

///////////////////////////////////////////////////////////////////
// Dumps conditions information into the MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::SiSpacePointsSeedMaker_Trigger::dumpConditions(EventData& data, MsgStream& out) const
{
  int n = 42-m_spacepointsPixel.key().size();
  std::string s2; for (int i=0; i<n; ++i) s2.append(" "); s2.append("|");
  n     = 42-m_spacepointsSCT.key().size();
  std::string s3; for (int i=0; i<n; ++i) s3.append(" "); s3.append("|");
  n     = 42-m_spacepointsOverlap.key().size();
  std::string s4; for (int i=0; i<n; ++i) s4.append(" "); s4.append("|");
  n     = 42-m_beamSpotKey.key().size();
  std::string s5; for (int i=0; i<n; ++i) s5.append(" "); s5.append("|");


  out<<"|---------------------------------------------------------------------|"
     <<endmsg;
  out<<"| Pixel    space points   | "<<m_spacepointsPixel.key() <<s2
     <<endmsg;
  out<<"| SCT      space points   | "<<m_spacepointsSCT.key() <<s3
     <<endmsg;
  out<<"| Overlap  space points   | "<<m_spacepointsOverlap.key() <<s4
     <<endmsg;
  out<<"| BeamConditionsService   | "<<m_beamSpotKey.key()<<s5
     <<endmsg;
  out<<"| usePixel                | "
     <<std::setw(12)<<m_pixel 
     <<"                              |"<<endmsg;
  out<<"| useSCT                  | "
     <<std::setw(12)<<m_sct 
     <<"                              |"<<endmsg;
  out<<"| maxSize                 | "
     <<std::setw(12)<<m_maxsize 
     <<"                              |"<<endmsg;
  out<<"| maxSizeSP               | "
     <<std::setw(12)<<m_maxsizeSP
     <<"                              |"<<endmsg;
  out<<"| pTmin  (mev)            | "
     <<std::setw(12)<<std::setprecision(5)<<m_ptmin
     <<"                              |"<<endmsg;
  out<<"| |rapidity|          <=  | " 
     <<std::setw(12)<<std::setprecision(5)<<m_rapcut
     <<"                              |"<<endmsg;
  out<<"| max radius SP           | "
     <<std::setw(12)<<std::setprecision(5)<<m_r_rmax 
     <<"                              |"<<endmsg;
  out<<"| radius step             | "
     <<std::setw(12)<<std::setprecision(5)<<m_r_rstep
     <<"                              |"<<endmsg;
  out<<"| min Z-vertex position   | "
     <<std::setw(12)<<std::setprecision(5)<<m_zmin
     <<"                              |"<<endmsg;
  out<<"| max Z-vertex position   | "
     <<std::setw(12)<<std::setprecision(5)<<m_zmax
     <<"                              |"<<endmsg;
  out<<"| min radius first  SP(3) | "
     <<std::setw(12)<<std::setprecision(5)<<m_r1min
     <<"                              |"<<endmsg;
  out<<"| min radius second SP(3) | "
     <<std::setw(12)<<std::setprecision(5)<<m_r2min
     <<"                              |"<<endmsg;
  out<<"| min radius last   SP(3) | "
     <<std::setw(12)<<std::setprecision(5)<<m_r3min
     <<"                              |"<<endmsg;
  out<<"| max radius first  SP(3) | "
     <<std::setw(12)<<std::setprecision(4)<<m_r1max
     <<"                              |"<<endmsg;
  out<<"| max radius second SP(3) | "
     <<std::setw(12)<<std::setprecision(5)<<m_r2max
     <<"                              |"<<endmsg;
  out<<"| max radius last   SP(3) | "
     <<std::setw(12)<<std::setprecision(5)<<m_r3max
     <<"                              |"<<endmsg;
  out<<"| min radius first  SP(2) | "
     <<std::setw(12)<<std::setprecision(5)<<m_r1minv
     <<"                              |"<<endmsg;
  out<<"| min radius second SP(2) | "
     <<std::setw(12)<<std::setprecision(5)<<m_r2minv
     <<"                              |"<<endmsg;
  out<<"| max radius first  SP(2) | "
     <<std::setw(12)<<std::setprecision(5)<<m_r1maxv
     <<"                              |"<<endmsg;
  out<<"| max radius second SP(2) | "
     <<std::setw(12)<<std::setprecision(5)<<m_r2maxv
     <<"                              |"<<endmsg;
  out<<"| min space points dR     | "
     <<std::setw(12)<<std::setprecision(5)<<m_drmin
     <<"                              |"<<endmsg;
  out<<"| max space points dR     | "
     <<std::setw(12)<<std::setprecision(5)<<m_drmax
     <<"                              |"<<endmsg;
  out<<"| max dZ    impact        | "
     <<std::setw(12)<<std::setprecision(5)<<m_dzver 
     <<"                              |"<<endmsg;
  out<<"| max dZ/dR impact        | "
     <<std::setw(12)<<std::setprecision(5)<<m_dzdrver 
     <<"                              |"<<endmsg;
  out<<"| max       impact        | "
     <<std::setw(12)<<std::setprecision(5)<<m_diver
     <<"                              |"<<endmsg;
  out<<"| max       impact pps    | "
     <<std::setw(12)<<std::setprecision(5)<<m_diverpps
     <<"                              |"<<endmsg;
  out<<"| max       impact sss    | "
     <<std::setw(12)<<std::setprecision(5)<<m_diversss
     <<"                              |"<<endmsg;
  out<<"|---------------------------------------------------------------------|"
     <<endmsg;
  out<<"| Beam X center           | "
     <<std::setw(12)<<std::setprecision(5)<<data.xbeam[0]
     <<"                              |"<<endmsg;
  out<<"| Beam Y center           | "
     <<std::setw(12)<<std::setprecision(5)<<data.ybeam[0]
     <<"                              |"<<endmsg;
  out<<"| Beam Z center           | "
     <<std::setw(12)<<std::setprecision(5)<<data.zbeam[0]
     <<"                              |"<<endmsg;
  out<<"| Beam X-axis direction   | "
     <<std::setw(12)<<std::setprecision(5)<<data.xbeam[1]
     <<std::setw(12)<<std::setprecision(5)<<data.xbeam[2]
     <<std::setw(12)<<std::setprecision(5)<<data.xbeam[3]
     <<"      |"<<endmsg;
  out<<"| Beam Y-axis direction   | "
     <<std::setw(12)<<std::setprecision(5)<<data.ybeam[1]
     <<std::setw(12)<<std::setprecision(5)<<data.ybeam[2]
     <<std::setw(12)<<std::setprecision(5)<<data.ybeam[3]
     <<"      |"<<endmsg;
  out<<"| Beam Z-axis direction   | "
     <<std::setw(12)<<std::setprecision(5)<<data.zbeam[1]
     <<std::setw(12)<<std::setprecision(5)<<data.zbeam[2]
     <<std::setw(12)<<std::setprecision(5)<<data.zbeam[3]
     <<"      |"<<endmsg;
  out<<"|---------------------------------------------------------------------|"
     <<endmsg;
  return out;
}

///////////////////////////////////////////////////////////////////
// Dumps event information into the MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::SiSpacePointsSeedMaker_Trigger::dumpEvent(EventData& data, MsgStream& out) 
{
  out<<"|---------------------------------------------------------------------|"
     <<endmsg;
  out<<"| data.ns                    | "
     <<std::setw(12)<<data.ns
     <<"                              |"<<endmsg;
  out<<"| data.nsaz                  | "
     <<std::setw(12)<<data.nsaz
     <<"                              |"<<endmsg;
  out<<"| data.nsazv                 | "
     <<std::setw(12)<<data.nsazv
     <<"                              |"<<endmsg;
  out<<"| seeds                   | "
     <<std::setw(12)<<data.l_seeds.size()
     <<"                              |"<<endmsg;
  out<<"|---------------------------------------------------------------------|"
     <<endmsg;
  return out;
}

///////////////////////////////////////////////////////////////////
// Find next set space points
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_Trigger::findNext(EventData& data) const
{
  if (data.endlist) return;

  data.i_seede = data.l_seeds.begin();

  if      (data.mode==0 || data.mode==1) production2Sp(data);
  else if (data.mode==2 || data.mode==3) production3Sp(data);
  else if (data.mode==5 || data.mode==6) production3Sp(data);

  data.i_seed = data.l_seeds.begin();
  data.seed  = data.mapSeeds.begin();
  data.seede = data.mapSeeds.end  ();
  ++data.nlist;
}                       

///////////////////////////////////////////////////////////////////
// New and old list vertices comparison
///////////////////////////////////////////////////////////////////

bool InDet::SiSpacePointsSeedMaker_Trigger::newVertices(EventData& data, const std::list<Trk::Vertex>& lV) const
{
  unsigned int s1 = data.l_vertex.size();
  unsigned int s2 = lV.size();

  if (s1==0 && s2==0) return false;

  data.l_vertex.clear();
  
  for (const Trk::Vertex& v : lV) {
    data.l_vertex.insert(static_cast<float>(v.position().z()));
    if (data.l_vertex.size() >= m_maxNumberVertices) break;
  }
  return false;
}

///////////////////////////////////////////////////////////////////
// Initiate frame work for seed generator
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_Trigger::buildFrameWork() 
{
  m_ptmin     = std::abs(m_ptmin);
  if (m_ptmin < 100.) m_ptmin = 100.;
  m_rapcut    = std::abs(m_rapcut);
  m_dzdrmax   = 1.f/std::tan(2.f*std::atan(exp(-m_rapcut)));
  m_dzdrmin   =-m_dzdrmax;
  m_r3max     = m_r_rmax;
  m_COF       =  134*.05*9.;
  m_ipt       = 1.f/std::abs(.9f*m_ptmin);
  m_ipt2      = m_ipt*m_ipt;

  // Build radius sorted containers
  //
  m_r_size = static_cast<int>((m_r_rmax+.1f)/m_r_rstep);

  // Build radius-azimuthal sorted containers
  //
  constexpr float pi2 = 2.*M_PI;
  const int NFmax = SizeRF;
  const float sFmax = static_cast<float>(NFmax)/pi2;
  const float sFmin = 100./60.;

  m_sF = m_ptmin /60.;
  if (m_sF > sFmax ) m_sF = sFmax;
  else if (m_sF < sFmin) m_sF = sFmin;
  m_fNmax = static_cast<int>(pi2*m_sF);
  if (m_fNmax >=NFmax) m_fNmax = NFmax-1;

  // Build radius-azimuthal-Z sorted containers for Z-vertices
  //
  const int NFtmax = 100;
  const float sFvmax = static_cast<float>(NFtmax)/pi2;
  m_sFv = m_ptmin/120.f;
  if (m_sFv>sFvmax) m_sFv = sFvmax;
  m_fvNmax = static_cast<int>(pi2*m_sFv);
  if (m_fvNmax>=NFtmax) m_fvNmax = NFtmax-1;

  // Build maps for radius-azimuthal-Z sorted collections 
  //
  for (int f=0; f<=m_fNmax; ++f) {
    int fb = f-1;
    if (fb<0) fb=m_fNmax; 
    int ft = f+1;
    if (ft>m_fNmax) ft=0;
    
    // For each azimuthal region loop through all Z regions
    //
    for (int z=0; z<SizeZ; ++z) {
      int a        = f *SizeZ+z;
      int b        = fb*SizeZ+z;
      int c        = ft*SizeZ+z;
      m_rfz_b [a]    = 3; m_rfz_t [a]    = 3;
      m_rfz_ib[a][0] = a; m_rfz_it[a][0] = a;
      m_rfz_ib[a][1] = b; m_rfz_it[a][1] = b;
      m_rfz_ib[a][2] = c; m_rfz_it[a][2] = c;
      if (z==5) {

        m_rfz_t [a]    = 9;
        m_rfz_it[a][3] = a+1;
        m_rfz_it[a][4] = b+1;
        m_rfz_it[a][5] = c+1;
        m_rfz_it[a][6] = a-1;
        m_rfz_it[a][7] = b-1;
        m_rfz_it[a][8] = c-1;
      } else if (z> 5) {

        m_rfz_b [a]    = 6;
        m_rfz_ib[a][3] = a-1;
        m_rfz_ib[a][4] = b-1;
        m_rfz_ib[a][5] = c-1;

        if (z<10) {

          m_rfz_t [a]    = 6;
          m_rfz_it[a][3] = a+1;
          m_rfz_it[a][4] = b+1;
          m_rfz_it[a][5] = c+1;
        }
      } else {

        m_rfz_b [a]    = 6;
        m_rfz_ib[a][3] = a+1;
        m_rfz_ib[a][4] = b+1;
        m_rfz_ib[a][5] = c+1;

        if (z>0) {

          m_rfz_t [a]    = 6;
          m_rfz_it[a][3] = a-1;
          m_rfz_it[a][4] = b-1;
          m_rfz_it[a][5] = c-1;
        }
      }

      if (z==3) {
        m_rfz_b[a]      = 9;
        m_rfz_ib[a][6] = a+2;
        m_rfz_ib[a][7] = b+2;
        m_rfz_ib[a][8] = c+2;
      } else if (z==7) {
        m_rfz_b[a]      = 9;
        m_rfz_ib[a][6] = a-2;
        m_rfz_ib[a][7] = b-2;
        m_rfz_ib[a][8] = c-2;
      }
    }
  }

  // Build maps for radius-azimuthal-Z sorted collections for Z
  //
  for (int f=0; f<=m_fvNmax; ++f) {

    int fb = f-1;
    if (fb<0) fb=m_fvNmax;
    int ft = f+1;
    if (ft>m_fvNmax) ft=0;
    
    // For each azimuthal region loop through central Z regions
    //
    for (int z=0; z<SizeZV; ++z) {
      
      int a  = f *SizeZV+z;
      int b  = fb*SizeZV+z;
      int c  = ft*SizeZV+z;
      m_rfzv_n[a]    = 3;
      m_rfzv_i[a][0] = a;
      m_rfzv_i[a][1] = b;
      m_rfzv_i[a][2] = c;
      if (z>1) {
        m_rfzv_n[a]    = 6;
        m_rfzv_i[a][3] = a-1;
        m_rfzv_i[a][4] = b-1;
        m_rfzv_i[a][5] = c-1;
      } else if (z<1) {
        m_rfzv_n[a]    = 6;
        m_rfzv_i[a][3] = a+1;
        m_rfzv_i[a][4] = b+1;
        m_rfzv_i[a][5] = c+1;
      }
    }
  }
}

///////////////////////////////////////////////////////////////////
// Initiate beam frame work for seed generator
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_Trigger::buildBeamFrameWork(EventData& data) const
{ 
  SG::ReadCondHandle<InDet::BeamSpotData> beamSpotHandle{m_beamSpotKey};

  const Amg::Vector3D& cb = beamSpotHandle->beamPos();
  double tx = std::tan(beamSpotHandle->beamTilt(0));
  double ty = std::tan(beamSpotHandle->beamTilt(1));

  double ph   = std::atan2(ty,tx);
  double th   = std::acos(1./std::sqrt(1.+tx*tx+ty*ty));
  double sint = sin(th);
  double cost = cos(th);
  double sinp = sin(ph);
  double cosp = cos(ph);
  
  data.xbeam[0] =  static_cast<float>(cb.x());
  data.xbeam[1] =  static_cast<float>(cost*cosp*cosp+sinp*sinp);
  data.xbeam[2] =  static_cast<float>(cost*sinp*cosp-sinp*cosp);
  data.xbeam[3] = -static_cast<float>(sint*cosp               );
  
  data.ybeam[0] =  static_cast<float>(cb.y());
  data.ybeam[1] =  static_cast<float>(cost*cosp*sinp-sinp*cosp);
  data.ybeam[2] =  static_cast<float>(cost*sinp*sinp+cosp*cosp);
  data.ybeam[3] = -static_cast<float>(sint*sinp               );
  
  data.zbeam[0] =  static_cast<float>(cb.z());
  data.zbeam[1] =  static_cast<float>(sint*cosp);
  data.zbeam[2] =  static_cast<float>(sint*sinp);
  data.zbeam[3] =  static_cast<float>(cost);
}

///////////////////////////////////////////////////////////////////
// Initiate beam frame work for seed generator
///////////////////////////////////////////////////////////////////

void  InDet::SiSpacePointsSeedMaker_Trigger::convertToBeamFrameWork
(EventData& data, const Trk::SpacePoint*const& sp, float* r) 
{ 
  r[0] = static_cast<float>(sp->globalPosition().x())-data.xbeam[0];
  r[1] = static_cast<float>(sp->globalPosition().y())-data.ybeam[0];
  r[2] = static_cast<float>(sp->globalPosition().z())-data.zbeam[0];
}
   
///////////////////////////////////////////////////////////////////
// Initiate space points seed maker
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_Trigger::fillLists(EventData& data) const
{
  constexpr float pi2 = 2.*M_PI;
  std::vector<InDet::SiSpacePointForSeed*>::iterator r;
  
  for (int i=0; i!= m_r_size;  ++i) {

    if (!data.r_map[i]) continue;
    r = data.r_Sorted[i].begin();

    while (r!=data.r_Sorted[i].end()) {
      
      // Azimuthal angle sort
      //
      float F = (*r)->phi();
      if (F<0.) F+=pi2;

      int f = static_cast<int>(F*m_sF);
      if (f < 0) f = m_fNmax;
      else if (f > m_fNmax) f = 0;

      int z;
      float Z = (*r)->z();

      // Azimuthal angle and Z-coordinate sort
      //
      if (Z>0.) {Z < 250.? z=5 : z=6;}
      else      {Z >-250.? z=5 : z=4;}
      int n = f*SizeZ+z;
      ++data.nsaz;
      data.rfz_Sorted[n].push_back(*r);
      if (!data.rfz_map[n]++) data.rfz_index[data.nrfz++] = n;

      if ((*r)->spacepoint->clusterList().second == 0 && z>=3 && z<=7) { 
        z<=4 ? z=0 : z>=6 ? z=2 : z=1;
 
        // Azimuthal angle and Z-coordinate sort for fast vertex search
        //
        f = static_cast<int>(F*m_sFv);
        if (f < 0) f+=m_fvNmax;
        else if (f > m_fvNmax) f-=m_fvNmax;

        n = f*3+z;
        ++data.nsazv;
        data.rfzv_Sorted[n].push_back(*r);
        if (!data.rfzv_map[n]++) data.rfzv_index[data.nrfzv++] = n;
      }
      data.r_Sorted[i].erase(r++);
    }
    data.r_map[i] = 0;
  }
  data.nr    = 0;
  data.state = 0;
}
 
///////////////////////////////////////////////////////////////////
// Erase space point information
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_Trigger::erase(EventData& data) 
{
  for (int i=0; i!=data.nr;    ++i) {
    int n = data.r_index[i];
    data.r_map[n] = 0;
    data.r_Sorted[n].clear();
  }

  for (int i=0; i!=data.nrfz;  ++i) {
    int n = data.rfz_index[i];
    data.rfz_map[n] = 0;
    data.rfz_Sorted[n].clear();
  }

  for (int i=0; i!=data.nrfzv; ++i) {
    int n = data.rfzv_index[i];
    data.rfzv_map[n] = 0;
    data.rfzv_Sorted[n].clear();
  }
  data.state = 0;
  data.ns    = 0;
  data.nsaz  = 0;
  data.nsazv = 0;
  data.nr    = 0;
  data.nrfz  = 0;
  data.nrfzv = 0;
}

///////////////////////////////////////////////////////////////////
// 2 space points seeds production
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_Trigger::production2Sp(EventData& data) const
{
  if (data.nsazv<2) return;

  std::vector<InDet::SiSpacePointForSeed*>::iterator r0,r0e,r,re;
  int nseed = 0;

  // Loop thorugh all azimuthal regions
  //
  for (int f=data.fvNmin; f<=m_fvNmax; ++f) {

    // For each azimuthal region loop through Z regions
    //
    int z = 0;
    if (!data.endlist) z = data.zMin;
    for (; z!=3; ++z) {
      
      int a  = f*3+z;
      if (!data.rfzv_map[a]) continue;
      r0  = data.rfzv_Sorted[a].begin();
      r0e = data.rfzv_Sorted[a].end  ();

      if (!data.endlist) {
        r0 = data.rMin;
        data.endlist = true;
      }

      // Loop through trigger space points
      //
      for (; r0!=r0e; ++r0) {

        float X  = (*r0)->x();
        float Y  = (*r0)->y();
        float R  = (*r0)->radius();
        if (R<m_r2minv) continue;
        if (R>m_r2maxv) break;
        float Z  = (*r0)->z();
        float ax = X/R;
        float ay = Y/R;

        // Bottom links production
        //
        int NB = m_rfzv_n[a];
        for (int i=0; i!=NB; ++i) {
   
          int an = m_rfzv_i[a][i];
          if (!data.rfzv_map[an]) continue;

          r  =  data.rfzv_Sorted[an].begin();
          re =  data.rfzv_Sorted[an].end  ();
   
          for (; r!=re; ++r) {
     
            float Rb =(*r)->radius();
            if (Rb<m_r1minv) continue;
            if (Rb>m_r1maxv) break;
            float dR = R-Rb;
            if (dR<m_drminv) break;
            if (dR>m_drmax) continue;
            float dZ = Z-(*r)->z();
            float Tz = dZ/dR;
            if (Tz<m_dzdrmin || Tz>m_dzdrmax) continue;
            float Zo = Z-R*Tz;

            // Comparison with vertices Z coordinates
            //
            if (!isZCompatible(data, Zo, Rb, Tz)) continue;

            // Momentum cut
            //
            float dx =(*r)->x()-X;
            float dy =(*r)->y()-Y;
            float x  = dx*ax+dy*ay;
            float y  =-dx*ay+dy*ax;
            float xy = x*x+y*y; if (xy == 0.) continue;
            float r2 = 1./xy;
            float Ut = x*r2;
            float Vt = y*r2;
            float UR = Ut*R+1.; if (UR == 0.) continue;
            float A  = Vt*R/UR;
            float B  = Vt-A*Ut;
            if (std::abs(B*data.K) > m_ipt*std::sqrt(1.f+A*A)) continue;
            ++nseed;
            newSeed(data, (*r)->spacepoint, (*r0)->spacepoint, Zo);
          }
        }
        if (nseed < m_maxsize) continue;
        data.endlist=false;
        data.rMin = (++r0);
        data.fvNmin=f;
        data.zMin=z;
        return;
      }
    }
  }
  data.endlist = true;
}

///////////////////////////////////////////////////////////////////
// Production 3 space points seeds 
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_Trigger::production3Sp(EventData& data) const
{ 
  data.mapSeeds.clear();
  if (data.nsaz<3) return;

  const int   ZI[SizeZ]= {5,6,7,8,9,10,4,3,2,1,0};
  std::vector<InDet::SiSpacePointForSeed*>::iterator rt[9],rte[9],rb[9],rbe[9];
  int nseed = 0;

  // Loop thorugh all azimuthal regions
  //
  for (int f=data.fNmin; f<=m_fNmax; ++f) {
    
    // For each azimuthal region loop through all Z regions
    //
    int z = 0;
    if (!data.endlist) z = data.zMin;

    for (; z<SizeZ; ++z) {

      int a  = f *SizeZ+ZI[z];
      if (!data.rfz_map[a]) continue;
      int NB = 0, NT = 0;
      for (int i=0; i!=m_rfz_b[a]; ++i) {
 
        int an =  m_rfz_ib[a][i];
        if (!data.rfz_map[an]) continue;
        rb [NB] = data.rfz_Sorted[an].begin();
        rbe[NB++] = data.rfz_Sorted[an].end();
      } 
      for (int i=0; i!=m_rfz_t[a]; ++i) {
 
        int an =  m_rfz_it[a][i];
        if (!data.rfz_map[an]) continue;
        rt [NT] = data.rfz_Sorted[an].begin();
        rte[NT++] = data.rfz_Sorted[an].end();
      } 
      if (!data.trigger) production3Sp       (data, rb, rbe, rt, rte, NB, NT, nseed);
      else            production3SpTrigger(data, rb, rbe, rt, rte, NB, NT, nseed);
      if (!data.endlist) {
        data.fNmin = f;
        data.zMin = z;
        return;
      } 
    }
  }
  data.endlist = true;
}

///////////////////////////////////////////////////////////////////
// Production 3 space points seeds for full scan
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_Trigger::production3Sp
(EventData& data,
 std::vector<InDet::SiSpacePointForSeed*>::iterator* rb ,
 std::vector<InDet::SiSpacePointForSeed*>::iterator* rbe,
 std::vector<InDet::SiSpacePointForSeed*>::iterator* rt ,
 std::vector<InDet::SiSpacePointForSeed*>::iterator* rte,
 int NB, int NT, int& nseed) const
{
  std::vector<InDet::SiSpacePointForSeed*>::iterator r0=rb[0],r;
  if (!data.endlist) {
    r0 = data.rMin;
    data.endlist = true;
  }

  // Loop through all trigger space points
  //
  for (; r0!=rbe[0]; ++r0) {

    data.nOneSeeds = 0;
    data.mapOneSeeds.erase(data.mapOneSeeds.begin(), data.mapOneSeeds.end());
 
    float R  = (*r0)->radius();
    if (R<m_r2min) continue;
    if (R>m_r2max) break;

    const Trk::SpacePoint* SP0 = (*r0)->spacepoint;

    bool pix = true;
    if (SP0->clusterList().second) pix = false;
    const Trk::Surface* sur0 = (*r0)->sur();
    float               X    = (*r0)->x();
    float               Y    = (*r0)->y();
    float               Z    = (*r0)->z();
    int                 Nb   = 0;

    // Bottom links production
    //
    for (int i=0; i!=NB; ++i) {

      for (r=rb[i]; r!=rbe[i]; ++r) {
 
        float Rb =(*r)->radius();
        if (Rb<m_r1min) {
          rb[i]=r;
          continue;
        }
        if (Rb>m_r1max) break;

        float dR = R-Rb;
        if (dR<m_drmin) break;

        if (dR > m_drmax || (*r)->sur()==sur0) continue;

        if ( !pix && !(*r)->spacepoint->clusterList().second) continue;
 
        float Tz = (Z-(*r)->z())/dR;

        if (Tz < m_dzdrmin || Tz > m_dzdrmax) continue;
 
        // Comparison with vertices Z coordinates
        //
        float Zo = Z-R*Tz;
        if (!isZCompatible(data, Zo, Rb, Tz)) continue;
        data.SP[Nb] = (*r);
        if (++Nb==m_maxsizeSP) goto breakb;
      }
    }
  breakb:
    if (!Nb || Nb==m_maxsizeSP) continue;
    int Nt = Nb;
    
    // Top   links production
    //
    for (int i=0; i!=NT; ++i) {
      
      for (r=rt[i]; r!=rte[i]; ++r) {
 
        float Rt =(*r)->radius();
        float dR = Rt-R;
        if (dR<m_drmin || Rt<m_r3min) {
          rt[i]=r;
          continue;
        }
        if (Rt>m_r3max || dR>m_drmax) break;

        if ( (*r)->sur()==sur0) continue;

        float Tz = ((*r)->z()-Z)/dR;

        if (Tz < m_dzdrmin || Tz > m_dzdrmax) continue;

        // Comparison with vertices Z coordinates
        //
        float Zo = Z-R*Tz;
        if (!isZCompatible(data, Zo, Rt, Tz)) continue;
        data.SP[Nt] = (*r);
        if (++Nt==m_maxsizeSP) goto breakt;
      }
    }
    
  breakt:
    if (!(Nt-Nb)) continue;

    float covr0 = (*r0)->covr ();
    float covz0 = (*r0)->covz ();

    float ax   = X/R;
    float ay   = Y/R;
    
    for (int i=0; i!=Nt; ++i) {

      InDet::SiSpacePointForSeed* sp = data.SP[i];

      float dx  = sp->x()-X;
      float dy  = sp->y()-Y;
      float dz  = sp->z()-Z;
      float x   = dx*ax+dy*ay;
      float y   =-dx*ay+dy*ax;
      float r2  = 1.f/(x*x+y*y);
      float dr  = std::sqrt(r2);
      float tz  = dz*dr;
      if (i < Nb) tz = -tz;

      data.Tz[i]   = tz;
      data.Zo[i]   = Z-R*tz;
      data.R [i]   = dr;
      data.U [i]   = x*r2;
      data.V [i]   = y*r2;
      data.Er[i]   = (covz0+sp->covz()+tz*tz*(covr0+sp->covr()))*r2;
    }
 
    float imc   = m_diver;
    float imcs  = m_diverpps;
    float ipt2  = m_ipt2;
    float K     = data.K;
    float K2    = K*K;
    float COF   = m_COF;
    float ipt2K = ipt2/K2;
    float ipt2C = ipt2*COF;
    float COFK  = COF*K2;
    covr0      *= 2.f;
    covz0      *= 2.f;
    
    // Three space points comparison
    //
    for (int b=0; b!=Nb; ++b) {
    
      const Trk::SpacePoint* SPb = data.SP[b]->spacepoint;
 
      float  Zob  = data.Zo[b];
      float  Tzb  = data.Tz[b];
      float  Rb2r = data.R [b]*covr0;
      float  Rb2z = data.R [b]*covz0;
      float  Erb  = data.Er[b];
      float  Vb   = data.V [b];
      float  Ub   = data.U [b];
      float  Tzb2 = (1.f+Tzb*Tzb);
      float  CSA  = Tzb2*COFK;
      float ICSA  = Tzb2*ipt2C;
      float dZ    = dZVertexMin(data, Zob);
      float Iz    = (dZ*dZ)/Tzb2;

      for (int t=Nb;  t!=Nt; ++t) {
 
        float Ts  = .5*(Tzb+data.Tz[t]);
        float dt  =     Tzb-data.Tz[t];
        float dT  = dt*dt-Erb-data.Er[t]-data.R[t]*(Ts*Ts*Rb2r+Rb2z);
        if ( dT > ICSA) continue;
        float dU  = data.U[t]-Ub;
        if (dU == 0.) continue;
        float A   = (data.V[t]-Vb)/dU;
        float S2  = 1.+A*A;
        float B   = Vb-A*Ub;
        float B2  = B*B;
        if (B2  > ipt2K*S2 || dT*S2 > B2*CSA) continue;
        float Im  = std::abs((A-B*R)*R);
 
        if (Im > imc ) continue;
 
        if (pix) {
          if (data.SP[t]->spacepoint->clusterList().second && Im > imcs) continue;
        }
        if (Im > m_diversss) continue;
        Im = Im*Im+Iz;
        newOneSeed(data, SPb, SP0, data.SP[t]->spacepoint, Zob, Im);
      }
    }
    nseed += data.mapOneSeeds.size();
    fillSeeds(data);
    if (nseed>=m_maxsize) {
      data.endlist=false;
      ++r0;
      data.rMin = r0;
      return;
    }
  }
}

///////////////////////////////////////////////////////////////////
// Production 3 space points seeds in ROI
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_Trigger::production3SpTrigger
(EventData& data, 
 std::vector<InDet::SiSpacePointForSeed*>::iterator* rb ,
 std::vector<InDet::SiSpacePointForSeed*>::iterator* rbe,
 std::vector<InDet::SiSpacePointForSeed*>::iterator* rt ,
 std::vector<InDet::SiSpacePointForSeed*>::iterator* rte,
 int NB, int NT, int& nseed) const
{
  constexpr float pi2 = 2.*M_PI;

  std::vector<InDet::SiSpacePointForSeed*>::iterator r0=rb[0],r;
  if (!data.endlist) {
    r0 = data.rMin;
    data.endlist = true;
  }

  // Loop through all trigger space points
  //
  for (; r0!=rbe[0]; ++r0) {

    data.nOneSeeds = 0;
    data.mapOneSeeds.erase(data.mapOneSeeds.begin(), data.mapOneSeeds.end());
 
    float R  = (*r0)->radius();
    if (R<m_r2min) continue;
    if (R>m_r2max) break;

    const Trk::SpacePoint* SP0 = (*r0)->spacepoint;

    bool pix = true;
    if (SP0->clusterList().second) pix = false;
    const Trk::Surface* sur0 = (*r0)->sur();
    float               X    = (*r0)->x();
    float               Y    = (*r0)->y();
    float               Z    = (*r0)->z();
    int                 Nb   = 0;

    // Bottom links production
    //
    for (int i=0; i!=NB; ++i) {

      for (r=rb[i]; r!=rbe[i]; ++r) {
 
        float Rb =(*r)->radius();
        if (Rb<m_r1min) {
          rb[i]=r;
          continue;
        }
        if (Rb>m_r1max) break;

        float dR = R-Rb;
        if (dR<m_drmin) break;

        if (dR > m_drmax || (*r)->sur()==sur0) continue;

        if ( !pix && !(*r)->spacepoint->clusterList().second) continue;
 
        // Comparison with  bottom and top Z 
        //
        float Tz = (Z-(*r)->z())/dR;
        float Zo = Z-R*Tz;
        if (Zo < data.zminB || Zo > data.zmaxB) continue;
        float Zu = Z+(550.f-R)*Tz;
        if (Zu < data.zminU || Zu > data.zmaxU) continue;
        data.SP[Nb] = (*r);
        if (++Nb==m_maxsizeSP) goto breakb;
      }
    }
  breakb:
    if (!Nb || Nb==m_maxsizeSP) continue;
    int Nt = Nb;
    
    // Top   links production
    //
    for (int i=0; i!=NT; ++i) {
      
      for (r=rt[i]; r!=rte[i]; ++r) {
 
        float Rt =(*r)->radius();
        float dR = Rt-R;
        if (dR<m_drmin || Rt<m_r3min) {
          rt[i]=r;
          continue;
        }
        if (Rt>m_r3max || dR>m_drmax) break;

        if ( (*r)->sur()==sur0) continue;

        // Comparison with  bottom and top Z 
        //
        float Tz = ((*r)->z()-Z)/dR;
        float Zo = Z-R*Tz;
        if (Zo < data.zminB || Zo > data.zmaxB) continue;
        float Zu = Z+(550.f-R)*Tz;
        if (Zu < data.zminU || Zu > data.zmaxU) continue;
        data.SP[Nt] = (*r);
        if (++Nt==m_maxsizeSP) goto breakt;
      }
    }
    
  breakt:
    if (!(Nt-Nb)) continue;

    float covr0 = (*r0)->covr ();
    float covz0 = (*r0)->covz ();

    float ax   = X/R;
    float ay   = Y/R;
    
    for (int i=0; i!=Nt; ++i) {

      InDet::SiSpacePointForSeed* sp = data.SP[i];

      float dx  = sp->x()-X;
      float dy  = sp->y()-Y;
      float dz  = sp->z()-Z;
      float x   = dx*ax+dy*ay;
      float y   =-dx*ay+dy*ax;
      float r2  = 1.f/(x*x+y*y);
      float dr  = std::sqrt(r2);
      float tz  = dz*dr;
      if (i < Nb) tz = -tz;

      data.Tz[i]   = tz;
      data.Zo[i]   = Z-R*tz;
      data.R [i]   = dr;
      data.U [i]   = x*r2;
      data.V [i]   = y*r2;
      data.Er[i]   = (covz0+sp->covz()+tz*tz*(covr0+sp->covr()))*r2;
    }
 
    float imc   = m_diver;
    float imcs  = m_diverpps;
    float ipt2  = m_ipt2;
    float K     = data.K;
    float K2    = K*K;
    float COF   = m_COF;
    float ipt2K = ipt2/K2;
    float ipt2C = ipt2*COF;
    float COFK  = COF*K2;
    covr0      *= 2.f;
    covz0      *= 2.f;
   
    // Three space points comparison
    //
    for (int b=0; b!=Nb; ++b) {

      const Trk::SpacePoint* SPb = data.SP[b]->spacepoint;
 
      float  Zob  = data.Zo[b];
      float  Tzb  = data.Tz[b];
      float  Rb2r = data.R [b]*covr0;
      float  Rb2z = data.R [b]*covz0;
      float  Erb  = data.Er[b];
      float  Vb   = data.V [b];
      float  Ub   = data.U [b];
      float  Tzb2 = (1.f+Tzb*Tzb);
      float  CSA  = Tzb2*COFK;
      float ICSA  = Tzb2*ipt2C;
      float dZ    = dZVertexMin(data, Zob);
      float Iz    = (dZ*dZ)/Tzb2;

      for (int t=Nb;  t!=Nt; ++t) {
 
        float Ts  = .5f*(Tzb+data.Tz[t]);
        float dt  =     Tzb-data.Tz[t];
        float dT  = dt*dt-Erb-data.Er[t]-data.R[t]*(Ts*Ts*Rb2r+Rb2z);
        if ( dT > ICSA) continue;
        float dU  = data.U[t]-Ub;
        if (dU == 0.) continue;
        float A   = (data.V[t]-Vb)/dU;
        float S2  = 1.f+A*A;
        float B   = Vb-A*Ub;
        float B2  = B*B;
        if (B2  > ipt2K*S2 || dT*S2 > B2*CSA) continue;
        float Im  = std::abs((A-B*R)*R);

        if (Im > imc ) continue;
        if (pix) {
          if (data.SP[t]->spacepoint->clusterList().second && Im > imcs) continue;
        }

        if (Im > m_diversss) continue;

        // Azimuthal angle test
        //
        float y  = 1.;
        float x  = 2.f*B*R-A;
        float df = std::abs(std::atan2(ay*y-ax*x,ax*y+ay*x)-data.ftrig);
        if (df > M_PI) df=pi2-df;
        if (df > data.ftrigW) continue;
        Im = Im*Im+Iz;
        newOneSeed(data, SPb, SP0, data.SP[t]->spacepoint, Zob, Im);
      }
    }
    nseed += data.mapOneSeeds.size();
    fillSeeds(data);
    if (nseed>=m_maxsize) {
      data.endlist=false;
      ++r0;
      data.rMin = r0;
      return;
    }
  }
}

///////////////////////////////////////////////////////////////////
// New 3 space points seeds from one space points
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_Trigger::newOneSeed
(EventData& data,
 const Trk::SpacePoint*& p1,const Trk::SpacePoint*& p2, 
 const Trk::SpacePoint*& p3,const float& z,const float& q) const
{
  if (data.nOneSeeds < m_maxOneSize) {

    data.OneSeeds[data.nOneSeeds].erase();
    data.OneSeeds[data.nOneSeeds].add(p1);
    data.OneSeeds[data.nOneSeeds].add(p2);
    data.OneSeeds[data.nOneSeeds].add(p3);
    data.OneSeeds[data.nOneSeeds].setZVertex(static_cast<double>(z));
    data.mapOneSeeds.insert(std::make_pair(q, &(data.OneSeeds[data.nOneSeeds])));
    ++data.nOneSeeds;
  } else {
    std::multimap<float,InDet::SiSpacePointsSeed*>::reverse_iterator 
      l = data.mapOneSeeds.rbegin();
    if ((*l).first <= q) return;
    
    InDet::SiSpacePointsSeed* s = (*l).second;
    s->erase     (  );
    s->add       (p1);
    s->add       (p2);
    s->add       (p3);
    s->setZVertex(static_cast<double>(z));
    std::multimap<float,InDet::SiSpacePointsSeed*>::iterator 
      i = data.mapOneSeeds.insert(std::make_pair(q,s));
                       
    for (++i; i!=data.mapOneSeeds.end(); ++i) {
      if ((*i).second==s) {
        data.mapOneSeeds.erase(i);
        return;
      }
    }
  }
}

const InDet::SiSpacePointsSeed* InDet::SiSpacePointsSeedMaker_Trigger::next(const EventContext&, EventData& data) const
{
  if (not data.initialized) initializeEventData(data);

  if (data.i_seed==data.i_seede) {
    findNext(data);
    //cppcheck-suppress identicalInnerCondition
    if (data.i_seed==data.i_seede) return nullptr;
  }
  if (data.mode==0 || data.mode==1) return &(*data.i_seed++);
  ++data.i_seed;
  return (*data.seed++).second;
}
    
bool InDet::SiSpacePointsSeedMaker_Trigger::isZCompatible  
(EventData& data, float& Zv, float& R, float& T) const
{
  if (Zv < m_zmin || Zv > m_zmax) return false;

  if (data.l_vertex.empty()) return true;

  float dZmin = std::numeric_limits<float>::max();
  for (const float& v : data.l_vertex) {
    float dZ = std::abs(v-Zv);
    if (dZ<dZmin) dZmin=dZ;
  }
  return dZmin < (m_dzver+m_dzdrver*R)*sqrt(1.+T*T);
}

float InDet::SiSpacePointsSeedMaker_Trigger::dZVertexMin(EventData& data, float& Z) 
{
  if (data.l_vertex.empty()) return 0.;

  float dZmin = std::numeric_limits<float>::max();
  for (const float& v : data.l_vertex) {
    float dZ = std::abs(v-Z);
    if (dZ<dZmin) dZmin = dZ;
  }
  return dZmin;
}

///////////////////////////////////////////////////////////////////
// New space point for seeds 
///////////////////////////////////////////////////////////////////

InDet::SiSpacePointForSeed* InDet::SiSpacePointsSeedMaker_Trigger::newSpacePoint
(EventData& data, const Trk::SpacePoint*const& sp) 
{
  InDet::SiSpacePointForSeed* sps = nullptr;

  float r[3];
  convertToBeamFrameWork(data, sp, r);

  if (data.i_spforseed!=data.l_spforseed.end()) {
    sps = &(*data.i_spforseed++);
    sps->set(sp, r);
  } else {
    data.l_spforseed.emplace_back(sp, r);
    sps = &(data.l_spforseed.back());
    data.i_spforseed = data.l_spforseed.end();
  }
      
  return sps;
}

///////////////////////////////////////////////////////////////////
// New 2 space points seeds 
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_Trigger::newSeed
(EventData& data,
 const Trk::SpacePoint*& p1,const Trk::SpacePoint*& p2, 
 const float& z) 
{
  if (data.i_seede!=data.l_seeds.end()) {
    InDet::SiSpacePointsSeed* s = &(*data.i_seede++);
    s->erase     (  );
    s->add       (p1);
    s->add       (p2);
    s->setZVertex(static_cast<double>(z));
  } else {
    data.l_seeds.emplace_back(p1, p2, z);
    data.i_seede = data.l_seeds.end();
  }
}

///////////////////////////////////////////////////////////////////
// Fill seeds
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_Trigger::fillSeeds(EventData& data) 
{
  std::multimap<float, InDet::SiSpacePointsSeed*>::iterator 
    l  = data.mapOneSeeds.begin(),
    le = data.mapOneSeeds.end  ();

  for (; l!=le; ++l) {
    float                     q  = (*l).first;
    InDet::SiSpacePointsSeed* s0 = (*l).second;

    if ((*s0->spacePoints().rbegin())->clusterList().second) {
      (*s0->spacePoints().begin())->clusterList().second ? q+=1000. : q+=10000.;
    }

    if (data.i_seede!=data.l_seeds.end()) {
      InDet::SiSpacePointsSeed* s = &(*data.i_seede++);
      *s = *s0;
      data.mapSeeds.insert(std::make_pair(q,s));
    } else {
      data.l_seeds.emplace_back(*s0);
      InDet::SiSpacePointsSeed* s = &(data.l_seeds.back());
      data.i_seede = data.l_seeds.end();
      data.mapSeeds.insert(std::make_pair(q, s));
    }
  }
}

void InDet::SiSpacePointsSeedMaker_Trigger::initializeEventData(EventData& data) const {
  data.initialize(EventData::ToolType::Trigger,
                  m_maxsizeSP,
                  m_maxOneSize,
                  0, // maxsize not used
                  m_r_size,
                  0, // sizeRF not used
                  SizeRFZ,
                  SizeRFZV,
                  false); // checkEta not used
}

void InDet::SiSpacePointsSeedMaker_Trigger::writeNtuple(const SiSpacePointsSeed*, const Trk::Track*, int, long) const{
}

bool InDet::SiSpacePointsSeedMaker_Trigger::getWriteNtupleBoolProperty() const{
    return false;
}
