## Config for Py8 tune AZNLO with CTEQ6L1
## changed to Pythia8.2 standard for pdfs, for Pythia8.1 was PDF:LHAPDFset = cteq6ll.LHpdf
include("Pythia8_i/Pythia8_Base_Fragment.py")

genSeq.Pythia8.Commands += [
    "Tune:pp = 5",
    "BeamRemnants:primordialKThard = 1.74948",
    "SpaceShower:alphaSorder = 2",
    "SpaceShower:alphaSvalue = 0.118",
    "SpaceShower:pT0Ref = 1.923589",
    "MultipartonInteractions:pT0Ref = 2.002887"
    ]

rel = os.popen("echo $AtlasVersion").read()

if rel[:2].isdigit() and int(rel[:2])<20:
  ver =  os.popen("cmt show versions External/Pythia8").read()
  print ("Pythia8 version: " + ver)
  if 'Pythia8-01' in ver[:50]:
   genSeq.Pythia8.Commands += [
    "PDF:useLHAPDF = on",
    "PDF:LHAPDFset = cteq6ll.LHpdf"
    ]
  else:
   genSeq.Pythia8.Commands += ["PDF:pSet=LHAPDF6:cteq6l1"]
else:
  genSeq.Pythia8.Commands += ["PDF:pSet=LHAPDF6:cteq6l1"]

evgenConfig.tune = "AZNLO CTEQ6L1"

# needs Pythia8 Main31 matching
include('Pythia8_i/Pythia8_Powheg_Main31.py')

#this block may be needed for future reference
#genSeq.Pythia8.UserModes += ['Main31:NFinal = 1',
#                             'Main31:pTHard = 0',
#                             'Main31:pTdef = 2'
#                             ]
