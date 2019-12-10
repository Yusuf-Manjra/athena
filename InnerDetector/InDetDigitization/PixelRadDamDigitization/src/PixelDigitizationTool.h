/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PixelDigitizationTool.h
//   Header file for class PixelDigitizationTool
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef PIXELDIGITIZATION_PIXELDIGITIZATIONTOOL_H
#define PIXELDIGITIZATION_PIXELDIGITIZATIONTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "PileUpTools/PileUpToolBase.h"
#include "AthenaKernel/IAthRNGSvc.h"
#include "HitManagement/TimedHitCollection.h"
#include "InDetSimEvent/SiHitCollection.h"

#include "InDetRawData/InDetRawDataCLASS_DEF.h"
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteHandle.h"
#include "PileUpTools/PileUpMergeSvc.h"

#include "InDetSimData/InDetSimDataCollection.h"
#include "PixelProcessorTool.h"
#include "SensorSimTool.h"
#include "FrontEndSimTool.h"
#include "EnergyDepositionTool.h"

namespace RadDam{

class PixelDigitizationTool : public PileUpToolBase {

  public:
    PixelDigitizationTool(const std::string &type, const std::string &name, const IInterface *pIID);

    virtual StatusCode initialize() override;
    virtual StatusCode processAllSubEvents() override;
    virtual StatusCode finalize() override;

    virtual StatusCode prepareEvent(unsigned int) override;
    StatusCode digitizeEvent();
    virtual StatusCode mergeEvent() override;
    virtual StatusCode processBunchXing(int bunchXing, SubEventIterator bSubEvents, SubEventIterator eSubEvents) override final;

  protected:
    void addSDO(SiChargedDiodeCollection &collection);

  private:

    PixelDigitizationTool();
    PixelDigitizationTool(const PixelDigitizationTool&);
    PixelDigitizationTool &operator=(const PixelDigitizationTool&);

    std::vector<SiHitCollection*> m_hitCollPtrs;

    SG::ReadCondHandleKey<InDetDD::SiDetectorElementCollection> m_pixelDetEleCollKey{this, "PixelDetEleCollKey", "PixelDetectorElementCollection", "Key of SiDetectorElementCollection for Pixel"};
    SG::WriteHandle<PixelRDO_Container>     m_rdoContainer;
    SG::WriteHandle<InDetSimDataCollection> m_simDataColl;

    int               m_HardScatterSplittingMode;
    bool              m_HardScatterSplittingSkipper;
    std::string       m_rndmEngineName;

    bool              m_onlyHitElements;

    ToolHandleArray<PixelProcessorTool>            m_processorTool;
    ToolHandleArray<SensorSimTool>                 m_chargeTool;
    ToolHandleArray<FrontEndSimTool>               m_fesimTool;
    ToolHandle<EnergyDepositionTool>                     m_energyDepositionTool;

    const PixelID            *m_detID;

    TimedHitCollection<SiHit> *m_timedHits;

  protected:

    ServiceHandle<IAthRNGSvc> m_rndmSvc{this, "RndmSvc", "AthRNGSvc", ""};  //!< Random number service
    ServiceHandle <PileUpMergeSvc> m_mergeSvc;

    CLHEP::HepRandomEngine *m_rndmEngine;

    std::string   m_inputObjectName;
    bool          m_createNoiseSDO;

};

}
#endif // PIXELDIGITIZATION_PIXELDIGITIZATIONTOOL_H
