/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCALIB_MUONSEGMENTTOCALIBSEGMENT_H
#define MUONCALIB_MUONSEGMENTTOCALIBSEGMENT_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

/////////////////////////////////////////////////////////////////////////////
#include "MuonCalibEvent/MuonCalibPatternCollection.h"

#include "MuonSegment/MuonSegmentCombinationCollection.h"
#include "TrkSegment/SegmentCollection.h"
#include "MuonSegment/MuonSegment.h"
#include "MuonCalibITools/IIdToFixedIdTool.h"
#include "MuonRecToolInterfaces/IMuonPatternSegmentAssociationTool.h"
#include "MdtCalibSvc/MdtCalibrationTool.h"
#include "MuonIdHelpers/MuonIdHelperTool.h"

namespace Muon{
  class MuonPatternCombination;
}

namespace MuonGM {
  class MuonDetectorManager;
}

namespace MuonCalib {

/** @class MuonSegmentToCalibSegment
Algorithm to retrieve MuonSegments or MuonSegmentCombinations from StoreGate and 
output the muon calibration input.
    @author Niels.Van.Eldik@cern.ch
*/

  class MuonSegmentToCalibSegment : public AthAlgorithm {
  public:
    /** Algorithm constructor */
    MuonSegmentToCalibSegment(const std::string& name, ISvcLocator* pSvcLocator);

    /** Algorithm initialize */
    StatusCode initialize();
     
    /** Algorithm execute, called once per event */
    StatusCode execute();

    /** Algorithm finalize */
    StatusCode finalize();

  private:
    /** retrieve patterns and segments from storegate */
    //    const Muon::MuonSegmentCollection* retrieveSegments();
  
    /** retrieve patterns from storegate */
    const MuonSegmentCombinationCollection* retrieveSegmentCombinations() const;
    const Trk::SegmentCollection* retrieveSegments(const std::string colName) const;
    const MuonSegmentCombinationCollection* retrieveCscSegmentCombinations() const;
    const Trk::SegmentCollection* retrieveCscSegments() const;

    StatusCode convertPatterns();

    /** save global patterns to storegate */
    StatusCode savePatterns( const MuonCalibPatternCollection* newPatterns ) const;
    
    
    MuonCalibSegment* createMuonCalibSegment( const Muon::MuonSegment& seg ) const;
    MuonCalibPattern* createMuonCalibPattern( const Muon::MuonPatternCombination* pat ) const;
    Identifier        getChId( const Muon::MuonSegment& seg ) const;
    Amg::Transform3D    getGlobalToStation( const Identifier& id ) const;

    unsigned int getQuality( const Muon::MuonSegment& seg ) const;

    /**  segment location */
    bool        m_readSegments;
    bool        m_useCscSegments;
    std::string m_segmentLocation;
    std::string m_segmentCscLocation;
    std::string m_segmentCombiLocation;
    StringArrayProperty m_segmentLocationVector;
    std::vector<int> m_segment_authors;
    bool        m_newImpactParameter;

    /** segment author */
    int segmentAuthor(const std::string& collectionName);
    
    /** pattern location */
    std::string m_patternLocation;

    std::string         m_associationInputLocation; //!< Location of the association object for PatternCombis and SegmentCombis
    std::string         m_cscAssociationInputLocation; //!< Location of the association object for PatternCombis and SegmentCombis

    /** Pointer to MuonDetectorManager */
    const MuonGM::MuonDetectorManager*  m_detMgr;
   
    /** Tool for the Identifier Helpers */
    ToolHandle<Muon::MuonIdHelperTool> m_muonIdHelperTool{this, "idHelper", 
      "Muon::MuonIdHelperTool/MuonIdHelperTool", "Handle to the MuonIdHelperTool"};

    /** pointer to MdtCalibSvc */
    ToolHandle<MdtCalibrationTool> m_calibrationTool;

    /** IdentifierTool initialization */
    ToolHandle< Muon::IMuonPatternSegmentAssociationTool> m_assocTool;
    ToolHandle< IIdToFixedIdTool> m_idToFixedIdTool;


    /** -1: Take infirmation from error-strategy
         0: fitted t0 is not applied to drift times
	 1: fitted t0 is applied to drift time
	 */
    int m_updateForT0Shift;
    bool m_doTof;
    bool m_cosmics_tof;

    /** maximum number of segments each algorithm can store in ntuple */
    int m_maxStoredSegs;
  };

}

#endif

