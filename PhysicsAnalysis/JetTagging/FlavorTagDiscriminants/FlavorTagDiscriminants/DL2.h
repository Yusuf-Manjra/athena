/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DL2_H
#define DL2_H

// local includes
#include "FlavorTagDiscriminants/customGetter.h"
#include "FlavorTagDiscriminants/FlipTagEnums.h"
#include "FlavorTagDiscriminants/AssociationEnums.h"
#include "FlavorTagDiscriminants/DL2DataDependencyNames.h"
#include "xAODBTagging/ftagfloat_t.h"

// EDM includes
#include "xAODJet/Jet.h"
#include "xAODBTagging/BTagging.h"

// external libraries
#include "lwtnn/lightweight_network_config.hh"

// STL includes
#include <string>
#include <vector>
#include <functional>
#include <exception>
#include <type_traits>

// forward declarations
namespace lwt {
  class NanReplacer;
  class LightweightGraph;
}

namespace FlavorTagDiscriminants {

  enum class EDMType {UCHAR, INT, FLOAT, DOUBLE, CUSTOM_GETTER};
  enum class SortOrder {
    ABS_D0_SIGNIFICANCE_DESCENDING, D0_SIGNIFICANCE_DESCENDING, PT_DESCENDING};
  enum class TrackSelection {ALL, IP3D_2018, DIPS_LOOSE_202102};


  // Structures to define DL2 input.
  //
  struct DL2InputConfig
  {
    std::string name;
    EDMType type;
    std::string default_flag;
  };
  struct DL2TrackInputConfig
  {
    std::string name;
    EDMType type;
  };
  struct DL2TrackSequenceConfig
  {
    std::string name;
    SortOrder order;
    TrackSelection selection;
    std::vector<DL2TrackInputConfig> inputs;
  };

  // other DL2 options
  struct DL2Options {
    DL2Options();
    std::string track_prefix;
    FlipTagConfig flip;
    std::string track_link_name;
    std::map<std::string,std::string> remap_scalar;
    TrackLinkType track_link_type;
  };


  // _____________________________________________________________________
  // Internal code

  namespace internal {
    // typedefs
    typedef std::pair<std::string, double> NamedVar;
    typedef std::pair<std::string, std::vector<double> > NamedSeq;
    typedef xAOD::Jet Jet;
    typedef xAOD::BTagging BTagging;
    typedef std::vector<const xAOD::TrackParticle*> Tracks;
    typedef std::function<double(const xAOD::TrackParticle*,
                                 const xAOD::Jet&)> TrackSortVar;
    typedef std::function<bool(const xAOD::TrackParticle*)> TrackFilter;
    typedef std::function<Tracks(const Tracks&,
                                 const xAOD::Jet&)> TrackSequenceFilter;

    // getter functions
    typedef std::function<NamedVar(const SG::AuxElement&)> VarFromBTag;
    typedef std::function<NamedSeq(const Jet&, const Tracks&)> SeqFromTracks;

    // ___________________________________________________________________
    // Getter functions
    //
    // internally we want a bunch of std::functions that return pairs
    // to populate the lwtnn input map. We define a functor here to
    // deal with the b-tagging cases.
    //
    template <typename T>
    class BVarGetter
    {
    private:
      typedef SG::AuxElement AE;
      AE::ConstAccessor<T> m_getter;
      AE::ConstAccessor<char> m_default_flag;
      std::string m_name;
    public:
      BVarGetter(const std::string& name, const std::string& default_flag):
        m_getter(name),
        m_default_flag(default_flag),
        m_name(name)
        {
        }
      NamedVar operator()(const SG::AuxElement& btag) const {
        T ret_value = m_getter(btag);
        bool is_default = m_default_flag(btag);
        if constexpr (std::is_floating_point<T>::value) {
          if (std::isnan(ret_value) && !is_default) {
            throw std::runtime_error(
              "Found NAN value for '" + m_name
              + "'. This is only allowed when using a default"
              " value for this input");
          }
        }
        return {m_name, is_default ? NAN : ret_value};
      }
    };

    template <typename T>
    class BVarGetterNoDefault
    {
    private:
      typedef SG::AuxElement AE;
      AE::ConstAccessor<T> m_getter;
      std::string m_name;
    public:
      BVarGetterNoDefault(const std::string& name):
        m_getter(name),
        m_name(name)
        {
        }
      NamedVar operator()(const SG::AuxElement& btag) const {
        T ret_value = m_getter(btag);
        if constexpr (std::is_floating_point<T>::value) {
          if (std::isnan(ret_value)) {
            throw std::runtime_error(
              "Found NAN value for '" + m_name + "'.");
          }
        }
        return {m_name, ret_value};
      }
    };

    // The track getter is responsible for getting the tracks from the
    // jet applying a selection, and then sorting the tracks.
    class TracksFromJet
    {
    public:
      TracksFromJet(SortOrder, TrackSelection, const DL2Options&);
      Tracks operator()(const xAOD::Jet& jet,
                        const SG::AuxElement& btag) const;
    private:
      using AE = SG::AuxElement;
      using IPC = xAOD::IParticleContainer;
      using TPC = xAOD::TrackParticleContainer;
      using TrackLinks = std::vector<ElementLink<TPC>>;
      using PartLinks = std::vector<ElementLink<IPC>>;
      using TPV = std::vector<const xAOD::TrackParticle*>;
      std::function<TPV(const SG::AuxElement&)> m_associator;
      TrackSortVar m_trackSortVar;
      TrackFilter m_trackFilter;
    };

    // The sequence getter takes in tracks and calculates arrays of
    // values which are better suited for inputs to the NNs
    template <typename T>
    class SequenceGetter
    {
    private:
      SG::AuxElement::ConstAccessor<T> m_getter;
      std::string m_name;
    public:
      SequenceGetter(const std::string& name):
        m_getter(name),
        m_name(name)
        {
        }
      NamedSeq operator()(const xAOD::Jet&, const Tracks& trks) const {
        std::vector<double> seq;
        for (const xAOD::TrackParticle* track: trks) {
          seq.push_back(m_getter(*track));
        }
        return {m_name, seq};
      }
    };
  } // end internal namespace
  class DL2
  {
  public:
    DL2(const lwt::GraphConfig&,
        const std::vector<DL2InputConfig>&,
        const std::vector<DL2TrackSequenceConfig>& = {},
        const DL2Options& = DL2Options());
    void decorate(const xAOD::BTagging& btag) const;
    void decorate(const xAOD::Jet& jet) const;
    void decorate(const xAOD::Jet& jet, const SG::AuxElement& decorated) const;

    // functions to report data depdedencies
    DL2DataDependencyNames getDataDependencyNames() const;

  private:
    struct TrackSequenceBuilder {
      TrackSequenceBuilder(SortOrder,
                           TrackSelection,
                           const DL2Options&);
      std::string name;
      internal::TracksFromJet tracksFromJet;
      internal::TrackSequenceFilter flipFilter;
      std::vector<internal::SeqFromTracks> sequencesFromTracks;
    };
    typedef SG::AuxElement::Decorator<float> OutputSetter;
    typedef std::vector<std::pair<std::string, OutputSetter > > OutNode;
    SG::AuxElement::ConstAccessor<ElementLink<xAOD::JetContainer>> m_jetLink;
    std::string m_input_node_name;
    std::unique_ptr<lwt::LightweightGraph> m_graph;
    std::unique_ptr<lwt::NanReplacer> m_variable_cleaner;
    std::vector<internal::VarFromBTag> m_varsFromBTag;
    std::vector<TrackSequenceBuilder> m_trackSequenceBuilders;
    std::map<std::string, OutNode> m_decorators;

    DL2DataDependencyNames m_dataDependencyNames;

  };


  //
  // Filler functions
  namespace internal {
    // factory functions to produce callable objects that build inputs
    namespace get {
      VarFromBTag varFromBTag(const std::string& name,
                              EDMType,
                              const std::string& defaultflag);
      TrackSortVar trackSortVar(SortOrder, const DL2Options&);
      std::pair<TrackFilter,std::set<std::string>> trackFilter(
        TrackSelection, const DL2Options&);
      std::pair<SeqFromTracks,std::set<std::string>> seqFromTracks(
        const DL2TrackInputConfig&, const DL2Options&);
      std::pair<TrackSequenceFilter,std::set<std::string>> flipFilter(
        const DL2Options&);
    }
  }
}
#endif
