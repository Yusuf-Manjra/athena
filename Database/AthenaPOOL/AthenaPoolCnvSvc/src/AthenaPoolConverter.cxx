/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/** @file AthenaPoolConverter.cxx
 *  @brief This file contains the implementation for the AthenaPoolConverter base class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 **/

#include "AthenaPoolCnvSvc/AthenaPoolConverter.h"
#include "AthenaPoolCnvSvc/IAthenaPoolCnvSvc.h"

#include "SGTools/DataProxy.h"

#include "PersistentDataModel/Guid.h"
#include "PersistentDataModel/Placement.h"
#include "PersistentDataModel/Token.h"
#include "PersistentDataModel/TokenAddress.h"

//__________________________________________________________________________
AthenaPoolConverter::~AthenaPoolConverter() {
   delete m_placement; m_placement = nullptr;
   delete m_i_poolToken; m_i_poolToken = nullptr;
}
//__________________________________________________________________________
StatusCode AthenaPoolConverter::initialize() {
   if (!::Converter::initialize().isSuccess()) {
      ATH_MSG_FATAL("Cannot initialize Converter base class.");
      return(StatusCode::FAILURE);
   }
   // Retrieve AthenaPoolCnvSvc
   if (!m_athenaPoolCnvSvc.retrieve().isSuccess()) {
      ATH_MSG_FATAL("Cannot get AthenaPoolCnvSvc.");
      return(StatusCode::FAILURE);
   }
   return(StatusCode::SUCCESS);
}
//__________________________________________________________________________
StatusCode AthenaPoolConverter::finalize() {
   // Release AthenaPoolCnvSvc
   if (!m_athenaPoolCnvSvc.release().isSuccess()) {
      ATH_MSG_WARNING("Cannot release AthenaPoolCnvSvc.");
   }
   return(::Converter::finalize());
}
//__________________________________________________________________________
long AthenaPoolConverter::repSvcType() const {
   return(POOL_StorageType);
}
//__________________________________________________________________________
StatusCode AthenaPoolConverter::createObj(IOpaqueAddress* pAddr, DataObject*& pObj) {
   TokenAddress* tokAddr = dynamic_cast<TokenAddress*>(pAddr);
   bool ownTokAddr = false;
   if (tokAddr == nullptr || tokAddr->getToken() == nullptr) {
      ownTokAddr = true;
      Token* token = new Token;
      token->fromString(*(pAddr->par()));
      GenericAddress* genAddr = dynamic_cast<GenericAddress*>(pAddr);
      tokAddr = new TokenAddress(*genAddr, token);
   }
   std::lock_guard<CallMutex> lock(m_conv_mut);
   m_i_poolToken = tokAddr->getToken();
   try {
      std::string key = pAddr->par()[1];
      if (!PoolToDataObject(pObj, tokAddr->getToken(), key).isSuccess()) {
         ATH_MSG_ERROR("createObj PoolToDataObject() failed, Token = " << (tokAddr->getToken() ? tokAddr->getToken()->toString() : "NULL"));
         pObj = nullptr;
      }
   } catch (std::exception& e) {
      ATH_MSG_ERROR("createObj - caught exception: " << e.what());
      pObj = nullptr;
   }
   if (pObj == nullptr) {
      ATH_MSG_ERROR("createObj failed to get DataObject, Token = " << (tokAddr->getToken() ? tokAddr->getToken()->toString() : "NULL"));
   }
   if (ownTokAddr) {
      delete tokAddr; tokAddr = nullptr;
   }
   m_i_poolToken = nullptr;
   if (pObj == nullptr) {
      return(StatusCode::FAILURE);
   }
   return(StatusCode::SUCCESS);
}
//__________________________________________________________________________
StatusCode AthenaPoolConverter::createRep(DataObject* pObj, IOpaqueAddress*& pAddr) {
   const SG::DataProxy* proxy = dynamic_cast<SG::DataProxy*>(pObj->registry());
   if (proxy == nullptr) {
      ATH_MSG_ERROR("AthenaPoolConverter CreateRep failed to cast DataProxy, key = " << pObj->name());
      return(StatusCode::FAILURE);
   }
   try {
      std::lock_guard<CallMutex> lock(m_conv_mut);
      if (!DataObjectToPers(pObj, pAddr).isSuccess()) {
         ATH_MSG_ERROR("CreateRep failed, key = " << pObj->name());
         return(StatusCode::FAILURE);
      }
   } catch (std::exception& e) {
      ATH_MSG_ERROR("createRep - caught exception: " << e.what());
      return(StatusCode::FAILURE);
   }
   const CLID clid = proxy->clID();
   if (pAddr == nullptr) {
      // Create a IOpaqueAddress for this object.
      pAddr = new TokenAddress(this->storageType(), clid, "", "", 0, 0);
   } else {
      GenericAddress* gAddr = dynamic_cast<GenericAddress*>(pAddr);
      if (gAddr != nullptr) {
         gAddr->setSvcType(this->storageType());
      }
   }
   return(StatusCode::SUCCESS);
}
//__________________________________________________________________________
StatusCode AthenaPoolConverter::fillRepRefs(IOpaqueAddress* pAddr, DataObject* pObj) {
   std::lock_guard<CallMutex> lock(m_conv_mut);
   try {
      pObj->registry()->setAddress(pAddr);
      if (!DataObjectToPool(pAddr, pObj).isSuccess()) {
         ATH_MSG_ERROR("FillRepRefs failed, key = " << pObj->name());
         return(StatusCode::FAILURE);
      }
   } catch (std::exception& e) {
      ATH_MSG_ERROR("fillRepRefs - caught exception: " << e.what());
      return(StatusCode::FAILURE);
   }
   return(StatusCode::SUCCESS);
}
//__________________________________________________________________________
long AthenaPoolConverter::storageType() {
   return(POOL_StorageType);
}
//__________________________________________________________________________
AthenaPoolConverter::AthenaPoolConverter(const CLID& myCLID, ISvcLocator* pSvcLocator,
                                         const char* name /*= nullptr*/) :
		::Converter(POOL_StorageType, myCLID, pSvcLocator),
		::AthMessaging((pSvcLocator != nullptr ? msgSvc() : nullptr),
                               name ? name : "AthenaPoolConverter"),
	m_athenaPoolCnvSvc("AthenaPoolCnvSvc", "AthenaPoolConverter"),
	m_placement(nullptr),
	m_placementHints(),
	m_className(),
	m_classDescs(),
	m_dataObject(nullptr),
	m_i_poolToken(nullptr) {
}
//__________________________________________________________________________
void AthenaPoolConverter::setPlacementWithType(const std::string& tname, const std::string& key, const std::string& output) {
   if (m_placement == nullptr) {
      // Create placement for this converter if needed
      m_placement = new Placement();
   }
   // Override streaming parameters from StreamTool if requested.
   std::string::size_type pos1 = output.find("[");
   std::string outputConnectionSpec = output.substr(0, pos1);
   int tech = 0;
   m_athenaPoolCnvSvc->decodeOutputSpec(outputConnectionSpec, tech).ignore();
   // Set DB and Container names
   m_placement->setFileName(outputConnectionSpec);
   std::string containerName;
   if (m_placementHints.find(tname + key) != m_placementHints.end()) { // PlacementHint already generated?
      containerName = m_placementHints[tname + key];
   } else { // Generate PlacementHint
      // Override streaming parameters from StreamTool if requested.
      IProperty* propertyServer(dynamic_cast<IProperty*>(m_athenaPoolCnvSvc.operator->()));
      StringProperty containerPrefixProp("PoolContainerPrefix", "ROOTTREEINDEX:CollectionTree");
      StringProperty containerNameHintProp("TopLevelContainerName", "");
      StringProperty branchNameHintProp("SubLevelBranchName", "<type>/<key>");
      if (propertyServer != nullptr) {
         propertyServer->getProperty(&containerPrefixProp).ignore();
         propertyServer->getProperty(&containerNameHintProp).ignore();
         propertyServer->getProperty(&branchNameHintProp).ignore();
      }
      std::string containerPrefix = containerPrefixProp.value();
      std::string dhContainerPrefix = "POOLContainer";
      // Get Technology from containerPrefix
      std::size_t colonPos = containerPrefix.find(":");
      if (colonPos != std::string::npos) {
         dhContainerPrefix = containerPrefix.substr(0, colonPos + 1) + dhContainerPrefix;
      }
      std::string containerNameHint = containerNameHintProp.value();
      std::string branchNameHint = branchNameHintProp.value();
      while (pos1 != std::string::npos) {
         const std::string::size_type pos2 = output.find("=", pos1);
         const std::string key = output.substr(pos1 + 1, pos2 - pos1 - 1);
         const std::string::size_type pos3 = output.find("]", pos2);
         const std::string value = output.substr(pos2 + 1, pos3 - pos2 - 1);
         if (key == "OutputCollection") {
            dhContainerPrefix = value;
         } else if (key == "PoolContainerPrefix") {
            containerPrefix = value;
         } else if (key == "TopLevelContainerName") {
            containerNameHint = value;
         } else if (key == "SubLevelBranchName") {
            branchNameHint = value;
         }
         pos1 = output.find("[", pos3);
      }
      if (tname.substr(0, 14) == "DataHeaderForm") {
         containerName = dhContainerPrefix + "Form" + "(" + tname + ")";
      } else if (tname.substr(0, 10) == "DataHeader") {
         if (key.substr(key.size() - 1) == "/") {
            containerName = dhContainerPrefix + "(" + key + tname + ")";
         } else {
            containerName = dhContainerPrefix + "(" + tname + ")";
         }
      } else if (tname.substr(0, 13) == "AttributeList") {
         containerName = "ROOTTREE:POOLCollectionTree(" + key + ")";
      } else if (key.empty()) {
         containerName = containerPrefix + tname;
      } else {
         const std::string typeTok = "<type>", keyTok = "<key>";
         containerName = containerPrefix + containerNameHint;
         if (!branchNameHint.empty()) {
            containerName += "(" + branchNameHint + ")";
         }
         const std::size_t pos1 = containerName.find(typeTok);
         if (pos1 != std::string::npos) {
            containerName.replace(pos1, typeTok.size(), tname);
         }
         const std::size_t pos2 = containerName.find(keyTok);
         if (pos2 != std::string::npos) {
            if (key.empty()) {
               containerName.replace(pos2, keyTok.size(), tname);
            } else {
               containerName.replace(pos2, keyTok.size(), key);
            }
         }
      }
      m_placementHints.insert(std::pair<std::string, std::string>(key, containerName));
   }
   m_athenaPoolCnvSvc->decodeOutputSpec(containerName, tech).ignore();
   m_placement->setContainerName(containerName);
   m_placement->setTechnology(tech);
}
//__________________________________________________________________________
const DataObject* AthenaPoolConverter::getDataObject() const {
   return(m_dataObject);
}
//__________________________________________________________________________
bool AthenaPoolConverter::compareClassGuid(const Guid &guid) const {
   return(m_i_poolToken ? (guid == m_i_poolToken->classID()) : false);
}
//__________________________________________________________________________
StatusCode AthenaPoolConverter::cleanUp() {
   ATH_MSG_DEBUG("AthenaPoolConverter cleanUp called for base class.");
   return(StatusCode::SUCCESS);
}
