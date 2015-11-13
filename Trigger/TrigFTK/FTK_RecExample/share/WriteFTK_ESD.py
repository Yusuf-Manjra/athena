# FTK
FtkESDList = [
    "TrackCollection#FTK_TrackCollection",
    "TrackCollection#FTK_TrackCollectionRefit",
    "VxContainer#FTK_VxContainer",
    "VxContainer#FTK_VxContainerRefit",
    "xAOD::TrackParticleContainer#FTK_TrackParticleContainer",
    "xAOD::TrackParticleAuxContainer#FTK_TrackParticleContainerAux.",
    "xAOD::TrackParticleContainer#FTK_TrackParticleContainerRefit",
    "xAOD::TrackParticleAuxContainer#FTK_TrackParticleContainerRefitAux.",
    "xAOD::VertexContainer#FTK_VertexContainer",
    "xAOD::VertexAuxContainer#FTK_VertexContainerAux.-vxTrackAtVertex",
    "xAOD::VertexContainer#FTK_VertexContainerRefit",
    "xAOD::VertexAuxContainer#FTK_VertexContainerRefitAux.-vxTrackAtVertex",
    "InDet::PixelClusterContainer#FTK_PixelClusterContainer",
    "InDet::SCT_ClusterContainer#FTK_SCT_ClusterContainer",
    ]

from RecExConfig.RecFlags import rec
if rec.doTruth():
    FtkESDList += [
        "PRD_MultiTruthCollection#FTK_PRD_MultiTruthPixel",
        "PRD_MultiTruthCollection#FTK_PRD_MultiTruthSCT",
        ]
