def get_and_fix_PDGTABLE(replace):
    import os, shutil, re

    # Download generic PDGTABLE (overwrite existing one if it exists)
    os.system('get_files -remove -data PDGTABLE.MeV')
    shutil.move('PDGTABLE.MeV', 'PDGTABLE.MeV.org')

    # an example line to illustrate the fixed format, see PDGTABLE.MeV for more details
    # M 1000022                          0.E+00         +0.0E+00 -0.0E+00 ~chi(0,1)     0

    lines = open('PDGTABLE.MeV.org').readlines()
    for pdgid,mass,name,charge in replace:
        if not re.search(r'[MW]\s+'+str(pdgid)+'\s+\S+', ''.join(lines)):
            lines.append('M' + str(pdgid).rjust(8) +''.ljust(26) +
                         ('%11.5E' % mass).ljust(15) +
                         '+0.0E+00'.ljust(9) + '-0.0E+00'.ljust(9) +
                         name.strip() + ''.ljust(6) + charge.strip() + '\n')
            lines.append('W' + str(pdgid).rjust(8) +''.ljust(26) +
                         '0.E+00'.ljust(15) + '+0.0E+00'.ljust(9) + '-0.0E+00'.ljust(9) +
                         name.strip() + ''.ljust(6) + charge.strip() + '\n')
        else:
            for i in xrange(len(lines)):
                if re.search(r'M\s+'+str(pdgid)+'\s+\S+', lines[i]):
                    l = lines[i]
                    lines[i] = l[0:35] + ('%11.5E' % mass).ljust(14) + l[49:]

    update = open('PDGTABLE.MeV', 'w')
    update.write(''.join(lines))
    update.close()


doG4SimConfig = True
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
import PyUtils.AthFile as af
try:
    f = af.fopen(athenaCommonFlags.FilesInput()[0])

    if "StreamHITS" in f.infos["stream_names"]:
        from Digitization.DigitizationFlags import digitizationFlags
        simdict = digitizationFlags.specialConfiguration.get_Value()
        doG4SimConfig = False
    else:
        from G4AtlasApps import AtlasG4Eng
        simdict = AtlasG4Eng.G4Eng.Dict_SpecialConfiguration
except:
    from G4AtlasApps import AtlasG4Eng
    simdict = AtlasG4Eng.G4Eng.Dict_SpecialConfiguration

C1Mass = eval(simdict["AMSBC1Mass"])
N1Mass = eval(simdict["AMSBN1Mass"])
# patching PDGTABLE
get_and_fix_PDGTABLE([(1000022, N1Mass, '~chi(0,1)', '0'), (1000024, C1Mass, '~chi(+,1)', '+')])

if doG4SimConfig:
    from G4AtlasApps.SimFlags import simFlags
    def amsb_processlist():
        from G4AtlasApps import AtlasG4Eng
        AtlasG4Eng.G4Eng.gbl.G4Commands().process.list()

    simFlags.InitFunctions.add_function("postInit", amsb_processlist)

    def amsb_setparams():
        from G4AtlasApps import AtlasG4Eng
        from GaudiKernel.SystemOfUnits import GeV, ns
        C1Mass = eval(AtlasG4Eng.G4Eng.Dict_SpecialConfiguration["AMSBC1Mass"])
        N1Mass = eval(AtlasG4Eng.G4Eng.Dict_SpecialConfiguration["AMSBN1Mass"])
        C1Lifetime = eval(AtlasG4Eng.G4Eng.Dict_SpecialConfiguration["AMSBC1Lifetime"])


        AtlasG4Eng.G4Eng._ctrl.load("Charginos")
        charginoPlus = AtlasG4Eng.G4Eng.gbl.ParticleDataModifier("s_chi_plus_1")
        charginoPlus.SetParticleMass(C1Mass)
        if C1Lifetime == -1:
            charginoPlus.Stable(True)
        else:
            charginoPlus.SetParticleLifeTime(C1Lifetime)
            charginoPlus.Stable(False)
            charginoPlus.AddDecayChannel("s_chi_plus_1", 1., "s_chi_0_1=pi+")

        charginoMinus = AtlasG4Eng.G4Eng.gbl.ParticleDataModifier("s_chi_minus_1")
        charginoMinus.SetParticleMass(C1Mass)
        if C1Lifetime == -1:
            charginoMinus.Stable(True)
        else:
            charginoMinus.SetParticleLifeTime(C1Lifetime)
            charginoMinus.Stable(False)
            charginoMinus.AddDecayChannel("s_chi_minus_1", 1., "s_chi_0_1=pi-")

        neutralino = AtlasG4Eng.G4Eng.gbl.ParticleDataModifier("s_chi_0_1")
        neutralino.SetParticleMass(N1Mass)
        neutralino.Stable(True)


    simFlags.InitFunctions.add_function("preInitPhysics", amsb_setparams)

    def amsb_applycalomctruthstrategy():
## Applying the MCTruth strategies: add decays in the Calorimeter
        from G4AtlasApps import AtlasG4Eng
        myDecay = AtlasG4Eng.G4Eng.Dict_MCTruthStrg.get('Decay')
        myDecay.add_Volumes('CALO::CALO', 1)

    simFlags.InitFunctions.add_function("postInit", amsb_applycalomctruthstrategy)

del doG4SimConfig, simdict
