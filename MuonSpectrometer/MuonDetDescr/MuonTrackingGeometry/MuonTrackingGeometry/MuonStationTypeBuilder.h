/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// MuonStationTypeBuilder.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef MUONTRACKINGGEOMETRY_MUONSTATIONTYPEBUILDER_H
#define MUONTRACKINGGEOMETRY_MUONSTATIONTYPEBUILDER_H

//Trk
#include "TrkGeometry/TrackingVolume.h"
#include "TrkGeometry/Material.h"
#include "TrkDetDescrUtils/SharedObject.h"
#include "TrkDetDescrGeoModelCnv/GeoMaterialConverter.h"
#include "TrkDetDescrInterfaces/ITrackingVolumeArrayCreator.h"
#include "TrkGeometry/MaterialProperties.h"
// Gaudi
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "GeoModelKernel/GeoVPhysVol.h"
#include "GeoModelKernel/GeoMaterial.h"

namespace Trk {
 class Volume;
 class Layer;
 class CuboidVolumeBounds;
 class TrapezoidVolumeBounds;
 class DoubleTrapezoidVolumeBounds;
 class PlaneLayer;   
}

namespace MuonGM {
  class MuonDetectorManager;
  class MuonStation;
}
 
namespace Muon {

  typedef std::pair<Trk::SharedObject<const Trk::Layer>,const Amg::Transform3D*> LayTr;
     
  /** @class MuonStationTypeBuilder
  
      The Muon::MuonStationTypeBuilder retrieves components of muon stations from Muon Geometry Tree,
      builds 'prototype' object (TrackingVolume with NameType)
      
      by Sarka.Todorova@cern.ch
    */
    
  class MuonStationTypeBuilder : public AthAlgTool{
  public:
      /** Constructor */
      MuonStationTypeBuilder(const std::string&,const std::string&,const IInterface*);
      /** Destructor */
      virtual ~MuonStationTypeBuilder() = default;
      /** AlgTool initailize method.*/
      StatusCode initialize();
      /** AlgTool finalize method */
      StatusCode finalize();
      /** Interface methode */
      static const InterfaceID& interfaceID();
      /** steering routine */
      const Trk::TrackingVolumeArray* processBoxStationComponents(const GeoVPhysVol* cv, Trk::CuboidVolumeBounds* envBounds); 
      const Trk::TrackingVolumeArray* processTrdStationComponents(const GeoVPhysVol* cv, Trk::TrapezoidVolumeBounds* envBounds); 
      Trk::TrackingVolume* processCscStation(const GeoVPhysVol* cv, std::string name); 
      std::vector<const Trk::TrackingVolume*> processTgcStation(const GeoVPhysVol* cv); 
      /** components */
      const Trk::TrackingVolume* processMdtBox(Trk::Volume*&,const GeoVPhysVol*&, Amg::Transform3D*, double );
      const Trk::TrackingVolume* processMdtTrd(Trk::Volume*&,const GeoVPhysVol*&, Amg::Transform3D*); 
      const Trk::TrackingVolume* processRpc(Trk::Volume*&,std::vector<const GeoVPhysVol*>,std::vector<Amg::Transform3D>);
      const Trk::TrackingVolume* processSpacer(Trk::Volume&,std::vector<const GeoVPhysVol*>,std::vector<Amg::Transform3D>) const;
      const Trk::TrackingVolume* processNSW(std::vector<const Trk::Layer*>) const;
      const Trk::LayerArray* processCSCTrdComponent(const GeoVPhysVol*&, Trk::TrapezoidVolumeBounds*&, Amg::Transform3D*&);
      const Trk::LayerArray* processCSCDiamondComponent(const GeoVPhysVol*&, Trk::DoubleTrapezoidVolumeBounds*&, Amg::Transform3D*&);
      const Trk::LayerArray* processTGCComponent(const GeoVPhysVol*&, Trk::TrapezoidVolumeBounds*&, Amg::Transform3D*&);
      std::pair<const Trk::Layer*,const std::vector<const Trk::Layer*>* > createLayerRepresentation(const Trk::TrackingVolume* trVol) const; 
      const Trk::Layer* createLayer(const Trk::TrackingVolume* trVol,Trk::MaterialProperties*, Amg::Transform3D&) const; 
      Identifier identifyNSW( std::string, Amg::Transform3D ) const;
   
      void printChildren(const GeoVPhysVol*) const ;
      // used to be private ..
      double get_x_size(const GeoVPhysVol*) const ;
      double decodeX(const GeoShape*) const ;
      double getVolume(const GeoShape*) const;
      Trk::MaterialProperties getAveragedLayerMaterial(const GeoVPhysVol*,double,double) const;
      void collectMaterial(const GeoVPhysVol*,Trk::MaterialProperties&,double) const;
      Trk::MaterialProperties collectStationMaterial(const Trk::TrackingVolume* trVol,double) const; 

  private:
     /** Private method to fill default material */
      //void fillDefaultServiceMaterial();

      const MuonGM::MuonDetectorManager*  m_muonMgr;               //!< the MuonDetectorManager
      Gaudi::Property<std::string>        m_muonMgrLocation{this,"MuonDetManagerLocation","MuonMgr"};//!< the location of the Muon Manager
      Gaudi::Property<bool>               m_multilayerRepresentation{this,"BuildMultilayerRepresentation",true};   
      Gaudi::Property<bool>               m_resolveSpacer{this,"ResolveSpacerBeams",false};   

      ToolHandle<Trk::ITrackingVolumeArrayCreator>   m_trackingVolumeArrayCreator{this,"TrackingVolumeArrayCreator","Trk::TrackingVolumeArrayCreator/TrackingVolumeArrayCreator"};  //!< Helper Tool to create TrackingVolume Arrays

      std::unique_ptr<Trk::Material>              m_muonMaterial;               //!< the material
      std::unique_ptr<Trk::GeoMaterialConverter>          m_materialConverter;

      std::unique_ptr<Trk::MaterialProperties>    m_mdtTubeMat;
      std::vector<std::unique_ptr<Trk::MaterialProperties> > m_mdtFoamMat;
      std::unique_ptr<Trk::MaterialProperties>    m_rpc46;                  
      std::vector<std::unique_ptr<Trk::MaterialProperties> > m_rpcDed;                
      std::unique_ptr<Trk::MaterialProperties>    m_rpcLayer;                  
      std::unique_ptr<Trk::MaterialProperties>    m_rpcExtPanel;                  
      std::unique_ptr<Trk::MaterialProperties>    m_rpcMidPanel;                  
      std::unique_ptr<Trk::MaterialProperties>    m_matCSC01;        
      std::unique_ptr<Trk::MaterialProperties>    m_matCSCspacer1;
      std::unique_ptr<Trk::MaterialProperties>    m_matCSC02;       
      std::unique_ptr<Trk::MaterialProperties>    m_matCSCspacer2;
      std::unique_ptr<Trk::MaterialProperties>    m_matTGC01;       
      std::unique_ptr<Trk::MaterialProperties>    m_matTGC06;       

    };

} // end of namespace

#endif //MUONTRACKINGGEOMETRY_MUONSTATIONTYPEBUILDER_H
