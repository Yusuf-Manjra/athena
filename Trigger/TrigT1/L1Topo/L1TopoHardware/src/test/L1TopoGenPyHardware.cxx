/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "L1TopoHardware/L1TopoHardware.h"

#include <iostream>
#include <fstream>

using namespace std;
using namespace TCS;

void generateCode(ofstream & f, const HardwareParam& hwp) {
   f << hwp.name << " = HardwareConstrainedParameter( '" << hwp.name << "', " << hwp.value << ", '" << hwp.description << "', '" << hwp.rule << "')" << endl;
}

int main(int argc, char** argv) {

   string pyname = "L1TopoHardware.py";
   if(argc==2)
      pyname = argv[1];

   ofstream f(pyname, ios_base::out | ios_base::trunc);
   f << "'''This file is autogenerated by TrigConfL1TopoGenPyHardware'''" << endl << endl;
   f << "from collections import namedtuple" << endl
     << "HardwareConstrainedParameter = namedtuple('HardwareConstrainedParameter','name value description rule')" << endl << endl;
   for(const auto & hwp : L1TopoHardwareParameters ) {
      generateCode(f, hwp.second);
   }
   
   f.close();

   return 0;
}
