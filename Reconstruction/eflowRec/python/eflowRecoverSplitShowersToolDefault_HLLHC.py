# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging  # loads logger 

from eflowRec.eflowRecConf import eflowRecoverSplitShowersTool

class eflowRecoverSplitShowersToolDefault_HLLHC ( eflowRecoverSplitShowersTool )  :  # python way to define inheritance

    def __init__(self,name="eflowRecoverSplitShowersToolDefault_HLLHC"):

        eflowRecoverSplitShowersTool.__init__(self,name)

        mlog = logging.getLogger( 'eflowRecoverSplitShowersToolDefault_HLLHC::__init__ ' )
        mlog.info("entering")

        from eflowRec.eflowRecoverSplitShowersSetup import setup_eflowRecoverSplitShowers
        return setup_eflowRecoverSplitShowers(self,"EM_HLLHC",mlog)
