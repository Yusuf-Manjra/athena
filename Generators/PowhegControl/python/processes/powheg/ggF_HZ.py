# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import Logging
from ..powheg_V2 import PowhegV2

## Get handle to Athena logging
logger = Logging.logging.getLogger("PowhegControl")


class ggF_HZ(PowhegV2):
    """! Default Powheg configuration for gluon-fusion Higgs boson plus Z-boson production with quark mass effects.

    Create a configurable object with all applicable Powheg options.

    @author James Robinson  <james.robinson@cern.ch>
    """

    def __init__(self, base_directory, **kwargs):
        """! Constructor: all process options are set here.

        @param base_directory: path to PowhegBox code.
        @param kwargs          dictionary of arguments from Generate_tf.
        """
        super(ggF_HZ, self).__init__(base_directory, "ggHZ", **kwargs)

        # Add algorithms to the sequence
        self.add_algorithm("quark colour fixer")

        # Add parameter validation functions
        self.validation_functions.append("validate_vdecaymode")

        ## List of allowed decay modes
        self.allowed_Z_decay_modes = ["z > all", "z > j j", "z > l+ l-", "z > v v~", "z > e+ e-", "z > mu+ mu-",
                                      "z > tau+ tau-", "z > ve ve~", "z > vm vm~", "z > vt vt~"]

        # Add all keywords for this process, overriding defaults if required
        self.add_keyword("bmass")
        self.add_keyword("bornktmin")
        self.add_keyword("bornonly", 1)
        self.add_keyword("bornsuppfact")
        self.add_keyword("bornzerodamp")
        self.add_keyword("bottomthr")
        self.add_keyword("bottomthrpdf")
        self.add_keyword("btildeborn")
        self.add_keyword("btildecoll")
        self.add_keyword("btildereal")
        self.add_keyword("btildevirt")
        self.add_keyword("charmthr")
        self.add_keyword("charmthrpdf")
        self.add_keyword("check_bad_st2")
        self.add_keyword("clobberlhe")
        self.add_keyword("colltest")
        self.add_keyword("compress_lhe")
        self.add_keyword("compress_upb")
        self.add_keyword("compute_rwgt")
        self.add_keyword("doublefsr")
        self.add_keyword("evenmaxrat")
        self.add_keyword("facscfact", self.default_scales[0])
        self.add_keyword("fastbtlbound")
        self.add_keyword("fixedgrid")
        self.add_keyword("flg_debug")
        self.add_keyword("foldcsi")
        self.add_keyword("foldphi")
        self.add_keyword("foldy")
        self.add_keyword("fullrwgt")
        self.add_keyword("fullrwgtmode")
        self.add_keyword("hdamp")
        self.add_keyword("hfact")
        self.add_keyword("higgsfixedwidth")
        self.add_keyword("hmass")
        self.add_keyword("hwidth")
        self.add_keyword("ih1")
        self.add_keyword("ih2")
        self.add_keyword("itmx1")
        self.add_keyword("itmx1rm")
        self.add_keyword("itmx2", 4)
        self.add_keyword("itmx2rm")
        self.add_keyword("iupperfsr")
        self.add_keyword("iupperisr")
        self.add_keyword("kappa_ghb")
        self.add_keyword("kappa_ght")
        self.add_keyword("kappa_ghz")
        self.add_keyword("lhans1", self.default_PDFs)
        self.add_keyword("lhans2", self.default_PDFs)
        self.add_keyword("lhapdf6maxsets")
        self.add_keyword("lhrwgt_descr")
        self.add_keyword("lhrwgt_group_combine")
        self.add_keyword("lhrwgt_group_name")
        self.add_keyword("lhrwgt_id")
        self.add_keyword("LOevents", 1)
        self.add_keyword("manyseeds")
        self.add_keyword("max_h_mass")
        self.add_keyword("max_io_bufsize")
        self.add_keyword("max_z_mass")
        self.add_keyword("maxseeds")
        self.add_keyword("min_h_mass")
        self.add_keyword("min_z_mass", 10)
        self.add_keyword("minlo")
        self.add_keyword("mintupbratlim")
        self.add_keyword("mintupbxless")
        self.add_keyword("ncall1", 15000)
        self.add_keyword("ncall1rm")
        self.add_keyword("ncall2", 60000)
        self.add_keyword("ncall2rm")
        self.add_keyword("ncallfrominput")
        self.add_keyword("noevents")
        self.add_keyword("novirtual")
        self.add_keyword("par_2gsupp")
        self.add_keyword("par_diexp")
        self.add_keyword("par_dijexp")
        self.add_keyword("parallelstage")
        self.add_keyword("pdfreweight")
        self.add_keyword("ptsqmin")
        self.add_keyword("ptsupp")
        self.add_keyword("rand1")
        self.add_keyword("rand2")
        self.add_keyword("renscfact", self.default_scales[1])
        self.add_keyword("runningscales")
        self.add_keyword("rwl_add")
        self.add_keyword("rwl_file")
        self.add_keyword("rwl_format_rwgt")
        self.add_keyword("rwl_group_events")
        self.add_keyword("smartsig")
        self.add_keyword("softtest")
        self.add_keyword("stage2init")
        self.add_keyword("storeinfo_rwgt")
        self.add_keyword("storemintupb")
        self.add_keyword("testplots")
        self.add_keyword("testsuda")
        self.add_keyword("tmass")
        self.add_keyword("ubexcess_correct")
        self.add_keyword("ubsigmadetails")
        self.add_keyword("use-old-grid")
        self.add_keyword("vdecaymode", self.allowed_Z_decay_modes[0], name="decay_mode_Z", description="Z boson decay mode.")
        self.add_keyword("withdamp")
        self.add_keyword("withnegweights")
        self.add_keyword("withsubtr")
        self.add_keyword("xgriditeration")


    def validate_vdecaymode(self):
        """! Validate vdecaymode keyword."""
        self.expose()  # convenience call to simplify syntax
        self.check_decay_mode(self.decay_mode_Z, self.allowed_Z_decay_modes)
        __decay_mode_lookup = {"j j": 0, "e+ e-": 1, "mu+ mu-": 2, "tau+ tau-": 3, "ve ve~": 4, "vm vm~": 5,
                               "vt vt~": 6, "all": 10, "l+ l-": 11, "v v~": 12}
        list(self.parameters_by_keyword("vdecaymode"))[0].value = __decay_mode_lookup[self.decay_mode_Z.split("z > ")[1].strip()]
