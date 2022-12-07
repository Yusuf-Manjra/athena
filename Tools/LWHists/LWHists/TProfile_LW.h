/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class TProfile_LW                         //
//                                                            //
//  Description: LightWeight version of TProfile.             //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: July 2009                                //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef TPROFILE_LW_H
#define TPROFILE_LW_H

#include "LWHists/LWHist.h"
class TProfile;
class Flex1DProfileHisto;

class TProfile_LW : public LWHist {
public:

  //To allocate from pool - remember to delete with LWHist::safeDelete(..):
  static TProfile_LW * create( const char* name, const char* title,unsigned nbinsx,
			       const double& xlow, const double& xup, const char* option=" " );
  static TProfile_LW * create( const char* name, const char* title,unsigned nbinsx,
			       const double& xlow, const double& xup,
			       const double& ylow, const double& yup, const char* option=" ");
  static TProfile_LW * create( const char* name, const char* title,unsigned nbinsx,
			       const float* xbins, const char* option=" ");
  static TProfile_LW * create( const char* name, const char* title,unsigned nbinsx,
			       const double* xbins, const char* option=" ");
  static TProfile_LW * create( const char* name, const char* title,unsigned nbinsx,
			       const double* xbins,
 			       const double& ylow, const double& yup, const char* option=" ");

  void Fill(const double& x, const double& y );
  void Fill(const double& x, const double& y, const double& w );
  unsigned GetNbinsX() const;
  double GetBinEntries(unsigned bin) const;
  double GetBinContent(unsigned bin) const;
  double GetBinError(unsigned bin) const;
  void SetBinEntries(unsigned bin, const double& );
  void SetBinContent(unsigned bin, const double& );
  void SetBinError(unsigned bin, const double& );
  virtual unsigned GetEntries() const override;
  virtual void SetEntries(unsigned) override;

  virtual void Reset() override;

  double getXMin() const;
  double getXMax() const;
  void GetBinInfo(unsigned bin,double& entries, double& content, double&error) const;
  void SetBinInfo(unsigned bin, const double& entries, const double& content, const double& error);

  void SetErrorOption(const char* option);
  const char* GetErrorOption() const;

  void getSums( double& sumW,  double& sumW2,
		double& sumWX, double& sumWX2,
		double& sumWY, double&sumWY2 ) const;
  void setSums( const double& sumW,  const double&sumW2,
		const double& sumWX, const double& sumWX2,
		const double& sumWY, const double& sumWY2 );

  TProfile* getROOTHist();
  virtual TH1* getROOTHistBase() override;


  virtual double Integral() const override;
private:
  friend class LWHistInt;
  friend class LWHistVal;
  virtual void clear() override;
  virtual const TH1* getROOTHistBaseNoAlloc() const override;
  virtual       TH1* getROOTHistBaseNoAlloc() override;
  virtual void clearKeptROOTHist() override;//Does nothing if root-backend.
  const float * getVarBins() const;//null if fixed bin-widths
  float * getVarBins();//null if fixed bin-widths

  static TProfile_LW * actualcreate( const char* name, const char* title,
				     unsigned nbinsx, const double& xlow, const double& xup,
				     const double& ylow, const double& yup,
				     const float* xbins_flt, const double* xbins_dbl,
				     const char* option);
  TProfile_LW( const char* name, const char* title,
	       unsigned nbinsx, const double& xlow, const double& xup,
	       const double& ylow, const double& yup,
	       const float* xbins_flt, const double* xbins_dbl, const char* option,bool rootbackend );

  virtual ~TProfile_LW();
  TProfile_LW( const TProfile_LW & );
  TProfile_LW & operator= ( const TProfile_LW & );
  Flex1DProfileHisto * m_flexHisto;
  TProfile * m_rootHisto;
  virtual double actualGetBinCenterX(int bin) const override;
  virtual double actualGetBinCenterY(int) const override { return 0; }
  virtual unsigned actualFindBinX(const double&) const override;
  virtual unsigned actualFindBinY(const double&) const override { return 0; }
  virtual unsigned actualGetNBinsX() const override { return GetNbinsX(); }
  virtual unsigned actualGetNBinsY() const override { return 0; }

};

#endif
