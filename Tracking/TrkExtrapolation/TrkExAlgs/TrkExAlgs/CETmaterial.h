/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CETmaterial.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKEXALGS_CETMATERIAL_H
#define TRKEXALGS_CETMATERIAL_H

// Gaudi includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/IRndmGenSvc.h"
#include "GaudiKernel/RndmGenerators.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/IChronoStatSvc.h"
#include "TrkParameters/TrackParameters.h"
#include <string>


namespace Trk 
{

  class IExtrapolator;
  class Surface;
  class TrackingVolume;
  class TrackingGeometry;

  /** @class CETmaterial

     The ExtrapolatorTest Algorithm runs a number of n test extrapolations
     from randomly distributed Track Parameters to geometry outer boundary and back to perigee

      @author Sarka Todorova <sarka.todorova@cern.ch>
  */  

  class CETmaterial : public AthAlgorithm
    {
    public:

       /** Standard Athena-Algorithm Constructor */
       CETmaterial(const std::string& name, ISvcLocator* pSvcLocator);
       /** Default Destructor */
       ~CETmaterial();

       /** standard Athena-Algorithm method */
       StatusCode          initialize();
       /** standard Athena-Algorithm method */
       StatusCode          execute();
       /** standard Athena-Algorithm method */
       StatusCode          finalize();

    private:
       void printMat(double th, double ph, double mat,double dtheta=0.,double dphi=0.) const;
       void printMatScan(double theta, double phi, double r, double z, double mat, const std::string& name) const;
       void printMatPrec(double theta, double phi, const Trk::TrackParameters*, const Trk::TrackParameters*, 
			 double mat, int id, const std::string& name);
       void printMatComp(double theta, double phi, const Trk::TrackParameters* currPar, const std::string& name, double mat, double matApp,double dx, double dy) const;
            
      /** The Extrapolator(s) to be retrieved */
      ToolHandle<IExtrapolator>         m_extrapolator;
      ToolHandle<IExtrapolator>         m_extraprec;

      /** Random Number setup */
      Rndm::Numbers*                    m_gaussDist;
      Rndm::Numbers*                    m_flatDist;

      double                   m_sigmaD0;                    //!< Sigma of distribution for D0
      double                   m_minZ0;                      //!< min Z0
      double                   m_maxZ0;                      //!< max Z0
      double                   m_minEta;                     //!< Minimal eta value
      double                   m_maxEta;                     //!< Maximal eta value
      double                   m_minTheta;                   //!< Minimal theta value
      double                   m_maxTheta;                   //!< Maximal theta value
      double                   m_minPhi;                     //!< Minimal phi value
      double                   m_maxPhi;                     //!< Maximal phi value
      double                   m_minP;                       //!< Minimal p value 
      double                   m_maxP;                       //!< Maximal p value
      double                   m_charge;                     //!< charge
      unsigned int             m_numScan;                    //!< number of scan tracks
      bool                     m_checkStepWise; 
      bool                     m_printMaterial;
      bool                     m_printActive;
      const char*              m_matTotFile;
      const char*              m_matScanFile;
      const char*              m_matActiveFile;
      const char*              m_matCompFile;
      bool                     m_backward;
      bool                     m_domsentry;
      bool                     m_doprecision;
      double                   m_th;
      double                   m_ph;
      int                      m_id;
      double                   m_matSaved;
      Trk::TrackParameters*    m_next;
      Amg::MatrixX*            m_err;

      const Trk::Surface*                m_outerBoundary;
      const Trk::TrackingGeometry*       m_trackingGeometry;
      const Trk::TrackingVolume*         m_msentry;

      int                      m_particleType;               //!< the particle typre for the extrap.

      typedef ServiceHandle<IChronoStatSvc> IChronoStatSvc_t;
      IChronoStatSvc_t              m_chronoStatSvc;      
     
    }; 
} // end of namespace

#endif
