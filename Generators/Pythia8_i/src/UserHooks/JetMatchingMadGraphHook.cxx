/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "Pythia8_i/UserHooksFactory.h"
#include "Pythia8Plugins/JetMatching.h"

namespace Pythia8{
  Pythia8_UserHooks::UserHooksFactory::Creator<Pythia8::JetMatchingMadgraph> JetMatchingMadGraphCreator("JetMatchingMadgraph");
}
