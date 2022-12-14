/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LIGHTWEIGHT_NETWORK_CONFIG_HH_TAURECTOOLS
#define LIGHTWEIGHT_NETWORK_CONFIG_HH_TAURECTOOLS

// NNLayerConfig is the "low level" configuration, which should
// contain everything needed to create a "Stack" or "Graph".
//
#include "NNLayerConfig.h"

// The code below is to configure the "high level" interface.  These
// structures are used to initalize the "LightweightNeuralNetwork" and
// "LightweightGraph".

#include <map>
#include <string>

namespace lwtDev {

  typedef std::map<std::string, std::vector<double> > VectorMap;

  struct Input
  {
    std::string name;
    double offset;
    double scale;
  };

  // feed forward structure
  //
  // Note that this isn't technically JSON-dependant, the name is mostly
  // historical
  struct JSONConfig
  {
    std::vector<LayerConfig> layers;
    std::vector<Input> inputs;
    std::vector<std::string> outputs;
    std::map<std::string, double> defaults;
    std::map<std::string, std::string> miscellaneous;
  };

  // graph structures
  struct InputNodeConfig
  {
    std::string name;
    std::vector<Input> variables;
    std::map<std::string, std::string> miscellaneous;
    std::map<std::string, double> defaults;
  };
  struct OutputNodeConfig
  {
    std::vector<std::string> labels;
    size_t node_index;
  };
  struct GraphConfig
  {
    std::vector<InputNodeConfig> inputs;
    std::vector<InputNodeConfig> input_sequences;
    std::vector<NodeConfig> nodes;
    std::map<std::string, OutputNodeConfig> outputs;
    std::vector<LayerConfig> layers;
  };

}

#endif
