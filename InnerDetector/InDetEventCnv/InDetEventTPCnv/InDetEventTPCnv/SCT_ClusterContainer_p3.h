/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_CLUSTERCONTAINER_P3_H
#define SCT_CLUSTERCONTAINER_P3_H

/*

Author: Davide Costanzo

*/

#include <vector>
#include <string>
#include "InDetEventTPCnv/InDetPrepRawData/SCT_Cluster_p3.h"
#include "InDetEventTPCnv/InDetPrepRawData/InDetPRD_Collection_p2.h"
#include "Identifier/Identifier.h"


namespace InDet{
class SCT_ClusterContainer_p3   
{
 public:
/// Default constructor
  SCT_ClusterContainer_p3 ();
  //private:
  std::vector<InDet::InDetPRD_Collection_p2>  m_collections;
  std::vector<InDet::SCT_Cluster_p3>      m_rawdata;
// The delta identifiers of the PRD:
  //std::vector<unsigned short>                 m_prdDeltaId;
  std::vector<Identifier::diff_type>                 m_prdDeltaId;
};


// inlines

inline
SCT_ClusterContainer_p3::SCT_ClusterContainer_p3 () {}
}

#endif
