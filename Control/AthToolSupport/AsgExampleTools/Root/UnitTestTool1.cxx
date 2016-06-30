//        Copyright Iowa State University 2016.
//                  Author: Nils Krumnack
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (nils.erik.krumnack@cern.ch) for bug
// reports, feature suggestions, praise and complaints.


//
// includes
//

#include <AsgExampleTools/UnitTestTool1.h>

//
// method implementations
//

namespace asg
{
  UnitTestTool1 ::
  UnitTestTool1 (const std::string& val_name)
    : AsgTool (val_name)
  {
    declareProperty ("propertyInt", m_propertyInt, "the integer property");
    declareProperty ("initializeFail", m_initializeFail, "whether initialize should fail");
  }



  StatusCode UnitTestTool1 ::
  initialize ()
  {
    if (m_initializeFail)
    {
      ATH_MSG_ERROR ("tool configured to fail initialize");
      return StatusCode::FAILURE;
    }
    if (m_isInitialized)
    {
      ATH_MSG_ERROR ("initialize called twice");
      return StatusCode::FAILURE;
    }
    m_isInitialized = true;
    return StatusCode::SUCCESS;
  }



  int UnitTestTool1 ::
  getPropertyInt () const
  {
    return m_propertyInt;
  }



  void UnitTestTool1 ::
  setPropertyInt (int val_property)
  {
    m_propertyInt = val_property;
  }



  bool UnitTestTool1 ::
  isInitialized () const
  {
    return m_isInitialized;
  }
}
