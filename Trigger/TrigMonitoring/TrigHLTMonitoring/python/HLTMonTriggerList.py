# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import TrigBjetMonitoring.TrigBjetMonitCategory as bjet
import TrigBphysMonitoring.TrigBphysMonitCategory as bphys
import TrigEgammaMonitoring.TrigEgammaMonitCategory as egamma
import TrigJetMonitoring.TrigJetMonitCategory as jets
import TrigMETMonitoring.TrigMETMonitCategory as met
import TrigMinBiasMonitoring.TrigMinBiasMonitCategory as minbias
import TrigMuonMonitoring.TrigMuonMonitCategory as muon
import TrigTauMonitoring.TrigTauMonitCategory as tau

class HLTMonTriggerList:
     _configured = False
     monitoring_mujet = []
     monitoring_bjet = []
     monitoring_bphys = []
     primary_bphys = []
     monitoring_egamma = []
     monitoring_l1jet = []
     monitoring_jet = []
     primary_l1jet = []
     primary_jet = []
     monitoring_met = []
     monitoring_minbias = []
     monitoring_muonNonIso = []
     monitoring_muonIso = []
     monitoring_MSonly = []
     monitoring_tau = []
     monitoring_singleTau = []
     
     def __init__ (self):
          if not self._configured:
               self.config()

     def config (self):

          # set the bjet and mujet triggers to the default values
          self.monitoring_bjet = bjet.monitoring_bjet
          self.monitoring_mujet = bjet.monitoring_mujet

          # set the bphys triggers to the default values
          self.monitoring_bphys = bphys.monitoring_bphys
          self.primary_bphys = bphys.primary_bphys

          # set the egamma triggers to the default values
          self.monitoring_egamma = egamma.monitoring_egamma

          # set the jet triggers to the default values
          self.monitoring_l1jet = jets.monitoring_l1jet
          self.monitoring_jet = jets.monitoring_jet
          self.primary_l1jet = jets.primary_l1jet
          self.primary_jet = jets.primary_jet

          # set the met triggers to the default values
          self.monitoring_met = met.monitoring_met

          # set the minbias triggers to the default values
          self.monitoring_minbias = minbias.monitoring_minbias
          
          # set the muon triggers to the default values
          self.monitoring_muonNonIso = muon.monitoring_muonNonIso
          self.monitoring_muonIso = muon.monitoring_muonIso
          self.monitoring_MSonly = muon.monitoring_MSonly

          # set the tau triggers to the default values
          self.monitoring_tau = tau.monitoring_tau
          self.monitoring_singleTau = tau.monitoring_singleTau

          self._configured = True

hltmonList = HLTMonTriggerList()
