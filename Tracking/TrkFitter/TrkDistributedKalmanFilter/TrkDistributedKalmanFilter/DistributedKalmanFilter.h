/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// DistributedKalmanFilter.h
//   Header file for class DistributedKalmanFilter
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: Dmitry Emeliyanov, RAL
// D.Emeliyanov@rl.ac.uk
///////////////////////////////////////////////////////////////////
#ifndef __TRK_DISTRIBUTED_KALMAN_FILTER__
#define __TRK_DISTRIBUTED_KALMAN_FILTER__

#include "TrkFitterUtils/FitterTypes.h"
#include "TrkFitterInterfaces/ITrackFitter.h"
//#include "GaudiKernel/AlgTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "TrkEventPrimitives/ParticleHypothesis.h"

#include "StoreGate/ReadCondHandleKey.h"
#include "MagFieldConditions/AtlasFieldCacheCondObj.h"

#include <vector>

class IAlgTool;
class AtlasDetectorID;

namespace Trk {
	
    class Track;                   //!> ATLAS standard track class
    class TrkBaseNode;             //!> Base class for track filtering nodes
    class TrkTrackState;           //!> Simple track state: 5 parameters + covariance
    class TrkPlanarSurface;        //!> Simple class for planar surface description
    class TrackStateOnSurface;
    class IMagneticFieldTool;
    class IRIO_OnTrackCreator;     //!> Interface for creation of ROT
    class IExtrapolator;           //!> Extrapolation Tool
    class MeasuredPerigee;


    class DistributedKalmanFilter : virtual public ITrackFitter, public AthAlgTool { 
    public:
	// standard AlgTool methods
	DistributedKalmanFilter(const std::string&,const std::string&,const IInterface*);
	virtual ~DistributedKalmanFilter();
		
	// standard Athena methods
	StatusCode initialize();
	StatusCode finalize();

	// Filter settings:
	//			RunOutlierRemoval - use logic to remove bad hits
	//			ParticleHypothesis   - allow for multiple scattering and energy loss?

	virtual Track* fit(const Track&,
			   const RunOutlierRemoval  runOutlier=false,
			   const ParticleHypothesis matEffects=Trk::nonInteracting) const;

	// refit a track adding a PrepRawDataSet -- use base class method.
	virtual Track* fit(const Track&,
			   const PrepRawDataSet&,
			   const RunOutlierRemoval,
			   const ParticleHypothesis) const {return NULL;}
	
	// fit a set of PrepRawData objects
	virtual Track* fit(const PrepRawDataSet&,
			   const TrackParameters&   estimatedParametersNearOrigine,
			   const RunOutlierRemoval  runOutlier=false,
			   const ParticleHypothesis matEffects=Trk::nonInteracting) const;
	
	// fit a set of RIO_OnTrack objects
	virtual Track* fit(const RIO_OnTrackSet&,
			   const TrackParameters&   estimatedParametersNearOrigine,
			   const RunOutlierRemoval  runOutlier=false,
			   const ParticleHypothesis matEffects=Trk::nonInteracting) const;
        
        virtual Track* fit(const MeasurementSet&,
                           const TrackParameters& estim,
                           const RunOutlierRemoval runo=false, 
                           const ParticleHypothesis 
			   mat=Trk::nonInteracting) const;

	virtual Track* fit(const Track&,
			   const MeasurementSet&,
			   const RunOutlierRemoval runOutlier=false,
			   const ParticleHypothesis matEffects=Trk::nonInteracting) const;

	virtual Track* fit(const Track&,
			   const Track&,
			   const RunOutlierRemoval,
			   const ParticleHypothesis) const {return NULL;}
                        
private:
	///////////////////////////////////////////////////////////////////
	// Private functions:
	///////////////////////////////////////////////////////////////////

	Trk::TrkTrackState* extrapolate(TrkTrackState*,
                                        TrkPlanarSurface*,
                                        TrkPlanarSurface*,
                                        MagField::AtlasFieldCache &fieldCache) const;
        bool runForwardKalmanFilter(TrkTrackState*,
                                    MagField::AtlasFieldCache &fieldCache) const;
	void runSmoother() const;
	int  findOutliers(double) const;
	void calculateLRsolution() const;
	TrackStateOnSurface* createTrackStateOnSurface(TrkBaseNode*) const;
	void deleteNodes() const;
	void deleteSurfaces() const;
	void deleteTrackStates() const;
	void report();
	void report(char fileName[]);
	void getMagneticField(double[3],
                              double*,
                              MagField::AtlasFieldCache &fieldCache) const;
	void matrixInversion5x5(double a[5][5]) const;
	Perigee* createMeasuredPerigee(TrkTrackState*) const;

	void numericalJacobian(TrkTrackState*,
                               TrkPlanarSurface*,
                               TrkPlanarSurface*,
                               double A[5][5],
                               MagField::AtlasFieldCache &fieldCache) const;
	double integrate(double Rk[5],
                         TrkPlanarSurface* pSB,
                         TrkPlanarSurface* pSE,
                         double* Rf,
                         MagField::AtlasFieldCache &fieldCache) const;

	///////////////////////////////////////////////////////////////////
	// Private data:
	///////////////////////////////////////////////////////////////////

	//    RunOutlierRemoval             m_option_runOutlier;
	//    ParticleHypothesis               m_option_matEffects;

	ToolHandle<IRIO_OnTrackCreator>          m_ROTcreator;
	ToolHandle<IExtrapolator>                m_extrapolator;
	const AtlasDetectorID*      m_idHelper;

        SG::ReadCondHandleKey<AtlasFieldCacheCondObj> m_fieldCacheCondObjInputKey
           {this, "AtlasFieldCacheCondObj", "fieldCondObj", "Name of the Magnetic Field conditions object key"};

	std::vector<TrkBaseNode*>* m_pvpNodes;
	std::vector<TrkPlanarSurface*>* m_pvpSurfaces;
	std::vector<TrkTrackState*>* m_pvpTrackStates;

	// ME temporary fix
	std::vector<double> m_option_sortingRefPoint;

    };

   inline
   void Trk::DistributedKalmanFilter::getMagneticField(double gP[3],
                                                       double* pB,
                                                       MagField::AtlasFieldCache &fieldCache) const
   {
      pB[0]=0.0;pB[1]=0.0;pB[2]=0.0;
      double field[3];
      fieldCache.getField(gP,field);//field is returned in kT
      for(int i=0;i<3;i++) pB[i]=field[i]/Gaudi::Units::kilogauss;//convert to kG
   }

} // end of namespace

#endif
