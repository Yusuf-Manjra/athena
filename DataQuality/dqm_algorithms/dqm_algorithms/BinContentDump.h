/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DQM_ALGORITHMS_BINCONTENTDUMP_H
#define DQM_ALGORITHMS_BINCONTENTDUMP_H

#include <dqm_core/Algorithm.h>
#include <string>
#include <iosfwd>

namespace dqm_algorithms
{
	struct BinContentDump : public dqm_core::Algorithm
        {
	  BinContentDump();

	  ~BinContentDump();

	    //overwrites virtual functions
	  BinContentDump * clone( );
	  dqm_core::Result * execute( const std::string & , const TObject & , const dqm_core::AlgorithmConfig & );
	  using dqm_core::Algorithm::printDescription;
	  virtual void  printDescription(std::ostream& out);
	};
}

#endif // DQM_ALGORITHMS_BINCONTENTDUMP_H
