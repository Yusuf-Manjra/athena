/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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

#include <list>
#include "GaudiKernel/ServiceHandle.h"
#include "MagFieldInterfaces/IMagFieldSvc.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "InDetRecToolInterfaces/ISiSpacePointsSeedMaker.h"
#include "TrkSpacePoint/SpacePointContainer.h" 
#include "TrkSpacePoint/SpacePointOverlapCollection.h"
#include "InDetBeamSpotService/IBeamCondSvc.h"
#include "SiSpacePointsSeedTool_xk/SiSpacePointForSeedITK.h"
#include "SiSpacePointsSeedTool_xk/SiSpacePointsProSeedITK.h" 
#include "SiSpacePointsSeedTool_xk/SiSpacePointsSeedMakerEventData.h" // Load Indet::FloatInt
#include "GaudiKernel/ITHistSvc.h"
#include "TFile.h"
#include "TTree.h"

class MsgStream   ;
class IBeamCondSvc;

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
      virtual ~SiSpacePointsSeedMaker_ITK();
      virtual StatusCode               initialize();
      virtual StatusCode               finalize  ();

      ///////////////////////////////////////////////////////////////////
      // Methods to initialize tool for new event or region
      ///////////////////////////////////////////////////////////////////

      void newEvent (int);
      void newRegion
	(const std::vector<IdentifierHash>&,const std::vector<IdentifierHash>&);
      void newRegion
	(const std::vector<IdentifierHash>&,const std::vector<IdentifierHash>&,const IRoiDescriptor&);
      
      ///////////////////////////////////////////////////////////////////
      // Methods to initilize different strategies of seeds production
      // with two space points with or without vertex constraint
      // Unused but needed for common interface with other SiSpacePointsSeedMaker
      ///////////////////////////////////////////////////////////////////

      void find2Sp (const std::list<Trk::Vertex>&){};

      ///////////////////////////////////////////////////////////////////
      // Methods to initialize different strategies of seeds production
      // with three space points with or without vertex constraint
      ///////////////////////////////////////////////////////////////////

      void find3Sp (const std::list<Trk::Vertex>&);
      void find3Sp (const std::list<Trk::Vertex>&,const double*);

      ///////////////////////////////////////////////////////////////////
      // Methods to initialize different strategies of seeds production
      // with variable number space points with or without vertex constraint
      // Variable means (2,3,4,....) any number space points
      ///////////////////////////////////////////////////////////////////
 
      void findVSp (const std::list<Trk::Vertex>&);
      
      ///////////////////////////////////////////////////////////////////
      // Iterator through seeds pseudo collection produced accordingly
      // methods find    
      ///////////////////////////////////////////////////////////////////
      
      const SiSpacePointsSeed* next();
      
      ///////////////////////////////////////////////////////////////////
      // Print internal tool parameters and status
      ///////////////////////////////////////////////////////////////////

      MsgStream&    dump          (MsgStream   & out) const;
      std::ostream& dump          (std::ostream& out) const;
      
      /** This method is called by the SiSPSeededTrackFinder algorithm to fill ntuples for 
      * seeds seen by the algorithm. seedType represents Pixel/SCT type seeds, where 0->SCT
      * and 1->Pixel. givesTrack is determined by whether or not the given seed forms atleast 
      * one track candidate. 0->No track candidate 1->At least one track Candidate
      **/  
      virtual void writeNtuple(const SiSpacePointsSeed* seed, const Trk::Track* track, int seedType, long eventNumber) const override;
      
      virtual bool getWriteNtupleBoolProperty() const override;

    protected:
      
      ///////////////////////////////////////////////////////////////////
      // Protected data and methods
      ///////////////////////////////////////////////////////////////////
  
      ServiceHandle<MagField::IMagFieldSvc>  m_fieldServiceHandle ;
      MagField::IMagFieldSvc*                m_fieldService       ;
        
      IBeamCondSvc* p_beam                                        ;
      bool                        m_pixel                         ;
      bool                        m_sct                           ;
      bool                        m_endlist                       ;
      bool                        m_useOverlap                    ;
      bool                        m_useassoTool                   ;
      bool                        m_trigger                       ;
      bool                        m_checketa                      ;
      bool                        m_isvertex                      ;
      bool                        m_fastTracking                  ;
      int                         m_outputlevel                   ;
      int                         m_nprint                        ;
      int                         m_state                         ;
      int                         m_nspoint                       ;
      int                         m_mode                          ;
      int                         m_nlist                         ;
      int                         m_maxsize                       ;
      int                         m_iteration                     ;
      int                         m_ndyncut                       ;
      int                         m_nCmSp                         ;

      float                       m_etamin, m_etamax              ;
      float                       m_drminSSS                      ;
      float                       m_drmaxSSS                      ;
      float                       m_drminPPP                      ;
      float                       m_drmaxPPP                      ;
      float                       m_rmaxPPPSeedConfBarrel         ;
      float                       m_rmaxPPPSeedConfEndcap         ;
      float                       m_zmaxPPPSeedConfBarrel         ;
      float                       m_dzmaxSSS                      ;
      float                       m_dzdrmin0                      ;
      float                       m_dzdrmax0                      ;
      float                       m_dzdrmin                       ;
      float                       m_dzdrmax                       ;
      float                       m_zmin                          ;
      float                       m_zmax                          ;
      float                       m_zmaxPPP                       ;
      float                       m_zmaxSSS                       ;
      float                       m_zminU                         ;
      float                       m_zmaxU                         ;
      float                       m_zminB                         ;
      float                       m_zmaxB                         ;
      float                       m_ftrig                         ;
      float                       m_ftrigW                        ;
      float                       r_rmax                          ;
      float                       r_rmin                          ;
      float                       r_rstep                         ;
      float                       m_dzver                         ;
      float                       m_dzdrver                       ;
      float                       m_diver                         ;
      float                       m_diversss                      ;
      float                       m_rminSSS                       ;
      float                       m_rmaxSSS                       ;
      float                       m_dazmax                        ;
      float                       m_ptmin                         ;
      float                       m_ipt                           ;
      float                       m_ipt2                          ;
      float                       m_COF                           ;
      float                       m_K                             ;
      float                       m_ipt2K                         ;
      float                       m_ipt2C                         ;
      float                       m_COFK                          ;  
      float                       m_RTmin                         ;
      float                       m_RTmax                         ;
      
      bool                        m_isLRT                         ;

      int r_size                                                  ;
      int r_first                                                 ;
      int rf_size                                                 ;
      int rfz_size                                                ;
      std::list<InDet::SiSpacePointForSeedITK*>* r_Sorted            ;
      std::vector<InDet::SiSpacePointForSeedITK*>  rfz_Sorted [2211] ;
      std::list<InDet::SiSpacePointForSeedITK*>  l_spforseed         ;
      std::list<InDet::SiSpacePointForSeedITK*>::iterator i_spforseed; 

      int m_ns,m_nsaz,m_nsazv                                     ;
      int m_fNmax[2]                                              ;
      int m_fNmin                                                 ;
      int  m_nr     ; int* r_index   ; int* r_map                 ;
      int  m_nrfz   , rfz_index  [2211], rfz_map  [2211]          ;
      int rfz_b[2][2211],rfz_t[2][2211],rfz_ib[2][2211][9],rfz_it[2][2211][9];
      float m_sF[2]                                               ;

      ///////////////////////////////////////////////////////////////////
      // Tables for 3 space points seeds search
      ///////////////////////////////////////////////////////////////////
     
      int    m_maxsizeSP                                          ; 
      InDet::SiSpacePointForSeedITK** m_SP                        ;

      float               *  m_Tz                                 ;
      float               *  m_R                                  ;
      float               *  m_U                                  ;
      float               *  m_V                                  ;
      float               *  m_X                                  ;
      float               *  m_Y                                  ;
      float               *  m_Er                                 ;
      float               *  m_L                                  ;
      float               *  m_Im                                 ;
      FloatInt            *  m_Tn                                 ;
      FloatInt               m_CmSp[500]                          ;

      InDet::SiSpacePointsSeed* m_seedOutput                      ;

      std::list<InDet::SiSpacePointsProSeedITK*>           l_seeds;
      std::list<InDet::SiSpacePointsProSeedITK*>::iterator i_seed ; 
      std::list<InDet::SiSpacePointsProSeedITK*>::iterator i_seede ;

      std::multimap<float,InDet::SiSpacePointsProSeedITK*> m_seeds          ;
      std::multimap<float,InDet::SiSpacePointsProSeedITK*>::iterator m_seed ;

      std::multimap<float,InDet::SiSpacePointsProSeedITK*> m_mapOneSeeds;
      InDet::SiSpacePointsProSeedITK*                   m_OneSeeds   ;
      int                                               m_nOneSeeds  ;

      std::multimap<float,InDet::SiSpacePointsProSeedITK*> m_mapOneSeedsQ;
      InDet::SiSpacePointsProSeedITK*                   m_OneSeedsQ  ;
      int                                               m_nOneSeedsQ ;


      int                                               m_maxOneSize ;
      int                                               m_fillOneSeeds;
      std::set<float>                                   l_vertex     ;

      ///////////////////////////////////////////////////////////////////
      // Beam geometry
      ///////////////////////////////////////////////////////////////////
 
      float m_xbeam[4];    // x,ax,ay,az - center and x-axis direction
      float m_ybeam[4];    // y,ax,ay,az - center and y-axis direction
      float m_zbeam[4];    // z,ax,ay,az - center and z-axis direction

      ///////////////////////////////////////////////////////////////////
      // Space points container
      ///////////////////////////////////////////////////////////////////
      
      std::string                                 m_beamconditions    ;
      SG::ReadHandle<SpacePointContainer>         m_spacepointsSCT    ;
      SG::ReadHandle<SpacePointContainer>         m_spacepointsPixel  ;
      SG::ReadHandle<SpacePointOverlapCollection> m_spacepointsOverlap;
      ToolHandle<Trk::IPRD_AssociationTool>       m_assoTool          ;

      ///////////////////////////////////////////////////////////////////
      // Protected methods
      ///////////////////////////////////////////////////////////////////

      MsgStream&    dumpConditions(MsgStream   & out) const;
      MsgStream&    dumpEvent     (MsgStream   & out) const;

      void buildFrameWork()                                      ;
      void buildBeamFrameWork()                                  ;

      SiSpacePointForSeedITK* newSpacePoint
	(Trk::SpacePoint*const&)                                 ;
      SiSpacePointForSeedITK* newSpacePoint
	(Trk::SpacePoint*const&,float*,bool)                     ;

      void newSeed
      (SiSpacePointForSeedITK*&,SiSpacePointForSeedITK*&,float)  ; 

      void newOneSeed
	(SiSpacePointForSeedITK*&,SiSpacePointForSeedITK*&,
	 SiSpacePointForSeedITK*&,float,float)                   ;

      void newOneSeedQ
	(SiSpacePointForSeedITK*&,SiSpacePointForSeedITK*&,
	 SiSpacePointForSeedITK*&,float,float)                   ;

      void newOneSeedWithCurvaturesComparisonPPP
	(SiSpacePointForSeedITK*&,SiSpacePointForSeedITK*&,float);
      void newOneSeedWithCurvaturesComparisonSSS
	(SiSpacePointForSeedITK*&,SiSpacePointForSeedITK*&,float);
 
      void fillSeeds       () ;
      void fillLists       () ;
      void fillListsSSS    () ;
      void fillListsPPP    () ;
      void fillListsSSSFast() ;
      void fillListsPPPFast() ;

      void erase           () ;
      void production3Sp   () ;
      void production3SpSSS() ;
      void production3SpPPP() ;

      void production3SpSSS
	(std::vector<InDet::SiSpacePointForSeedITK*>::iterator*,
	 std::vector<InDet::SiSpacePointForSeedITK*>::iterator*,
	 std::vector<InDet::SiSpacePointForSeedITK*>::iterator*,
	 std::vector<InDet::SiSpacePointForSeedITK*>::iterator*,
	 int,int,int&);
      void production3SpPPP
	(std::vector<InDet::SiSpacePointForSeedITK*>::iterator*,
	 std::vector<InDet::SiSpacePointForSeedITK*>::iterator*,
	 std::vector<InDet::SiSpacePointForSeedITK*>::iterator*,
	 std::vector<InDet::SiSpacePointForSeedITK*>::iterator*,
	 int,int,int&);
       void production3SpTrigger
	 (std::vector<InDet::SiSpacePointForSeedITK*>::iterator*,
	  std::vector<InDet::SiSpacePointForSeedITK*>::iterator*,
	  std::vector<InDet::SiSpacePointForSeedITK*>::iterator*,
	  std::vector<InDet::SiSpacePointForSeedITK*>::iterator*,
	  int,int,int&);

      bool newVertices(const std::list<Trk::Vertex>&)             ;
      void findNext()                                             ;
      bool isZCompatible     (float&,float&,float&)               ;
      void convertToBeamFrameWork(Trk::SpacePoint*const&,float*)  ;
      bool isUsed(const Trk::SpacePoint*); 
      
      void pixInform(Trk::SpacePoint*const&,float*);
      void sctInform(Trk::SpacePoint*const&,float*);
      float AzimuthalStep(float,float,float,float);
            
      void sort (FloatInt*,int);
      void QuickSort(FloatInt*,int);
      void SmallSort(FloatInt*,int);
      int  Partition(FloatInt*,int);
      void Middle   (FloatInt*,int);
      
      
      // for validation purpose
      bool                        m_writeNtuple                   ;
      ITHistSvc*                  m_thistSvc                      ;
      TTree*                      m_outputTree                    ;
      std::string                 m_treeName                      ;
      std::string                 m_treeFolder                    ;
      mutable float                       m_d0                            ;
      mutable float                       m_z0                            ;
      mutable float                       m_pt                            ;
      mutable float                       m_eta                           ;
      mutable double                      m_x1                            ;
      mutable double                      m_x2                            ;
      mutable double                      m_x3                            ;
      mutable double                      m_y1                            ;
      mutable double                      m_y2                            ;
      mutable double                      m_y3                            ;
      mutable double                      m_z1                            ;
      mutable double                      m_z2                            ;
      mutable double                      m_z3                            ;
      mutable double                      m_r1                            ;
      mutable double                      m_r2                            ;
      mutable double                      m_r3                            ;
      mutable float                       m_quality                       ;
      mutable int                         m_type                          ;
      mutable double                      m_dzdr_t                        ;
      mutable double                      m_dzdr_b                        ;
      mutable bool                        m_givesTrack                    ;
      mutable float                       m_trackPt                       ;
      mutable float                       m_trackEta                      ;
      mutable long                        m_eventNumber                   ;
      
      

};

  MsgStream&    operator << (MsgStream&   ,const SiSpacePointsSeedMaker_ITK&);
  std::ostream& operator << (std::ostream&,const SiSpacePointsSeedMaker_ITK&); 

  ///////////////////////////////////////////////////////////////////
  // Inline methods
  ///////////////////////////////////////////////////////////////////

  inline const SiSpacePointsSeed* SiSpacePointsSeedMaker_ITK::next()
  {
    if(m_nspoint==3) {
      do {
	while(i_seed!=i_seede) {
	  if((*i_seed++)->set3(*m_seedOutput)) return m_seedOutput;
	}
	findNext();
      }
      while(i_seed!=i_seede);
      return 0;
    }
    else             {
      if(i_seed==i_seede) {findNext(); if(i_seed==i_seede) return 0;} 
      (*i_seed++)->set2(*m_seedOutput); return(m_seedOutput);
    }
    return 0;
  }

  inline bool SiSpacePointsSeedMaker_ITK::isZCompatible  
    (float& Zv,float& R,float& T)
    {
      if(Zv < m_zminU || Zv > m_zmaxU) return false;
      if(!m_isvertex) return true;

      std::set<float>::iterator v=l_vertex.begin(),ve=l_vertex.end(); 

      float dZmin = std::abs((*v)-Zv); 
      for(++v; v!=ve; ++v) {
	float dZ = std::abs((*v)-Zv); if(dZ >= dZmin) break; dZmin=dZ;
      }
      return dZmin < (m_dzver+m_dzdrver*R)*sqrt(1.+T*T);
    }

  ///////////////////////////////////////////////////////////////////
  // New space point for seeds 
  ///////////////////////////////////////////////////////////////////

  inline SiSpacePointForSeedITK* SiSpacePointsSeedMaker_ITK::newSpacePoint
    (Trk::SpacePoint*const& sp) 
    {

      float r[15];
      return newSpacePoint(sp,r,true);

    }


  inline SiSpacePointForSeedITK* SiSpacePointsSeedMaker_ITK::newSpacePoint
    (Trk::SpacePoint*const& sp,float* r,bool usePixSctInform=false)
    {
      SiSpacePointForSeedITK* sps;

      convertToBeamFrameWork(sp,r);

      if(m_checketa) {

	float z = (std::abs(r[2])+m_zmax);
	float x = r[0]*m_dzdrmin     ;
	float y = r[1]*m_dzdrmin     ;
	if((z*z )<(x*x+y*y)) return 0;
      }

      if(m_fastTracking) {

	float R2 =  r[0]*r[0]+r[1]*r[1];
	if(std::abs(r[2]) > 200. && R2 < 2500.       ) return 0;
	if(std::abs(r[2])-m_zmax > m_dzdrmax*sqrt(R2)) return 0;

      }

      if(usePixSctInform){

	if(!sp->clusterList().second) pixInform(sp,r);
	else sctInform(sp,r);

      }

      if(i_spforseed!=l_spforseed.end()) {
	sps = (*i_spforseed++); sps->set(sp,r); 
      }
      else                               {
	l_spforseed.push_back((sps=new SiSpacePointForSeedITK(sp,r)));
	i_spforseed = l_spforseed.end();	
      }
      
      return sps;
    }

  ///////////////////////////////////////////////////////////////////
  // New 2 space points seeds 
  ///////////////////////////////////////////////////////////////////

  inline void SiSpacePointsSeedMaker_ITK::newSeed
    (SiSpacePointForSeedITK*& p1,SiSpacePointForSeedITK*& p2, float z) 
    {
      SiSpacePointForSeedITK* p3 = 0;

      if(i_seede!=l_seeds.end()) {
	SiSpacePointsProSeedITK* s = (*i_seede++);
	s->set(p1,p2,p3,z);
      }
      else                  {
	l_seeds.push_back(new SiSpacePointsProSeedITK(p1,p2,p3,z));
	i_seede = l_seeds.end(); 
      }
    }

  ///////////////////////////////////////////////////////////////////
  // The procedure sorts the elements into ascending order.
  ///////////////////////////////////////////////////////////////////
  
  inline void SiSpacePointsSeedMaker_ITK::sort(FloatInt* s,int n) 
  {
    if(m_fastTracking){
      if(n>1) QuickSort(s,n-1);
    }
    else{
      std::sort(s,s+n,[](const FloatInt &a,const FloatInt&b)->bool {return a.Fl < b.Fl;});
    }
  }
 } // end of name space

#endif // SiSpacePointsSeedMaker_ITK_H

