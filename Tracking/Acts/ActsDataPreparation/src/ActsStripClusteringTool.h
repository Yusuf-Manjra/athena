/*
  Copyright (C) 2002-2023 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ACTSTRK_DATAPREPARATION_STRIP_CLUSTERING_TOOL_H
#define ACTSTRK_DATAPREPARATION_STRIP_CLUSTERING_TOOL_H

#include <optional>
#include <vector>

#include <Acts/Clusterization/Clusterization.hpp>

#include <AthenaBaseComps/AthAlgTool.h>
#include <ActsTrkToolInterfaces/IStripClusteringTool.h>
#include <InDetConditionsSummaryService/IInDetConditionsTool.h>
#include <InDetIdentifier/SCT_ID.h>
#include <InDetRawData/InDetRawDataCollection.h>
#include <InDetRawData/SCT_RDORawData.h>
#include <InDetReadoutGeometry/SiDetectorElement.h>
#include <InDetReadoutGeometry/SiDetectorElementStatus.h>
#include <xAODInDetMeasurement/StripClusterContainer.h>

namespace ActsTrk {


class StripClusteringTool : public extends<AthAlgTool, IStripClusteringTool> {
public:

    using StripRDORawData = SCT_RDORawData;
    using StripID = SCT_ID;

    struct Cell {
	size_t index;
	Identifier id;
	std::bitset<3> timeBits;
	Acts::Ccl::Label label{Acts::Ccl::NO_LABEL}; // required by ACTS

	Cell(size_t i, Identifier id, const std::bitset<3>& timeBits)
	    : index(i), id(id), timeBits(timeBits) {}
    };

    struct Cluster {
	std::vector<Identifier> ids;
	uint16_t hitsInThirdTimeBin{0};
    };

    using CellCollection = std::vector<Cell>;
    using ClusterCollection = std::vector<Cluster>;

    StripClusteringTool(const std::string& type,
			const std::string& name,
			const IInterface* parent);

    virtual StatusCode initialize() override;

    virtual StatusCode
    clusterize(const InDetRawDataCollection<StripRDORawData>& RDOs,
	       const StripID& stripID,
	       const InDetDD::SiDetectorElement* element,
	       const InDet::SiDetectorElementStatus *stripDetElStatus,
	       xAOD::StripClusterContainer& container) const override;

private:

    bool passTiming(const std::bitset<3>& timePattern) const;
    
    StatusCode decodeTimeBins();

    std::optional<std::pair<std::vector<Cell>, bool>>
    unpackRDOs(const InDetRawDataCollection<StripRDORawData>& RDOs,
	       const StripID& idHelper,
	       const InDet::SiDetectorElementStatus *sctDetElStatus) const;

    bool isBadStrip(const InDet::SiDetectorElementStatus *sctDetElStatus,
		    const StripID& idHelper,
		    IdentifierHash waferHash,
		    Identifier stripId) const;

    // N.B. the cluster is added to the container
    StatusCode makeCluster(const StripClusteringTool::Cluster &cluster,
			   double LorentzShift,
			   const StripID& stripID,
			   const InDetDD::SiDetectorElement* element,
			   xAOD::StripCluster& container) const;

    StringProperty m_timeBinStr{this, "timeBins", ""};

    ToolHandle<IInDetConditionsTool> m_conditionsTool{
	this,
	"StripConditionsTool",
	"SCT_ConditionsSummaryTool/InDetSCT_ConditionsSummaryTool",
	"Tool to retrieve Strip Conditions summary"
    };

    ToolHandle<ISiLorentzAngleTool> m_lorentzAngleTool{
	this,
	"LorentzAngleTool",
	"SiLorentzAngleTool/SCTLorentzAngleTool",
	"Tool to retreive Lorentz angle of Si detector module"
    };


    int m_timeBinBits[3]{-1, -1, -1};


    

};

} // namespace ActsTrk

#endif
