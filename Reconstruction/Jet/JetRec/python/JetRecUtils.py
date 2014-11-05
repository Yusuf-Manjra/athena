# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.AppMgr import ToolSvc

def retrieveAODList():
    from JetRec.JetRecFlags import jetFlags
    from RecExConfig.RecFlags import rec

    if rec.doESD():
        return jetFlags.jetAODList()

    # else : just copy what's in input.
    from RecExConfig.ObjKeyStore import objKeyStore

    inputcontent = objKeyStore['inputFile'].list()
    typeToSave = [ 'xAOD::JetContainer_v1', 'xAOD::JetAuxContainer_v1', 'xAOD::JetTrigAuxContainer_v1' , 'xAOD::EventShape_v1', 'xAOD::EventShapeAuxInfo_v1' ]

    def saveThisObject(o):
        # return True if o is of a desired type
        return any( o.startswith( typ ) for typ in typeToSave )

    esdjets = [ o for o in inputcontent if saveThisObject(o) ]

    return esdjets

def buildJetAlgName(finder, mainParam):
    return finder + str(int(mainParam*10))

def buildJetContName(finder, mainParam, input):
    return buildJetAlgName(finder, mainParam) +input+"Jets" # could be more elaborated...

def interpretJetName(jetcollName,  finder = None,input=None, mainParam=None):
    # first step : guess the finder, input , mainParam, if needed
    if finder is None:
        for a in [ 'AntiKt','CamKt','Kt', 'Cone','SISCone','CMSCone']:
            if jetcollName.startswith(a):
                finder = a
                break
        if finder is None:
            print "Error could not guess jet finder type in ",jetcollName
            return 

    if input is None:
        for i in ['LCTopo','Tower','Topo', "Truth", "TrackZ"]:
            if i in jetcollName:
                input = i
                if i== "Tower":
                    if 'Ghost' in jetcollName:
                        input = 'Tower'
                    else:
                        input = "TopoTower"
                break
        if input is None:
            print "Error could not guess input type in ",jetcollName            
            return
        
    if mainParam is None:
        # get the 2 chars following finder :
        mp = jetcollName[len(finder):len(finder)+2]
        mp = mp[0] if not mp[1] in '0123456789' else mp
        try :
            mainParam = float(mp)/10.
        except ValueError :
            print "Error could not guess main parameter in ",jetcollName
            return

    return finder, mainParam, input
