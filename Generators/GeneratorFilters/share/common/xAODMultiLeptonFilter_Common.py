# common fragment for xAODMultiLepton filter
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

from GeneratorFilters.GeneratorFiltersConf import xAODMultiLeptonFilter
xAODMultiLeptonFilter = xAODMultiLeptonFilter("xAODMultiLeptonFilter")
filtSeq += xAODMultiLeptonFilter

# to modiify cuts put into JOs e.g.:
#filtSeq.xAODMultiLeptonFilter.Ptcut = 12000.0
#filtSeq.xAODMultiLeptonFilter.Etacut = 10.0
#filtSeq.xAODMultiLeptonFilter.NLeptons = 4

