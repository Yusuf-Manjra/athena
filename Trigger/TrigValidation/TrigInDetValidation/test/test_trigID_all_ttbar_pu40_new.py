#!/usr/bin/env python

# art-description: art job for all_ttbar_pu40_new
# art-type: grid
# art-include: master/Athena
# art-athena-mt: 8
# art-memory: 4096
# art-html: https://idtrigger-val.web.cern.ch/idtrigger-val/TIDAWeb/TIDAart/?jobdir=
# art-output: *.txt
# art-output: *.log
# art-output: log.*
# art-output: *.out
# art-output: *.err
# art-output: *.log.tar.gz
# art-output: *.new
# art-output: *.json
# art-output: *.root
# art-output: *.check*
# art-output: HLT*
# art-output: times*
# art-output: cost-perCall
# art-output: cost-perEvent
# art-output: cost-perCall-chain
# art-output: cost-perEvent-chain
# art-output: *.dat 


Slices  = ['muon','electron','tau','bjet','fsjet']
RunEF   = False
Events  = 4000
Threads = 8 
Slots   = 8
Input   = 'ttbar_ID'    # defined in TrigValTools/share/TrigValInputs.json  

Jobs = [ ( "Offline",  " TIDAdata-run3-offline.dat -o data-hists.root" ) ]


Comp = [ ( "L2muon",       "L2muon",      "data-hists.root",          " -c TIDAhisto-panel.dat  -d HLTL2-plots-muon " ),
         ( "L2electron",   "L2electron",  "data-hists.root",          " -c TIDAhisto-panel.dat  -d HLTL2-plots-electron " ),
         ( "L2tau",        "L2tau",       "data-hists.root",          " -c TIDAhisto-panel.dat  -d HLTL2-plots-tau " ),
         ( "L2bjet",       "L2bjet",      "data-hists.root",          " -c TIDAhisto-panel.dat  -d HLTL2-plots-bjet " ),   
         ( "FSjetoffline", "L2fsjet",     "data-hists.root",          " -c TIDAhisto-panel.dat  -d HLTL2-plots-FS " ),
         ( "FSvtx",        "L2fsjetvtx",  "data-hists.root",          " -c TIDAhistos-vtx.dat   -d HLTL2-plots-vtx " ),
 
         ( "EFmuon",       "EFmuon",      "data-hists.root",          " -c TIDAhisto-panel.dat  -d HLTEF-plots-muon " ),
         ( "EFelectron",   "EFelectron",  "data-hists.root",          " -c TIDAhisto-panel.dat  -d HLTEF-plots-electron " ),
         ( "EFtau",        "EFtau",       "data-hists.root",          " -c TIDAhisto-panel.dat  -d HLTEF-plots-tau " ),
         ( "EFbjet",       "EFmuon",      "data-hists.root",          " -c TIDAhisto-panel.dat  -d HLTEF-plots-bjet " ) ]
   
from AthenaCommon.Include import include 
include("TrigInDetValidation/TrigInDetValidation_NewBase.py")

