/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MMT_FITTER_H
#define MMT_FITTER_H

#include "AthenaBaseComps/AthMessaging.h"

#include "MMT_struct.h"


class MMT_Fitter : AthMessaging {
 public:
  MMT_Fitter(MMT_Parameters *par, int nlg=256, double LG_min=0., double LG_max=0.5);
  void Get_Fit(std::vector<Hit>& track, std::map<int,evFit_entry>& Event_Fit, std::map<hitData_key,hitData_entry>& Hits_Data_Set_Time);
  evFit_entry fit_event(int event, std::vector<Hit>& track, std::vector<hitData_entry>& hitDatas, int& nfit, std::vector<std::pair<double,double> >&mxmy, double& mxl, double& mv, double& mu) const;
  int get_last() const {return m_last;}
  int SC_ROI_n_x() const {return m_par->n_x;}
  int SC_ROI_n_y() const {return m_par->n_y;}
  int find_hitData(const std::vector<hitData_entry>& hitDatas, const hitData_key& key) const;

 protected:
  int m_last;
  //some variables
  MMT_Parameters *m_par;

  //functions translated
  int Filter_UV(std::vector<Hit>& track) const;
  float32fixed<2> Get_Global_Slope(const std::vector<Hit>& track, const std::string& type) const;
  float32fixed<2> Get_Local_Slope(const std::vector<Hit>& Track,double theta=-999.,double phi=-999.) const;
  ROI Get_ROI(float32fixed<2> M_x,float32fixed<2> M_u,float32fixed<2> M_v,const std::vector<Hit>&track) const;
  double phi_correct_factor(const std::vector<Hit>&track)const;
  float32fixed<2> Get_Delta_Theta(float32fixed<2> M_local,float32fixed<2> M_global) const;
  float32fixed<2> Get_Delta_Theta_division(float32fixed<2> M_local,float32fixed<2> M_global, float32fixed<4> a=1.) const;
  int Rough_ROI_temp(float32fixed<4> theta, float32fixed<4> phi) const;

  //sim hit code stuff
  int track_to_index(const std::vector<Hit>&track)const;

  //ideal local slope for debugging purposes
  double ideal_local_slope(const std::vector<Hit>& Track)const;
  double ideal_z(const Hit& hit)const;
  double ideal_ak(const std::vector<Hit>& Track)const;
  double ideal_zbar(const std::vector<Hit>& Track)const;

  //translated from Table_Generators.m
  float32fixed<2> LG_lgr(int ilgr, double a, int number_LG_regions, float32fixed<2> min, float32fixed<2> max) const;
  float32fixed<2> mult_factor_lgr(int ilgr, double a, int number_LG_regions, float32fixed<2> min, float32fixed<2> max) const;
  float32fixed<4> Slope_Components_ROI_val(int jy, int ix, int thetaphi) const;
  float32fixed<4> Slope_Components_ROI_theta(int jy, int ix) const;
  float32fixed<4> Slope_Components_ROI_phi(int jy, int ix) const;
  float32fixed<2> DT_Factors_val(int i, int j) const;


 private:
  //some functions
  std::vector<Hit> q_hits(const std::string& type,const std::vector<Hit>& hits) const;

  //Fitter components
  int m_number_LG_regions,m_n_fit;
  float32fixed<2> m_LG_min,m_LG_max;
  std::vector<int> q_planes(char type) const;//return position of what planes are where

};
#endif
