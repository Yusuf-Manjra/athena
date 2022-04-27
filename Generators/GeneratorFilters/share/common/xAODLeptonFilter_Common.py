# common fragment for xAODLepton filter
# conversion to XAOD, 
# creation of slimmed container containing electrons and muons
# connecting the filter

if not hasattr(prefiltSeq, 'xAODCnv'):  
  from xAODTruthCnv.xAODTruthCnvConf import xAODMaker__xAODTruthCnvAlg
  prefiltSeq += xAODMaker__xAODTruthCnvAlg('xAODCnv',WriteTruthMetaData=False)
  prefiltSeq.xAODCnv.AODContainerName = 'GEN_EVENT'

if not hasattr(prefiltSeq, "xAODTruthParticleSlimmerElectron"):
  from GeneratorFilters.GeneratorFiltersConf import xAODTruthParticleSlimmerElectron
  prefiltSeq += xAODTruthParticleSlimmerElectron('xAODTruthParticleSlimmerElectron')

if not hasattr(prefiltSeq, "xAODTruthParticleSlimmerMuon"):
  from GeneratorFilters.GeneratorFiltersConf import xAODTruthParticleSlimmerMuon   
  prefiltSeq += xAODTruthParticleSlimmerMuon('xAODTruthParticleSlimmerMuon')

from GeneratorFilters.GeneratorFiltersConf import xAODLeptonFilter
xAODLeptonFilter = xAODLeptonFilter("xAODLeptonFilter")
filtSeq += xAODLeptonFilter

# to modiify cuts put into JOs e.g.:
#filtSeq.xAODLeptonFilter.Ptcut = 12000.0
#filtSeq.xAODLeptonFilter.Etacut = 10.0
#filtSeq.xAODLeptonFilter.PtcutMax = 90000000000.0

