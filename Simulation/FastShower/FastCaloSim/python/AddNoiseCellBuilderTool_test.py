#
# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration.
#
# File: FastCaloSim/python/AddNoiseCellBuilderTool_test.py
# Author: scott snyder
# Date: Aug, 2019
# Brief: Test for AddNoiseCellBuilderTool.
#

from __future__ import print_function


from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaPython.PyAthenaComps import Alg, StatusCode
from AthenaCommon.Logging import logging
import ROOT

# address:
# subcalo, barec_or_posneg, sampling_or_module, region_or_dummy, eta, phi

def i32(x):
    if x >= (1<<31):
        x -= (1<<32)
    return x

LAREM = 0
LARHEC = 1
LARFCAL = 2
TILE = 3
LARHIGHGAIN = 0
LARMEDIUMGAIN = 1
LARLOWGAIN = 2
TILEHIGHHIGH = -11
TILEHIGHLOW = -12
cell_desc = {
    # larem barrel
    (LAREM, 1, 1, 0, 100, 10) : (1000,   LARHIGHGAIN,     1016.54),
    (LAREM, 1, 1, 0, 100, 11) : (50000,  LARMEDIUMGAIN,  49967.99),
    (LAREM, 1, 1, 0, 100, 12) : (300000, LARLOWGAIN,    300459.37),

    # larem endcap
    (LAREM, 2, 1, 2, 30, 10) : (1000,     LARHIGHGAIN,      997.80),
    (LAREM, 2, 1, 2, 30, 11) : (50000,    LARMEDIUMGAIN,  50016.62),
    (LAREM, 2, 1, 2, 30, 12) : (300000,   LARLOWGAIN,    300066.75),

    # larhec
    (LARHEC, 2, 1, 0, 3, 10) : (1000,     LARMEDIUMGAIN,    1114.68),
    (LARHEC, 2, 1, 0, 3, 11) : (50000,    LARMEDIUMGAIN,   50116.44),
    (LARHEC, 2, 1, 0, 3, 12) : (300000,   LARLOWGAIN,     300149.56),

    # fcal
    (LARFCAL, 2, 1, 0, 3, 10) : (1000,     LARHIGHGAIN,      964.61),
    (LARFCAL, 2, 1, 0, 3, 11) : (50000,    LARMEDIUMGAIN,  49740.66),
    (LARFCAL, 2, 1, 0, 3, 12) : (800000,   LARLOWGAIN,    801551.06),

    # tile
    (TILE, 1, 1, 10, 3, 1) : (1000,      TILEHIGHHIGH,    1011.98),
    (TILE, 1, 1, 11, 3, 1) : (50000,     TILEHIGHLOW,    49821.16),
    }

def make_calo_cells (mgr, idhelper, desc):
    ccc = ROOT.CaloCellContainer()
    for addr, (e, gain, exp) in desc.items():
        cellid = idhelper.cell_id (*addr)
        elt = mgr.get_element (cellid)
        assert elt is not None
        if addr[0] == TILE:
            cc = ROOT.TileCell (elt, e)
        else:
            cc = ROOT.CaloCell (elt, e, 0, 0, 0)
        ccc.push_back (cc)
        ROOT.SetOwnership (cc, False)
    ccc.order()
    ccc.updateCaloIterators()
    return ccc


def region (idhelper, cid):
    if idhelper.sub_calo(cid) == ROOT.CaloCell_ID.LARFCAL:
        return 0
    return idhelper.region(cid)


def compare_cells (detStore, ccc, exp_cells):
    exp_cells = exp_cells.copy()
    idhelper = detStore['CaloCell_ID']
    tilehelper = idhelper.tile_idHelper()

    for c in ccc:
        lcell = [i32(c.gain()), c.energy()]

        cid = c.ID()
        sub_calo = idhelper.sub_calo(cid)
        if sub_calo == 3:
            addr = (sub_calo,
                    tilehelper.section(cid),
                    tilehelper.side(cid),
                    tilehelper.module(cid),
                    tilehelper.tower(cid),
                    tilehelper.sampling(cid))
        else:
            addr = (sub_calo,
                    idhelper.pos_neg(cid),
                    idhelper.sampling(cid),
                    region(idhelper, cid),
                    idhelper.eta(cid),
                    idhelper.phi(cid))

        l = exp_cells.get (addr)
        if not l:
            print ('xxx unexpected cell', addr, lcell)
            assert 0
            continue
        l = l[1:]

        if (lcell[0] != l[0] or
            abs ((lcell[1] - l[1])/l[1]) > 1e-3):
            print ('xxx cell mismatch: ', addr, lcell, l)
            import sys
            sys.stdout.flush()
            assert 0
        del exp_cells[addr]

    for extra in exp_cells:
        print ('xxx unfound cell', extra)
        assert 0
    return


class TestAlg (Alg):
    def __init__ (self, name):
        Alg.__init__ (self, name)
        return

    def initialize (self):
        ROOT.ICaloCellMakerTool

        self.tool = ROOT.ToolHandle(ROOT.ICaloCellMakerTool)('AddNoiseCellBuilderTool')
        if not self.tool.retrieve():
            assert 0
        return StatusCode.Success


    def execute (self):
        ctx = self.getContext()
        log = logging.getLogger ('TestAlg')
        mgr = self.condStore['CaloDetDescrManager'].find (ctx.eventID())
        idhelper = self.detStore['CaloCell_ID']
        ccc = make_calo_cells (mgr, idhelper, cell_desc)
        assert self.tool.process (ccc, ctx).isSuccess()
        compare_cells (self.detStore, ccc, cell_desc)
        log.info ('finished')
        return StatusCode.Success


def testCfg (flags):
    result = ComponentAccumulator()

    from LArGeoAlgsNV.LArGMConfig import LArGMCfg
    from TileGeoModel.TileGMConfig import TileGMCfg
    result.merge(LArGMCfg(flags))
    result.merge(TileGMCfg(flags))

    from FastCaloSim.AddNoiseCellBuilderToolConfig import AddNoiseCellBuilderToolCfg
    acc = AddNoiseCellBuilderToolCfg (flags)
    acc.popPrivateTools()
    result.merge (acc)

    result.addEventAlgo (TestAlg ('TestAlg'))
    return result


from AthenaConfiguration.AllConfigFlags import initConfigFlags
flags = initConfigFlags()
from AthenaConfiguration.TestDefaults import defaultTestFiles

flags.Input.Files = defaultTestFiles.HITS_RUN2
flags.Input.TimeStamp = 1000
flags.Detector.GeometryLAr = True
flags.Detector.GeometryTile = True
flags.needFlagsCategory('Tile')
flags.needFlagsCategory('LAr')

flags.lock()
from AthenaConfiguration.MainServicesConfig import MainServicesCfg 
acc = MainServicesCfg (flags)

from McEventSelector.McEventSelectorConfig import McEventSelectorCfg
acc.merge (McEventSelectorCfg (flags))

acc.merge (testCfg (flags))
acc.run(1)
