/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include <algorithm>
#include "DecisionHandling/HLTIdentifier.h"
#include "DecisionHandling/Combinators.h"
#include "AthenaMonitoringKernel/Monitored.h"

#include "TrigEgammaPrecisionElectronHypoToolInc.h"


using namespace TrigCompositeUtils;

TrigEgammaPrecisionElectronHypoToolInc::TrigEgammaPrecisionElectronHypoToolInc( const std::string& type, 
		    const std::string& name, 
		    const IInterface* parent ) 
  : base_class( type, name, parent ),
    m_decisionId( HLT::Identifier::fromToolName( name ) ) {
        declareProperty("ElectronLHSelector"        ,m_egammaElectronCutIDTool   );
    }

StatusCode TrigEgammaPrecisionElectronHypoToolInc::initialize()  {
  ATH_MSG_DEBUG( "Initialization completed successfully"   );    
  ATH_MSG_DEBUG( "EtaBins        = " << m_etabin      );
  ATH_MSG_DEBUG( "ETthr          = " << m_eTthr    );
  ATH_MSG_DEBUG( "dPHICLUSTERthr = " << m_dphicluster );
  ATH_MSG_DEBUG( "dETACLUSTERthr = " << m_detacluster );
  
   if ( m_etabin.empty() ) {
    ATH_MSG_ERROR(  " There are no cuts set (EtaBins property is an empty list)" );
    return StatusCode::FAILURE;
  }

  // Now we try to retrieve the ElectronPhotonSelectorTools that we will use to apply the electron Identification. This is a *must*
  ATH_MSG_DEBUG( "Retrieving egammaElectronCutIDTool..."  );
  CHECK( m_egammaElectronCutIDTool.retrieve() );

  unsigned int nEtaBin = m_etabin.size();
  ATH_CHECK( m_eTthr.size() == nEtaBin-1 );

  ATH_MSG_DEBUG( "Tool configured for chain/id: " << m_decisionId );

  if ( not m_monTool.name().empty() ) 
    CHECK( m_monTool.retrieve() );

  return StatusCode::SUCCESS;
}



TrigEgammaPrecisionElectronHypoToolInc::~TrigEgammaPrecisionElectronHypoToolInc(){}


bool TrigEgammaPrecisionElectronHypoToolInc::decide( const ITrigEgammaPrecisionElectronHypoTool::ElectronInfo& input ) const {

  bool pass = false;

  auto ET           = Monitored::Scalar( "Et_em"   , -1.0 );
  auto dEta         = Monitored::Scalar( "dEta", -1. ); 
  auto dPhi         = Monitored::Scalar( "dPhi", -1. );
  auto etaBin       = Monitored::Scalar( "EtaBin", -1. );
  auto monEta       = Monitored::Scalar( "Eta", -99. ); 
  auto monPhi       = Monitored::Scalar( "Phi", -99. );
  auto PassedCuts   = Monitored::Scalar<int>( "CutCounter", -1 );  
  auto monitorIt    = Monitored::Group( m_monTool, 
					       dEta, dPhi, 
                                               etaBin, monEta,
					       monPhi,PassedCuts );
 // when leaving scope it will ship data to monTool
  PassedCuts = PassedCuts + 1; //got called (data in place)

  auto roiDescriptor = input.roi;


  if ( fabs( roiDescriptor->eta() ) > 2.6 ) {
      ATH_MSG_DEBUG( "REJECT The electron had eta coordinates beyond the EM fiducial volume : " << roiDescriptor->eta() << "; stop the chain now" );       
      return false;
  } 

  ATH_MSG_DEBUG( "; RoI ID = " << roiDescriptor->roiId()
  		 << ": Eta = " << roiDescriptor->eta()
  		 << ", Phi = " << roiDescriptor->phi() );

  // fill local variables for RoI reference position
  double etaRef = roiDescriptor->eta();
  double phiRef = roiDescriptor->phi();
  // correct phi the to right range ( probably not needed anymore )   
  if ( fabs( phiRef ) > M_PI ) phiRef -= 2*M_PI; // correct phi if outside range


  auto pClus = input.electron->caloCluster();
  
  float absEta = fabs( pClus->eta() );
  const int cutIndex = findCutIndex( absEta );
  

  
  dEta =  pClus->eta() - etaRef;
  //  Deal with angle diferences greater than Pi
  dPhi =  fabs( pClus->phi() - phiRef );
  dPhi = ( dPhi < M_PI ? dPhi : 2*M_PI - dPhi ); // TB why only <
  ET  = pClus->et();
  // apply cuts: DeltaEta( clus-ROI )
  ATH_MSG_DEBUG( "Electron : eta="  << pClus->eta()
  		 << " roi eta=" << etaRef << " DeltaEta=" << dEta
  		 << " cut: <"   << m_detacluster          );
  
  if ( fabs( pClus->eta() - etaRef ) > m_detacluster ) {
    ATH_MSG_DEBUG("REJECT Electron a cut failed");
    return pass;
  }
  PassedCuts = PassedCuts + 1; //Deta
  
  // DeltaPhi( clus-ROI )
  ATH_MSG_DEBUG( ": phi="  << pClus->phi()
  		 << " roi phi="<< phiRef    << " DeltaPhi="<< dPhi
  		 << " cut: <"  << m_dphicluster );
  
  if( dPhi > m_dphicluster ) {
    ATH_MSG_DEBUG("REJECT Clsuter dPhi cut failed");
    return pass;
  }
  PassedCuts = PassedCuts + 1; //DPhi

  // eta range
  if ( cutIndex == -1 ) {  // VD
    ATH_MSG_DEBUG( "Electron : " << absEta << " outside eta range " << m_etabin[m_etabin.size()-1] );
    return pass;
  } else { 
    ATH_MSG_DEBUG( "eta bin used for cuts " << cutIndex );
  }
  PassedCuts = PassedCuts + 1; // passed eta cut
  
  // ET_em
  ATH_MSG_DEBUG( "Electron: ET_em=" << ET << " cut: >"  << m_eTthr[cutIndex] );
  if ( ET < m_eTthr[cutIndex] ) {
    ATH_MSG_DEBUG("REJECT et cut failed");
    return pass;
  }
  PassedCuts = PassedCuts + 1; // ET_em
  
 
// This is the last step. So pass is going to be the result of isEM
  asg::AcceptData accept =  m_egammaElectronCutIDTool->accept(input.electron); 
  pass = (bool) accept;

  std::bitset<32> isEMdecision = m_egammaElectronCutIDTool->accept(input.electron).getCutResultInvertedBitSet();
  ATH_MSG_DEBUG("isEM Result bitset: " << isEMdecision);


  float Rhad1(0), Rhad(0), Reta(0), Rphi(0), e277(0), weta2c(0), //emax2(0), 
    Eratio(0), DeltaE(0), f1(0), weta1c(0), wtot(0), fracm(0);

    
  // variables based on HCAL
  // transverse energy in 1st scintillator of hadronic calorimeter/ET
  input.electron->showerShapeValue(Rhad1, xAOD::EgammaParameters::Rhad1);
  // transverse energy in hadronic calorimeter/ET
  input.electron->showerShapeValue(Rhad, xAOD::EgammaParameters::Rhad);

  // variables based on S2 of EM CAL
  // E(7*7) in 2nd sampling
  input.electron->showerShapeValue(e277, xAOD::EgammaParameters::e277);
  // E(3*7)/E(7*7) in 2nd sampling
  input.electron->showerShapeValue(Reta, xAOD::EgammaParameters::Reta);
  // E(3*3)/E(3*7) in 2nd sampling
  input.electron->showerShapeValue(Rphi, xAOD::EgammaParameters::Rphi);
  // shower width in 2nd sampling
  input.electron->showerShapeValue(weta2c, xAOD::EgammaParameters::weta2);

  // variables based on S1 of EM CAL
  // fraction of energy reconstructed in the 1st sampling
  input.electron->showerShapeValue(f1, xAOD::EgammaParameters::f1);
  // shower width in 3 strips in 1st sampling
  input.electron->showerShapeValue(weta1c, xAOD::EgammaParameters::weta1);
  // E of 2nd max between max and min in strips [NOT USED]
  // eg->showerShapeValue(emax2, xAOD::EgammaParameters::e2tsts1);
  // (E of 1st max in strips-E of 2nd max)/(E of 1st max+E of 2nd max)
  input.electron->showerShapeValue(Eratio, xAOD::EgammaParameters::Eratio);
  // E(2nd max)-E(min) in strips
  input.electron->showerShapeValue(DeltaE, xAOD::EgammaParameters::DeltaE);
  // total shower width in 1st sampling
  input.electron->showerShapeValue(wtot, xAOD::EgammaParameters::wtots1);
  // E(+/-3)-E(+/-1)/E(+/-1)
  input.electron->showerShapeValue(fracm, xAOD::EgammaParameters::fracs1);

  ATH_MSG_DEBUG( "  Rhad1  " << Rhad1 ) ;
  ATH_MSG_DEBUG( "  Rhad   " << Rhad ) ;
  ATH_MSG_DEBUG( "  e277   " << e277 ) ;
  ATH_MSG_DEBUG( "  Reta   " << Reta ) ;
  ATH_MSG_DEBUG( "  Rphi   " << Rphi ) ;
  ATH_MSG_DEBUG( "  weta2c " << weta2c ) ;
  ATH_MSG_DEBUG( "  f1     " << f1 ) ;
  ATH_MSG_DEBUG( "  weta1c " << weta1c ) ;
  ATH_MSG_DEBUG( "  Eratio " << Eratio ) ;
  ATH_MSG_DEBUG( "  DeltaE " << DeltaE ) ;
  ATH_MSG_DEBUG( "  wtot   " << wtot ) ;
  ATH_MSG_DEBUG( "  fracm  " << fracm ) ;



  if ( !pass ){
      ATH_MSG_DEBUG("REJECT Likelihood failed");
  } else {
      ATH_MSG_DEBUG("ACCEPT Likelihood passed");
  }

  // Reach this point successfully  
  ATH_MSG_DEBUG( "pass = " << pass );

  return pass;
}

int TrigEgammaPrecisionElectronHypoToolInc::findCutIndex( float eta ) const {
  const float absEta = std::abs(eta);
  
  auto binIterator = std::adjacent_find( m_etabin.begin(), m_etabin.end(), [=](float left, float right){ return left < absEta and absEta < right; }  );
  if ( binIterator == m_etabin.end() ) {
    return -1;
  }
  return  binIterator - m_etabin.begin();
}


StatusCode TrigEgammaPrecisionElectronHypoToolInc::decide( std::vector<ElectronInfo>& input )  const {
  for ( auto& i: input ) {
    if ( passed ( m_decisionId.numeric(), i.previousDecisionIDs ) ) {
      if ( decide( i ) ) {
	addDecisionID( m_decisionId, i.decision );
      }
    }
  }
  return StatusCode::SUCCESS;
}
