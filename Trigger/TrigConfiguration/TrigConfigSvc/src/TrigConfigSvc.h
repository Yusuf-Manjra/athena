/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////
//
// NAME:     TrigConfigSvc.h
// PACKAGE:  TrigConfig
//
// AUTHORs:  Till Eifert, University of Geneva
//
// CREATED:  27-Dec-2007
//
// PURPOSE:
//
//////////////////////////////////////////////////////////////////////

#ifndef TrigConfig_TrigConfigSvc
#define TrigConfig_TrigConfigSvc

#include "AthenaBaseComps/AthService.h"
#include "TrigConfInterfaces/ITrigConfigSvc.h"
#include "TrigConfL1Data/CTPConfig.h"
#include "TrigConfHLTData/HLTFrame.h"

#include <stdint.h>


namespace TrigConf {

   enum TrigConfSrc {DS = 0, XML, XMLL1, XMLHLT };

   class TrigConfigSvc : public AthService, 
                         virtual public ITrigConfigSvc
   {
   public:

      TrigConfigSvc( const std::string& name, ISvcLocator* pSvcLocator );

      virtual ~TrigConfigSvc(){}

      virtual StatusCode initialize() override;

      virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvIF ) override;
      static const InterfaceID& interfaceID() { return ITrigConfigSvc::interfaceID(); }

      // L1Topo menu
      virtual const TXC::L1TopoMenu* menu() const override;

      // Lvl1 Accessors
      virtual const Muctpi*          muctpiConfig() const override;
      virtual const CTPConfig*       ctpConfig() const override;
      virtual const ThresholdConfig* thresholdConfig() const override;
      virtual const BunchGroupSet*   bunchGroupSet() const override;

      // HLT accessors
      virtual const HLTChainList*    chainList() const override;
      virtual const HLTChainList&    chains() const override;
      virtual const HLTSequenceList* sequenceList() const override;
      virtual const HLTSequenceList& sequences() const override;

      // Super Master Key - 0 if from XML, other cases TBI.
      virtual uint32_t masterKey() const override;
      virtual uint32_t lvl1PrescaleKey() const override;
      virtual uint32_t hltPrescaleKey() const override;
      virtual std::string configurationSource() const override {
         return "";
      }

      virtual StatusCode updatePrescaleSets(uint requestcount) override;

      virtual StatusCode assignPrescalesToChains(uint lumiblock ) override;

   private:

      // jobProperties
      ServiceHandle<TrigConf::IL1TopoConfigSvc> m_l1topoSvc; //!< read from xml/db
      ServiceHandle<TrigConf::ILVL1ConfigSvc>   m_lvl1Svc; //!< read from xml/db
      ServiceHandle<TrigConf::IHLTConfigSvc>    m_hltSvc;  //!< read from xml/db
      ServiceHandle<TrigConf::ITrigConfigSvc >  m_dsSvc;  //!< read from detectorStore (header)
      std::vector<std::string> m_priorityList;
      
      TrigConf::IL1TopoConfigSvc* m_l1toposervice = nullptr;    //!< the available l1 service that will be used
      TrigConf::ILVL1ConfigSvc*   m_l1service = nullptr;    //!< the available l1 service that will be used
      TrigConf::IHLTConfigSvc*    m_hltservice = nullptr;   //!< the available hlt service that will be used

      HLTFrame m_NullFrame;
   };

}

#endif
