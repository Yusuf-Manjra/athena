################################################################################
# Package: MuonRPC_Cabling
################################################################################

# Declare the package name:
atlas_subdir( MuonRPC_Cabling )

# Component(s) in the package:
atlas_add_library( MuonRPC_CablingLib
		   MuonRPC_Cabling/*.h src/*.cxx
		   PUBLIC_HEADERS MuonRPC_Cabling
		   LINK_LIBRARIES AthenaBaseComps AthenaKernel Identifier GaudiKernel MuonCablingTools MuonIdHelpersLib
		   PRIVATE_LINK_LIBRARIES StoreGateLib AthenaPoolUtilities MuonCondInterface PathResolver RPC_CondCablingLib)
atlas_add_component( MuonRPC_Cabling
		     src/components/*.cxx
		     LINK_LIBRARIES MuonRPC_CablingLib )

# Install files from the package:
#atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_runtime( share/LVL1confAtlas.corr share/ATLAS.data share/LVL1confAtlasRUN2_ver016.corr )

