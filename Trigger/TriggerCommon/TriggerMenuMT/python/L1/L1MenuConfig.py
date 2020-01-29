# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

import re
from collections import defaultdict as ddict

from TriggerJobOpts.TriggerFlags import TriggerFlags
from AthenaCommon.Logging import logging

from .Base.L1MenuFlags import L1MenuFlags
from .Base.Limits import Limits
from .Base.L1Menu import L1Menu
from .Base.Thresholds import TopoThreshold
from .Base.TopoAlgorithms import AlgCategory
from .Base.L1Menu2JSON import L1MenuJSONConverter
from .Config.TriggerTypeDef import TT

"""
L1MenuConfig is responsible for building the L1 Menu

Building a menu happens in two stages

1) Configuration objects are defined in the directory TriggerMenuMT/Config/ in *Def.py files
  * Items are definded in Config/ItemDef.py
  * Topo algorithms are define in files TopoAlgoDef*.py, with separate files for
    - multiplicity counting algorithms on the Run 3 topo boards 
    - topological selection algorithms on the Run 3 topo boards
    - topological selection algorithms on the MUCTPI board
    - topological selection algorithms on the Run 2 topo boards (legacy boards)
  * Thresholds are defined in files ThresholdDef.py
"""

log = logging.getLogger("Menu.L1.L1MenuConfig")

class L1MenuConfig(object):

    def __init__(self, menuName = None, outputFile = None , inputFile = None ):

        self.menuFullName   = menuName if menuName else TriggerFlags.triggerMenuSetup()
        self.menuName       = self._getMenuBaseName(self.menuFullName)
        self.inputFile      = inputFile
        self.outputFile     = outputFile if outputFile else TriggerFlags.inputLVL1configFile()
        self.l1menuFromFile = (self.inputFile is not None)
        self.generated      = False

        # all registered items
        self.registeredItems = {}
        
        # all registered thresholds
        self._definedThresholds = {}
        self._definedThresholdsStats = { "muon": 0, "calo": 0, "nim": 0, "legacy": 0, 
                                         AlgCategory.TOPO : 0, AlgCategory.MUCTPI : 0, AlgCategory.LEGACY : 0 }

        # all registered topo algos
        self._definedTopoAlgos = {}
        for cat in AlgCategory.getAllCategories():
            self._definedTopoAlgos[cat] = {}

        # menu
        self.l1menu = L1Menu(self.menuName)
        self.l1menu.setBunchGroupSplitting() # I like this much more, but let's see what the menu group says

        if not self._checkMenuExistence():
            log.warning("Generating L1 menu %s is not possible", self.menuName)
        else:
            log.info("=== Generating L1 menu %s ===", self.menuName)
            self._generate()

    def _generate(self):

        log.info("=== Reading definition of algorithms, thresholds, and items ===")

        self._registerDefinedConfigurationObjects() # defines and registers all items, thresholds, ...

        log.info("=== Reading definition of menu ===")

        # Build menu
        self._importMenuDefinition() # puts thresholds and items names for the menu into the LVL1Flags

        self._extendInformation()

        self._generateTopoMenu()

        self._generateMenu()

        self.generated = True
        

    def thresholdExists(self,thrName):
        return thrName in self._definedThresholds

    def getDefinedThreshold(self, name):
        try:
            return self._definedThresholds[name]
        except KeyError:
            raise RuntimeError("Threshold %s is not defined" % name)

    def getDefinedThresholds(self):
        return self._definedThresholds.values()



    ###
    # registration of available components
    ###

    ## topo algorithms
    def registerTopoAlgo(self, algo):
        """ Add a L1Topo algo to the set of algos which are registered for further use"""

        if self.currentAlgoDef == "multi":
            if algo.name in self._definedTopoAlgos[AlgCategory.MULTI]:
                raise RuntimeError('L1Topo multiplicity algo %s is already registered as such' % algo.name)
            self._definedTopoAlgos[AlgCategory.MULTI][algo.name] = algo
            log.debug("Added in the multiplicity topo algo list: {0}, ID:{1}" .format(algo.name,algo.algoId))

        elif self.currentAlgoDef == "muctpi":
            if algo.name in self._definedTopoAlgos[AlgCategory.MUCTPI]:
                raise RuntimeError('L1Topo MuCTPi algo %s is already registered as such' % algo.name)
            self._definedTopoAlgos[AlgCategory.MUCTPI][algo.name] = algo
            log.debug("Added in the MuCTPi topo algo list: {0}, ID:{1}" .format(algo.name,algo.algoId))

        elif self.currentAlgoDef == "topo":
            if algo.name in self._definedTopoAlgos[AlgCategory.TOPO]:
                raise RuntimeError('L1Topo algo %s is already registered as such' % algo.name)
            self._definedTopoAlgos[AlgCategory.TOPO][algo.name] = algo
            log.debug("Added in the topo algo list: {0}, ID:{1}" .format(algo.name,algo.algoId))

        elif self.currentAlgoDef == "legacy":
            if algo.name in self._definedTopoAlgos[AlgCategory.LEGACY]:
                raise RuntimeError('L1Topo legacy algo %s is already registered as such' % algo.name)
            self._definedTopoAlgos[AlgCategory.LEGACY][algo.name] = algo
            log.debug("Added in the legacy topo algo list: {0}, ID:{1}" .format(algo.name,algo.algoId))

        return algo



    def registerThreshold(self, thr):
        """
        Add externally created L1 threshold to the list of available thresholds.
        """
        if thr.run!=0:
            self._addThrToRegistry(thr)
        else:
            raise RuntimeError("For threshold %s the run (=2 or 3) was not set!" % thr.name)

    def _addThrToRegistry(self, thr):
        if self.thresholdExists(thr.name):
            raise RuntimeError("LVL1 threshold of name '%s' already defined, need to abort", thr.name)

        self._definedThresholds[thr.name] = thr

        # collect stats
        isTopo = isinstance(thr, TopoThreshold)
        if isTopo:
            self._definedThresholdsStats[thr.algCategory] += 1
            return

        isNIM = thr.ttype in ['NIM','CALREQ','MBTSSI', 'MBTS', 'LUCID', 'BCM', 'BCMCMB', 'ZDC', 'ALFA', 'BPTX', 'ZB']
        if thr.isLegacy():
            self._definedThresholdsStats["legacy"] += 1
        elif isNIM:
            self._definedThresholdsStats["nim"] += 1
        elif thr.ttype == 'MU':
            self._definedThresholdsStats["muon"] += 1
        else:
            self._definedThresholdsStats["calo"] += 1




    def _registerTopoOutputsAsThresholds(self):
        """
        Add all L1Topo triggers that are part of the menu as allowed input to the menu
        """

        self._topoTriggers = {}

        # for all topo algorithm categories the outputs (sometimes multiple) are defined as thresholds
        for cat in AlgCategory.getAllCategories():
            outputLines = []        
            for algo in self._definedTopoAlgos[cat].values():
                outputLines += algo.outputs if (type(algo.outputs) == list) else [ algo.outputs ]
            self._topoTriggers[cat] = sorted(outputLines)
            log.info("... found %i topo triggerlines (source: %s)", len(self._topoTriggers[cat]), cat )
            log.debug("%r", self._topoTriggers[cat])


        for cat in [AlgCategory.TOPO, AlgCategory.MUCTPI]:
            for topoLineName in self._topoTriggers[cat]:
                topoThrName = cat.prefix + topoLineName
                TopoThreshold( name = topoThrName, algCategory = cat )

        # multibit topo triggers are only supported for legacy central muon trigger
        multibitTopoTriggers = set()
        multibitPattern = re.compile(r"(?P<line>.*)\[(?P<bit>\d+)\]")
        for topoLineName in self._topoTriggers[AlgCategory.LEGACY]:
            m = multibitPattern.match(topoLineName) # tries to match "trigger[bit]"
            if m:
                topoThrName = AlgCategory.LEGACY.prefix + m.groupdict()['line']
                multibitTopoTriggers.add( topoThrName )
            else:
                topoThrName = AlgCategory.LEGACY.prefix + topoLineName # "topo", "muctpi", "legacy"
                TopoThreshold( name = topoThrName, algCategory = AlgCategory.LEGACY )

        # create thresholds from topo-multibit 
        for topoThrName in multibitTopoTriggers:  # ( 'MULT-CMU4ab', ...)
            TopoThreshold( name = topoThrName, algCategory = AlgCategory.LEGACY )



    ## Items
    def registerItem(self, name, item):
        """ Adds a LVL1 item to the set of items which are registered for further use"""
        if name in self.registeredItems:
            log.error('LVL1 item %s is already registered with ctpid=%d',
                      name, int(self.registeredItems[name].ctpid))
        else:
            self.registeredItems[name] = item

    def getRegisteredItem(self, name):
        if name in self.registeredItems:
            return self.registeredItems[name]
        return None


    def writeJSON(self, outputFile, destdir="./", screenprint = False):
        if self.generated:
            outputFile = destdir.rstrip('/') + '/' + outputFile
            L1MenuJSONConverter(l1menu = self.l1menu, outputFile = outputFile).writeJSON(pretty=True)
            return outputFile
        else:
            log.warning("No menu was generated, can not create json file")
            return None

    def menuToLoad(self,silent=False):
        menuToLoad = self.menuName
        if menuToLoad == "LS2_v1" or menuToLoad == "pp_run3_v1" in menuToLoad:
            menuToLoad = "MC_pp_v8"
            if not silent:
                log.info("Menu LS2_v1/*_pp_run3_v1 was requested but is not available yet. Will load MC_pp_v8 instead. This is a TEMPORARY meassure")
        return menuToLoad

    def _checkMenuExistence(self):
        menuToLoad = self.menuToLoad(silent=True)
        from PyUtils.moduleExists import moduleExists
        modname = 'TriggerMenuMT.L1.Menu.Menu_%s' % menuToLoad
        if not moduleExists (modname):
            log.warning("No L1 menu available for %s", self.menuName )
            return False

        return True


    def _importMenuDefinition(self):
        """
        Defines the list if item and threshold names that will be in the menu
        Calls defineMenu() of the correct module 'Menu_<menuname>.py'
        Menu.defineMenu() defines the menu via L1MenuFlags "items", "thresholds", 
        """

        # we apply a hack here. menu group is working on LS2_v1, until ready we will use MC_pp_v8
        menuToLoad = self.menuToLoad()
        log.info("Reading TriggerMenuMT.Menu.Menu_%s", menuToLoad)
        menumodule = __import__('TriggerMenuMT.L1.Menu.Menu_%s' % menuToLoad, globals(), locals(), ['defineMenu'], 0)
        menumodule.defineMenu()
        log.info("... L1 menu '%s' contains:", menuToLoad)

        log.info("Reading TriggerMenuMT.Menu.Menu_%s_inputs", menuToLoad)
        topomenumodule = __import__('TriggerMenuMT.L1.Menu.Menu_%s_inputs' % menuToLoad, globals(), locals(), ['defineMenu'], 0)
        topomenumodule.defineInputsMenu()
        algoCount = 0
        for boardName, boardDef in L1MenuFlags.boards().items():
            if "connectors" in boardDef:
                for c in boardDef["connectors"]:
                    if "thresholds" in c:
                        algoCount += len(c["thresholds"])
                    else:
                        for t in c["algorithmGroups"]:
                            algoCount += len(t["algorithms"])
        log.info("... L1Topo menu '%s' contains %i algorithms", menuToLoad, algoCount)

        try:
            log.info("Reading TriggerMenuMT.Menu.Menu_%s_inputs_legacy", menuToLoad)
            legacymenumodule = __import__('TriggerMenuMT.L1.Menu.Menu_%s_inputs_legacy' % menuToLoad, globals(), locals(), ['defineMenu'], 0)
            legacymenumodule.defineLegacyInputsMenu()
            log.info("... L1 legacy menu %s contains %i legacy boards (%s)", menuToLoad, len(L1MenuFlags.legacyBoards()), ', '.join(L1MenuFlags.legacyBoards().keys()))
        except ImportError:
            log.info("==> No menu defining the legacy inputs was found, will assume this intended")


    def _registerDefinedConfigurationObjects(self):
        """
        Registers the list if items and thresholds that could be used in the menu
        Calls registerItem() of the correct module 'ItemDef.py'
        """
        from .Base.Items import MenuItem
        MenuItem.setMenuConfig(self) # from now on all newly created MenuItems are automatically registered here
        from .Base.Thresholds import Threshold
        Threshold.setMenuConfig(self) # from now on all newly created MenuItems are automatically registered here
        
        log.info("Reading TriggerMenuMT.Config.TopoAlgoDef")
        self.currentAlgoDef = "topo"
        from .Config.TopoAlgoDef import TopoAlgoDef
        TopoAlgoDef.registerTopoAlgos(self)
        log.info("... registered %i defined topo algos for the new topo boards", len(self._definedTopoAlgos[AlgCategory.TOPO]))

        log.info("Reading TriggerMenuMT.Config.TopoAlgoDefMuctpi")
        self.currentAlgoDef = "muctpi"
        from .Config.TopoAlgoDefMuctpi import TopoAlgoDefMuctpi
        TopoAlgoDefMuctpi.registerTopoAlgos(self)
        log.info("... registered %i defined topo algos for the MuCTPi", len(self._definedTopoAlgos[AlgCategory.MUCTPI]))

        log.info("Reading TriggerMenuMT.Config.TopoMultiplicityAlgoDef")
        self.currentAlgoDef = "multi"
        from .Config.TopoMultiplicityAlgoDef import TopoMultiplicityAlgoDef
        TopoMultiplicityAlgoDef.registerTopoAlgos(self)
        log.info("... registered %i defined topo multiplicity algos for the new topo boards", len(self._definedTopoAlgos[AlgCategory.MULTI]))

        log.info("Reading TriggerMenuMT.Config.TopoAlgoDefLegacy")
        self.currentAlgoDef = "legacy"
        from .Config.TopoAlgoDefLegacy import TopoAlgoDefLegacy
        TopoAlgoDefLegacy.registerTopoAlgos(self)
        log.info("... registered %i defined topo algos for the legacy topo boards", len(self._definedTopoAlgos[AlgCategory.LEGACY]))

        log.info("Reading TriggerMenuMT.Config.ThreholdDef")
        from .Config.ThresholdDef import ThresholdDef
        ThresholdDef.registerThresholds(self)
        log.info("... registered %i calo thresholds", self._definedThresholdsStats["calo"])
        log.info("... registered %i muon thresholds", self._definedThresholdsStats["muon"])
        log.info("... registered %i nim thresholds", self._definedThresholdsStats["nim"])

        log.info("Reading TriggerMenuMT.Config.ThreholdDefLegacy")
        from .Config.ThresholdDefLegacy import ThresholdDefLegacy
        ThresholdDefLegacy.registerThresholds(self)
        log.info("... registered %i legacy calo thresholds", self._definedThresholdsStats["legacy"])

        log.info("Turning topo algo outputs into thresholds (except multiplicity counters)")
        self._registerTopoOutputsAsThresholds()
        log.info("... registered %i topo thresholds", self._definedThresholdsStats[AlgCategory.TOPO])
        log.info("... registered %i muctpi topo thresholds", self._definedThresholdsStats[AlgCategory.MUCTPI])
        log.info("... registered %i legacy topo thresholds", self._definedThresholdsStats[AlgCategory.LEGACY])

        log.info("Reading TriggerMenuMT.Config.ItemDef")
        from .Config.ItemDef import ItemDef
        ItemDef.registerItems(self)
        log.info("... registered %i defined items", len(self.registeredItems))


    def _getTopoAlgo(self, algoName, category):
        if algoName in self._definedTopoAlgos[category]:
            return self._definedTopoAlgos[category][algoName]
        raise RuntimeError("Algorithm of name %s is not defined in category %s" % (algoName, category) )


    def _getSortingAlgoThatProvides(self, input):
        """
        returns a list of all sorting algorithms that are needed to
        produce the required output. A missing input will raise a
        runtime exception
        """
        sortingAlgs = []
        for name, alg in self._definedTopoAlgos[AlgCategory.TOPO].items():  # TODO: extend to legacy topo
            if type(alg.outputs)==list:
                foundOutput = (input in alg.outputs)
            else:
                foundOutput = (input == alg.outputs)
            if foundOutput:
                sortingAlgs += [alg]

        if len(sortingAlgs)==0:
            raise RuntimeError("No sorting algorithm is providing this output: %s" % input)
        if len(sortingAlgs)>1:
            raise RuntimeError("More than one sorting algorithm is providing this output: %s. Here the list: %s" % (input, ', '.join(sortingAlgs)))

        return sortingAlgs[0]

 
    def _extendInformation(self):
        """
        this function is called first after the menu definition is imported
        it can be used to update some of the information or to perform initial checks
        """
        allBoards = (list(L1MenuFlags.boards().items()) + list(L1MenuFlags.legacyBoards().items()))
        # set boardName in each connector
        for (boardName, boardDef) in allBoards:
            boardDef["name"] = boardName
            if "connectors" in boardDef:
                for connDef in boardDef["connectors"]:
                    connDef["board"] = boardName
            else:
                for connDef in boardDef["inputConnectors"]:
                    connDef["board"] = boardName
        

    def _generateTopoMenu(self):

        allBoards = (list(L1MenuFlags.boards().items()) + list(L1MenuFlags.legacyBoards().items()))
        allBoardsWithTopo = filter( lambda n : ('topo' in n[0].lower() or 'muctpi' in n[0].lower()), allBoards )

        #
        # Add the topo thresholds to the menu
        #
        nAlgos = 0
        nLines = 0
        for (boardName, boardDef) in allBoardsWithTopo:
            currentTopoCategory = AlgCategory.getCategoryFromBoardName(boardName)
            for connDef in boardDef["connectors"]:
                if connDef["format"] == 'multiplicity': # multiplicity algorithms don't define thresholds
                    continue
                for algGrp in connDef["algorithmGroups"]:
                    for topodef in algGrp["algorithms"]:
                        nAlgos += 1
                        nLines += len(topodef.outputlines)
                        for lineName in topodef.outputlines:
                            thrName = currentTopoCategory.prefix + lineName
                            thr = self.getDefinedThreshold(thrName) # threshold has to be defined
                            self.l1menu.addThreshold( thr )
        log.info("Generating topo menu using %i topo algorithms with %i trigger lines", nAlgos, nLines )

        #
        # Add the topo algorithms to the menu
        #

        # collect sorting algorithms from all decision algorithms (their inputs)
        # they need to be kept separated by source at this point
        allRequiredSortedInputs = {
            AlgCategory.TOPO : set(),
            AlgCategory.MUCTPI : set(), # TODO: think about MUCTP sorting input 
            AlgCategory.LEGACY : set()
        }

        for (boardName, boardDef) in allBoardsWithTopo:
            for connDef in boardDef["connectors"]:
                if ('muctpi' in boardName.lower()) and (connDef["format"]=='multiplicity'):
                    # muctpi doesn't need multiplicity algorithms defined
                    continue

                currentTopoCategory = AlgCategory.getCategoryFromBoardName(boardName)
                if currentTopoCategory == AlgCategory.TOPO and connDef["format"] == 'multiplicity':
                    currentTopoCategory = AlgCategory.MULTI
                algoNames = []
                if connDef["format"] == 'multiplicity':
                    for thrName in connDef["thresholds"]:
                        algoname = "Mult_" + thrName
                        algoNames += [ algoname ]
                elif connDef["format"] == 'topological':
                    for algGrp in connDef["algorithmGroups"]:
                        for topodef in algGrp["algorithms"]:
                            algoNames += [ topodef.algoname ]

                for algoName in algoNames:
                    algo = self._getTopoAlgo(algoName, currentTopoCategory)

                    # add the decision algorithms to the menu
                    self.l1menu.addTopoAlgo( algo, category = currentTopoCategory )

                    # remember the inputs
                    if algo.isDecisionAlg():
                        allRequiredSortedInputs[currentTopoCategory].update( algo.inputs )

        # now also add the sorting algorithms to the menu
        for cat in allRequiredSortedInputs:
            for input in allRequiredSortedInputs[cat]:
                sortingAlgo = self._getSortingAlgoThatProvides(input)
                self.l1menu.addTopoAlgo( sortingAlgo, category = cat )





    def _generateMenu(self):

        if len(self.l1menu.items) > 0:
            log.info("L1MenuConfig.generate() has already been called. Will ignore")
            return

        """
        Generates the menu structure from the list of item and threshold names in the L1MenuFlags
        """

        # ------------------
        # Bunchgroups
        # ------------------

        self.l1menu.ctp.bunchGroupSet.setDefaultBunchGroupDefinition()


        # ------------------
        # Items
        # ------------------

        # build list of items for the menu from the list of requested names
        itemsForMenu = []
        for itemName in L1MenuFlags.items():
            registeredItem = self.getRegisteredItem(itemName)
            if registeredItem is None:
                msg = "LVL1 item '%s' has not been defined in LVL1Menu/ItemDef.py" % itemName
                log.error(msg)
                raise RuntimeError(msg)

            if itemName in L1MenuFlags.CtpIdMap():
                newCTPID = L1MenuFlags.CtpIdMap()[itemName]
                registeredItem.setCtpid(newCTPID)

            itemsForMenu += [ registeredItem ]


        # CTP IDs available for assignment
        assigned_ctpids = set([item.ctpid for item in itemsForMenu])
        available_ctpids = sorted( list( set(range(Limits.MaxTrigItems)) - assigned_ctpids ), reverse=True )

        # add the items to the menu
        for item in itemsForMenu:
            # set the physics bit
            if not item.name.startswith('L1_CALREQ'):
                item.setTriggerType( item.trigger_type | TT.phys )
            # assign ctp IDs to items that don't have one
            if item.ctpid == -1:
                item.setCtpid( available_ctpids.pop() )
            # add the items into the menu
            self.l1menu.addItem( item )


        # ------------------
        # Thresholds
        # ------------------

        allBoards = (list(L1MenuFlags.boards().items()) + list(L1MenuFlags.legacyBoards().items()))
        
        list_of_undefined_thresholds = []
        # new thresholds
        for (boardName, boardDef) in L1MenuFlags.boards().items():
            if boardName.startswith("Ctpin"):
                continue
            for connDef in boardDef["connectors"]:
                if connDef["format"] != "multiplicity":
                    continue
                for thrName in connDef["thresholds"]:
                    if type(thrName) == tuple:
                        (thrName, _) = thrName
                    if thrName is None or thrName in self.l1menu.thresholds:
                        continue
                    threshold = self.getDefinedThreshold(thrName)
                    if threshold is None:
                        log.error('Threshold %s is listed in menu but not defined', thrName )
                        list_of_undefined_thresholds += [ thrName ]
                    else:
                        self.l1menu.addThreshold( threshold )

        # ctpin thresholds
        for (boardName, boardDef) in allBoards:
            if not boardName.startswith("Ctpin"):
                continue
            for connDef in boardDef["connectors"]:
                for entry in connDef["thresholds"]:
                    if type(entry) == dict:
                        # section that defines topo legacy thresholds 
                        thrNames = sum([x.outputlines for x in entry["algorithms"]],[])
                    elif type(entry) == str:
                        thrNames = [ entry ]
                    elif type(entry) == tuple:
                        thrNames = [ entry[0] ]

                    for thrName in thrNames:
                        if thrName is None or thrName in self.l1menu.thresholds:
                            continue
                        threshold = self.getDefinedThreshold(thrName)
                        if threshold is None:
                            log.error('Threshold %s is listed in menu but not defined', thrName )
                            list_of_undefined_thresholds += [ thrName ]
                        else:
                            self.l1menu.addThreshold( threshold )
                try:
                    zbThrName = connDef["zeroBias"]
                    zbThr = self.getDefinedThreshold(zbThrName)
                    if zbThr is None:
                        log.error('Zero bias threshold %s is listed in menu but not defined', zbThrName )
                        list_of_undefined_thresholds += [ zbThrName ]
                    else:
                        self.l1menu.addThreshold( zbThr )
                except KeyError:
                    pass

        if len(list_of_undefined_thresholds)>0:
            raise RuntimeError("Found undefined threshold in menu %s, please add these thresholds to l1menu/ThresholdDef.py: %s" % \
                               (self.l1menu.menuName, ', '.join(list_of_undefined_thresholds)) )


        # ------------------
        # Connectors
        # ------------------

        for (boardName, boardDef) in allBoards:
            for connDef in boardDef["connectors"]:
                self.l1menu.addConnector( connDef )

        # ------------------
        # Boards
        # ------------------

        for (boardName, boardDef) in allBoards:
            self.l1menu.addBoard(boardDef)
            
        # ------------------
        # Mark items legacy
        # ------------------
        legacyThresholdsSet = set() # determine from connectors
        for conn in self.l1menu.connectors:
            if not conn.isLegacy():
                continue
            legacyThresholdsSet.update(conn.triggerThresholds())
        for item in self.l1menu.items:
            item.markLegacy(legacyThresholdsSet)
            

        # assign mapping to thresholds according to their use in the menu
        self.mapThresholds()

        # update the prescales that are not 1
        #self.updateItemPrescales()

        # set the ctp monitoring (only now after the menu is defined)
        self.l1menu.setupCTPMonitoring()

        # final consistency check
        self.l1menu.check()




    def mapThresholds(self):
        """
        Set the correct mapping of thresholds according to the
        order it was given in L1MenuFlags.thresholds list. That list
        is usually defined in the setupMenu function of each menu

        NIM and CALREQ types are not remapped !!
        """

        existingMappings = ddict(set)
        for thr in self.l1menu.thresholds:
            if thr.mapping<0:
                continue
            existingMappings[thr.ttype].add(thr.mapping)

        nextFreeMapping = ddict(lambda: 0)
        for k in  existingMappings:
            nextFreeMapping[k] = 0

        for thr in self.l1menu.thresholds():
            if thr.mapping < 0:
                while nextFreeMapping[thr.ttype] in existingMappings[thr.ttype]:
                    nextFreeMapping[thr.ttype] += 1
                log.debug('Setting mapping of threshold %s as %i', thr, nextFreeMapping[thr.ttype])
                thr.mapping = nextFreeMapping[thr.ttype]
                nextFreeMapping[thr.ttype] += 1

            
    def updateItemPrescales(self):
        for (it_name, ps) in L1MenuFlags.prescales().items():
            item = self.l1menu.getItem(it_name)
            if item:
                item.prescale = ps
            else:
                log.warning('Cannot find item %s to set the prescale', it_name )


    def configureCTP(self):
        self.l1menu.ctp.addMonCounters()


    # remove prescale suffixes
    def _getMenuBaseName(self, menuName):
        pattern = re.compile(r'_v\d+|DC14')
        patternPos = pattern.search(menuName)
        if patternPos:
            menuName=menuName[:patternPos.end()]
        else:
            log.info('Can\'t find pattern to shorten menu name, either non-existent in name or not implemented.')
        return menuName         

