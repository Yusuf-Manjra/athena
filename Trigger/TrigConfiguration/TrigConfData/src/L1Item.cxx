/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfData/L1Item.h"

TrigConf::L1Item::L1Item()
{}

TrigConf::L1Item::L1Item(const boost::property_tree::ptree & data) 
   : DataStructure(data)
{}

TrigConf::L1Item::~L1Item()
{}

const std::string &
TrigConf::L1Item::name() const
{
   return m_data.get_child("name").data();
}

unsigned int
TrigConf::L1Item::ctpId() const
{
   return m_data.get_child("ctpid").get_value<unsigned int>();
}

const std::string &
TrigConf::L1Item::definition() const
{
   return m_data.get_child("definition").data();
}

unsigned int
TrigConf::L1Item::complexDeadtime() const
{
   return m_data.get_child("complexDeadtime").get_value<unsigned int>();
}

const std::string &
TrigConf::L1Item::monitor() const
{
   return m_data.get_child("monitor").data();
}

unsigned int
TrigConf::L1Item::partition() const
{
   return m_data.get_child("partition").get_value<unsigned int>();
}

const std::string &
TrigConf::L1Item::triggerType() const
{
   return m_data.get_child("triggerType").data();
}

TrigConf::DataStructure
TrigConf::L1Item::logic() const
{
   return getObject("AND");
}
