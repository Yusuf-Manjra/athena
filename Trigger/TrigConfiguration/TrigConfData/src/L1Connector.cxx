/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfData/L1Connector.h"
using namespace std;

#include <stdexcept>

TrigConf::L1Connector::L1Connector()
{}

TrigConf::L1Connector::L1Connector(const std::string & connName, const boost::property_tree::ptree & data) 
   : DataStructure(data)
{
   m_name = connName;
   L1Connector::update();
}

TrigConf::L1Connector::~L1Connector()
{}

std::string
TrigConf::L1Connector::className() const {
   return "L1Connector";
}

void
TrigConf::L1Connector::update()
{
   m_name = getAttribute("name", true, m_name);

   std::string connType(getAttribute("type"));
   if( connType == "electrical" ) {
      m_type = ConnectorType::ELECTRICAL;      
   } else if( connType == "optical" ) {
      m_type = ConnectorType::OPTICAL;
   } else if( connType == "ctpin" ) {
      m_type = ConnectorType::CTPIN;
   } else {
      throw std::runtime_error("Unknown connector type " + connType);
   }

   // triggerlines
   bool hasMultipleFPGAs = ! hasChild("triggerlines.clock0"); // connector from merger board (no fpga)
   if(m_type == ConnectorType::ELECTRICAL) {
      m_maxClock = 2;
      m_maxFpga = hasMultipleFPGAs ? 2 : 1;
   }

   for( size_t fpga = 0; fpga < m_maxFpga; ++fpga ) {
      for( size_t clock = 0; clock < m_maxClock; ++clock ) {
         std::string path = "triggerlines";
         if( m_type == ConnectorType::ELECTRICAL ) {
	    if(hasMultipleFPGAs) {
 	       path += ".fpga";
	       path += std::to_string(fpga);
	    }
            path += ".clock";
            path += std::to_string(clock);
         }
         const auto & triggerlines = data().get_child(path);
         m_triggerLines[fpga][clock].reserve(triggerlines.size());
         for( auto & tl : triggerlines ) {
            const std::string & name = tl.second.get_child("name").data();
            m_triggerLines[fpga][clock].emplace_back( name,
                                                      tl.second.get_child("startbit").get_value<unsigned int>(),
                                                      tl.second.get_child("nbits").get_value<unsigned int>(),
                                                      fpga, clock, m_name);
            m_lineByName[name] = & m_triggerLines[fpga][clock].back();
         }
      }
   }
   m_isLegacy = getAttribute<bool>("legacy", true, false);
}


std::string
TrigConf::L1Connector::type() const 
{
   switch( m_type ) {
   case ConnectorType::ELECTRICAL:
      return "electrical";
   case ConnectorType::OPTICAL:
      return "optical";
   case ConnectorType::CTPIN:
      return "ctpin";
   }
   return "";
}

TrigConf::L1Connector::ConnectorType
TrigConf::L1Connector::connectorType() const 
{
   return m_type;
}

std::size_t
TrigConf::L1Connector::size() const
{
   size_t nlines{0};
   for( size_t fpga = 0; fpga<m_maxFpga; ++fpga) {
      for( size_t clock = 0; clock<m_maxClock; ++clock) {
         nlines += m_triggerLines[fpga][clock].size();
      }
   }
   return nlines;
}

std::vector<std::string>
TrigConf::L1Connector::triggerLineNames() const
{
   std::vector<std::string> tln{};
   for( size_t fpga = 0; fpga<m_maxFpga; ++fpga) {
      for( size_t clock = 0; clock<m_maxClock; ++clock) {
         for( auto & tl : m_triggerLines[fpga][clock] ) {
            tln.emplace_back(tl.name());
         }
      }
   }
   return tln;
}

const std::vector<TrigConf::TriggerLine> &
TrigConf::L1Connector::triggerLines(unsigned int fpga, unsigned int clock) const
{
   return m_triggerLines[fpga][clock];
}

bool
TrigConf::L1Connector::hasLine( const std::string & lineName ) const
{
   return m_lineByName.count(lineName);
}

const TrigConf::TriggerLine & 
TrigConf::L1Connector::triggerLine( const std::string & lineName ) const
{
   return *m_lineByName.at(lineName);
}
