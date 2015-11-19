/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// HIJetSubtractorToolBase.h

#ifndef HIJETREC_HIJETSUBTRACTORTOOLBASE_H
#define HIJETREC_HIJETSUBTRACTORTOOLBASE_H

////////////////////////////////////////////////////////////////////////////////
///
/// \class HIJetSubtractorToolBase
/// \author Aaron Angerami <angerami@cern.ch>
/// \date Jan, 2015
///
/// \brief Abstract base for JetModifiers applying constituent based subtraction.
///
////////////////////////////////////////////////////////////////////////////////

#include "HIJetRec/IHISubtractorTool.h"
#include "AsgTools/AsgTool.h"

class HIJetSubtractorToolBase : public asg::AsgTool, virtual public IHISubtractorTool
{
  ASG_TOOL_CLASS(HIJetSubtractorToolBase, IHISubtractorTool)
public:

  HIJetSubtractorToolBase(const std::string& myname);
  virtual ~HIJetSubtractorToolBase(){};
  inline float MinEnergyForMoments() const {return m_E_min_moment;};
  inline float MinEnergySigForMoments() const {return m_E_sig_moment;};
private:

  /// %% FIX needs some cleanup
  float m_E_min_moment;
  float m_E_sig_moment;
  bool m_update_clusters;


protected:
  inline void SetMinEnergyForMoment(float min_E) {m_E_min_moment=min_E;};
  inline void SetUpdateClusters(bool up) {m_update_clusters=up;};


  inline bool UpdateClusters() const {return m_update_clusters;};

  bool inTowerBoundary(float eta0, float phi0, float eta, float phi) const;
  void setSubtractedEtaPhi(float E, float& eta, float& phi, float eta0, float phi0, float sig) const;
  

};

#endif
