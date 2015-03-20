/*********************************
 * DeltaEtaPhiIncl1.cpp
 * Created by Joerg Stelzer on 11/16/12.
 * Modified by V Sorin 
 * Copyright (c) 2012 Joerg Stelzer. All rights reserved.
 *
 * @brief algorithm calculates the eta and  phi-distance between one lists and applies box cut criteria
 *
 * @param NumberLeading
**********************************/

#include <cmath>

#include "L1TopoAlgorithms/DeltaEtaPhiIncl1.h"
#include "L1TopoCommon/Exception.h"
#include "L1TopoInterfaces/Decision.h"

REGISTER_ALG_TCS(DeltaEtaPhiIncl1)

using namespace std;

namespace {
   unsigned int
   calcDeltaPhi(const TCS::GenericTOB* tob1, const TCS::GenericTOB* tob2) {
      double dphi = fabs( tob1->phiDouble() - tob2->phiDouble() );
      if(dphi>M_PI)
         dphi = 2*M_PI - dphi;
      
      return round( 10 * dphi );
   }

   unsigned int
   calcDeltaEta(const TCS::GenericTOB* tob1, const TCS::GenericTOB* tob2) {
      double deta = fabs( tob1->eta() - tob2->eta() );
      return deta;
   }
}


TCS::DeltaEtaPhiIncl1::DeltaEtaPhiIncl1(const std::string & name) : DecisionAlg(name)
{
   defineParameter("InputWidth", 3);
   defineParameter("MaxTob", 0);
   defineParameter("NumResultBits", 3);
   defineParameter("MinEt1",1);
   defineParameter("MinEt2",1);
   defineParameter("MinDeltaEta",  0, 0);
   defineParameter("MaxDeltaEta", 49, 0);
   defineParameter("MinDeltaPhi",  0, 0);
   defineParameter("MaxDeltaPhi", 63, 0);
   defineParameter("MinDeltaEta",  0, 1);
   defineParameter("MaxDeltaEta", 49, 1);
   defineParameter("MinDeltaPhi",  0, 1);
   defineParameter("MaxDeltaPhi", 63, 1);
   defineParameter("MinDeltaEta",  0, 2);
   defineParameter("MaxDeltaEta", 49, 2);
   defineParameter("MinDeltaPhi",  0, 2);
   defineParameter("MaxDeltaPhi", 63, 2);
   setNumberOutputBits(3);
}

TCS::DeltaEtaPhiIncl1::~DeltaEtaPhiIncl1(){}


TCS::StatusCode
TCS::DeltaEtaPhiIncl1::initialize() {

   if(parameter("MaxTob").value() > 0) {
    p_NumberLeading1 = parameter("MaxTob").value();
    p_NumberLeading2 = parameter("MaxTob").value();
   } else {
    p_NumberLeading1 = parameter("InputWidth").value();
    p_NumberLeading2 = parameter("InputWidth").value();
   }

   for(int i=0; i<3; ++i) {
      p_DeltaPhiMin[i] = parameter("MinDeltaPhi", i).value();
      p_DeltaPhiMax[i] = parameter("MaxDeltaPhi", i).value();
      p_DeltaEtaMin[i] = parameter("MinDeltaEta", i).value();
      p_DeltaEtaMax[i] = parameter("MaxDeltaEta", i).value();

   }
   p_MinET1 = parameter("MinEt1").value();
   p_MinET2 = parameter("MinEt2").value();

   TRG_MSG_INFO("NumberLeading1 : " << p_NumberLeading1);  // note that the reading of generic parameters doesn't work yet
   TRG_MSG_INFO("NumberLeading2 : " << p_NumberLeading2);
   TRG_MSG_INFO("DeltaPhiMin0   : " << p_DeltaPhiMin[0]);
   TRG_MSG_INFO("DeltaPhiMax0   : " << p_DeltaPhiMax[0]);
   TRG_MSG_INFO("DeltaPhiMin1   : " << p_DeltaPhiMin[1]);
   TRG_MSG_INFO("DeltaPhiMax1   : " << p_DeltaPhiMax[1]);
   TRG_MSG_INFO("DeltaPhiMin2   : " << p_DeltaPhiMin[2]);
   TRG_MSG_INFO("DeltaPhiMax2   : " << p_DeltaPhiMax[2]);
   TRG_MSG_INFO("DeltaEtaMin0   : " << p_DeltaEtaMin[0]);
   TRG_MSG_INFO("DeltaEtaMax0   : " << p_DeltaEtaMax[0]);
   TRG_MSG_INFO("DeltaEtaMin1   : " << p_DeltaEtaMin[1]);
   TRG_MSG_INFO("DeltaEtaMax1   : " << p_DeltaEtaMax[1]);
   TRG_MSG_INFO("DeltaEtaMin2   : " << p_DeltaEtaMin[2]);
   TRG_MSG_INFO("DeltaEtaMax2   : " << p_DeltaEtaMax[2]);
   TRG_MSG_INFO("MinET1          : " << p_MinET1);
   TRG_MSG_INFO("MinET2          : " << p_MinET2);
   TRG_MSG_INFO("number output : " << numberOutputBits());
   
   return StatusCode::SUCCESS;
}



TCS::StatusCode
TCS::DeltaEtaPhiIncl1::process( const std::vector<TCS::TOBArray const *> & input,
                             const std::vector<TCS::TOBArray *> & output,
                             Decision & decison )
{

   if(input.size() == 1) {

      TRG_MSG_DEBUG("input size     : " << input[0]->size());

      unsigned int nLeading = p_NumberLeading1;

      for( TOBArray::const_iterator tob1 = input[0]->begin(); 
           tob1 != input[0]->end() && distance( input[0]->begin(), tob1) < nLeading;
           ++tob1) 
         {
            
            if( parType_t((*tob1)->Et()) <= min(p_MinET1,p_MinET2)) continue; // ET cut

            TCS::TOBArray::const_iterator tob2 = tob1; ++tob2;      
            for( ;
                 tob2 != input[0]->end() && distance( input[0]->begin(), tob2) < p_NumberLeading2;
                 ++tob2) {

               if( parType_t((*tob2)->Et()) <= min(p_MinET1,p_MinET2)) continue; // ET cut
               if( (parType_t((*tob1)->Et()) <= max(p_MinET1,p_MinET2)) && (parType_t((*tob2)->Et()) <= max(p_MinET1,p_MinET2))) continue;

               // DeltaPhi cuts
               unsigned int deltaPhi = calcDeltaPhi( *tob1, *tob2 );
	       // DeltaEta cuts
	       unsigned int deltaEta = calcDeltaEta( *tob1, *tob2 );
	       //
	       // to-do change message output

               std::stringstream msgss;
               msgss << "    Combination : " << distance( input[0]->begin(), tob1) << " x " << distance( input[0]->begin(), tob2) << "  phi1=" << (*tob1)->phi() << " , phi2=" << (*tob2)->phi()
                     << ", DeltaPhi = " << deltaPhi << ", DeltaEta = " << deltaEta << " -> ";

               bool accept[3];
               for(unsigned int i=0; i<numberOutputBits(); ++i) {
                  accept[i] = ( deltaEta >= p_DeltaEtaMin[i] ||  deltaPhi >= p_DeltaPhiMin[i] ) && deltaPhi <= p_DeltaPhiMax[i] && deltaEta <= p_DeltaEtaMax[i];
                  if( accept[i] ) {
                     decison.setBit(i, true);  
                     output[i]->push_back( TCS::CompositeTOB(*tob1, *tob2) );
                  }
                  msgss << (accept[i]?"pass":"fail") << "|";
               }
               TRG_MSG_DEBUG(msgss.str());
            }
         }
      
      

   } else {

      TCS_EXCEPTION("DeltaEtaPhiIncl1 alg must have  1 input, but got " << input.size());

   }
   return TCS::StatusCode::SUCCESS;
}
