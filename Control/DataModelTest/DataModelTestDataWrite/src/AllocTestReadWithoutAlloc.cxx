/*
 * Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file DataModelTestDataWrite/src/AllocTestReadWithoutAlloc.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2022
 * @brief Test reading AllocTest with a non-default allocator.
 */


#include "AllocTestReadWithoutAlloc.h"
#include "DataModelTestDataWrite/AllocTestAuxContainer.h"


namespace DMTest {


/**
 * @brief Gaudi initialize method.
 */
StatusCode AllocTestReadWithoutAlloc::initialize()
{
  ATH_CHECK( m_containerKey.initialize() );
  return StatusCode::SUCCESS;
}


/**
 * @brief Algorithm event processing.
 */
StatusCode AllocTestReadWithoutAlloc::execute (const EventContext& ctx) const
{
  SG::ReadHandle<AllocTestContainer> cont (m_containerKey, ctx);
  std::cout << m_containerKey.key() << " ";
  for (const AllocTest* at : *cont) {
    std::cout << at->atInt1() << " " << at->atInt2() << " ";
  }
  std::cout << "\n";
  return StatusCode::SUCCESS;
}


} // namespace DMTest
