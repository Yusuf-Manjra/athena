###############################################################
#
# Job options file
#
#==============================================================
#--------------------------------------------------------------
# General Application Configuration options
#--------------------------------------------------------------
include( "AthenaCommon/Atlas_Gen.UnixStandardJob.py" )

include( "PartPropSvc/PartPropSvc.py" )

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
theApp.Dlls  += [ "Sherpa_i", "Tauola_i", "Photos_i","TruthExamples"]
theApp.TopAlg = ["Sherpa_i", "Tauola","Photos","PrintMC"]
theApp.ExtSvc += ["AtRndmGenSvc"]
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel               = 2 
#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
# Number of events to be processed (default is 10)
theApp.EvtMax = 5
#--------------------------------------------------------------
# Algorithms Private Options
#--------------------------------------------------------------
AtRndmGenSvc = Service( "AtRndmGenSvc" )
AtRndmGenSvc.Seeds = ["SHERPA 4789899 989240512", "SHERPA_INIT 889223465 78782321",
                      "TAUOLA 530753022 122769126", "TAUOLA_INIT 772087946 292053749",
                      "PHOTOS 478375275 346831679", "PHOTOS_INIT 239264166 848234952"
                      ]

#-- Tauola
Tauola = Algorithm( "Tauola" )
Tauola.TauolaCommand = [        "tauola polar 1",
                                "tauola radcor 1",
                                "tauola phox 0.01",
                                "tauola dmode 0",
                                "tauola jak1 0",
                                "tauola jak2 0"]

#-- Photos
Photos = Algorithm( "Photos" )
Photos.PhotosCommand = [        "photos pmode 2",
                                "photos xphcut 0.01",
                                "photos alpha -1.",
                                "photos interf 1",
                                "photos isec 1",
				"photos itre 0",
				"photos iexp 1",
                                "photos iftop 0"]

#-- PrintMC
PrintMC = Algorithm( "PrintMC" )
#event SVC key
PrintMC.McEventKey = "GEN_EVENT"
# do you want output at all? TRUE/FALSE
PrintMC.VerboseOutput = TRUE
# Event print style Vertex(traditional)/Barcode(barcode ordered)
PrintMC.PrintStyle = "Barcode"
# First and last event to print, if no last events => job end are printed
PrintMC.FirstEvent = 1
PrintMC.LastEvent = 10

#---------------------------------------------------------------
# Ntuple service output
#---------------------------------------------------------------
#==============================================================
#
# End of job options file
#
###############################################################
