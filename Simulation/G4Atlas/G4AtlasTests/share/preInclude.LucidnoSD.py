include("G4AtlasTests/preInclude.Lucid.py")

from G4AtlasApps import AtlasG4Eng
AtlasG4Eng.G4Eng._InitList += ['init_SenDetector']
simFlags.ReleaseGeoModel.set_Off()

