/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCOMBINEDEVENT_MUONSEGMENTINFO_H
#define MUONCOMBINEDEVENT_MUONSEGMENTINFO_H

#include "TrkParameters/TrackParameters.h"
#include "xAODMuon/MuonSegmentContainer.h"
#include "xAODMuon/MuonSegment.h"
#include "AthLinks/ElementLink.h"

namespace Muon{
  class MuonSegment;
}

namespace Trk{
  class Track;
}



namespace MuonCombined {

    struct MuonSegmentInfo {

    MuonSegmentInfo() : track(0), segment(0), trackAtSegment(0) {}
      /**pointer to the track object*/
      const Trk::Track* track;

      /** pointer to the muon object*/
      const Muon::MuonSegment* segment;

      /** track extrapolated to the segment */
      const Trk::TrackParameters* trackAtSegment;

      /** element link to xAOD segment */
      ElementLink<xAOD::MuonSegmentContainer> link;

      /** global theta direction track - segment in bending plane */
      double dtheta;

      /** global phi direction track - segment in bending plane */
      double dphi;

      /** global theta position track - segment in bending plane */
      double dthetaPos;

      /** global phi position track - segment in bending plane */
      double dphiPos;

      /** residual track - segment in Local coordinate non-bending plane */
      double resX;

      /** error from extrapolation on residual in non-bending plane */
      double exErrorX;

      /** error from segment on residual in non-bending plane */
      double segErrorX;

      /** pull on residual in non-bending plane */
      double pullX;

      /** residual track - segment in Local coordinate in bending plane */
      double resY;

      /** error from extrapolation on residual in bending plane */
      double exErrorY;
  
      /** error from segment on residual in bending plane */
      double segErrorY;

      /** pull on residual in bending plane */
      double pullY;

      /** covariance  loc Y theta from track in bending plane */
      double exCovYTheta;

      /** covariance  loc Y angleYZ from track in bending plane */
      double exCovYZY;

      /** angular residual in the Local coordinate non-bending plane */
      double dangleXZ;

     /** error from extrapolation on angle in non-bending plane */
      double exErrorXZ;

      /** error from segment on angle in non-bending plane */
      double segErrorXZ;

      /** pull on angle in non-bending plane */
      double pullXZ;

      /** angular residual in the Local coordinate bending plane */
      double dangleYZ;

     /** error from extrapolation on angle in bending plane */
      double exErrorYZ;

      /** error from segment on angle in bending plane */
      double segErrorYZ;

      /** pull on angle in bending plane */
      double pullYZ;

      /** residual on combined local position Y and angle YZ  */
      double resCY;

      /** pull on combined local position Y and angle YZ  */
      double pullCY;

      /** chi2 in the Y position and YZ angle bending and precision plane */
      double chi2Y;

     /** Expect phi hits in EI or EM or BM or BO */
      int hasPhi;

      /** hit count summary */
      //Muon::IMuonSegmentHitSummaryTool::HitCounts hitCounts;

      /** segment quality */
      int quality;

      /** station layer */
      int stationLayer;

      /** number of holes */
      int nholes;

      /** number of segments in track */
      int nsegments;

      /** 1 if single ML */
      int singleML;

      /** maximum residual with tube endplugs (positive means outside of the tube */
      double maximumResidualAlongTube;

      /** pullChamber =  maximumResidualAlongTube / extrapolationError  < 5 means within chamber at 5.s.d from edges */

      double pullChamber;

      /** maximum residual with the phi hits on the segment */
      double maximumResidualPhi;

      /** maximum pull with the phi hits on the segment */
      double maximumPullPhi;

      /** minimum residual with the phi hits on the segment */
      double minimumResidualPhi;

      /** minimum pull with the phi hits on the segment */
      double minimumPullPhi;

      /** fitted t0 */
      double t0;

     /** RLocY parametrized rejection as a function of locY
 *          parameter is used for selecting best segment */

      double RLocY;

    /** RAYZ parametrized rejection as a function of Angle YZ
 *          parameter is/could be used for selecting best segment */

      double RAYZ;

    /** selected == 1 if the segment accepted for MuonTagIMO */

      int selected;

  };     
                 
}


#endif

