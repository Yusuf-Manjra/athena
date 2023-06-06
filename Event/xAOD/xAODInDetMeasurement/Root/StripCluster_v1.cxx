/*
   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// EDM include(s):
#include "xAODCore/AuxStoreAccessorMacros.h"
// Local include(s):
#include "xAODInDetMeasurement/versions/StripCluster_v1.h"

static const SG::AuxElement::Accessor<std::array<float, 3> > globalPosAcc(
    "globalPosition");
static const SG::AuxElement::Accessor<std::vector<Identifier::value_type> >
    rdoListAcc("rdoList");

xAOD::ConstVectorMap<3> xAOD::StripCluster_v1::globalPosition() const {
    const auto& values = globalPosAcc(*this);
    return ConstVectorMap<3>{values.data()};
}

xAOD::VectorMap<3> xAOD::StripCluster_v1::globalPosition() {
    auto& values = globalPosAcc(*this);
    return VectorMap<3>{values.data()};
}

void xAOD::StripCluster_v1::setRDOlist(const std::vector<Identifier>& rdoList) {
    std::vector<Identifier::value_type> rdos(rdoList.size());
    for (std::size_t i(0); i < rdos.size(); ++i) {
        rdos[i] = rdoList[i].get_compact();
    }
    rdoListAcc(*this) = rdos;
}

const std::vector<Identifier> xAOD::StripCluster_v1::rdoList() const {
    const std::vector<Identifier::value_type>& values = rdoListAcc(*this);
    std::vector<Identifier> rdos(values.size());
    for (std::size_t i(0); i < rdos.size(); ++i) {
        rdos[i].set_literal(values[i]);
    }
    return rdos;
}

AUXSTORE_PRIMITIVE_SETTER_AND_GETTER(xAOD::StripCluster_v1, int, channelsInPhi,
                                     setChannelsInPhi)
