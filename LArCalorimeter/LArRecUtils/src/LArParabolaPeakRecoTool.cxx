/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRecUtils/LArParabolaPeakRecoTool.h"

#include "PathResolver/PathResolver.h"

#include "GaudiKernel/MsgStream.h"
#include "CLHEP/Matrix/Matrix.h"
#include "CLHEP/Matrix/Vector.h"
#include <algorithm>
#include <stdio.h>

using CLHEP::HepMatrix;
using CLHEP::HepVector;


LArParabolaPeakRecoTool::LArParabolaPeakRecoTool(const std::string& type, const std::string& name, 
						 const IInterface* parent) 
  : AthAlgTool(type, name, parent), 
    m_correctBias(true), 
    m_fileShape("parabola_Shape.dat"), 
    m_fileADCcor("parabola_adccor.dat")

{
  declareProperty("correctBias",m_correctBias);
  declareProperty("fileShape",m_fileShape);
  declareProperty("fileADCcor",m_fileADCcor);
  declareInterface<LArParabolaPeakRecoTool>(this);
}

StatusCode LArParabolaPeakRecoTool::initialize()
{
  
  if(m_correctBias){

    std::cout << "LArParabolaPeakRecoTool: correctBias flag is ON " << std::endl;
    // if want to correct bias, open files

    m_fileShape = PathResolver::find_file (m_fileShape, "DATAPATH");
    m_fileADCcor = PathResolver::find_file (m_fileADCcor, "DATAPATH");

    fileShape = fopen(m_fileShape.c_str(),"r");
    fileADCcor = fopen(m_fileADCcor.c_str(),"r");
    int idelay, ilayer;
    float timeValue, adccorValue;
    
    // fill correction table
    while( fscanf(fileShape,"%d %d %f",&idelay,&ilayer,&timeValue) != EOF )
      {
	if ( ilayer >= 0 && ilayer < 4 && idelay >= 0 && idelay < 25 )
	  {
	    QT_Shape[ilayer][idelay] = timeValue;
	    
	    if ( idelay == 0 && QT_Shape[ilayer][0] != 0. )
	      {
		QT_Shape[ilayer][25]  = QT_Shape[ilayer][0] + 25;
	      }
	  }
      }
    
    while( fscanf(fileADCcor,"%d %d %f",&idelay,&ilayer,&adccorValue) != EOF )
      {
	if ( ilayer >= 0 && ilayer < 4 && idelay >= 0 && idelay < 25 )
	  {
	    QT_ADCcor[ilayer][idelay] = adccorValue;
	  }
      }
    fclose(fileADCcor);
    fclose(fileShape);
  }

  return StatusCode::SUCCESS;
}

StatusCode LArParabolaPeakRecoTool::finalize()
{return StatusCode::SUCCESS;}

std::vector<float> LArParabolaPeakRecoTool::peak (const std::vector<float>& samples) const 
{
  return peak(samples, -1, -1.);
}

std::vector<float> LArParabolaPeakRecoTool::peak (const std::vector<short>& samples) const 
{
  std::vector<float> newsamples;
  for(unsigned int i=0;i<samples.size();i++)
    {
      newsamples.push_back((float)samples[i]);
    }
  return peak(newsamples, -1, -1.);
}


std::vector<float> LArParabolaPeakRecoTool::peak (const std::vector<short>& samples, int layer, float pedestal) const 
{

  std::vector<float> newsamples;
  for(unsigned int i=0;i<samples.size();i++)
    {
      newsamples.push_back((float)samples[i]);
    }
  return peak(newsamples, layer, pedestal);

}

std::vector<float> LArParabolaPeakRecoTool::peak (const std::vector<float>& samples, int layer, float pedestal) const 
{//const float dt=25.0;//ns
  MsgStream log(msgSvc(), name());
  static std::vector<float> solution; 
  solution.clear();

  const std::vector<float>::const_iterator it_max=max_element(samples.begin(),samples.end());
  if (it_max==samples.end())
    {log << MSG::ERROR << "Maximum ADC sample not found!" << endreq;
    return solution; 
    }
  if (it_max==samples.begin() || it_max==samples.end()-1)
    {
      solution.push_back(*it_max);
      return solution;
    }

  // Fit a+bx+cx^2
  HepMatrix alpha(3,3);
  HepVector beta(3);
  for (int i=0;i<3;i++)
    {beta[i]=*(it_max-1+i);
    //std::cout << " " << beta[i];
    for (int j=0;j<3;j++)
      alpha[i][j]=pow(-1+i,j);
    }
  
  float retval, tmax, traw, trec;
  if(!m_correctBias){

    HepVector comp=solve(alpha,beta);
    //solve b+2cx=0 to get maximum
    tmax=-comp[1]/(2*comp[2]);
    //substitute-back
    retval=comp[0]-comp[1]*comp[1]/(4*comp[2]);

  }else{
    
    // obtain time as in EMTB
    traw = 25./2.*(beta[0]-beta[2])/(beta[0] - beta[1] + beta[2] - beta[1]);
    
    // normalize back to the reference scheme    
    traw += ( it_max-samples.begin() - 3 )*25.;
    
    if(layer < 0 || layer > 3) {
      log << MSG::ERROR << "Layer index is wrong ! Layer = " << layer << endreq;
      return solution;
    }else{
      // get true time 
      trec = ParabolaRawToTrueTime(traw, layer);
      // get true ADC
      if(pedestal<0) log << MSG::ERROR << "Pedestal is wrong ! Ped = " << pedestal << endreq;
      retval =  ParabolaRawToTrueADC(trec, beta[1], pedestal, layer);
    }
  }
  
  //std::cout << "Parabola: Time=" << tmax << " adcmax=" << maxadc << " retval= " << retval << std::endl;
  
  solution.push_back(retval);
  if(!m_correctBias){
    solution.push_back(tmax);
  }else{
    // if corrected bias, return both uncorrected and corrected times
    solution.push_back(traw);
    solution.push_back(trec);
  }
  return solution;
}

// function taken from EMTB to obtaine the true time from the biased time
float LArParabolaPeakRecoTool::ParabolaRawToTrueTime(float& QT_fittime, int& layer) const
{
  int iOffset;
  float QT_true_lower;
  float QT_raw_lower;
  float QT_raw_upper;
  float QT_true_time;
  int idelay;
  int ilayer;
  
  /*
   * initialize
   */
  
  ilayer = layer;
  
  QT_true_lower = -999.;
  QT_raw_lower  = -999.;
  QT_raw_upper  = -999.;
  iOffset = 0;
  
  /*
   * the transfer function is defined from 0 to 25 ns
   * the function has a periodicity of 25 ns, adapted
   * the input value to the nominal regime and correct
   * afterwards
   */
  while ( QT_fittime < QT_Shape[ilayer][0] )
    {
      QT_fittime +=25;
      iOffset++;
    }
  
  while ( QT_fittime > QT_Shape[ilayer][25] )
    {
      QT_fittime -= 25;
      iOffset--;
    }
  
  /*
   * now find the correct bin to do the correction
   */
  
  for ( idelay=0; idelay < 25; idelay++ )
    {
      if ( QT_fittime >= QT_Shape[ilayer][idelay] 
	   && QT_fittime < QT_Shape[ilayer][idelay+1])
	{
	  QT_true_lower = (float) (idelay - iOffset * 25.);
	  QT_raw_lower  = QT_Shape[ilayer][idelay];
	  QT_raw_upper  = QT_Shape[ilayer][idelay+1];
	}
    }
  
  /*
   * from lower and upper bounds calculate the true time (linear extrapolation)
   */
  
  if ( QT_true_lower == -999. )
    {
      QT_true_time = -999.;
    }
  else 
    {
      QT_true_time = QT_true_lower + 
	(QT_fittime-QT_raw_lower)/(QT_raw_upper - QT_raw_lower);
    }
  /* 
   * return the true time
   */
  
  return QT_true_time;
  
}

// function taken from EMTB to obtaine the true ADC from the biased ADC
float LArParabolaPeakRecoTool::ParabolaRawToTrueADC(float& QT_true_time, double& ADCref, float& PEDref, int& layer) const
{
  int ichoosedelay;
  float QT_true_ADC;
  float QT_correct_ADC;
  int ilayer;
  
  /*
   * save in local variable because of extensive use
   */
  
  ilayer       = layer;
  
  /*
   * now transform the time into a positive value to
   * make the calculation easier
   */
  
  if ( QT_true_time < 0. )
    {
      QT_true_time += ( ( (int) fabs(QT_true_time / 25)) + 1)*25;
    }
  
  /*
   * determine the bin (==delay) of the reference signal
   */
  
  ichoosedelay = (int) QT_true_time % 25;
  
  /*
   * make sure that we are in-bounds for the correction matrix
   */
  
  if ( ilayer >=0 && ilayer< 4 ) 
    {
      if ( ichoosedelay < 25 - 1 ){
	QT_correct_ADC= QT_ADCcor[ilayer][ichoosedelay] 
	  + (QT_ADCcor[ilayer][ichoosedelay+1]-QT_ADCcor[ilayer][ichoosedelay]) 
	  * (QT_true_time - (int) QT_true_time);
      }
      else {
	QT_correct_ADC= QT_ADCcor[ilayer][ichoosedelay] 
	  + (QT_ADCcor[ilayer][0]-QT_ADCcor[ilayer][ichoosedelay]) 
	  * (QT_true_time - (int) QT_true_time);
      }
    }
  else
    {
      QT_correct_ADC = 100.;
    }
  
  /*
   * for consistency reasons:
   * - subtract pedestal
   * - correct to ADCmax
   * - add pedestal (subtraction in EMTB later on)
   */
  
  QT_true_ADC  = ADCref - PEDref;
  QT_true_ADC = QT_true_ADC / QT_correct_ADC * 100.;
  QT_true_ADC += PEDref;
  
  /*
   * the corrected time is returned
   */
  
  return QT_true_ADC;
}
