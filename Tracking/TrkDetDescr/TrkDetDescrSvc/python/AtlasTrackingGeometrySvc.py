# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

##################################################################################
# The AtlasTrackingGeometry Svc fragment
#
# usage: 
#   include('TrkDetDescrSvc/AtlasTrackingGeometrySvc.py')
#
#    + for the navigator, set the TrackingGeometryName to:
#      AtlasTrackingGeometrySvc.trackingGeometryName()
#
##################################################################################

from __future__ import print_function

# import the DetFlags for the setting
from AthenaCommon.DetFlags import DetFlags

#################################################################################
# Material for the Geometry comes from COOL or local database
#################################################################################

from TrkDetDescrSvc.TrkDetDescrJobProperties import TrkDetFlags


# import the Extrapolator configurable
from TrkDetDescrSvc.TrkDetDescrSvcConf import Trk__TrackingGeometrySvc
   
class ConfiguredTrackingGeometrySvc( Trk__TrackingGeometrySvc ) :   

    # constructor
    def __init__(self,name = 'AtlasTrackingGeometrySvc'):
        
        if TrkDetFlags.ConfigurationOutputLevel() < 3 :
          print ('[ Configuration : start ] *** '+name+' ********************************')
          print ('[ TrackingGeometrySvc ]')
        
        from AthenaCommon.AppMgr import ToolSvc 

        #################################################################################
        # The Geometry Builder
        #################################################################################
        
        # the name to register the Geometry
        AtlasTrackingGeometryName = 'AtlasTrackingGeometry'

        # the geometry builder alg tool
        from TrkDetDescrTools.TrkDetDescrToolsConf import Trk__GeometryBuilder
        AtlasGeometryBuilder = Trk__GeometryBuilder(
            name='AtlasGeometryBuilder')
        # switch the building outputlevel on
        AtlasGeometryBuilder.OutputLevel = TrkDetFlags.ConfigurationOutputLevel()

        # the envelope definition service
        from AthenaCommon.CfgGetter import getService
        AtlasEnvelopeSvc = getService('AtlasGeometry_EnvelopeDefSvc')
           
        # (ID) 
        if DetFlags.ID_on() :
          # get hand on the ID Tracking Geometry Builder
          if TrkDetFlags.ISF_FatrasCustomGeometry() :
              if hasattr(ToolSvc, TrkDetFlags.ISF_FatrasCustomGeometryBuilderName()):
                  InDetTrackingGeometryBuilder = getattr(ToolSvc, TrkDetFlags.ISF_FatrasCustomGeometryBuilderName())
          elif TrkDetFlags.XMLFastCustomGeometry() :
              if hasattr(ToolSvc, TrkDetFlags.InDetTrackingGeometryBuilderName()):
                  InDetTrackingGeometryBuilder = getattr(ToolSvc, TrkDetFlags.InDetTrackingGeometryBuilderName())
          else:
              if not TrkDetFlags.InDetStagedGeometryBuilder():
                  from InDetTrackingGeometry.ConfiguredInDetTrackingGeometryBuilder import ConfiguredInDetTrackingGeometryBuilder as IDGeometryBuilder
              else:
                  from InDetTrackingGeometry.ConfiguredStagedTrackingGeometryBuilder import ConfiguredStagedTrackingGeometryBuilder as IDGeometryBuilder
              InDetTrackingGeometryBuilder = IDGeometryBuilder(name ='InDetTrackingGeometryBuilder')
                
          InDetTrackingGeometryBuilder.EnvelopeDefinitionSvc = AtlasEnvelopeSvc
          InDetTrackingGeometryBuilder.OutputLevel = TrkDetFlags.InDetBuildingOutputLevel()
          # and give it to the Geometry Builder
          AtlasGeometryBuilder.InDetTrackingGeometryBuilder = InDetTrackingGeometryBuilder
          # 
        # (Calo)
        if DetFlags.Calo_on() :
           from TrkDetDescrTools.TrkDetDescrToolsConf import Trk__CylinderVolumeCreator
           CaloVolumeCreator = Trk__CylinderVolumeCreator("CaloVolumeCreator")
           CaloVolumeCreator.OutputLevel = TrkDetFlags.CaloBuildingOutputLevel()
           ToolSvc += CaloVolumeCreator

           from CaloTrackingGeometry.ConfiguredCaloTrackingGeometryBuilder import ConfiguredCaloTrackingGeometryBuilder as ConfiguredCaloGeo 
           CaloTrackingGeometryBuilder = ConfiguredCaloGeo(name='CaloTrackingGeometryBuilder')
           CaloTrackingGeometryBuilder.TrackingVolumeCreator = CaloVolumeCreator
           CaloTrackingGeometryBuilder.EnvelopeDefinitionSvc = AtlasEnvelopeSvc
           CaloTrackingGeometryBuilder.OutputLevel           = TrkDetFlags.CaloBuildingOutputLevel()
           CaloTrackingGeometryBuilder.GeometryName          = 'Calo'
           # and give it to the Geometry Builder
           AtlasGeometryBuilder.CaloTrackingGeometryBuilder = CaloTrackingGeometryBuilder
        
        # (Muon)
        if DetFlags.Muon_on() :
           from MuonTrackingGeometry.ConfiguredMuonTrackingGeometry import MuonTrackingGeometryBuilder
           MuonTrackingGeometryBuilder.EnvelopeDefinitionSvc = AtlasEnvelopeSvc
           # and give it to the Geometry Builder
           AtlasGeometryBuilder.MuonTrackingGeometryBuilder = MuonTrackingGeometryBuilder      
           
        # processors
        AtlasGeometryProcessors = []   
           
        # check whether the material retrieval is ment to be from COOL
        if TrkDetFlags.MaterialSource() == 'COOL':
            # the material provider
            from TrkDetDescrTools.TrkDetDescrToolsConf import Trk__LayerMaterialProvider as LayerMaterialProvider
            AtlasMaterialProvider = LayerMaterialProvider('AtlasMaterialProvider')
            AtlasMaterialProvider.OutputLevel           = TrkDetFlags.ConfigurationOutputLevel()
            AtlasMaterialProvider.LayerMaterialMapName  = TrkDetFlags.MaterialStoreGateKey()
            AtlasMaterialProvider.LayerMaterialMapKey   = ''
        
            AtlasGeometryProcessors += [ AtlasMaterialProvider ]
        
            # the tag names
            CoolDataBaseFolder = TrkDetFlags.MaterialStoreGateKey()
            AtlasMaterialTag = TrkDetFlags.MaterialTagBase()+str(TrkDetFlags.MaterialVersion())+TrkDetFlags.MaterialSubVersion()+'_'
        
            if TrkDetFlags.ConfigurationOutputLevel() < 3 :
               print ('[ TrackingGeometrySvc ] Associating DB folder : ', CoolDataBaseFolder)
        
            # we need the conditions interface
            from IOVDbSvc.CondDB import conddb
            # use a local database
            if TrkDetFlags.MaterialDatabaseLocal():
                # specify the local database
                DataBasePath  = TrkDetFlags.MaterialDatabaseLocalPath() 
                DataBaseName  = TrkDetFlags.MaterialDatabaseLocalName()
                MagicTag      = TrkDetFlags.MaterialMagicTag()
                DataBaseConnection = '<dbConnection>sqlite://X;schema='+DataBasePath+DataBaseName+';dbname=OFLP200</dbConnection>'
                conddb.blockFolder('/GLOBAL/TrackingGeo/LayerMaterialV2')
                conddb.addFolderWithTag('',DataBaseConnection+CoolDataBaseFolder,AtlasMaterialTag+MagicTag,force=True)
                if TrkDetFlags.ConfigurationOutputLevel() < 3 :
                    print ('[ TrackingGeometrySvc ] Using Local Database: '+DataBaseConnection        )
                # make sure that the pool files are in the catalog
            else :
                print ('[ TrackingGeometrySvc ]     base material tag : ', AtlasMaterialTag)
                cfolder = CoolDataBaseFolder +'<tag>TagInfoMajor/'+AtlasMaterialTag+'/GeoAtlas</tag>'
                print ('[ TrackingGeometrySvc ]     translated to COOL: ', cfolder)
                # load the right folders (preparation for calo inclusion)
                conddb.addFolderSplitMC('GLOBAL',cfolder,cfolder)

        elif TrkDetFlags.MaterialSource() == 'Input' :
            # the material provider
            from TrkDetDescrTools.TrkDetDescrToolsConf import Trk__InputLayerMaterialProvider
            AtlasMaterialProvider = Trk__InputLayerMaterialProvider('AtlasMaterialProvider')
            AtlasMaterialProvider.OutputLevel           = TrkDetFlags.ConfigurationOutputLevel()
            AtlasGeometryProcessors += [ AtlasMaterialProvider ]            

        # material validation
        if TrkDetFlags.MaterialValidation() :
            # load the material inspector
            from TrkDetDescrTestTools.TrkDetDescrTestToolsConf import Trk__LayerMaterialInspector
            AtlasLayerMaterialInspector = Trk__LayerMaterialInspector('AtlasLayerMaterialInspector')
            AtlasLayerMaterialInspector.OutputLevel = TrkDetFlags.ConfigurationOutputLevel()
            
            AtlasGeometryProcessors += [ AtlasLayerMaterialInspector ]

        # call the base class constructor : sets the tools
        Trk__TrackingGeometrySvc.__init__(self,name,
                                          GeometryBuilder = AtlasGeometryBuilder,
                                          TrackingGeometryName = AtlasTrackingGeometryName,
                                          GeometryProcessors = AtlasGeometryProcessors,
                                          BuildGeometryFromTagInfo = True,
                                          OutputLevel = TrkDetFlags.ConfigurationOutputLevel())
        
        # screen output of the configuration
        if TrkDetFlags.ConfigurationOutputLevel() < 3 :
           print (self)
           print ('* [ GeometryBuilder       ]')
           print (AtlasGeometryBuilder)
           print ('* [ Configuration : end   ] ***'+name+'********************************')
        
##################################################################################    

# now create the instance
AtlasTrackingGeometrySvc = ConfiguredTrackingGeometrySvc('AtlasTrackingGeometrySvc')
# add it to the ServiceManager
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
svcMgr += AtlasTrackingGeometrySvc
