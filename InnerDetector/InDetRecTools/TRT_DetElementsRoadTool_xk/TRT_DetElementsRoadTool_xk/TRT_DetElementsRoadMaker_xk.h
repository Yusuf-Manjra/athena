/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


/////////////////////////////////////////////////////////////////////////////////
//  Header file for class TRT_DetElementsRoadMaker_xk
/////////////////////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
/////////////////////////////////////////////////////////////////////////////////
// Class for  InDetDD::TRT_BaseElement* collection production
/////////////////////////////////////////////////////////////////////////////////
// Version 1.0 3/10/2004 I.Gavrilenko
/////////////////////////////////////////////////////////////////////////////////

#ifndef TRT_DetElementsRoadMaker_xk_H
#define TRT_DetElementsRoadMaker_xk_H



#include "GaudiKernel/ServiceHandle.h"
#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "MagFieldInterfaces/IMagFieldSvc.h"
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ServiceHandle.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "InDetRecToolInterfaces/ITRT_DetElementsRoadMaker.h"

#include "TrkGeometry/MagneticFieldProperties.h"
#include "TrkSurfaces/CylinderBounds.h"

#include "TRT_DetElementsRoadTool_xk/TRT_DetElementsLayer_xk.h"
#include <list>
#include <vector>
#include <iosfwd>

class MsgStream;

namespace Trk {

  class IPropagator       ;
}

namespace InDet {

  /**
  @class TRT_DetElementsRoadMaker_xk 
  InDet::TRT_DetElementsRoadMaker_xk is algorithm which produce list of 
  InDetDD::TRT_BaseElement* sorted in propagation order.
  @author Igor.Gavrilenko@cern.ch     
  */


  class TRT_DetElementsRoadMaker_xk : 

    virtual public ITRT_DetElementsRoadMaker, public AthAlgTool
    {
      ///////////////////////////////////////////////////////////////////
      // Public methods:
      ///////////////////////////////////////////////////////////////////
      
    public:
      
      ///////////////////////////////////////////////////////////////////
      // Standard tool methods
      ///////////////////////////////////////////////////////////////////

      TRT_DetElementsRoadMaker_xk
	(const std::string&,const std::string&,const IInterface*);
      virtual ~TRT_DetElementsRoadMaker_xk();
      virtual StatusCode initialize();
      virtual StatusCode finalize  ();

      ///////////////////////////////////////////////////////////////////
      // Main methods for road builder
      ///////////////////////////////////////////////////////////////////
      
      virtual void detElementsRoad
	(const Trk::TrackParameters&,Trk::PropDirection, 
	 std::vector<const InDetDD::TRT_BaseElement*>&) const;

      virtual void detElementsRoad
	(const Trk::TrackParameters&,Trk::PropDirection, 
	 std::vector<std::pair<const InDetDD::TRT_BaseElement*,const Trk::TrackParameters*> >&) const;

      ///////////////////////////////////////////////////////////////////
      // Print internal tool parameters and status
      ///////////////////////////////////////////////////////////////////

      MsgStream&    dump(MsgStream&    out) const;
      std::ostream& dump(std::ostream& out) const;

    protected:
      
      ///////////////////////////////////////////////////////////////////
      // Protected Data
      ///////////////////////////////////////////////////////////////////

      ServiceHandle<MagField::IMagFieldSvc> m_fieldServiceHandle;
      MagField::IMagFieldSvc*               m_fieldService{}      ;
      ServiceHandle<IGeoModelSvc>           m_geoModelSvc{this, "GeoModelSvc", "GeoModelSvc"};
      ToolHandle<Trk::IPropagator>          m_proptool ;  // Propagator     tool

      float                                m_width{}    ;  // Width of the roadInnerDetector/InDetRecTools/
      double                               m_step{}     ;  // Max step allowed
      double                               m_rminTRT{}  ;
      Trk::CylinderBounds                  m_bounds   ;  //  
      int                                  m_map  [3] ;
      std::vector<TRT_DetElementsLayer_xk> m_layer[3] ;  // Layers

      std::string                          m_trt      ;  // PIX manager   location
      std::string                          m_fieldmode;  // Mode of magnetic field
      Trk::MagneticFieldMode               m_fieldModeEnum{Trk::FullField};

      ///////////////////////////////////////////////////////////////////
      // Methods
      ///////////////////////////////////////////////////////////////////
      
      StatusCode mapDetectorElementsProduction(IOVSVC_CALLBACK_ARGS);
      void detElementInformation(const InDetDD::TRT_BaseElement&,double*) const;
      void detElementsRoadATL(std::list<Amg::Vector3D>&, 
			      std::vector<const InDetDD::TRT_BaseElement*>&) const;
      void detElementsRoadCTB(std::list<Amg::Vector3D>&, 
			      std::vector<const InDetDD::TRT_BaseElement*>&) const;
      double stepToDetElement
	(const InDetDD::TRT_BaseElement*&,Amg::Vector3D&,Amg::Vector3D&) const;

      Trk::CylinderBounds getBound(const Trk::TrackParameters&) const;

      MsgStream&    dumpConditions(MsgStream   & out) const;

      MsgStream&    dumpEvent     (MsgStream   & out, int size_road) const;

  };

} // end of name space

#endif // TRT_DetElementsRoadMaker_xk_H

