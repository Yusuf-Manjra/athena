# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

# System import(s):

class AnaAlgorithmMeta ( object ):
    """the meta-information for a single algorithm inside AnaAlgSequence

    There is a fair amount of meta-information we store for each
    algorithm.  Originally we had stored each piece of
    meta-information in a separate array (with each entry in each
    array corresponding to a different algorithm).  Though now this is
    changed to a single array with each entry being a meta-object for
    a separate algorithm.
    """

    def __init__( self, stageName, affectingSystematics, inputPropName, outputPropName, selectionDecorNames, selectionDecorCount ):

        if not stageName in self.allowedStageNames() :
            raise ValueError ('unknown stage name ' + stageName + ' allowed stage names are ' + ', '.join(self.allowedStageNames()))

        self.stageName = stageName

        if len (selectionDecorNames) != len (selectionDecorCount) :
            raise ValueError ("selectionDecorNames and selectionDecorCount don't have the same length: " + str (len (selectionDecorNames)) + " " + str (len (selectionDecorCount)));
        self.selectionDecorNames = selectionDecorNames
        self.selectionDecorCount = selectionDecorCount

        if isinstance( inputPropName, dict ):
            self.inputPropName = inputPropName
        else:
            if inputPropName:
                self.inputPropName = { "default" : inputPropName }
            else:
                self.inputPropName = None
                pass
            pass

        if isinstance( outputPropName, dict ):
            self.outputPropName = outputPropName
        else:
            if outputPropName:
                self.outputPropName = { "default" : outputPropName }
            else:
                self.outputPropName = None
                pass
            pass

        if isinstance( affectingSystematics, dict ):
            self.affectingSystematics = affectingSystematics
        else:
            if affectingSystematics:
                self.affectingSystematics = { "default" : affectingSystematics }
            else:
                self.affectingSystematics = None
                pass
            pass
        pass

    @staticmethod
    def allowedStageNames():
        return ["calibration", "selection", "efficiency", "undefined"]

    pass
