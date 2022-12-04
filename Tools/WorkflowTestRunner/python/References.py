# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

#####
# CI Reference Files Map
#####

# The top-level directory for the files is /eos/atlas/atlascerngroupdisk/data-art/grid-input/WorkflowReferences/
# Then the subfolders follow the format branch/test/version, i.e. for s3760 in master the reference files are under
# /eos/atlas/atlascerngroupdisk/data-art/grid-input/WorkflowReferences/master/s3760/v1 for v1 version

# Format is "test" : "version"
references_map = {
    # Simulation
    "s3759": "v11",
    "s3760": "v10",
    "s3761": "v1",
    "s3779": "v4",
    "s4005": "v1",
    "s4006": "v1",
    "s4007": "v1",
    "s4008": "v1",
    # Overlay
    "d1590": "v11",
    "d1726": "v7",
    "d1759": "v13",
    # Reco
    "q442": "v1",
    "q443": "v1",
    "q445": "v1",
    "q449": "v2",
}
