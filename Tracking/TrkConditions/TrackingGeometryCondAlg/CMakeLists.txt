# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrackingGeometryCondAlg )

# Component(s) in the package:
atlas_add_component( TrackingGeometryCondAlg
   TrackingGeometryCondAlg/*.h src/*.cxx src/components/*.cxx
   LINK_LIBRARIES
   AthenaBaseComps
   AthenaKernel
   GaudiKernel
   PersistentDataModel
   StoreGateLib
   TrkDetDescrInterfaces
   TrkGeometry
   InDetReadoutGeometry
   TrkDetDescrSvcLib
    )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
