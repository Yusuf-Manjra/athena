/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfL1Data/ThresholdMonitor.h"
#include "TrigConfL1Data/ClusterThresholdValue.h"
#include "TrigConfL1Data/JetThresholdValue.h"
#include "TrigConfL1Data/MuonThresholdValue.h"
#include "TrigConfL1Data/EtThresholdValue.h"
#include "TrigConfL1Data/NimThresholdValue.h"
#include "TrigConfL1Data/L1DataDef.h"
#include "TrigConfL1Data/CaloInfo.h"

#include <stdexcept>
#include <iostream>

using namespace std;

TrigConf::ThresholdMonitor::ThresholdMonitor() :
   L1DataBaseclass()
{}
  
TrigConf::ThresholdMonitor::ThresholdMonitor(const ThresholdMonitor& mon) :
   L1DataBaseclass(mon),
   m_CounterType(mon.m_CounterType), 
   m_ThresholdName(mon.m_ThresholdName),
   m_CtpinSlot(mon.m_CtpinSlot),
   m_CtpinConnector(mon.m_CtpinConnector),
   m_ThresholdId(mon.m_ThresholdId),
   m_BunchGroupId(mon.m_BunchGroupId),
   m_Multiplicity(mon.m_Multiplicity),
   m_InternalCounter(mon.m_InternalCounter),
   m_ThresholdStartBit(mon.m_ThresholdStartBit),
   m_ThresholdEndBit(mon.m_ThresholdEndBit),
   m_ThresholdActive(mon.m_ThresholdActive)
{
   setName(mon.name());
   setVersion(mon.version());
}
 
TrigConf::ThresholdMonitor::~ThresholdMonitor()
{}
   
void
TrigConf::ThresholdMonitor::print(const std::string& indent, unsigned int /*detail*/) const {
   std::cout << indent << "MonCounter : '" << name() << "'" << std::endl;
   std::cout << indent << "ThresholdId: " << m_ThresholdId << std::endl;
   std::cout << indent << "ThresholdName: " << m_ThresholdName << std::endl;
   std::cout << indent << "Ctpin slot: " << m_CtpinSlot << std::endl;
   std::cout << indent << "Connector: " << m_CtpinConnector << std::endl;
   std::cout << indent << "Multiplicity: " << m_Multiplicity << std::endl;
   std::cout << indent << "Start bit: " << m_ThresholdStartBit << std::endl; 
   std::cout << indent << "End bit: " << m_ThresholdEndBit << std::endl; 
   std::cout << indent << "Active: " << m_ThresholdActive << std::endl;
   std::cout << indent << "InternalCounter: " << m_InternalCounter << std::endl;
   std::cout << indent << "BunchGroupId: " << m_BunchGroupId << std::endl;
   std::cout << indent << "CounterType: " << m_CounterType << std::endl;
}


void
TrigConf::ThresholdMonitor::writeXML(std::ostream & xmlfile, int indentLevel, int indentWidth) const {
   indent(xmlfile, indentLevel, indentWidth) << "<TriggerCounter name=\"" << name() << "\" type=\"CTPIN\">" << endl;
   indent(xmlfile, indentLevel+1, indentWidth) << "<TriggerCondition name=\"" << name() << "\" triggerthreshold=\"" << m_ThresholdName << "\" multi=\"" << m_Multiplicity << "\"/>" << endl;
   indent(xmlfile, indentLevel, indentWidth) << "</TriggerCounter>" << endl;
}
