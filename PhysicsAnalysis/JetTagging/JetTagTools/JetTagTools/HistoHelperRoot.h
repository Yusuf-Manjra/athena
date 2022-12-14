/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETTAGTOOLS_HISTOHELPERROOT_H
#define JETTAGTOOLS_HISTOHELPERROOT_H

/******************************************************
    @class HistoHelper
********************************************************/

#include <string>
#include <map>
#include <vector>

#include "CxxUtils/checker_macros.h"
#include "GaudiKernel/LockedHandle.h"
#include "JetTagTools/HistoLimits.h"

class ITHistSvc;
class TH1;
class TH2;
class TH3;


namespace Analysis
{

/** Helper class for histograming
 *
 * The class is made thread-safe through the use of LockedHandle for
 * histogram filling.
 */

class HistoHelperRoot
{

 public:

  /** Constructor with histoSvc only */
  HistoHelperRoot(ITHistSvc*);
  ~HistoHelperRoot();

  std::string baseName(const std::string& fullHistoName); // removes the path and return the histogram name
  void bookHisto(const std::string& histoName, const std::string& histoTitle, unsigned int bins, double minx, double maxx);
  void bookHisto(const std::string& histoName, const std::string& histoTitle, unsigned int bins, double* edge);
  void bookHisto(const std::string& histoName, const std::string& histoTitle, unsigned int binsx, double minx, double maxx, unsigned int binsy, double miny, double maxy);
  void bookHisto(const std::string& histoName, const std::string& histoTitle, unsigned int binsx, double* edgex, unsigned int binsy, double* edgey);
  void bookHisto(const std::string& histoName, const std::string& histoTitle, unsigned int binsx, double minx, double maxx, unsigned int binsy, double miny, double maxy, unsigned int binsz, double minz, double maxz);
  void fillHisto(const std::string& histoName, double ) const;
  void fillHistoWithWeight(const std::string& histoName, double ,double) const;
  void fillHisto(const std::string& histoName, double, double ) const;
  void fillHisto(const std::string& histoName, double, double, double) const;

  ///@{ Histogram accessors (not thread-safe because exposed bare pointer)
  TH1* getHisto1D ATLAS_NOT_THREAD_SAFE (const std::string& histoName);
  TH2* getHisto2D ATLAS_NOT_THREAD_SAFE (const std::string& histoName);
  TH3* getHisto3D ATLAS_NOT_THREAD_SAFE (const std::string& histoName);
  ///@}

  // Interpolating
  static double Interpol1d(double, TH1*);
  static double Interpol2d(double, double, TH2*);
  static double Interpol3d(double, double, double, TH3*);
  // Smoothing (ASH)
  static void smoothASH2D(TH2*, int m1=3, int m2=3, bool debug=false);
  static void smoothASH3D(TH3*, int m1=3, int m2=3, int m3=2, bool debug=false);

  //changing directory
  //void setHistoDir(std::string name) {m_histoDir = name;};
  void setCheckOverflows(bool b) {m_checkOverflows = b;};
  void print();

 private:

  ITHistSvc* m_histoSvc;
  std::map<std::string, LockedHandle<TH1>> m_histoMap1D;
  std::map<std::string, LockedHandle<TH2>> m_histoMap2D;
  std::map<std::string, LockedHandle<TH3>> m_histoMap3D;
  std::map<std::string, HistoLimits> m_histoLimitsMap1D;
  std::map<std::string, HistoLimits> m_histoLimitsMap2D;
  std::map<std::string, HistoLimits> m_histoLimitsMap3D;
  bool m_checkOverflows;
};

}
#endif /// JETTAGTOOLS_HISTOHELPERROOT_H

