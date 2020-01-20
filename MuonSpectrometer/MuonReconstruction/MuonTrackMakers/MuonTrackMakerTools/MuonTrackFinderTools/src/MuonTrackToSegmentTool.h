/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


#ifndef MUON_MUONTRACKTOSEGMENTTOOL_H
#define MUON_MUONTRACKTOSEGMENTTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "MuonRecHelperTools/IMuonEDMHelperSvc.h"
#include "MuonRecToolInterfaces/IMuonTrackToSegmentTool.h"
#include "TrkParameters/TrackParameters.h"
#include "MuonCondData/MdtCondDbData.h"
#include "Identifier/Identifier.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"


#include <vector>

class MdtCondDbData;
class MuonStationIntersectSvc;
class MsgStream;

namespace Trk {
  class IPropagator;
  class Track;
  class MeasurementBase;
}

namespace Muon {
  class MuonSegment;
  class MuonIdHelperTool;
  class MuonEDMPrinterTool;
}

namespace Muon {

  /** 
      @class MuonTrackToSegmentTool

      Tool to combine two segments. It appoximates the combined segment by a straight line. 
      The tool assumes that this is a valid assumption.
      
      For more details look at the mainpage of this package.
  */
  class MuonTrackToSegmentTool :  virtual public IMuonTrackToSegmentTool, public AthAlgTool   {
  public:
    typedef std::vector<const Trk::MeasurementBase*> MeasVec;
    typedef MeasVec::iterator                        MeasIt;
    typedef MeasVec::const_iterator                  MeasCit;

  public:
    /** default AlgTool constructor */
    MuonTrackToSegmentTool(const std::string&, const std::string&, const IInterface*);
    
    /** destructor */
    ~MuonTrackToSegmentTool();
    
    /** initialize method, method taken from bass-class AlgTool */
    StatusCode initialize();

    /** finialize method, method taken from bass-class AlgTool */
    StatusCode finalize();
    
    /** @brief convert track to segment */
    MuonSegment* convert( const Trk::Track& track ) const;

  private:
    /** @brief calculate holes */
    std::vector<Identifier> calculateHoles( const Identifier& chid, const Trk::TrackParameters& pars, const MeasVec& measurements ) const;

    SG::ReadCondHandleKey<MuonGM::MuonDetectorManager> m_DetectorManagerKey {this, "DetectorManagerKey", 
	"MuonDetectorManager", 
	"Key of input MuonDetectorManager condition data"};    

    ServiceHandle<MuonStationIntersectSvc> m_intersectSvc;  //<! pointer to hole search service
    ToolHandle<Trk::IPropagator>        m_propagator;       //<! propagator
    ToolHandle<MuonIdHelperTool>        m_idHelperTool;     //<! tool to assist with Identifiers
    ServiceHandle<IMuonEDMHelperSvc>    m_edmHelperSvc {this, "edmHelper", 
      "Muon::MuonEDMHelperSvc/MuonEDMHelperSvc", 
      "Handle to the service providing the IMuonEDMHelperSvc interface" };       //<! multipurpose helper tool
    ToolHandle<MuonEDMPrinterTool>      m_printer;          //<! tool to print out EDM objects

    SG::ReadCondHandleKey<MdtCondDbData> m_condKey{this, "MdtCondKey", "MdtCondDbData", "Key of MdtCondDbData"};

  };


}


#endif
