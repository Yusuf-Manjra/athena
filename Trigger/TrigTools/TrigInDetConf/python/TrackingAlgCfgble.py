# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigFastTrackFinder.TrigFastTrackFinder_Config import TrigFastTrackFinderBase

class TrigFastTrackFinder(TrigFastTrackFinderBase):
  def __init__(self, name="TrigTrackFinder_Muon", type="muon"):

    #dictionary to accommodate old instance naming of the TrigFastTrackFinder
    #new configurations should not enter here
    remap  = {
      "muon"        : "Muon",
      "muonCore"    : "MuonCore",
      "muonIso"     : "MuonIso",
      "electron"    : "eGamma",
      "tau"         : "Tau",
      "tauCore"     : "TauCore",
      "tauIso"      : "TauIso",
      "bjet"        : "Jet",
      #"bjetVtx"     : "Jet",       #if configs are the same they can map to the same instance
      "fullScan"    : "FullScan",
      "beamSpot"    : "BeamSpot",
      "bphysics"    : "Bphysics",
      "cosmics"     : "Cosmic",
      }

    #this should not happen here
    hacktype = type
    if 'Core' in name and 'Core' not in type:
        hacktype = type+'Core'
    elif 'Iso' in name and 'Iso' not in type:
        hacktype = type+'Iso'

    conftype = hacktype
    if hacktype in remap:
      conftype = remap[hacktype]
      
    TrigFastTrackFinderBase.__init__(self, name, conftype)

