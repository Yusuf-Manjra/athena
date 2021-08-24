/*
    Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#include "CaloEvent/CaloCellContainer.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/CaloCell_SuperCell_ID.h"

#include "xAODTrigL1Calo/TriggerTowerContainer.h"

#include "L1CaloFEXSim/jTower.h"
#include "L1CaloFEXSim/jTowerBuilder.h"

#include "L1CaloFEXSim/jTowerContainer.h"

#include "TROOT.h"
#include "TH1.h"
#include "TH1F.h"
#include "TPad.h"
#include "TCanvas.h"

// TOWER IS A COLLECTION OF SUPER CELLS
// IT SHOULD HAVE A UNIQUE ID
// IT SHOULD BE ABLE TO RETURN LIST OF SUPER CELLS BELONGING TO IT

// THIS IS A CLASS DESIGNED TO BUILD AN JTOWER USING THE JTOWER CLASS AND THEN PRINT THE RELEVANT INFORMATION TO THE SCREEN USING FUNCTION CALLS FROM THE JTOWER CLASS

namespace LVL1 {

jTowerBuilder::jTowerBuilder(const std::string& type,const std::string& name,const IInterface* parent):
    AthAlgTool(type,name,parent)
{
  declareInterface<IjTowerBuilder>(this);
}


void jTowerBuilder::init(std::unique_ptr<jTowerContainer> & jTowerContainerRaw) 
{

  execute(jTowerContainerRaw);

}


void jTowerBuilder::reset() 
{

}


void jTowerBuilder::execute(std::unique_ptr<jTowerContainer> & jTowerContainerRaw) 
{
  BuildAllTowers(jTowerContainerRaw);
}
 
  // TOWER IDs FOR CLARITY
  // Left Barrel IETower = 100000 + X
  // Right Barrel IETower = 200000 + X
  // Left Transition ID Tower = 300000 + X;
  // Right Transition ID Tower = 400000 + X;
  // Left Endcap ID Tower = 500000 + X
  // Right Endcap ID Tower = 600000 + X
  // Left Hadronic Endcap ID Tower = 11100000 + X --> These are just Layer 5 of Endcap Towers.  They will never be generated as standalone jTowers.
  // Right Haronic Endcap ID Tower = 22200000 + X --> These are just Layer 5 of Endcap Towers.  They will never be generated as standalone jTowers.

 void jTowerBuilder::BuildEMBjTowers(std::unique_ptr<jTowerContainer> & jTowerContainerRaw) const 
{
  static constexpr float TT_Size_eta = 0.1;
  static constexpr float TT_Size_phi = M_PI/32;
  // Regions 0 only.  Region 1 is 'transition region'.
  for (int ieta = 0; ieta < 14; ++ieta) { // loop over 14 eta steps (ignoring last step as it is transition region)
    float centre_eta = (TT_Size_eta*ieta) + (0.5*TT_Size_eta);
    for (int iphi = 0; iphi < 64; ++iphi){ // loop over 64 phi steps
      int key_eta = ieta;
      float centre_phi = (TT_Size_phi*iphi) + (0.5*TT_Size_phi);
      BuildSingleTower(jTowerContainerRaw, ieta, iphi, key_eta, 100000, -1, -1*centre_eta, centre_phi);
      BuildSingleTower(jTowerContainerRaw, ieta, iphi, key_eta, 200000, 1, centre_eta, centre_phi);
    }
  }

}

void jTowerBuilder::BuildTRANSjTowers(std::unique_ptr<jTowerContainer> & jTowerContainerRaw) const
{
  static constexpr float TT_Size = M_PI/32;
  int TRANS_MODIFIER = 14;
  int tmpVal = TRANS_MODIFIER;

  for (int ieta = tmpVal; ieta < tmpVal + 1; ieta++){ // loop over eta steps
    float centre_eta = (0.1*ieta) + (0.05);
    for (int iphi = 0; iphi < 64; ++iphi){ // loop over 64 phi steps
      int key_eta = ieta;
      float centre_phi = (TT_Size*iphi) + (0.5*TT_Size);
      BuildSingleTower(jTowerContainerRaw, ieta, iphi, key_eta, 300000, -1,-1*centre_eta, centre_phi);
      BuildSingleTower(jTowerContainerRaw, ieta, iphi, key_eta, 400000, 1,centre_eta, centre_phi);
    }
  }

}

void jTowerBuilder::BuildEMEjTowers(std::unique_ptr<jTowerContainer> & jTowerContainerRaw) const
{
  static constexpr float TT_Size = M_PI/32;
  // Region 1
  int EME_MODIFIER = 15;
  int tmpVal = EME_MODIFIER;

  for (int ieta = tmpVal; ieta < tmpVal + 3; ++ieta){ // loop over eta steps
    float centre_eta =(0.1*ieta) + (0.05) ;
    for (int iphi = 0; iphi < 64; ++iphi){ // loop over 64 phi steps
      int key_eta = ieta;
      float centre_phi = (TT_Size*iphi) + (0.5*TT_Size);
      BuildSingleTower(jTowerContainerRaw, ieta, iphi, key_eta, 500000, -1, -1*centre_eta, centre_phi);
      BuildSingleTower(jTowerContainerRaw, ieta, iphi, key_eta, 600000, 1, centre_eta, centre_phi);
    }
    EME_MODIFIER++;
  }

  // Region 2
  tmpVal = EME_MODIFIER;
  for (int ieta = tmpVal; ieta < tmpVal + 2; ++ieta){ // loop over eta steps
    float centre_eta = (0.1*ieta) + (0.05);
    for (int iphi = 0; iphi < 64; ++iphi){ // loop over 64 phi steps
      int key_eta = ieta;
      float centre_phi =(TT_Size*iphi) + (0.5*TT_Size) ;
      BuildSingleTower(jTowerContainerRaw, ieta, iphi, key_eta, 500000, -1,-1*centre_eta, centre_phi);
      BuildSingleTower(jTowerContainerRaw, ieta, iphi, key_eta, 600000, 1,centre_eta, centre_phi);
    }
    EME_MODIFIER++;
  }

  // Region 3
  tmpVal = EME_MODIFIER;
  for (int ieta = tmpVal; ieta < tmpVal + 4; ++ieta){ // loop over eta steps
    float centre_eta= (0.1*ieta) + (0.05) ;
    for (int iphi = 0; iphi < 64; ++iphi){ // loop over 64 phi steps
      int key_eta = ieta;
      float centre_phi =(TT_Size*iphi) + (0.5*TT_Size) ;
      BuildSingleTower(jTowerContainerRaw, ieta, iphi, key_eta, 500000, -1,-1*centre_eta, centre_phi);
      BuildSingleTower(jTowerContainerRaw, ieta, iphi, key_eta, 600000, 1,centre_eta, centre_phi); 
    }
    EME_MODIFIER++;
  }

  // Region 4
  tmpVal = EME_MODIFIER;
  for (int ieta = tmpVal; ieta < tmpVal + 1; ++ieta){ // loop over eta steps
    float centre_eta = (0.1*ieta) + (0.05);
    for (int iphi = 0; iphi < 64; ++iphi){ // loop over 64 phi steps
      int key_eta = ieta;
      float centre_phi =(TT_Size*iphi) + (0.5*TT_Size) ;
      BuildSingleTower(jTowerContainerRaw, ieta, iphi, key_eta, 500000, -1, -1*centre_eta, centre_phi);
      BuildSingleTower(jTowerContainerRaw, ieta, iphi, key_eta, 600000, 1, centre_eta, centre_phi);
    }
    EME_MODIFIER++;
  }


}

  // EMIE = Electromagnetic Inner ECAL - i.e. the forward ECAL region at high eta
  void jTowerBuilder::BuildEMIEjTowers(std::unique_ptr<jTowerContainer> & jTowerContainerRaw) const
  {
    static constexpr float TT_Size = M_PI/32;
    int EMIE_MODIFIER = 25;
    int tmpVal = EMIE_MODIFIER;
    int cellCountEta = 0;

    for (int ieta = tmpVal; ieta < tmpVal + 3; ++ieta){ // loop over eta steps (there are 3 here, 2.5-2.7, 2.7-2.9, 2.9-3.1)
      cellCountEta++;
      float centre_eta =(0.1*ieta) + (0.1*cellCountEta) ;
      for (int iphi = 0; iphi < 32; ++iphi){ // loop over 32 phi steps
	     int key_eta = ieta;
         float centre_phi = (2*TT_Size*iphi) + (2*0.5*TT_Size);
	     BuildSingleTower(jTowerContainerRaw, ieta, iphi, key_eta, /*7*/500000, -1, -1*centre_eta, centre_phi);
	     BuildSingleTower(jTowerContainerRaw, ieta, iphi, key_eta, /*8*/600000, 1,centre_eta, centre_phi);
      }
      EMIE_MODIFIER++;
    }
    
    tmpVal = EMIE_MODIFIER; 
    for (int ieta = tmpVal; ieta < tmpVal + 1; ++ieta){ // loop over eta steps (there are 1 here, 3.1-3.2)
      float centre_eta = (0.1*ieta + 0.3) + (0.05);
      for (int iphi = 0; iphi < 32; ++iphi){ // loop over 32 phi steps
         int key_eta = ieta;
         float centre_phi = (2*TT_Size*iphi) + (2*0.5*TT_Size);
         BuildSingleTower(jTowerContainerRaw, ieta, iphi, key_eta, /*7*/500000, -1, -1*centre_eta, centre_phi);
         BuildSingleTower(jTowerContainerRaw, ieta, iphi, key_eta, /*8*/600000, 1,centre_eta, centre_phi);
      }
      EMIE_MODIFIER++;
    }

  }

  void jTowerBuilder::BuildFCALjTowers(std::unique_ptr<jTowerContainer> & jTowerContainerRaw) const
  {
    static constexpr float TT_Size = M_PI/32;
    int FCAL_MODIFIER = 29; // there's 0.1 overlap with EMIE here in eta, but in counting we pretend it's the next one along.
    int tmpVal = FCAL_MODIFIER;

    //These jTowers split between all of the layers as well (FCAL0,1,2) but we treat them as though they are a single flat layer of 24 supercells and also pretend that they do not overlap - when they definitely do...
    //This means that from a tower numbering perspective we start with FCAL0 and work upwards (in numbers?), but real ordering in eta is different and this has to be built into the jTower internal properties
    //Right now we are unfortunately using hard-coded eta and phi positions to do this, but maybe these should be drawn from a database someday

    // THIS REGION DEFINES 1 jTOWER PER SUPERCELL! AS SUCH THE jTOWER AND SUPERCELL POSITIONS IN ETA-PHI SPACE WILL MATCH EXACTY
    // (That's good right? Supposed to make life easier?)

    // 21/01/21 IN THE MC:
    // FCAL 0 Region [NOT SPECIFIED IN MC] has 12 supercells in 3.2 < eta < 4.88, and 16 supercells in phi.  Supercells are 0.14 x 0.4. posneg +-2
    // FCAL 1 Region [NOT SPECIFIED IN MC] has 8 supercells in 3.2 < eta < 4.48, and 16 supercells in phi.  Supercells are 0.16 x 0.4. posneg +-2
    // FCAL 2 Region [NOT SPECIFIED IN MC] has 4 supercells in 3.2 < eta < 4.48, and 16 supercells in phi.  Supercells are 0.32 x 0.4. posneg +-2

    //FCAL0
    float eta_base = 3.2;
    float eta_width =1.4 * TT_Size; //0.14;
    float phi_base = 0.0;
    float phi_width = 4*TT_Size;//0.4;
    int cellCountEta = 0;
    int FCAL0_INITIAL = FCAL_MODIFIER;
    for (int ieta = tmpVal; ieta < tmpVal + 12; ++ieta){ // loop over eta steps (there are 12 here in varying positions for FCAL0)
      int key_eta = ieta - FCAL0_INITIAL;
      cellCountEta++;
      float centre_eta = eta_base + (cellCountEta * eta_width) - (eta_width);// * 0.5);
      for (int iphi = 0; iphi < 16; ++iphi){ // loop over 16 phi steps
     	float centre_phi = phi_base + ((iphi+1) * phi_width) - (phi_width * 0.5);
        BuildSingleTower(jTowerContainerRaw, ieta, iphi, key_eta, 700000, -1, -1*centre_eta, centre_phi, 0); 
        BuildSingleTower(jTowerContainerRaw, ieta, iphi, key_eta, 800000, 1, centre_eta, centre_phi, 0);
      }
      FCAL_MODIFIER++;
    }
    
    //FCAL1
    eta_width = 1.6 * TT_Size;//0.16;
    cellCountEta = 0;
    tmpVal = FCAL_MODIFIER;
    int FCAL1_INITIAL = FCAL_MODIFIER;
    for (int ieta = tmpVal; ieta < tmpVal + 8; ++ieta){ // loop over eta steps (there are 8 here in varying positions for FCAL1)
      int key_eta = ieta - FCAL1_INITIAL;
      cellCountEta++;
      float centre_eta = eta_base + (cellCountEta * eta_width) - (eta_width * 0.5);
      for (int iphi = 0; iphi < 16; ++iphi){ // loop over 16 phi steps
	    float centre_phi = phi_base + ((iphi+1) * phi_width) - (phi_width * 0.5);;
	    BuildSingleTower(jTowerContainerRaw, ieta, iphi, key_eta, 900000, -1,-1*centre_eta, centre_phi, 1);
	    BuildSingleTower(jTowerContainerRaw, ieta, iphi, key_eta, 1000000, 1, centre_eta, centre_phi, 1);
      }
      FCAL_MODIFIER++;
    }
  
    
    //FCAL2
    eta_width = 3.2 * TT_Size; //0.32;
    cellCountEta = 0;
    tmpVal = FCAL_MODIFIER;
    int FCAL2_INITIAL = FCAL_MODIFIER;
    for (int ieta = tmpVal; ieta < tmpVal + 4; ++ieta){ // loop over eta steps (there are 4 here in varying positions for FCAL2)
      int key_eta = ieta - FCAL2_INITIAL;
      cellCountEta++;
      float centre_eta = eta_base + (cellCountEta * eta_width) - (eta_width * 0.5);
      for (int iphi = 0; iphi < 16; ++iphi){ // loop over 16 phi steps
	    float centre_phi = phi_base + ((iphi+1) * phi_width) - (phi_width * 0.5);
	    BuildSingleTower(jTowerContainerRaw, ieta, iphi, key_eta, 1100000, -1, -1*centre_eta, centre_phi, 2);
	    BuildSingleTower(jTowerContainerRaw, ieta, iphi, key_eta, 1200000, 1, centre_eta, centre_phi, 2);
      }
      FCAL_MODIFIER++;
    }
    

  }



  void jTowerBuilder::BuildHECjTowers(std::unique_ptr<jTowerContainer> & jTowerContainerRaw) const
{
  // Region 0
  int HEC_MODIFIER = 29;
  int tmpVal = HEC_MODIFIER;
  for (int ieta = tmpVal; ieta < tmpVal + 10; ++ieta){ // loop over eta steps
    for (int iphi = 0; iphi < 64; ++iphi){ // loop over 64 phi steps
      int key_eta = ieta;
      BuildSingleTower(jTowerContainerRaw, ieta, iphi, key_eta, 11100000, -1, ieta, iphi);
      BuildSingleTower(jTowerContainerRaw, ieta, iphi, key_eta, 22200000, 1, ieta, iphi);
    }
    HEC_MODIFIER++;
  }

  // Region 1
  tmpVal = HEC_MODIFIER;
  for (int ieta = tmpVal; ieta < tmpVal + 4; ++ieta){ // loop over eta steps
    for (int iphi = 0; iphi < 32; ++iphi){ // loop over 64 phi steps
      int key_eta = ieta;
      BuildSingleTower(jTowerContainerRaw, ieta, iphi, key_eta, 11100000, -1, ieta, iphi);
      BuildSingleTower(jTowerContainerRaw, ieta, iphi, key_eta, 22200000, 1, ieta, iphi);
    }
    HEC_MODIFIER++;
  }

}
//=================================================================================================================================================================

  void jTowerBuilder::BuildSingleTower(std::unique_ptr<jTowerContainer> & jTowerContainerRaw,float eta, float phi, int key_eta, float keybase, int posneg, float centre_eta, float centre_phi, int fcal_layer)  const
{

  if(centre_eta == 0.0){
    centre_eta = eta + 0.05;
  }
  if(centre_phi == 0.0){
    centre_phi = phi + 0.05;
  }
   
  jTowerContainerRaw->push_back(eta, phi, key_eta, keybase, posneg, centre_eta, centre_phi, fcal_layer);

}

void jTowerBuilder::BuildAllTowers(std::unique_ptr<jTowerContainer> & jTowerContainerRaw) const
{
//  BuildSingleTower(jTowerContainerRaw, 0.0, 0.0, 0, -1.0, 0, 0.0, 0.0, 0);
  BuildEMBjTowers(jTowerContainerRaw);
  BuildTRANSjTowers(jTowerContainerRaw);
  BuildEMEjTowers(jTowerContainerRaw);
  BuildEMIEjTowers(jTowerContainerRaw);
  BuildFCALjTowers(jTowerContainerRaw);

}

} // end of LVL1 namespace

