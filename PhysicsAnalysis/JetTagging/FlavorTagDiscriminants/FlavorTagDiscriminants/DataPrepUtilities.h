/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DATA_PREP_UTILITIES_H
#define DATA_PREP_UTILITIES_H

// local includes
#include "FlavorTagDiscriminants/customGetter.h"
#include "FlavorTagDiscriminants/FlipTagEnums.h"
#include "FlavorTagDiscriminants/AssociationEnums.h"
#include "FlavorTagDiscriminants/FTagDataDependencyNames.h"
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
#include <regex>


namespace FlavorTagDiscriminants {

  enum class EDMType {UCHAR, INT, FLOAT, DOUBLE, CUSTOM_GETTER};
  enum class SortOrder {
   ABS_D0_SIGNIFICANCE_DESCENDING,
    D0_SIGNIFICANCE_DESCENDING,
    PT_DESCENDING,
    ABS_D0_DESCENDING
  };
  enum class TrackSelection {
    ALL,
    IP3D_2018,
    DIPS_LOOSE_202102,
    LOOSE_202102_NOIP
  };


  // Structures to define DL2/GNNTool input.
  //
  struct FTagInputConfig
  {
    std::string name;
    EDMType type;
    std::string default_flag;
  };
  struct FTagTrackInputConfig
  {
    std::string name;
    EDMType type;
  };
  struct FTagTrackSequenceConfig
  {
    std::string name;
    SortOrder order;
    TrackSelection selection;
    std::vector<FTagTrackInputConfig> inputs;
  };

  // other DL2/GNNTool options
  struct FTagOptions {
    FTagOptions();
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
    typedef std::function<NamedVar(const Jet&)> VarFromJet;
    typedef std::function<NamedSeq(const Jet&, const Tracks&)> SeqFromTracks;

    // ___________________________________________________________________
    // Getter functions
    //
    // internally we want a bunch of std::functions that return pairs
    // to populate the lwtnn input map. We define a functor here to
    // deal with the b-tagging cases.
    //
    template <typename T>
    class BVarGetter {
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
    class BVarGetterNoDefault {
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
    class TracksFromJet{
      public:
        TracksFromJet(SortOrder, TrackSelection, const FTagOptions&);
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
    class SequenceGetter{
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


  // ____________________________________________________________________
  // High level adapter stuff
  //
  // We define a few structures to map variable names to type, default
  // value, etc. These are only used by the high level interface.
  //
  typedef std::vector<std::pair<std::regex, EDMType> > TypeRegexes;
  typedef std::vector<std::pair<std::regex, std::string> > StringRegexes;
  typedef std::vector<std::pair<std::regex, SortOrder> > SortRegexes;
  typedef std::vector<std::pair<std::regex, TrackSelection> > TrkSelRegexes;

  // Function to map the regular expressions + the list of inputs to a
  // list of variable configurations.
  std::vector<FTagInputConfig> get_input_config(
    const std::vector<std::string>& variable_names,
    const TypeRegexes& type_regexes,
    const StringRegexes& default_flag_regexes);

  // Since the names of the inputs are stored in the NN config, we
  // also allow some user-configured remapping. Items in replaced_vars
  // are removed as they are used.
  void remap_inputs(std::vector<lwt::Input>& nn,
                    std::map<std::string, std::string>& replaced_vars,
                    std::map<std::string, double>& defaults);

  // Function to map the regex + list of inputs to variable config,
  // this time for sequence inputs.
  std::vector<FTagTrackSequenceConfig> get_track_input_config(
    const std::vector<std::pair<std::string, std::vector<std::string>>>& names,
    const TypeRegexes& type_regexes,
    const SortRegexes& sort_regexes,
    const TrkSelRegexes& select_regexes);

  // replace strings for flip taggers
  void rewriteFlipConfig(lwt::GraphConfig&, const StringRegexes&);
  void flipSequenceSigns(lwt::GraphConfig&, const std::regex&);


  //
  // Filler functions
  namespace internal {
    // factory functions to produce callable objects that build inputs
    namespace get {
      VarFromBTag varFromBTag(const std::string& name,
                              EDMType,
                              const std::string& defaultflag);
      TrackSortVar trackSortVar(SortOrder, const FTagOptions&);
      std::pair<TrackFilter,std::set<std::string>> trackFilter(
        TrackSelection, const FTagOptions&);
      std::pair<SeqFromTracks,std::set<std::string>> seqFromTracks(
        const FTagTrackInputConfig&, const FTagOptions&);
      std::pair<TrackSequenceFilter,std::set<std::string>> flipFilter(
        const FTagOptions&);
    }
  }

  namespace internal {

    typedef SG::AuxElement::Decorator<float> OutputSetter;
    typedef std::vector<std::pair<std::string, OutputSetter > > OutNode;

    struct TrackSequenceBuilder {
      TrackSequenceBuilder(SortOrder,
                           TrackSelection,
                           const FTagOptions&);
      std::string name;
      internal::TracksFromJet tracksFromJet;
      internal::TrackSequenceFilter flipFilter;
      std::vector<internal::SeqFromTracks> sequencesFromTracks;
    };

  }


  namespace dataprep {

    void createGetterConfig( lwt::GraphConfig& graph_config,
      FlipTagConfig flip_config,
      std::map<std::string, std::string> remap_scalar,
      TrackLinkType track_link_type,
      std::vector<FTagInputConfig>& input_config,
      std::vector<FTagTrackSequenceConfig>& trk_config,
      FTagOptions& options);

    std::tuple<std::vector<internal::VarFromBTag>, 
    std::vector<internal::VarFromJet>, std::set<std::string>> 
    createBvarGetters(
      const std::vector<FTagInputConfig>& inputs);

    std::tuple<std::vector<internal::TrackSequenceBuilder>, std::set<std::string>, 
    std::set<std::string>> createTrackGetters(
      const std::vector<FTagTrackSequenceConfig>& track_sequences,
      const FTagOptions& options, const std::string& jetLinkName);

    std::tuple<std::map<std::string, internal::OutNode>, std::set<std::string>> 
    createDecorators( 
      const lwt::GraphConfig& config,
      const FTagOptions& options);
  }
}
#endif
