################################################################################
# Package: MuonSimEvent
################################################################################

# Declare the package name:
atlas_subdir( MuonSimEvent )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( MuonSimEvent
                   src/*.cxx
                   PUBLIC_HEADERS MuonSimEvent
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES AthenaKernel GeoPrimitives Identifier GeneratorObjects HitManagement MuonIdHelpersLib
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} )

atlas_add_dictionary( MuonSimEventDict
                      MuonSimEvent/MuonSimEventDict.h
                      MuonSimEvent/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaKernel GeoPrimitives Identifier GeneratorObjects MuonIdHelpersLib HitManagement MuonSimEvent )

