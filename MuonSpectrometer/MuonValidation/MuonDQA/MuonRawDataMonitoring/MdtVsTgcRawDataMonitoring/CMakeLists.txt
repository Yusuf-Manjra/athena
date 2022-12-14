################################################################################
# Package: MdtVsTgcRawDataMonitoring
################################################################################

# Declare the package name:
atlas_subdir( MdtVsTgcRawDataMonitoring )

# External dependencies:
find_package( ROOT COMPONENTS Graf Gpad Core Tree MathCore Hist RIO pthread Graf3d Html Postscript Gui GX11TTF GX11 )

# Component(s) in the package:
atlas_add_component( MdtVsTgcRawDataMonitoring
                     src/*.cxx
                     src/components/*.cxx
                     src/functions/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaMonitoringLib StoreGateLib xAODMuon GaudiKernel MuonReadoutGeometry MuonPrepRawData MuonSegment MuonTrigCoinData MuonDQAUtilsLib muonEvent TrkSegment GeoPrimitives Identifier EventPrimitives MuonCalibIdentifier MuonRDO MuonCompetingRIOsOnTrack MuonRIO_OnTrack TrkSurfaces TrkEventPrimitives TrkRIO_OnTrack )

# Install files from the package:
atlas_install_joboptions( share/*.py )

