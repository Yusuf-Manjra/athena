#Copyright(C) 2002 - 2022 CERN for the benefit of the ATLAS collaboration

#Declare the package name:
atlas_subdir( ISF_FastCaloSimEvent )

#External dependencies:
find_package( Boost )
find_package( CLHEP )
find_package( HepPDT )
find_package( ROOT COMPONENTS Core Tree MathCore MathMore Hist RIO Matrix Physics )
find_package( lwtnn REQUIRED )
find_package( LibXml2 )

option(USE_GPU "whether to run FCS on GPU or not" OFF)

if(USE_GPU AND CMAKE_CUDA_COMPILER)
  add_definitions( -DUSE_GPU)
endif()

#Remove the-- as - needed linker flags:
atlas_disable_as_needed()

#Component(s) in the package:
atlas_add_root_dictionary( ISF_FastCaloSimEvent _dictSource
                           ROOT_HEADERS ISF_FastCaloSimEvent/IntArray.h
                           ISF_FastCaloSimEvent/DoubleArray.h
                           ISF_FastCaloSimEvent/TFCSFunction.h
                           ISF_FastCaloSimEvent/TFCS1DFunction.h
                           ISF_FastCaloSimEvent/TFCS1DFunctionHistogram.h
                           ISF_FastCaloSimEvent/TFCS1DFunctionInt16Histogram.h
                           ISF_FastCaloSimEvent/TFCS1DFunctionInt32Histogram.h
                           ISF_FastCaloSimEvent/TFCS1DFunctionRegression.h
                           ISF_FastCaloSimEvent/TFCS1DFunctionRegressionTF.h
                           ISF_FastCaloSimEvent/TFCS1DFunctionSpline.h
                           ISF_FastCaloSimEvent/TFCS1DFunctionTemplateHelpers.h
                           ISF_FastCaloSimEvent/TFCS1DFunctionTemplateHistogram.h
                           ISF_FastCaloSimEvent/TFCS1DFunctionTemplateInterpolationHistogram.h
                           ISF_FastCaloSimEvent/TFCS2DFunction.h
                           ISF_FastCaloSimEvent/TFCS2DFunctionHistogram.h
                           ISF_FastCaloSimEvent/TFCS2DFunctionTemplateHistogram.h
                           ISF_FastCaloSimEvent/TFCS2DFunctionTemplateInterpolationHistogram.h
                           ISF_FastCaloSimEvent/TFCS2DFunctionTemplateInterpolationExpHistogram.h
                           ISF_FastCaloSimEvent/TFCSParametrizationBase.h
                           ISF_FastCaloSimEvent/TFCSParametrizationPlaceholder.h
                           ISF_FastCaloSimEvent/TFCSParametrization.h
                           ISF_FastCaloSimEvent/TFCSInvisibleParametrization.h
                           ISF_FastCaloSimEvent/TFCSInitWithEkin.h
                           ISF_FastCaloSimEvent/TFCSEnergyInterpolationHistogram.h
                           ISF_FastCaloSimEvent/TFCSEnergyInterpolationLinear.h
                           ISF_FastCaloSimEvent/TFCSEnergyInterpolationPiecewiseLinear.h
                           ISF_FastCaloSimEvent/TFCSEnergyInterpolationSpline.h
                           ISF_FastCaloSimEvent/TFCSParametrizationChain.h
                           ISF_FastCaloSimEvent/TFCSParametrizationBinnedChain.h
                           ISF_FastCaloSimEvent/TFCSParametrizationFloatSelectChain.h
                           ISF_FastCaloSimEvent/TFCSParametrizationPDGIDSelectChain.h
                           ISF_FastCaloSimEvent/TFCSParametrizationEbinChain.h
                           ISF_FastCaloSimEvent/TFCSParametrizationEkinSelectChain.h
                           ISF_FastCaloSimEvent/TFCSParametrizationEtaSelectChain.h
                           ISF_FastCaloSimEvent/TFCSParametrizationAbsEtaSelectChain.h
                           ISF_FastCaloSimEvent/TFCSEnergyParametrization.h
                           ISF_FastCaloSimEvent/TFCSPCAEnergyParametrization.h
                           ISF_FastCaloSimEvent/TFCSEnergyBinParametrization.h
                           ISF_FastCaloSimEvent/TFCSGANXMLParameters.h
                           ISF_FastCaloSimEvent/TFCSGANLWTNNHandler.h
                           ISF_FastCaloSimEvent/TFCSGANEtaSlice.h
                           ISF_FastCaloSimEvent/TFCSEnergyAndHitGAN.h
                           ISF_FastCaloSimEvent/TFCSEnergyAndHitGANV2.h
                           ISF_FastCaloSimEvent/TFCSPredictExtrapWeights.h
                           ISF_FastCaloSimEvent/TFCSLateralShapeParametrization.h
                           ISF_FastCaloSimEvent/TFCSLateralShapeParametrizationHitBase.h
                           ISF_FastCaloSimEvent/TFCSLateralShapeParametrizationHitChain.h
                           ISF_FastCaloSimEvent/TFCSLateralShapeParametrizationFluctChain.h
                           ISF_FastCaloSimEvent/TFCSCenterPositionCalculation.h
                           ISF_FastCaloSimEvent/TFCSFlatLateralShapeParametrization.h
                           ISF_FastCaloSimEvent/TFCSHistoLateralShapeParametrization.h
                           ISF_FastCaloSimEvent/TFCSHistoLateralShapeParametrizationFCal.h
                           ISF_FastCaloSimEvent/TFCS2DFunctionLateralShapeParametrization.h
                           ISF_FastCaloSimEvent/TFCSHistoLateralShapeWeight.h
                           ISF_FastCaloSimEvent/TFCSHistoLateralShapeGausLogWeight.h
                           ISF_FastCaloSimEvent/TFCSHistoLateralShapeWeightHitAndMiss.h
                           ISF_FastCaloSimEvent/TFCSHistoLateralShapeGausLogWeightHitAndMiss.h
                           ISF_FastCaloSimEvent/TFCSLateralShapeParametrizationHitNumberFromE.h
                           ISF_FastCaloSimEvent/TFCSLateralShapeTuning.h
                           ISF_FastCaloSimEvent/TFCSHitCellMapping.h
                           ISF_FastCaloSimEvent/TFCSHitCellMappingFCal.h
                           ISF_FastCaloSimEvent/TFCSHitCellMappingWiggle.h
                           ISF_FastCaloSimEvent/TFCSHitCellMappingWiggleEMB.h
                           ISF_FastCaloSimEvent/TFCSEnergyRenormalization.h
                           ISF_FastCaloSimEvent/TFCSExtrapolationState.h
                           ISF_FastCaloSimEvent/TFCSSimulationState.h
                           ISF_FastCaloSimEvent/TFCSTruthState.h
                           ISF_FastCaloSimEvent/TFCSVoxelHistoLateralCovarianceFluctuations.h
			   ISF_FastCaloSimEvent/LinkDef.h
			   EXTERNAL_PACKAGES HepPDT)

if(USE_GPU AND CMAKE_CUDA_COMPILER)
  message("compiling ISF_FastCaoSimEvent using cuda")
  atlas_add_library( ISF_FastCaloSimEvent
    ISF_FastCaloSimEvent/*.h ${_dictSource} src/*.cxx
    PUBLIC_HEADERS ISF_FastCaloSimEvent
    INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${HEPPDT_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${LIBXML2_INCLUDE_DIRS} ${LWTNN_INCLUDE_DIRS}
    DEFINITIONS ${CLHEP_DEFINITIONS}
    LINK_LIBRARIES ${CLHEP_LIBRARIES} ${HEPPDT_LIBRARIES} ${ROOT_LIBRARIES} AthContainers AthenaKernel AthenaBaseComps CaloDetDescrLib ${LWTNN_LIBRARIES} ${LIBXML2_LIBRARIES}
    CaloGeoHelpers CxxUtils TileSimEvent ISF_FastCaloGpuLib
    PRIVATE_LINK_LIBRARIES GaudiKernel )
else()
  atlas_add_library( ISF_FastCaloSimEvent
    ISF_FastCaloSimEvent/*.h ${_dictSource} src/*.cxx
    PUBLIC_HEADERS ISF_FastCaloSimEvent
    INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${HEPPDT_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${LIBXML2_INCLUDE_DIRS} ${LWTNN_INCLUDE_DIRS}
    DEFINITIONS ${CLHEP_DEFINITIONS}
    LINK_LIBRARIES ${Boost_LIBRARIES} ${CLHEP_LIBRARIES} ${HEPPDT_LIBRARIES} ${ROOT_LIBRARIES} AthContainers AthenaKernel AthenaBaseComps CaloDetDescrLib ${LWTNN_LIBRARIES} ${LIBXML2_LIBRARIES}
    CaloGeoHelpers CxxUtils TileSimEvent
    PRIVATE_LINK_LIBRARIES GaudiKernel )
endif()

atlas_add_dictionary( ISF_FastCaloSimEventDict
   ISF_FastCaloSimEvent/ISF_FastCaloSimEventDict.h
   ISF_FastCaloSimEvent/selection.xml
   LINK_LIBRARIES ISF_FastCaloSimEvent )
