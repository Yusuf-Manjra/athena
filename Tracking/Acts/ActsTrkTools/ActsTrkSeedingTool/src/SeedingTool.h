/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ACTSTRKSEEDINGTOOL_SEEDINGTOOL_H
#define ACTSTRKSEEDINGTOOL_SEEDINGTOOL_H 1

// ATHENA
#include "ActsTrkToolInterfaces/ISeedingTool.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "BeamSpotConditionsData/BeamSpotData.h"
#include "MagFieldConditions/AtlasFieldCacheCondObj.h"
#include "MagFieldElements/AtlasFieldCache.h"

#include "StoreGate/ReadCondHandleKey.h"
#include "ActsGeometry/ATLASMagneticFieldWrapper.h"

// ACTS CORE
#include "Acts/Definitions/Units.hpp"
#include "Acts/Definitions/Common.hpp"
#include "Acts/Definitions/Algebra.hpp"
#include "Acts/MagneticField/MagneticFieldContext.hpp"
#include "Acts/Seeding/SpacePointGrid.hpp"
#include "Acts/Seeding/BinFinder.hpp"
#include "Acts/Seeding/BinnedSPGroup.hpp"
#include "Acts/Seeding/SeedfinderConfig.hpp"
#include "Acts/Seeding/SeedFilterConfig.hpp"
#include "Acts/Seeding/SeedFilter.hpp"
#include "Acts/Seeding/Seedfinder.hpp"
#include "Acts/Seeding/Seed.hpp"

#include <math.h>

namespace ActsTrk {
  
  class SeedingTool :
    public extends<AthAlgTool, ActsTrk::ISeedingTool> {
    
  public:
    SeedingTool(const std::string& type, const std::string& name,
		    const IInterface* parent);
    virtual ~SeedingTool() = default;
    
    virtual StatusCode initialize() override;
    
    // Interface
    virtual StatusCode
      createSeeds(const EventContext& ctx,
		  const std::vector<const ActsTrk::SpacePoint*>& spContainer,
		  const InDet::BeamSpotData& beamSpotData,
		  const Acts::MagneticFieldContext& magFieldContext,
		  ActsTrk::SeedContainer& seedContainer ) const override;
    
  private:        
    // metafunction to obtain correct type in iterated container given the iterator type
    template<typename spacepoint_iterator_t>
    struct external_spacepoint {
      using type = typename std::conditional< 
                       std::is_pointer< typename spacepoint_iterator_t::value_type >::value,  
                       typename std::remove_const< typename std::remove_pointer< typename spacepoint_iterator_t::value_type >::type >::type,
                       typename std::remove_const< typename spacepoint_iterator_t::value_type >::type
                       >::type;
    };

    template< typename spacepoint_iterator_t >
      StatusCode
      createSeeds( spacepoint_iterator_t spBegin,
		   spacepoint_iterator_t spEnd,
		   const InDet::BeamSpotData& beamSpotData,
		   const Acts::MagneticFieldContext& magFieldContext, 
       std::vector< Acts::Seed< typename external_spacepoint<spacepoint_iterator_t>::type > >&  seeds) const;
    
    template< typename external_spacepoint_t >
      const std::pair< 
                   Acts::SpacePointGridConfig, 
                   Acts::SeedfinderConfig< external_spacepoint_t > 
      > 
      prepareConfiguration( const Acts::Vector2& beamPos, 
			    const Acts::Vector3& bField ) const;
    
    // *********************************************************************
    // *********************************************************************
  private:
    // Properties
    Gaudi::Property< std::vector<std::pair<int, int>> > m_zBinNeighborsTop{this, "zBinNeighborsTop", {}, ""};
    Gaudi::Property< std::vector<std::pair<int, int>> > m_zBinNeighborsBottom{this, "zBinNeighborsBottom", {}, ""};

    // Used by SeedfinderConfig
    Gaudi::Property< float > m_minPt {this,"minPt",500. * Acts::UnitConstants::MeV,
	"lower cutoff for seeds"}; // Also used by SpacePointGridConfig
    Gaudi::Property< float > m_cotThetaMax {this,"cotThetaMax",7.40627,
	"cot of maximum theta angle"}; // Also used by SpacePointGridConfig 

    Gaudi::Property< float > m_deltaRMin {this, "deltaRMin", 5. * Acts::UnitConstants::mm,
	"minimum distance in r between two measurements within one seed"}; // Also used by SeedFilterConfig
    Gaudi::Property< float > m_deltaRMax {this, "deltaRMax", 60. * Acts::UnitConstants::mm,
	"maximum distance in r between two measurements within one seed"}; // Also used by SpacePointGridConfig 
    Gaudi::Property< float > m_deltaRMinTopSP {this, "deltaRMinTopSP", 5. * Acts::UnitConstants::mm,
	"minimum distance in r between middle and top SP"}; 
    Gaudi::Property< float > m_deltaRMaxTopSP {this, "deltaRMaxTopSP", 60. * Acts::UnitConstants::mm,
	"maximum distance in r between middle and top SP"}; 
    Gaudi::Property< float > m_deltaRMinBottomSP {this, "deltaRMinBottomSP", 5. * Acts::UnitConstants::mm,
	"minimum distance in r between middle and top SP"}; 
    Gaudi::Property< float > m_deltaRMaxBottomSP {this, "deltaRMaxBottomSP", 60. * Acts::UnitConstants::mm,
	"maximum distance in r between middle and top SP"};

    Gaudi::Property< std::vector<std::vector<double>> > m_rRangeMiddleSP {this, "rRangeMiddleSP", {},
  "radial range for middle SP"};
    Gaudi::Property<bool> m_useVariableMiddleSPRange {this, "useVariableMiddleSPRange", false,
	"use variable middle SP range or not"};
    Gaudi::Property< float > m_deltaRMiddleSPRange {this, "deltaRMiddleSPRange", 10. /** Acts::UnitConstants::mm*/, // this will have to change to ACTS units, but right now isn't correctly converted on the ACTS side.
	"delta R for middle SP range"};


    Gaudi::Property<bool> m_seedConfirmation {this, "seedConfirmation", false,
  "run seed confirmation"};

    Gaudi::Property<float> m_seedConfCentralZMin {this, "seedConfCentralZMin", 0,
  "minimum z for central seed confirmation "};
    Gaudi::Property<float> m_seedConfCentralZMax {this, "seedConfCentralZMax", 0,
  "maximum z for central seed confirmation "};
    Gaudi::Property<float> m_seedConfCentralRMax {this, "seedConfCentralRMax", 0,
  "maximum r for central seed confirmation "};
    Gaudi::Property<size_t> m_seedConfCentralNTopLR {this, "seedConfCentralNTopLR", 0,
  "nTop for large R central seed confirmation"};
    Gaudi::Property<size_t> m_seedConfCentralNTopSR {this, "seedConfCentralNTopSR", 0,
  "nTop for small R central seed confirmation"};

    Gaudi::Property<float> m_seedConfForwardZMin {this, "seedConfForwardZMin", 0,
  "minimum z for forward seed confirmation "};
    Gaudi::Property<float> m_seedConfForwardZMax {this, "seedConfForwardZMax", 0,
  "maximum z for forward seed confirmation "};
    Gaudi::Property<float> m_seedConfForwardRMax {this, "seedConfForwardRMax", 0,
  "maximum r for forward seed confirmation "};
    Gaudi::Property<size_t> m_seedConfForwardNTopLR {this, "seedConfForwardNTopLR", 0,
  "nTop for large R forward seed confirmation"};
    Gaudi::Property<size_t> m_seedConfForwardNTopSR {this, "seedConfForwardNTopSR", 0,
  "nTop for small R forward seed confirmation"};

    Gaudi::Property<bool> m_skipPreviousTopSP {this, "skipPreviousTopSP", false,
  "additional cut to skip top SPs when producing triplets"};


    Gaudi::Property< float > m_impactMax {this,"impactMax",3. * Acts::UnitConstants::mm,
	"maximum impact parameter"}; // Also used by SpacePointGridConfig 
    Gaudi::Property< float > m_sigmaScattering {this,"sigmaScattering",5,
	"how many sigmas of scattering angle should be considered"};    
    Gaudi::Property< float > m_maxPtScattering {this,"maxPtScattering",10 * Acts::UnitConstants::mm,
	"Upper pt limit for scattering calculation"};
    Gaudi::Property< float > m_maxSeedsPerSpM {this,"maxSeedsPerSpM",5,
	"In dense environments many seeds may be found per middle space point. Only seeds with the highest weight will be kept if this limit is reached."}; // Also used by SeedFilterConfig
    Gaudi::Property< float > m_collisionRegionMin {this,"collisionRegionMin",-250. * Acts::UnitConstants::mm,
	"limiting location of collision region in z"};
    Gaudi::Property< float > m_collisionRegionMax {this,"collisionRegionMax",250. * Acts::UnitConstants::mm,
	"limiting location of collision region in z"};
    Gaudi::Property< float > m_phiMin {this,"phiMin",-M_PI,
	"limiting location of collision region in z"};
    Gaudi::Property< float > m_phiMax {this,"phiMax",M_PI,
	"limiting location of collision region in z"};
    Gaudi::Property< float > m_zMin {this,"zMin",-2000. * Acts::UnitConstants::mm,
	"limiting location of measurements"}; // Also used by SpacePointGridConfig 
    Gaudi::Property< float > m_zMax {this,"zMax",2000. * Acts::UnitConstants::mm,
	"limiting location of measurements"}; // Also used by SpacePointGridConfig 
    Gaudi::Property< float > m_rMax {this,"rMax",200. * Acts::UnitConstants::mm,
	"limiting location of measurements"}; // Also used by SpacePointGridConfig 
    Gaudi::Property< float > m_rMin {this,"rMin",33 * Acts::UnitConstants::mm,
	"limiting location of measurements"}; 
    Gaudi::Property< float > m_radLengthPerSeed {this,"radLengthPerSeed",0.5,
	"average radiation lengths of material on the length of a seed. used for scattering"};
    Gaudi::Property< float > m_zAlign {this,"zAlign",0 * Acts::UnitConstants::mm,
	"alignment uncertainties, used for uncertainties in the non-measurement-plane of the modules which otherwise would be 0."
	"will be added to spacepoint measurement uncertainties (and therefore also multiplied by sigmaError)"};
    Gaudi::Property< float > m_rAlign {this,"rAlign",0 * Acts::UnitConstants::mm,
	"alignment uncertainties, used for uncertainties in the non-measurement-plane of the modules which otherwise would be 0."
	"will be added to spacepoint measurement uncertainties (and therefore also multiplied by sigmaError)"};
    Gaudi::Property< float > m_sigmaError {this,"sigmaError",5,
      "used for measurement (+alignment) uncertainties. Find seeds within 5sigma error ellipse"};    

    // Used by SeedFilterConfig
    Gaudi::Property< float > m_deltaInvHelixDiameter {this,"deltaInvHelixDiameter", 0.00003 * 1. / Acts::UnitConstants::mm, 
	"the allowed delta between two inverted seed radii for them to be considered compatible."};
    Gaudi::Property< float > m_impactWeightFactor {this,"impactWeightFactor",1.,
	"the impact parameters (d0) is multiplied by this factor and subtracted from weight"};
    Gaudi::Property< float > m_compatSeedWeight {this,"compatSeedWeight",200.,
	"seed weight increased by this value if a compatible seed has been found"};  
    Gaudi::Property< std::size_t > m_compatSeedLimit {this,"compatSeedLimit",2,
	"how often do you want to increase the weight of a seed for finding a compatible seed"};

    // Used by SpacePointGridConfig
    Gaudi::Property< int > m_numPhiNeighbors {this, "numPhiNeighbors", 1, 
	"sets of consecutive phi bins in the seed making step"};
    Gaudi::Property< int > m_phiBinDeflectionCoverage {this, "phiBinDeflectionCoverage", 1, 
	"number of bins to capture deflection"};
    Gaudi::Property< std::vector<float> > m_zBinEdges {this, "zBinEdges", {} , 
	"enable non equidistant binning in z"}; // Also used by SpacePointGridConfig 

  };
  
} // namespace

#endif

