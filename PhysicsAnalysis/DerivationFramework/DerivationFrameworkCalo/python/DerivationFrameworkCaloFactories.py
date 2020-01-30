# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

# Declares a GainDecorator factory. To add it to your decoration:
# from DerivationFrameworkCalo.DerivationFrameworkCaloFactories import GainDecorator, getExtraContent
# GainDecoratorTool = GainDecorator()
# MySlimmingHelper.ExtraVariables.extend( getGainDecorations(GainDecoratorTool) )

# Currently it adds:
# ['Electrons.E_Lr0_HiG', 'Electrons.E_Lr0_MedG', 'Electrons.E_Lr0_LowG', 'Electrons.E_Lr1_HiG', 'Electrons.E_Lr1_MedG', 'Electrons.E_Lr1_LowG', 'Electrons.E_Lr2_HiG', 'Electrons.E_Lr2_MedG', 'Electrons.E_Lr2_LowG', 'Electrons.E_Lr3_HiG', 'Electrons.E_Lr3_MedG', 'Electrons.E_Lr3_LowG', 'Electrons.nCells_Lr0_HiG', 'Electrons.nCells_Lr0_MedG', 'Electrons.nCells_Lr0_LowG', 'Electrons.nCells_Lr1_HiG', 'Electrons.nCells_Lr1_MedG', 'Electrons.nCells_Lr1_LowG', 'Electrons.nCells_Lr2_HiG', 'Electrons.nCells_Lr2_MedG', 'Electrons.nCells_Lr2_LowG', 'Electrons.nCells_Lr3_HiG', 'Electrons.nCells_Lr3_MedG', 'Electrons.nCells_Lr3_LowG', 'Photons.E_Lr0_HiG', 'Photons.E_Lr0_MedG', 'Photons.E_Lr0_LowG', 'Photons.E_Lr1_HiG', 'Photons.E_Lr1_MedG', 'Photons.E_Lr1_LowG', 'Photons.E_Lr2_HiG', 'Photons.E_Lr2_MedG', 'Photons.E_Lr2_LowG', 'Photons.E_Lr3_HiG', 'Photons.E_Lr3_MedG', 'Photons.E_Lr3_LowG', 'Photons.nCells_Lr0_HiG', 'Photons.nCells_Lr0_MedG', 'Photons.nCells_Lr0_LowG', 'Photons.nCells_Lr1_HiG', 'Photons.nCells_Lr1_MedG', 'Photons.nCells_Lr1_LowG', 'Photons.nCells_Lr2_HiG', 'Photons.nCells_Lr2_MedG', 'Photons.nCells_Lr2_LowG', 'Photons.nCells_Lr3_HiG', 'Photons.nCells_Lr3_MedG', 'Photons.nCells_Lr3_LowG']

from egammaRec.Factories import ToolFactory, getPropertyValue, FullNameWrapper
from egammaRec import egammaKeys
from DerivationFrameworkCalo.DerivationFrameworkCaloConf import DerivationFramework__GainDecorator, DerivationFramework__ClusterEnergyPerLayerDecorator, DerivationFramework__ClusterDecorator, DerivationFramework__CellReweight, DerivationFramework__EGammaReweight
from CaloClusterCorrection.CaloClusterCorrectionConf import CaloFillRectangularCluster as CFRC
from GaudiKernel.Constants import DEBUG
from egammaRec.egammaRecFlags import jobproperties # to set jobproperties.egammaRecFlags
from egammaTools import egammaToolsConf
from CaloRec.CaloRecConf import CaloCellContainerFinalizerTool
from egammaTools.egammaToolsFactories import EMShowerBuilder



################
# GainDecorator
################
GainDecorator = ToolFactory( DerivationFramework__GainDecorator, 
                             SGKey_electrons = egammaKeys.outputElectronKey(),
                             SGKey_photons = egammaKeys.outputPhotonKey(), 
                             OutputLevel=DEBUG )


############################
# CaloFillRectangularCluster
############################
# Define defaults for CaloFillRectangularCluster and ClusterEnergyPerLayerDecorator
# will be overridden later depending on the number of cells neta X nphi
CaloFillRectangularCluster = ToolFactory( CFRC, 
                                          cells_name = egammaKeys.caloCellKey(), 
                                          fill_cluster = True )


################################
# ClusterEnergyPerLayerDecorator
################################
ClusterEnergyPerLayerDecorator = ToolFactory( DerivationFramework__ClusterEnergyPerLayerDecorator,
                                              CaloFillRectangularTool = CaloFillRectangularCluster,
                                              SGKey_electrons = egammaKeys.outputElectronKey(),
                                              SGKey_photons = egammaKeys.outputPhotonKey(), 
                                              SGKey_caloCells = egammaKeys.caloCellKey() )

ClusterEnergyPerLayerDecoratorNew = ToolFactory( DerivationFramework__ClusterEnergyPerLayerDecorator,
                                                 CaloFillRectangularTool = CaloFillRectangularCluster,
                                                 SGKey_electrons = "NewSwElectrons",
                                                 SGKey_photons = "NewSwPhotons",
                                                 SGKey_caloCells = "NewCellContainer" )

ClusterEnergyPerLayerDecoratorMaxVar = ToolFactory( DerivationFramework__ClusterEnergyPerLayerDecorator,
                                                    CaloFillRectangularTool = CaloFillRectangularCluster,
                                                    SGKey_electrons = "MaxVarSwElectrons",
                                                    SGKey_photons = "MaxVarSwPhotons",
                                                    SGKey_caloCells = "MaxVarCellContainer" )

ClusterEnergyPerLayerDecoratorMinVar = ToolFactory( DerivationFramework__ClusterEnergyPerLayerDecorator,
                                                    CaloFillRectangularTool = CaloFillRectangularCluster,
                                                    SGKey_electrons = "MinVarSwElectrons",
                                                    SGKey_photons = "MinVarSwPhotons",
                                                    SGKey_caloCells = "MinVarCellContainer" )

###################
# getGainLayerNames
###################
def getGainLayerNames( obj ):
  """getGainLayerNames( obj ) -> return a list of names of the decorations added to the
  egamma object, given the GainDecorator object"""
  return [getPropertyValue(obj, "decoration_pattern").format(info=info, layer=layer, gain=gain) \
    for info in ["E", "nCells"] \
    for layer in getPropertyValue(obj, "layers") \
    for gain in getPropertyValue(obj, "gain_names").values() ]


#########################
# getGainLayerDecorations
#########################
def getGainDecorations( obj, 
  collections=[egammaKeys.outputElectronKey(), egammaKeys.outputPhotonKey()],
  info = ["E", "nCells"] ):
  """getGainDecorations( obj, collections=["Electrons", "Photons"] ) -> 
  Return a list with the 'ExtraContent' to be added to the decorations to save the gain
  information per layer"""
  return ["{part}.{info}".format(part=part, info=info) for part in collections \
    for info in getGainLayerNames(obj) ]


###################################
# getClusterEnergyPerLayerDecorator
###################################
def getClusterEnergyPerLayerDecorator( neta, nphi, prefix = '', **kw ):
  """Return a factory for ClusterEnergyPerLayerDecorator given neta, nphi"""
  return ClusterEnergyPerLayerDecorator.copy( 
    name = "%sClusterEnergyPerLayerDecorator_%sx%s" % (prefix, neta, nphi),
    CaloFillRectangularTool = CaloFillRectangularCluster.copy( 
      name = "%sCaloFillRectangularCluster_%sx%s" % (prefix, neta, nphi), 
      eta_size = neta,
      phi_size = nphi),
    neta = neta,
    nphi = nphi,
    **kw)

def getClusterEnergyPerLayerDecoratorNew( neta, nphi, prefix = '', **kw ):
  """Return a factory for ClusterEnergyPerLayerDecorator given neta, nphi"""
  return ClusterEnergyPerLayerDecoratorNew.copy( 
    name = "%sClusterEnergyPerLayerDecoratorNew_%sx%s" % (prefix, neta, nphi),
    CaloFillRectangularTool = CaloFillRectangularCluster.copy( 
      name = "%sCaloFillRectangularClusterNew_%sx%s" % (prefix, neta, nphi), 
      eta_size = neta,
      phi_size = nphi),
    neta = neta,
    nphi = nphi,
    **kw)

def getClusterEnergyPerLayerDecoratorMaxVar( neta, nphi, prefix = '', **kw ):
  """Return a factory for ClusterEnergyPerLayerDecorator given neta, nphi"""
  return ClusterEnergyPerLayerDecoratorMaxVar.copy( 
    name = "%sClusterEnergyPerLayerDecoratorMaxVar_%sx%s" % (prefix, neta, nphi),
    CaloFillRectangularTool = CaloFillRectangularCluster.copy( 
      name = "%sCaloFillRectangularClusterMaxVar_%sx%s" % (prefix, neta, nphi), 
      eta_size = neta,
      phi_size = nphi),
    neta = neta,
    nphi = nphi,
    **kw)

def getClusterEnergyPerLayerDecoratorMinVar( neta, nphi, prefix = '', **kw ):
  """Return a factory for ClusterEnergyPerLayerDecorator given neta, nphi"""
  return ClusterEnergyPerLayerDecoratorMinVar.copy( 
    name = "%sClusterEnergyPerLayerDecoratorMinVar_%sx%s" % (prefix, neta, nphi),
    CaloFillRectangularTool = CaloFillRectangularCluster.copy( 
      name = "%sCaloFillRectangularClusterMinVar_%sx%s" % (prefix, neta, nphi), 
      eta_size = neta,
      phi_size = nphi),
    neta = neta,
    nphi = nphi,
    **kw)

#####################################
# getClusterEnergyPerLayerDecorations
#####################################
def getClusterEnergyPerLayerDecorations( obj ):
  """getClusterEnergyPerLayerDecorations( obj ) -> return a list of names of the
  decorations added to the egamma object, given the ClusterEnergyPerLayerDecorations
  object (e.g. Photons.E7x11_Lr0, ...)"""
  properties = 'SGKey_photons', 'SGKey_electrons'
  collections = filter(bool, (getPropertyValue(obj, x) for x in properties) )
  return ['{part}.E{neta}x{nphi}_Lr{layer}'.format(part=part, 
    neta=getPropertyValue(obj, 'neta'),
    nphi=getPropertyValue(obj, 'nphi'),
    layer=layer) \
    for layer in getPropertyValue(obj, "layers") \
    for part in collections]



#############################
# configureClusterCorrections
#############################
def configureClusterCorrections(swTool):
  "Add attributes ClusterCorrectionToolsXX to egammaSwTool object"
  from CaloClusterCorrection.CaloSwCorrections import  make_CaloSwCorrections, rfac, etaoff_b1, etaoff_e1, \
      etaoff_b2,etaoff_e2,phioff_b2,phioff_e2,update,time,listBadChannel
  from CaloRec.CaloRecMakers import _process_tools
 
  clusterTypes = dict(
    Ele35='ele35', Ele55='ele55', Ele37='ele37',
    Gam35='gam35_unconv', Gam55='gam55_unconv',Gam37='gam37_unconv',
    Econv35='gam35_conv', Econv55='gam55_conv', Econv37='gam37_conv'
    )
  for attrName, clName in clusterTypes.iteritems():
    x = 'ClusterCorrectionTools' + attrName
    if not hasattr(swTool, x) or getattr(swTool, x):
      continue
    y = make_CaloSwCorrections(clName, suffix='EGDF',
                               version = jobproperties.egammaRecFlags.clusterCorrectionVersion(),
                               cells_name=egammaKeys.caloCellKey() )
    setattr(swTool, x, _process_tools (swTool, y) )


#########################################
# configureClusterCorrectionsWithNewCells
#########################################
def configureClusterCorrectionsWithNewCells(swTool):
  "Add attributes ClusterCorrectionToolsXX to egammaSwToolWithNewCells object"
  from CaloClusterCorrection.CaloSwCorrections import  make_CaloSwCorrections, rfac, etaoff_b1, etaoff_e1, \
      etaoff_b2,etaoff_e2,phioff_b2,phioff_e2,update,time,listBadChannel
  from CaloRec.CaloRecMakers import _process_tools

  clusterTypes = dict(
    Ele35='ele35', Ele55='ele55', Ele37='ele37',
    Gam35='gam35_unconv', Gam55='gam55_unconv',Gam37='gam37_unconv',
    Econv35='gam35_conv', Econv55='gam55_conv', Econv37='gam37_conv'
    )
  for attrName, clName in clusterTypes.iteritems():
    x = 'ClusterCorrectionTools' + attrName
    if not hasattr(swTool, x) or getattr(swTool, x):
      continue
    y = make_CaloSwCorrections(clName, suffix='EGDF',
                               version = jobproperties.egammaRecFlags.clusterCorrectionVersion(),
                               cells_name='NewCellContainer')
    setattr(swTool, x, _process_tools (swTool, y) )

############################################
# configureClusterCorrectionsWithMaxVarCells
############################################
def configureClusterCorrectionsWithMaxVarCells(swTool):
  "Add attributes ClusterCorrectionToolsXX to egammaSwToolWithMaxVarCells object"
  from CaloClusterCorrection.CaloSwCorrections import  make_CaloSwCorrections, rfac, etaoff_b1, etaoff_e1, \
      etaoff_b2,etaoff_e2,phioff_b2,phioff_e2,update,time,listBadChannel
  from CaloRec.CaloRecMakers import _process_tools

  clusterTypes = dict(
    Ele35='ele35', Ele55='ele55', Ele37='ele37',
    Gam35='gam35_unconv', Gam55='gam55_unconv',Gam37='gam37_unconv',
    Econv35='gam35_conv', Econv55='gam55_conv', Econv37='gam37_conv'
    )
  for attrName, clName in clusterTypes.iteritems():
    x = 'ClusterCorrectionTools' + attrName
    if not hasattr(swTool, x) or getattr(swTool, x):
      continue
    y = make_CaloSwCorrections(clName, suffix='EGDF',
                               version = jobproperties.egammaRecFlags.clusterCorrectionVersion(),
                               cells_name='MaxVarCellContainer')
    setattr(swTool, x, _process_tools (swTool, y) )
    
############################################
# configureClusterCorrectionsWithMinVarCells
############################################
def configureClusterCorrectionsWithMinVarCells(swTool):
  "Add attributes ClusterCorrectionToolsXX to egammaSwToolWithMinVarCells object"
  from CaloClusterCorrection.CaloSwCorrections import  make_CaloSwCorrections, rfac, etaoff_b1, etaoff_e1, \
      etaoff_b2,etaoff_e2,phioff_b2,phioff_e2,update,time,listBadChannel
  from CaloRec.CaloRecMakers import _process_tools

  clusterTypes = dict(
    Ele35='ele35', Ele55='ele55', Ele37='ele37',
    Gam35='gam35_unconv', Gam55='gam55_unconv',Gam37='gam37_unconv',
    Econv35='gam35_conv', Econv55='gam55_conv', Econv37='gam37_conv'
    )
  for attrName, clName in clusterTypes.iteritems():
    x = 'ClusterCorrectionTools' + attrName
    if not hasattr(swTool, x) or getattr(swTool, x):
      continue
    y = make_CaloSwCorrections(clName, suffix='EGDF',
                               version = jobproperties.egammaRecFlags.clusterCorrectionVersion(),
                               cells_name='MinVarCellContainer')
    setattr(swTool, x, _process_tools (swTool, y) )   


##############
# egammaSwTool
##############
egammaSwTool = ToolFactory( egammaToolsConf.egammaSwTool,
                            name = 'DFEgammaSWTool',
                            postInit=[configureClusterCorrections])

##########################
# egammaSwToolWithNewCells
##########################
egammaSwToolWithNewCells = ToolFactory( egammaToolsConf.egammaSwTool,
                                        name = 'DFEgammaSWToolWithNewCells',
                                        postInit=[configureClusterCorrectionsWithNewCells])

#############################
# egammaSwToolWithMaxVarCells
#############################
egammaSwToolWithMaxVarCells = ToolFactory( egammaToolsConf.egammaSwTool,
                                        name = 'DFEgammaSWToolWithMaxVarCells',
                                        postInit=[configureClusterCorrectionsWithMaxVarCells])

#############################
# egammaSwToolWithMaxVarCells
#############################
egammaSwToolWithMinVarCells = ToolFactory( egammaToolsConf.egammaSwTool,
                                        name = 'DFEgammaSWToolWithMinVarCells',
                                        postInit=[configureClusterCorrectionsWithMinVarCells])

##################
# ClusterDecorator
##################
ClusterDecorator = ToolFactory( DerivationFramework__ClusterDecorator,
                                ClusterCorrectionToolName = FullNameWrapper(egammaSwTool),  
                                SGKey_electrons = egammaKeys.outputElectronKey(),
                                SGKey_photons = egammaKeys.outputPhotonKey(), 
                                OutputClusterSGKey="EGammaSwCluster",
                                OutputClusterLink="SwClusterLink",
                                SGKey_caloCells = egammaKeys.caloCellKey() )


ClusterDecoratorWithNewCells = ToolFactory( DerivationFramework__ClusterDecorator,
                                            ClusterCorrectionToolName = FullNameWrapper(egammaSwToolWithNewCells),  
                                            SGKey_electrons = egammaKeys.outputElectronKey(),
                                            SGKey_photons = egammaKeys.outputPhotonKey(), 
                                            OutputClusterSGKey="EGammaSwClusterWithNewCells",
                                            OutputClusterLink="NewSwClusterLink",
                                            SGKey_caloCells = 'NewCellContainer' )

ClusterDecoratorWithMaxVarCells = ToolFactory( DerivationFramework__ClusterDecorator,
                                               ClusterCorrectionToolName = FullNameWrapper(egammaSwToolWithMaxVarCells),  
                                               SGKey_electrons = egammaKeys.outputElectronKey(),
                                               SGKey_photons = egammaKeys.outputPhotonKey(), 
                                               OutputClusterSGKey="EGammaSwClusterWithMaxVarCells",
                                               OutputClusterLink="MaxVarSwClusterLink",
                                               SGKey_caloCells = 'MaxVarCellContainer' )

ClusterDecoratorWithMinVarCells = ToolFactory( DerivationFramework__ClusterDecorator,
                                               ClusterCorrectionToolName = FullNameWrapper(egammaSwToolWithMinVarCells),  
                                               SGKey_electrons = egammaKeys.outputElectronKey(),
                                               SGKey_photons = egammaKeys.outputPhotonKey(), 
                                               OutputClusterSGKey="EGammaSwClusterWithMinVarCells",
                                               OutputClusterLink="MinVarSwClusterLink",
                                               SGKey_caloCells = 'MinVarCellContainer' )
#######################
# getClusterDecorations
#######################
def getClusterDecorations( obj, 
  collections=[egammaKeys.outputElectronKey(), egammaKeys.outputPhotonKey()] ):
  """getSwClusterDecorations( obj, collections=["Electrons", "Photons"] ) -> 
  Return a list with the 'ExtraContent' to be added to the decorations to save the Sw cluster per object-layer"""
  return ["{part}".format(part=part) for part in collections ]


###################################
# theCaloCellContainerFinalizerTool
###################################
theCaloCellContainerFinalizerTool=  ToolFactory( CaloCellContainerFinalizerTool )


#############
# NewCellTool
# this is the tool that reweights the cell energies, using the nominal weights
#############
NewCellTool = ToolFactory( DerivationFramework__CellReweight,
                           ReweightCellContainerName = "NewCellContainer",
                           ReweightCoefficients2DPath = "DerivationFrameworkCalo/CellReweight_v2d/rewCoeffs10.root",
                           SGKey_electrons = egammaKeys.outputElectronKey(),
                           SGKey_photons = egammaKeys.outputPhotonKey(),
                           CaloFillRectangularTool711=CaloFillRectangularCluster.copy(name = "NewCaloFillRectangularCluster_%sx%s" % (7, 11), eta_size = 7, phi_size = 11),
                           CaloFillRectangularTool37=CaloFillRectangularCluster.copy(name = "NewCaloFillRectangularCluster_%sx%s" % (3, 7), eta_size = 3, phi_size = 7),
                           CaloFillRectangularTool55=CaloFillRectangularCluster.copy(name = "NewCaloFillRectangularCluster_%sx%s" % (5, 5), eta_size = 5, phi_size = 5),
                           CaloCellContainerFinalizerTool=theCaloCellContainerFinalizerTool
)

################
# MaxVarCellTool
# this is the tool that reweights the cell energies, using the maximum variations
################
MaxVarCellTool = ToolFactory( DerivationFramework__CellReweight,
                              ReweightCellContainerName = "MaxVarCellContainer",
                              ReweightCoefficients2DPath = "DerivationFrameworkCalo/CellReweight_v2d/rewCoeffs11.root",
                              SGKey_electrons = egammaKeys.outputElectronKey(),
                              SGKey_photons = egammaKeys.outputPhotonKey(),
                              CaloFillRectangularTool711=CaloFillRectangularCluster.copy(name = "MaxVarCaloFillRectangularCluster_%sx%s" % (7, 11), eta_size = 7, phi_size = 11),
                              CaloFillRectangularTool37=CaloFillRectangularCluster.copy(name = "MaxVarCaloFillRectangularCluster_%sx%s" % (3, 7), eta_size = 3, phi_size = 7),
                              CaloFillRectangularTool55=CaloFillRectangularCluster.copy(name = "MaxVarCaloFillRectangularCluster_%sx%s" % (5, 5), eta_size = 5, phi_size = 5),
                              CaloCellContainerFinalizerTool=theCaloCellContainerFinalizerTool
)

################
# MinVarCellTool
# this is the tool that reweights the cell energies, using the minimum variations
################
MinVarCellTool = ToolFactory( DerivationFramework__CellReweight,
                              ReweightCellContainerName = "MinVarCellContainer",
                              ReweightCoefficients2DPath = "DerivationFrameworkCalo/CellReweight_v2d/rewCoeffs00.root",
                              SGKey_electrons = egammaKeys.outputElectronKey(),
                              SGKey_photons = egammaKeys.outputPhotonKey(),
                              CaloFillRectangularTool711=CaloFillRectangularCluster.copy(name = "MinVarCaloFillRectangularCluster_%sx%s" % (7, 11), eta_size = 7, phi_size = 11),
                              CaloFillRectangularTool37=CaloFillRectangularCluster.copy(name = "MinVarCaloFillRectangularCluster_%sx%s" % (3, 7), eta_size = 3, phi_size = 7),
                              CaloFillRectangularTool55=CaloFillRectangularCluster.copy(name = "MinVarCaloFillRectangularCluster_%sx%s" % (5, 5), eta_size = 5, phi_size = 5),
                              CaloCellContainerFinalizerTool=theCaloCellContainerFinalizerTool
)

#################
# EMShowerBuilder
#################
EMNewShowerBuilderTool = EMShowerBuilder("EMNewShowerBuilderTool", CellsName="NewCellContainer")
EMMaxVarShowerBuilderTool = EMShowerBuilder("EMMaxVarShowerBuilderTool", CellsName="MaxVarCellContainer")
EMMinVarShowerBuilderTool = EMShowerBuilder("EMMinVarShowerBuilderTool", CellsName="MinVarCellContainer")


######################
# EGammaReweightTool
######################
EGammaReweightTool = ToolFactory( DerivationFramework__EGammaReweight,
                                  NewCellContainerName = "NewCellContainer",
                                  SGKey_photons="Photons",
                                  SGKey_electrons="Electrons",
                                  EMShowerBuilderTool = EMNewShowerBuilderTool,
                                  NewElectronContainer = "NewSwElectrons",
                                  NewPhotonContainer = "NewSwPhotons",
                                  CaloClusterLinkName="NewSwClusterLink",
                                  DecorateEGammaObjects = True,
                                  DecorationPrefix = "RW_",
                                  SaveReweightedContainer = True
)

EGammaMaxVarReweightTool = ToolFactory( DerivationFramework__EGammaReweight,
                                        NewCellContainerName = "MaxVarCellContainer",
                                        SGKey_photons="Photons",
                                        SGKey_electrons="Electrons",
                                        EMShowerBuilderTool = EMMaxVarShowerBuilderTool,
                                        NewElectronContainer = "MaxVarSwElectrons",
                                        NewPhotonContainer = "MaxVarSwPhotons",
                                        CaloClusterLinkName="MaxVarSwClusterLink",
                                        DecorateEGammaObjects = True, 
                                        DecorationPrefix = "Max_",
                                        SaveReweightedContainer = False
)

EGammaMinVarReweightTool = ToolFactory( DerivationFramework__EGammaReweight,
                                        NewCellContainerName = "MinVarCellContainer",
                                        SGKey_photons="Photons",
                                        SGKey_electrons="Electrons",
                                        EMShowerBuilderTool = EMMinVarShowerBuilderTool,
                                        NewElectronContainer = "MinVarSwElectrons",
                                        NewPhotonContainer = "MinVarSwPhotons",
                                        CaloClusterLinkName="MinVarSwClusterLink", 
                                        DecorateEGammaObjects = True,
                                        DecorationPrefix = "Min_",
                                        SaveReweightedContainer = False
)
