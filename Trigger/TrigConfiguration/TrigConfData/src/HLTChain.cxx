/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfData/HLTChain.h"

TrigConf::Chain::Chain()
{}

TrigConf::Chain::Chain(const boost::property_tree::ptree & data) 
   : DataStructure(data)
{
   update();
}

void
TrigConf::Chain::update()
{
   if(! isInitialized() || empty() ) {
      return;
   }
   m_name = getAttribute("name");
}

TrigConf::Chain::~Chain()
{}

std::string
TrigConf::Chain::className() const {
   return "Chain";
}


unsigned int
TrigConf::Chain::counter() const
{
   return getAttribute<unsigned int>("counter");
}

unsigned int
TrigConf::Chain::namehash() const
{
   return getAttribute<unsigned int>("nameHash");
}

const std::string &
TrigConf::Chain::l1item() const
{
   return getAttribute("l1item");
}


std::vector<std::string>
TrigConf::Chain::l1thresholds() const
{

   std::vector<std::string> thrV;
   const auto & thrs = getList("l1thresholds");
   if( !thrs.empty() ) {
      thrV.reserve(thrs.size());
      for( auto & thr : thrs ) {
         thrV.emplace_back( thr.getValue<std::string>() );
      }
   } 
   return thrV;
}


std::vector<TrigConf::DataStructure>
TrigConf::Chain::streams() const
{
   std::vector<DataStructure> strlist;
   const auto & streams = data().get_child("streams");
   strlist.reserve(streams.size());

   for( auto & strData : streams )
      strlist.emplace_back( strData.second );
   
   return strlist;
}

std::vector<std::string>
TrigConf::Chain::groups() const
{

   std::vector<std::string> grouplist;
   const auto & groups = getList("groups", /*ignoreIfMissing=*/ true);
   if( !groups.empty() ) {
      grouplist.reserve(groups.size());
      for( auto & group : groups ) {
         if (group.hasAttribute("name")) {
            grouplist.emplace_back( group["name"] );
         } else if (group.isValue()) {
            grouplist.emplace_back( group.getValue<std::string>() );
         }
      }
   } 

   return grouplist;
}

