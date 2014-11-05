# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
BarcodeServices configurations
Elmar Ritsch, 23/10/2014
"""

from AthenaCommon.CfgGetter import getPrivateTool,getPrivateToolClone,getPublicTool,getPublicToolClone,\
        getService,getServiceClone,getAlgorithm,getAlgorithmClone

from AthenaCommon.Constants import *  # FATAL,ERROR etc.
from AthenaCommon.SystemOfUnits import *
from AthenaCommon.DetFlags import DetFlags

def getMC15BarcodeSvc(name="Barcode_MC15BarcodeSvc", **kwargs):
    kwargs.setdefault("FirstVertexBarcode"            ,  -1000001 )
    kwargs.setdefault("VertexIncrement"               ,        -1 )
    kwargs.setdefault("FirstSecondaryBarcode"         ,   1000001 )
    kwargs.setdefault("SecondaryIncrement"            ,         1 )
    kwargs.setdefault("ParticleRegenerationIncrement" ,  10000000 )
    kwargs.setdefault("DoUnderAndOverflowChecks"      ,      True )
    kwargs.setdefault("EncodePhysicsProcessInVertexBC",     False )
    #
    from BarcodeServices.BarcodeServicesConf import Barcode__GenericBarcodeSvc
    return Barcode__GenericBarcodeSvc(name, **kwargs)

def getMC12BarcodeSvc(name="Barcode_MC12BarcodeSvc", **kwargs):
    kwargs.setdefault("FirstVertexBarcode"          , -200001)
    kwargs.setdefault("VertexIncrement"             , -1)
    kwargs.setdefault("FirstSecondaryBarcode"       , 200001)
    kwargs.setdefault("SecondaryIncrement"          , 1)
    kwargs.setdefault("ParticleGenerationIncrement" , 1000000)
    #
    from BarcodeServices.BarcodeServicesConf import Barcode__LegacyBarcodeSvc
    return Barcode__LegacyBarcodeSvc(name, **kwargs)

def getPhysicsProcessBarcodeSvc(name="Barcode_PhysicsProcessBarcodeSvc", **kwargs):
    kwargs.setdefault("EncodePhysicsProcessInVertexBC",  False  )
    kwargs.setdefault("FirstVertexBarcode"            , -200000 )
    kwargs.setdefault("VertexIncrement"               , -1000000)
    kwargs.setdefault("FirstSecondaryBarcode"         ,  200001 )
    kwargs.setdefault("SecondaryIncrement"            ,  1      )
    kwargs.setdefault("EncodePhysicsProcessInVertexBC",  True   )
    #
    getMC15BarcodeSvc(name, **kwargs)

def getGlobalBarcodeSvc(name="Barcode_GlobalBarcodeSvc", **kwargs):
    kwargs.setdefault("FirstVertexBarcode"            ,  -200000    );
    kwargs.setdefault("VertexIncrement"               ,  -1000000   );
    kwargs.setdefault("FirstSecondaryBarcode"         ,   200001    );
    kwargs.setdefault("SecondaryIncrement"            ,   1         );
    kwargs.setdefault("DoUnderAndOverflowChecks"      ,   True      );
    kwargs.setdefault("EncodePhysicsProcessInVertexBC",   True      );
    #
    from BarcodeServices.BarcodeServicesConf import Barcode__GlobalBarcodeSvc
    return Barcode__GlobalBarcodeSvc(name, **kwargs)


def getValidationBarcodeSvc(name="Barcode_ValidationBarcodeSvc", **kwargs):
    kwargs.setdefault("FirstVertexBarcode"          , -200001)
    kwargs.setdefault("VertexIncrement"             , -1)
    kwargs.setdefault("FirstSecondaryBarcode"       , 200001)
    kwargs.setdefault("SecondaryIncrement"          , 1)
    kwargs.setdefault("ParticleGenerationIncrement" , 1000000)
    kwargs.setdefault("DoUnderAndOverflowChecks"    , True)
    #
    from BarcodeServices.BarcodeServicesConf import Barcode__ValidationBarcodeSvc
    return Barcode__ValidationBarcodeSvc(name, **kwargs)

