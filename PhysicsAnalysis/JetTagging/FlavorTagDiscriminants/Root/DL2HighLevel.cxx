/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "FlavorTagDiscriminants/DL2HighLevel.h"
#include "FlavorTagDiscriminants/DL2.h"

#include "lwtnn/parse_json.hh"
#include "lwtnn/LightweightGraph.hh"
#include "lwtnn/NanReplacer.hh"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/exceptions.hpp>

#include <fstream>

namespace {
  // define a regex literal operator
  std::regex operator "" _r(const char* c, size_t /* length */) {
    return std::regex(c);
  }
}

namespace FlavorTagDiscriminants {

  DL2HighLevel::DL2HighLevel(const std::string& nn_file_name,
                             EDMSchema schema):
    m_dl2(nullptr)
  {
    // get the graph
    std::ifstream input_stream(nn_file_name);
    lwt::GraphConfig config = lwt::parse_json_graph(input_stream);

    // __________________________________________________________________
    // build the standard inputs
    //

    // type and default value-finding regexes are hardcoded for now
    TypeRegexes type_regexes{
      {"(IP[23]D_|SV[12]_|rnnip_)p(b|c|u|tau)"_r, EDMType::DOUBLE},
      {"(JetFitter_|SV1_)[Nn].*"_r, EDMType::INT},
      {"(JetFitter_|SV1_).*"_r, EDMType::FLOAT},
      {"(pt|abs_eta|eta)"_r, EDMType::CUSTOM_GETTER}};
    StringRegexes default_flag_regexes{
      {"IP2D_.*"_r, "IP2D_isDefaults"},
      {"IP3D_.*"_r, "IP3D_isDefaults"},
      {"SV1_.*"_r, "SV1_isDefaults"},
      {"JetFitter_.*"_r, "JetFitter_isDefaults"},
      {"secondaryVtx_.*"_r, "secondaryVtx_isDefaults"},
      {"rnnip_.*"_r, "rnnip_isDefaults"},
      {"(pt|abs_eta|eta)"_r, ""}}; // no default required for custom cases

    std::vector<DL2InputConfig> input_config;
    if (config.inputs.size() == 1) {
      std::vector<std::string> input_names;
      for (const auto& var: config.inputs.at(0).variables) {
        input_names.push_back(var.name);
      }

      input_config = get_input_config(
        input_names, type_regexes, default_flag_regexes);
    } else if (config.inputs.size() > 1) {
      throw std::logic_error("DL2 doesn't support multiple inputs");
    }


    // ___________________________________________________________________
    // build the track inputs
    //
    std::vector<std::pair<std::string, std::vector<std::string> > > trk_names;
    for (const auto& node: config.input_sequences) {
      std::vector<std::string> names;
      for (const auto& var: node.variables) {
        names.push_back(var.name);
      }
      trk_names.emplace_back(node.name, names);
    }

    TypeRegexes trk_type_regexes {
      {"numberOf.*"_r, EDMType::UCHAR},
      {".*_(d|z)0.*"_r, EDMType::CUSTOM_GETTER},
      {"(log_)?(ptfrac|dr)"_r, EDMType::CUSTOM_GETTER}
    };
    SortRegexes trk_sort_regexes {
      {".*absSd0sort"_r, SortOrder::ABS_D0_SIGNIFICANCE_DESCENDING},
      {".*sd0sort"_r, SortOrder::D0_SIGNIFICANCE_DESCENDING},
      {".*ptsort"_r, SortOrder::PT_DESCENDING},
    };
    TrkSelRegexes trk_select_regexes {
      {".*_ip3d_.*"_r, TrackSelection::IP3D_2018},
      {".*_all_.*"_r, TrackSelection::ALL},
    };
    std::vector<DL2TrackSequenceConfig> trk_config = get_track_input_config(
      trk_names, trk_type_regexes, trk_sort_regexes, trk_select_regexes);

    m_dl2.reset(new DL2(config, input_config, trk_config, schema));
  }

  DL2HighLevel::~DL2HighLevel() = default;

  void DL2HighLevel::decorate(const xAOD::Jet& jet) const {
    m_dl2->decorate(jet);
  }

}
