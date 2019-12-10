/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


///////////////////////////////////////////////////////////////////
// MuonCurvedSegmentCombiner.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef MUONMUONCURVEDSEGMENTCOMBINER_H
#define MUONMUONCURVEDSEGMENTCOMBINER_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/MsgStream.h"
#include "MuonSegmentCombinerToolInterfaces/IMuonCurvedSegmentCombiner.h"
#include "MuonSegmentMakerToolInterfaces/IMuonSegmentPairFittingTool.h"
#include "MuonEDM_AssociationObjects/MuonSegPatAssMap.h"

#include <string>
#include <map>

#include "GaudiKernel/ToolHandle.h"

#include "MuonCurvedSegmentCombiner/MCSCSegmentInfo.h"

#include "MuonIdHelpers/MuonIdHelperTool.h"

class Identifier;

namespace MuonGM {
    class MuonDetectorManager;
}

namespace Muon
{

  class MuonEDMPrinterTool;

  /** @class MuonCurvedSegmentCombiner 

  This is for the Doxygen-Documentation.  
  Please delete these lines and fill in information about
  the Algorithm!
  Please precede every member function declaration with a
  short Doxygen comment stating the purpose of this function.
      
  @author  Edward Moyse <Edward.Moyse@cern.ch>
  */  

  class MuonCurvedSegmentCombiner : virtual public IMuonCurvedSegmentCombiner, virtual public IMuonSegmentPairFittingTool, public AthAlgTool
  {
  public:
    MuonCurvedSegmentCombiner(const std::string&,const std::string&,const IInterface*);

    /** default destructor */
    virtual ~MuonCurvedSegmentCombiner ();
      
    /** standard Athena-Algorithm method */
    virtual StatusCode initialize();
    /** standard Athena-Algorithm method */
    virtual StatusCode finalize  ();
      
    /** INSERT main method here.*/
    std::unique_ptr<MuonSegmentCombinationCollection> combineSegments(    const MuonSegmentCombinationCollection& mdtCombiColl, 
									  const MuonSegmentCombinationCollection& csc4DCombiColl, 
									  const MuonSegmentCombinationCollection& csc2DCombiColl,
									  MuonSegmentCombPatternCombAssociationMap* segPattMap);

    void trackParameters( Muon::MuonSegment& seg ,double &theta,double &curvature,int &imeth );
    //    void fulltrackParameters( const Muon::MuonSegment& seg ,HepMatrix & T , HepMatrix & CovT ,int &imeth );
    void extrapolateSegment( Muon::MuonSegment& seg , Muon::MuonSegment& sege, Amg::VectorX & Res, Amg::VectorX & Pull );

    void fit2Segments( Muon::MuonSegment& seg , Muon::MuonSegment& sege, Amg::VectorX & Res, Amg::VectorX & Pull );
    void fit2SegmentsC( Muon::MuonSegment& seg , Muon::MuonSegment& sege, Amg::VectorX & Res, Amg::VectorX & Pull );
    void fit2SegmentsSL( Muon::MuonSegment& seg , Muon::MuonSegment& sege, Amg::VectorX & Res, Amg::VectorX & Pull );

  private:
          
    std::unique_ptr<MuonSegmentCombinationCollection>  processCombinationCollection(const MuonSegmentCombinationCollection& mdtCombiCol, MuonSegmentCombPatternCombAssociationMap* segPattMap);
    void processCscCombinationCollection( const MuonSegmentCombinationCollection& cscCombiCol, MuonSegmentCombPatternCombAssociationMap* segPattMap) ;
    void process2DCscCombinationCollection( const MuonSegmentCombinationCollection& csc2DcombiCol);

    void muonCurvedSegmentCombinations(MuonSegmentCombinationCollection* curvedCombiCol, MuonSegmentCombPatternCombAssociationMap* segPattMap);

    Muon::MCSCSegmentInfo segInfo( Muon::MuonSegment* seg );

    unsigned int missedHits( Muon::MuonSegment* segment );  



  private:
     
    //       const Muon::MuonSegPatAssMap* m_assCscMap;

    ToolHandle<Muon::MuonIdHelperTool> m_muonIdHelperTool{this, "idHelper", 
      "Muon::MuonIdHelperTool/MuonIdHelperTool", "Handle to the MuonIdHelperTool"};
    ToolHandle< MuonEDMPrinterTool> m_printer;

    bool m_debug;    
    bool m_doCosmics;
    bool m_summary;
    bool m_useCscSegments;
    bool m_addAll2DCscs;
    bool m_mergePatterns;

    int    m_missedHitsCut;
    double m_pullAssociationCut;
    double m_phiAssociationCut1;  
    double m_phiAssociationCut2;  
    bool m_addUnassociatedMiddleEndcapSegments;

    typedef std::map<Muon::MuonSegment*,Muon::MCSCSegmentInfo>  SegInfoMap;
    typedef SegInfoMap::iterator SegInfoIt;
    int    m_segmentIndex;
    SegInfoMap m_segInfoMap;

    SegInfoMap m_seg2DCscInfoMap;
    SegInfoMap m_seg4DCscInfoMap;
    SegInfoMap m_segMdtInfoMap;

    std::map<Muon::MuonSegment*, const MuonPatternCombination* > m_segAssoMap; 
    std::set <Identifier> m_cscIdSet;
       
      
  }; 
} // end of namespace

#endif 
