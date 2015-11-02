/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////////////
//  TRT_ElectronPidToolRun2_HTcalculation.cxx, (c) ATLAS Detector software   //
///////////////////////////////////////////////////////////////////////////////

/*****************************************************************************\
  This file contains the implementation of the class HTcalculator.

  It is intended to contain all the code that is used for the calculation of
  the likelihood of a TRT hit being from an electron based on the time over
  threshold.

  Authors:  Troels C. Petersen  (petersen@nbi.dk), 
            Jared G. Vasquez  (jared.vasquez@yale.edu),
            Nikita Belyaev  (nikita.belyaev@cern.ch) 

\*****************************************************************************/

#ifdef INDETTRT_ELECTRONPIDTOOLRUN2_H
#ifndef TRT_ELECTRONPIDRUN2_HTCALCULATION_CXX
#define TRT_ELECTRONPIDRUN2_HTCALCULATION_CXX


InDet::TRT_ElectronPidToolRun2::StorePIDinfo::StorePIDinfo(){
	m_nbins = 0;
	m_min 	= -9999.9;
	m_max	= 10000000*2;
	m_values.clear();
}

InDet::TRT_ElectronPidToolRun2::StorePIDinfo::StorePIDinfo(int nbins, float min, float max, std::vector<float> values){
	update (nbins, min, max, values);
}

InDet::TRT_ElectronPidToolRun2::StorePIDinfo::~StorePIDinfo(){}

void InDet::TRT_ElectronPidToolRun2::StorePIDinfo::update( int nbins, float min, float max, std::vector<float> values){
	m_nbins = nbins	;
	m_min 	= min	;
	m_max	= max	;
	if (values.size()!=m_nbins){
    		printf("  ERROR: DIFFERENT Values of n_bins and vector size!!!\n");
	}
	m_values.clear();
	for (unsigned int i = 0; i<values.size(); i++ ){
		m_values.push_back( values.at(i));
	}
}

// THIS HAS TO BE CALLED in order!!!
void InDet::TRT_ElectronPidToolRun2::StorePIDinfo::push_back( float value ){
	// Just to read the DB
      if 	(m_nbins==0) 		{
			m_nbins = int(value)	;
			m_min   = -9999.9	;
			m_max   = 10000000*2	;
			m_values.clear()	;
      }
      else if 	(m_min < -9999) 	m_min 	= value		;
      else if 	(m_max >  10000000) 	m_max   = value		;
      else	 m_values.push_back(value);
}

StatusCode InDet::TRT_ElectronPidToolRun2::StorePIDinfo::check( int gas, int detpart){
	if 	( m_nbins == 0)
	{
		std::cout << "ERROR: PIDDB no bins in the DB!! Gas: " << gas << " detPart: " << detpart << std::endl;
          	return StatusCode::FAILURE;
	}
	else if ( m_nbins != m_values.size() )
	{
		std::cout << "ERROR: PIDDB different number of numbers!!!!! " << gas << " detPart: " << detpart << std::endl;
          	return StatusCode::FAILURE;
	}
	else if ( (m_max < m_min) || (m_max == m_min) )
	{
	 	std::cout << "ERROR: PIDDB Max is smaller or equal than min!!!" << gas << " detPart: " << detpart << std::endl;
          	return StatusCode::FAILURE;
	}
        return StatusCode::SUCCESS;
}


float InDet::TRT_ElectronPidToolRun2::StorePIDinfo::GetValue 	( float input  ){
	return m_values.at(	GetBin(	input	)	);
}

float InDet::TRT_ElectronPidToolRun2::StorePIDinfo::GetBinValue 	( int bin){
	return m_values.at(	bin	);
}
int InDet::TRT_ElectronPidToolRun2::StorePIDinfo::GetBin	( float input  ){
	if (input < m_min) 		return 0;
        else if (input > m_max) 	return m_nbins-1;
	else{
		float dr = (m_max-m_min)/m_nbins;
		unsigned int bin = int (                       (input - m_min)/dr    ) ;
		if 	(bin >  m_nbins)  	printf("  ERROR: Bin number is larger than number of bins!!!\n");
		return bin;
	}
	return 0;
}

/*****************************************************************************\
|*%%%  Default Constructor  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*|
\*****************************************************************************/

InDet::TRT_ElectronPidToolRun2::HTcalculator::HTcalculator(AthAlgTool & parent):
  BaseTRTPIDCalculator(parent, (SIZE_OF_HEADER+SIZE_OF_BLOB),"HT")
{
  m_datainplace = false;
  CurrentVersion = my_CurrentVersion;
}

/*****************************************************************************\
|*%%%  Default Destructor  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*|
\*****************************************************************************/

InDet::TRT_ElectronPidToolRun2::HTcalculator::~HTcalculator(){
  //Nothing here yet
}


/*****************************************************************************\
|*%%%  Get The Pobability of this hit being a Hight THreshold hit  %%%%%%%%%%*|
\*****************************************************************************/

// TrtPart: 0 = Barrel, 1 = EndcapA, 2 = EndcapB
// GasType: 0 = Xenon,  1 = Argon,   2 = Krypton
float InDet::TRT_ElectronPidToolRun2::HTcalculator::getProbHT(
      float pTrk, Trk::ParticleHypothesis hypothesis,
      int TrtPart, int GasType, int StrawLayer,
      float ZR, float rTrkWire, float Occupancy ) {

  checkInitialization();

  //FIXME: This setup the Troels constants. THIS OVERRIDES CURRENT DB!!!
  // setDefaultCalibrationConstants();

  float pHT = 1.0;       // Default/unit value, which ensures that unusable hits does not change probability product!
  // Make sure that the information passed makes sense:
  // --------------------------------------------------
  if (pTrk < 250.0 || pTrk > 7000000.0) return pHT;

  if (TrtPart < 0 || TrtPart > 2) return pHT;
  if (GasType < 0 || GasType > 2) return pHT;

  if ((TrtPart == 0 && (StrawLayer < 0 || StrawLayer > 72)) ||
      (TrtPart == 1 && (StrawLayer < 0 || StrawLayer > 95)) ||
      (TrtPart == 2 && (StrawLayer < 0 || StrawLayer > 63))) return pHT;

  if ((TrtPart == 0 && (ZR > 720.0)) ||
      (TrtPart  > 0 && (ZR < 630.0 || ZR > 1030.0))) return pHT;

  if (rTrkWire < 0.0 || rTrkWire > 2.2) return pHT;


  // Calculate the High Threshold probability, pHT:
  // ----------------------------------------------
  float correctionSL, correctionZR, correctionTW;
  float mass = m_particlemasses.mass[hypothesis];
  float correctionPGOG = pHTvsPGOG(TrtPart, GasType, pTrk, mass, Occupancy);
  if (fabs(mass-0.511) < 0.1) {      // Electron! OK, ugly way but works...
    correctionSL = m_CpHT_B_Zee_SL_new[GasType][TrtPart].GetValue(StrawLayer);
    correctionZR = m_CpHT_B_Zee_ZR_new[GasType][TrtPart].GetValue(ZR);
    correctionTW = m_CpHT_B_Zee_TW_new[GasType][TrtPart].GetValue(rTrkWire);
  } else {                           // Non-electron!
    correctionSL = m_CpHT_B_Zmm_SL_new[GasType][TrtPart].GetValue(StrawLayer);
    correctionZR = m_CpHT_B_Zmm_ZR_new[GasType][TrtPart].GetValue(ZR);
    correctionTW = m_CpHT_B_Zmm_TW_new[GasType][TrtPart].GetValue(rTrkWire);
  }
  parent.msg(MSG::DEBUG) << "check       "
                         << "  GammaOccupan: " << correctionPGOG
                         << "  correctionSL: " << correctionSL
                         << "  correctionZR: " << correctionZR
                         << "  correctionTW: " << correctionTW << endmsg;
  return correctionPGOG * correctionSL * correctionZR * correctionTW;
}

// ------------------------------------------------------------------------------------------------------------ //
// PART, GAMMA, OCCUPANCY, and GAS dependence functions:
// ------------------------------------------------------------------------------------------------------------ //

float InDet::TRT_ElectronPidToolRun2::HTcalculator::pHTvsPGOG(int TrtPart, int GasType, float p, float mass, float occ) {

  float gamma = sqrt(p*p + mass*mass) / mass;
   
  // The position of the upper point of linearity varies with occupancy!
  double par1 = m_par_pHTvsPGOG_new[GasType][TrtPart].GetBinValue(1) + m_par_pHTvsPGOG_new[GasType][TrtPart].GetBinValue(6)*occ;
  // The position of the onset varies with occupancy!
  double par4 = m_par_pHTvsPGOG_new[GasType][TrtPart].GetBinValue(4) + m_par_pHTvsPGOG_new[GasType][TrtPart].GetBinValue(7)*occ;

  // TR onset part (main part):
  double exp_term = exp(-(log10(gamma) - par4)/m_par_pHTvsPGOG_new[GasType][TrtPart].GetBinValue(5));
  double pHT_TR   = m_par_pHTvsPGOG_new[GasType][TrtPart].GetBinValue(2) + m_par_pHTvsPGOG_new[GasType][TrtPart].GetBinValue(3)/(1.0 + exp_term);

  // dE/dx part (linear at low gamma):
  double exp_term0 = exp(-(m_par_pHTvsPGOG_new[GasType][TrtPart].GetBinValue(0) - par4)/m_par_pHTvsPGOG_new[GasType][TrtPart].GetBinValue(5));
  double alpha0 = m_par_pHTvsPGOG_new[GasType][TrtPart].GetBinValue(2) + m_par_pHTvsPGOG_new[GasType][TrtPart].GetBinValue(3)/(1.0 + exp_term0);
  double beta0 = m_par_pHTvsPGOG_new[GasType][TrtPart].GetBinValue(3) / sqr(1.0 + exp_term0) * exp_term0 / m_par_pHTvsPGOG_new[GasType][TrtPart].GetBinValue(5);
  double pHT_dEdx = alpha0 + beta0*(log10(gamma) - m_par_pHTvsPGOG_new[GasType][TrtPart].GetBinValue(0));

  // High-gamma part (linear at high gamma):
  double exp_term1 = exp(-(par1 - par4)/m_par_pHTvsPGOG_new[GasType][TrtPart].GetBinValue(5));
  double alpha1 = m_par_pHTvsPGOG_new[GasType][TrtPart].GetBinValue(2) + m_par_pHTvsPGOG_new[GasType][TrtPart].GetBinValue(3)/(1.0 + exp_term1);
  double beta1 = m_par_pHTvsPGOG_new[GasType][TrtPart].GetBinValue(3) / sqr(1.0 + exp_term1) * exp_term1 / m_par_pHTvsPGOG_new[GasType][TrtPart].GetBinValue(5);
  double pHT_HG   = alpha1 + beta1*(log10(gamma) - par1);

  double pHT_OccZero = pHT_TR;
  if      (log10(gamma) < m_par_pHTvsPGOG_new[GasType][TrtPart].GetBinValue(0))   pHT_OccZero = pHT_dEdx;
  else if (log10(gamma) > par1  )                                                 pHT_OccZero = pHT_HG;


  // The occupancy dependency is included through the Anatoli-Petersen formula and a quadratic fit from the muon plateau:
  // ------------------------------------------------------------------------------------------------------------------------
  double DeltaOcc = m_par_pHTvsPGOG_new[GasType][TrtPart].GetBinValue(8)*occ + m_par_pHTvsPGOG_new[GasType][TrtPart].GetBinValue(9)*occ*occ;
  double pHT = pHT_OccZero + (1.0 - pHT_OccZero) * DeltaOcc;
  
  return pHT;
}



StatusCode InDet::TRT_ElectronPidToolRun2::HTcalculator::ReadVectorDB( const DataHandle<CondAttrListVec> channel_values){
   parent.msg(MSG::INFO) << "Set TRT HT PID Parameters from the Vector Database "<<endmsg;
   if ( channel_values->size() < 1){
          parent.msg(MSG::ERROR) << "There are no channels available!!" << endmsg;
          return StatusCode::FAILURE;
   }

   CondAttrListVec::const_iterator first_channel = channel_values->begin();
   CondAttrListVec::const_iterator last_channel  = channel_values->end();

   parent.msg(MSG::DEBUG) << "There are " << channel_values->size() << "  Channels " << endmsg;
   int inichan = 0;
   for (; first_channel != last_channel; ++first_channel) {
     switch(first_channel->first){
        case 0:			  // gamma_All_Xenon_All_Barrel
		m_par_pHTvsPGOG_new [0][0].push_back(first_channel->second["array_value"].data<float>());
		inichan += 1;
                break;
        case 1 :                  // gamma_All_Xenon_All_EndcapA
		m_par_pHTvsPGOG_new [0][1].push_back(first_channel->second["array_value"].data<float>());
		inichan += 1;
                break;
        case 2 :                  // gamma_All_Xenon_All_EndcapB
		m_par_pHTvsPGOG_new [0][2].push_back(first_channel->second["array_value"].data<float>());
		inichan += 1;
                break;
        case 3 :                  // gamma_All_Argon_All_Barrel
		m_par_pHTvsPGOG_new [1][0].push_back(first_channel->second["array_value"].data<float>());
		inichan += 1;
                break;
        case 4 :                  // gamma_All_Argon_All_EndcapA
		m_par_pHTvsPGOG_new [1][1].push_back(first_channel->second["array_value"].data<float>());
		inichan += 1;
                break;
        case 5 :                  // gamma_All_Argon_All_EndcapB
		m_par_pHTvsPGOG_new [1][2].push_back(first_channel->second["array_value"].data<float>());
		inichan += 1;
                break;
        case 6 :                  // gamma_All_Krypton_All_Barrel
		m_par_pHTvsPGOG_new [2][0].push_back(first_channel->second["array_value"].data<float>());
		inichan += 1;
                break;
        case 7 :                  // gamma_All_Krypton_All_EndcapA
		m_par_pHTvsPGOG_new [2][1].push_back(first_channel->second["array_value"].data<float>());
		inichan += 1;
                break;
        case 8 :                  // gamma_All_Krypton_All_EndcapB
		m_par_pHTvsPGOG_new [2][2].push_back(first_channel->second["array_value"].data<float>());
		inichan += 1;
                break;

	// Xenon Corrections: 
       case 9 :                  // SL_Zee_Xenon_Electrons_Barrel
		m_CpHT_B_Zee_SL_new [0][0].push_back(first_channel->second["array_value"].data<float>());
		inichan += 1;
                break;
        case 10 :                 // SL_Zee_Xenon_Electrons_EndcapA
		m_CpHT_B_Zee_SL_new [0][1].push_back(first_channel->second["array_value"].data<float>());
		inichan += 1;
                break;
        case 11 :                 // SL_Zee_Xenon_Electrons_EndcapB
		m_CpHT_B_Zee_SL_new [0][2].push_back(first_channel->second["array_value"].data<float>());
		inichan += 1;
                break;
        case 12 :                 // ZR_Zee_Xenon_Electrons_Barrel
		m_CpHT_B_Zee_ZR_new [0][0].push_back(first_channel->second["array_value"].data<float>());
		inichan += 1;
                break;
        case 13 :                 // ZR_Zee_Xenon_Electrons_EndcapA
		m_CpHT_B_Zee_ZR_new [0][1].push_back(first_channel->second["array_value"].data<float>());
		inichan += 1;
                break;
        case 14 :                 // ZR_Zee_Xenon_Electrons_EndcapB
		m_CpHT_B_Zee_ZR_new [0][2].push_back(first_channel->second["array_value"].data<float>());
		inichan += 1;
                break;
        case 15 :                 // TW_Zee_Xenon_Electrons_Barrel
		m_CpHT_B_Zee_TW_new [0][0].push_back(first_channel->second["array_value"].data<float>());
		inichan += 1;
                break;
        case 16 :                 // TW_Zee_Xenon_Electrons_EndcapA
		m_CpHT_B_Zee_TW_new [0][1].push_back(first_channel->second["array_value"].data<float>());
		inichan += 1;
                break;
        case 17 :                 // TW_Zee_Xenon_Electrons_EndcapB
		m_CpHT_B_Zee_TW_new [0][2].push_back(first_channel->second["array_value"].data<float>());
		inichan += 1;
                break;
        case 18 :                 // OR_Zee_Xenon_Electrons_Barrel
		m_CpHT_B_Zee_OR_new [0][0].push_back(first_channel->second["array_value"].data<float>());
		inichan += 1;
                break;
        case 19 :                 // OR_Zee_Xenon_Electrons_EndcapA
		m_CpHT_B_Zee_OR_new [0][1].push_back(first_channel->second["array_value"].data<float>());
		inichan += 1;
                break;
        case 20 :                 // OR_Zee_Xenon_Electrons_EndcapB
		m_CpHT_B_Zee_OR_new [0][2].push_back(first_channel->second["array_value"].data<float>());
		inichan += 1;
                break;
        case 21 :                 // SL_Zmm_Xenon_NonElecs_Barrel
		m_CpHT_B_Zmm_SL_new [0][0].push_back(first_channel->second["array_value"].data<float>());
		inichan += 1;
                break;
        case 22 :                 // SL_Zmm_Xenon_NonElecs_EndcapA
		m_CpHT_B_Zmm_SL_new [0][1].push_back(first_channel->second["array_value"].data<float>());
		inichan += 1;
                break;
        case 23 :                 // SL_Zmm_Xenon_NonElecs_EndcapB
		m_CpHT_B_Zmm_SL_new [0][2].push_back(first_channel->second["array_value"].data<float>());
		inichan += 1;
                break;
        case 24 :                 // ZR_Zmm_Xenon_NonElecs_Barrel
		m_CpHT_B_Zmm_ZR_new [0][0].push_back(first_channel->second["array_value"].data<float>());
		inichan += 1;
                break;
        case 25 :                 // ZR_Zmm_Xenon_NonElecs_EndcapA
		m_CpHT_B_Zmm_ZR_new [0][1].push_back(first_channel->second["array_value"].data<float>());
		inichan += 1;
                break;
        case 26 :                 // ZR_Zmm_Xenon_NonElecs_EndcapB
		m_CpHT_B_Zmm_ZR_new [0][2].push_back(first_channel->second["array_value"].data<float>());
		inichan += 1;
                break;
        case 27 :                 // TW_Zmm_Xenon_NonElecs_Barrel
		m_CpHT_B_Zmm_TW_new [0][0].push_back(first_channel->second["array_value"].data<float>());
		inichan += 1;
                break;
        case 28 :                 // TW_Zmm_Xenon_NonElecs_EndcapA
		m_CpHT_B_Zmm_TW_new [0][1].push_back(first_channel->second["array_value"].data<float>());
		inichan += 1;
                break;
        case 29 :                 // TW_Zmm_Xenon_NonElecs_EndcapB
		m_CpHT_B_Zmm_TW_new [0][2].push_back(first_channel->second["array_value"].data<float>());
		inichan += 1;
                break;
        case 30 :                 // OR_Zmm_Xenon_NonElecs_Barrel
		m_CpHT_B_Zmm_OR_new [0][0].push_back(first_channel->second["array_value"].data<float>());
		inichan += 1;
                break;
        case 31 :                 // OR_Zmm_Xenon_NonElecs_EndcapA
		m_CpHT_B_Zmm_OR_new [0][1].push_back(first_channel->second["array_value"].data<float>());
		inichan += 1;
                break;
        case 32 :                 // OR_Zmm_Xenon_NonElecs_EndcapB
		m_CpHT_B_Zmm_OR_new [0][2].push_back(first_channel->second["array_value"].data<float>());
		inichan += 1;
                break;

	// Argon Corrections: 
        case 33 :                  // SL_Zee_Xenon_Electrons_Barrel
		m_CpHT_B_Zee_SL_new [1][0].push_back(first_channel->second["array_value"].data<float>());
		inichan += 1;
                break;
        case 34 :                 // SL_Zee_Xenon_Electrons_EndcapA
		m_CpHT_B_Zee_SL_new [1][1].push_back(first_channel->second["array_value"].data<float>());
		inichan += 1;
                break;
        case 35 :                 // SL_Zee_Xenon_Electrons_EndcapB
		m_CpHT_B_Zee_SL_new [1][2].push_back(first_channel->second["array_value"].data<float>());
		inichan += 1;
                break;
        case 36 :                 // ZR_Zee_Xenon_Electrons_Barrel
		m_CpHT_B_Zee_ZR_new [1][0].push_back(first_channel->second["array_value"].data<float>());
		inichan += 1;
                break;
        case 37 :                 // ZR_Zee_Xenon_Electrons_EndcapA
		m_CpHT_B_Zee_ZR_new [1][1].push_back(first_channel->second["array_value"].data<float>());
		inichan += 1;
                break;
        case 38 :                 // ZR_Zee_Xenon_Electrons_EndcapB
		m_CpHT_B_Zee_ZR_new [1][2].push_back(first_channel->second["array_value"].data<float>());
		inichan += 1;
                break;
        case 39 :                 // TW_Zee_Xenon_Electrons_Barrel
		m_CpHT_B_Zee_TW_new [1][0].push_back(first_channel->second["array_value"].data<float>());
		inichan += 1;
                break;
        case 40 :                 // TW_Zee_Xenon_Electrons_EndcapA
		m_CpHT_B_Zee_TW_new [1][1].push_back(first_channel->second["array_value"].data<float>());
		inichan += 1;
                break;
        case 41 :                 // TW_Zee_Xenon_Electrons_EndcapB
		m_CpHT_B_Zee_TW_new [1][2].push_back(first_channel->second["array_value"].data<float>());
		inichan += 1;
                break;
        case 42 :                 // OR_Zee_Xenon_Electrons_Barrel
		m_CpHT_B_Zee_OR_new [1][0].push_back(first_channel->second["array_value"].data<float>());
		inichan += 1;
                break;
        case 43 :                 // OR_Zee_Xenon_Electrons_EndcapA
		m_CpHT_B_Zee_OR_new [1][1].push_back(first_channel->second["array_value"].data<float>());
		inichan += 1;
                break;
        case 44 :                 // OR_Zee_Xenon_Electrons_EndcapB
		m_CpHT_B_Zee_OR_new [1][2].push_back(first_channel->second["array_value"].data<float>());
		inichan += 1;
                break;
        case 45 :                 // SL_Zmm_Xenon_NonElecs_Barrel
		m_CpHT_B_Zmm_SL_new [1][0].push_back(first_channel->second["array_value"].data<float>());
		inichan += 1;
                break;
        case 46 :                 // SL_Zmm_Xenon_NonElecs_EndcapA
		m_CpHT_B_Zmm_SL_new [1][1].push_back(first_channel->second["array_value"].data<float>());
		inichan += 1;
                break;
        case 47 :                 // SL_Zmm_Xenon_NonElecs_EndcapB
		m_CpHT_B_Zmm_SL_new [1][2].push_back(first_channel->second["array_value"].data<float>());
		inichan += 1;
                break;
        case 48 :                 // ZR_Zmm_Xenon_NonElecs_Barrel
		m_CpHT_B_Zmm_ZR_new [1][0].push_back(first_channel->second["array_value"].data<float>());
		inichan += 1;
                break;
        case 49 :                 // ZR_Zmm_Xenon_NonElecs_EndcapA
		m_CpHT_B_Zmm_ZR_new [1][1].push_back(first_channel->second["array_value"].data<float>());
		inichan += 1;
                break;
        case 50 :                 // ZR_Zmm_Xenon_NonElecs_EndcapB
		m_CpHT_B_Zmm_ZR_new [1][2].push_back(first_channel->second["array_value"].data<float>());
		inichan += 1;
                break;
        case 51 :                 // TW_Zmm_Xenon_NonElecs_Barrel
		m_CpHT_B_Zmm_TW_new [1][0].push_back(first_channel->second["array_value"].data<float>());
		inichan += 1;
                break;
        case 52 :                 // TW_Zmm_Xenon_NonElecs_EndcapA
		m_CpHT_B_Zmm_TW_new [1][1].push_back(first_channel->second["array_value"].data<float>());
		inichan += 1;
                break;
        case 53 :                 // TW_Zmm_Xenon_NonElecs_EndcapB
		m_CpHT_B_Zmm_TW_new [1][2].push_back(first_channel->second["array_value"].data<float>());
		inichan += 1;
                break;
        case 54 :                 // OR_Zmm_Xenon_NonElecs_Barrel
		m_CpHT_B_Zmm_OR_new [1][0].push_back(first_channel->second["array_value"].data<float>());
		inichan += 1;
                break;
        case 55 :                 // OR_Zmm_Xenon_NonElecs_EndcapA
		m_CpHT_B_Zmm_OR_new [1][1].push_back(first_channel->second["array_value"].data<float>());
		inichan += 1;
                break;
        case 56 :                 // OR_Zmm_Xenon_NonElecs_EndcapB
		m_CpHT_B_Zmm_OR_new [1][2].push_back(first_channel->second["array_value"].data<float>());
		inichan += 1;
                break;


	// Krypton Corrections: 
        case 57 :                  // SL_Zee_Xenon_Electrons_Barrel
		m_CpHT_B_Zee_SL_new [2][0].push_back(first_channel->second["array_value"].data<float>());
		inichan += 1;
                break;
        case 58 :                 // SL_Zee_Xenon_Electrons_EndcapA
		m_CpHT_B_Zee_SL_new [2][1].push_back(first_channel->second["array_value"].data<float>());
		inichan += 1;
                break;
        case 59 :                 // SL_Zee_Xenon_Electrons_EndcapB
		m_CpHT_B_Zee_SL_new [2][2].push_back(first_channel->second["array_value"].data<float>());
		inichan += 1;
                break;
        case 60 :                 // ZR_Zee_Xenon_Electrons_Barrel
		m_CpHT_B_Zee_ZR_new [2][0].push_back(first_channel->second["array_value"].data<float>());
		inichan += 1;
                break;
        case 61 :                 // ZR_Zee_Xenon_Electrons_EndcapA
		m_CpHT_B_Zee_ZR_new [2][1].push_back(first_channel->second["array_value"].data<float>());
		inichan += 1;
                break;
        case 62 :                 // ZR_Zee_Xenon_Electrons_EndcapB
		m_CpHT_B_Zee_ZR_new [2][2].push_back(first_channel->second["array_value"].data<float>());
		inichan += 1;
                break;
        case 63 :                 // TW_Zee_Xenon_Electrons_Barrel
		m_CpHT_B_Zee_TW_new [2][0].push_back(first_channel->second["array_value"].data<float>());
		inichan += 1;
                break;
        case 64 :                 // TW_Zee_Xenon_Electrons_EndcapA
		m_CpHT_B_Zee_TW_new [2][1].push_back(first_channel->second["array_value"].data<float>());
		inichan += 1;
                break;
        case 65 :                 // TW_Zee_Xenon_Electrons_EndcapB
		m_CpHT_B_Zee_TW_new [2][2].push_back(first_channel->second["array_value"].data<float>());
		inichan += 1;
                break;
        case 66 :                 // OR_Zee_Xenon_Electrons_Barrel
		m_CpHT_B_Zee_OR_new [2][0].push_back(first_channel->second["array_value"].data<float>());
		inichan += 1;
                break;
        case 67 :                 // OR_Zee_Xenon_Electrons_EndcapA
		m_CpHT_B_Zee_OR_new [2][1].push_back(first_channel->second["array_value"].data<float>());
		inichan += 1;
                break;
        case 68 :                 // OR_Zee_Xenon_Electrons_EndcapB
		m_CpHT_B_Zee_OR_new [2][2].push_back(first_channel->second["array_value"].data<float>());
		inichan += 1;
                break;
        case 69 :                 // SL_Zmm_Xenon_NonElecs_Barrel
		m_CpHT_B_Zmm_SL_new [2][0].push_back(first_channel->second["array_value"].data<float>());
		inichan += 1;
                break;
        case 70 :                 // SL_Zmm_Xenon_NonElecs_EndcapA
		m_CpHT_B_Zmm_SL_new [2][1].push_back(first_channel->second["array_value"].data<float>());
		inichan += 1;
                break;
        case 71 :                 // SL_Zmm_Xenon_NonElecs_EndcapB
		m_CpHT_B_Zmm_SL_new [2][2].push_back(first_channel->second["array_value"].data<float>());
		inichan += 1;
                break;
        case 72 :                 // ZR_Zmm_Xenon_NonElecs_Barrel
		m_CpHT_B_Zmm_ZR_new [2][0].push_back(first_channel->second["array_value"].data<float>());
		inichan += 1;
                break;
        case 73 :                 // ZR_Zmm_Xenon_NonElecs_EndcapA
		m_CpHT_B_Zmm_ZR_new [2][1].push_back(first_channel->second["array_value"].data<float>());
		inichan += 1;
                break;
        case 74 :                 // ZR_Zmm_Xenon_NonElecs_EndcapB
		m_CpHT_B_Zmm_ZR_new [2][2].push_back(first_channel->second["array_value"].data<float>());
		inichan += 1;
                break;
        case 75 :                 // TW_Zmm_Xenon_NonElecs_Barrel
		m_CpHT_B_Zmm_TW_new [2][0].push_back(first_channel->second["array_value"].data<float>());
		inichan += 1;
                break;
        case 76 :                 // TW_Zmm_Xenon_NonElecs_EndcapA
		m_CpHT_B_Zmm_TW_new [2][1].push_back(first_channel->second["array_value"].data<float>());
		inichan += 1;
                break;
        case 77 :                 // TW_Zmm_Xenon_NonElecs_EndcapB
		m_CpHT_B_Zmm_TW_new [2][2].push_back(first_channel->second["array_value"].data<float>());
		inichan += 1;
                break;
        case 78 :                 // OR_Zmm_Xenon_NonElecs_Barrel
		m_CpHT_B_Zmm_OR_new [2][0].push_back(first_channel->second["array_value"].data<float>());
		inichan += 1;
                break;
        case 79 :                 // OR_Zmm_Xenon_NonElecs_EndcapA
		m_CpHT_B_Zmm_OR_new [2][1].push_back(first_channel->second["array_value"].data<float>());
		inichan += 1;
                break;
        case 80 :                 // OR_Zmm_Xenon_NonElecs_EndcapB
		m_CpHT_B_Zmm_OR_new [2][2].push_back(first_channel->second["array_value"].data<float>());
		inichan += 1;
                break;
	}
    }

   parent.msg(MSG::DEBUG) << "We have read " << inichan << " good channels" << endmsg;
   parent.msg(MSG::DEBUG) << m_par_pHTvsPGOG_new [0][0].GetBinValue(0) << "\t" << m_par_pHTvsPGOG_new [0][0].GetBinValue(1) << " " << m_par_pHTvsPGOG_new [0][0].GetBinValue(2) << endmsg;


   for (int i = 0 ; i < N_DET; i++)
   {
    for (int j = 0 ; j < N_GAS; j++)
	{
	 if (m_par_pHTvsPGOG_new	      [j][i].check(j,i) != StatusCode::SUCCESS) 	return StatusCode::FAILURE;
   	 if (m_CpHT_B_Zee_SL_new              [j][i].check(j,i) != StatusCode::SUCCESS) 	return StatusCode::FAILURE;
   	 if (m_CpHT_B_Zmm_SL_new              [j][i].check(j,i) != StatusCode::SUCCESS) 	return StatusCode::FAILURE;
  	 if (m_CpHT_B_Zee_ZR_new              [j][i].check(j,i) != StatusCode::SUCCESS) 	return StatusCode::FAILURE;
  	 if (m_CpHT_B_Zmm_ZR_new              [j][i].check(j,i) != StatusCode::SUCCESS) 	return StatusCode::FAILURE;
   	 if (m_CpHT_B_Zee_TW_new              [j][i].check(j,i) != StatusCode::SUCCESS) 	return StatusCode::FAILURE;
   	 if (m_CpHT_B_Zmm_TW_new              [j][i].check(j,i) != StatusCode::SUCCESS) 	return StatusCode::FAILURE;
   	 if (m_CpHT_B_Zee_OR_new              [j][i].check(j,i) != StatusCode::SUCCESS) 	return StatusCode::FAILURE;
   	 if (m_CpHT_B_Zmm_OR_new              [j][i].check(j,i) != StatusCode::SUCCESS) 	return StatusCode::FAILURE;
	}
   } 

   HasBeenInitialized=1;
   parent.msg(MSG::INFO) <<  "TRT PID HT Vector DB loaded: " << endmsg;
   return StatusCode::SUCCESS;
}


/*****************************************************************************\
|*%%%  Hard-coded HT Calibration Constants  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*|
\*****************************************************************************/

void InDet::TRT_ElectronPidToolRun2::HTcalculator::setDefaultCalibrationConstants(){
  /*****************************************************************************\

    This code is never called in production. It is used to set all
    constants in the positions in the HTBlob where they are needed, and 
    finally print out the blob as an array of numbers. This is far easier and 
    less error prone than having a separate setter-script which might itself 
    have a version mismatch with this code.

    PLEASE REMEMBER to increment the version number precisely when you change 
    the addresses of the various arrays inside the HTBlob, and NEVER otherwise!
    
  \*****************************************************************************/
	//FIXME
  if (m_datainplace) return;  // Just to load 1 time
  parent.msg(MSG::ERROR) << "Looks like HT PID DB is NOT available, so lets set hard-coded PID calibration constants. Derived from Run1 Data Zee and Zmumu 50 ns. FIXME!!"<<endmsg;
  HasBeenInitialized=1;

// Expanding to a 2D fit (gamma,occupancy) for three types of gases: Xenon, Argon, Krypton:
// ----------------------------------------------------------------------------------------
  float par2[N_GAS][N_DET][N_PAR2] = { 
    // Xenon Gas Parameters
       {{ 1.0000, 3.7204, 0.0260, 0.1445, 3.0461, 0.2206, 0.0000, 0.0078, 0.0918, 0.0744},    // Barrel   Prob: 0.9992 
        { 1.0000, 3.5836, 0.0468, 0.1475, 3.0943, 0.1303, 0.0000, 0.0089, 0.1054, 0.0472},    // EndcapA  Prob: 1.0000 
        { 1.0000, 3.4798, 0.0433, 0.1824, 3.0730, 0.1244, 0.0000, 0.0300, 0.1007, 0.1261}},   // EndcapB  Prob: 0.8536
    // Argon Gas Parameters 
       {{ 1.0000, 2.8342, 0.0384, 0.0185, 2.7161, 0.0366, 0.0000, 0.0013, 0.1261, 0.1241},    // Barrel   Prob: 1.0000 
        { 1.0000, 3.2551, 0.0388, 0.0338, 2.9090, 0.1663, 0.0000, 0.1604, 0.1100, 0.0521},    // EndcapA  Prob: 0.9970
        { 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000}},   // EndcapB  ------------
    // Krypton Gas Parameters (Place Holder) 
       {{ 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000},    // Barrel   ------------
        { 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000},    // EndcapA  ------------
        { 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000}}};  // EndcapB  ------------

/*
  // Parameters for 1D onset function:
  for (int j = 0 ; j < N_DET; j++)
    for (int k = 0; k < N_PAR ; k++)
      m_par_pHTvsP[j][k] = par[j][k];
*/
  
  for (int i = 0 ; i < N_GAS; i++)
    for (int j = 0 ; j < N_DET; j++)
	m_par_pHTvsPGOG_new[i][j] .update (  10, 50, 1000000.0,   std::vector<float> (par2[i][j] , par2[i][j]  + sizeof par2[i][j] / sizeof par2[i][j][0]) );


  const int N_SL_B  = 73;
  const int N_SL_EA = 96;
  const int N_SL_EB = 64;
  const int N_ZR_B  = 36;
  const int N_ZR_EA = 40;
  const int N_ZR_EB = 40;
  const int N_TW_B  = 44;
  const int N_TW_EA = 44;
  const int N_TW_EB = 44;
/*
  const int N_OL_B  = 50;
  const int N_OL_EA = 50;
  const int N_OL_EB = 50;
*/

  // ---------------------------------------
  // Electrons:
  // ---------------------------------------
  // Straw Layer (SL):
  double CpHT_Zee_Barrel_SL[N_SL_B] = { 0.637, 0.887, 0.966, 1.034, 1.059, 1.009, 1.131, 1.073, 1.086, 0.925, 0.890, 0.987, 0.937, 0.964, 0.976, 0.929, 1.006, 0.979, 0.992, 0.812, 0.935, 0.950, 0.984, 0.994, 1.011, 0.952, 1.051, 0.997, 1.026, 1.018, 0.978, 1.066, 1.016, 1.039, 1.040, 0.979, 1.057, 1.018, 1.032, 1.052, 0.994, 1.055, 1.023, 0.823, 1.013, 0.977, 1.051, 1.031, 0.973, 1.077, 1.025, 1.056, 1.047, 0.992, 1.085, 1.032, 1.061, 1.054, 0.998, 1.093, 1.039, 1.058, 1.056, 0.988, 1.090, 1.057, 1.046, 1.053, 0.994, 1.081, 1.041, 1.040, 1.061};
  double CpHT_Zee_EndcapA_SL[N_SL_EA] = { 0.671, 0.802, 0.890, 0.918, 0.946, 0.963, 0.974, 0.979, 1.030, 1.023, 1.029, 1.004, 1.030, 1.037, 1.033, 1.013, 0.913, 0.968, 0.998, 0.994, 1.036, 1.032, 1.043, 1.044, 1.042, 1.009, 1.026, 1.007, 1.032, 1.046, 1.020, 1.032, 0.913, 0.955, 0.974, 0.995, 1.035, 1.042, 1.039, 1.047, 1.032, 1.036, 1.033, 1.010, 1.047, 1.049, 1.055, 1.046, 0.877, 0.938, 0.968, 0.983, 1.004, 1.010, 1.013, 1.014, 1.038, 1.031, 1.042, 1.018, 1.016, 1.049, 1.023, 1.025, 0.923, 0.978, 0.995, 1.001, 1.038, 1.042, 1.026, 1.037, 1.042, 1.062, 1.041, 1.039, 1.078, 1.058, 1.036, 1.049, 0.897, 0.965, 0.993, 0.985, 1.040, 1.068, 1.053, 1.049, 1.037, 1.050, 1.043, 1.065, 1.026, 1.058, 1.058, 1.070};
  double CpHT_Zee_EndcapB_SL[N_SL_EB] = { 0.494, 0.771, 0.887, 0.931, 0.939, 0.989, 0.994, 1.005, 0.866, 0.971, 1.027, 1.057, 1.021, 1.056, 1.046, 1.073, 0.901, 0.992, 1.043, 1.055, 1.034, 1.087, 1.094, 1.087, 0.920, 0.995, 1.048, 1.068, 1.042, 1.075, 1.086, 1.126, 0.920, 0.987, 1.062, 1.072, 1.059, 1.096, 1.070, 1.082, 0.927, 1.020, 1.068, 1.083, 1.054, 1.089, 1.078, 1.103, 0.961, 1.050, 1.100, 1.107, 1.098, 1.124, 1.101, 1.141, 0.988, 1.106, 1.127, 1.174, 1.109, 1.134, 1.134, 1.182};

  // ZR-position (ZR - Z in Barrel, R in Endcaps):
  double CpHT_Zee_Barrel_ZR[N_ZR_B] = { 0.861, 0.901, 0.909, 0.915, 0.919, 0.924, 0.922, 0.931, 0.928, 0.937, 0.945, 0.908, 0.921, 0.959, 0.970, 0.988, 0.986, 0.999, 1.010, 1.005, 0.996, 1.005, 1.018, 0.992, 1.042, 1.076, 1.101, 1.116, 1.134, 1.157, 1.170, 1.196, 1.208, 1.230, 1.230, 1.039};
  double CpHT_Zee_EndcapA_ZR[N_ZR_EA] = { 0.458, 0.621, 0.694, 0.758, 0.798, 0.838, 0.871, 0.900, 0.956, 0.980, 1.001, 1.006, 1.016, 1.027, 1.019, 1.038, 1.046, 1.045, 1.054, 1.064, 1.064, 1.077, 1.081, 1.089, 1.101, 1.113, 1.102, 1.113, 1.107, 1.113, 1.098, 1.105, 1.083, 1.054, 1.036, 0.994, 0.965, 0.887, 0.771, 0.575};
  double CpHT_Zee_EndcapB_ZR[N_ZR_EB] = { 1.000, 0.754, 0.926, 0.941, 1.001, 1.001, 0.987, 1.033, 1.054, 1.060, 1.057, 1.064, 1.061, 1.067, 1.052, 1.062, 1.045, 1.057, 1.053, 1.047, 1.053, 1.050, 1.042, 1.073, 1.050, 1.050, 1.028, 0.972, 0.928, 0.896, 0.881, 0.854, 0.828, 0.793, 0.755, 0.652, 0.511, 0.291, 0.481, 1.000};

  // Track-to-Wire distance (TW):
  double CpHT_Zee_Barrel_TW[44] = { 1.233, 1.261, 1.276, 1.296, 1.307, 1.338, 1.349, 1.386, 1.395, 1.434, 1.441, 1.448, 1.440, 1.439, 1.425, 1.406, 1.388, 1.363, 1.334, 1.320, 1.295, 1.269, 1.240, 1.212, 1.183, 1.144, 1.109, 1.073, 1.028, 0.981, 0.938, 0.879, 0.817, 0.752, 0.678, 0.606, 0.531, 0.465, 0.428, 0.443, 0.504, 0.553, 0.579, 0.766};
  double CpHT_Zee_EndcapA_TW[44] = { 1.236, 1.260, 1.291, 1.292, 1.304, 1.325, 1.354, 1.363, 1.387, 1.394, 1.409, 1.415, 1.407, 1.414, 1.405, 1.394, 1.385, 1.357, 1.345, 1.331, 1.309, 1.282, 1.252, 1.226, 1.197, 1.176, 1.135, 1.097, 1.047, 1.013, 0.946, 0.892, 0.834, 0.756, 0.696, 0.610, 0.547, 0.480, 0.444, 0.445, 0.469, 0.513, 0.584, 0.892};
  double CpHT_Zee_EndcapB_TW[44] = { 1.186, 1.202, 1.219, 1.246, 1.257, 1.270, 1.291, 1.297, 1.307, 1.319, 1.333, 1.338, 1.340, 1.326, 1.314, 1.327, 1.321, 1.310, 1.289, 1.279, 1.266, 1.240, 1.222, 1.194, 1.168, 1.153, 1.125, 1.091, 1.033, 1.009, 0.963, 0.917, 0.846, 0.802, 0.746, 0.690, 0.615, 0.560, 0.514, 0.485, 0.478, 0.473, 0.523, 0.726};


  // ---------------------------------------
  // Non-Electrons (here muons):
  // ---------------------------------------

  // Straw Layer (SL):
  double CpHT_Zmm_Barrel_SL[N_SL_B] = { 1.100, 1.186, 1.209, 1.241, 1.199, 1.174, 1.209, 1.178, 1.150, 1.053, 1.033, 1.054, 1.033, 1.029, 1.041, 1.021, 1.027, 0.992, 0.988, 0.983, 0.998, 1.022, 1.043, 1.023, 1.027, 1.016, 1.034, 1.009, 1.014, 1.022, 1.001, 1.024, 1.003, 1.010, 1.004, 0.983, 0.992, 0.978, 0.981, 1.000, 0.984, 0.974, 0.953, 0.941, 0.982, 0.990, 1.005, 0.993, 0.966, 0.997, 1.000, 0.988, 0.992, 0.969, 1.003, 0.964, 0.989, 0.961, 0.956, 0.971, 0.948, 0.963, 0.951, 0.943, 0.964, 0.965, 0.925, 0.919, 0.918, 0.928, 0.919, 0.912, 0.906};
  double CpHT_Zmm_EndcapA_SL[N_SL_EA] = { 0.883, 0.898, 0.923, 0.899, 0.892, 0.909, 0.893, 0.925, 0.964, 0.964, 0.979, 0.949, 0.944, 0.998, 0.940, 0.937, 0.950, 0.976, 0.972, 0.950, 0.998, 1.005, 1.007, 1.028, 1.018, 0.995, 1.006, 0.998, 1.031, 1.047, 1.031, 1.015, 1.017, 0.983, 1.018, 1.018, 1.025, 1.033, 1.046, 1.069, 1.033, 1.027, 1.006, 0.982, 1.066, 1.080, 1.048, 1.058, 0.955, 0.971, 0.973, 0.992, 1.013, 1.046, 1.022, 1.029, 1.040, 1.016, 1.077, 1.024, 1.011, 1.095, 1.019, 1.045, 1.001, 1.057, 1.043, 1.022, 1.033, 1.108, 1.062, 1.110, 1.090, 1.058, 1.060, 1.099, 1.065, 1.120, 1.077, 1.060, 1.024, 1.006, 1.022, 1.007, 1.051, 1.118, 1.079, 1.118, 1.070, 1.064, 1.108, 1.127, 1.039, 1.107, 1.088, 1.154};
  double CpHT_Zmm_EndcapB_SL[N_SL_EB] = { 0.828, 0.961, 0.941, 0.991, 0.986, 1.015, 0.993, 0.957, 0.892, 1.005, 1.100, 1.054, 0.995, 1.042, 1.022, 1.007, 0.918, 1.019, 1.056, 1.034, 0.978, 0.981, 1.014, 1.026, 0.988, 0.978, 1.062, 1.085, 1.029, 0.989, 1.067, 1.054, 0.978, 0.971, 1.051, 1.114, 1.152, 1.172, 1.034, 1.170, 1.055, 0.990, 1.112, 1.047, 1.068, 1.013, 1.089, 1.141, 0.903, 0.960, 1.138, 1.218, 0.991, 1.087, 0.997, 1.028, 1.042, 1.155, 1.060, 1.130, 1.077, 1.186, 1.006, 1.054};

  // ZR-position (ZR - Z in Barrel, R in Endcaps):
  double CpHT_Zmm_Barrel_ZR[N_ZR_B] = { 0.846, 0.874, 0.880, 0.882, 0.876, 0.887, 0.901, 0.894, 0.894, 0.903, 0.902, 0.907, 0.918, 0.934, 0.941, 0.948, 0.963, 0.969, 0.990, 0.991, 1.012, 1.019, 1.029, 1.033, 1.072, 1.088, 1.111, 1.144, 1.164, 1.192, 1.225, 1.242, 1.271, 1.314, 1.309, 1.078};
  double CpHT_Zmm_EndcapA_ZR[N_ZR_EA] = { 0.613, 0.757, 0.783, 0.849, 0.866, 0.886, 0.915, 0.939, 0.930, 0.976, 0.969, 0.984, 0.992, 0.979, 1.006, 1.000, 1.005, 1.022, 1.020, 1.030, 1.031, 1.036, 1.053, 1.050, 1.050, 1.048, 1.065, 1.071, 1.060, 1.077, 1.067, 1.072, 1.070, 1.067, 1.090, 1.059, 1.032, 1.081, 1.011, 0.984};
  double CpHT_Zmm_EndcapB_ZR[N_ZR_EB] = { 1.000, 1.375, 0.962, 0.702, 0.869, 0.899, 0.953, 0.905, 1.052, 1.025, 1.016, 1.009, 1.033, 0.920, 1.056, 1.031, 1.070, 1.042, 1.052, 1.066, 1.024, 1.023, 1.046, 1.046, 1.007, 1.009, 1.009, 1.024, 1.007, 0.993, 0.968, 0.997, 0.911, 0.922, 0.938, 0.921, 0.883, 0.653, 0.917, 1.000};

  // Track-to-Wire distance (TWdist):
  double CpHT_Zmm_Barrel_TW[N_TW_B] = { 1.124, 1.058, 1.065, 1.079, 1.094, 1.124, 1.141, 1.173, 1.207, 1.226, 1.250, 1.250, 1.258, 1.249, 1.258, 1.243, 1.229, 1.211, 1.206, 1.180, 1.165, 1.138, 1.123, 1.100, 1.074, 1.052, 1.014, 0.981, 0.953, 0.896, 0.866, 0.809, 0.776, 0.736, 0.690, 0.644, 0.609, 0.615, 0.680, 0.854, 1.094, 1.274, 1.208, 1.219};
  double CpHT_Zmm_EndcapA_TW[N_TW_EA] = { 1.210, 1.161, 1.177, 1.201, 1.221, 1.244, 1.279, 1.300, 1.319, 1.341, 1.362, 1.372, 1.376, 1.378, 1.384, 1.361, 1.349, 1.334, 1.325, 1.284, 1.264, 1.250, 1.223, 1.183, 1.121, 1.104, 1.077, 1.016, 0.969, 0.912, 0.863, 0.815, 0.753, 0.662, 0.604, 0.555, 0.513, 0.490, 0.511, 0.627, 0.843, 1.019, 0.932, 0.922};
  double CpHT_Zmm_EndcapB_TW[N_TW_EB] = { 1.132, 1.150, 1.125, 1.174, 1.170, 1.282, 1.165, 1.244, 1.287, 1.293, 1.270, 1.366, 1.317, 1.285, 1.319, 1.291, 1.304, 1.239, 1.256, 1.279, 1.212, 1.221, 1.200, 1.174, 1.143, 1.120, 1.022, 0.983, 0.938, 0.895, 0.906, 0.826, 0.766, 0.765, 0.664, 0.566, 0.553, 0.556, 0.541, 0.626, 0.780, 0.964, 0.817, 0.542};


//%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-//
//%%   Correction Factors derived from MC15   %%// 
//%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-//
/*
  // ---------------------------------------
  // Electrons:
  // ---------------------------------------
 
  // Straw Layer (SL):
  double CpHT_Electrons_Barrel_SL[N_SL_B] = { 1.000, 1.000, 1.000, 1.000, 1.000, 1.000, 1.000, 1.000, 1.000, 1.000, 1.000, 1.000, 1.000, 1.000, 1.000, 1.000, 1.000, 1.000, 1.000, 0.981, 1.100, 1.052, 1.109, 1.052, 1.086, 1.017, 1.099, 1.038, 1.078, 1.048, 0.999, 1.072, 1.030, 1.053, 1.032, 0.982, 1.055, 1.015, 1.025, 1.023, 0.968, 1.051, 1.029, 0.785, 0.967, 0.922, 1.002, 0.968, 0.928, 1.008, 0.958, 0.993, 0.976, 0.920, 1.033, 0.950, 0.996, 0.992, 0.925, 1.005, 0.960, 0.983, 0.984, 0.922, 1.005, 0.959, 0.975, 0.979, 0.915, 0.996, 0.963, 0.952, 0.974}; 
  double CpHT_Electrons_EndcapA_SL[N_SL_EA] = { 0.687, 0.842, 0.964, 1.019, 1.021, 1.047, 1.042, 1.048, 1.038, 1.063, 1.061, 1.061, 1.067, 1.058, 1.083, 1.053, 0.926, 0.978, 1.015, 1.029, 1.053, 1.021, 1.036, 1.039, 1.037, 1.028, 1.029, 1.022, 1.038, 1.040, 1.020, 1.006, 0.880, 0.948, 0.961, 0.984, 1.007, 0.994, 1.017, 1.016, 1.019, 1.043, 1.027, 1.024, 1.019, 0.993, 1.032, 1.029, 0.879, 0.963, 0.944, 0.970, 0.970, 0.960, 1.011, 1.015, 1.010, 1.009, 1.016, 1.034, 1.011, 0.966, 1.011, 1.021, 0.921, 0.999, 0.979, 0.997, 1.015, 1.004, 1.009, 1.022, 0.991, 1.003, 0.982, 1.018, 1.006, 1.037, 1.009, 1.011, 0.875, 0.962, 0.936, 0.957, 0.949, 1.004, 0.968, 0.991, 0.975, 1.054, 0.993, 1.033, 0.988, 0.993, 1.010, 1.024}; 
  double CpHT_Electrons_EndcapB_SL[N_SL_EB] = { 0.443, 0.711, 0.819, 0.870, 0.906, 0.934, 0.987, 0.988, 0.834, 0.918, 0.978, 0.998, 0.992, 1.013, 1.042, 1.040, 0.897, 0.983, 1.037, 1.064, 1.036, 1.080, 1.063, 1.092, 0.915, 1.001, 1.052, 1.096, 1.055, 1.070, 1.087, 1.085, 0.940, 1.009, 1.062, 1.081, 1.070, 1.098, 1.086, 1.112, 0.922, 1.018, 1.059, 1.094, 1.099, 1.064, 1.143, 1.127, 0.939, 1.030, 1.063, 1.133, 1.136, 1.146, 1.141, 1.154, 0.979, 1.048, 1.125, 1.112, 1.137, 1.184, 1.154, 1.193}; 
  
  // ZR-position (ZR - Z in Barrel, R in Endcaps):
  double CpHT_Electrons_Barrel_ZR[N_ZR_B] = { 0.889, 0.926, 0.925, 0.932, 0.933, 0.939, 0.943, 0.943, 0.948, 0.958, 0.954, 0.967, 0.982, 0.986, 0.989, 1.001, 1.001, 1.020, 1.022, 1.036, 1.045, 1.050, 1.055, 1.067, 1.065, 1.074, 1.076, 1.078, 1.090, 1.079, 1.081, 1.084, 1.082, 1.084, 1.077, 0.995}; 
  double CpHT_Electrons_EndcapA_ZR[N_ZR_EA] = { 1.000, 0.623, 0.749, 0.882, 0.918, 0.951, 0.963, 0.966, 0.984, 1.001, 1.001, 1.005, 1.010, 1.030, 1.035, 1.029, 1.049, 1.061, 1.058, 1.045, 1.054, 1.061, 1.065, 1.056, 1.080, 1.092, 1.091, 1.102, 1.117, 1.100, 1.089, 1.067, 1.054, 1.053, 1.058, 1.014, 0.924, 0.834, 0.419, 1.000}; 
  double CpHT_Electrons_EndcapB_ZR[N_ZR_EB] = { 1.000, 0.626, 0.833, 0.941, 0.973, 0.999, 1.013, 1.011, 1.017, 1.038, 1.038, 1.020, 1.021, 1.045, 1.052, 1.037, 1.046, 1.041, 1.049, 1.070, 1.053, 1.043, 1.072, 1.071, 1.056, 1.058, 1.069, 1.084, 1.074, 1.062, 1.014, 0.962, 0.956, 0.900, 0.893, 0.916, 0.809, 0.620, 0.578, 1.000}; 
   
  // Track-to-Wire distance (TWdist):
  double CpHT_Electrons_Barrel_TW[N_TW_B] = { 1.133, 1.183, 1.195, 1.219, 1.242, 1.267, 1.281, 1.285, 1.324, 1.342, 1.344, 1.365, 1.352, 1.344, 1.327, 1.300, 1.303, 1.294, 1.269, 1.257, 1.242, 1.220, 1.197, 1.174, 1.144, 1.117, 1.098, 1.058, 1.027, 1.004, 0.954, 0.905, 0.831, 0.750, 0.656, 0.553, 0.450, 0.383, 0.368, 0.438, 0.520, 0.567, 0.605, 0.617}; 
  double CpHT_Electrons_EndcapA_TW[N_TW_EA] = { 1.024, 1.075, 1.092, 1.089, 1.134, 1.136, 1.142, 1.151, 1.175, 1.196, 1.223, 1.202, 1.234, 1.231, 1.212, 1.204, 1.224, 1.192, 1.183, 1.201, 1.165, 1.167, 1.125, 1.121, 1.103, 1.079, 1.082, 1.059, 1.022, 0.978, 0.954, 0.910, 0.867, 0.819, 0.751, 0.659, 0.595, 0.515, 0.482, 0.531, 0.571, 0.581, 0.791, 0.782}; 
  double CpHT_Electrons_EndcapB_TW[N_TW_EB] = { 1.024, 1.038, 1.082, 1.070, 1.109, 1.118, 1.139, 1.146, 1.167, 1.167, 1.201, 1.176, 1.191, 1.192, 1.193, 1.187, 1.190, 1.181, 1.167, 1.130, 1.167, 1.146, 1.125, 1.107, 1.102, 1.087, 1.076, 1.011, 1.008, 0.985, 0.952, 0.935, 0.851, 0.810, 0.771, 0.743, 0.638, 0.577, 0.543, 0.503, 0.568, 0.552, 0.586, 0.649}; 
  
  // ---------------------------------------
  // Non-Electrons (here muons):
  // ---------------------------------------
  // Straw Layer (SL):
  double CpHT_NonElecs_Barrel_SL[73] = { 1.000, 1.000, 1.000, 1.000, 1.000, 1.000, 1.000, 1.000, 1.000, 1.000, 1.000, 1.000, 1.000, 1.000, 1.000, 1.000, 1.000, 1.000, 1.000, 1.263, 1.224, 1.217, 1.207, 1.200, 1.208, 1.188, 1.197, 1.151, 1.124, 1.136, 1.117, 1.137, 1.105, 1.105, 1.088, 1.076, 1.082, 1.071, 1.027, 1.030, 1.034, 1.028, 1.033, 0.979, 0.986, 0.963, 0.969, 0.975, 0.974, 0.968, 0.962, 0.944, 0.945, 0.936, 0.927, 0.954, 0.914, 0.864, 0.904, 0.900, 0.890, 0.888, 0.864, 0.836, 0.879, 0.868, 0.860, 0.865, 0.840, 0.824, 0.838, 0.815, 0.804}; 
  double CpHT_NonElecs_EndcapA_SL[96] = { 1.030, 1.012, 1.053, 1.023, 1.001, 1.037, 1.026, 1.005, 1.026, 0.938, 0.964, 1.046, 0.951, 1.017, 0.974, 1.015, 0.971, 0.969, 0.967, 1.039, 0.970, 1.001, 0.967, 0.971, 1.028, 0.946, 0.970, 0.930, 0.938, 1.010, 0.991, 1.002, 0.970, 0.956, 0.965, 1.039, 0.985, 0.971, 0.955, 1.012, 1.021, 1.010, 1.006, 0.959, 1.020, 0.935, 1.028, 0.965, 0.931, 0.956, 1.052, 1.015, 0.937, 1.043, 1.023, 0.991, 1.099, 1.000, 1.061, 0.991, 1.036, 1.029, 0.982, 1.063, 0.984, 1.010, 1.040, 0.947, 1.074, 0.942, 0.986, 1.045, 1.102, 1.052, 1.023, 0.984, 1.016, 1.068, 1.087, 1.061, 1.053, 0.998, 1.059, 0.973, 1.150, 1.069, 1.066, 1.072, 1.057, 0.904, 1.050, 0.920, 1.064, 0.938, 1.053, 1.172}; 
  double CpHT_NonElecs_EndcapB_SL[64] = { 0.934, 0.971, 0.988, 0.940, 0.831, 1.008, 0.999, 1.011, 0.834, 0.952, 0.845, 0.917, 0.899, 0.924, 1.059, 0.994, 1.016, 1.056, 0.915, 1.111, 0.995, 1.110, 1.063, 0.978, 0.941, 0.928, 1.008, 1.228, 0.966, 1.003, 1.001, 0.961, 1.112, 0.979, 1.000, 1.009, 1.072, 1.033, 1.003, 1.121, 1.052, 0.944, 1.092, 1.040, 1.094, 1.033, 0.984, 1.117, 1.192, 1.179, 1.042, 0.981, 1.084, 1.057, 1.269, 0.981, 1.028, 1.114, 1.181, 1.164, 1.172, 1.100, 1.033, 1.117}; 
  
  // ZR-position (ZR - Z in Barrel, R in Endcaps):
  double CpHT_NonElecs_Barrel_ZR[36] = { 0.992, 1.011, 1.007, 1.013, 1.001, 0.998, 1.011, 0.976, 0.984, 1.007, 1.024, 0.984, 1.014, 1.014, 0.986, 1.005, 1.009, 0.998, 1.001, 1.020, 0.996, 0.998, 0.985, 1.003, 1.015, 1.003, 1.000, 0.986, 0.992, 0.992, 0.982, 0.993, 0.986, 1.008, 0.970, 0.998}; 
  double CpHT_NonElecs_EndcapA_ZR[40] = { 1.000, 0.948, 0.968, 0.975, 0.973, 0.968, 1.000, 0.943, 1.014, 0.954, 0.983, 0.983, 1.021, 1.037, 0.999, 0.959, 1.000, 0.993, 0.993, 0.981, 1.023, 0.995, 1.027, 1.014, 1.007, 0.962, 1.026, 1.063, 1.042, 1.054, 1.009, 1.044, 1.004, 1.038, 1.047, 1.027, 0.975, 0.961, 0.724, 1.000}; 
  double CpHT_NonElecs_EndcapB_ZR[40] = { 1.000, 0.995, 0.926, 1.016, 1.060, 0.991, 1.017, 1.043, 0.975, 1.030, 1.016, 1.032, 0.976, 1.014, 0.919, 1.037, 0.952, 1.068, 0.948, 0.995, 1.042, 1.040, 1.006, 1.004, 0.987, 0.946, 1.051, 1.015, 0.978, 1.005, 1.064, 0.996, 0.940, 1.021, 0.987, 1.019, 0.971, 0.895, 1.000, 1.000}; 
  
  // Track-to-Wire distance (TWdist):
  double CpHT_NonElecs_Barrel_TW[44] = { 1.051, 1.059, 1.064, 1.071, 1.072, 1.098, 1.100, 1.112, 1.111, 1.095, 1.138, 1.123, 1.116, 1.117, 1.123, 1.101, 1.105, 1.096, 1.103, 1.070, 1.093, 1.070, 1.051, 1.061, 1.056, 1.032, 1.021, 1.006, 0.995, 0.968, 0.957, 0.928, 0.862, 0.843, 0.762, 0.709, 0.668, 0.661, 0.737, 0.970, 1.339, 1.423, 1.144, 1.389}; 
  double CpHT_NonElecs_EndcapA_TW[44] = { 0.997, 1.041, 1.039, 1.013, 1.055, 1.064, 1.063, 1.046, 1.077, 1.115, 1.137, 1.220, 1.107, 1.171, 1.179, 1.128, 1.196, 1.222, 1.114, 1.146, 1.121, 1.115, 1.058, 1.084, 1.089, 1.085, 1.046, 1.035, 0.984, 0.999, 1.001, 0.909, 0.883, 0.831, 0.779, 0.723, 0.593, 0.632, 0.689, 0.878, 0.999, 1.076, 1.536, 0.916}; 
  double CpHT_NonElecs_EndcapB_TW[44] = { 1.085, 0.980, 1.051, 1.017, 1.050, 1.047, 1.126, 1.091, 1.087, 1.071, 1.187, 0.967, 1.098, 1.087, 1.041, 1.101, 1.065, 1.092, 1.178, 1.060, 1.090, 1.115, 1.125, 1.078, 1.058, 0.932, 1.031, 0.849, 1.066, 1.024, 0.974, 0.869, 1.039, 0.894, 0.796, 0.821, 0.665, 0.672, 0.813, 0.991, 1.074, 1.184, 1.281, 1.000}; 
*/

  // --------------------------------------------------------------


// Same corrections for all gases:
 
  for (int j = 0 ; j < N_GAS; j++){
   m_CpHT_B_Zee_SL_new     [j][0].update ( N_SL_B  ,-0.5 ,72.5 , std::vector<float> (CpHT_Zee_Barrel_SL , CpHT_Zee_Barrel_SL  + sizeof CpHT_Zee_Barrel_SL / sizeof CpHT_Zee_Barrel_SL[0])  ); 
   m_CpHT_B_Zee_SL_new     [j][1].update ( N_SL_EA ,-0.5 ,95.5 , std::vector<float> (CpHT_Zee_EndcapA_SL, CpHT_Zee_EndcapA_SL + sizeof CpHT_Zee_EndcapA_SL/ sizeof CpHT_Zee_EndcapA_SL[0]) ); 
   m_CpHT_B_Zee_SL_new     [j][2].update ( N_SL_EB ,-0.5 ,63.5 , std::vector<float> (CpHT_Zee_EndcapB_SL, CpHT_Zee_EndcapB_SL + sizeof CpHT_Zee_EndcapB_SL/ sizeof CpHT_Zee_EndcapB_SL[0]) ); 

   m_CpHT_B_Zmm_SL_new     [j][0].update ( N_SL_B  ,-0.5 ,72.5 , std::vector<float> (CpHT_Zmm_Barrel_SL , CpHT_Zmm_Barrel_SL  + sizeof CpHT_Zmm_Barrel_SL / sizeof CpHT_Zmm_Barrel_SL[0])  ); 
   m_CpHT_B_Zmm_SL_new     [j][1].update ( N_SL_EA ,-0.5 ,95.5 , std::vector<float> (CpHT_Zmm_EndcapA_SL, CpHT_Zmm_EndcapA_SL + sizeof CpHT_Zmm_EndcapA_SL/ sizeof CpHT_Zmm_EndcapA_SL[0]) ); 
   m_CpHT_B_Zmm_SL_new     [j][2].update ( N_SL_EB ,-0.5 ,63.5 , std::vector<float> (CpHT_Zmm_EndcapB_SL, CpHT_Zmm_EndcapB_SL + sizeof CpHT_Zmm_EndcapB_SL/ sizeof CpHT_Zmm_EndcapB_SL[0]) ); 

   m_CpHT_B_Zee_ZR_new     [j][0].update ( N_ZR_B  , 0.0  , 720.0  , std::vector<float> (CpHT_Zee_Barrel_ZR , CpHT_Zee_Barrel_ZR  + sizeof CpHT_Zee_Barrel_ZR / sizeof CpHT_Zee_Barrel_ZR[0])  ); 
   m_CpHT_B_Zee_ZR_new     [j][1].update ( N_ZR_EA , 630.0, 1030.0 , std::vector<float> (CpHT_Zee_EndcapA_ZR, CpHT_Zee_EndcapA_ZR + sizeof CpHT_Zee_EndcapA_ZR/ sizeof CpHT_Zee_EndcapA_ZR[0]) ); 
   m_CpHT_B_Zee_ZR_new     [j][2].update ( N_ZR_EB , 630.0, 1030.0 , std::vector<float> (CpHT_Zee_EndcapB_ZR, CpHT_Zee_EndcapB_ZR + sizeof CpHT_Zee_EndcapB_ZR/ sizeof CpHT_Zee_EndcapB_ZR[0]) ); 


   m_CpHT_B_Zmm_ZR_new     [j][0].update ( N_ZR_B  , 0.0  , 720.0  , std::vector<float> (CpHT_Zmm_Barrel_ZR , CpHT_Zmm_Barrel_ZR  + sizeof CpHT_Zmm_Barrel_ZR / sizeof CpHT_Zmm_Barrel_ZR[0])  ); 
   m_CpHT_B_Zmm_ZR_new     [j][1].update ( N_ZR_EA , 630.0, 1030.0 , std::vector<float> (CpHT_Zmm_EndcapA_ZR, CpHT_Zmm_EndcapA_ZR + sizeof CpHT_Zmm_EndcapA_ZR/ sizeof CpHT_Zmm_EndcapA_ZR[0]) ); 
   m_CpHT_B_Zmm_ZR_new     [j][2].update ( N_ZR_EB , 630.0, 1030.0 , std::vector<float> (CpHT_Zmm_EndcapB_ZR, CpHT_Zmm_EndcapB_ZR + sizeof CpHT_Zmm_EndcapB_ZR/ sizeof CpHT_Zmm_EndcapB_ZR[0]) ); 

   m_CpHT_B_Zee_TW_new     [j][0].update ( N_TW_B  , 0.0, 2.2, std::vector<float> (CpHT_Zee_Barrel_TW , CpHT_Zee_Barrel_TW  + sizeof CpHT_Zee_Barrel_TW / sizeof CpHT_Zee_Barrel_TW[0])  ); 
   m_CpHT_B_Zee_TW_new     [j][1].update ( N_TW_EA , 0.0, 2.2, std::vector<float> (CpHT_Zee_EndcapA_TW, CpHT_Zee_EndcapA_TW + sizeof CpHT_Zee_EndcapA_TW/ sizeof CpHT_Zee_EndcapA_TW[0]) ); 
   m_CpHT_B_Zee_TW_new     [j][2].update ( N_TW_EB , 0.0, 2.2, std::vector<float> (CpHT_Zee_EndcapB_TW, CpHT_Zee_EndcapB_TW + sizeof CpHT_Zee_EndcapB_TW/ sizeof CpHT_Zee_EndcapB_TW[0]) ); 

   m_CpHT_B_Zmm_TW_new     [j][0].update ( N_TW_B  , 0.0, 2.2, std::vector<float> (CpHT_Zmm_Barrel_TW , CpHT_Zmm_Barrel_TW  + sizeof CpHT_Zmm_Barrel_TW / sizeof CpHT_Zmm_Barrel_TW[0])  ); 
   m_CpHT_B_Zmm_TW_new     [j][1].update ( N_TW_EA , 0.0, 2.2, std::vector<float> (CpHT_Zmm_EndcapA_TW, CpHT_Zmm_EndcapA_TW + sizeof CpHT_Zmm_EndcapA_TW/ sizeof CpHT_Zmm_EndcapA_TW[0]) ); 
   m_CpHT_B_Zmm_TW_new     [j][2].update ( N_TW_EB , 0.0, 2.2, std::vector<float> (CpHT_Zmm_EndcapB_TW, CpHT_Zmm_EndcapB_TW + sizeof CpHT_Zmm_EndcapB_TW/ sizeof CpHT_Zmm_EndcapB_TW[0]) ); 
  }
 

  m_datainplace = true;

}

#endif
#endif
