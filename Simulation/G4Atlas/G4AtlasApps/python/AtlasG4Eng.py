# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
Geant4 simulation Python facilities for the ATLAS experiment.

"G4Eng", the instance of PyG4Atlas.G4AtlasEngine, is defined here and it will be
accessed from any other module.

The basic elements and materials are also defined here. If custom materials are
needed, they can be produced using the template atlas_materials.py.
"""

__author__ = "A. Dell`Acqua, M. Gallas"

from AthenaCommon.SystemOfUnits import *

## Create the instance we will use later in other modules.
import PyG4Atlas
G4Eng = PyG4Atlas.G4AtlasEngine()

## Define the "basic elements"
G4Eng.add_Element("Hydrogen", "H", 1, 1.00797*g/mole, 0.0708*g/cm3)
G4Eng.add_Element("Deuterium", "D", 1, 2.01*g/mole, 0.162*g/cm3)
G4Eng.add_Element("Helium", "He", 2, 4.0026*g/mole, 0.125*g/cm3)
G4Eng.add_Element("Lithium", "Li", 3, 6.9410*g/mole, 0.534*g/cm3)
G4Eng.add_Element("Beryllium", "Be", 4, 9.0122*g/mole, 1.8480*g/cm3)
G4Eng.add_Element("Boron10", "B10", 5, 10.0*g/mole, 2.34*g/cm3)
G4Eng.add_Element("Boron11", "B11", 5, 11.0*g/mole, 2.34*g/cm3)
G4Eng.add_Element("Carbon", "C", 6, 12.011*g/mole, 2.2650*g/cm3)
G4Eng.add_Element("Nitrogen", "N", 7, 14.0067*g/mole, 0.8073*g/cm3)
G4Eng.add_Element("Oxygen", "O", 8, 15.9995*g/mole, 0.00143*g/cm3)
G4Eng.add_Element("Fluorine", "F", 9, 18.9984*g/mole, 1.1080*g/cm3)
G4Eng.add_Element("Neon", "Ne", 10, 20.179*g/mole, 1.2070*g/cm3)
G4Eng.add_Element("Sodium", "Na", 11, 22.9898*g/mole, 0.969*g/cm3)
G4Eng.add_Element("Magnesium", "Mg", 12, 24.305*g/mole, 1.735*g/cm3)
G4Eng.add_Element("Aluminum", "Al", 13, 26.9815*g/mole, 2.700*g/cm3)
G4Eng.add_Element("Silicon", "Si", 14, 28.086*g/mole, 2.33*g/cm3)
G4Eng.add_Element("Phosphorus", "P", 15, 30.9738*g/mole, 1.82*g/cm3)
G4Eng.add_Element("Sulfur", "S", 16, 32.06*g/mole, 2.07*g/cm3)
G4Eng.add_Element("Chlorine", "Cl", 17, 35.453*g/mole, 1.56*g/cm3)
G4Eng.add_Element("Argon", "Ar", 18, 39.948*g/mole, 0.001639*g/cm3)
G4Eng.add_Element("Potassium", "K", 19, 39.098*g/mole, 0.860*g/cm3)
G4Eng.add_Element("Calcium", "Ca", 20, 40.08*g/mole, 1.55*g/cm3)
G4Eng.add_Element("Scandium", "Sc", 21, 44.956*g/mole, 2.98*g/cm3)
G4Eng.add_Element("Titanium", "Ti", 22, 47.90*g/mole, 4.53*g/cm3)
G4Eng.add_Element("Vanadium", "V", 23, 50.9414*g/mole, 6.10*g/cm3)
G4Eng.add_Element("Chromium", "Cr", 24, 51.996*g/mole, 7.18*g/cm3)
G4Eng.add_Element("Manganese", "Mn", 25, 54.9830*g/mole, 7.43*g/cm3)
G4Eng.add_Element("Iron", "Fe", 26, 55.847*g/mole, 7.87*g/cm3)
G4Eng.add_Element("Cobalt", "Co", 27, 58.9332*g/mole, 8.90*g/cm3)
G4Eng.add_Element("Nickel", "Ni", 28, 58.70*g/mole, 8.876*g/cm3)
G4Eng.add_Element("Copper", "Cu", 29, 63.546*g/mole, 8.96*g/cm3)
G4Eng.add_Element("Zinc", "Zn", 30, 65.38*g/mole, 7.112*g/cm3)
G4Eng.add_Element("Gallium", "Ga", 31, 69.72*g/mole, 5.877*g/cm3)
G4Eng.add_Element("Germanium", "Ge", 32, 72.59*g/mole, 5.3230*g/cm3)
G4Eng.add_Element("Arsenic", "As", 33, 74.9216*g/mole, 5.72*g/cm3)
G4Eng.add_Element("Selenium", "Se", 34, 78.96*g/mole, 4.78*g/cm3)
G4Eng.add_Element("Bromine", "Br", 35, 79.904*g/mole, 3.11*g/cm3)
G4Eng.add_Element("Krypton", "Kr", 36, 83.80*g/mole, 2.60*g/cm3)
G4Eng.add_Element("Rubidium", "Rb", 37, 85.4678*g/mole, 1.529*g/cm3)
G4Eng.add_Element("Strontium", "Sr", 38, 87.62*g/mole, 2.54*g/cm3)
G4Eng.add_Element("Yttrium", "Y", 39, 88.909*g/mole, 4.456*g/cm3)
G4Eng.add_Element("Zirconium", "Zr", 40, 91.22*g/mole, 6.494*g/cm3)
G4Eng.add_Element("Niobium", "Nb", 41, 92.9064*g/mole, 8.55*g/cm3)
G4Eng.add_Element("Molybdenum", "Mo", 42, 95.94*g/mole, 10.20*g/cm3)
G4Eng.add_Element("Technetium", "Tc", 43, 97*g/mole, 11.48*g/cm3)
G4Eng.add_Element("Ruthenium", "Ru", 44, 101.07*g/mole, 12.39*g/cm3)
G4Eng.add_Element("Rhodium", "Rh", 45, 102.905*g/mole, 12.39*g/cm3)
G4Eng.add_Element("Palladium", "Pd", 46, 106.04*g/mole, 12.00*g/cm3)
G4Eng.add_Element("Silver", "Ag", 47, 107.868*g/mole, 10.48*g/cm3)
G4Eng.add_Element("Cadmium", "Cd", 48, 112.40*g/mole, 8.63*g/cm3)
G4Eng.add_Element("Indium", "In", 49, 114.82*g/mole, 7.30*g/cm3)
G4Eng.add_Element("Tin", "Sn", 50, 118.69*g/mole, 7.31*g/cm3)
G4Eng.add_Element("Antimony", "Sb", 51, 121.75*g/mole, 6.679*g/cm3)
G4Eng.add_Element("Tellurium", "Te", 52, 127.60*g/mole, 6.23*g/cm3)
G4Eng.add_Element("Iodine", "I", 53, 126.9046*g/mole, 7.30*g/cm3)
G4Eng.add_Element("Xenon", "Xe", 54, 131.30*g/mole, 3.057*g/cm3)
G4Eng.add_Element("Cesium", "Cs", 55, 132.905*g/mole, 1.870*g/cm3)
G4Eng.add_Element("Barium", "Ba", 56, 137.34*g/mole, 3.50*g/cm3)
G4Eng.add_Element("Larthanum", "La", 57, 138.91*g/mole, 6.127*g/cm3)
G4Eng.add_Element("Cerium", "Ce", 58, 140.12*g/mole, 6.637*g/cm3)
G4Eng.add_Element("Praseodymium", "Pr", 59, 140.908*g/mole, 0)
G4Eng.add_Element("Neodymium", "Nd", 60, 144.24*g/mole, 0)
G4Eng.add_Element("Promethium", "Pm", 61, 145*g/mole, 0)
G4Eng.add_Element("Samarium", "Sm", 62, 150.4*g/mole, 0)
G4Eng.add_Element("Europium", "Eu", 63, 151.96*g/mole, 0)
G4Eng.add_Element("Gadolinium", "Gd", 64, 157.25*g/mole, 0)
G4Eng.add_Element("Terbium", "Tb", 65, 158.925*g/mole, 0)
G4Eng.add_Element("Dysprosium", "Dy", 66, 162.50*g/mole, 0)
G4Eng.add_Element("Holmium", "Ho", 67, 164.930*g/mole, 0)
G4Eng.add_Element("Erbium", "Er", 68, 167.26*g/mole, 0)
G4Eng.add_Element("Thulium", "Tm", 69, 168.934*g/mole, 0)
G4Eng.add_Element("Ytterbium", "Yb", 70, 173.04*g/mole, 0)
G4Eng.add_Element("Lutetium", "Lu", 71, 174.97*g/mole, 0)
G4Eng.add_Element("Hafnium", "Hf", 72, 178.49*g/mole, 0)
G4Eng.add_Element("Tantalum", "Ta", 73, 180.948*g/mole, 0)
G4Eng.add_Element("Tungsten", "W", 74, 183.85*g/mole, 19.3*g/cm3)
G4Eng.add_Element("Rhenium", "Re", 75, 186.207*g/mole, 0)
G4Eng.add_Element("Osmium", "Os", 76, 190.2*g/mole, 0)
G4Eng.add_Element("Iridium", "Ir", 77, 192.22*g/mole, 0)
G4Eng.add_Element("Platinum", "Pt", 78, 195.09*g/mole, 0)
G4Eng.add_Element("Gold", "Au", 79, 196.967*g/mole, 18.85*g/cm3)
G4Eng.add_Element("Mercury", "Hg", 80, 200.59*g/mole, 0)
#G4Eng.add_Element("Thallium", "Tl", 81, 204.37*g/mole, 0)
G4Eng.add_Element("Lead", "Pb", 82, 207.2*g/mole, 11.35*g/cm3)
#G4Eng.add_Element("Bismuth", "Bi", 83, 208.980*g/mole, 9.37*g/cm3)
#G4Eng.add_Element("Polonium", "Po", 84, 209*g/mole, 0)
#G4Eng.add_Element("Astatine", "At", 85, 210*g/mole, 0)
#G4Eng.add_Element("Radon", "Rn", 86, 222*g/mole, 0)
#G4Eng.add_Element("Francium", "Fr", 87, 223*g/mole, 0)
#G4Eng.add_Element("Radium", "Ra", 88, 226.03*g/mole, 0)
#G4Eng.add_Element("Actinium", "Ac", 89, 227*g/mole, 0)
#G4Eng.add_Element("Thorium", "Th", 90, 232.038*g/mole, 0)
#G4Eng.add_Element("Protactinium", "Pa", 91, 231.038*g/mole, 0)
G4Eng.add_Element("Uranium", "U", 92, 238.03*g/mole, 18.950*g/cm3)
#G4Eng.add_Element("Neptunium", "Np", 93, 237.048*g/mole, 0)

## Define the "basic composite materials": air and approximate vacuum
G4Eng.add_CompositeMaterial("Air", 0.001214*g/cm3)
air = G4Eng.Dict_Materials.get("Air")
air.AddMaterial("Nitrogen", 0.7494)
air.AddMaterial("Oxygen", 0.2369)
air.AddMaterial("Argon", 0.0129)
air.AddMaterial("Hydrogen", 0.0008)

G4Eng.add_CompositeMaterial("Vacuum", 0.000001*g/cm3)
vacuum = G4Eng.Dict_Materials.get("Vacuum")
vacuum.AddMaterial("Air", 1.0)
