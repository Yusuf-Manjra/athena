
# +++++++++++++++++++ beginning of InDetRec_jobOptions.py
# jobOptions Fragment for ID software
# -----------------------------------
# edward.moyse@cern.ch
# markus.elsing@cern.ch
# -----------------------------------

from AthenaCommon.DetFlags import DetFlags
from AthenaCommon.BeamFlags import jobproperties
from AtlasGeoModel.InDetGMJobProperties import GeometryFlags as geoFlags


# check (for robustness) if ID is on at all!
if not DetFlags.detdescr.ID_on():
  print "InDetRec_jobOptions.py: DetFlags ID is turned off. Not including anything!"
else:
  # +++++++++++++++++++++++
  # Control 
  # +++++++++++++++++++++++
  #
  from InDetRecExample.InDetJobProperties import InDetFlags
  InDetFlags.init()
  #
  if InDetFlags.Enabled():

    from AthenaCommon.GlobalFlags import globalflags
    # rec flags are needed (e.g. for commissioning steering ...)
    from RecExConfig.RecFlags import rec

    # --- ensure that Calo clustering is running if we run in calo seeded mode 
    if InDetFlags.doBremRecovery() and InDetFlags.doCaloSeededBrem() and not DetFlags.detdescr.Calo_allOn():
      print "*************************************************************"
      print "* Major FAILURE : InDet reconstruction in seeded brem mode, *"
      print "*                 but LAr is off                            *"
      print "*************************************************************"
      # raise RuntimeError,"Conflict in InDetRec setup: seeded Brem recovery, but Calo is off"
      if InDetFlags.doBremRecovery.is_locked():
        InDetFlags.doBremRecovery.unlock()
      print " ------------> WARNING: turning off brem recovery"
      InDetFlags.doBremRecovery.set_Value_and_Lock(False)
      # --- ditto for calo seeded TRT segments
    if InDetFlags.doCaloSeededTRTSegments() and not DetFlags.detdescr.Calo_allOn():
      print "******************************************************"
      print "* Major FAILURE : Back tracking in Calo seeded mode, *"
      print "*                 but Calo is off                    *"
      print "******************************************************"
      if InDetFlags.doCaloSeededTRTSegments.is_locked():
        InDetFlags.doCaloSeededTRTSegments.unlock()
      print " ------------> WARNING: Using Standard Back Tracking (not calo seeded)"
      InDetFlags.doCaloSeededTRTSegments.set_Value_and_Lock(False)

    
    # --- print setup
    InDetFlags.printInfo()
    InDetFlags.print_JobProperties()    
    
    #
    # ----------- import, lock and print InDetKeys
    #
    from InDetRecExample.InDetKeys import InDetKeys
    InDetKeys.lockAllExceptAlias()
    print "Printing InDetKeys"
    InDetKeys.print_JobProperties()
    
    #
    # --- setup of cut values for NewTracking  
    #
    if (not 'InDetNewTrackingCuts' in dir()):
      print "InDetRec_jobOptions: InDetNewTrackingCuts not set before - import them now"
      from InDetRecExample.ConfiguredNewTrackingCuts import ConfiguredNewTrackingCuts
      if InDetFlags.doDBM():
        InDetNewTrackingCuts      = ConfiguredNewTrackingCuts("DBM")
      elif InDetFlags.doVtxLumi():
        InDetNewTrackingCuts      = ConfiguredNewTrackingCuts("VtxLumi")
      elif InDetFlags.doCosmics():
        InDetNewTrackingCuts      = ConfiguredNewTrackingCuts("Cosmics")
      elif InDetFlags.doHeavyIon():
        InDetNewTrackingCuts      = ConfiguredNewTrackingCuts("HeavyIon")
      elif InDetFlags.doSLHC():
        InDetNewTrackingCuts      = ConfiguredNewTrackingCuts("SLHC")
      elif InDetFlags.doIBL():
        InDetNewTrackingCuts      = ConfiguredNewTrackingCuts("IBL")
      elif InDetFlags.doHighPileup():
        InDetNewTrackingCuts      = ConfiguredNewTrackingCuts("HighPileup")
      elif InDetFlags.doMinBias():
        InDetNewTrackingCuts      = ConfiguredNewTrackingCuts("MinBias")        
      else:
        InDetNewTrackingCuts      = ConfiguredNewTrackingCuts("Offline")
    InDetNewTrackingCuts.printInfo()
    # --- backward compatible
    InDetCutValues = InDetNewTrackingCuts

    #
    # --- setup of cut values for  Primary vertexing  
    #
    if (not 'InDetPrimaryVertexingCuts' in dir()):
      print "InDetRec_jobOptions: InDetPrimaryVertexingCuts not set before - import them now"
      from InDetRecExample.ConfiguredVertexingCuts import ConfiguredVertexingCuts
      InDetPrimaryVertexingCuts      = ConfiguredVertexingCuts(InDetFlags.primaryVertexCutSetup())
      InDetPrimaryVertexingCuts.printInfo()

    # ----------- 
    #
    # --- setup key aliases to pass to configurables
    #
    InDetSpSeededTracksKey    = InDetKeys.SiSpSeededTracks()
    InDetResolvedTracksKey    = InDetKeys.ResolvedTracks() 
    InDetExtendedTracksKey    = InDetKeys.ExtendedTracks()
    InDetExtendedTracksMapKey = InDetKeys.ExtendedTracksMap()  
    
    if InDetFlags.doSLHC():
      InDetSpSeededTracksKey    = InDetKeys.SiSpSeededSLHCTracks() 
      InDetResolvedTracksKey    = InDetKeys.ResolvedSLHCTracks() 
      InDetExtendedTracksKey    = InDetKeys.ExtendedSLHCTracks()
      InDetExtendedTracksMapKey = InDetKeys.ExtendedTracksMapSLHC()              
      
    if globalflags.InputFormat() == 'bytestream':
      ServiceMgr.ByteStreamCnvSvc.IsSimulation = (globalflags.DataSource() == 'geant4')
              
    
    # ------------------------------------------------------------
    #
    # ----------- special case for Calo seeded brem recovery
    #
    # ------------------------------------------------------------
    if InDetFlags.doBremRecovery() and InDetFlags.doCaloSeededBrem() and DetFlags.detdescr.Calo_allOn():
      include ("InDetRecExample/InDetRecCaloSeededROISelection.py")

    # ------------------------------------------------------------
    # 
    # -----------ROI seeding for SSS seeds
    #
    # ------------------------------------------------------------
    #
    if InDetFlags.doHadCaloSeededSSS() and DetFlags.detdescr.Calo_allOn():
      include ("InDetRecExample/InDetRecHadCaloSeededROISelection.py")

    # ------------------------------------------------------------
    # 
    # ----------- Configuring the conditions access
    #
    # ------------------------------------------------------------
    
    include ("InDetRecExample/InDetRecConditionsAccess.py")

    # ------------------------------------------------------------
    # 
    # ----------- Loading the Tracking Tools and Services 
    #
    # ------------------------------------------------------------
    
    include ("InDetRecExample/InDetRecLoadTools.py")

    # ------------------------------------------------------------
    #
    # ----------- Data-Preparation stage
    #
    # ------------------------------------------------------------
    # --- silicon
    include ("InDetRecExample/InDetRecPreProcessingSilicon.py")

    # --- TRT, no drift information if cosmics, do not use extrenal phase in any case
    if not InDetFlags.doDBM():
      include ("InDetRecExample/ConfiguredInDetPreProcessingTRT.py")
      InDetPreProcessingTRT = ConfiguredInDetPreProcessingTRT(not InDetFlags.doTRTPhaseCalculation() or jobproperties.Beam.beamType()=="collisions",False)

    # ------------------------------------------------------------
    #
    # --- some collection lists (bookkeeping)
    #
    # ------------------------------------------------------------

    # track collection keys for statistics
    if not 'TrackCollectionKeys' in dir():
      TrackCollectionKeys        = []
    if not 'TrackCollectionTruthKeys' in dir():
      TrackCollectionTruthKeys   = []

    # NewTracking collection keys
    InputCombinedInDetTracks = []
 
    # ------------------------------------------------------------
    #
    # ----------- Subdetector pattern from New Tracking
    #
    # ------------------------------------------------------------
    #
    # --- Pixel track segment finding
    #
    if InDetFlags.doTrackSegmentsPixel():
      # --- load cuts for pixel segment finding
      if (not 'InDetNewTrackingCutsPixel' in dir()):
        print "InDetRec_jobOptions: InDetNewTrackingCutsPixel not set before - import them now"
        from InDetRecExample.ConfiguredNewTrackingCuts import ConfiguredNewTrackingCuts
        InDetNewTrackingCutsPixel = ConfiguredNewTrackingCuts("Pixel")
      InDetNewTrackingCutsPixel.printInfo()
      # --- configure pixel segment finding
      include ("InDetRecExample/ConfiguredNewTrackingSiPattern.py")
      InDetNewTrackingSiPattern = ConfiguredNewTrackingSiPattern([],InDetKeys.PixelTracks(),
                                                                 InDetKeys.SiSpSeededPixelTracks(),
                                                                 InDetNewTrackingCutsPixel,
                                                                 TrackCollectionKeys,
                                                                 TrackCollectionTruthKeys)

      if InDetFlags.doTruth():
        PixelTracksTruth = ConfiguredInDetTrackTruth(InDetKeys.PixelTracks(),
                                                     InDetKeys.PixelDetailedTracksTruth(),
                                                     InDetKeys.PixelTracksTruth())

    #
    # --- SCT track segment finding
    #
    if InDetFlags.doTrackSegmentsSCT():
      # --- load cuts for SCT segment finding
      if (not 'InDetNewTrackingCutsSCT' in dir()):
        print "InDetRec_jobOptions: InDetNewTrackingCutsSCT not set before - import them now"
        from InDetRecExample.ConfiguredNewTrackingCuts import ConfiguredNewTrackingCuts
        InDetNewTrackingCutsSCT = ConfiguredNewTrackingCuts("SCT")
      InDetNewTrackingCutsSCT.printInfo()
      # --- configure pixel segment finding
      include ("InDetRecExample/ConfiguredNewTrackingSiPattern.py")
      InDetNewTrackingSiPattern = ConfiguredNewTrackingSiPattern([],InDetKeys.SCTTracks(),
                                                                 InDetKeys.SiSpSeededSCTTracks(),
                                                                 InDetNewTrackingCutsSCT,
                                                                 TrackCollectionKeys,
                                                                 TrackCollectionTruthKeys)

   
    # ------------------------------------------------------------
    #
    # ----------- Cosmics Si pattern before we do the TRT phase
    #
    # ------------------------------------------------------------
    #
    # In case of cosmics TRT needs to have the drifttime adjusted for the phase
    #
    if InDetFlags.doTRTPhaseCalculation() and not jobproperties.Beam.beamType()=="collisions":
      #
      # We need to run the silicon tracking already at this stage together with 
      # the TRT tracking on uncalibrated PRDs to be able to calculate the phase

      # input collection for TRT Phase
      InDetCosmicSiTrackCollection = ""
      
      if InDetFlags.doNewTracking():
        #
        # --- run NewTracking Si pattern
        #
        include ("InDetRecExample/ConfiguredNewTrackingSiPattern.py")
        InDetNewTrackingSiPattern = ConfiguredNewTrackingSiPattern([], InDetResolvedTracksKey,
                                                                   InDetSpSeededTracksKey,
                                                                   InDetNewTrackingCuts,
                                                                   TrackCollectionKeys,
                                                                   TrackCollectionTruthKeys)
        # set output track collection
        InDetCosmicSiTrackCollection = InDetNewTrackingSiPattern.SiTrackCollection()

      # --- we only run the TRT phase, if the TRT is on !
      #
      if InDetFlags.doPRDFormation() and DetFlags.makeRIO.TRT_on():


        include ("InDetRecExample/ConfiguredNewTrackingTRTExtension.py")
        InDetPhaseTRTExtension = ConfiguredNewTrackingTRTExtension(InDetNewTrackingCuts,
                                                                   InDetNewTrackingSiPattern.SiTrackCollection(),
                                                                   InDetKeys.ExtendedTracksPhase(),
                                                                   InDetKeys.ExtendedTracksMapPhase(),
                                                                   TrackCollectionKeys,
                                                                   TrackCollectionTruthKeys,
                                                                   True)


        include ("InDetRecExample/ConfiguredTRTSegmentFinding.py")
        InDetPhaseTRTSegementFinding = ConfiguredTRTSegmentFinding("",
                                                                   InputCombinedInDetTracks,
                                                                   InDetNewTrackingCuts,
                                                                   InDetKeys.TRT_Segments_Phase(),
                                                                   None,
                                                                   True)

        # --- making tacks out of segments	      
        #include ("InDetRecExample/ConfiguredTRTStandalone.py")
        #InDetRecPhaseTRTStandalone = ConfiguredTRTStandalone ("", [],
        #                                                 InDetNewTrackingCuts,
        #                                                 InDetKeys.TRT_Segments_Phase(),
        #                        #                        InDetKeys.TRT_SegmentsTRT_EC(),
        #                                                 TrackCollectionKeys,
        #                                                 TrackCollectionTruthKeys)


        from InDetTrackPRD_Association.InDetTrackPRD_AssociationConf import InDet__InDetTrackPRD_Association
        InDetTRTonly_PRD_AssociationPhase = InDet__InDetTrackPRD_Association(name            = 'InDetTRTonly_PRD_AssociationPhase',
                                                                        AssociationTool = InDetPrdAssociationTool,
                                                                        TracksName      = TrackCollectionKeys) 
        topSequence += InDetTRTonly_PRD_AssociationPhase
        if (InDetFlags.doPrintConfigurables()):
          print InDetTRTonly_PRD_AssociationPhase


        from TRT_SegmentsToTrack.TRT_SegmentsToTrackConf import InDet__TRT_SegmentsToTrack
        InDetTrkSegmenttoTrkPhase = InDet__TRT_SegmentsToTrack(name                      = "InDetTRT_SegmentsToTrack_BarrelPhase",
                                                        InputSegmentsCollection   = InDetKeys.TRT_Segments_Phase(),
                                                        OutputTrackCollection     = InDetKeys.TRT_Tracks_Phase(),
                                                        TrackFitter               = InDetTrackFitter,
                                                        MinNHit                   = InDetNewTrackingCuts.minTRTonly(),
                                                        CombineTracks             = False,
                                                        OutputCombiCollection     = "",
                                                        InputSCTCollection        = "",
                                                        OutlierRemoval            = True,
                                                        MaterialEffects           = False)
        topSequence += InDetTrkSegmenttoTrkPhase
        if InDetFlags.doPrintConfigurables():
          print InDetTrkSegmenttoTrkPhase


        include ("InDetRecExample/ConfiguredInDetTRTPhase.py")
        InDetRecTRTPhase = ConfiguredInDetTRTPhase ([ InDetKeys.TRT_Tracks_Phase(),
                                                      InDetPhaseTRTExtension.ForwardTrackCollection() ])

        # --- now rerun the preprocessing of the TRT with external phase
        #
        include ("InDetRecExample/ConfiguredInDetPreProcessingTRT.py")
        InDetPreProcessingTRT = ConfiguredInDetPreProcessingTRT(True, True)

    # ------------------------------------------------------------
    #
    # ----------- now we do legacy pattern if requested
    #
    # ------------------------------------------------------------
    
    if InDetFlags.doxKalman() or InDetFlags.doiPatRec():
      include ("InDetRecExample/InDetRecXKalIPat.py")
    
    #
    # --- TRT track segment finding
    #
    if InDetFlags.doTrackSegmentsTRT():
      # --- load cuts for TRT segment finding
      if (not 'InDetNewTrackingCutsTRT' in dir()):
        print "InDetRec_jobOptions: InDetNewTrackingCutsTRT not set before - import them now"
        from InDetRecExample.ConfiguredNewTrackingCuts import ConfiguredNewTrackingCuts
        InDetNewTrackingCutsTRT = ConfiguredNewTrackingCuts("TRT")
      InDetNewTrackingCutsTRT.printInfo()
      # --- segment finingd
      include ("InDetRecExample/ConfiguredTRTSegmentFinding.py")
      InDetRecTRTSegementFinding = ConfiguredTRTSegmentFinding ("_TRT", [],
                                                                InDetNewTrackingCutsTRT,
                                                                InDetKeys.TRT_SegmentsTRT()
                                                                )
      # --- making tacks out of segments


      include ("InDetRecExample/ConfiguredTRTStandalone.py")
      InDetRecTRTStandalone = ConfiguredTRTStandalone ("_TRT", [],
                                                       InDetNewTrackingCutsTRT,
                                                       InDetKeys.TRT_SegmentsTRT(),
                                #                       InDetKeys.TRT_SegmentsTRT_EC(),
                                                       TrackCollectionKeys,
                                                       TrackCollectionTruthKeys)

    # ------------------------------------------------------------
    #
    # ----------- now we do the New Tracking
    #
    # ------------------------------------------------------------

    if InDetFlags.doNewTracking():
      #
      # --- do the Si pattern if not done for the cosmic case above 
      #
      if not InDetFlags.doTRTPhaseCalculation() or jobproperties.Beam.beamType()=="collisions":
        include ("InDetRecExample/ConfiguredNewTrackingSiPattern.py")
        InDetNewTrackingSiPattern = ConfiguredNewTrackingSiPattern([], InDetResolvedTracksKey,
                                                                   InDetSpSeededTracksKey,
                                                                   InDetNewTrackingCuts,
                                                                   TrackCollectionKeys,
                                                                   TrackCollectionTruthKeys)
      #
      # --- do the TRT pattern
      #
      if InDetFlags.doTRTExtension() :

        include ("InDetRecExample/ConfiguredNewTrackingTRTExtension.py")
        InDetNewTrackingTRTExtension = ConfiguredNewTrackingTRTExtension(InDetNewTrackingCuts,
                                                                         InDetNewTrackingSiPattern.SiTrackCollection(),
                                                                         InDetExtendedTracksKey,
                                                                         InDetExtendedTracksMapKey,
                                                                         TrackCollectionKeys,
                                                                         TrackCollectionTruthKeys,
                                                                         False)
        # --- add into list for combination
        InputCombinedInDetTracks += [ InDetNewTrackingTRTExtension.ForwardTrackCollection() ]
      else :
        InputCombinedInDetTracks += [ InDetNewTrackingSiPattern.SiTrackCollection() ]
        
    # ------------------------------------------------------------
    #
    # --- Now comes Back Tracktracking
    # 
    # ------------------------------------------------------------
    #
    # --- TRT segment finding after forward tracking on remaining hits
    #
    if InDetFlags.doTrtSegments():
      include ("InDetRecExample/ConfiguredTRTSegmentFinding.py")
      InDetRecTRTSegementFinding = ConfiguredTRTSegmentFinding ("",
                                                                InputCombinedInDetTracks,
                                                                InDetNewTrackingCuts,
                                                                InDetKeys.TRT_Segments()
                                                                )
    #
    # --- normal Back Tracking
    #
    if InDetFlags.doBackTracking():
      include ("InDetRecExample/ConfiguredBackTracking.py")
      InDetRecBackTracking = ConfiguredBackTracking (InputCombinedInDetTracks,
                                                     InDetNewTrackingCuts,
                                                     TrackCollectionKeys,
                                                     TrackCollectionTruthKeys)
      # --- add into list for combination
      InputCombinedInDetTracks += [ InDetRecBackTracking.BackTrackingTracks() ]

    # ------------------------------------------------------------
    #
    # --- Low Pt option (after BackTracking)
    #
    # ------------------------------------------------------------

    if InDetFlags.doLowPt():
      #
      # --- configure cuts for Low Pt tracking
      #
      if (not 'InDetNewTrackingCutsLowPt' in dir()):
        print "InDetRec_jobOptions: InDetNewTrackingCutsLowPt not set before - import them now"      
        from InDetRecExample.ConfiguredNewTrackingCuts import ConfiguredNewTrackingCuts
        InDetNewTrackingCutsLowPt = ConfiguredNewTrackingCuts("LowPt")
      InDetNewTrackingCutsLowPt.printInfo()
      #
      # --- now run Si pattern for Low Pt
      #
      include ("InDetRecExample/ConfiguredNewTrackingSiPattern.py")
      InDetLowPtSiPattern = ConfiguredNewTrackingSiPattern(InputCombinedInDetTracks,
                                                           InDetKeys.ResolvedLowPtTracks(),
                                                           InDetKeys.SiSpSeededLowPtTracks(),
                                                           InDetNewTrackingCutsLowPt,
                                                           TrackCollectionKeys,
                                                           TrackCollectionTruthKeys)
      #
      # --- do the TRT pattern
      #
      include ("InDetRecExample/ConfiguredNewTrackingTRTExtension.py")
      InDetLowPtTRTExtension = ConfiguredNewTrackingTRTExtension(InDetNewTrackingCutsLowPt,
                                                                 InDetLowPtSiPattern.SiTrackCollection(),
                                                                 InDetKeys.ExtendedLowPtTracks(),
                                                                 InDetKeys.ExtendedTracksMapLowPt(),
                                                                 TrackCollectionKeys,
                                                                 TrackCollectionTruthKeys)
      # --- add into list for combination
      InputCombinedInDetTracks += [ InDetLowPtTRTExtension.ForwardTrackCollection() ]

    # ------------------------------------------------------------
    #
    # --- Very Low Pt option (after LowPt)
    #
    # ------------------------------------------------------------

    if InDetFlags.doVeryLowPt():
      #
      # --- configure cuts for very Low Pt tracking
      #
      if (not 'InDetNewTrackingCutsVeryLowPt' in dir()):
        print "InDetRec_jobOptions: InDetNewTrackingCutsVeryLowPt not set before - import them now"      
        from InDetRecExample.ConfiguredNewTrackingCuts import ConfiguredNewTrackingCuts
        InDetNewTrackingCutsVeryLowPt = ConfiguredNewTrackingCuts("VeryLowPt")
      InDetNewTrackingCutsVeryLowPt.printInfo()
      #
      # --- now run Si pattern for Low Pt
      #
      include ("InDetRecExample/ConfiguredNewTrackingSiPattern.py")
      InDetVeryLowPtSiPattern = ConfiguredNewTrackingSiPattern(InputCombinedInDetTracks,
                                                               InDetKeys.ResolvedVeryLowPtTracks(),
                                                               InDetKeys.SiSpSeededVeryLowPtTracks(),
                                                               InDetNewTrackingCutsVeryLowPt,
                                                               TrackCollectionKeys,
                                                               TrackCollectionTruthKeys)
      # --- add into list for combination
      InputCombinedInDetTracks += [ InDetVeryLowPtSiPattern.SiTrackCollection() ]
 
    # ------------------------------------------------------------
    #
    # --- TRT standalone (after LowPt)
    #
    # ------------------------------------------------------------
    if InDetFlags.doTRTStandalone():

      include ("InDetRecExample/ConfiguredTRTStandalone.py")
      InDetRecTRTStandalone = ConfiguredTRTStandalone ("",
                                                       InputCombinedInDetTracks,
                                                       InDetNewTrackingCuts,
                                                       InDetKeys.TRT_Segments(),
                                                  #     InDetKeys.TRT_Segments_EC(),
                                                       TrackCollectionKeys,
                                                       TrackCollectionTruthKeys)
      # --- add into list for combination
      InputCombinedInDetTracks += [ InDetRecTRTStandalone.TRTStandaloneTracks() ]  


    # ------------------------------------------------------------
    #
    # --- Forward Tracklets (after standard reconstruction)
    #
    # ------------------------------------------------------------         

    if InDetFlags.doForwardTracks() and InDetFlags.doSLHC():
      if InDetFlags.doSLHCVeryForward(): 
       if (not 'InDetNewTrackingCutsForwardTracks' in dir()): 
         print "InDetRec_jobOptions: InDetNewTrackingCutsForwardTracks not set before - import them now"       
         from InDetRecExample.ConfiguredNewTrackingCuts import ConfiguredNewTrackingCuts 
         InDetNewTrackingCutsForwardTracks = ConfiguredNewTrackingCuts("VeryForwardSLHCTracks") 
         InDetNewTrackingCutsForwardTracks.printInfo() 
         # 
         # --- now run Si pattern for Low Pt 
         # 
         include ("InDetRecExample/ConfiguredNewTrackingSiPattern.py") 
         InDetForwardTracksSiPattern = ConfiguredNewTrackingSiPattern(InputCombinedInDetTracks, 
 		                                                      InDetKeys.ResolvedForwardTracks(), 
 		                                                      InDetKeys.SiSpSeededForwardTracks(), 
 		                                                      InDetNewTrackingCutsForwardTracks, 
 		                                                      TrackCollectionKeys, 
 		                                                      TrackCollectionTruthKeys)   
         # for ITK, forward tracks get added to the combined collection
         InputCombinedInDetTracks += [ InDetForwardTracksSiPattern.SiTrackCollection() ] 


      else:
       if (not 'InDetNewTrackingCutsForwardTracks' in dir()):
        print "InDetRec_jobOptions: InDetNewTrackingCutsForwardTracks not set before - import them now"      
        from InDetRecExample.ConfiguredNewTrackingCuts import ConfiguredNewTrackingCuts
        InDetNewTrackingCutsForwardTracks = ConfiguredNewTrackingCuts("ForwardSLHCTracks")
        InDetNewTrackingCutsForwardTracks.printInfo()
        #
        # --- now run Si pattern for Low Pt
        #
        include ("InDetRecExample/ConfiguredNewTrackingSiPattern.py")
        InDetForwardTracksSiPattern = ConfiguredNewTrackingSiPattern(InputCombinedInDetTracks,
                                                                   InDetKeys.ResolvedForwardTracks(),
                                                                   InDetKeys.SiSpSeededForwardTracks(),
                                                                   InDetNewTrackingCutsForwardTracks,
                                                                   TrackCollectionKeys,
                                                                   TrackCollectionTruthKeys)  
        # for ITK, forward tracks get added to the combined collection
        InputCombinedInDetTracks += [ InDetForwardTracksSiPattern.SiTrackCollection() ]


    elif InDetFlags.doForwardTracks():
      #
      # --- configure cuts for forward tracklets
      #
      if (not 'InDetNewTrackingCutsForwardTracks' in dir()):
        print "InDetRec_jobOptions: InDetNewTrackingCutsForwardTracks not set before - import them now"      
        from InDetRecExample.ConfiguredNewTrackingCuts import ConfiguredNewTrackingCuts
        InDetNewTrackingCutsForwardTracks = ConfiguredNewTrackingCuts("ForwardTracks")
        InDetNewTrackingCutsForwardTracks.printInfo()
      #
      # --- now run Si pattern for Low Pt
      #
      include ("InDetRecExample/ConfiguredNewTrackingSiPattern.py")
      InDetForwardTracksSiPattern = ConfiguredNewTrackingSiPattern(InputCombinedInDetTracks,
                                                                   InDetKeys.ResolvedForwardTracks(),
                                                                   InDetKeys.SiSpSeededForwardTracks(),
                                                                   InDetNewTrackingCutsForwardTracks,
                                                                   TrackCollectionKeys,
                                                                   TrackCollectionTruthKeys)  
      # --- do not add into list for combination YET
      # InputCombinedInDetTracks += [ InDetVeryLowPtSiPattern.SiTrackCollection() ]
    

    # ------------------------------------------------------------
    #
    # --- Pixel Tracklets on unassociated PRDs (after standard reconstruction + forward tracking)
    #
    # ------------------------------------------------------------
    
    if InDetFlags.doTrackSegmentsPixelPrdAssociation():
      InputPixelInDetTracks = []
      InputPixelInDetTracks += InputCombinedInDetTracks
      if InDetFlags.doForwardTracks(): 
        InputPixelInDetTracks +=[ InDetForwardTracksSiPattern.SiTrackCollection()]
      # --- load cuts for pixel segment finding
      if (not 'InDetNewTrackingCutsPixelPrdAssociation' in dir()):
        print "InDetRec_jobOptions: InDetNewTrackingCutsPixelPrdAssociation not set before - import them now"
        from InDetRecExample.ConfiguredNewTrackingCuts import ConfiguredNewTrackingCuts
        InDetNewTrackingCutsPixelPrdAssociation = ConfiguredNewTrackingCuts("PixelPrdAssociation")
      InDetNewTrackingCutsPixelPrdAssociation.printInfo()
      # --- configure pixel segment finding
      include ("InDetRecExample/ConfiguredNewTrackingSiPattern.py")
      InDetPixelTrackingSiPattern = ConfiguredNewTrackingSiPattern(InputPixelInDetTracks,InDetKeys.PixelPrdAssociationTracks(),
                                                                 InDetKeys.SiSpSeededPixelTracks(),
                                                                 InDetNewTrackingCutsPixelPrdAssociation,
                                                                 TrackCollectionKeys,
                                                                 TrackCollectionTruthKeys)
      
    # ------------------------------------------------------------
    #
    # --- Beam Gas option (runs after NewT + LowPt)
    #
    # ------------------------------------------------------------

    if InDetFlags.doBeamGas():
      #
      # --- configure cuts
      #
      if (not 'InDetNewTrackingCutsBeamGas' in dir()):
        print "InDetRec_jobOptions: InDetNewTrackingCutsBeamGas not set before - import them now"      
        from InDetRecExample.ConfiguredNewTrackingCuts import ConfiguredNewTrackingCuts
        InDetNewTrackingCutsBeamGas = ConfiguredNewTrackingCuts("BeamGas")
      if (InDetFlags.doPrintConfigurables()):
        InDetNewTrackingCutsBeamGas.printInfo()
      #
      # --- now run Si pattern for beam gas
      #
      include ("InDetRecExample/ConfiguredNewTrackingSiPattern.py")
      InDetBeamGasSiPattern = ConfiguredNewTrackingSiPattern(InputCombinedInDetTracks,
                                                             InDetKeys.ResolvedBeamGasTracks(),
                                                             InDetKeys.SiSpSeededBeamGasTracks(),
                                                             InDetNewTrackingCutsBeamGas,
                                                             TrackCollectionKeys,
                                                             TrackCollectionTruthKeys)
      #
      # --- do the TRT pattern
      #
      include ("InDetRecExample/ConfiguredNewTrackingTRTExtension.py")
      InDetBeamGasTRTExtension = ConfiguredNewTrackingTRTExtension(InDetNewTrackingCutsBeamGas,
                                                                   InDetBeamGasSiPattern.SiTrackCollection(),
                                                                   InDetKeys.ExtendedBeamGasTracks(),
                                                                   InDetKeys.ExtendedTracksMapBeamGas(),
                                                                   TrackCollectionKeys,
                                                                   TrackCollectionTruthKeys)

      # --- add into list for combination for single-beam mode (not if running after collisions)
      if jobproperties.Beam.beamType() == "singlebeam":
        InputCombinedInDetTracks += [ InDetBeamGasTRTExtension.ForwardTrackCollection() ]

    # ------------------------------------------------------------
    #
    # ----------- Ambi solve cosmic track collections from New Tracking
    #
    # ------------------------------------------------------------

    # --- Ambi solve the extended (Si + TRT) and TRT standalone tracks if both run
    if InDetFlags.doCosmics() and InDetFlags.doNewTracking() and len(InputCombinedInDetTracks) > 1:
      InputCosmicsCombinedAmbiSolver = list(InputCombinedInDetTracks)
      from TrkAmbiguitySolver.TrkAmbiguitySolverConf import Trk__TrkAmbiguitySolver
      InDetAmbiguitySolver_combinedCosmics = Trk__TrkAmbiguitySolver(name               = 'InDetCombinedCosmicsAmbiSolver',
                                                                     TrackInput         = InputCosmicsCombinedAmbiSolver,
                                                                     TrackOutput        = "CombinedCosmicTracks",
                                                                     AmbiguityProcessor = InDetAmbiguityProcessorCosmics)
      topSequence += InDetAmbiguitySolver_combinedCosmics
      if (InDetFlags.doPrintConfigurables()):
        print InDetAmbiguitySolver_combinedCosmics

      InputCombinedInDetTracks = [ InDetKeys.CombinedCosmicTracks() ]
	  
    # ------------------------------------------------------------
    #
    # ----------- now we do the Pseudo Tracking 
    #
    # ------------------------------------------------------------
 
    # if new tracking is off, pseudo tracking replaces the output collection
    # if new tracking is on, pseudo tracking simply runs alongside 
    # if split reco is on, pseudo tracking runs only on pileup and the output collections are merged
    if InDetFlags.doPseudoTracking() :
        # --- needed for sorting the PrepRawData properly
        from InDetTruthTools.InDetTruthToolsConf import InDet__PRD_TruthTrajectorySorterID
        InDetTruthTrajectorySorter = InDet__PRD_TruthTrajectorySorterID(name='InDetTruthTrajectorySorter')
        ToolSvc += InDetTruthTrajectorySorter

        # --- tool that provides the PRDs to the trajectory builder
        from InDetAssociationTools.InDetAssociationToolsConf import InDet__InDetPRD_Provider
        # change input collection key if taking only pileup
        if InDetFlags.doSplitReco() :
            PixelClusterCont    = InDetKeys.PixelPUClusters()
            SCT_ClusterCont     = InDetKeys.SCT_PU_Clusters()
            TRT_DriftCircleCont = InDetKeys.TRT_PU_DriftCircles()
        else:
            PixelClusterCont    = InDetKeys.PixelClusters()
            SCT_ClusterCont     = InDetKeys.SCT_Clusters()
            TRT_DriftCircleCont = InDetKeys.TRT_DriftCircles()
        InDetPRD_Provider = InDet__InDetPRD_Provider(name                     = 'InDetPRD_Provider',
                                                     PixelClusterContainer    = PixelClusterCont,
                                                     SCT_ClusterContainer     = SCT_ClusterCont,
                                                     TRT_DriftCircleContainer = TRT_DriftCircleCont)
        ToolSvc += InDetPRD_Provider

        # --- the truth track builder 
        from TrkTruthTrackTools.TrkTruthTrackToolsConf import Trk__TruthTrackBuilder
        InDetTruthTrackBuilder = Trk__TruthTrackBuilder(name                = 'InDetTruthTrackBuilder',
                                                        TrackFitter         = InDetTrackFitter,
                                                        ExtrapolationTool   = InDetExtrapolator,
                                                        RotCreatorTool      = InDetRotCreator,
                                                        BroadRotCreatorTool = BroadInDetRotCreator,
                                                        MinDegreesOfFreedom = 1)
#        InDetTruthTrackBuilder.OutputLevel = VERBOSE
        ToolSvc += InDetTruthTrackBuilder
        
        # --- the trajectory manipulator
        from InDetTruthTools.InDetTruthToolsConf import InDet__PRD_TruthTrajectoryManipulatorID
        InDetTruthTrajectoryManipulator = InDet__PRD_TruthTrajectoryManipulatorID(name='InDetTruthTrajectoryManipulator')
        ToolSvc += InDetTruthTrajectoryManipulator
#        InDetTruthTrajectoryManipulator.OutputLevel = VERBOSE

        # --- the trajectory shared cluster hits fixer
#        from InDetTruthTools.InDetTruthToolsConf import InDet__PRD_TruthTrajectorySharedFixerID
#        InDetTruthTrajectorySharedFixer = InDet__PRD_TruthTrajectorySharedFixerID(name = 'InDetTruthTrajectorySharedFixer' )                       
#        ToolSvc += InDetTruthTrajectorySharedFixer

        # --- the truth PRD trajectory builder
        
        # 
        from TrkTruthTrackTools.TrkTruthTrackToolsConf import Trk__PRD_TruthTrajectoryBuilder
        # change input collection key if taking only pileup
        if InDetFlags.doSplitReco() :
            PixelClusterTruth      = InDetKeys.PixelPUClustersTruth()
            SCT_ClusterTruth       = InDetKeys.SCT_PU_ClustersTruth()
            TRT_DriftCircleTruth   = InDetKeys.TRT_PU_DriftCirclesTruth()
        else:
            PixelClusterTruth    = InDetKeys.PixelClustersTruth()
            SCT_ClusterTruth     = InDetKeys.SCT_ClustersTruth()
            TRT_DriftCircleTruth = InDetKeys.TRT_DriftCirclesTruth()
        InDetPRD_TruthTrajectoryBuilder=Trk__PRD_TruthTrajectoryBuilder(name = 'InDetPRD_TruthTrajectoryBuilder',
                                         PRD_MultiTruthCollections       = [ PixelClusterTruth,SCT_ClusterTruth,TRT_DriftCircleTruth],
                                         InDetPRD_Provider               = InDetPRD_Provider,
                                         MinimumPt                       =  InDetNewTrackingCuts.minPT(),
                                 #        Geantinos                       =  True,
                                         PRD_TruthTrajectoryManipulators = [ InDetTruthTrajectorySorter, InDetTruthTrajectoryManipulator  ])
#                                        PRD_TruthTrajectoryManipulators = [ InDetTruthTrajectorySorter, InDetTruthTrajectorySharedFixer  ])
        ToolSvc+=InDetPRD_TruthTrajectoryBuilder
#        InDetPRD_TruthTrajectoryBuilder.OutputLevel = VERBOSE

        # --- the (1st) trajectory selector
        from InDetTruthTools.InDetTruthToolsConf import InDet__PRD_TruthTrajectorySelectorID
        InDetTruthTrajectorySelector = InDet__PRD_TruthTrajectorySelectorID(name='InDetTruthTrajectorySelector')
        ToolSvc += InDetTruthTrajectorySelector

        # --- the truth track creation algorithm
        from TrkTruthTrackAlgs.TrkTruthTrackAlgsConf import Trk__TruthTrackCreation
        InDetTruthTrackCreation = Trk__TruthTrackCreation(name = 'InDetTruthTrackCreation',
                                                          PRD_TruthTrajectoryBuilder = InDetPRD_TruthTrajectoryBuilder,
                                                          TruthTrackBuilder          = InDetTruthTrackBuilder,
                                                          OutputTrackCollection      = InDetKeys.PseudoTracks(),
                                                          AssoTool                   = InDetPrdAssociationTool,
                                                          TrackSummaryTool           = InDetTrackSummaryToolSharedHits,
                                                          PRD_TruthTrajectorySelectors  = [ InDetTruthTrajectorySelector ] )
#        InDetTruthTrackCreation.OutputLevel = VERBOSE
        topSequence += InDetTruthTrackCreation

        # --- add the truth to the truth tracks ;-)
        include ("InDetRecExample/ConfiguredInDetTrackTruth.py")
        InDetTracksTruth = ConfiguredInDetTrackTruth(InDetKeys.PseudoTracks(),
                                                     InDetKeys.PseudoDetailedTracksTruth(),
                                                     InDetKeys.PseudoTracksTruth())

        from TrkTruthToTrack.TrkTruthToTrackConf import Trk__TruthToTrack
        InDetTruthToTrack  = Trk__TruthToTrack(name         = "InDetTruthToTrack",
                                               Extrapolator = InDetExtrapolator)
        ToolSvc += InDetTruthToTrack
    
        # Register the track collections for further processing - only if new tracking has not been running
        if not InDetFlags.doNewTracking():
            InputTrackCollection      = InDetKeys.PseudoTracks()
            InputTrackCollectionTruth = InDetKeys.PseudoTracksTruth()
            InputDetailedTrackTruth   = InDetKeys.PseudoDetailedTracksTruth()
            # make sure downstream clients get the right tracks
            InDetKeys.Tracks                   = InDetKeys.PseudoTracks()
            InDetKeys.UnslimmedTracks          = InDetKeys.PseudoTracks()
            InDetKeys.TracksTruth              = InDetKeys.PseudoTracksTruth()
            InDetKeys.UnslimmedTracksTruth     = InDetKeys.PseudoTracksTruth()
            # add as the combined ID collection
            InputCombinedInDetTracks = [ InDetKeys.PseudoTracks() ]
        if InDetFlags.doSplitReco() :
            InputCombinedInDetTracks += [ InDetKeys.PseudoTracks() ]


        # add final output for statistics
        TrackCollectionKeys      += [ InDetKeys.PseudoTracks() ]
        TrackCollectionTruthKeys += [ InDetKeys.PseudoTracksTruth() ]
    # ------------------------------------------------------------
    #
    # ----------- Merge track collections from NewTracking
    #
    # ------------------------------------------------------------

    # always merge track collections. even if only one collection ran!
    # (this is because the track merger fills the prd association tool!)
    # *ME* fix, only merge if more than 1 track collection
    if InDetFlags.doNewTrackingPattern() or InDetFlags.doBeamHalo():
      from TrkTrackCollectionMerger.TrkTrackCollectionMergerConf import Trk__TrackCollectionMerger
      TrkTrackCollectionMerger = Trk__TrackCollectionMerger(name                    = "InDetTrackCollectionMerger",
                                                            TracksLocation          = InputCombinedInDetTracks,
                                                            OutputTracksLocation    = InDetKeys.UnslimmedTracks(),
                                                            AssoTool                = InDetPrdAssociationTool,
                                                            UpdateSharedHitsOnly    = False,
                                                            UpdateAdditionalInfo    = True,
                                                            SummaryTool             = InDetTrackSummaryToolSharedHits)
      topSequence += TrkTrackCollectionMerger
      if (InDetFlags.doPrintConfigurables()):
        print TrkTrackCollectionMerger

      # --- Delete unmerged tracks (Si, back-tracking, TRT)
      from InDetRecExample.ConfiguredInDetSGDeletion import InDetSGDeletionAlg
      if not InDetFlags.doMonitoringAlignment():
        InDetSGDeletionAlg(key = InputCombinedInDetTracks)
      else:
        InDetSGDeletionAlg(key = [k for k in InputCombinedInDetTracks if not k == "ExtendedTracks"])
      

      #
      # ------------ Track truth.
      #
      if not InDetFlags.doSGDeletion():
        TrackCollectionKeys      += [ InDetKeys.UnslimmedTracks() ]
        if InDetFlags.doTruth():
          # set up the truth info for this container
          #
          include ("InDetRecExample/ConfiguredInDetTrackTruth.py")
          InDetTracksTruth = ConfiguredInDetTrackTruth(InDetKeys.UnslimmedTracks(),
                                                       InDetKeys.UnslimmedDetailedTracksTruth(),
                                                       InDetKeys.UnslimmedTracksTruth())
          #
          # add final output for statistics
          #
          TrackCollectionTruthKeys += [ InDetKeys.UnslimmedTracksTruth() ]
          
    # ------------------------------------------------------------
    #
    # ----------- prepare output track collection for post processing
    #
    # ------------------------------------------------------------

    #
    # -- Pick one of the result collections and turn it into tracks
    #
    if InDetFlags.doNewTrackingPattern():
      InputTrackCollection = InDetKeys.UnslimmedTracks()
    elif InDetFlags.doPseudoTracking():
      InputTrackCollection = InDetKeys.PseudoTracks()
    elif InDetFlags.doiPatRec():
      InputTrackCollection = InDetKeys.IPatConvertedTracks()
    elif InDetFlags.doxKalman():
      InputTrackCollection = InDetKeys.XKalConvertedTracks()
    else:
      # --- in case of reading from ESD, so we just set the Collection and truth
      InputTrackCollection      = InDetKeys.Tracks()
      InputTrackCollectionTruth = InDetKeys.TracksTruth()

    # ---------------------------------------------------------------- 
    #
    # --- do we refit all tracks ?
    #
    # ---------------------------------------------------------------- 
    if InDetFlags.doRefit():
      from TrkRefitAlg.TrkRefitAlgConf import Trk__ReFitTrack
      InDetReFitTrack = Trk__ReFitTrack (name           = "InDetRefitTrack",
                                         FitterTool     = InDetTrackFitter,
                                         FitterToolTRT  = InDetTrackFitterTRT,
                                         SummaryTool    = InDetTrackSummaryToolSharedHits,
                                         AssoTool       = InDetPrdAssociationTool,
                                         TrackName      = InputTrackCollection,
                                         NewTrackName   = InDetKeys.RefittedTracks(),
                                         fitRIO_OnTrack = InDetFlags.refitROT(),
                                         useParticleHypothesisFromTrack = True)
        
      if InDetFlags.materialInteractions():
        InDetReFitTrack.matEffects = InDetFlags.materialInteractionsType()
      else:
        InDetReFitTrack.matEffects = 0
            
      topSequence += InDetReFitTrack
      if (InDetFlags.doPrintConfigurables()):
        print InDetReFitTrack
      # set input track name
      InputTrackCollection = InDetKeys.RefittedTracks()
      if InDetFlags.doTruth():
        InputDetailedTrackTruth   = InDetKeys.RefittedDetailedTracksTruth()
        InputTrackCollectionTruth = InDetKeys.RefittedTracksTruth()

    # ---------------------------------------------------------------- 
    #
    # --- slimm the tracks down before writing them ?
    #
    # ---------------------------------------------------------------- 
    if InDetFlags.doSlimming(): # not an "elif" as we may want to slim or alias even if refit, in case pattern runs 
      #
      # --- do track slimming
      #
      from TrkTrackSlimmingTool.TrkTrackSlimmingToolConf import Trk__TrackSlimmingTool as ConfigurableTrackSlimmingTool
      InDetTrkSlimmingTool = ConfigurableTrackSlimmingTool(name           = "InDetTrackSlimmingTool",
                                                           KeepParameters = InDetFlags.KeepParameters(),
                                                           KeepOutliers   = True )
      ToolSvc += InDetTrkSlimmingTool
      if (InDetFlags.doPrintConfigurables()):
        print InDetTrkSlimmingTool
            
      from TrkTrackSlimmer.TrkTrackSlimmerConf import Trk__TrackSlimmer as ConfigurableTrackSlimmer
      InDetTrkSlimmer = ConfigurableTrackSlimmer(name                 = "InDetTrackSlimmer",
                                                 TrackLocation        = [ InputTrackCollection ],
                                                 SlimmedTrackLocation = [ InDetKeys.Tracks() ],
                                                 TrackSlimmingTool    = InDetTrkSlimmingTool)
      topSequence += InDetTrkSlimmer
      if (InDetFlags.doPrintConfigurables()):
        print InDetTrkSlimmer

      if not (InDetFlags.doMonitoringGlobal() or 
              InDetFlags.doNtupleCreation() or
              (InDetFlags.doMonitoringPixel() and not InDetFlags.doTrackSegmentsPixel()) or 
              (InDetFlags.doMonitoringSCT()   and not InDetFlags.doTrackSegmentsSCT()  ) or 
              (InDetFlags.doMonitoringTRT()   and not InDetFlags.doTrackSegmentsTRT()  )):
        # --- Delete unslimmed tracks
        from InDetRecExample.ConfiguredInDetSGDeletion import InDetSGDeletionAlg
        InDetSGDeletionAlg(key = InputTrackCollection)
      
      # --- for output
      InDetKeys.AliasToTracks = 'none'
      # --- input for next algorithm
      InputTrackCollection    = InDetKeys.Tracks()
      if InDetFlags.doTruth():
        InputDetailedTrackTruth   = InDetKeys.DetailedTracksTruth()
        InputTrackCollectionTruth = InDetKeys.TracksTruth()        

    # ---------------------------------------------------------------- 
    #
    # --- or just make an alias ?
    #
    # ---------------------------------------------------------------- 
    elif InDetFlags.doPattern() :
      #
      # --- configure Algorithm to create output alias
      #
      from TrkCollectionAliasAlg.TrkCollectionAliasAlgConf import Trk__TrkCollectionAliasAlg
      InDetCopyAlg = Trk__TrkCollectionAliasAlg (name             = "InDetCopyAlg",
                                                 CollectionName   = InputTrackCollection,
                                                 AliasName        = InDetKeys.Tracks()) 
      topSequence += InDetCopyAlg
      if (InDetFlags.doPrintConfigurables()):
        print InDetCopyAlg
      # --- for output
      InDetKeys.AliasToTracks = InputTrackCollection
      # --- input for next algorithm
      InputTrackCollection    = InDetKeys.Tracks()
      if InDetFlags.doTruth():
        InputDetailedTrackTruth   = InDetKeys.DetailedTracksTruth()
        InputTrackCollectionTruth = InDetKeys.TracksTruth()

    # -----------------------------------------------------------------
    #
    # --- in case of slimming or alias creation of tracks container, do the truth association
    #
    # -----------------------------------------------------------------
    if (InDetFlags.doRefit() or InDetFlags.doSlimming() or InDetFlags.doPattern()):
      ## ME drop this for tracks container # TrackCollectionKeys      += [ InputTrackCollection ]
      if InDetFlags.doTruth():
        #
        # set up the truth info for this container
        #
        include ("InDetRecExample/ConfiguredInDetTrackTruth.py")
        InDetTracksTruth = ConfiguredInDetTrackTruth(InputTrackCollection,
                                                     InputDetailedTrackTruth,
                                                     InputTrackCollectionTruth)
        #
        # add to keys lists for statistics
        #
        ## ME drop this for tracks contrain # TrackCollectionTruthKeys += [ InputTrackCollectionTruth ]
        
    # ------------------------------------------------------------
    #
    # ----------- now we do post-processing
    #
    # ------------------------------------------------------------
    #
    #
    # xAOD creation/conversion
    if (InDetFlags.doxAOD()):
      include("InDetRecExample/InDetxAODCreator.py")

    # Do post-processing algorithms (may depend on xAOD objects)
    include("InDetRecExample/InDetRecPostProcessing.py")

    # ------------------------------------------------------------
    #
    # ----------- now we do validation and  
    #
    # ------------------------------------------------------------
    #

    # statistics
    if InDetFlags.doStatistics():
      if InDetFlags.doVeryLowPt():
        cuts = InDetNewTrackingCutsVeryLowPt
      elif InDetFlags.doLowPt():
        cuts = InDetNewTrackingCutsLowPt
      else:
        cuts = InDetNewTrackingCuts
      include("InDetRecExample/ConfiguredInDetValidation.py")
      InDetValidation = ConfiguredInDetValidation("",True,InDetFlags.doTruth(),cuts,TrackCollectionKeys,TrackCollectionTruthKeys)

    # ntuple creation for validation purposes    
    if (InDetFlags.doNtupleCreation() or InDetFlags.doStandardPlots()) or InDetFlags.doPhysValMon():
      include("InDetRecExample/InDetRecNtupleCreation.py")

    # D3PD Creation
    if (InDetFlags.doD3PDCreation()):
      include("InDetRecExample/InDetRecD3PDCreation.py")

    if not InDetFlags.doMonitoring():
      # --- Delete spacepoint collections
      from InDetRecExample.ConfiguredInDetSGDeletion import InDetSGDeletionAlg
      InDetSGDeletionAlg(container = "SpacePointOverlapCollection#", key = InDetKeys.OverlapSpacePoints())
      
      # Delete Pixel and Silicon space points
      from InDetRecExample.ConfiguredInDetSGDeletion import InDetSGDeletionAlg
      InDetSGDeletionAlg(container = "SpacePointContainer#", key = [InDetKeys.PixelSpacePoints(), InDetKeys.SCT_SpacePoints()])
    
    # +++++++++++++++++++ end of InDetRec_jobOptions.py
  # END if InDetFlags.Enabled()    

