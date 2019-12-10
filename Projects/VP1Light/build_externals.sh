#!/bin/bash
#
# Script building all the externals necessary for the nightly build.
#

# Stop on errors:
set -e

# Function printing the usage information for the script
usage() {
    echo "Usage: build_externals.sh [-t build_type] [-b build_dir] [-f] [-c] [-x]"
    echo " -f: Force rebuild of externals, otherwise if script"
    echo "     finds an external build present it will simply exit"
    echo " -c: Build the externals for the continuous integration (CI) system,"
    echo "     skipping the build of the externals RPMs."
    echo " -x: Extra cmake argument(s) to provide for the build(configuration)"
    echo "     of all externals needed by Athena."
    echo "If a build_dir is not given the default is '../build'"
    echo "relative to the athena checkout"
}

# Parse the command line arguments:
BUILDDIR=""
BUILDTYPE="RelWithDebInfo"
FORCE=""
CI=""
EXTRACMAKE=()
while getopts ":t:b:fch" opt; do
    case $opt in
        t)
            BUILDTYPE=$OPTARG
            ;;
        b)
            BUILDDIR=$OPTARG
            ;;
        f)
            FORCE="1"
            ;;
        c)
            CI="1"
            ;;
        x)
            EXTRACMAKE+=($OPTARG)
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

# We are in BASH, get the path of this script in a simple way:
thisdir=$(dirname ${BASH_SOURCE[0]})
thisdir=$(cd ${thisdir};pwd)

# Go to the main directory of the repository:
cd ${thisdir}/../..

# Check if the user specified any source/build directories:
if [ "$BUILDDIR" = "" ]; then
    BUILDDIR=${thisdir}/../../../build
fi
mkdir -p ${BUILDDIR}
BUILDDIR=$(cd $BUILDDIR; pwd)

if [ "$FORCE" = "1" ]; then
    echo "Force deleting existing build area..."
    rm -fr ${BUILDDIR}/install/VP1LightExternals
    rm -fr ${BUILDDIR}/src/VP1LightExternals
    rm -fr ${BUILDDIR}/build/VP1LightExternals
fi

# Create some directories:
mkdir -p ${BUILDDIR}/install

# Set some environment variables that the builds use internally:
export NICOS_PROJECT_VERSION=`cat ${thisdir}/version.txt`
export NICOS_ATLAS_RELEASE=${NICOS_PROJECT_VERSION}
export NICOS_PROJECT_RELNAME=${NICOS_PROJECT_VERSION}

# The directory holding the helper scripts:
scriptsdir=${thisdir}/../../Build/AtlasBuildScripts
scriptsdir=$(cd ${scriptsdir}; pwd)

# Flag for triggering the build of RPMs for the externals:
RPMOPTIONS="-r ${BUILDDIR}"
if [ "$CI" = "1" ]; then
    RPMOPTIONS=
fi

# Read in the tag/branch to use for VP1LightExternals:
VP1LightExternalsVersion=$(awk '/^VP1LightExternalsVersion/{print $3}' ${thisdir}/externals.txt)

# Check out VP1LightExternals from the right branch/tag:
${scriptsdir}/checkout_atlasexternals.sh \
    -t ${VP1LightExternalsVersion} \
    -s ${BUILDDIR}/src/VP1LightExternals

# Build VP1LightExternals:
export NICOS_PROJECT_HOME=$(cd ${BUILDDIR}/install;pwd)/VP1LightExternals
${scriptsdir}/build_atlasexternals.sh \
    -s ${BUILDDIR}/src/VP1LightExternals \
    -b ${BUILDDIR}/build/VP1LightExternals \
    -i ${BUILDDIR}/install/VP1LightExternals/${NICOS_PROJECT_VERSION} \
    -p VP1LightExternals ${RPMOPTIONS} -t ${BUILDTYPE} \
    -v ${NICOS_PROJECT_VERSION} \
    ${EXTRACMAKE[@]/#/-x }
