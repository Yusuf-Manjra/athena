#!/bin/bash
#
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
# Script for building the release on top of externals built using one of the
# scripts in this directory.
#
_time_() { local c="time -p " ; while test "X$1" != "X" ; do c+=" \"$1\"" ; shift; done; ( eval "$c" ) 2>&1 | sed "s,^real[[:space:]],time::${c}:: real ," ; }

# Function printing the usage information for the script
usage() {
    echo "Usage: build.sh [-t build type] [-b build dir] [-c] [-m] [-i] [-p] [-a] [-x opt] [-N]"
    echo " -c: Execute CMake step"
    echo " -m: Execute make step"
    echo " -i: Execute install step"
    echo " -p: Execute CPack step"
    echo " -a: Abort on error"
    echo " -x: Extra configuration argument(s) for CMake"
    echo " -N: Use Ninja"

    echo "If none of the c, m, i or p options are set then the script will do"
    echo "*all* steps. Otherwise only the enabled steps are run - it's your"
    echo "reponsibility to ensure that precusors are in good shape"
}

# Parse the command line arguments:
BUILDDIR=""
BUILDTYPE="RelWithDebInfo"
EXE_CMAKE=""
EXE_MAKE=""
EXE_INSTALL=""
EXE_CPACK=""
NIGHTLY=true
EXTRACMAKE=()
BUILDTOOLTYPE=""
BUILDTOOL="make -k"
INSTALLRULE="install/fast"
while getopts ":t:b:hcmipax:N" opt; do
    case $opt in
        t)
            BUILDTYPE=$OPTARG
            ;;
        b)
            BUILDDIR=$OPTARG
            ;;
        c)
            EXE_CMAKE="1"
            ;;
        m)
            EXE_MAKE="1"
            ;;
        i)
            EXE_INSTALL="1"
            ;;
        p)
            EXE_CPACK="1"
            ;;
        a)
            NIGHTLY=false
            ;;
        x)
            EXTRACMAKE+=($OPTARG)
            ;;
        N)
            BUILDTOOL="ninja -k 0"
            BUILDTOOLTYPE="-GNinja"
            INSTALLRULE="install"
            ;;
        h)
            usage
            exit 0
            ;;
        :)
            echo "Argument -$OPTARG requires a parameter!"
            usage
            exit 1
            ;;
        ?)
            echo "Unknown argument: -$OPTARG"
            usage
            exit 1
            ;;
    esac
done

# If no step was explicitly specified, turn them all on:
if [ -z "$EXE_CMAKE" -a -z "$EXE_MAKE" -a -z "$EXE_INSTALL" -a -z "$EXE_CPACK" ]; then
    EXE_CMAKE="1"
    EXE_MAKE="1"
    EXE_INSTALL="1"
    EXE_CPACK="1"
fi

# Stop on errors from here on out:
set -e
# consider a pipe failed if ANY of the commands fails
set -o pipefail

{
 test "X${NIGHTLY_STATUS}" != "X" && {
    scriptsdir_nightly_status=${NIGHTLY_STATUS_SCRIPTS}
    test "X$scriptsdir_nightly_status" = "X" && scriptsdir_nightly_status=${scriptsdir}/nightly_status 
    test -x $scriptsdir_nightly_status/status_on_exit.sh  && trap $scriptsdir_nightly_status/status_on_exit.sh EXIT
 }
}

# Source in our environment
AthenaSrcDir=$(dirname ${BASH_SOURCE[0]})

# The directory holding the helper scripts:
scriptsdir=${AthenaSrcDir}/../../Build/AtlasBuildScripts
scriptsdir=$(cd ${scriptsdir}; pwd)

if [ -z "$BUILDDIR" ]; then
    BUILDDIR=${AthenaSrcDir}/../../../build
fi
mkdir -p ${BUILDDIR}
BUILDDIR=$(cd ${BUILDDIR} && pwd)
source $AthenaSrcDir/build_env.sh -b $BUILDDIR  >& ${BUILDDIR}/build_env.log
cat  ${BUILDDIR}/build_env.log

# create the actual build directory
mkdir -p ${BUILDDIR}/build/Athena
cd ${BUILDDIR}/build/Athena

# CMake:
if [ -n "$EXE_CMAKE" ]; then
    # Remove the CMakeCache.txt file, to force CMake to find externals
    # from scratch in an incremental build.
    rm -f CMakeCache.txt
    # Now run the actual CMake configuration:
    { _time_ cmake ${BUILDTOOLTYPE} -DCMAKE_BUILD_TYPE:STRING=${BUILDTYPE} \
        ${EXTRACMAKE[@]} \
        -DCTEST_USE_LAUNCHERS:BOOL=TRUE \
        ${AthenaSrcDir}; } 2>&1 | tee cmake_config.log
fi

#log analyzer never affects return status in the parent shell:
{
 test "X${NIGHTLY_STATUS}" != "X" && {
    branch=$(basename $(cd ${BUILDDIR}/.. ; pwd)) #FIXME: should be taken from env.
    timestamp_tmp=` basename ${BUILDDIR}/../.@@__* 2>/dev/null | sed 's,^\.,,' `
    test "X$timestamp_tmp" != "X" || {
        timestamp_tmp=@@__`date "+%Y-%m-%dT%H%M"`__@@ #to be used until the final stamp from ReleaseData is available
        touch ${BUILDDIR}/../.${timestamp_tmp}
    }
    (set +e
     ${scriptsdir_nightly_status}/cmake_config_status.sh "$branch" "$BINARY_TAG" "$timestamp_tmp" Athena ${BUILDDIR}/build/Athena/cmake_config.log 
    )
 } || true
}

# for nightly builds we want to get as far as we can
if [ "$NIGHTLY" = true ]; then
    # At this point stop worrying about errors:
    set +e
fi

# make:
if [ -n "$EXE_MAKE" ]; then
    # Forcibly remove the merged CLID file from the previous build, to
    # avoid issues with some library possibly changing the name/CLID
    # of something during the build. Note that ${platform} is coming from
    # the build_env.sh script.
    rm -f ${platform}/share/clid.db
    # Build the project.
    { _time_ ${BUILDTOOL}; } 2>&1 | tee cmake_build.log
fi

{
 test "X${NIGHTLY_STATUS}" != "X" && {
    (set +e
     ${scriptsdir_nightly_status}/cmake_build_status.sh  "$branch" "$BINARY_TAG" "$timestamp_tmp" Athena ${BUILDDIR}/build/Athena/cmake_build.log 
    )
 } || true
}

# Install the results:
if [ -n "$EXE_INSTALL" ]; then
    { DESTDIR=${BUILDDIR}/install _time_ ${BUILDTOOL} ${INSTALLRULE}; } \
	 2>&1 | tee cmake_install.log
fi
#^^^ do we need to analyze local install logs?

# Build an RPM for the release:
if [ -n "$EXE_CPACK" ]; then
    { _time_ cpack; } 2>&1 | tee cmake_cpack.log
    if [ "$BUILDTYPE" = "RelWithDebInfo" ]; then
	{ _time_ cpack --config CPackDbgRPMConfig.cmake; } 2>&1 | tee -a cmake_cpack.log
    fi
    cp Athena*.rpm ${BUILDDIR}/
fi

{
 test "X${NIGHTLY_STATUS}" != "X" && {
    (set +e
     for project in `ls ${BUILDDIR}/build 2>/dev/null` ; do
        ${scriptsdir_nightly_status}/cmake_cpack_status.sh  "$branch" "$BINARY_TAG" "$timestamp_tmp" "${project}" ${BUILDDIR}/build/${project}/cmake_cpack.log 
     done
     ${scriptsdir_nightly_status}/collect_nightly_status.sh "$branch" "$BINARY_TAG" "$timestamp_tmp" "$BUILDDIR"
    )
 } || true
}
