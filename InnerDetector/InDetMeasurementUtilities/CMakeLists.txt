# Copyright (C) 2002-2023 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( InDetMeasurementUtilities )

atlas_add_library( InDetMeasurementUtilitiesLib
		   src/ClusterConversionUtilities.cxx
		   src/SpacePointConversionUtilities.cxx
		   PUBLIC_HEADERS 
		      InDetMeasurementUtilities
   		   LINK_LIBRARIES 
		      InDetPrepRawData
		      xAODInDetMeasurement 
		      InDetIdentifier
		      InDetReadoutGeometry
		      PixelReadoutGeometryLib
		      SCT_ReadoutGeometry
		      TrkSpacePoint
		      SiSpacePoint
		   )

# Component(s) in the package:
atlas_add_component( InDetMeasurementUtilities
                     src/*.cxx 
		     src/components/*.cxx
                     LINK_LIBRARIES 
		        InDetMeasurementUtilitiesLib 
			GaudiKernel 
			AthenaBaseComps 
			StoreGateLib
			BeamSpotConditionsData
		     )

atlas_install_scripts( share/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )

