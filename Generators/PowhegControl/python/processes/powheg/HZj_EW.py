# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import Logging
from ..powheg_RES import PowhegRES

## Get handle to Athena logging
logger = Logging.logging.getLogger("PowhegControl")


class HZj_EW(PowhegRES):
    """! Default Powheg configuration for associated Higgs boson and Z-boson production plus one jet, with electroweak and quark mass effects.

    Create a configurable object with all applicable Powheg options.
    @author James Robinson  <james.robinson@cern.ch>
    """

    def __init__(self, base_directory, **kwargs):
        """! Constructor: all process options are set here.

        @param base_directory  path to PowhegBox code.
        @param kwargs          dictionary of arguments from Generate_tf.
        """
        super(HZj_EW, self).__init__(base_directory, "HZJ_ew", **kwargs)

        ## List of allowed decay modes
        self.allowed_decay_modes = ["z > e+ e-", "z > mu+ mu-", "z > tau+ tau-", "z > ve ve~", "z > vm vm~",
                                    "z > vt vt~", "z > j j", "z > l l~", "z > v v~", "z > all"]

        # Add all keywords for this process, overriding defaults if required
        self.add_keyword("allrad", 1)
        self.add_keyword("alphaem")
        self.add_keyword("bmass")
        self.add_keyword("bornktmin", 0.26)
        self.add_keyword("bornonly")
        self.add_keyword("bornsuppfact", 0.0)
        self.add_keyword("bornsuppfactV", 700.0)
        self.add_keyword("bornzerodamp")
        self.add_keyword("bottomthr")
        self.add_keyword("bottomthrpdf")
        self.add_keyword("btildeborn")
        self.add_keyword("btildecoll")
        self.add_keyword("btildereal")
        self.add_keyword("btildevirt")
        self.add_keyword("btlscalect")
        self.add_keyword("btlscalereal")
        self.add_keyword("charmthr")
        self.add_keyword("charmthrpdf")
        self.add_keyword("chklimseed")
        self.add_keyword("CKM_diagonal")
        self.add_keyword("clobberlhe")
        self.add_keyword("colltest")
        self.add_keyword("comparison_paper")
        self.add_keyword("compress_lhe")
        self.add_keyword("compress_upb")
        self.add_keyword("compute_rwgt")
        self.add_keyword("doublefsr")
        self.add_keyword("enhancereg")
        self.add_keyword("evenmaxrat")
        self.add_keyword("ew_renorm_scheme")
        self.add_keyword("facscfact", self.default_scales[0])
        self.add_keyword("factsc2min")
        self.add_keyword("fastbtlbound")
        self.add_keyword("flg_debug")
        self.add_keyword("foldcsi", 5)
        self.add_keyword("foldphi", 5)
        self.add_keyword("foldy", 5)
        self.add_keyword("for_reweighting")
        self.add_keyword("frensc2min")
        self.add_keyword("fullrwgt")
        self.add_keyword("hdamp")
        self.add_keyword("hdecaymode")
        self.add_keyword("hfact")
        self.add_keyword("hmass")
        self.add_keyword("icsimax")
        self.add_keyword("idvecbos", hidden=True)
        self.add_keyword("ih1")
        self.add_keyword("ih2")
        self.add_keyword("itmx1", 2)
        self.add_keyword("itmx1btl")
        self.add_keyword("itmx1btlbrn")
        self.add_keyword("itmx1reg")
        self.add_keyword("itmx1rm")
        self.add_keyword("itmx2", 2)
        self.add_keyword("itmx2btl")
        self.add_keyword("itmx2btlbrn")
        self.add_keyword("itmx2reg")
        self.add_keyword("itmx2rm")
        self.add_keyword("iupperfsr")
        self.add_keyword("iupperisr")
        self.add_keyword("iymax")
        self.add_keyword("kappa_ghb")
        self.add_keyword("kappa_ght")
        self.add_keyword("kappa_ghw")
        self.add_keyword("lambdaHHH")
        self.add_keyword("lhans1", self.default_PDFs)
        self.add_keyword("lhans2", self.default_PDFs)
        self.add_keyword("LOevents")
        self.add_keyword("manyseeds")
        self.add_keyword("max_h_mass", 2.0 * self.parameters_by_name("beam_energy")[0].value)
        self.add_keyword("max_io_bufsize")
        self.add_keyword("max_z_mass", 2.0 * self.parameters_by_name("beam_energy")[0].value)
        self.add_keyword("maxseeds")
        self.add_keyword("min_h_mass")
        self.add_keyword("min_z_mass", 10.0)
        self.add_keyword("minlo_nnll")
        self.add_keyword("minlo", 1)
        self.add_keyword("mint_density_map")
        self.add_keyword("mintupbratlim", 1000)
        self.add_keyword("ncall1", 120000)
        self.add_keyword("ncall1btl")
        self.add_keyword("ncall1btlbrn")
        self.add_keyword("ncall1reg")
        self.add_keyword("ncall1rm")
        self.add_keyword("ncall2", 600000)
        self.add_keyword("ncall2btl")
        self.add_keyword("ncall2btlbrn")
        self.add_keyword("ncall2reg")
        self.add_keyword("ncall2rm")
        self.add_keyword("no_photon_radiation")
        self.add_keyword("noevents")
        self.add_keyword("nohad")
        self.add_keyword("nores")
        self.add_keyword("novirtual")
        self.add_keyword("nubound", 400000)
        self.add_keyword("olpreset")
        self.add_keyword("olverbose")
        self.add_keyword("onshellhiggs")
        self.add_keyword("par_2gsupp")
        self.add_keyword("par_diexp")
        self.add_keyword("par_dijexp")
        self.add_keyword("parallelstage")
        self.add_keyword("pdfreweight")
        self.add_keyword("ptsupp")
        self.add_keyword("ptVhigh")
        self.add_keyword("ptVlow")
        self.add_keyword("qed_qcd")
        self.add_keyword("radregion")
        self.add_keyword("raisingscales")
        self.add_keyword("rand1")
        self.add_keyword("rand2")
        self.add_keyword("regridfix")
        self.add_keyword("renscfact", self.default_scales[1])
        self.add_keyword("runningscales", 3, description="scale at underlying Born level. [0: mh+mw; 1: Ht; 2: sqrt(pt_l1*pt_l2); 3: sqrt((p_H+p_l1+p_l2)^2)]")
        self.add_keyword("rwl_add")
        self.add_keyword("rwl_file")
        self.add_keyword("rwl_format_rwgt")
        self.add_keyword("rwl_group_events")
        self.add_keyword("select_EW_virt")
        self.add_keyword("shortlist_onlyb")
        self.add_keyword("shortlist")
        self.add_keyword("smartsig")
        self.add_keyword("softmismch")
        self.add_keyword("softonly")
        self.add_keyword("softtest")
        self.add_keyword("stage2init")
        self.add_keyword("storeinfo_rwgt")
        self.add_keyword("storemintupb")
        self.add_keyword("sudscalevar")
        self.add_keyword("testplots")
        self.add_keyword("testsuda")
        self.add_keyword("tmass")
        self.add_keyword("ubexcess_correct")
        self.add_keyword("ubsigmadetails", 0) # causes crash if enabled
        self.add_keyword("use-old-grid")
        self.add_keyword("use-old-ubound")
        self.add_keyword("user_reshists_sep")
        self.add_keyword("vdecaymode", self.allowed_decay_modes[0], name="decay_mode", description="W boson decay mode. [{}]".format("; ".join(self.allowed_decay_modes)))
        self.add_keyword("virtonly")
        self.add_keyword("Vstep")
        self.add_keyword("whichpwhgevent")
        self.add_keyword("withbtilde")
        self.add_keyword("withdamp")
        self.add_keyword("withnegweights")
        self.add_keyword("withregular")
        self.add_keyword("withremnants")
        self.add_keyword("withsubtr")
        self.add_keyword("xgriditeration")
        self.add_keyword("xupbound", 2)
        self.add_keyword("zerowidth")

        # Add parameter validation functions
        self.validation_functions.append("validate_decays")

    def validate_decays(self):
        """! Validate idvecbos and vdecaymode keywords."""
        self.expose()  # convenience call to simplify syntax
        self.check_decay_mode(self.decay_mode, self.allowed_decay_modes)
        # Calculate appropriate decay mode numbers
        __decay_mode_lookup = {"e+ e-": 1, "mu+ mu-": 2, "tau+ tau-": 3,
                               "ve ve~": 4, "vm vm~": 5, "vt vt~": 6,
                               "j j": 0, "l l~": 11, "v v~": 12, "all": 10}
        self.parameters_by_keyword("vdecaymode")[0].value = __decay_mode_lookup[self.decay_mode.split("> ")[1]]


#        self.add_keyword("icsimax", 1)
#        self.add_keyword("iymax", 1)
#        self.add_keyword("ncall1", 25000)
#        self.add_keyword("ncall2", 200000)
#        self.add_keyword("nubound", 40000)
#  Cross-section test : 0.16% PASSED
#  Neg. weight test   : 1.02% FAILED
#  Upper bound test   : 3.14% FAILED
#    > cross-section  : 3.14%
#    > radiation      : 0.00%
#  Output EVNT files  : 0     FAILED
#  Multi-core generation of 5500 events took 10.61 hours [0.144Hz]
#  ... time taken for integration:   10.07 hours
#  ... est. generation of 5k events: 0.49 hours

#        self.add_keyword("icsimax", 1)
#        self.add_keyword("iymax", 1)
#        self.add_keyword("ncall1", 30000)
#        self.add_keyword("ncall2", 250000)
#        self.add_keyword("nubound", 80000)
#  Cross-section test : 0.15% PASSED
#  Neg. weight test   : 1.06% FAILED
#  Upper bound test   : 3.51% FAILED
#    > cross-section  : 3.51%
#    > radiation      : 0.00%
#  Output EVNT files  : 0     FAILED
#  Multi-core generation of 5500 events took 15.62 hours [0.098Hz]
#  ... time taken for integration:   15.30 hours
#  ... est. generation of 5k events: 0.29 hours

#        self.add_keyword("icsimax", 2)
#        self.add_keyword("iymax", 2)
#        self.add_keyword("ncall1", 35000)
#        self.add_keyword("ncall2", 250000)
#        self.add_keyword("nubound", 80000)
#  Cross-section test : 0.15% PASSED
#  Neg. weight test   : 0.97% PASSED
#  Upper bound test   : 3.71% FAILED
#    > cross-section  : 3.71%
#    > radiation      : 0.00%
#  Output EVNT files  : 0     FAILED
#  Multi-core generation of 5500 events took 14.01 hours [0.109Hz]
#  ... time taken for integration:   13.73 hours
#  ... est. generation of 5k events: 0.25 hours

#        self.add_keyword("icsimax", 2)
#        self.add_keyword("iymax", 2)
#        self.add_keyword("ncall1", 35000)
#        self.add_keyword("ncall2", 400000)
#        self.add_keyword("nubound", 150000)
#  Cross-section test : 0.11% PASSED
#  Neg. weight test   : 0.99% PASSED
#  Upper bound test   : 3.02% FAILED
#    > cross-section  : 3.02%
#    > radiation      : 0.00%
#  Output EVNT files  : 0     FAILED
#  Multi-core generation of 5500 events took 25.09 hours [0.061Hz]
#  ... time taken for integration:   24.74 hours
#  ... est. generation of 5k events: 0.31 hours

#        self.add_keyword("icsimax", 1)
#        self.add_keyword("iymax", 1)
#        self.add_keyword("ncall1", 35000)
#        self.add_keyword("ncall2", 420000)
#        self.add_keyword("nubound", 150000)
#  Cross-section test : 0.11% PASSED
#  Neg. weight test   : 0.99% PASSED
#  Upper bound test   : 2.87% FAILED
#    > cross-section  : 2.87%
#    > radiation      : 0.00%
#  Output EVNT files  : 0     FAILED
#  Multi-core generation of 5500 events took 26.72 hours [0.057Hz]
#  ... time taken for integration:   26.43 hours
#  ... est. generation of 5k events: 0.26 hours

#        self.add_keyword("icsimax", 1)
#        self.add_keyword("iymax", 1)
#        self.add_keyword("ncall1", 40000)
#        self.add_keyword("ncall2", 500000)
#        self.add_keyword("nubound", 200000)
#  Cross-section test : 0.10% PASSED
#  Neg. weight test   : 1.03% FAILED
#  Upper bound test   : 3.31% FAILED
#    > cross-section  : 3.31%
#    > radiation      : 0.00%
#  Output EVNT files  : 0     FAILED
#  Multi-core generation of 5500 events took 21.99 hours [0.069Hz]
#  ... time taken for integration:   21.61 hours
#  ... est. generation of 5k events: 0.35 hours

#        self.add_keyword("icsimax", 1)
#        self.add_keyword("iymax", 1)
#        self.add_keyword("ncall1", 100000)
#        self.add_keyword("ncall2", 600000)
#        self.add_keyword("nubound", 400000)
#  Cross-section test : 0.12% PASSED
#  Neg. weight test   : 1.04% FAILED
#  Upper bound test   : 3.56% FAILED
#    > cross-section  : 3.56%
#    > radiation      : 0.00%
#  Output EVNT files  : 0     FAILED
#  Multi-core generation of 5500 events took 28.20 hours [0.054Hz]
#  ... time taken for integration:   27.96 hours
#  ... est. generation of 5k events: 0.21 hours

#        self.add_keyword("icsimax", 1)
#        self.add_keyword("iymax", 1)
#        self.add_keyword("ncall1", 120000)
#        self.add_keyword("ncall2", 620000) 
#        self.add_keyword("nubound", 420000)
# failed

#        self.add_keyword("icsimax", 1)
#        self.add_keyword("iymax", 1)
#        self.add_keyword("ncall1", 120000)
#        self.add_keyword("ncall2", 600000)
#        self.add_keyword("nubound", 400000)
#  Cross-section test : 0.10% PASSED
#  Neg. weight test   : 1.07% FAILED
#  Upper bound test   : 2.40% FAILED
#    > cross-section  : 2.40%
#    > radiation      : 0.00%
#  Output EVNT files  : 0     FAILED
#  Multi-core generation of 5500 events took 28.96 hours [0.053Hz]
#  ... time taken for integration:   28.64 hours
#  ... est. generation of 5k events: 0.28 hours

