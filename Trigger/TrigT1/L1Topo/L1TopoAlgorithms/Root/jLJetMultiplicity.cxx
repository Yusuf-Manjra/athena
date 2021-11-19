/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
/*********************************
 * jLJetMultiplicity
 *
 * @brief algorithm that computes the multiplicity for a specified list and ET threshold
 * line 1: 0 or 1, line 1 and 2 : 2 or more, uses 2 bits
 *
 * @param NumberLeading largeR jets, MinET
**********************************/

#include <cmath>

#include "L1TopoAlgorithms/jLJetMultiplicity.h"
#include "L1TopoCommon/Exception.h"
#include "L1TopoInterfaces/Count.h"

#include "L1TopoEvent/TOBArray.h"
#include "L1TopoEvent/jLargeRJetTOBArray.h"
#include "L1TopoEvent/GenericTOB.h"

REGISTER_ALG_TCS(jLJetMultiplicity)

using namespace std;


TCS::jLJetMultiplicity::jLJetMultiplicity(const std::string & name) : CountingAlg(name)
{
   
   
   setNumberOutputBits(12); //To-Do: Make this flexible to addapt to the menu. Each counting requires more than one bit

}

TCS::jLJetMultiplicity::~jLJetMultiplicity(){}


TCS::StatusCode
TCS::jLJetMultiplicity::initialize() { 

  m_threshold = getThreshold();

  // book histograms
  std::string hname_accept = "hjLJetMultiplicity_accept_"+m_threshold->name();
  bookHist(m_histAccept, hname_accept, "eta vs pT", 150, -100, 100, 30, 0., 2000.);

  hname_accept = "hjLJetMultiplicity_accept_counts_"+m_threshold->name();
  bookHist(m_histAccept, hname_accept, "Counts", 15, 0., 10. );

  return StatusCode::SUCCESS;
     
}


TCS::StatusCode
TCS::jLJetMultiplicity::processBitCorrect( const TCS::InputTOBArray & input,
					 Count & count)

{
   return process(input, count);
}

TCS::StatusCode
TCS::jLJetMultiplicity::process( const TCS::InputTOBArray & input,
			       Count & count )
{

  // Grab the threshold and cast it into the right type
  auto jLJThr = dynamic_cast<const TrigConf::L1Threshold_jLJ &>(*m_threshold);

  // Grab inputs
  const jLargeRJetTOBArray & jLargeRjets = dynamic_cast<const jLargeRJetTOBArray&>(input);

  int counting = 0; 
  
  // loop over input TOBs
  for(jLargeRJetTOBArray::const_iterator jLargeRjet = jLargeRjets.begin();
      jLargeRjet != jLargeRjets.end();
      ++jLargeRjet ) {
    
    const GenericTOB gtob(**jLargeRjet);

    // Dividing by 4 standing for converting eta from 0.025 to 0.1 granularity as it is defined in the menu as 0.1 gran.
    bool passed = gtob.Et() >= jLJThr.thrValueCounts(gtob.eta()/4);

    if (passed) {
      counting++; 
      fillHist2D( m_histAccept[0], gtob.eta(), gtob.EtDouble() );
    }

  }

  fillHist1D( m_histAccept[1], counting);
  
  // Pass counting to TCS::Count object - output bits are composed there
  count.setSizeCount(counting);
  
  return TCS::StatusCode::SUCCESS;

}
