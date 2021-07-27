/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


// System include(s):
#include <stdexcept>

// xAOD include(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODTrigger/jFexSRJetRoI.h"

namespace xAOD {

  const float jFexSRJetRoI_v1::s_tobEtScale = 200.;
  const float jFexSRJetRoI_v1::s_towerEtaWidth = 0.1;
  const float jFexSRJetRoI_v1::s_towerPhiWidth = 0.1; 
  const float jFexSRJetRoI_v1::s_minEta = -4.9;   


  // globalEta/Phi calculation in the FCAL varies depending on position in eta space due to TT granularity change.
  //| Region          |      eta region     | TT (eta x phi)
  //---------------------------------------------------------
  // Region 1  EMB    |      |eta| <  25    | (1 x 1)
  // Region 2  EMIE   | 25 < |eta|< 31      | (2 x 2)
  // Region 3  TRANS  | 31 < |eta| < 32     | (1 x 2)
  // Region 4  FCAL   |      |eta| > 32     | (2 x 4)                 
 
  //eta position in array for jfex module 0
  const std::vector<int> jFexSRJetRoI_v1::s_EtaPosition_C_Side = {28, 36,   //Region 1 
                                                                  25, 27,   //Region 2
                                                                      24,   //Region 3
                                                                  13, 23};  //Region 4
  //eta position in array for jfex module 5
  const std::vector<int> jFexSRJetRoI_v1::s_EtaPosition_A_Side = { 8, 16,   //Region 1 
                                                                  17, 19,   //Region 2
                                                                      20,   //Region 3
                                                                  21, 31};  //Region 4


  jFexSRJetRoI_v1::jFexSRJetRoI_v1()
      : SG::AuxElement() {
   }
   void jFexSRJetRoI_v1::initialize( uint8_t jFexNumber, uint8_t fpgaNumber, uint32_t tobWord) {
 
     setTobWord( tobWord );
     setjFexNumber( jFexNumber );
     setfpgaNumber( fpgaNumber);
     setTobEt(unpackEtTOB());
     setTobLocalEta( unpackEtaIndex() );
     setTobLocalPhi( unpackPhiIndex() );      
     setTobSat(unpackSaturationIndex());
     setGlobalEta(unpackGlobalEta());
     setGlobalPhi(unpackGlobalPhi()); 
     setEta( (unpackGlobalEta()+0.5)/10 ); 
     setPhi( (unpackGlobalPhi()+0.5)/10 );     
   //include in future when xTOB in jFEX has been implemented.

   // If the object is a TOB then the isTOB should be true.
   // For xTOB default is false, but should be set if a matching TOB is found 
   // if (type() == TOB) setIsTOB(1);
   // else               setIsTOB(0);

      return;
   }

   //----------------
   /// Raw data words
   //----------------

   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( jFexSRJetRoI_v1, uint32_t, tobWord     , setTobWord    )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( jFexSRJetRoI_v1, uint8_t , jFexNumber  , setjFexNumber )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( jFexSRJetRoI_v1, uint8_t , fpgaNumber  , setfpgaNumber )
   
 
   /// Extracted from data words, stored for convenience
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( jFexSRJetRoI_v1, uint16_t, tobEt       , setTobEt    )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( jFexSRJetRoI_v1, uint8_t , tobLocalEta , setTobLocalEta )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( jFexSRJetRoI_v1, uint8_t , tobLocalPhi , setTobLocalPhi )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( jFexSRJetRoI_v1, uint8_t , tobSat      , setTobSat   )

   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( jFexSRJetRoI_v1, int , globalEta, setGlobalEta )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( jFexSRJetRoI_v1, uint, globalPhi, setGlobalPhi )
   
  ///global coordinates, stored for furture use but not sent to L1Topo    
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( jFexSRJetRoI_v1, float, eta, setEta)
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( jFexSRJetRoI_v1, float, phi, setPhi)
     
  
  
  
   //-----------------
   /// Methods to decode data from the TOB/RoI and return to the user
   //-----------------

  //include in future when xTOB in jFEX has been implemented.
   
   /// TOB or xTOB?
   //jFexSRJetRoI_v1::ObjectType jFexSRJetRoI_v1::type() const {
   //if (Word1() == 0) return TOB;
   //else              return xTOB;
   //}

   //Hardware coordinate elements  

   //Raw ET on TOB scale (200 MeV/count)

    unsigned int jFexSRJetRoI_v1::unpackEtTOB() const{
     //Data content = TOB
     return (tobWord() >> s_etBit) & s_etMask;
   } 


   //Return an eta index
   unsigned int jFexSRJetRoI::unpackEtaIndex() const {
     return (tobWord() >> s_etaBit) & s_etaMask;
   }
   //Return a phi index
   unsigned int jFexSRJetRoI::unpackPhiIndex() const {
     return (tobWord() >> s_phiBit) & s_phiMask;
   }

   //Return sat flag
   unsigned int jFexSRJetRoI::unpackSaturationIndex() const{
     return (tobWord() >> s_satBit) & s_satMask;
   }

   /// Methods that require combining results or applying scales

   /// ET on TOB scale
   unsigned int jFexSRJetRoI_v1::et() const {
       // Returns the TOB Et in a 1 MeV scale
       return tobEt()*s_tobEtScale; 
   }

  /// As the Trigger towers are 1x1 in Eta - Phi coords (x10)
  int jFexSRJetRoI_v1::unpackGlobalEta() const{
      
    int globalEta = 0;
    
    if(jFexNumber()==0){
      //Region 2 
      if((s_EtaPosition_C_Side[2] <= tobLocalEta()) && (tobLocalEta() <=s_EtaPosition_C_Side[3])){
	globalEta = 25 + (2*(tobLocalEta() -16));
      }
      //Region 3
      if(s_EtaPosition_C_Side[5] == tobLocalEta()){
	globalEta = 32;
      }
      //Region 4
      if((s_EtaPosition_C_Side[6] <= tobLocalEta()) && (tobLocalEta() <=s_EtaPosition_C_Side[7])){
	globalEta = 32 + (tobLocalEta()-20);
      }
    }

    if(jFexNumber()==5){
      //Region 2
      if((s_EtaPosition_A_Side[2] <= tobLocalEta()) && (tobLocalEta() <=s_EtaPosition_A_Side[3])){
        globalEta = -25 - (2*(tobLocalEta() -16));
      }
      //Region 3
      if(s_EtaPosition_A_Side[5] == tobLocalEta()){
        globalEta = -32;
      }
      //Region 4
      if((s_EtaPosition_A_Side[6] <= tobLocalEta()) && (tobLocalEta() <=s_EtaPosition_A_Side[7])){
        globalEta = -32 - (tobLocalEta()-20);
      }
    }
    //Region 1
    else{
        globalEta = tobLocalEta()+(8*(jFexNumber() - 3)) ;  // for module 1 to 4 
    }
    return globalEta;
  }

  uint jFexSRJetRoI_v1::unpackGlobalPhi() const{
    uint globalPhi = 0;
    //16 is the phi height of an FPGA
    if(jFexNumber()==0 || jFexNumber()==5){
      //Region 2
      if((s_EtaPosition_A_Side[2] <= tobLocalEta()) && (tobLocalEta() <=s_EtaPosition_A_Side[3])){
        globalPhi = 16*fpgaNumber() + (2*(tobLocalPhi()-4));
      }
      //Region 3
      if(s_EtaPosition_A_Side[5] == tobLocalEta()){
        globalPhi = 16*fpgaNumber() + (2*(tobLocalPhi()-4));
      }
      //Region 4
      if((s_EtaPosition_A_Side[6] <= tobLocalEta()) && (tobLocalEta() <=s_EtaPosition_A_Side[7])){
        globalPhi = (16*fpgaNumber()) + (4*(tobLocalPhi() -2));
      }
    }
    //Region 1
    else{  globalPhi = tobLocalPhi() + (fpgaNumber() * 16);}
    return globalPhi; 
  
  }
} // namespace xAOD

