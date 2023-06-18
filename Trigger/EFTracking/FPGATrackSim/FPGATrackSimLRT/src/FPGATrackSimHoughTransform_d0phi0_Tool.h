// Copyright (C) 2002-2023 CERN for the benefit of the ATLAS collaboration

#ifndef FPGATrackSimHOUGHTRANSFORM_D0PHI0_TOOL_H
#define FPGATrackSimHOUGHTRANSFORM_D0PHI0_TOOL_H

/**
 * @file FPGATrackSimHoughTransform_d0phi0_Tool.h
 * @brief Implements road finding using a Hough transform in d0 vs phi0, assuming q/pt = 0 
 *        (i.e. a straight track).
 *
 *
 */

#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "FPGATrackSimObjects/FPGATrackSimTypes.h"
#include "FPGATrackSimObjects/FPGATrackSimVectors.h"
#include "FPGATrackSimObjects/FPGATrackSimRoad.h"
#include "FPGATrackSimObjects/FPGATrackSimHit.h"
#include "FPGATrackSimObjects/FPGATrackSimTrackPars.h"
#include "FPGATrackSimHough/IFPGATrackSimRoadFinderTool.h"
#include "FPGATrackSimBanks/IFPGATrackSimBankSvc.h"
#include "FPGATrackSimMaps/IFPGATrackSimMappingSvc.h"
  
#include "TFile.h"

#include <string>
#include <vector>
#include <utility>
#include <unordered_set>

class IFPGATrackSimBankSvc;
class IFPGATrackSimMappingSvc;


class FPGATrackSimHoughTransform_d0phi0_Tool : public extends <AthAlgTool, IFPGATrackSimRoadFinderTool>
{
    public:

        ///////////////////////////////////////////////////////////////////////
        // AthAlgTool

        FPGATrackSimHoughTransform_d0phi0_Tool(const std::string&, const std::string&, const IInterface*);

        virtual StatusCode initialize() override;
        virtual StatusCode finalize() override;

        ///////////////////////////////////////////////////////////////////////
        // IFPGATrackSimRoadFinderTool

        virtual StatusCode getRoads(const std::vector<const FPGATrackSimHit*> & hits, std::vector<FPGATrackSimRoad*> & roads) override;

    private:

        ///////////////////////////////////////////////////////////////////////
        // Handles

	ServiceHandle<IFPGATrackSimBankSvc> m_FPGATrackSimBankSvc {this, "FPGATrackSimBankSvc", "FPGATrackSimBankSvc"};
	ServiceHandle<IFPGATrackSimMappingSvc> m_FPGATrackSimMapping {this, "FPGATrackSimMappingSvc", "FPGATrackSimMappingSvc"};


        ///////////////////////////////////////////////////////////////////////
        // Properties

	Gaudi::Property <int> m_subRegion { this, "subRegion", 0," -1 for entire region (no slicing)"};
	Gaudi::Property <float> m_tempMin_phi { this, "phi_min", 0, "min phi"};
	Gaudi::Property <float> m_tempMax_phi { this, "phi_max", 1, "max phi"};
	Gaudi::Property <float> m_tempMin_d0 { this, "d0_min", -1, "min q/pt"};
	Gaudi::Property <float> m_tempMax_d0 { this, "d0_max", 1, "max q/pt"};
	Gaudi::Property<std::vector<int> > m_threshold  { this, "threshold", {1},"Minimum number of hit layers to fire a road"};
	Gaudi::Property <unsigned int> m_imageSize_x { this, "nBins_x", 1, ""};
	Gaudi::Property <unsigned int> m_imageSize_y { this, "nBins_y", 1, ""};
	Gaudi::Property<std::vector<int> > m_conv  { this, "convolution", {}, "Convolution filter, with size m_convSize_y * m_convSize_x"};
	Gaudi::Property<std::vector<unsigned int> > m_combineLayers  { this, "combine_layers", {0,1,2,3,4,5,6,7}, ""};
	Gaudi::Property<std::vector<unsigned int> > m_binScale  { this, "scale", {}, "Vector containing the scales for each layers"};
	Gaudi::Property <unsigned int> m_convSize_x { this, "convSize_x", 0, ""};
	Gaudi::Property <unsigned int> m_convSize_y { this, "convSize_y", 0, ""};
	Gaudi::Property<std::vector<unsigned int> > m_hitExtend_x  { this, "hitExtend_x", {}, "Hit lines will fill extra bins in x by this amount on each side, size == nLayers"};
	Gaudi::Property <bool> m_traceHits { this, "traceHits", true, "Trace each hit that goes in a bin. Disabling this will save memory/time since each bin doesn't have to store all its hits but the roads created won't have hits from convolution, etc."};
	Gaudi::Property <bool> m_stereo { this, "stereo", false, "Combine stereo layers"};
	Gaudi::Property <bool> m_localMaxWindowSize { this, "localMaxWindowSize", 0, "Only create roads that are a local maximum within this window size. Set this to 0 to turn off local max filtering"};
	Gaudi::Property <bool> m_useSectors { this, "useSectors", false, "Will reverse calculate the sector for track-fitting purposes"};
	Gaudi::Property <bool> m_idealGeoRoads { this, "IdealGeoRoads", false, "Set sectors to use ideal geometry fit constants"};

        FPGATrackSimTrackPars::pars_index m_par_x = FPGATrackSimTrackPars::IPHI; // sets phi as the x variable
        FPGATrackSimTrackPars::pars_index m_par_y = FPGATrackSimTrackPars::ID0; // sets d0 as the y variable

        FPGATrackSimTrackPars m_parMin; // These are the bounds of the image, i.e. the region of interest
        FPGATrackSimTrackPars m_parMax; // Only the two parameters chosen above are used

        unsigned m_nCombineLayers = 0U; // number of layers after combined
        std::vector< std::vector<unsigned> > m_combineLayer2D; // 2d array of combined layers i.e. [[1,2,3],[0,4,5],[6,7]] will combine (L1, L2, L3), (L0, L4, L5), (L6, L7)

        ///////////////////////////////////////////////////////////////////////
        // Convenience

        unsigned m_nLayers = 0U; // alias to m_FPGATrackSimMapping->PlaneMap1stStage()->getNLogiLayers();

        double m_step_x = 0; // step size of the bin boundaries in x
        double m_step_y = 0; // step size of the bin boundaries in y
        std::vector<double> m_bins_x; // size == m_imageSize_x + 1.
        std::vector<double> m_bins_y; // size == m_imageSize_y + 1
            // Bin boundaries, where m_bins_x[i] is the lower bound of bin i.
            // These are calculated from m_parMin/Max.
        std::unordered_map<int, std::vector<size_t>> m_yBins_scaled; // saved all scaled binnings

        typedef vector2D<std::pair<int, std::unordered_set<const FPGATrackSimHit*>>> Image;
            // An image is a 2d array of points, where each point has a value.
            // This starts as the number of hits (or hit layers), but will
            // change after the convolution. Also stored are all hits that
            // contributed to each bin.
            // Size m_imageSize_y * m_imageSize_x. (NOTE y is row coordinate)

        ///////////////////////////////////////////////////////////////////////
        // Event Storage

        std::vector<FPGATrackSimRoad_Hough> m_roads;

        ///////////////////////////////////////////////////////////////////////
        // Metadata and Monitoring

        int m_event = 0;
        std::string m_name; // Gets the instance name from the full gaudi name
        TFile m_monitorFile;

        ///////////////////////////////////////////////////////////////////////
        // Core

        // std::vector<FPGATrackSimHit const *> filterHits(std::vector<FPGATrackSimHit const *> const & hits) const;
        Image createLayerImage(std::vector<unsigned> const & combine_layers, std::vector<FPGATrackSimHit const *> const & hits, unsigned const scale) const;
        Image createImage(std::vector<FPGATrackSimHit const *> const & hits) const;
        Image convolute(Image const & image) const;

        ///////////////////////////////////////////////////////////////////////
        // Helpers

        double yToX(double y, FPGATrackSimHit const * h) const;
        std::pair<unsigned, unsigned> yToXBins(size_t yBin_min, size_t yBin_max, FPGATrackSimHit const * hit) const;
        bool passThreshold(Image const & image, unsigned x, unsigned y) const;
        void matchIdealGeoSector(FPGATrackSimRoad_Hough & r) const;
	FPGATrackSimRoad_Hough createRoad(std::unordered_set<const FPGATrackSimHit*> const & hits, unsigned x, unsigned y) const;
        FPGATrackSimRoad_Hough createRoad(std::vector<std::vector<const FPGATrackSimHit*>> const & hits, layer_bitmask_t hitLayers, unsigned x, unsigned y) const;
        void addRoad(std::unordered_set<const FPGATrackSimHit*> const & hits, unsigned x, unsigned y);
        void addRoad(std::vector<const FPGATrackSimHit*> const & hits, unsigned x, unsigned y);
        int conv(unsigned y, unsigned x) { return m_conv[y * m_convSize_x + x]; } // NOTE: y index is first
        void drawImage(Image const & image, std::string const & name);

};



#endif // FPGATrackSimHOUGHTRANSFORM_D0PHI0_TOOL_H
