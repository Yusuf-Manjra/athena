#+++++++++++++++++ Beginning of WriteInDetAOD.py

from InDetRecExample.InDetKeys import InDetKeys
InDetAODList = []

# Add Vertices and Particles.
# -----------------------------

if InDetFlags.doxAOD():

  excludedAuxData = "-caloExtension.-cellAssociation.-clusterAssociation.-TTVA_AMVFVertices_forReco.-TTVA_AMVFWeights_forReco"

  if not (InDetFlags.KeepFirstParameters() or InDetFlags.keepAdditionalHitsOnTrackParticle()):
    excludedAuxData += '.-trackParameterCovarianceMatrices.-parameterX.-parameterY.-parameterZ.-parameterPX.-parameterPY.-parameterPZ.-parameterPosition'

  excludedVertexAuxData = "-vxTrackAtVertex.-MvfFitInfo.-isInitialized.-VTAV"

  # remove track decorations used internally by FTAG software
  excludedAuxData += ".-TrackCompatibility.-VxTrackAtVertex.-btagIp_d0Uncertainty.-btagIp_z0SinThetaUncertainty.-btagIp_z0SinTheta.-btagIp_d0.-btagIp_trackMomentum.-btagIp_trackDisplacement"

  InDetAODList+=['xAOD::TrackParticleContainer#'+InDetKeys.xAODTrackParticleContainer()]
  InDetAODList+=['xAOD::TrackParticleAuxContainer#'+InDetKeys.xAODTrackParticleContainer()+'Aux.' + excludedAuxData]
  from  InDetPhysValMonitoring.InDetPhysValJobProperties import InDetPhysValFlags
  from  InDetPhysValMonitoring.ConfigUtils import extractCollectionPrefix
  for col in InDetPhysValFlags.validateExtraTrackCollections() :
    prefix=extractCollectionPrefix(col)
    InDetAODList+=['xAOD::TrackParticleContainer#'+prefix+'TrackParticles']
    InDetAODList+=['xAOD::TrackParticleAuxContainer#'+prefix+'TrackParticlesAux.' + excludedAuxData]
  if not InDetFlags.doSLHC():
   InDetAODList+=['xAOD::TrackParticleContainer#'+InDetKeys.xAODForwardTrackParticleContainer()]
   InDetAODList+=['xAOD::TrackParticleAuxContainer#'+InDetKeys.xAODForwardTrackParticleContainer()+'Aux.' + excludedAuxData]
  InDetAODList+=['xAOD::VertexContainer#'+InDetKeys.xAODVertexContainer()]
  InDetAODList+=['xAOD::VertexAuxContainer#'+InDetKeys.xAODVertexContainer()+'Aux.' + excludedVertexAuxData]
  InDetAODList+=['xAOD::VertexContainer#'+InDetKeys.xAODV0VertexContainer()]
  InDetAODList+=['xAOD::VertexAuxContainer#'+InDetKeys.xAODV0VertexContainer()+'Aux.' + excludedVertexAuxData]
  InDetAODList+=['xAOD::VertexContainer#'+InDetKeys.xAODKshortVertexContainer()]
  InDetAODList+=['xAOD::VertexAuxContainer#'+InDetKeys.xAODKshortVertexContainer()+'Aux.' + excludedVertexAuxData]
  InDetAODList+=['xAOD::VertexContainer#'+InDetKeys.xAODLambdaVertexContainer()]
  InDetAODList+=['xAOD::VertexAuxContainer#'+InDetKeys.xAODLambdaVertexContainer()+'Aux.' + excludedVertexAuxData]
  InDetAODList+=['xAOD::VertexContainer#'+InDetKeys.xAODLambdabarVertexContainer()]
  InDetAODList+=['xAOD::VertexAuxContainer#'+InDetKeys.xAODLambdabarVertexContainer()+'Aux.' + excludedVertexAuxData]
  InDetAODList+=['xAOD::VertexContainer#'+InDetKeys.Conversions()]
  InDetAODList+=['xAOD::VertexAuxContainer#'+InDetKeys.Conversions() +'Aux.' + excludedVertexAuxData]
  # save a separate LRT container anyway. Currently it does not create problems if the container does not exist.
  InDetAODList+=['xAOD::TrackParticleContainer#'+InDetKeys.xAODLargeD0TrackParticleContainer()]
  InDetAODList+=['xAOD::TrackParticleAuxContainer#'+InDetKeys.xAODLargeD0TrackParticleContainer()+'Aux.' + excludedAuxData]
  if InDetFlags.doTrackSegmentsPixel():
    InDetAODList+=['xAOD::TrackParticleContainer#'+InDetKeys.xAODPixelTrackParticleContainer()]
    InDetAODList+=['xAOD::TrackParticleAuxContainer#'+InDetKeys.xAODPixelTrackParticleContainer()+'Aux.' + excludedAuxData]
  if InDetFlags.doTrackSegmentsDisappearing():
    InDetAODList+=['xAOD::TrackParticleContainer#'+InDetKeys.xAODDisappearingTrackParticleContainer()]
    InDetAODList+=['xAOD::TrackParticleAuxContainer#'+InDetKeys.xAODDisappearingTrackParticleContainer()+'Aux.' + excludedAuxData]
  if InDetFlags.doTrackSegmentsSCT():
    InDetAODList+=['xAOD::TrackParticleContainer#'+InDetKeys.xAODSCTTrackParticleContainer()]
    InDetAODList+=['xAOD::TrackParticleAuxContainer#'+InDetKeys.xAODSCTTrackParticleContainer()+'Aux.' + excludedAuxData]
  if InDetFlags.doTrackSegmentsTRT():
    InDetAODList+=['xAOD::TrackParticleContainer#'+InDetKeys.xAODTRTTrackParticleContainer()]
    InDetAODList+=['xAOD::TrackParticleAuxContainer#'+InDetKeys.xAODTRTTrackParticleContainer()+'Aux.' + excludedAuxData]
  if InDetFlags.doDBMstandalone() or InDetFlags.doDBM(): 
    InDetAODList+=['xAOD::TrackParticleContainer#'+InDetKeys.xAODDBMTrackParticleContainer()] 
    InDetAODList+=['xAOD::TrackParticleAuxContainer#'+InDetKeys.xAODDBMTrackParticleContainer()+'Aux.' + excludedAuxData] 
    InDetAODList+=["TrackCollection#"+InDetKeys.DBMTracks()] 
    if InDetFlags.doTruth(): 
      InDetAODList += ["TrackTruthCollection#"+InDetKeys.DBMTracks()+'TruthCollection'] 
      InDetAODList += ["DetailedTrackTruthCollection#"+InDetKeys.DBMTracks()+'DetailedTruth'] 

# next is only for InDetRecExample stand alone! RecExCommon uses InDetAODList directly
StreamAOD.ItemList += InDetAODList 
#+++++++++++++++++ End of WriteInDetAOD.py
