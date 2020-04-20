/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKCaloCluster_OnTrack_H
#define TRKCaloCluster_OnTrack_H
 // Trk
#include "TrkMeasurementBase/MeasurementBase.h"
//#include "TrkEventPrimitives/GlobalPosition.h"
#include "EventPrimitives/EventPrimitives.h"  
#include "GeoPrimitives/GeoPrimitives.h" 

#include <iosfwd>
class MsgStream;

namespace Trk {
  class Surface;
  class EnergyLoss;

  /** @class CaloCluster_OnTrack
    Class to handle Cluster On Tracks (ROT) for CaloClusters,
    it inherits from the common MeasurementBase. 

    The Track holds a vector of TrackStateOnSurface 
    that carry the MeasurmentBase class pointers.
  */

  class CaloCluster_OnTrack : public MeasurementBase {
    public:
      /** Default Constructor for POOL */
      CaloCluster_OnTrack();
      /** Copy Constructor */
      CaloCluster_OnTrack(const CaloCluster_OnTrack& cot);
 
      /** Assignment operator */
      CaloCluster_OnTrack& operator=(const CaloCluster_OnTrack& cot);

      /** Constructor with parameters  LocalParameters*,  LocalErrorMatrix*, Surface&
      This class owns the LocalParameters, ErrorMatrix & EnergyLoss.  A copy of the  
			surface will be made if it is not owned by a detector element.    */
      CaloCluster_OnTrack( const LocalParameters& locpars,
                           const Amg::MatrixX& locerr,
                           const Surface& surf,
                           const EnergyLoss* eloss = nullptr);
    
      /** Destructor */
      virtual ~CaloCluster_OnTrack();

      /** Pseudo-constructor, needed to avoid excessive RTTI*/
      virtual CaloCluster_OnTrack* clone() const override final;
            
      /** returns the surface for the local to global transformation 
      - interface from MeasurementBase */
      virtual const Surface& associatedSurface() const override final;
     
      /** Interface method to get the global Position
      - interface from MeasurementBase */
      virtual const Amg::Vector3D& globalPosition() const override final;

      /** Extended method to get the EnergyLoss */
      const Trk::EnergyLoss* energyLoss() const;

      /** Extended method checking the type*/
       virtual bool type(MeasurementBaseType::Type type) const override final{
         return (type==MeasurementBaseType::CaloCluster_OnTrack);
       }

      /**returns the some information about this CaloCluster_OnTrack. */
      virtual MsgStream&    dump( MsgStream& out ) const override final;  

      /**returns the some information about this CaloCluster_OnTrack. */
      virtual std::ostream& dump( std::ostream& out ) const override final;
 

    protected:
      /** Surface associated to the measurement*/
      const Surface* m_surface;
      
      /** global position of the cluster hit*/
      const Amg::Vector3D* m_globalpos;
      
      /** Energy Loss */
      const Trk::EnergyLoss* m_eloss;
      

  };//End of Class

}//Namespace

inline const Trk::Surface& Trk::CaloCluster_OnTrack::associatedSurface() const
{ return *m_surface; }

inline const Trk::EnergyLoss* Trk::CaloCluster_OnTrack::energyLoss() const
{ return m_eloss; }

#endif //TRKCaloCluster_OnTrack_H
