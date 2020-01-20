/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file SiLorentzAngleTool/PixelSiLorentzAngleCondAlg.h
 * @author Soshi Tsuno <Soshi.Tsuno@cern.ch>
 * @date November, 2019
 * @brief Return Lorentz angle information for pixel.
 */

#ifndef PIXELSILORENTZANGLECONDALG
#define PIXELSILORENTZANGLECONDALG

#include "AthenaBaseComps/AthAlgorithm.h"

#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "PixelConditionsData/PixelDCSHVData.h"
#include "PixelConditionsData/PixelDCSTempData.h"
#include "GaudiKernel/ICondSvc.h"
#include "SiPropertiesTool/ISiPropertiesTool.h"

#include "SiLorentzAngleTool/SiLorentzAngleCondData.h"
#include "InDetIdentifier/PixelID.h"

// forward declarations
namespace MagField {
  class IMagFieldSvc;
}

class PixelSiLorentzAngleCondAlg: public AthAlgorithm {
  public:
    PixelSiLorentzAngleCondAlg(const std::string& name, ISvcLocator* pSvcLocator);
    virtual ~PixelSiLorentzAngleCondAlg() = default;
    virtual StatusCode initialize() override;
    virtual StatusCode execute() override;
    virtual StatusCode finalize() override;

  private:
    const PixelID* m_pixid;

    ServiceHandle<ICondSvc> m_condSvc;
    ServiceHandle<MagField::IMagFieldSvc> m_magFieldSvc;

    SG::ReadCondHandleKey<PixelDCSTempData> m_readKeyTemp
    {this, "ReadKeyeTemp", "PixelDCSTempCondData", "Key of input sensor temperature conditions folder"};

    SG::ReadCondHandleKey<PixelDCSHVData> m_readKeyHV
    {this, "ReadKeyHV", "PixelDCSHVCondData", "Key of input bias voltage conditions folder"};

    SG::ReadCondHandleKey<CondAttrListCollection> m_readKeyBFieldSensor
    {this, "ReadKeyBFieldSensor", "/EXT/DCS/MAGNETS/SENSORDATA", "Key of input B-field sensor"};

    SG::ReadCondHandleKey<InDetDD::SiDetectorElementCollection> m_pixelDetEleCollKey
    {this, "PixelDetEleCollKey", "PixelDetectorElementCollection", "Key of SiDetectorElementCollection for Pixel"};

    SG::WriteCondHandleKey<SiLorentzAngleCondData> m_writeKey
    {this, "WriteKey", "PixelSiLorentzAngleCondData", "Key of output SiLorentzAngleCondData"};

    ToolHandle<ISiPropertiesTool> m_siPropertiesTool
    {this, "SiPropertiesTool", "SiPropertiesTool", "Tool to retrieve SiProperties"};

    // Properties
    double                   m_nominalField;
    bool                     m_useMagFieldSvc;
    bool                     m_useMagFieldDcs;
    double                   m_correctionFactor;

    Amg::Vector3D getMagneticField(const InDetDD::SiDetectorElement* element) const;
};

#endif
