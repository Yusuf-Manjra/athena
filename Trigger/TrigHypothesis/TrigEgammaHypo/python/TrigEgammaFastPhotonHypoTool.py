# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration


from AthenaCommon.SystemOfUnits import GeV


#
# For photons
#
class TrigEgammaFastPhotonHypoToolConfig:

  __operation_points  = [  'tight'    , 
                           'medium'   , 
                           'loose'    , 
                           'etcut'    , 
                           ]

  def same( self, val ):
    return [val]*( len( self.tool().EtaBins ) - 1 )

  def __init__(self, name, threshold, sel):

    from AthenaCommon.Logging import logging
    self.__log = logging.getLogger('TrigEgammaFastPhotonHypoTool')
    self.__name       = name
    self.__threshold  = float(threshold) 
    self.__sel        = sel

    from AthenaConfiguration.ComponentFactory import CompFactory
    tool = CompFactory.TrigEgammaFastPhotonHypoToolInc(name)
    tool.EtaBins = [0, 0.6, 0.8, 1.15, 1.37, 1.52, 1.81, 2.01, 2.37, 2.47] 

    self.__tool = tool

 
    tool.ETthr = self.same( 0. )
    tool.CARCOREthr = self.same( 0.0 )
    tool.CAERATIOthr = self.same( 0.0)
    tool.F1thr = self.same( 0.005 )
    tool.ET2thr = self.same( 90.0 * GeV )
    tool.HADET2thr = self.same( 999. * GeV )
    tool.HADETthr =  self.same( 999. * GeV ) 

 

    
    self.__log.debug( 'Chain     :%s', name )
    self.__log.debug( 'Threshold :%s', threshold )
    self.__log.debug( 'Pidname   :%s', sel )



  def chain(self):
    return self.__name
  
  def pidname( self ):
    return self.__sel

  def etthr(self):
    return self.__threshold

  def tool(self):
    return self.__tool
  
  def etcut(self):
    self.__log.debug( 'Chain     :%s configured with etcut selection', self.chain() )
    self.tool().ETthr = self.same( self.etthr() *GeV - 3.* GeV) # Set the eT cut 3 GeV below HLT threshold

  def nocut(self):
    self.__log.debug( 'Chain     :%s configured with nocut selection', self.chain() )
    self.tool().AcceptAll = True

  def nominal(self):
    from TrigEgammaHypo.TrigEgammaFastCutDefs import TrigFastPhotonCutMaps
    self.__log.debug( 'Chain     :%s configured with nominal selection', self.chain() )
    self.tool().ETthr = self.same( self.etthr() *GeV - 3.* GeV) # Set the eT cut 3 GeV below HLT threshold
    self.tool().CARCOREthr     = TrigFastPhotonCutMaps( self.etthr() ).MapsCARCOREthr[ self.pidname() ]
    self.tool().CAERATIOthr    = TrigFastPhotonCutMaps( self.etthr() ).MapsCAERATIOthr [ self.pidname() ]
    self.tool().HADETthr       = TrigFastPhotonCutMaps( self.etthr() ).MapsHADETthr[ self.pidname() ]
    self.tool().HADET2thr = self.same(999.0 * GeV)
    self.tool().F1thr = self.same(0.005)
    self.tool().ET2thr = self.same( 90.0*GeV )


  #
  # Compile the chain
  #
  def compile(self):
    if 'etcut' == self.pidname() or 'ion' in self.pidname():       
        self.etcut()
    elif 'noalg' == self.pidname():
        self.nocut()
    else:
        self.nominal()


    # add mon tool
    self.addMonitoring()


  #
  # Monitoring code
  #
  def addMonitoring(self):
    
    from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool,defineHistogram

    monTool = GenericMonitoringTool("MonTool"+self.chain())
    monTool.Histograms = [
      defineHistogram('CutCounter', type='TH1I', path='EXPERT', title="FastPhoton Hypo Cut Counter;Cut Counter", xbins=8, xmin=-1.5, xmax=7.5, opt="kCumulative"),
      defineHistogram('PtCalo', type='TH1F', path='EXPERT', title="FastPhoton Hypo p_{T}^{calo} [MeV];p_{T}^{calo} [MeV];Nevents", xbins=50, xmin=0, xmax=100000),
      defineHistogram('CaloEta', type='TH1F', path='EXPERT', title="FastPhoton Hypo #eta^{calo} ; #eta^{calo};Nevents", xbins=200, xmin=-2.5, xmax=2.5),
      defineHistogram('CaloPhi', type='TH1F', path='EXPERT', title="FastPhoton Hypo #phi^{calo} ; #phi^{calo};Nevents", xbins=320, xmin=-3.2, xmax=3.2),
    ]
    monTool.HistPath = 'FastPhotonHypo/'+self.chain()
    self.__tool.MonTool = monTool




def _IncTool(name, threshold, sel):
  config = TrigEgammaFastPhotonHypoToolConfig(name, threshold, sel)
  config.compile()
  return config.tool()



def TrigEgammaFastPhotonHypoToolFromDict( d ):
    """ Use menu decoded chain dictionary to configure the tool """
    cparts = [i for i in d['chainParts'] if i['signature']=='Photon' ]

    def __th(cpart):
        return cpart['threshold']

    def __sel(cpart):
        return cpart['addInfo'][0] if cpart['addInfo'] else cpart['IDinfo']

    name = d['chainName']

    return _IncTool( name,  __th( cparts[0]),  __sel( cparts[0]) )



def TrigEgammaFastPhotonHypoToolFromName( name, conf ):
    """ provides configuration of the hypo tool given the chain name
    The argument will be replaced by "parsed" chain dict. For now it only serves simplest chain HLT_eXYZ.
    """

    from TriggerMenuMT.HLTMenuConfig.Menu.DictFromChainName import dictFromChainName
    decodedDict = dictFromChainName(conf)
    return TrigEgammaFastPhotonHypoToolFromDict( decodedDict )


if __name__ == "__main__":
    
    tool = TrigEgammaFastPhotonHypoToolFromName("HLT_g5_etcut_L1EM3", "HLT_g5_etcut_L1EM3")   
    assert tool, "Not configured simple tool"
