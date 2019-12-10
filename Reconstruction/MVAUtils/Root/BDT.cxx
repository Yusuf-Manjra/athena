/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "MVAUtils/BDT.h"
#include "MVAUtils/ForestTMVA.h"
#include "MVAUtils/ForestLGBM.h"

#include "TTree.h"
#include <stack>
#include <string>
#include <sstream>
#include <set>
#include <memory>
#include <stdexcept>

using namespace MVAUtils;

namespace{

/*  utility functions : to split option (e.g. "creator=lgbm;node=lgbm_simple")
*  in a std::map {{"creator", "lgbm"}, {"node", "lgbm_simple"}}
*/
std::string get_default_string_map(const std::map <std::string, std::string> & m,
                                   const std::string& key, const std::string & defval="")
{
   std::map<std::string, std::string>::const_iterator it = m.find(key);
   if (it == m.end()) { return defval; }
   else { return it->second; }
}

std::map<std::string, std::string> parseOptions(std::string raw_options)
{
  std::stringstream ss(raw_options);
  std::map<std::string, std::string> options;
  std::string item;
  while (std::getline(ss, item, ';')) {
    auto pos = item.find_first_of('=');
    const auto right = item.substr(pos+1);
    const auto left = item.substr(0, pos);
    if (!options.insert(std::make_pair(left, right)).second)
    {
      throw std::runtime_error(std::string("option ") + left + " duplicated in title of TTree used as input");
    }
  }

  return options;
}
}

/** c-tor from TTree **/
BDT::BDT(::TTree *tree)
{
  // at runtime decide which flavour of BDT we need to build
  // the information is coming from the title of the TTree
  std::map<std::string, std::string> options = parseOptions(tree->GetTitle());
  std::string creator = get_default_string_map(options, std::string("creator"));
  if (creator == "lgbm")
  {
    std::string node_type = get_default_string_map (options, std::string("node_type"));
    if (node_type == "lgbm") {
      m_forest = std::make_unique<ForestLGBM>(tree);
    }
    else if (node_type == "lgbm_simple") {
      m_forest = std::make_unique<ForestLGBMSimple>(tree);  // this do not support nan as inputs
    }
    else
    {
      throw std::runtime_error("the title of the input tree is misformatted: cannot understand which BDT implementation to use");
    }
  }
  else {
    // default for compatibility: old TTree (based on TMVA) don't have a special title
    m_forest = std::make_unique<ForestTMVA>(tree);
  }
}


unsigned int BDT::GetNTrees() const { return m_forest->GetNTrees(); }  
int BDT::GetNVars() const { return m_forest->GetNVars(); }
float BDT::GetOffset() const { return m_forest->GetOffset(); }
 
/** Return offset + the sum of the response of each tree  **/
float BDT::GetResponse(const std::vector<float>& values) const
{
  return m_forest->GetResponse(values);
}


/** Return offset + the sum of the response of each tree  **/
float BDT::GetResponse(const std::vector<float*>& pointers) const
{
  return m_forest->GetResponse(pointers);
}


float BDT::GetClassification(const std::vector<float>& values) const
{
  return m_forest->GetClassification(values);
}


float BDT::GetClassification(const std::vector<float*>& pointers) const
{
  return m_forest->GetClassification(pointers);
}

float BDT::GetGradBoostMVA(const std::vector<float>& values) const
{
  const float sum = m_forest->GetRawResponse(values);  // ignores the offset
  return 2. / (1 + exp(-2 * sum)) - 1;  //output shaping for gradient boosted decision tree (-1,1)
}

float BDT::GetGradBoostMVA(const std::vector<float*>& pointers) const
{
  const float sum = m_forest->GetRawResponse(pointers);  // ignores the offset
  return 2. / (1 + exp(-2 * sum)) - 1;  //output shaping for gradient boosted decision tree (-1,1)
}


std::vector<float> BDT::GetMultiResponse(const std::vector<float>& values,
                                         unsigned int numClasses) const
{
  return m_forest->GetMultiResponse(values, numClasses);
}


std::vector<float> BDT::GetMultiResponse(const std::vector<float*>& pointers,
                                         unsigned int numClasses) const
{
  return m_forest->GetMultiResponse(pointers, numClasses);
}


float BDT::GetTreeResponse(const std::vector<float>& values, MVAUtils::index_t index) const
{
  return m_forest->GetTreeResponse(values, index);
}


float BDT::GetTreeResponse(const std::vector<float*>& pointers, MVAUtils::index_t index) const
{
  return m_forest->GetTreeResponse(pointers, index);
}

TTree* BDT::WriteTree(TString name) const { return m_forest->WriteTree(name); }

void BDT::PrintForest() const { m_forest->PrintForest(); }
void BDT::PrintTree(unsigned int itree) const { m_forest->PrintTree(itree); }

