/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FTKCONSTANTBANK_H
#define FTKCONSTANTBANK_H

class FTKTrack;
#include <TMatrixD.h>

class FTKConstantBank {
private:
  int m_bankID; // ID of the related pattern bank

  int m_nsectors; // number of sectors in this bank

  int m_nplanes; // number of planes

  int m_npars; // numbers of parameters

  int m_ncoords; // number of coordinates used in the linear fit

  int *m_coordsmask; // mask used for the majority
  int *m_missid; // ids of the missing coordinates


  int m_nconstr; // number of constraints: m_ncoords-m_npars

  bool *m_isgood; //[m_nsectors] true if all constants are 0
  float ***m_fit_pars; //[m_nsectors][m_npars][m_ncoords] fit parameters
  float **m_fit_const; //[m_nsectors][m_npars] parameter offset in this sector
  float ***m_kernel; //[m_nsectors][m_nconstr][m_ncoords] covariance matrix
  float **m_kaverage; //[m_nsectors][m_nconstr] 

  TMatrixD *m_invfit_consts; // inverted constants for each sector

  /* The double precision for the following data is required because
     the code use TMatrixD */
  double **m_maj_a; //! 
  double ***m_maj_kk; //!
  double ***m_maj_invkk; //!
  
  // alternative constant vectors to be used when performing fits with integer precision
  // to match firmware specs -- these are not yet initialized anywhere!
  signed long long ***m_kernel_aux; //[m_nsectors][m_nconstr][m_ncoords] covariance matrix
  signed long long **m_kaverage_aux; //[m_nsectors][m_nconstr] 
  signed long long ***m_maj_invkk_aux; //!

  // function to model behavior of arithmetic shift register, used in firmware tests
  signed long long aux_asr( const signed long long &input , const int &shift , const int &width , bool &overflow ) const;

public:
  FTKConstantBank();
  FTKConstantBank(int,const char*);
  ~FTKConstantBank();

  int getBankID() const { return m_bankID; }

  int getNConstr() const { return m_nconstr; }
  int getNCoords() const { return m_ncoords; }
  int getNPars() const { return m_npars; }
  int getNPlanes() const { return m_nplanes; }
  int getNSectors() const { return m_nsectors; }

  float getFitConst( int isec , int ipar ) { return m_fit_const[isec][ipar]; }
  float getFitPar( int isec , int ipar , int icoord ) { return m_fit_pars[isec][ipar][icoord]; }
  float getKaverage( int isec , int iconstr ) { return m_kaverage[isec][iconstr]; }
  float getKernel( int isec , int iconstr , int icoord ) { return m_kernel[isec][iconstr][icoord]; }
  float *getKaverage(int sectid) const { return m_kaverage[sectid]; }
  float **getKernel(int sectid) const { return m_kernel[sectid]; }


  int  linfit(int, FTKTrack &) const;
  void linfit_chisq(int, FTKTrack&) const;
  void linfit_pars_eval(int, FTKTrack&) const;

  void prepareInvConstants();

  int invlinfit(int, FTKTrack &, double *constr=0x0) const;
  //  int missing_point_guess(float *,int *, int, float *newcoords=0);
  int missing_point_guess(FTKTrack &, int, float *newcoords=0) const;
  
  void extrapolate_coords(FTKTrack &,int,int *, int *) const;

  const bool& getIsGood(int isec) const { return m_isgood[isec]; }

  // additional fitting functions to match firmware specs
  void linfit_chisq_aux(int, FTKTrack&) const;

};
#endif // FTKCONSTANTBANK_H
