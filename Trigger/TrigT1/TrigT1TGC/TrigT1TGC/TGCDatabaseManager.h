/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCDatabaseManager_hh
#define TGCDatabaseManager_hh

#include "TrigT1TGC/TGCArguments.h"
#include "TrigT1TGC/TGCNumbering.h"
#include "TrigT1TGC/TGCElectronicsSystem.h"
#include "TrigT1TGC/TGCConnectionPPToSB.h"

#include "MuonCondSvc/TGCTriggerData.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "AthenaBaseComps/AthMessaging.h"

#include <vector>
#include <map>
#include <string>

namespace LVL1TGCTrigger {

class TGCConnectionASDToPP;
class TGCConnectionInPP;
class TGCPatchPanel;
class TGCConnectionPPToSL;
class TGCRPhiCoincidenceMap;
class TGCEIFICoincidenceMap;
class TGCTileMuCoincidenceMap;
class TGCNSWCoincidenceMap;

class TGCDatabaseManager : public AthMessaging
{
  typedef std::vector<int> PatchPanelIDs; 
  typedef std::vector<const TGCPatchPanel*> PatchPanelPointers; 

 public:
  TGCDatabaseManager(TGCArguments*);
  TGCDatabaseManager(TGCArguments*,
                     const SG::ReadCondHandleKey<TGCTriggerData>& readCondKey,
                     const std::string& version, bool flag=false);
  virtual ~TGCDatabaseManager();

  TGCDatabaseManager(const TGCDatabaseManager& right);
  TGCDatabaseManager& operator=(const TGCDatabaseManager& right);

  TGCConnectionPPToSL* getConnectionPPToSL(TGCRegionType type) const;
  TGCConnectionASDToPP* getConnectionASDToPP(TGCRegionType region, int type, TGCForwardBackwardType forwardBackward) const;
  TGCRPhiCoincidenceMap* getRPhiCoincidenceMap(int sideId, int octantId) const;
  TGCEIFICoincidenceMap* getEIFICoincidenceMap(int sideId) const;
  TGCTileMuCoincidenceMap* getTileMuCoincidenceMap() const;
  std::shared_ptr<TGCNSWCoincidenceMap> getNSWCoincidenceMap(int sideId, int octantId, int moduleId) const;

  TGCConnectionInPP* getConnectionInPP(TGCPatchPanel* patchPanel) const;
  void addConnectionInPP(const TGCPatchPanel* patchPanel, const TGCConnectionInPP* connectionInPP);

  void deleteConnectionPPToSL();

  static const std::string& getFilename(int type); 
  static const std::vector<std::string > splitCW(const std::string& input, char delimiter); 

  TGCArguments* tgcArgs() const;

 private:
  enum {NumberOfModuleInBW=9};

  TGCRPhiCoincidenceMap* m_mapRphi[NumberOfSide][NumberOfOctant];
  TGCEIFICoincidenceMap* m_mapEIFI[NumberOfSide];
  TGCTileMuCoincidenceMap* m_mapTileMu;
  std::array<std::array<std::array<std::shared_ptr<TGCNSWCoincidenceMap>, NumberOfModuleInBW>, NumberOfOctant>, NumberOfSide> m_mapNSW;
  TGCConnectionPPToSL* m_PPToSL[NumberOfRegionType];
  TGCConnectionASDToPP* m_ASDToPP[NumberOfRegionType][NumberOfPatchPanelType][TotalNumForwardBackwardType];

  std::map<PatchPanelIDs, std::pair<const TGCConnectionInPP, PatchPanelPointers> > m_patchPanelToConnectionInPP;
  
  TGCArguments* m_tgcArgs;
};

inline
TGCArguments* TGCDatabaseManager::tgcArgs() const {
  return m_tgcArgs;
}

inline 
TGCRPhiCoincidenceMap* TGCDatabaseManager::getRPhiCoincidenceMap(int sideId, int octantId) const
{
  return m_mapRphi[sideId][octantId];
}

inline 
TGCEIFICoincidenceMap* TGCDatabaseManager::getEIFICoincidenceMap(int sideId) const
{
  return m_mapEIFI[sideId];
}

inline 
TGCTileMuCoincidenceMap* TGCDatabaseManager::getTileMuCoincidenceMap() const
{
  return m_mapTileMu;
}

inline 
std::shared_ptr<TGCNSWCoincidenceMap> TGCDatabaseManager::getNSWCoincidenceMap(int sideId, int octantId, int moduleId) const
{
  return m_mapNSW[sideId][octantId][moduleId];
}

inline 
 TGCConnectionPPToSL* TGCDatabaseManager::getConnectionPPToSL(TGCRegionType type) const
{
  return m_PPToSL[type-1];
}

inline 
 TGCConnectionASDToPP* TGCDatabaseManager::getConnectionASDToPP(TGCRegionType region, int type, TGCForwardBackwardType forwardBackward) const
{
  return m_ASDToPP[region-1][type][forwardBackward];
}

} //end of namespace bracket

#endif // TGCDatabaseManager_hh
