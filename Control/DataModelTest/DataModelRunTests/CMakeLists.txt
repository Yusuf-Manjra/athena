# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( DataModelRunTests )

# Install files from the package:
atlas_install_joboptions( share/*.py )

# Tests in the package:
function (datamodel_run_test testName)
  cmake_parse_arguments( ARG "" "DEPENDS;COMMAND;ARG;EXTRA_IGNORE" "" ${ARGN} )

  if( ARG_COMMAND )
    set( _command ${ARG_COMMAND} )
  else()
     set( _command athena.py )
  endif()

  if( ARG_ARG )
    set( _arg ${ARG_ARG} )
  else()
    set( _arg DataModelRunTests/${testName}_jo.py )
  endif()

  atlas_add_test( ${testName}
                  SCRIPT ${_command} ${_arg}
                  PROPERTIES TIMEOUT 300
                  LOG_IGNORE_PATTERN "has different type than the branch|no dictionary for class|^StorageSvc +INFO|Cache alignment|Unable to locate catalog|IOVDbSvc[0-9 ]+INFO|Invalid ./TagInfo${ARG_EXTRA_IGNORE}|The database does not exist|IDatabaseSvc::openDatabase.*=>"
                  ENVIRONMENT "ATLAS_REFERENCE_TAG=DataModelRunTests/DataModelRunTestsReference-01-00-03"
                  DEPENDS ${ARG_DEPENDS} )
endfunction (datamodel_run_test)

datamodel_run_test (DataModelTestWrite)
datamodel_run_test (DataModelTestRead DEPENDS DataModelTestWrite)
datamodel_run_test (DataModelTestRead2 DEPENDS DataModelTestRead)
datamodel_run_test (AuxDataTestWrite) #i:auxdata.root
datamodel_run_test (AuxDataTestRead DEPENDS AuxDataTestWrite)
datamodel_run_test (AuxDataTestRead2 DEPENDS AuxDataTestRead)
datamodel_run_test (AuxDataTestTypelessRead DEPENDS AuxDataTestWrite)
datamodel_run_test (AuxDataTestRead3 DEPENDS AuxDataTestTypelessRead)
datamodel_run_test (xAODTestWrite) # in:xaoddata.root
datamodel_run_test (xAODTestRead DEPENDS xAODTestWrite) #i:xaoddata.root o:xaoddata2.root
datamodel_run_test (xAODTestRead2 DEPENDS xAODTestRead) #i:xaoddata2.root o:xaoddata2x.root
datamodel_run_test (xAODTestTypelessRead DEPENDS xAODTestWrite)
datamodel_run_test (xAODTestRead3 DEPENDS xAODTestTypelessRead)
datamodel_run_test (xAODTestRead3MT
                    COMMAND "athena.py --threads=1"
                    EXTRA_IGNORE "|^xAODTestRead"
                    DEPENDS xAODTestTypelessRead)
datamodel_run_test (xAODTestReadRename DEPENDS xAODTestWrite)
datamodel_run_test (xAODTestReadRenameMT
                    COMMAND "athena.py --threads=1"
                    DEPENDS xAODTestWrite)
datamodel_run_test (xAODTestReadFilter DEPENDS xAODTestWrite)
datamodel_run_test (xAODTestReadFilterRead DEPENDS xAODTestReadFilter)
datamodel_run_test (xAODRootTestRead
                    COMMAND python
                    ARG ${CMAKE_CURRENT_SOURCE_DIR}/share/xAODRootTestRead_t.py
                    DEPENDS xAODTestWrite)
datamodel_run_test (xAODRootTestRead2
                    COMMAND python
                    ARG ${CMAKE_CURRENT_SOURCE_DIR}/share/xAODRootTestRead2_t.py
                    DEPENDS xAODRootTestRead)

datamodel_run_test (ByteStreamTestWrite)
datamodel_run_test (ByteStreamTestRead DEPENDS ByteStreamTestWrite)

datamodel_run_test (xAODTestSymlinks1)
datamodel_run_test (xAODTestSymlinks1MT
                    EXTRA_IGNORE "|^xAODTestRead"
                    COMMAND "athena.py --threads=1")

datamodel_run_test (xAODTestSymlinks2
                    DEPENDS xAODTestWrite)
datamodel_run_test (xAODTestSymlinks2MT
                    COMMAND "athena.py --threads=1"
                    DEPENDS xAODTestWrite)

datamodel_run_test (xAODTestDecorHandle1)
datamodel_run_test (xAODTestDecorHandle1MT
                    COMMAND "athena.py --threads=1")

datamodel_run_test (xAODTestDecorHandle2
                    DEPENDS xAODTestWrite)
datamodel_run_test (xAODTestDecorHandle2MT
                    COMMAND "athena.py --threads=1"
                    DEPENDS xAODTestWrite)

datamodel_run_test (CondWriter)
datamodel_run_test (CondReader
                    DEPENDS CondWriter)
datamodel_run_test (CondReaderMT
                    COMMAND "athena.py --threads=1"
                    DEPENDS CondWriter)

datamodel_run_test (CondReadWrite)

datamodel_run_test (xAODTestWriteThinned)
datamodel_run_test (xAODTestReadThinned DEPENDS xAODTestWriteThinned)


##############################################################################
# AllocTest
#

datamodel_run_test (AllocTestWriteWithoutAlloc)
datamodel_run_test (AllocTestWriteWithAlloc)

datamodel_run_test (AllocTestReadNoAllocWithoutAlloc
                    COMMAND "athena.py --filesInput=alloctestWithoutAlloc.root"
                    ARG DataModelRunTests/AllocTestReadWithoutAlloc_jo.py
                    DEPENDS AllocTestWriteWithoutAlloc)
datamodel_run_test (AllocTestReadAllocWithAlloc
                    COMMAND "athena.py --filesInput=alloctestWithAlloc.root"
                    ARG DataModelRunTests/AllocTestReadWithAlloc_jo.py
                    DEPENDS AllocTestWriteWithAlloc)

datamodel_run_test (AllocTestReadNoAllocWithAlloc
                    COMMAND "athena.py --filesInput=alloctestWithoutAlloc.root"
                    ARG DataModelRunTests/AllocTestReadWithAlloc_jo.py
                    DEPENDS AllocTestWriteWithoutAlloc)
datamodel_run_test (AllocTestReadAllocWithoutAlloc
                    COMMAND "athena.py --filesInput=alloctestWithAlloc.root"
                    ARG DataModelRunTests/AllocTestReadWithoutAlloc_jo.py
                    DEPENDS AllocTestWriteWithAlloc)

datamodel_run_test (RootAllocTestReadNoAllocWithoutAlloc
                    COMMAND python
                    ARG ${CMAKE_CURRENT_SOURCE_DIR}/share/RootAllocTestReadNoAllocWithoutAlloc_t.py
                    DEPENDS AllocTestWriteWithoutAlloc)
datamodel_run_test (RootAllocTestReadAllocWithAlloc
                    COMMAND python
                    ARG ${CMAKE_CURRENT_SOURCE_DIR}/share/RootAllocTestReadAllocWithAlloc_t.py
                    DEPENDS AllocTestWriteWithAlloc)

datamodel_run_test (RootAllocTestReadNoAllocWithAlloc
                    COMMAND python
                    ARG ${CMAKE_CURRENT_SOURCE_DIR}/share/RootAllocTestReadNoAllocWithAlloc_t.py
                    DEPENDS AllocTestWriteWithoutAlloc)
datamodel_run_test (RootAllocTestReadAllocWithoutAlloc
                    COMMAND python
                    ARG ${CMAKE_CURRENT_SOURCE_DIR}/share/RootAllocTestReadAllocWithoutAlloc_t.py
                    DEPENDS AllocTestWriteWithAlloc)


##############################################################################


# xAODTestWrite:
# writes xaoddata.root (cvec, cvecWD, cview, gvec, ctrig, cinfo, hvec, hview, S2)
#        
# xAODTestRead:
# reads xaoddata.root
# writes xaoddata2.root (cvec, cvecWD, cview, gvec, ctrig, cinfo, hvec, hview,
#                        copy_cvec, copy_cvecWD, copy_cview, copy_gvec, copy_ctrig,
#                        copy_cinfo, copy_hvec, copy_hview)
# writes xaoddata2b.root (same, with different decor)
#
# xAODTestRead2
# reads xaoddata2.root
# writes xaoddata2x.root (cvec)
#
# xAODTestRead2b
# reads xaoddata2b.root
#
# xAODTestTypelessRead
# reads xaoddata.root
# writes xaoddata3.root (cvec, cvecWD, cview, ctrig, cinfo, hvec, hview,
#                        copy_cvec, copy_cvecWD, copy_cview, copy_ctrig,
#                        copy_cinfo, copy_hvec, copy_hview)
#
# xAODTestRead3
# reads xaoddata3.root
#
# xAODTestReadRename
# reads xaoddata.root
#
# xAODTestReadFilter
# reads xaoddata.root
# writes xaoddata_filt.root (cvec)
#
# xAODTestReadFilterRead
# reads xaoddata_filt.root
#
# xAODRootTestRead
# reads xaoddata.root
# writes xaodroot.root
#
# xAODRootTestRead2
# reads xaoddata.root

