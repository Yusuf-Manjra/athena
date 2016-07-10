/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 local parameter values used during fitter iterations
 ***************************************************************************/

#ifndef TRKIPATFITTERUTILS_FITPARAMETERS_H
# define TRKIPATFITTERUTILS_FITPARAMETERS_H

//<<<<<< INCLUDES                                                       >>>>>>

#include <vector>
#include "EventPrimitives/EventPrimitives.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "TrkAlgebraUtils/AlSymMat.h"
#include "TrkAlgebraUtils/AlVec.h"
#include "TrkParameters/TrackParameters.h"

//<<<<<< CLASS DECLARATIONS                                             >>>>>>

class MsgStream;

namespace Trk
{
    class ScatteringAngles;
    class Surface;
    class TrackSurfaceIntersection;
    
class FitParameters
{
public:
    FitParameters (const Perigee&	perigee);

    FitParameters (double			d0,
		   double			z0,
		   double			cosPhi,
		   double			sinPhi,
		   double			cotTheta,
		   double			ptInv0,
		   const PerigeeSurface&	surface);

    FitParameters (const FitParameters&	parameters);	// copy constructor
    // forbidden assignment operator
    
    ~FitParameters (void);	       			// destructor

    void			addScatterer (double phi, double theta);
    const Surface*		associatedSurface (void) const;	
    double			cosPhi (void) const;
    double			cosTheta (void) const;
    double			cotTheta (void) const;
    void			covariance (Amg::MatrixX*	finalCovariance,
					    const Amg::MatrixX*	fullCovariance);
    double			d0 (void) const;
    void			d0 (double value);
    double			difference (int param) const;
    const AlVec&		differences (void) const;
    Amg::Vector3D		direction (void) const;
    bool			extremeMomentum (void) const;
    void			extremeMomentum (bool value);
    const Amg::MatrixX*		finalCovariance (void) const;
    int				firstScatteringParameter (void) const;
    void			firstScatteringParameter (int value);
    bool			fitEnergyDeposit (void) const;
    void			fitEnergyDeposit (double minEnergyDeposit);
    bool			fitMomentum (void) const;
    void			fitMomentum (bool value);
    const Amg::MatrixX*		fullCovariance (void) const;
    void			numberParameters (int numberParameters);
    void			numberScatterers (int numberScatterers);
    TrackSurfaceIntersection*	intersection (void) const;
    int				numberOscillations (void) const;
    int				numberParameters (void) const;
    int				numberScatterers (void) const;
    void			performCutStep (double cutStep);
    bool			phiInstability (void) const;
    double			ptInv0 (void) const;
    const Amg::MatrixX		parameterDifference (const Amg::VectorX& parameters) const;
    Perigee*			perigee (void) const;
    const Amg::Vector3D&	position (void) const;
    void			print (MsgStream& log) const;
    void			printCovariance (MsgStream& log) const;
    void			printVerbose (MsgStream& log) const;
    double			qOverP (void) const;
    void			qOverP (double value);
    double			qOverP1 (void) const;
    void			qOverP1 (double value);
    void			reset (const FitParameters& parameters);
    void			resetOscillations (void);
    double		       	scattererPhi (int scatterer) const;
    double		       	scattererTheta (int scatterer) const;
    const ScatteringAngles*	scatteringAngles (const FitMeasurement& fitMeasurement,
						  int scatterer = -1) const;
    void			setPhiInstability (void);
    double			sinPhi (void) const;
    double			sinTheta (void) const;
    Perigee*			startingPerigee (void) const;
    const TrackParameters*	trackParameters (MsgStream&		log,
						 const FitMeasurement&	measurement,
						 bool			withCovariance=false) const;
    void			update (const AlVec&			differences);
    void			update (Amg::Vector3D			position,
					Amg::Vector3D			direction,
					double				qOverP,
					const Amg::MatrixX&	leadingCovariance);
    const Amg::Vector3D&	vertex (void) const;
    double			z0 (void) const;

private:
    // assignment: no semantics, no implementation
    FitParameters &operator= (const FitParameters&);
    
    double			m_cosPhi;
    mutable double		m_cosPhi1;
    double			m_cosTheta;
    mutable double		m_cosTheta1;
    double			m_cotTheta;
    double			m_d0;
    AlVec*			m_differences;
    bool			m_extremeMomentum;
    Amg::MatrixX*		m_finalCovariance;
    int				m_firstScatteringParameter;
    bool			m_fitEnergyDeposit;
    bool			m_fitMomentum;
    const Amg::MatrixX*		m_fullCovariance;
    double			m_minEnergyDeposit;
    int				m_numberOscillations;
    int				m_numberParameters;
    int				m_numberScatterers;
    double			m_oldDifference;
    const Perigee*		m_perigee;
    bool			m_phiInstability;
    Amg::Vector3D	       	m_position;
    double	       		m_qOverP;
    mutable double		m_qOverP1;
    std::vector<double>		m_scattererPhi;
    std::vector<double>		m_scattererTheta;
    double			m_sinPhi;
    mutable double		m_sinPhi1;
    double			m_sinTheta;
    mutable double		m_sinTheta1;
    const Surface*		m_surface;
    mutable Amg::Vector3D	m_vertex;
    double			m_z0;

};

//<<<<<< INLINE PUBLIC MEMBER FUNCTIONS                                 >>>>>>

inline double
FitParameters::cosPhi (void) const
{ return m_cosPhi; }

inline double
FitParameters::cosTheta (void) const
{ return m_cosTheta; }

inline double
FitParameters::cotTheta (void) const
{ return m_cotTheta; }

inline double
FitParameters::d0 (void) const
{ return m_d0; }

inline double
FitParameters::difference (int param) const
{
    if (! m_differences)  return 0.;
    return (*m_differences)[param];
}

inline const AlVec&
FitParameters::differences (void) const
{ return *m_differences; }

inline Amg::Vector3D
FitParameters::direction (void) const
{ return Amg::Vector3D(m_cosPhi*m_sinTheta,m_sinPhi*m_sinTheta,m_cosTheta); }

inline bool
FitParameters::extremeMomentum (void) const
{ return m_extremeMomentum; }

inline const Amg::MatrixX*
FitParameters::finalCovariance (void) const
{ return m_finalCovariance; }

inline int
FitParameters::firstScatteringParameter (void) const
{ return m_firstScatteringParameter; }

inline bool
FitParameters::fitEnergyDeposit (void) const
{ return m_fitEnergyDeposit; }

inline bool
FitParameters::fitMomentum (void) const
{ return m_fitMomentum; }

inline const Amg::MatrixX*
FitParameters::fullCovariance (void) const
{ return m_fullCovariance; }

inline int
FitParameters::numberOscillations (void) const
{ return m_numberOscillations; }

inline int
FitParameters::numberParameters (void) const
{ return m_numberParameters; }

inline int
FitParameters::numberScatterers (void) const
{ return m_numberScatterers; }

inline double
FitParameters::ptInv0 (void) const
{ return m_qOverP/m_sinTheta; }

inline const Amg::Vector3D&
FitParameters::position (void) const
{ return m_position; }

inline double
FitParameters::qOverP (void) const
{ return m_qOverP; }

inline double
FitParameters::qOverP1 (void) const
{ return m_qOverP1; }

inline double
FitParameters::scattererPhi (int scatterer) const
{ return m_scattererPhi[scatterer]; }

inline double
FitParameters::scattererTheta (int scatterer) const
{ return m_scattererTheta[scatterer]; }

inline double
FitParameters::sinPhi (void) const
{ return m_sinPhi; }

inline double
FitParameters::sinTheta (void) const
{ return m_sinTheta; }

inline void
FitParameters::resetOscillations (void) 
{ m_numberOscillations = 0; }

inline const Amg::Vector3D&
FitParameters::vertex (void) const
{ return m_vertex; }

inline double
FitParameters::z0 (void) const
{ return m_position.z(); }

}	// end of namespace

#endif	// TRKIPATFITTERUTILS_FITPARAMETERS_H
