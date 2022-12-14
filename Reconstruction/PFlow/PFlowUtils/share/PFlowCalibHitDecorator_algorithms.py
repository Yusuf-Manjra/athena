from CaloCalibHitRec.CaloCalibHitRecConf import CaloCalibClusterTruthMapMakerAlgorithm
CaloCalibClusterTruthMapMakerAlgorithm = CaloCalibClusterTruthMapMakerAlgorithm()
topSequence += CaloCalibClusterTruthMapMakerAlgorithm

from CaloCalibHitRec.CaloCalibHitRecConf import CaloCalibClusterDecoratorAlgorithm
CaloCalibClusterDecoratorAlgorithm = CaloCalibClusterDecoratorAlgorithm()

from CaloCalibHitRec.CaloCalibHitRecConf import CaloCalibClusterTruthAttributerTool as CalibClusterTruthAttributerTool
CaloCalibClusterTruthAttributerTool = CalibClusterTruthAttributerTool()
CaloCalibClusterDecoratorAlgorithm.TruthAttributerTool = CaloCalibClusterTruthAttributerTool
topSequence += CaloCalibClusterDecoratorAlgorithm

from PFlowUtils.PFlowUtilsConf import PFlowCalibPFODecoratorAlgorithm
PFlowCalibPFODecoratorAlgorithm = PFlowCalibPFODecoratorAlgorithm()
PFlowCalibPFOTruthAttributerTool = CalibClusterTruthAttributerTool("PFlowCalibPFOTruthAttributerTool")
PFlowCalibPFODecoratorAlgorithm.TruthAttributerTool = PFlowCalibPFOTruthAttributerTool

topSequence+=PFlowCalibPFODecoratorAlgorithm
