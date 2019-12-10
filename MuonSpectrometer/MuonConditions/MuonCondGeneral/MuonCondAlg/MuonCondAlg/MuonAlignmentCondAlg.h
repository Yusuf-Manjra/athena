/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCONDALG_MUONALIGNMENTCONDALG_H
#define MUONCONDALG_MUONALIGNMENTCONDALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/CondHandleKeyArray.h"
#include "GaudiKernel/ICondSvc.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
// typedefs for A/BLineMapContainer
#include "MuonAlignmentData/CorrContainer.h"
#include "GaudiKernel/ServiceHandle.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"

class StoreGateSvc; 
class StatusCode;

namespace MuonGM
{
  class MuonDetectorManager;
}

class MuonAlignmentCondAlg: public AthAlgorithm {

 public:

  MuonAlignmentCondAlg (const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~MuonAlignmentCondAlg() = default;
  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;
  virtual StatusCode finalize() override;

  std::vector<std::string> parlineFolder() { return m_parlineFolder; }
    
 private:

  // Read Handles
  SG::ReadCondHandleKey<CondAttrListCollection> m_readMdtBarrelKey        {this, "ReadMdtBarrelKey", 
                                                                                 "/MUONALIGN/MDT/BARREL", 
                                                                                 "Key of input MDT/BARREL condition data"};
  SG::ReadCondHandleKey<CondAttrListCollection> m_readMdtEndcapSideAKey   {this, "ReadMdtEndcapSideAKey", 
                                                                                 "/MUONALIGN/MDT/ENDCAP/SIDEA", 
                                                                                 "Key of input MDT/ENDCAP/SIDEA condition data"};
  SG::ReadCondHandleKey<CondAttrListCollection> m_readMdtEndcapSideCKey   {this, "ReadMdtEndcapSideCKey", 
                                                                                 "/MUONALIGN/MDT/ENDCAP/SIDEC", 
                                                                                 "Key of input MDT/ENDCAP/SIDEC condition data"};
  SG::ReadCondHandleKey<CondAttrListCollection> m_readTgcSideAKey         {this, "ReadTgcSideAKey", 
                                                                                 "/MUONALIGN/TGC/SIDEA", 
                                                                                 "Key of input TGC/SIDEA condition data"};
  SG::ReadCondHandleKey<CondAttrListCollection> m_readTgcSideCKey         {this, "ReadTgcSideCKey", 
                                                                                 "/MUONALIGN/TGC/SIDEC", 
                                                                                 "Key of input TGC/SIDEC condition data"};
  SG::ReadCondHandleKey<CondAttrListCollection> m_readCscILinesKey        {this, "ReadCscILinesKey", 
                                                                                 "/MUONALIGN/CSC/ILINES", 
                                                                                 "Key of input CSC/ILINES condition data"};
  SG::ReadCondHandleKey<CondAttrListCollection> m_readMdtAsBuiltParamsKey {this, "ReadMdtAsBuiltParamsKey", 
                                                                                 "/MUONALIGN/MDT/ASBUILTPARAMS", 
                                                                                 "Key of MDT/ASBUILTPARAMS input condition data"};
  // Write Handles
  SG::WriteCondHandleKey<ALineMapContainer> m_writeALineKey                {this, "WriteALineKey", 
                                                                                  "ALineMapContainer", 
                                                                                  "Key of output muon alignment ALine condition data"};    
  SG::WriteCondHandleKey<BLineMapContainer> m_writeBLineKey                {this, "WriteBLineKey", 
                                                                                  "BLineMapContainer", 
                                                                                  "Key of output muon alignment BLine condition data"};    
  SG::WriteCondHandleKey<CscInternalAlignmentMapContainer> m_writeILineKey {this, "WriteILineKey", 
                                                                                  "CscInternalAlignmentMapContainer", 
                                                                                  "Key of output muon alignment CSC/ILine condition data"};
  SG::WriteCondHandleKey<MdtAsBuiltMapContainer> m_writeAsBuiltKey         {this, "WriteAsBuiltKey", 
                                                                                  "MdtAsBuiltMapContainer", 
                                                                                  "Key of output muon alignment MDT/AsBuilt condition data"};

  ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
  const MuonGM::MuonDetectorManager* m_muonMgr;
  ServiceHandle<ICondSvc> m_condSvc;
  std::string m_geometryVersion;

  std::vector<std::string>       m_parlineFolder;
  bool m_dumpALines;
  bool m_dumpBLines;
  bool m_dumpILines;
  bool m_ILinesFromDb;
  
  std::string m_aLinesFile;
  std::string m_asBuiltFile;

  StatusCode InitializeGeometryAndIdHelpers();
  StatusCode loadParameters();
  StatusCode loadAlignABLines();
  StatusCode loadAlignABLines(std::string folderName,
			      ALineMapContainer* writeALineCdo,
			      BLineMapContainer* writeBLineCdo,
			      EventIDRange& rangeALineW,
			      EventIDRange& rangeBLineW);
  StatusCode loadAlignILines(std::string folderName);
  StatusCode loadAlignAsBuilt(std::string folderName);
 
  void setALinesFromAscii(ALineMapContainer* writeALineCdo) const;
  void setAsBuiltFromAscii(MdtAsBuiltMapContainer* writeCdo) const;
  void dumpALines(const std::string& folderName, ALineMapContainer* writeALineCdo);
  void dumpBLines(const std::string& folderName, BLineMapContainer* writeBLineCdo);
  void dumpILines(const std::string& folderName, CscInternalAlignmentMapContainer* writeCdo);

};

#endif
