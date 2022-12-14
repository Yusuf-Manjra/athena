/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TESTDRIVER_H
#define TESTDRIVER_H

#include <vector>
#include <string>

class TestDriver {
public:
  TestDriver();
  ~TestDriver();
  void loadLibraries( const std::vector<std::string>& libraries );
  std::string testWriting();
  void testReading(const std::string& testTypeID);
};

#endif
