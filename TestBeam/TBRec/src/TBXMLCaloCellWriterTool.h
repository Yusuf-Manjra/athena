/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TBREC_TBXMLCALOCELLWRITERTOOL_H
#define TBREC_TBXMLCALOCELLWRITERTOOL_H
///////////////////////////////////////////////////////////////////////////////
// \brief writes CaloCell for TB event display
///////////////////////////////////////////////////////////////////////////////

#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloGeoHelpers/CaloSampling.h"

#include "CaloDetDescr/CaloDetDescrManager.h"
#include "StoreGate/ReadCondHandleKey.h"

#include "TBXMLWriterToolBase.h"

#include <iostream>
#include <vector>
#include <string>
#include <unordered_set>

class TBXMLWriter;

class TBXMLCaloCellWriterTool : public TBXMLWriterToolBase
{

 public:

  /////////////////////////////////
  // Constructors and Destructor //
  /////////////////////////////////

  /// \brief tool constructor
  TBXMLCaloCellWriterTool(const std::string& type,
			  const std::string& name,
			  const IInterface* parent);

  ~TBXMLCaloCellWriterTool();

  virtual StatusCode initialize() override;

  ////////////
  // Action //
  ////////////

 protected:

  virtual StatusCode writeRunFiles(const std::string& fileDir,
				   unsigned int runNumber) override;
  virtual StatusCode writeEvent(std::ostream& outFile, 
				const std::string& /* entryTag */ ) override;

  virtual StatusCode convertProperties();

 private:

  static const unsigned int m_nCols;

  std::vector<std::string>              m_includedCalos;
  std::vector<std::string>              m_includedSamplings;

  double                                m_etaMin, m_etaMax, m_phiMin, m_phiMax;

  bool                                  m_firstEvent;

  std::vector<CaloCell_ID::SUBCALO>     m_caloIndices;
  std::vector<CaloSampling::CaloSample> m_caloSamplings;
  std::string                           m_cellContainer;

  const CaloCell_ID*                    m_idHelper; 

  const TBXMLWriter*                    m_mother;

  std::unordered_set<EventIDBase::number_type> m_runNumbers;

  SG::ReadCondHandleKey<CaloDetDescrManager> m_caloMgrKey { this
      , "CaloDetDescrManager"
      , "CaloDetDescrManager"
      , "SG Key for CaloDetDescrManager in the Condition Store" };

};
#endif
