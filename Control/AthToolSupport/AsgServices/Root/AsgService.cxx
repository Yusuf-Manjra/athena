/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack
/// @author David Adams <dladams@bnl.gov> (for original implementation for tools)



// Local include(s):
#include "AsgServices/AsgService.h"

// System include(s):
#include <iostream>

namespace asg
{
  AsgService ::
  AsgService( const std::string& name,
              ISvcLocator* pSvcLocator )
      : AsgServiceBase(
#ifndef XAOD_STANDALONE
                       name, pSvcLocator
#else // not XAOD_STANDALONE
                       name
#endif // not XAOD_STANDALONE
                    )
   {
     (void) pSvcLocator;
   }

  StatusCode AsgService ::
  initialize ()
  {
    return StatusCode::SUCCESS;
  }



  StatusCode AsgService ::
  finalize ()
  {
    return StatusCode::SUCCESS;
  }



  void AsgService::print() const
  {
    ATH_MSG_INFO( "AsgService " << name() << " @ " << this );
    return;
  }

} // namespace asg
