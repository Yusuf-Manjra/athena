/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/*! \file Chi2Test_Prob.h file declares the dqm_algorithms::Chi2Test_Prob  class.
 * \author Haleh Hadavand
*/

#ifndef DQM_ALGORITHMS_CHI2TEST_CHI2_H
#define DQM_ALGORITHMS_CHI2TEST_CHI2_H

#include <dqm_algorithms/Chi2Test.h>

namespace dqm_algorithms
{
	struct Chi2Test_Chi2 : public Chi2Test
        {
	  Chi2Test_Chi2(): Chi2Test("Chi2") {};

	};
}

#endif // DQM_ALGORITHMS_CHI2TEST_CHI2_H
