/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELMONMODULES_H_
#define PIXELMONMODULES_H_
#include <string.h>
#include "AthenaMonitoring/ManagedMonitorToolBase.h"

class TH2F;
class TH1F;
class TProfile;
class Identifier;
class PixelID;
class StatusCode;

// A helper class to remove a lot of the code duplication.
// This is a collection of 1744 1D histograms which make up one set of module histograms.
// It will book the histograms in the constructor. The fill method will take the identifier
// as the input and fill the correct histogram and bin.

class PixelMonModules
{
   public:
      //PixelMonModules();
      virtual ~PixelMonModules()=0;
      virtual void formatHist(std::string, bool doIBL)=0;//pass the bin lables here if needed
      virtual void Reset()=0;
      virtual StatusCode regHist(ManagedMonitorToolBase* thisptr, std::string path, ManagedMonitorToolBase::Interval_t Run, bool doIBL)=0;
      //virtual StatusCode regHist(ManagedMonitorToolBase* thisptr, std::string path, ManagedMonitorToolBase::Interval_t Run, ManagedMonitorToolBase::MgmtAttr_t m=ATTRIB_MANAGED, bool doIBL)=0;
protected:
      int nBins;
      StatusCode sc;
      std::string getHistName(int i, bool forPath=false, bool doIBL=false);
      
};

class PixelMonModules1D : public PixelMonModules
{
 public:
      PixelMonModules1D(std::string name, std::string title, int nbins, double low, double high, bool doIBL);
      PixelMonModules1D(std::string name, std::string title, int nbins, double *arr, bool doIBL);
virtual ~PixelMonModules1D();
 StatusCode regHist(ManagedMonitorToolBase* thisptr, std::string path, ManagedMonitorToolBase::Interval_t Run, bool doIBL);
//StatusCode regHist(ManagedMonitorToolBase* thisptr, std::string path, ManagedMonitorToolBase::Interval_t Run, ManagedMonitorToolBase::MgmtAttr_t m=ATTRIB_MANAGED,bool doIBL);
      TH1F* IBL[20][14];
      TH1F* B0[13][22];
      TH1F* B1[13][38];
      TH1F* B2[13][52];
      TH1F* A[3][48];
      TH1F* C[3][48];
      virtual void formatHist(std::string opt="", bool doIBL=false);  //pass the bin lables here if needed
      void Fill(double value, Identifier &id, const PixelID* pixID,bool doIBL);
      void SetBinLabel(const char* lable, int binN);
      virtual void Reset();
      double GetBinContent(double value, Identifier &id, const PixelID* pixID);
   private:
      TH1F* Dummy;           //shouldn't be used unless messed up;
      TH1F* &getHist(int i); //make looping over all the histos easier.
};

class PixelMonModulesProf : public PixelMonModules
{
 public:
      PixelMonModulesProf(std::string name, std::string title, int nbins, double low, double high, bool doIBL);
      PixelMonModulesProf(std::string name, std::string title, int nbins, double *arr, bool doIBL);
virtual ~PixelMonModulesProf();
 StatusCode regHist(ManagedMonitorToolBase* thisptr, std::string path, ManagedMonitorToolBase::Interval_t Run, bool doIBL);
//StatusCode regHist(ManagedMonitorToolBase* thisptr, std::string path, ManagedMonitorToolBase::Interval_t Run, ManagedMonitorToolBase::MgmtAttr_t m=ATTRIB_MANAGED,bool doIBL);
      TProfile* IBL[20][14];
      TProfile* B0[13][22];
      TProfile* B1[13][38];
      TProfile* B2[13][52];
      TProfile* A[3][48];
      TProfile* C[3][48];
      virtual void formatHist(std::string opt="", bool doIBL=false);  //pass the bin lables here if needed
      void Fill(double value0, double value1, Identifier &id, const PixelID* pixID,bool doIBL);
      void SetBinLabel(const char* lable, int binN);
      virtual void Reset();
      double GetBinContent(double value, Identifier &id, const PixelID* pixID);
   private:
      TProfile* Dummy;           //shouldn't be used unless messed up;
      TProfile* &getHist(int i); //make looping over all the histos easier.
};

class PixelMonModules2D : public PixelMonModules
{
   public:
      PixelMonModules2D(std::string name, std::string title, int nbins0, double low0, double high0,int nbins1, double low1, double high1, bool doIBL);
      virtual ~PixelMonModules2D();
      StatusCode regHist(ManagedMonitorToolBase* thisptr, std::string path, ManagedMonitorToolBase::Interval_t Run, bool doIBL);
      //StatusCode regHist(ManagedMonitorToolBase* thisptr, std::string path, ManagedMonitorToolBase::Interval_t Run, ManagedMonitorToolBase::MgmtAttr_t m=ATTRIB_MANAGED, bool doIBL);
      TH2F* IBL[20][14];
      TH2F* B0[13][22];
      TH2F* B1[13][38];
      TH2F* B2[13][52];
      TH2F* A[3][48];
      TH2F* C[3][48];
      virtual void formatHist(std::string opt="", bool doIBL=false);  //pass the bin lables here if needed
      void Fill(double value0, double value1, Identifier &id, const PixelID* pixID,double weight,bool doIBL);
      void Fill(double value0, double value1, Identifier &id, const PixelID* pixID,bool doIBL);
      virtual void Reset();
   private:
      TH2F* Dummy;           //shouldn't be used unless messed up;
      TH2F* &getHist(int i); //make looping over all the histos easier.
};

#endif
