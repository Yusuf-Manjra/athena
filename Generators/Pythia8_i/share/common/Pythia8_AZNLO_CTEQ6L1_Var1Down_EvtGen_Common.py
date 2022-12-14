## Config for Py8 tune AZNLO with CTEQ6L1 with AZNLO tune and renormalization scale down
include("Pythia8_i/Pythia8_AZNLO_CTEQ6L1_Var1Down_Common.py")

# Add EvtGen for b fragmentation as default.  No EvtGen is available in "nonStandard"
include("Pythia8_i/Pythia8_EvtGen.py")
