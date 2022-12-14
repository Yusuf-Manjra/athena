/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: BTaggingTrigAuxContainer_v1.cxx 688384 2015-08-10 11:21:34Z kleney $

// Local include(s):
#include "xAODBTagging/versions/BTaggingTrigAuxContainer_v1.h"

namespace xAOD {

   BTaggingTrigAuxContainer_v1::BTaggingTrigAuxContainer_v1()
      : AuxContainerBase() {

     AUX_VARIABLE( SV0_significance3D );
     // AUX_VARIABLE( SV0_TrackParticles );

     AUX_VARIABLE( SV1_pb );
     AUX_VARIABLE( SV1_pu );
     AUX_VARIABLE( SV1_pc );
     // AUX_VARIABLE( SV1_TrackParticles );

     AUX_VARIABLE( IP2D_pb );
     AUX_VARIABLE( IP2D_pu );
     AUX_VARIABLE( IP2D_pc );
     // AUX_VARIABLE( IP2D_TrackParticles );
     
     AUX_VARIABLE( IP3D_pb );
     AUX_VARIABLE( IP3D_pu );
     AUX_VARIABLE( IP3D_pc );
     // AUX_VARIABLE( IP3D_TrackParticles );

     AUX_VARIABLE( JetFitter_pb );
     AUX_VARIABLE( JetFitter_pu );
     AUX_VARIABLE( JetFitter_pc );

     AUX_VARIABLE( JetFitterCombNN_pb );
     AUX_VARIABLE( JetFitterCombNN_pu );
     AUX_VARIABLE( JetFitterCombNN_pc );

     AUX_VARIABLE( MV1_discriminant );

     AUX_VARIABLE( MV2c00_discriminant );
     AUX_VARIABLE( MV2c10_discriminant );
     AUX_VARIABLE( MV2c20_discriminant );

     AUX_VARIABLE( IP2D_valD0wrtPVofTracks );
     AUX_VARIABLE( IP2D_sigD0wrtPVofTracks );
     AUX_VARIABLE( IP2D_weightBofTracks );
     AUX_VARIABLE( IP2D_weightUofTracks );
     AUX_VARIABLE( IP2D_weightCofTracks );
     AUX_VARIABLE( IP2D_flagFromV0ofTracks );
     AUX_VARIABLE( IP2D_gradeOfTracks );

     AUX_VARIABLE( IP3D_valD0wrtPVofTracks );
     AUX_VARIABLE( IP3D_sigD0wrtPVofTracks );
     AUX_VARIABLE( IP3D_valZ0wrtPVofTracks );
     AUX_VARIABLE( IP3D_sigZ0wrtPVofTracks );
     AUX_VARIABLE( IP3D_weightBofTracks );
     AUX_VARIABLE( IP3D_weightUofTracks );
     AUX_VARIABLE( IP3D_weightCofTracks );
     AUX_VARIABLE( IP3D_flagFromV0ofTracks );
     AUX_VARIABLE( IP3D_gradeOfTracks );

     AUX_VARIABLE( SV0_normdist );
     AUX_VARIABLE( SV0_pb );
     AUX_VARIABLE( SV0_pu );
     AUX_VARIABLE( SV0_pc );
     AUX_VARIABLE( SV0_energyTrkInJet );
     AUX_VARIABLE( SV0_masssvx );
     AUX_VARIABLE( SV0_efracsvx );
     AUX_VARIABLE( SV0_N2Tpair );
     AUX_VARIABLE( SV0_NGTinSvx );
     AUX_VARIABLE( SV0_vertices );
     AUX_VARIABLE( SV0_badTracksIP );

     AUX_VARIABLE( SV1_normdist );
     AUX_VARIABLE( SV1_masssvx );
     AUX_VARIABLE( SV1_efracsvx );
     AUX_VARIABLE( SV1_N2Tpair );
     AUX_VARIABLE( SV1_significance3d );
     AUX_VARIABLE( SV1_energyTrkInJet );
     AUX_VARIABLE( SV1_NGTinSvx );
     AUX_VARIABLE( SV1_vertices );
     AUX_VARIABLE( SV1_badTracksIP );

     AUX_VARIABLE( JetFitter_N2Tpair );
     AUX_VARIABLE( JetFitter_massUncorr );
     AUX_VARIABLE( JetFitter_chi2 );
     AUX_VARIABLE( JetFitter_ndof );
     AUX_VARIABLE( JetFitter_nVTX );
     AUX_VARIABLE( JetFitter_nSingleTracks );
     AUX_VARIABLE( JetFitter_nTracksAtVtx );
     AUX_VARIABLE( JetFitter_mass );
     AUX_VARIABLE( JetFitter_energyFraction );
     AUX_VARIABLE( JetFitter_significance3d );
     AUX_VARIABLE( JetFitter_deltaeta );
     AUX_VARIABLE( JetFitter_deltaphi );
     AUX_VARIABLE( JetFitter_fittedPosition );
     AUX_VARIABLE( JetFitter_fittedCov );
     AUX_VARIABLE( JetFitter_tracksAtPVchi2 );
     AUX_VARIABLE( JetFitter_tracksAtPVndf );
     AUX_VARIABLE( JetFitter_JFvertices );
     AUX_VARIABLE( JetFitter_tracksAtPVlinks );

     AUX_VARIABLE( trkSum_VEta );
     AUX_VARIABLE( trkSum_ntrk );
     AUX_VARIABLE( trkSum_SPt );
     AUX_VARIABLE( trkSum_VPt );

     AUX_VARIABLE( SV0_TrackParticleLinks );
     AUX_VARIABLE( SV1_TrackParticleLinks );
     AUX_VARIABLE( IP2D_TrackParticleLinks );
     AUX_VARIABLE( IP3D_TrackParticleLinks );
     AUX_VARIABLE( BTagTrackToJetAssociator );
     AUX_VARIABLE( BTagBtagToJetAssociator ); 


   }

} // namespace xAOD
