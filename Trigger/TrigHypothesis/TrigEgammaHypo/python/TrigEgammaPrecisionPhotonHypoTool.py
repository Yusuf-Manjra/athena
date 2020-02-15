# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)
log = logging.getLogger("TrigEgammaHypo.TrigEgammaPrecisionPhotonHypoTool") 
from TriggerMenuMT.HLTMenuConfig.Egamma.EgammaDefs import TrigPhotonSelectors
def _IncTool(name, threshold, sel):

    log.debug('TrigEgammaPrecisionPhotonHypoTool _IncTool("'+name+'", threshold = '+str(threshold) + ', sel = '+str(sel))

    from TrigEgammaHypo.TrigEgammaHypoConf import TrigEgammaPrecisionPhotonHypoToolInc    

    tool = TrigEgammaPrecisionPhotonHypoToolInc( name ) 

    from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool, defineHistogram
    monTool = GenericMonitoringTool("MonTool_"+name)
    monTool.Histograms = [ defineHistogram('dEta', type='TH1F', path='EXPERT', title="PrecisionPhoton Hypo #Delta#eta_{EF L1}; #Delta#eta_{EF L1}", xbins=80, xmin=-0.01, xmax=0.01),
                           defineHistogram('dPhi', type='TH1F', path='EXPERT', title="PrecisionPhoton Hypo #Delta#phi_{EF L1}; #Delta#phi_{EF L1}", xbins=80, xmin=-0.01, xmax=0.01),
                           defineHistogram('Et_em', type='TH1F', path='EXPERT', title="PrecisionPhoton Hypo cluster E_{T}^{EM};E_{T}^{EM} [MeV]", xbins=50, xmin=-2000, xmax=100000),
                           defineHistogram('Eta', type='TH1F', path='EXPERT', title="PrecisionPhoton Hypo entries per Eta;Eta", xbins=100, xmin=-2.5, xmax=2.5),
                           defineHistogram('Phi', type='TH1F', path='EXPERT', title="PrecisionPhoton Hypo entries per Phi;Phi", xbins=128, xmin=-3.2, xmax=3.2),
                           defineHistogram('EtaBin', type='TH1I', path='EXPERT', title="PrecisionPhoton Hypo entries per Eta bin;Eta bin no.", xbins=11, xmin=-0.5, xmax=10.5)]

    cuts=['Input','#Delta #eta EF-L1', '#Delta #phi EF-L1','eta','E_{T}^{EM}']

    monTool.Histograms += [ defineHistogram('CutCounter', type='TH1I', path='EXPERT', title="PrecisionPhoton Hypo Passed Cuts;Cut",
                                            xbins=13, xmin=-1.5, xmax=12.5,  opt="kCumulative", xlabels=cuts) ]

    monTool.HistPath = 'PrecisionPhotonHypo/'+tool.name()
    tool.MonTool = monTool

    tool.EtaBins        = [0.0, 0.6, 0.8, 1.15, 1.37, 1.52, 1.81, 2.01, 2.37, 2.47]
    def same( val ):
        return [val]*( len( tool.EtaBins ) - 1 )

    tool.ETthr          = same( float(threshold) )
    tool.dETACLUSTERthr = 0.1
    tool.dPHICLUSTERthr = 0.1

    # configure the selector tool corresponding the selection set by sel
    tool.PhotonIsEMSelector = TrigPhotonSelectors(sel)

    return tool


def _MultTool(name):
    from TrigEgammaHypo.TrigEgammaHypoConf import TrigEgammaPrecisionPhotonHypoToolMult
    return TrigEgammaPrecisionPhotonHypoToolMult( name )


def TrigEgammaPrecisionPhotonHypoToolFromDict( d ):
    """ Use menu decoded chain dictionary to configure the tool """
    cparts = [i for i in d['chainParts'] if ((i['signature']=='Electron') or (i['signature']=='Photon'))]
    
    def __mult(cpart):
        return int( cpart['multiplicity'] )

    def __th(cpart):
        return cpart['threshold']
    
    def __sel(cpart):
        return cpart['addInfo'][0] if cpart['addInfo'] else cpart['IDinfo']
    
    name = d['chainName']

    # do we need to configure high multiplicity selection, either NeX or ex_ey_ez etc...?
    if len(cparts) > 1 or __mult(cparts[0]) > 1:
        tool = _MultTool(name)
        for cpart in cparts:
            for cutNumber in range( __mult( cpart ) ):
                tool.SubTools += [ _IncTool( cpart['chainPartName']+"_"+str(cutNumber), __th( cpart ), __sel( cpart) ) ]

        return tool
    else:        
        return _IncTool( name, __th( cparts[0]),  __sel( cparts[0] ) )
                   
    
