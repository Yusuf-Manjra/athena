#
# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#


from AthenaCommon.SystemOfUnits import GeV


def same( val , tool):
  return [val]*( len( tool.EtaBins ) - 1 )
  

#
# For photons
#
class TrigEgammaEmulationPrecisionPhotonHypoToolConfig:

  __operation_points  = [  'tight'    , 
                           'medium'   , 
                           'loose'    , 
                           ]

  __caloIsolationCut = {
                          None          : None,
                          'icaloloose'  : 0.1,
                          'icalomedium' : 0.075,
                          'icalotight'  : 0.
                        }

  def __init__(self, name, chain, threshold, sel, isoinfo):

    from AthenaCommon.Logging import logging
    self.__log = logging.getLogger('TrigEgammaPrecisionPhotonHypoTool')
    self.__chain      = chain
    self.__threshold  = float(threshold) 
    self.__sel        = sel
    self.__isoinfo    = isoinfo

    from TrigEgammaEmulationToolMT.TrigEgammaEmulationToolMTConf import Trig__TrigEgammaEmulationPrecisionPhotonHypoTool
    tool = Trig__TrigEgammaEmulationPrecisionPhotonHypoTool( name ) 
    tool.EtaBins        = [0.0, 0.6, 0.8, 1.15, 1.37, 1.52, 1.81, 2.01, 2.37, 2.47]
    tool.ETthr          = same( float(threshold) , tool)
    tool.dETACLUSTERthr = 0.1
    tool.dPHICLUSTERthr = 0.1
    tool.PidName        = ""

    self.__tool = tool
    self.__log.debug( 'Chain     :%s', name )
    self.__log.debug( 'Threshold :%s', threshold )
    self.__log.debug( 'Pidname   :%s', sel )
    self.__log.debug( 'isoinfo   :%s', isoinfo )


  def chain(self):
    return self.__chain
  
  def pidname( self ):
    return self.__sel

  def etthr(self):
    return self.__threshold

  def isoInfo(self):
    return self.__isoinfo

  def tool(self):
    return self.__tool
  

  def etcut(self):
    self.__log.debug( 'Configure etcut' )
    self.tool().ETthr          = same( ( self.etthr() -  3 )*GeV, self.tool() )
    # No other cuts applied
    self.tool().dETACLUSTERthr = 9999.
    self.tool().dPHICLUSTERthr = 9999.


  #
  # Isolation and nominal cut
  #
  def isoCut(self):
    self.tool().RelEtConeCut = self.__caloIsolationCut[self.isoInfo()]
    self.nominal()
 

  def nominal(self):
    if not self.pidname() in self.__operation_points:
      self.__log.fatal("Bad selection name: %s" % self.pidname())
    self.tool().PidName = self.pidname()


  #
  # Compile the chain
  #
  def compile(self):

    if 'etcut' == self.pidname():
      self.etcut()

    elif self.isoInfo() and self.isoInfo() != '':

      if self.isoInfo() not in self.__caloIsolationCut.keys():
        self.__log.error('Isolation cut %s not defined!', self.isoInfo())
      self.__log.debug('Configuring Isolation cut %s with value %d',self.isoInfo(),self.__caloIsolationCut[self.isoInfo()])
      self.isoCut()

    else:
      self.nominal()

 
#
# Create and compile the photon hypo tool
#
def createPrecisionPhoton( name, info ):

    cpart = info['chainParts'][0]
    sel = cpart['addInfo'][0] if cpart['addInfo'] else cpart['IDinfo']
    etthr = int(cpart['threshold'])
    isoinfo = cpart['isoInfo']
    chain = info['chainName']


    hypo = TrigEgammaEmulationPrecisionPhotonHypoToolConfig( name, chain, etthr, sel, isoinfo)
    hypo.compile()
    return hypo.tool()
    