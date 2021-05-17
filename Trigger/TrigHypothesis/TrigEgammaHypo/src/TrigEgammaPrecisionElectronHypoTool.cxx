/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include <algorithm>
#include "TrigCompositeUtils/HLTIdentifier.h"
#include "TrigCompositeUtils/Combinators.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "AthenaMonitoringKernel/Monitored.h"
#include "xAODEgamma/Electron.h"
#include "TrigEgammaPrecisionElectronHypoTool.h"

namespace TCU = TrigCompositeUtils;

TrigEgammaPrecisionElectronHypoTool::TrigEgammaPrecisionElectronHypoTool( const std::string& type, 
		    const std::string& name, 
		    const IInterface* parent ) 
  : base_class( type, name, parent ),
    m_decisionId( HLT::Identifier::fromToolName( name ) ) {
}


StatusCode TrigEgammaPrecisionElectronHypoTool::initialize()  
{
  ATH_MSG_DEBUG( "Initialization completed successfully"   );    
  ATH_MSG_DEBUG( "EtaBins        = " << m_etabin      );
  ATH_MSG_DEBUG( "ETthr          = " << m_eTthr    );
  ATH_MSG_DEBUG( "dPHICLUSTERthr = " << m_dphicluster );
  ATH_MSG_DEBUG( "dETACLUSTERthr = " << m_detacluster );
  
  if ( m_etabin.empty() ) {
    ATH_MSG_ERROR(  " There are no cuts set (EtaBins property is an empty list)" );
    return StatusCode::FAILURE;
  }

  unsigned int nEtaBin = m_etabin.size();
  ATH_CHECK( m_eTthr.size() == nEtaBin-1 );

  ATH_MSG_DEBUG( "Tool configured for chain/id: " << m_decisionId );

  if ( not m_monTool.name().empty() ) 
    CHECK( m_monTool.retrieve() );

  return StatusCode::SUCCESS;
}


bool TrigEgammaPrecisionElectronHypoTool::decide( const ITrigEgammaPrecisionElectronHypoTool::ElectronInfo& input) const {

  bool pass = false;

  auto ET           = Monitored::Scalar( "Et_em"   , -1.0 );
  auto dEta         = Monitored::Scalar( "dEta", -1. );
  auto dPhi         = Monitored::Scalar( "dPhi", -1. );
  auto etaBin       = Monitored::Scalar( "EtaBin", -1. );
  auto monEta       = Monitored::Scalar( "Eta", -99. );
  auto monPhi       = Monitored::Scalar( "Phi", -99. );
  auto PassedCuts   = Monitored::Scalar<int>( "CutCounter", -1 );
  auto mon_lhval    = Monitored::Scalar("LikelihoodRatio",   -99.);
  auto mon_mu       = Monitored::Scalar("mu",   -1.);
  auto mon_ptcone20 = Monitored::Scalar("ptcone20",   -99.);
  auto mon_relptcone20 = Monitored::Scalar("ptcone20",   -99.);
  auto monitorIt    = Monitored::Group( m_monTool, dEta, dPhi, 
                                        etaBin, monEta,
                                        monPhi,PassedCuts,mon_lhval,mon_mu, 
                                        mon_ptcone20, mon_relptcone20);

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
  ATH_MSG_DEBUG("etaRef: "<<etaRef);
  // correct phi the to right range ( probably not needed anymore )   
  if ( fabs( phiRef ) > M_PI ) phiRef -= 2*M_PI; // correct phi if outside range


  auto pClus = input.electron->caloCluster();
  
  float absEta = fabs( pClus->eta() );
  
  ATH_MSG_DEBUG("absEta: "<<absEta);

  const int cutIndex = findCutIndex( absEta );
  ATH_MSG_DEBUG("cutIndex: "<<cutIndex);  

  
  dEta =  pClus->eta() - etaRef;
  //  Deal with angle diferences greater than Pi
  dPhi =  fabs( pClus->phi() - phiRef );
  dPhi = ( dPhi < M_PI ? dPhi : 2*M_PI - dPhi ); // TB why only <
  ET  = pClus->et();

  // apply cuts: DeltaEta( clus-ROI )
  ATH_MSG_DEBUG( "Electron : eta="  << pClus->eta() 
                  << " roi eta=" << etaRef << " DeltaEta=" << dEta
                  << " cut: <"   << m_detacluster );

  
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
  
 
  // This is the last step. So pass is going to be the result of LH
  // get average luminosity information to calculate LH
  if(input.valueDecorator.count("avgmu")){
    mon_mu = input.valueDecorator.at("avgmu");
  }
  


  float Rhad1(0), Rhad(0), Reta(0), Rphi(0), e277(0), weta2c(0), //emax2(0), 
        Eratio(0), DeltaE(0), f1(0), weta1c(0), wtot(0), fracm(0);

  float ptcone20(999), ptcone30(999), ptcone40(999), etcone20(999), etcone30(999), 
        etcone40(999), topoetcone20(999), topoetcone30(999), topoetcone40(999), relptcone20(999);


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

  input.electron->isolationValue(ptcone20, xAOD::Iso::ptcone20);

  input.electron->isolationValue(ptcone30, xAOD::Iso::ptcone30);

  input.electron->isolationValue(ptcone40, xAOD::Iso::ptcone40);

  input.electron->isolationValue(etcone20, xAOD::Iso::etcone20);

  input.electron->isolationValue(etcone30, xAOD::Iso::etcone30);

  input.electron->isolationValue(etcone40, xAOD::Iso::etcone40);

  input.electron->isolationValue(topoetcone20, xAOD::Iso::topoetcone20);

  input.electron->isolationValue(topoetcone30, xAOD::Iso::topoetcone30);

  input.electron->isolationValue(topoetcone40, xAOD::Iso::topoetcone40);

  ATH_MSG_DEBUG(" electron Cluster Et "<<ET);
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
  ATH_MSG_DEBUG( " trackPT "<<input.electron->trackParticle()->pt());
  ATH_MSG_DEBUG( " d0      "<<input.electron->trackParticle()->d0());
  ATH_MSG_DEBUG( " z0      "<<input.electron->trackParticle()->z0());
  ATH_MSG_DEBUG( " ptcone20 " << ptcone20 ) ;
  ATH_MSG_DEBUG( " ptcone30 " << ptcone30 ) ;
  ATH_MSG_DEBUG( " ptcone40 " << ptcone40 ) ;
  ATH_MSG_DEBUG( " etcone20 " << etcone20 ) ;
  ATH_MSG_DEBUG( " etcone30 " << etcone30 ) ;
  ATH_MSG_DEBUG( " etcone40 " << etcone40 ) ;
  ATH_MSG_DEBUG( " topoetcone20 " << topoetcone20 ) ;
  ATH_MSG_DEBUG( " topoetcone30 " << topoetcone30 ) ;
  ATH_MSG_DEBUG( " topoetcone40 " << topoetcone40 ) ;
  // Monitor showershapes
  mon_ptcone20 = ptcone20;
  relptcone20 = ptcone20/input.electron->pt();
  ATH_MSG_DEBUG("relptcone20 = " <<relptcone20  );
  mon_relptcone20 = relptcone20;
  ATH_MSG_DEBUG("m_RelPtConeCut = " << m_RelPtConeCut );
  
  // Only for LH
  if( input.valueDecorator.count(m_pidName+"LHValue")){
    mon_lhval = input.valueDecorator.at(m_pidName+"LHValue");
  }


  // Should works for DNN and LH
  if( input.pidDecorator.count(m_pidName) )
  {
    pass = input.pidDecorator.at(m_pidName);
  }

  // Evaluating lh *after* retrieving variables for monitoing and debuging purposes
  ATH_MSG_DEBUG("AthenaLHSelectorTool: TAccept = " << pass);
  if ( !pass ){
      ATH_MSG_DEBUG("REJECT Likelihood failed");
      return pass;
  } else {
      ATH_MSG_DEBUG("ACCEPT Likelihood passed");
  }

  // Check if need to apply isolation
  // First check logic. if cut is very negative, then no isolation cut is defined
  // if m_RelPtConeCut <-100 then hypo is configured not to apply isolation
  if (m_RelPtConeCut < -100){
      ATH_MSG_DEBUG(" not applying isolation. Returning NOW");
      ATH_MSG_DEBUG("TAccept = " << pass);
      return pass;
  }
  // Then, It will pass if relptcone20 is less than cut:
  pass = (relptcone20 < m_RelPtConeCut);
  //
  // Reach this point successfully  
  ATH_MSG_DEBUG( "pass = " << pass );

  return pass;
}



int TrigEgammaPrecisionElectronHypoTool::findCutIndex( float eta ) const {
  const float absEta = std::abs(eta);
  auto binIterator = std::adjacent_find( m_etabin.begin(), m_etabin.end(), [=](float left, float right){ return left < absEta and absEta < right; }  );
  if ( binIterator == m_etabin.end() ) {
    return -1;
  }
  return  binIterator - m_etabin.begin();
}


StatusCode TrigEgammaPrecisionElectronHypoTool::decide( std::vector<ElectronInfo>& input)  const {
  for ( auto& i: input ) {
    if ( TCU::passed ( m_decisionId.numeric(), i.previousDecisionIDs ) ) {
      if ( decide( i ) ) {
        TCU::addDecisionID( m_decisionId, i.decision );
      }
    }
  }
  return StatusCode::SUCCESS;
}
