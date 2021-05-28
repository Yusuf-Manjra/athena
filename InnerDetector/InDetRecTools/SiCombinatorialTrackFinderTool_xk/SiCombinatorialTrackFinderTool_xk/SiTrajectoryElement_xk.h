/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////
//  Header file for class SiTrajectoryElement_xk
/////////////////////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
/////////////////////////////////////////////////////////////////////////////////
// Class for trajector elements in Pixels and SCT
/////////////////////////////////////////////////////////////////////////////////
// Version 1.0 3/10/2004 I.Gavrilenko
/////////////////////////////////////////////////////////////////////////////////

#ifndef SiTrajectoryElement_xk_H
#define SiTrajectoryElement_xk_H

#include "InDetPrepRawData/SiClusterCollection.h"
#include "TrkTrack/TrackStateOnSurface.h"
#include "TrkPatternParameters/PatternTrackParameters.h"
#include "TrkPatternParameters/NoiseOnSurface.h"

#include "SiCombinatorialTrackFinderTool_xk/SiTools_xk.h"
#include "SiCombinatorialTrackFinderTool_xk/SiClusterLink_xk.h"
#include "SiCombinatorialTrackFinderTool_xk/SiDetElementBoundaryLink_xk.h"

#include "TrkRIO_OnTrack/RIO_OnTrack.h"

namespace InDet{

  class SiTrajectoryElement_xk
    {
      ///////////////////////////////////////////////////////////////////
      // Public methods:
      ///////////////////////////////////////////////////////////////////
      
    public:
      
      SiTrajectoryElement_xk();
      SiTrajectoryElement_xk(const SiTrajectoryElement_xk&);
      ~SiTrajectoryElement_xk();
      SiTrajectoryElement_xk& operator  = (const SiTrajectoryElement_xk&);

      const int&  detstatus   () const {return m_detstatus;}
      const int&  inside      () const {return m_inside;    }
      const int&  ndist       () const {return m_ndist ;    }
      const int&  nlinksF     () const {return m_nlinksF;   }
      const int&  nlinksB     () const {return m_nlinksB;   }
      const int&  nholesF     () const {return m_nholesF;   }
      const int&  nholesB     () const {return m_nholesB;   }
      const int&  dholesF     () const {return m_dholesF;   }
      const int&  dholesB     () const {return m_dholesB;   }
      const int&  nclustersF  () const {return m_nclustersF;}
      const int&  nclustersB  () const {return m_nclustersB;}
      const int&  npixelsB    () const {return m_npixelsB;  }
      const bool& stereo      () const {return m_stereo;    }
      const int&  status      () const {return m_status;    }
      const int&  noiseModel  () const {return m_noisemodel;}
      const int&  ndf         () const {return m_ndf;       }
      const int&  ndfF        () const {return m_ndfF;      }
      const int&  ndfB        () const {return m_ndfB;      }
      const int&  ntsos       () const {return m_ntsos;     }
      const double& step      () const {return m_step;      }
      const double& dist      () const {return m_dist;      }
      bool        isBarrel    () const {return m_detelement->isBarrel();}

      const InDetDD::SiDetectorElement* detElement  () const {return m_detelement;  }  
      const InDet::SiCluster*           cluster     () const {return m_cluster   ;  }  
      const InDet::SiCluster*           clusterOld  () const {return m_clusterOld;  }  
      const InDet::SiCluster*           clusterNoAdd() const {return m_clusterNoAdd;} 
      Trk::TrackStateOnSurface*         tsos (int i);

      bool isNextClusterHoleB(bool&,double&);
      bool isNextClusterHoleF(bool&,double&);
      int getSCTLayerSide();
      int getSCTLayer();
      bool isPixel();
      bool isSCT();

      ///////////////////////////////////////////////////////////////////
      // Methods update with cluster information
      ///////////////////////////////////////////////////////////////////
 
      bool addCluster(Trk::PatternTrackParameters&,Trk::PatternTrackParameters&,double&);
      bool addCluster(Trk::PatternTrackParameters&,Trk::PatternTrackParameters&);
      bool addClusterPrecise(Trk::PatternTrackParameters&,Trk::PatternTrackParameters&,double&);
      bool addClusterPreciseWithCorrection(Trk::PatternTrackParameters&,Trk::PatternTrackParameters&,Trk::PatternTrackParameters&,double&);
 
      bool combineStates(Trk::PatternTrackParameters&,
                         Trk::PatternTrackParameters&,
                         Trk::PatternTrackParameters&);

      ///////////////////////////////////////////////////////////////////
      // Methods noise calculation
      ///////////////////////////////////////////////////////////////////

      // if rad_length==-1, then the private member m_radlength is used.
      void noiseProduction(int,const Trk::PatternTrackParameters&, double rad_length = -1.);
      void noiseProductionWithMomentum(int);
      void noiseInitiate();

      bool addNextClusterB();
      bool addNextClusterF();
      bool addNextClusterB(SiTrajectoryElement_xk&,const InDet::SiCluster*);
      bool addNextClusterF(SiTrajectoryElement_xk&,const InDet::SiCluster*);
      void setCluster(const InDet::SiCluster*);
      void setClusterB(const InDet::SiCluster*,double);

      void setParametersB(Trk::PatternTrackParameters&); 
      void setParametersF(Trk::PatternTrackParameters&); 
      void setNdist(int);

      int   numberClusters() const;
      const double& xi2F      () const {return m_xi2F      ;}
      const double& xi2B      () const {return m_xi2B      ;}
      const double& xi2totalF () const {return m_xi2totalF ;}
      const double& xi2totalB () const {return m_xi2totalB ;}
      const double& radlength () const {return m_radlength ;}
      const double& radlengthN() const {return m_radlengthN;}
      const double& energylose() const {return m_energylose;}

      const Trk::PatternTrackParameters& parametersPF() const {return m_parametersPF;} 
      const Trk::PatternTrackParameters& parametersUF() const {return m_parametersUF;} 
      const Trk::PatternTrackParameters& parametersPB() const {return m_parametersPB;} 
      const Trk::PatternTrackParameters& parametersUB() const {return m_parametersUB;} 
      const Trk::PatternTrackParameters& parametersSM() const {return m_parametersSM;} 

      const Trk::Surface*  surface()  const {return m_surface;}
      const InDet::SiClusterLink_xk& linkF (int i) const {return m_linkF[i];}
      const InDet::SiClusterLink_xk& linkB (int i) const {return m_linkB[i];}
 
      ///////////////////////////////////////////////////////////////////
      // Main methods
      ///////////////////////////////////////////////////////////////////

      bool set(int,
               const InDet::SiDetElementBoundaryLink_xk*&       ,
               const InDet::SiClusterCollection::const_iterator&, 
               const InDet::SiClusterCollection::const_iterator&,
               const InDet::SiCluster*);

      bool setDead(const Trk::Surface*);
      void setDeadRadLength(Trk::PatternTrackParameters&);
      void setTools(const InDet::SiTools_xk*); 
      void setParameters(); 
      void bremNoiseModel();

      ///////////////////////////////////////////////////////////////////
      // Update first trajectory element on the trajectory with track 
      // parameters
      ///////////////////////////////////////////////////////////////////

      bool firstTrajectorElement(const Trk::TrackParameters&);
      bool firstTrajectorElement();
      bool firstTrajectorElementWithCorrection();

      ///////////////////////////////////////////////////////////////////
      // Forward propagation  
      ///////////////////////////////////////////////////////////////////

      bool ForwardPropagationWithoutSearch(SiTrajectoryElement_xk&);
      bool ForwardPropagationWithoutSearchPreciseWithCorrection(SiTrajectoryElement_xk&);
      bool ForwardPropagationWithSearch(SiTrajectoryElement_xk&);
      bool ForwardPropagationForClusterSeach(int,
                                             const Trk::TrackParameters&,
                                             const InDet::SiDetElementBoundaryLink_xk*&       ,
                                             const InDet::SiClusterCollection::const_iterator&, 
                                             const InDet::SiClusterCollection::const_iterator&);
      
      void CloseClusterSeach(Trk::PatternTrackParameters&,
                             const InDet::SiDetElementBoundaryLink_xk*&       ,
                             const InDet::SiClusterCollection::const_iterator&, 
                             const InDet::SiClusterCollection::const_iterator&);
      
      void eraseClusterForwardPropagation();

      ///////////////////////////////////////////////////////////////////
      // For last detector element initiate smoother  
      ///////////////////////////////////////////////////////////////////

      bool lastTrajectorElement();
      bool lastTrajectorElementPrecise();

      ///////////////////////////////////////////////////////////////////
      // Backward propagation for smoother  
      ///////////////////////////////////////////////////////////////////

      bool BackwardPropagationSmoother(SiTrajectoryElement_xk&,bool);
      bool BackwardPropagationPrecise (SiTrajectoryElement_xk&);
      bool BackwardPropagationFilter  (SiTrajectoryElement_xk&);

      ///////////////////////////////////////////////////////////////////
      // Search clusters compatible with track  
      ///////////////////////////////////////////////////////////////////

      int searchClusters                (Trk::PatternTrackParameters&,SiClusterLink_xk*);
      int searchClustersWithoutStereoPIX(Trk::PatternTrackParameters&,SiClusterLink_xk*);
      int searchClustersWithoutStereoSCT(Trk::PatternTrackParameters&,SiClusterLink_xk*);
      int searchClustersWithStereo      (Trk::PatternTrackParameters&,SiClusterLink_xk*);

      ///////////////////////////////////////////////////////////////////
      // Search only clusters without track assosiation compatible with track  
      ///////////////////////////////////////////////////////////////////

      int searchClustersWithoutStereoAssPIX(Trk::PatternTrackParameters&,SiClusterLink_xk*);
      int searchClustersWithoutStereoAssSCT(Trk::PatternTrackParameters&,SiClusterLink_xk*);
      int searchClustersWithStereoAss      (Trk::PatternTrackParameters&,SiClusterLink_xk*);

      ///////////////////////////////////////////////////////////////////
      // Is difference between forward and backward propagation   
      ///////////////////////////////////////////////////////////////////
      
      bool difference() const;

      ///////////////////////////////////////////////////////////////////
      // TrackStateOnSurface production  
      ///////////////////////////////////////////////////////////////////

      Trk::TrackStateOnSurface* trackStateOnSurface(bool,bool,bool,int);
      Trk::TrackStateOnSurface* trackSimpleStateOnSurface(bool,bool,int);
      Trk::TrackStateOnSurface* trackPerigeeStateOnSurface();
      Trk::MeasurementBase* simpleMeasurement(const InDet::SiCluster*);

      ///////////////////////////////////////////////////////////////////
      // TrackParameters production  
      ///////////////////////////////////////////////////////////////////

      const Trk::TrackParameters* trackParameters(bool,int);
      const Trk::TrackParameters* trackParametersWithNewDirection(bool,int);
      const Trk::TrackParameters* trackParameters(Trk::PatternTrackParameters&,bool);

      ///////////////////////////////////////////////////////////////////
      // Initiate state
      ///////////////////////////////////////////////////////////////////

      bool initiateState(Trk::PatternTrackParameters&,Trk::PatternTrackParameters&);
      bool initiateStatePrecise(Trk::PatternTrackParameters&,Trk::PatternTrackParameters&);
      bool initiateStateWithCorrection(Trk::PatternTrackParameters&,Trk::PatternTrackParameters&,Trk::PatternTrackParameters&);
      void precisePosCov(Trk::PatternTrackParameters&);

      ///////////////////////////////////////////////////////////////////
      // Last detector elements with clusters
      ///////////////////////////////////////////////////////////////////
      
      void lastActive();

      ///////////////////////////////////////////////////////////////////
      // Step of trajectory calculation
      ///////////////////////////////////////////////////////////////////
      
      double step(SiTrajectoryElement_xk&);
      double stepToPerigee();

      ///////////////////////////////////////////////////////////////////
      // Global position of the track parameters
      ///////////////////////////////////////////////////////////////////

      Amg::Vector3D globalPosition();

      ///////////////////////////////////////////////////////////////////
      // Quality of the trajectory element
      ///////////////////////////////////////////////////////////////////

      double quality(int&) const;

      ///////////////////////////////////////////////////////////////////
      // Propagate parameters with covariance
      ///////////////////////////////////////////////////////////////////

      bool propagate(Trk::PatternTrackParameters  &,
                     Trk::PatternTrackParameters  &,
                     double                       &);

      bool propagateUsingTool(Trk::PatternTrackParameters  &,
			      Trk::PatternTrackParameters  &,
			      double                       &);

      ///////////////////////////////////////////////////////////////////
      // Propagate parameters without covariance
      ///////////////////////////////////////////////////////////////////

      bool propagateParameters(Trk::PatternTrackParameters&,
                               Trk::PatternTrackParameters&,
                               double                     &);

      ///////////////////////////////////////////////////////////////////
      // Work methods for propagation
      ///////////////////////////////////////////////////////////////////

      bool transformPlaneToGlobal(bool,Trk::PatternTrackParameters&,double*);
      bool transformGlobalToPlane(bool,double*,Trk::PatternTrackParameters&,Trk::PatternTrackParameters&);
      bool rungeKuttaToPlane(bool,double*);
      bool straightLineStepToPlane(bool,double*);

    protected:
      
      ///////////////////////////////////////////////////////////////////
      // Protected Data
      ///////////////////////////////////////////////////////////////////
      
      bool                                        m_stereo      ;
      bool                                        m_utsos[3]    ;
      bool                                        m_fieldMode   ;
      bool                                        m_useassoTool ;  // Use assosiation tool
      int                                         m_status      ;  
      int                                         m_detstatus   ; // 0 (no clusters) 
      int                                         m_inside      ;
      int                                         m_ndist       ;
      int                                         m_nlinksF     ;
      int                                         m_nlinksB     ;
      int                                         m_nholesF     ;
      int                                         m_nholesB     ;
      int                                         m_dholesF     ;
      int                                         m_dholesB     ;
      int                                         m_nclustersF  ;
      int                                         m_nclustersB  ;
      int                                         m_npixelsB    ;
      int                                         m_noisemodel  ;
      int                                         m_ndf         ;
      int                                         m_ndfF        ;
      int                                         m_ndfB        ;
      int                                         m_ntsos       ;
      int                                         m_maxholes    ;
      int                                         m_maxdholes   ;
      double                                      m_dist        ;
      double                                      m_xi2F        ;
      double                                      m_xi2B        ;
      double                                      m_xi2totalF   ;
      double                                      m_xi2totalB   ;
      double                                      m_radlength   ;
      double                                      m_radlengthN  ;
      double                                      m_energylose  ;
      double                                      m_step        ;
      double                                      m_xi2max      ;
      double                                      m_xi2maxNoAdd ;
      double                                      m_xi2maxlink  ;  
      double                                      m_xi2multi    ;
      double                                      m_Tr[13]      ;
      double                                      m_A [ 3]      ;
      double                                      m_invMoment   ;
      const InDetDD::SiDetectorElement*           m_detelement  ;
      const InDet::SiDetElementBoundaryLink_xk*   m_detlink     ;
      const Trk::Surface*                         m_surface     ;
      const PixelID*                              m_pixIdHelper ;
      const SCT_ID*                               m_sctIdHelper ;
      InDet::SiClusterCollection ::const_iterator m_sibegin     ;
      InDet::SiClusterCollection ::const_iterator m_siend       ;
      const InDet::SiCluster*                     m_cluster     ;
      const InDet::SiCluster*                     m_clusterOld  ;
      const InDet::SiCluster*                     m_clusterNoAdd;
      Trk::PatternTrackParameters                 m_parametersPF; 
      Trk::PatternTrackParameters                 m_parametersUF; 
      Trk::PatternTrackParameters                 m_parametersPB; 
      Trk::PatternTrackParameters                 m_parametersUB;
      Trk::PatternTrackParameters                 m_parametersSM;
      InDet::SiClusterLink_xk                     m_linkF[10]   ; 
      InDet::SiClusterLink_xk                     m_linkB[10]   ; 
      Trk::NoiseOnSurface                         m_noise       ; 
      const InDet::SiTools_xk*                    m_tools       ; 
      MagField::IMagFieldSvc*                     m_fieldService;
      Trk::IPatternParametersUpdator*             m_updatorTool ;
      Trk::IPatternParametersPropagator*          m_proptool    ;
      Trk::IPRD_AssociationTool*                  m_assoTool    ; 
      Trk::IRIO_OnTrackCreator*                   m_riotool     ;
      Trk::TrackStateOnSurface*                   m_tsos[3]     ;
      Amg::MatrixX                                m_covariance  ;
      Amg::Vector2D                               m_position    ;
      
      static constexpr double s_oneOverTwelve{0.08333}           ;

      ///////////////////////////////////////////////////////////////////
      // Methods
      ///////////////////////////////////////////////////////////////////
      
      void patternCovariances(const InDet::SiCluster*,Amg::MatrixX&);
    };
  
  /////////////////////////////////////////////////////////////////////////////////
  // Inline methods
  /////////////////////////////////////////////////////////////////////////////////

  inline SiTrajectoryElement_xk::SiTrajectoryElement_xk()
    {
      m_detstatus  =-1  ;
      m_status     = 0  ;
      m_nlinksF    = 0  ;
      m_nlinksB    = 0  ;
      m_ndist      = 0  ;
      m_radlength  = .03;
      m_radlengthN = .03;
      m_energylose = .4 ;
      m_tools      = 0  ;
      m_noisemodel = 0  ; 
      m_covariance.resize(2,2);
      m_covariance<<0.,0.,0.,0.;
      m_ndf         = 0 ;
      m_ndfF        = 0 ;
      m_ndfB        = 0 ;
      m_ntsos       = 0 ;
      m_maxholes    = 0 ;
      m_maxdholes   = 0 ;
      m_xi2F        = 0.;
      m_xi2B        = 0.;
      m_xi2totalF   = 0.;
      m_xi2totalB   = 0.;
      m_step        = 0.;
      m_xi2max      = 0.;
      m_dist        = 0.;
      m_xi2maxNoAdd = 0.;
      m_xi2maxlink  = 0.;  
      m_xi2multi    = 0.;
      m_invMoment   = 0.;
      m_detelement  = 0 ; 
      m_detlink     = 0 ;
      m_surface     = 0 ;
      m_pixIdHelper = 0 ;
      m_sctIdHelper = 0 ;
      m_cluster     = 0 ;
      m_clusterOld  = 0 ;
      m_clusterNoAdd= 0 ;
      m_fieldService= 0 ;
      m_updatorTool = 0 ;
      m_proptool    = 0 ;
      m_assoTool    = 0 ;
      m_riotool     = 0 ;
      m_inside      = 0 ;
      m_nholesF     = 0 ;
      m_nholesB     = 0 ;
      m_dholesF     = 0 ;
      m_dholesB     = 0 ;
      m_nclustersF  = 0 ;
      m_nclustersB  = 0 ;
      m_npixelsB    = 0 ;
      m_stereo      = false;
      m_fieldMode   = false;
      m_useassoTool = false;

      m_covariance(0); 
      m_tsos[0]=m_tsos[1]=m_tsos[2]=0; 
      m_A   [0]=m_A   [1]=m_A   [2]=0;
      for(int i=0; i!=13; ++i) m_Tr[i] = 0;
   }

  inline SiTrajectoryElement_xk::SiTrajectoryElement_xk(const SiTrajectoryElement_xk& E)
    {
      *this          = E;
    }
  
  inline SiTrajectoryElement_xk& SiTrajectoryElement_xk::operator = 
    (const SiTrajectoryElement_xk& E) 
    {
      if(&E==this) return(*this);

      m_fieldMode    = E.m_fieldMode   ;
      m_status       = E.m_status      ;
      m_detstatus    = E.m_detstatus   ;
      m_inside       = E.m_inside      ;
      m_ndist        = E.m_ndist       ;
      m_stereo       = E.m_stereo      ;
      m_detelement   = E.m_detelement  ;
      m_detlink      = E.m_detlink     ;
      m_surface      = E.m_surface     ;
      m_pixIdHelper  = E.m_pixIdHelper ;
      m_sctIdHelper  = E.m_sctIdHelper ;
      m_sibegin      = E.m_sibegin     ;
      m_siend        = E.m_siend       ; 
      m_cluster      = E.m_cluster     ;
      m_clusterOld   = E.m_clusterOld  ;
      m_clusterNoAdd = E.m_clusterNoAdd;
      m_parametersPF = E.m_parametersPF;
      m_parametersUF = E.m_parametersUF;
      m_parametersPB = E.m_parametersPB;
      m_parametersUB = E.m_parametersUB;
      m_parametersSM = E.m_parametersSM;
      m_dist         = E.m_dist        ;
      m_xi2F         = E.m_xi2F        ;
      m_xi2B         = E.m_xi2B        ;
      m_xi2totalF    = E.m_xi2totalF   ;
      m_xi2totalB    = E.m_xi2totalB   ;
      m_radlength    = E.m_radlength   ;
      m_radlengthN   = E.m_radlengthN  ;
      m_energylose   = E.m_energylose  ;
      m_step         = E.m_step        ;
      m_invMoment   =  E.m_invMoment   ;
      m_nlinksF      = E.m_nlinksF     ;
      m_nlinksB      = E.m_nlinksB     ;
      m_nholesF      = E.m_nholesF     ;
      m_nholesB      = E.m_nholesB     ;
      m_dholesF      = E.m_dholesF     ;
      m_dholesB      = E.m_dholesB     ;
      m_noisemodel   = E.m_noisemodel  ;
      m_ndf          = E.m_ndf         ;
      m_ndfF         = E.m_ndfF        ;
      m_ndfB         = E.m_ndfB        ;
      m_ntsos        = E.m_ntsos       ;
      m_nclustersF   = E.m_nclustersF  ; 
      m_nclustersB   = E.m_nclustersB  ;
      m_npixelsB     = E.m_npixelsB    ;
      m_noise        = E.m_noise       ;
      m_tools        = E.m_tools       ;
      m_covariance   = E.m_covariance  ;
      m_position     = E.m_position    ;
      for(int i=0; i!=m_nlinksF; ++i) {m_linkF[i]=E.m_linkF[i];}
      for(int i=0; i!=m_nlinksB; ++i) {m_linkB[i]=E.m_linkB[i];}
      for(int i=0; i!=m_ntsos  ; ++i) {m_tsos [i]=E.m_tsos [i];}
      for(int i=0; i!=m_ntsos  ; ++i) {m_utsos[i]=E.m_utsos [i];}
      for(int i=0; i!=3        ; ++i) m_A [i]=E.m_A [i];
      for(int i=0; i!=13       ; ++i) m_Tr[i]=E.m_Tr[i];
      return(*this);
    }

  inline SiTrajectoryElement_xk::~SiTrajectoryElement_xk() {}

  inline int SiTrajectoryElement_xk::numberClusters() const
    {
      int n = 0;
      if(m_detstatus<=0) return n;

      InDet::SiClusterCollection::const_iterator p =  m_sibegin;
      for(; p!=m_siend; ++p) ++n;
      return n;
    }
  
  ///////////////////////////////////////////////////////////////////
  // Search clusters compatible with track  
  ///////////////////////////////////////////////////////////////////

  inline int SiTrajectoryElement_xk::searchClusters
    (Trk::PatternTrackParameters& Tp,SiClusterLink_xk* L) 
    {
      if(!m_useassoTool) {
        if(!m_stereo) {
          if(m_ndf == 2) { return searchClustersWithoutStereoPIX(Tp,L); }
          else           { return searchClustersWithoutStereoSCT(Tp,L); }
        } else           { return searchClustersWithStereo      (Tp,L); }
      }
      
      if(!m_stereo) {
        if(m_ndf == 2) { return searchClustersWithoutStereoAssPIX(Tp,L); }
        else           { return searchClustersWithoutStereoAssSCT(Tp,L); }
      } else           { return searchClustersWithStereoAss      (Tp,L); }
    }

  inline bool SiTrajectoryElement_xk::difference() const
    {
      if( m_cluster == m_clusterOld && m_status == 3)  { return false; }
      else                                             { return true;  }
    }

  /////////////////////////////////////////////////////////////////////////////////
  // Test for next compatible cluster
  /////////////////////////////////////////////////////////////////////////////////

  inline bool SiTrajectoryElement_xk::isNextClusterHoleB(bool& cl,double& X)
    {
      cl = false             ;
      X  = m_xi2totalB-m_xi2B;

      if(m_nlinksB >  1 && m_linkB[1].xi2() <= m_xi2max) {
        X+=m_linkB[1].xi2();
        cl = true; return true;
      }

      if(m_inside < 0) {
        if(m_nholesB < m_maxholes && m_dholesB < m_maxdholes) return true;
        return false;
      }
      return true;
    }

  inline bool SiTrajectoryElement_xk::isNextClusterHoleF(bool& cl,double& X)
    {
      cl = false             ;
      X  = m_xi2totalF-m_xi2F;

     if(m_detstatus == 2) return false;
      if(m_nlinksF >  1 && m_linkF[1].xi2() <= m_xi2max) {
        X+=m_linkF[1].xi2();
        cl = true; return true;
      }

      if(m_inside < 0) {
        if(m_nholesF < m_maxholes && m_dholesF < m_maxdholes) return true;
        return false;
      }
      return true;
    }

  inline void SiTrajectoryElement_xk::setCluster(const InDet::SiCluster* Cl)
    {
      m_cluster = Cl;
    }
    
  inline void SiTrajectoryElement_xk::setClusterB(const InDet::SiCluster* Cl,double Xi2)
    {
      m_cluster = Cl ;
      m_status  = 2  ;
      m_xi2B    = Xi2;
    }
  

  inline void SiTrajectoryElement_xk::setParametersB(Trk::PatternTrackParameters& P)
  {
    m_parametersUB = P;
  } 

  inline void SiTrajectoryElement_xk::setParametersF(Trk::PatternTrackParameters& P)
  {
    m_parametersUF = P;
  }

  inline void SiTrajectoryElement_xk::setNdist(int n)
  {
    m_ndist = n;
  }
 
  /////////////////////////////////////////////////////////////////////////////////
  // Add pixel or SCT cluster to pattern track parameters with Xi2 calculation
  /////////////////////////////////////////////////////////////////////////////////
  
  inline bool SiTrajectoryElement_xk::addCluster(Trk::PatternTrackParameters& Ta,
                                                 Trk::PatternTrackParameters& Tb,
                                                 double& Xi2) {
    int N;
    patternCovariances(m_cluster,m_covariance);

    if(m_ndf==1) {
      return m_updatorTool->addToStateOneDimension(Ta,m_cluster->localPosition(),m_covariance,Tb,Xi2,N);
    } 
    else return m_updatorTool->addToState(Ta,m_cluster->localPosition(),m_covariance,Tb,Xi2,N);
  }

  /////////////////////////////////////////////////////////////////////////////////
  // Add pixel or SCT cluster to pattern track parameters without Xi2 calculation
  /////////////////////////////////////////////////////////////////////////////////

  inline bool SiTrajectoryElement_xk::addCluster(Trk::PatternTrackParameters& Ta,
                                                 Trk::PatternTrackParameters& Tb) {

    patternCovariances(m_cluster,m_covariance);
    
    if(m_ndf==1) {
      return m_updatorTool->addToStateOneDimension(Ta,m_cluster->localPosition(),m_covariance,Tb);
    }
    else return m_updatorTool->addToState(Ta,m_cluster->localPosition(),m_covariance,Tb);
  }
  
  /////////////////////////////////////////////////////////////////////////////////
  // Add pixel or SCT cluster to pattern track parameters with Xi2 calculation
  // using precise error
  /////////////////////////////////////////////////////////////////////////////////
  
  inline bool SiTrajectoryElement_xk::addClusterPrecise(Trk::PatternTrackParameters& Ta,
							Trk::PatternTrackParameters& Tb,
							double& Xi2) {
    int N; 
    if(m_ndf==1) {
      return m_updatorTool->addToStateOneDimension(Ta,m_position,m_covariance,Tb,Xi2,N);
    }
    else {
      return m_updatorTool->addToState           (Ta,m_position,m_covariance,Tb,Xi2,N);
    }
  }

  /////////////////////////////////////////////////////////////////////////////////
  // Add two pattern track parameters without Xi2 calculation
  /////////////////////////////////////////////////////////////////////////////////
 
  inline bool SiTrajectoryElement_xk::combineStates(Trk::PatternTrackParameters& Ta,
                                                    Trk::PatternTrackParameters& Tb,
                                                    Trk::PatternTrackParameters& Tc)
    {
      return m_updatorTool->combineStates(Ta,Tb,Tc);
    }

  /////////////////////////////////////////////////////////////////////////////////
  // Propagate pattern track parameters to surface
  /////////////////////////////////////////////////////////////////////////////////

  inline void  SiTrajectoryElement_xk::noiseInitiate()
    {
      m_noisemodel = 1; m_noise.initiate();
    }

  ///////////////////////////////////////////////////////////////////
  // Initiate state
  ///////////////////////////////////////////////////////////////////

  inline bool SiTrajectoryElement_xk::initiateState
    (Trk::PatternTrackParameters& Ta,Trk::PatternTrackParameters& Tb)
    {
      // using pattern covariance for all clusters
      Amg::MatrixX cov; 
      patternCovariances(m_cluster,cov);
      return Tb.initiate(Ta,m_cluster->localPosition(),cov);
    }
  
  ///////////////////////////////////////////////////////////////////
  // Initiate state with cluster correction
  ///////////////////////////////////////////////////////////////////

  inline bool SiTrajectoryElement_xk::initiateStatePrecise
    (Trk::PatternTrackParameters& Ta,Trk::PatternTrackParameters& Tb)
    {
      return Tb.initiate(Ta,m_position,m_covariance);
    }

  ///////////////////////////////////////////////////////////////////
  // Pattern covariances
  ///////////////////////////////////////////////////////////////////

  inline void  SiTrajectoryElement_xk::patternCovariances
    (const InDet::SiCluster* c, Amg::MatrixX& covp)
    {
      covp = c->localCovariance();
      if (m_stereo) return;
      
      covp(0,0) = c->width().phiR(); 
      covp(0,0)*=(covp(0,0)*s_oneOverTwelve);

      if(not m_tools->useFastTracking()) {
        if(covp(0,0) < (c->localCovariance())(0,0)) {
          covp(0,0)=(c->localCovariance())(0,0);
        }
        covp(0,1) = 0.;
        covp(1,0) = 0.;
      }
      
      if(m_ndf==2) {
        covp(1,1)=c->width().z(); 
        covp(1,1)*=(covp(1,1)*s_oneOverTwelve);
        if(not m_tools->useFastTracking()) {
          if(covp(1,1) < (c->localCovariance())(1,1)) {
            covp(1,1)=(c->localCovariance())(1,1);
          }
        }
      }
    }

  ///////////////////////////////////////////////////////////////////
  // Last detector elements with clusters
  ///////////////////////////////////////////////////////////////////

  inline void  SiTrajectoryElement_xk::lastActive()
    {
      m_detstatus = 2;
    }

  inline Trk::TrackStateOnSurface* SiTrajectoryElement_xk::tsos (int i) 
    {
      if(i<0 || i>2) return 0;

      bool us = m_utsos[i]; 
      m_utsos[i] = true;

      if(us) return new Trk::TrackStateOnSurface(*m_tsos[i]);

      return m_tsos[i];  
    }

  ///////////////////////////////////////////////////////////////////
  // Set electrom noise model
  ///////////////////////////////////////////////////////////////////
  
  inline void SiTrajectoryElement_xk::bremNoiseModel()
    {
      m_noisemodel = 2;
    }
    
  ///////////////////////////////////////////////////////////////////
  // Initiate state with cluster correction
  ///////////////////////////////////////////////////////////////////

  inline bool SiTrajectoryElement_xk::initiateStateWithCorrection(Trk::PatternTrackParameters& Tc,
								  Trk::PatternTrackParameters& Ta,
								  Trk::PatternTrackParameters& Tb)
  {
    precisePosCov(Tc); 
    return Tb.initiate(Ta,m_position,m_covariance); 
  }

  /////////////////////////////////////////////////////////////////////////////////
  // Add pixel or SCT cluster to pattern track parameters with Xi2 calculation
  // using precise error
  /////////////////////////////////////////////////////////////////////////////////

  inline bool SiTrajectoryElement_xk::addClusterPreciseWithCorrection(Trk::PatternTrackParameters& Tc,
								      Trk::PatternTrackParameters& Ta,
								      Trk::PatternTrackParameters& Tb,
								      double& Xi2)  
  {
    int N; 
    precisePosCov(Tc); 

    if(m_ndf==1) {
      return m_updatorTool->addToStateOneDimension(Ta,m_position,m_covariance,Tb,Xi2,N);
    }
    else         {
      return m_updatorTool->addToState(Ta,m_position,m_covariance,Tb,Xi2,N);
    }
  }

} // end of name space

#endif // SiTrajectoryElement_xk


