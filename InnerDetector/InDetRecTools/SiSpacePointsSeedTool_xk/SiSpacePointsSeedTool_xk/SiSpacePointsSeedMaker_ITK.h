// -*- C++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////
//  Header file for class SiSpacePointsSeedMaker_ITK
/////////////////////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
/////////////////////////////////////////////////////////////////////////////////
// Class for track candidates generation using space points information
// for standard Atlas geometry
/////////////////////////////////////////////////////////////////////////////////
// Version 1.0 3/10/2004 I.Gavrilenko
/////////////////////////////////////////////////////////////////////////////////

#ifndef SiSpacePointsSeedMaker_ITK_H
#define SiSpacePointsSeedMaker_ITK_H

#include "InDetRecToolInterfaces/ISiSpacePointsSeedMaker.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "BeamSpotConditionsData/BeamSpotData.h"
#include "MagFieldInterfaces/IMagFieldSvc.h"
#include "SiSpacePointsSeedTool_xk/SiSpacePointForSeedITK.h"
#include "SiSpacePointsSeedTool_xk/SiSpacePointsProSeedITK.h" 
#include "TrkSpacePoint/SpacePointContainer.h" 
#include "TrkSpacePoint/SpacePointOverlapCollection.h"

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include <iosfwd>
#include <list>
#include <map>
#include <mutex>
#include <set>
#include <vector>

class MsgStream;

namespace Trk {
  class IPRD_AssociationTool;
}

namespace InDet {

  class SiSpacePointsSeedMaker_ITK : 
    virtual public ISiSpacePointsSeedMaker, public AthAlgTool
  {
    ///////////////////////////////////////////////////////////////////
    // Public methods:
    ///////////////////////////////////////////////////////////////////
      
  public:
      
    ///////////////////////////////////////////////////////////////////
    // Standard tool methods
    ///////////////////////////////////////////////////////////////////

    SiSpacePointsSeedMaker_ITK
    (const std::string&,const std::string&,const IInterface*);
    virtual ~SiSpacePointsSeedMaker_ITK() = default;
    virtual StatusCode initialize();
    virtual StatusCode finalize();

    ///////////////////////////////////////////////////////////////////
    // Methods to initialize tool for new event or region
    ///////////////////////////////////////////////////////////////////

    virtual void newEvent(int iteration);
    virtual void newRegion(const std::vector<IdentifierHash>& vPixel, const std::vector<IdentifierHash>& vSCT);
    virtual void newRegion(const std::vector<IdentifierHash>& vPixel, const std::vector<IdentifierHash>& vSCT, const IRoiDescriptor& IRD);
      
    ///////////////////////////////////////////////////////////////////
    // Methods to initilize different strategies of seeds production
    // with two space points with or without vertex constraint
    ///////////////////////////////////////////////////////////////////

    virtual void find2Sp(const std::list<Trk::Vertex>& lv);

    ///////////////////////////////////////////////////////////////////
    // Methods to initilize different strategies of seeds production
    // with three space points with or without vertex constraint
    ///////////////////////////////////////////////////////////////////

    virtual void find3Sp(const std::list<Trk::Vertex>& lv);
    virtual void find3Sp(const std::list<Trk::Vertex>& lv, const double* ZVertex);

    ///////////////////////////////////////////////////////////////////
    // Methods to initilize different strategies of seeds production
    // with variable number space points with or without vertex constraint
    // Variable means (2,3,4,....) any number space points
    ///////////////////////////////////////////////////////////////////
 
    virtual void findVSp(const std::list<Trk::Vertex>& lv);
      
    ///////////////////////////////////////////////////////////////////
    // Iterator through seeds pseudo collection produced accordingly
    // methods find    
    ///////////////////////////////////////////////////////////////////
      
    virtual const SiSpacePointsSeed* next();
      
    ///////////////////////////////////////////////////////////////////
    // Print internal tool parameters and status
    ///////////////////////////////////////////////////////////////////

    virtual MsgStream&    dump(MsgStream   & out) const;
    virtual std::ostream& dump(std::ostream& out) const;

  private:
    enum Size {SizeRF=53,
               SizeZ=11,
               SizeRFZ=SizeRF*SizeZ,
               SizeI=9,
               SizeRFV=100,
               SizeZV=3,
               SizeRFZV=SizeRFV*SizeZV,
               SizeIV=6};

    ///////////////////////////////////////////////////////////////////
    // Private data and methods
    ///////////////////////////////////////////////////////////////////
  
    ServiceHandle<MagField::IMagFieldSvc> m_fieldServiceHandle{this, "MagFieldSvc", "AtlasFieldSvc"};
    PublicToolHandle<Trk::IPRD_AssociationTool> m_assoTool{this, "AssociationTool", "InDet::InDetPRD_AssociationToolGangedPixels"};
        
    ///////////////////////////////////////////////////////////////////
    // Space points container
    ///////////////////////////////////////////////////////////////////
    SG::ReadHandleKey<SpacePointContainer> m_spacepointsSCT{this, "SpacePointsSCTName", "SCT_SpacePoints"};
    SG::ReadHandleKey<SpacePointContainer> m_spacepointsPixel{this, "SpacePointsPixelName", "PixelSpacePoints"};
    SG::ReadHandleKey<SpacePointOverlapCollection> m_spacepointsOverlap{this, "SpacePointsOverlapName", "OverlapSpacePoints"};

    SG::ReadCondHandleKey<InDet::BeamSpotData> m_beamSpotKey{this, "BeamSpotKey", "BeamSpotData", "SG key for beam spot"};

    BooleanProperty m_pixel{this, "usePixel", true};
    BooleanProperty m_sct{this, "useSCT", true};
    BooleanProperty m_useOverlap{this, "useOverlapSpCollection", true};
    BooleanProperty m_useassoTool{this, "UseAssociationTool", false};
    IntegerProperty m_maxsize{this, "maxSize", 50000};
    IntegerProperty m_maxsizeSP{this, "maxSizeSP", 5000};
    IntegerProperty m_maxOneSize{this, "maxSeedsForSpacePoint", 5};
    FloatProperty m_etamax{this, "etaMax", 2.7};
    FloatProperty m_r1minv{this, "minVRadius1", 0.};
    FloatProperty m_r1maxv{this, "maxVRadius1", 60.};
    FloatProperty m_r2minv{this, "minVRadius2", 70.};
    FloatProperty m_r2maxv{this, "maxVRadius2", 200.};
    FloatProperty m_drmin{this, "mindRadius", 5.};
    FloatProperty m_drmax{this, "maxdRadius", 300.};
    FloatProperty m_zmin{this, "minZ", -250.};
    FloatProperty m_zmax{this , "maxZ", +250.};
    FloatProperty m_r_rmin{this, "radMin", 0.};
    FloatProperty m_r_rstep{this, "radStep", 2.};
    FloatProperty m_dzver{this, "maxdZver", 5.};
    FloatProperty m_dzdrver{this, "maxdZdRver", 0.02};
    FloatProperty m_diver{this, "maxdImpact", 10.};
    FloatProperty m_diversss{this, "maxdImpactSSS", 20.};
    FloatProperty m_divermax{this, "maxdImpactForDecays", 20.};
    FloatProperty m_dzmaxPPP{this, "dZmaxForPPPSeeds", 600.};

    // Properties, which will be updated in initialize
    FloatProperty m_etamin{this, "etaMin", 0.};
    FloatProperty m_r_rmax{this, "radMax", 1100.};
    FloatProperty m_ptmin{this, "pTmin", 500.};
    FloatProperty m_umax{this, "minSeedsQuality", 0.};

    // Properties, which will be updated in event methods, checketa is prepared in EventData.
    BooleanProperty m_checketa{this, "checkEta", false};

    // Properties, which are not used in this implementation of SiSpacePointsSeedMaker_ITK class
    BooleanProperty m_dbm{this, "useDBM", false};
    UnsignedIntegerProperty m_maxNumberVertices{this, "maxNumberVertices", 99};
    FloatProperty m_r1min{this, "minRadius1", 0.};
    FloatProperty m_r1max{this, "maxRadius1", 600.};
    FloatProperty m_r2min{this, "minRadius2", 0.};
    FloatProperty m_r2max{this, "maxRadius2", 600.};
    FloatProperty m_r3min{this, "minRadius3", 0.};
    FloatProperty m_r3max{this, "maxRadius3", 600.};
    FloatProperty m_rapcut{this, "RapidityCut", 2.7};
    FloatProperty m_diverpps{this, "maxdImpactPPS", 1.7};

    // Not updated at all
    float m_drminv{20.};

    // Updated only in initialize
    bool m_initialized{false};
    int m_outputlevel{0};
    int m_r_size{0};
    int m_fNmax{0};
    int m_fvNmax{0};
    int m_rfz_b[SizeRFZ];
    int m_rfz_t[SizeRFZ];
    int m_rfz_ib[SizeRFZ][SizeI];
    int m_rfz_it[SizeRFZ][SizeI];
    int m_rfzv_n[SizeRFZV];
    int m_rfzv_i[SizeRFZV][SizeIV];
    float m_dzdrmin0{0.};
    float m_dzdrmax0{0.};
    float m_ipt{0.};
    float m_ipt2{0.};
    float m_COF{0.};
    float m_sF{0.};
    float m_sFv{0.};

    mutable std::mutex m_mutex;
    mutable std::vector<EventContext::ContextEvt_t> m_cache ATLAS_THREAD_SAFE; // Guarded by m_mutex
    struct EventData { // To hold event dependent data
      bool checketa{false};
      bool endlist{true};
      bool trigger{false};
      bool isvertex{false};
      int nprint{0};
      int state{0};
      int nspoint{2};
      int mode{0};
      int nlist{0};
      int iteration{0};
      int iteration0{0};
      int r_first{0};
      int ns{0};
      int nsaz{0};
      int nsazv{0};
      int zMin{0};
      int nr{0};
      int nrfz{0};
      int nrfzv{0};
      int fNmin{0};
      int fvNmin{0};
      int nOneSeeds{0};
      int fillOneSeeds{0};
      int rfz_index[SizeRFZ];
      int rfz_map[SizeRFZ];
      int rfzv_index[SizeRFZV];
      int rfzv_map[SizeRFZV];
      float dzdrmin{0.};
      float dzdrmax{0.};
      float zminU{0.};
      float zmaxU{0.};
      float zminB{0.};
      float zmaxB{0.};
      float ftrig{0.};
      float ftrigW{0.};
      float K{0.};
      float ipt2K{0.};
      float ipt2C{0.};
      float COFK{0.};
      InDet::SiSpacePointsSeed seedOutput;
      std::vector<std::list<InDet::SiSpacePointForSeedITK*>> r_Sorted;
      std::vector<int> r_index;
      std::vector<int> r_map;
      std::vector<InDet::SiSpacePointsProSeedITK> OneSeeds;
      std::vector<std::pair<float,InDet::SiSpacePointForSeedITK*>> CmSp;
      std::multimap<float,InDet::SiSpacePointsProSeedITK*> seeds;
      std::multimap<float,InDet::SiSpacePointsProSeedITK*>::iterator seed;
      std::multimap<float,InDet::SiSpacePointsProSeedITK*> mapOneSeeds;
      std::list<InDet::SiSpacePointForSeedITK*> rfz_Sorted[SizeRFZ];
      std::list<InDet::SiSpacePointForSeedITK*> rfzv_Sorted[SizeRFZV];
      std::list<InDet::SiSpacePointForSeedITK> l_spforseed;
      std::list<InDet::SiSpacePointForSeedITK>::iterator i_spforseed;
      std::list<InDet::SiSpacePointForSeedITK*>::iterator rMin; 
      std::list<InDet::SiSpacePointsProSeedITK> l_seeds;
      std::list<InDet::SiSpacePointsProSeedITK>::iterator i_seed;
      std::list<InDet::SiSpacePointsProSeedITK>::iterator i_seede;
      std::set<float> l_vertex;
      ///////////////////////////////////////////////////////////////////
      // Tables for 3 space points seeds search
      ///////////////////////////////////////////////////////////////////
      std::vector<InDet::SiSpacePointForSeedITK*> SP;
      std::vector<float> R;
      std::vector<float> Tz;
      std::vector<float> Er;
      std::vector<float> U;
      std::vector<float> V;
      std::vector<float> X;
      std::vector<float> Y;
      std::vector<float> Zo;
      ///////////////////////////////////////////////////////////////////
      // Beam geometry
      ///////////////////////////////////////////////////////////////////
      float xbeam[4]{0., 1., 0., 0,}; // x,ax,ay,az - center and x-axis direction
      float ybeam[4]{0., 0., 1., 0.}; // y,ax,ay,az - center and y-axis direction
      float zbeam[4]{0., 0., 0., 1.}; // z,ax,ay,az - center and z-axis direction
    };
    mutable std::vector<EventData> m_eventData ATLAS_THREAD_SAFE; // Guarded by m_mutex

    ///////////////////////////////////////////////////////////////////
    // Private methods
    ///////////////////////////////////////////////////////////////////
    /**    @name Disallow default instantiation, copy, assignment */
    //@{
    SiSpacePointsSeedMaker_ITK() = delete;
    SiSpacePointsSeedMaker_ITK(const SiSpacePointsSeedMaker_ITK&) = delete;
    SiSpacePointsSeedMaker_ITK &operator=(const SiSpacePointsSeedMaker_ITK&) = delete;
    //@}

    MsgStream& dumpConditions(EventData& data, MsgStream& out) const;
    MsgStream& dumpEvent(EventData& data, MsgStream& out) const;

    void buildFrameWork();
    void buildBeamFrameWork(EventData& data) const;

    SiSpacePointForSeedITK* newSpacePoint
    (EventData& data, const Trk::SpacePoint*const&) const;
    void newSeed
    (EventData& data,
     SiSpacePointForSeedITK*&,SiSpacePointForSeedITK*&,float) const;

    void newOneSeed
    (EventData& data, 
     SiSpacePointForSeedITK*&,SiSpacePointForSeedITK*&,
     SiSpacePointForSeedITK*&,float,float) const;

    void newOneSeedWithCurvaturesComparison
    (EventData& data,
     SiSpacePointForSeedITK*&,SiSpacePointForSeedITK*&,float) const;

    void fillSeeds(EventData& data) const;
    void fillLists(EventData& data) const;
    void erase(EventData& data) const;
    void production2Sp(EventData& data) const;
    void production3Sp(EventData& data) const;
    void production3SpSSS
    (EventData& data,
     std::list<InDet::SiSpacePointForSeedITK*>::iterator*,
     std::list<InDet::SiSpacePointForSeedITK*>::iterator*,
     std::list<InDet::SiSpacePointForSeedITK*>::iterator*,
     std::list<InDet::SiSpacePointForSeedITK*>::iterator*,
     int,int,int&) const;
    void production3SpPPP
    (EventData& data,
     std::list<InDet::SiSpacePointForSeedITK*>::iterator*,
     std::list<InDet::SiSpacePointForSeedITK*>::iterator*,
     std::list<InDet::SiSpacePointForSeedITK*>::iterator*,
     std::list<InDet::SiSpacePointForSeedITK*>::iterator*,
     int,int,int&) const;
    void production3SpTrigger
    (EventData& data,
     std::list<InDet::SiSpacePointForSeedITK*>::iterator*,
     std::list<InDet::SiSpacePointForSeedITK*>::iterator*,
     std::list<InDet::SiSpacePointForSeedITK*>::iterator*,
     std::list<InDet::SiSpacePointForSeedITK*>::iterator*,
     int,int,int&) const;
 
    bool newVertices(EventData& data, const std::list<Trk::Vertex>&) const;
    void findNext(EventData& data) const;
    bool isZCompatible(EventData& data, float&,float&,float&) const;
    void convertToBeamFrameWork(EventData& data, const Trk::SpacePoint*const&,float*) const;
    bool isUsed(const Trk::SpacePoint*) const;

    EventData& getEventData() const;
  };

  MsgStream&    operator << (MsgStream&   ,const SiSpacePointsSeedMaker_ITK&);
  std::ostream& operator << (std::ostream&,const SiSpacePointsSeedMaker_ITK&);

} // end of name space

///////////////////////////////////////////////////////////////////
// Object-function for curvature seeds comparison
///////////////////////////////////////////////////////////////////

class comCurvatureITK {
public:
  bool operator ()
  (const std::pair<float,InDet::SiSpacePointForSeedITK*>& i1, 
   const std::pair<float,InDet::SiSpacePointForSeedITK*>& i2)
  {
    return i1.first < i2.first;
  }
};

#endif // SiSpacePointsSeedMaker_ITK_H
