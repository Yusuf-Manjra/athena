///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// CaloEnergyCnv_p2.cxx 
// Implementation file for class CaloEnergyCnv_p2
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 


// STL includes

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// muonEvent includes
#include "muonEvent/DepositInCalo.h"
#include "muonEvent/CaloEnergy.h"

// RecTPCnv includes
#include "RecTPCnv/DepositInCaloCnv_p2.h"
#include "RecTPCnv/CaloEnergyCnv_p2.h"
#include "TrkEventTPCnv/TrkMaterialOnTrack/EnergyLossCnv_p1.h"

// For converter.
#include "RecTPCnv/DepositInCalo_p1.h"
#include "RecTPCnv/DepositInCalo_p2.h"
#include "RootConversions/VectorConverter.h"
#include "RootConversions/TConverterRegistry.h"

// pre-allocate converters
static const DepositInCaloCnv_p2 depositCnv;
static const EnergyLossCnv_p1 energyLossCnv;

CaloEnergyCnv_p2::CaloEnergyCnv_p2()
{
}

void CaloEnergyCnv_p2::persToTrans( const CaloEnergy_p2* pers,
				    CaloEnergy* trans, 
				    MsgStream& msg ) const
{
//   msg << MSG::DEBUG << "Loading CaloEnergy from persistent state..."
//       << endmsg;

      /// energy loss in calorimeter
  energyLossCnv.persToTrans( &pers->m_energyLoss,
			     trans,
			     msg );

  trans->set_energyLossType   (static_cast<CaloEnergy::EnergyLossType>(pers->m_energyLossType));
  trans->set_caloLRLikelihood  (pers->m_caloLRLikelihood);
  trans->set_caloMuonIdTag     (pers->m_caloMuonIdTag);
  trans->set_fsrCandidateEnergy(pers->m_fsrCandidateEnergy);

  // reserve enough space so no re-alloc occurs
  std::vector<DepositInCalo> deposits (pers->m_deposits.size());

  size_t ideposit = 0;
  for (const DepositInCalo_p2& pers_dep : pers->m_deposits) {
    depositCnv.persToTrans( &pers_dep, &deposits[ideposit++], msg );
  }
  trans->set_deposits (std::move (deposits));

  trans->set_etCore (pers->m_etCore);

//   msg << MSG::DEBUG << "Loaded CaloEnergy from persistent state [OK]"
//       << endmsg;

  return;
}

void CaloEnergyCnv_p2::transToPers( const CaloEnergy* trans, 
				    CaloEnergy_p2* pers, 
				    MsgStream& msg ) const
{
//   msg << MSG::DEBUG << "Creating persistent state of CaloEnergy..."
//       << endmsg;

  energyLossCnv.transToPers ( trans, &pers->m_energyLoss,  msg );

  pers->m_energyLossType     = trans->energyLossType();
  pers->m_caloLRLikelihood   = trans->caloLRLikelihood();
  pers->m_caloMuonIdTag      = trans->caloMuonIdTag();
  pers->m_fsrCandidateEnergy = trans->fsrCandidateEnergy();
  
  // reserve enough space so no re-alloc occurs
  pers->m_deposits.resize( trans->depositInCalo().size() );

  size_t ipersistent = 0;
  for (const DepositInCalo& trans_dep : trans->depositInCalo()) {
    depositCnv.transToPers( &trans_dep, &pers->m_deposits[ipersistent++], msg );
  }

  pers->m_etCore = trans->etCore();

//   msg << MSG::DEBUG << "Created persistent state of CaloEnergy [OK]"
//       << endmsg;
  return;
}


/**
 * @brief Register a streamer converter for backwards compatibility
 *        for the vector<DepositInCalo_p1> -> vector<DepositInCalo_p2>
 *        change.
 */
void CaloEnergyCnv_p2::registerStreamerConverter()
{
  // m_deposits was changed from vector<DepositInCalo_p1> to
  // vector<DepositInCalo_p2>.  Install a converter so that we can read
  // the old data.
  TConverterRegistry::Instance()->AddStreamerConverter
    ("vector<DepositInCalo_p1>", "vector<DepositInCalo_p2>",
     new RootConversions::VectorConverter<DepositInCalo_p1,DepositInCalo_p2>
     ("DepositInCalo_p1"));
}


void CaloEnergyCnv_p2::persToTrans( const CaloEnergy_p2* persObj, 
                                    CaloEnergy* transObj, 
                                    MsgStream& msg )
{
  const CaloEnergyCnv_p2* cc = this;
  return cc->persToTrans (persObj, transObj, msg);
}


void CaloEnergyCnv_p2::transToPers( const CaloEnergy* transObj, 
                                    CaloEnergy_p2* persObj, 
                                    MsgStream& msg )
{
  const CaloEnergyCnv_p2* cc = this;
  return cc->transToPers (transObj, persObj, msg);
}


/**
 * @brief register a C function to be executed at library loading time
 *        this is b/c there is no more the ability run the above static method
 *        `CaloEnergyCnv_p2::registerStreamerConverter` from pyroot (persistent
 *        classes' dictionaries are generated by reflex with --dataonly)
 */
extern "C" {
void caloenergy_cnv_p2_register_streamer()
{
  CaloEnergyCnv_p2::registerStreamerConverter();
}

}
