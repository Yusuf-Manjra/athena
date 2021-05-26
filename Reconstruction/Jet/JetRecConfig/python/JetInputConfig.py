# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
"""
# JetInputConfig: A helper module providing function to setup algorithms
# in charge of preparing input sources to jets (ex: EventDensity algo, track
# or truth selection,...)
#
# Author: P-A Delsart                                              #
"""


from AthenaConfiguration.ComponentFactory import CompFactory

def _buildJetAlgForInput(suffix, tools ):
    jetalg = CompFactory.JetAlgorithm("jetalg_"+suffix,
                                      Tools = tools,
    )
    return jetalg

def buildJetSelectedTracks( parentjetdef, inputspec ):
    from JetRecTools import JetRecToolsConfig
    # Jet track selection
    t = JetRecToolsConfig.getTrackSelTool(doWriteTracks=True,
                                          cutLevel="NoCut", minPt=500) 
    return _buildJetAlgForInput("JetSelectedTrack",
                                tools = [ t ]
                                )

def buildJetTrackUsedInFitDeco( parentjetdef, inputspec ):
    from JetRecTools import JetRecToolsConfig
    # Jet track used-in-fit decoration
    return _buildJetAlgForInput("JetUsedInFitDeco",
                                tools = [ JetRecToolsConfig.getTrackUsedInFitTool() ]
    )

def buildJetTrackVertexAssoc( parentjetdef, inputspec ):
    from JetRecTools import JetRecToolsConfig
    # Jet track TTVA
    return _buildJetAlgForInput("JetTVA",
                                tools = [ JetRecToolsConfig.getTrackVertexAssocTool() ]
    )
    


    
def buildJetInputTruth(parentjetdef, truthmod):
    truthmod = truthmod or ""
    from ParticleJetTools.ParticleJetToolsConfig import getCopyTruthJetParticles
    return _buildJetAlgForInput("truthpartcopy_"+truthmod,
                                tools = [ getCopyTruthJetParticles(truthmod) ]
    )

def buildLabelledTruth(parentjetdef, truthmod):
    from ParticleJetTools.ParticleJetToolsConfig import getCopyTruthLabelParticles
    tool = getCopyTruthLabelParticles(truthmod)
    return _buildJetAlgForInput("truthlabelcopy_"+truthmod,
                                tools = [ tool ]
    )



########################################################################
def getEventShapeName( parentjetdef, inputspec):
    """ Get the name of the event shape container for a given event shape alg """
    nameprefix = inputspec or ""
    label = parentjetdef.inputdef.label
    return nameprefix+"Kt4"+label+"EventShape"


def buildEventShapeAlg( parentjetdef, inputspec ):
    """Function producing an EventShapeAlg to calculate
     median energy density for pileup correction"""
    
    rhokey = getEventShapeName(parentjetdef, inputspec)
    nameprefix = inputspec or ""
    label = parentjetdef.inputdef.label
    rhotoolname = "EventDensity_"+nameprefix+"Kt4"+label
    
    rhotool = CompFactory.EventDensityTool(rhotoolname)
    rhotool.InputContainer = "PseudoJet"+label # same as in PseudoJet algs
    rhotool.OutputContainer = rhokey
    
    eventshapealg = CompFactory.EventDensityAthAlg("{0}{1}Alg".format(nameprefix,rhotoolname))
    eventshapealg.EventDensityTool = rhotool

    return eventshapealg

