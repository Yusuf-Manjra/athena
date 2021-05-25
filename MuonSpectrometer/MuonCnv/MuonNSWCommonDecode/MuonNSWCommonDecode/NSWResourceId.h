/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef _MUON_NSW_RESOURCE_ID_H_
#define _MUON_NSW_RESOURCE_ID_H_

#include "eformat/SourceIdentifier.h"

#include "MuonNSWCommonDecode/NSWDecodeBitmaps.h"
#include "MuonNSWCommonDecode/NSWDecodeHelper.h"

namespace Muon
{
  namespace nsw
  {
    class NSWResourceId
    {
     private:
      uint8_t m_Elink;
      uint8_t m_Radius;
      uint8_t m_Layer;
      uint8_t m_Sector;
      uint8_t m_ResourceType;
      uint8_t m_DataType;
      uint8_t m_Version;
      uint8_t m_DetId;

      // For obsolete pre-versioned data

      uint8_t m_Eta;
      uint8_t m_Tech;

      bool m_pre_version;

     public:
      explicit NSWResourceId (uint32_t logical_id);
      virtual ~NSWResourceId () {};

      uint8_t elink          () {return m_Elink;};
      uint8_t radius         () {return m_Radius;};
      uint8_t layer          () {return m_Layer;};
      uint8_t sector         () {return m_Sector;};
      uint8_t resourceType   () {return m_ResourceType;};
      uint8_t dataType       () {return m_DataType;};
      uint8_t version        () {return m_Version;};
      uint8_t detId          () {return m_DetId;};

      // For obsolete pre-versioned data

      uint8_t group          () {return m_Elink;};
      uint8_t eta            () {return m_Eta;};
      uint8_t technology     () {return m_Tech;}

      bool pre_version       () {return m_pre_version;};
    };
  }
}

Muon::nsw::NSWResourceId::NSWResourceId (uint32_t logical_id)
{
  m_DetId        = Muon::nsw::helper::get_bits (logical_id, Muon::nsw::bitMaskDetId, Muon::nsw::bitPosDetId);

  if (m_DetId == 0) // pre-versioned logical ID data format
  {
    m_pre_version     = true;
    m_Version         = 0;
    m_Elink           = Muon::nsw::helper::get_bits (logical_id, Muon::nsw::bitMaskElink, Muon::nsw::bitPosElink);
    m_Radius          = Muon::nsw::helper::get_bits (logical_id, Muon::nsw::bitMaskRadius, Muon::nsw::bitPosRadius);
    m_Layer           = Muon::nsw::helper::get_bits (logical_id, Muon::nsw::bitMaskLayer, Muon::nsw::bitPosLayer);
    m_Sector          = Muon::nsw::helper::get_bits (logical_id, Muon::nsw::bitMaskSector, Muon::nsw::bitPosSector);
    m_Eta             = Muon::nsw::helper::get_bits (logical_id, Muon::nsw::bitMaskObsEta, Muon::nsw::bitPosObsEta);
    m_ResourceType    = Muon::nsw::helper::get_bits (logical_id, Muon::nsw::bitMaskObsResType, Muon::nsw::bitPosObsResType);
    m_Tech            = Muon::nsw::helper::get_bits (logical_id, Muon::nsw::bitMaskObsTech, Muon::nsw::bitPosObsTech);
    m_DataType        = Muon::nsw::helper::get_bits (logical_id, Muon::nsw::bitMaskObsDataType, Muon::nsw::bitPosObsDataType);
    m_DetId           = eformat::MUON_STGC_ENDCAP_A_SIDE + m_Eta - 2 * m_Tech;
  }
  else
  {
    m_pre_version  = false;
    m_Elink        = Muon::nsw::helper::get_bits (logical_id, Muon::nsw::bitMaskElink, Muon::nsw::bitPosElink);
    m_Radius       = Muon::nsw::helper::get_bits (logical_id, Muon::nsw::bitMaskRadius, Muon::nsw::bitPosRadius);
    m_Layer        = Muon::nsw::helper::get_bits (logical_id, Muon::nsw::bitMaskLayer, Muon::nsw::bitPosLayer);
    m_Sector       = Muon::nsw::helper::get_bits (logical_id, Muon::nsw::bitMaskSector, Muon::nsw::bitPosSector);
    m_ResourceType = Muon::nsw::helper::get_bits (logical_id, Muon::nsw::bitMaskResType, Muon::nsw::bitPosResType);
    m_DataType     = Muon::nsw::helper::get_bits (logical_id, Muon::nsw::bitMaskDataType, Muon::nsw::bitPosDataType);
    m_Version      = Muon::nsw::helper::get_bits (logical_id, Muon::nsw::bitMaskVersion, Muon::nsw::bitPosVersion);
  }
}

#endif // _MUON_NSW_RESOURCE_ID_H_


