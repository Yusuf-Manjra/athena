from TrigT2MinBias.TrigT2MinBiasConf import MbtsFexMT
from TrigT2MinBias.TrigT2MinBiasMonitoringMT import MbtsFexMTMonitoring

class MbtsFexMT( MbtsFexMT ):
    __slots__ = []
    def __init__ (self, name ):
        super(MbtsFexMT, self).__init__(name)
        self.monTool += [ MbtsFexMTMonitoring()]
