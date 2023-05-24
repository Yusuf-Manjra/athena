#  Copyright (C) 2002-2023 CERN for the benefit of the ATLAS collaboration


from GeneratorModules.EvgenAlg import EvgenAlg
from AthenaPython.PyAthena import StatusCode
try:
   from AthenaPython.PyAthena import HepMC3  as HepMC
except ImportError:
   from AthenaPython.PyAthena import HepMC   as HepMC

import os
import math
import ROOT


def init_pz(energy):
    return math.sqrt(energy**2 - 938.272046**2)


class LheEVNTFiller(EvgenAlg):

    def __init__(self, ecmEnergy, name="LheEVNTFiller"):
        super(LheEVNTFiller, self).__init__(name=name)
        self.beamEnergyMeV = ecmEnergy * 1000. / 2.

    fileName = "evrecs/evrecout.dat"
    outputFileName = "outputs/outputout.dat"
    eventsProcessed = 0

    def initialize(self):

        if(os.path.isfile(self.fileName) and os.path.isfile(self.outputFileName)):
            print(self.fileName)
            return StatusCode.Success
        else:
            return StatusCode.Failure


    def fillEvent(self, evt):

        eventsSeen = 0
        firstLine = True

        with open(self.outputFileName, 'r') as inputOutputFile:
            if self.eventsProcessed == 0:
                for line in inputOutputFile:
                    if 'Cross section =' in line:
                        splitLine = line.split()
                        factor = 1.
                        if(splitLine[-1] == "pb"):
                            factor = 0.001
                        if(splitLine[-1] == "fb"):
                            factor = 0.000001
                        if(splitLine[-1] == "ub"):
                            factor = 1000.
                        if(splitLine[-1] == "mb"):
                            factor = 1000000.
                        print("MetaData: cross-section (nb)= "+str(float(splitLine[3])*factor))

                        break

        with open(self.fileName, 'r') as inputfile:
            event = False
            for line in inputfile:
                if not event and '<event>' not in line:
                    continue
            # Check if we are just starting an event
                if not event and '<event>' in line and eventsSeen == self.eventsProcessed:
                    event = True
                    continue
            # Check if we have finished and are doing something else
                if '<' in line or '>' in line:
                    event = False
                    eventsSeen += 1
                    firstLine = True
                    continue
                if event and firstLine:
                    firstLine = False
                    evt.weights().push_back(float(line.split()[2]))

                    # Add the initial state protons
                    pos = HepMC.FourVector(0.0, 0.0, 0.0, 0.0)
                    gv = HepMC.GenVertex(pos)
                    ROOT.SetOwnership(gv, False)
                    evt.add_vertex(gv)
                    mom = HepMC.FourVector(0., 0., init_pz(self.beamEnergyMeV), self.beamEnergyMeV)
                    gp = HepMC.GenParticle()
                    gp.set_status( 4 )
                    gp.set_pdg_id( 2212 )
                    gp.set_momentum(mom)
                    gp.set_generated_mass(938.272046)
                    ROOT.SetOwnership(gp, False)
                    gv.add_particle_out(gp)

                    pos = HepMC.FourVector(0.0, 0.0, 0.0, 0.0)
                    gv = HepMC.GenVertex(pos)
                    ROOT.SetOwnership(gv, False)
                    evt.add_vertex(gv)
                    mom = HepMC.FourVector(0., 0., -init_pz(self.beamEnergyMeV), self.beamEnergyMeV)
                    gp = HepMC.GenParticle()
                    gp.set_status( 4 )
                    gp.set_pdg_id( 2212 )
                    gp.set_momentum(mom)
                    gp.set_generated_mass(938.272046)
                    ROOT.SetOwnership(gp, False)
                    gv.add_particle_out(gp)

                    continue
                if event:
                    pos = HepMC.FourVector(0.0, 0.0, 0.0, 0.0)
                    gv = HepMC.GenVertex(pos)
                    ROOT.SetOwnership(gv, False)
                    evt.add_vertex(gv)
                    mom = HepMC.FourVector( float(line.split()[6])*1000. , float(line.split()[7])*1000. , float(line.split()[8])*1000. , float(line.split()[9])*1000. )
                    gp = HepMC.GenParticle()
                    gp.set_status(int(line.split()[1]) )
                    gp.set_pdg_id(int(line.split()[0]) )
                    gp.set_momentum(mom)
                    gp.set_generated_mass(float(line.split()[10]) * 1000.)
                    ROOT.SetOwnership(gp, False)
                    gv.add_particle_out(gp)

        self.eventsProcessed += 1

        return StatusCode.Success

