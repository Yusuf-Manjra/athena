/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MM_DIGITIZATION_MM_ELECTRONICSRESPONSESIMULATION_H
#define MM_DIGITIZATION_MM_ELECTRONICSRESPONSESIMULATION_H
/** @class MM_ElectronicsResponseSimulation

// ------------
// Authors:
//   Iakovidis George        <george.iakovidis@cern.ch>
//   Karakostas Konstantinos <konstantinos.karakostas@cern.ch>
//   Leontsinis Stefanos     <stefanos.leontsinis@cern.ch>
//   Nektarios Chr. Benekos  <nbenekos@cern.ch>
//   Jessica Metcalfe        <jessica.metcalfe@gmail.com>
//
// Major Contributions From: Verena Martinez
//                           Tomoyuki Saito
//
// Major Restructuring for r21+ From: Lawrence Lee <lawrence.lee.jr@cern.ch>
//
//////////////////////////////////////////////////////////////////////////////

*/

#include <TF1.h>

/// STD'S
#include <algorithm>
#include <cmath>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <utility>
#include <string>
#include <sys/stat.h>
#include <memory>

/// Projects
#include "MM_Digitization/MM_ElectronicsToolInput.h"
#include "MM_Digitization/MM_ElectronicsToolTriggerOutput.h"
#include "MM_Digitization/MM_DigitToolOutput.h"

//VMM Mapping
#include "MM_Digitization/MM_StripVmmMappingTool.h"
#include "MM_Digitization/VMM_Shaper.h"

class MM_ElectronicsResponseSimulation {

public :

  MM_ElectronicsResponseSimulation() = default;
   ~MM_ElectronicsResponseSimulation() = default;

  void initialize();

  void clearValues ();
  void vmmPeakResponseFunction(const std::vector <int> & numberofStrip, const std::vector<std::vector <float>> & qStrip, const std::vector<std::vector <float>> & tStrip, const std::vector<float>& electronicsThreshold);
  void vmmThresholdResponseFunction(const std::vector <int> & numberofStrip, const std::vector<std::vector <float>> & qStrip, const std::vector<std::vector <float>> & tStrip, const std::vector<float>& electronicsThreshold);

  MM_DigitToolOutput getPeakResponseFrom(const MM_ElectronicsToolInput & digiInput);
  MM_DigitToolOutput getThresholdResponseFrom(const MM_ElectronicsToolInput & digiInput);

  MM_ElectronicsToolTriggerOutput getTheFastestSignalInVMM(const MM_DigitToolOutput & ElectronicThresholdOutput, const int chMax, const int stationEta);
  int getIdTheFastestSignalInVMM(float time, int VMM_id, std::vector<int> trigger_VMM_id, const std::vector<float>& ElectronicsThreshold_stripTime, float timeWindowLower, float timeWindowUpper);
  void getVMMId(const std::vector< int > & ElectronicsThreshold_stripPos, const int chMax, const int stationEta, std::vector< int > & trigger_VMM_id, std::vector< int > & trigger_MMFE_VMM_id);
  MM_DigitToolOutput applyDeadTimeStrip(const MM_DigitToolOutput & ElectronicsTriggerOutput);
  MM_ElectronicsToolTriggerOutput applyDeadTimeART(const MM_ElectronicsToolTriggerOutput & ElectronicsTriggerOutput);
  MM_ElectronicsToolTriggerOutput applyARTTiming(const MM_ElectronicsToolTriggerOutput & ElectronicsTriggerOutput, float jitter, float offset);
  bool deadChannel(int id, float time, std::vector<int> & v_id, const std::vector<float> & v_time, float deadtime);

  inline void setPeakTime(float val) { m_peakTime = val;  };
  inline void setTimeWindowLowerOffset(float val) { m_timeWindowLowerOffset = val;};
  inline void setTimeWindowUpperOffset(float val) { m_timeWindowUpperOffset = val;};
  inline void setVmmDeadtime(float val) {m_vmmDeadtime = val;}
  inline void setVmmUpperGrazeWindow(float val) {m_vmmUpperGrazeWindow = val;}
  inline void setStripdeadtime(float val) { m_stripDeadTime = val;};
  inline void setARTdeadtime(float val)   { m_artDeadTime = val;};

  inline void setNeighborLogic              (bool  val) { m_useNeighborLogic           = val; };


  float getPeakTime() const { return m_peakTime;};
  float getTimeWindowLowerOffset() const { return m_timeWindowLowerOffset ;};
  float getTimeWindowUpperOffset() const { return m_timeWindowUpperOffset ;};
  float getVmmDeadTime() {return m_vmmDeadtime;}
  float getVmmUpperGrazeWindow() {return m_vmmUpperGrazeWindow;}
  float getStripdeadtime() const { return m_stripDeadTime;};
  float getARTdeadtime() const { return m_artDeadTime;};


  const std::vector <float>&  getTStripElectronicsAbThr () const { return m_tStripElectronicsAbThr;};
  const std::vector <float>&  getQStripElectronics      () const { return m_qStripElectronics;};
  const std::vector <int>  &  getNStripElectronics      () const { return m_nStripElectronics;};

private:

  /** power of responce function */
  float m_peakTime{0.};
  float m_timeWindowLowerOffset{0.};
  float m_timeWindowUpperOffset{0.};
  float m_stripDeadTime{0.};
  float m_artDeadTime{0.};
  float m_vmmDeadtime{0.};
  float m_vmmUpperGrazeWindow{0.};
  bool  m_useNeighborLogic{false};


  std::vector <float> m_tStripElectronicsAbThr{};
  std::vector <float> m_qStripElectronics{};
  std::vector <int>   m_nStripElectronics{};

  std::unique_ptr<VMM_Shaper> m_vmmShaper{};

};

#endif
