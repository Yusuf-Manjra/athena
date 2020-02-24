#
# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
# 

def setupFilterMonitoring( filterAlg ):    
    from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool
    monTool = GenericMonitoringTool('MonTool')
    
    inputKeys = filterAlg.Input

    monTool.HistPath="HLTFramework/Filters"
    monTool.defineHistogram( 'name,stat;'+filterAlg.name(),  path='EXPERT', type='TH2I',
                             title='Input activity fraction;;presence',
                             xbins=len(inputKeys), xmin=0, xmax=len(inputKeys)+2, xlabels=['exec', 'anyvalid']+inputKeys,
                             ybins=2, ymin=-0.5, ymax=1.5, ylabels=['no', 'yes'] )

    filterAlg.MonTool = monTool

def TriggerSummaryAlg( name ):
    from DecisionHandling.DecisionHandlingConf import TriggerSummaryAlg as Alg
    alg = Alg( name )
    from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool
    monTool = GenericMonitoringTool('MonTool', HistPath='HLTFramework/'+name)
    monTool.defineHistogram('TIME_SinceEventStart', path='EXPERT', type='TH1F',
                                   title='Time since beginning of event processing;time [ms]',
                                   xbins=100, xmin=0, xmax=3.5e3   )
    alg.MonTool = monTool
    return alg
