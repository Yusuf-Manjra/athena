/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************
 
 NAME:     LArAutoCorrBuilder.cxx
 PACKAGE:  offline/LArCalorimeter/LArCalibUtils
 
 AUTHORS:  R. Lafaye
 CREATED:  Jun. 12, 2008 from LArPedestalMaker
    
 PURPOSE:  Get the autocorrelation for each cell from LArAccumulatedDigits
           at each gain and records them in TDS

           In fact only the last (m_nsamples-1) elements of the 
           first line (or column) of autocorrelation matrix are
           recovered and stored in TDS, for these reasons:
           - symetrie of autocorrelation matrix
           - equivalence of autocorrelation elements: 
             B(n,n+i)\eq B(m,m+i) (eg B12 \eq B23).

  HISTORY:
          Walter Lampl, 26 Aug 2009:
          Derive from LArPedestalBuilder 
             
********************************************************************/

// Include files
#include "LArCalibUtils/LArAutoCorrBuilder.h"
//#include "LArIdentifier/LArOnlineID.h"

//#include <math.h>
//#include <unistd.h>


LArAutoCorrBuilder::LArAutoCorrBuilder(const std::string& name, ISvcLocator* pSvcLocator) 
  : LArPedestalBuilder(name, pSvcLocator)
{
  declareProperty("normalize",    m_normalize=1); 
  declareProperty("AutoCorrKey",   m_acContName="LArAutoCorr");
}


LArAutoCorrBuilder::~LArAutoCorrBuilder()
{}

//---------------------------------------------------------------------------
StatusCode LArAutoCorrBuilder::stop() {

  ATH_MSG_DEBUG(">>> stop()");

  LArAutoCorrComplete* larAutoCorrComplete = new LArAutoCorrComplete();
  // Initialize LArAutoCorrComplete 
  StatusCode sc=larAutoCorrComplete->setGroupingType(m_groupingType,msg());
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Failed to set groupingType for LArAutoCorrComplete object" << endreq;
    return sc;
  }
  sc=larAutoCorrComplete->initialize(); 
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Failed initialize LArAutoCorrComplete object" << endreq;
    return sc;
  }


  int n_zero,n_min, n_max, n_cur;
  n_zero=0; n_max=n_min=-1;
  unsigned NAutoCorr=0;
  std::vector<float> cov;
  //Loop over gains
  for (unsigned k=0;k<(int)CaloGain::LARNGAIN;k++) {
    CaloGain::CaloGain gain=(CaloGain::CaloGain)k;
    //Loop over cells
    ACCU::ConstConditionsMapIterator cell_it=m_accu.begin(gain);
    ACCU::ConstConditionsMapIterator cell_it_e=m_accu.end(gain);
    if (cell_it==cell_it_e) continue; //No data for this gain
    for (;cell_it!=cell_it_e;cell_it++) {
      const LArAccumulatedDigit& dg=*cell_it;
      n_cur = dg.nTrigger();
      if(n_cur==0) { n_zero++; continue; }

      HWIdentifier chid = cell_it.channelId();
     
      
      if(n_cur<n_min || n_min<0) n_min=n_cur;
      if(n_cur>n_max || n_max<0) n_max=n_cur;
      
      // Fill the data class with pedestal and rms values
      dg.getCov(cov,m_normalize);

      larAutoCorrComplete->set(chid,gain,cov);
      NAutoCorr++;
    }//end loop over all cells

    msg(MSG::DEBUG) << "Gain " << gain << " Number of cells with 0 events to compute autocorr: " <<  n_zero << endreq;
    msg(MSG::DEBUG) << "Gain " << gain << " Minimum number of events to compute autocorr: " <<  n_min << endreq;
    msg(MSG::DEBUG) << "Gain " << gain << " Maximum number of events to compute autocorr: " <<  n_max << endreq;
  }//end loop over gains

  //msg(MSG::INFO << " Summary : Number of cells with a autocorr value computed : " << larAutoCorrComplete->totalNumberOfConditions()  << endreq;
  msg(MSG::INFO) << " Summary : Number of cells with a autocorr value computed : " << NAutoCorr  << endreq;
  msg(MSG::INFO) << " Summary : Number of Barrel PS cells side A or C (connected+unconnected):   3904+ 192 =  4096 " << endreq;
  msg(MSG::INFO) << " Summary : Number of Barrel    cells side A or C (connected+unconnected):  50944+2304 = 53248 " << endreq;
  msg(MSG::INFO) << " Summary : Number of EMEC      cells side A or C (connected+unconnected):  31872+3456 = 35328 " << endreq;
  msg(MSG::INFO) << " Summary : Number of HEC       cells side A or C (connected+unconnected):   2816+ 256 =  3072 " << endreq;
  msg(MSG::INFO) << " Summary : Number of FCAL      cells side A or C (connected+unconnected):   1762+  30 =  1792 " << endreq;
  
  // Record LArAutoCorrComplete
  sc = detStore()->record(larAutoCorrComplete,m_acContName);
  if (sc != StatusCode::SUCCESS) { 
    msg(MSG::ERROR)  << " Cannot store LArAutoCorrComplete in TDS "<< endreq;
    delete larAutoCorrComplete;
    return sc;
  }
  else
    msg(MSG::INFO) << "Recorded LArAutCorrComplete object with key " << m_acContName << endreq;
  // Make symlink
  sc = detStore()->symLink(larAutoCorrComplete, (ILArAutoCorr*)larAutoCorrComplete);
  if (sc != StatusCode::SUCCESS)  {
    msg(MSG::ERROR)  << " Cannot make link for Data Object " << endreq;
    return sc;
  }
   
  return StatusCode::SUCCESS;
}








