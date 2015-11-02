/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file  CaloIdentifier/test/larhec_id_test_common.cxx
 * @author scott snyder
 * @date May 2013
 * @brief Code common between LArHEC_ID_test and LArHEC_SuperCell_ID test.
 *        This file is meant to be included in another file,
 *        not compiled separately.
 */


#include "boost/foreach.hpp"
#include "hash_test.h"


// This because test differencing ignored lines containing `0x...'
std::string munghex (const std::string& h)
{
  if (h.size() > 2 && h[0] == '0' && h[1] == 'x')
    return "hex(" + h.substr (2, std::string::npos) + ")";
  return h;
}
void basic_print_id (const LArHEC_Base_ID& idhelper, const Identifier& id)
{
  std::cout << idhelper.show_to_string (id) << " "
            << munghex(id.getString()) << "\n";
}


class LArHEC_ID_Test
  : public LArHEC_Base_ID
{
public:
  using LArHEC_Base_ID::lar_field_value;
  using LArHEC_Base_ID::lar_hec_field_value;
};


class CellCounter
{
public:
  CellCounter();
  void count(int bec, int samp);
  void report();


private:
  enum {
    N_REG = 2,
    N_SAMP = 4
  };
  unsigned m_counts[N_REG][N_SAMP];
};


CellCounter::CellCounter()
{
  for (unsigned i=0; i < N_REG; i++)
    for (unsigned j=0; j < N_SAMP; j++)
      m_counts[i][j] = 0;
}


void CellCounter::count (int reg, int samp)
{
  if (reg < 0 || reg >= N_REG) std::abort();
  if (samp < 0 || samp >= N_SAMP) std::abort();
  ++m_counts[reg][samp];
}


void CellCounter::report()
{
  unsigned tot[N_REG] = {0};
  printf ("Sampling   Outer         Inner\n");
  for (unsigned s=0; s < N_SAMP; s++) {
    printf ("%1d          %6d        %6d\n",
            s, m_counts[0][s], m_counts[1][s]);
    for (unsigned b=0; b < N_REG; b++)
      tot[b] += m_counts[b][s];
  }
  printf ("Total      %6d        %6d   Grand total: %6d\n",
          tot[0], tot[1],
          tot[0]+tot[1]);
}


void test_connected (const LArHEC_Base_ID& idhelper, bool supercell)
{
  std::cout << "test_connected\n";

  HASH_TEST1(channel,hec,HASH_TEST_EXTRA(channel));
  HASH_TEST1(region,reg,HASH_TEST_EXTRA(region));

  CellCounter counts;
  TEST_LOOP (Identifier id, idhelper.hec_range()) {
    assert (idhelper.is_lar_hec (id));
    assert (idhelper.is_connected (id));
    IdentifierHash hashId = idhelper.channel_hash (id);
    assert (idhelper.is_connected (hashId));
    assert (hashId == idhelper.channel_hash_binary_search (id));

    int side = idhelper.pos_neg(id);
    int samp = idhelper.sampling(id);
    int reg  = idhelper.region(id);
    int eta  = idhelper.eta(id);
    int phi  = idhelper.phi(id);

    assert (idhelper.is_supercell(id) == supercell);
      
    assert (idhelper.is_connected (side, samp, reg, eta, phi));
    assert (!idhelper.is_disconnected (side, samp, reg, eta, phi));

    Identifier id2 = idhelper.channel_id (side, samp, reg, eta, phi);
    assert (id == id2);

    Identifier reg_id = idhelper.region_id (side, samp, reg);
    id2 = idhelper.channel_id (reg_id, eta, phi);
    assert (id == id2);
    assert (reg_id == idhelper.region_id (id));

    counts.count (reg, samp);

    ExpandedIdentifier exp_id;
    LArHEC_ID_Test* idhelper_test = (LArHEC_ID_Test*)&idhelper;
    exp_id << idhelper_test->lar_field_value()
      	   << idhelper_test->lar_hec_field_value()
	   << idhelper.pos_neg(id)
	   << idhelper.sampling(id)
	   << idhelper.region(id)
           << idhelper.eta(id)
           << idhelper.phi(id)
           << (unsigned)idhelper.is_supercell(id);
    assert (idhelper.channel_id (exp_id) == id);
  }
  counts.report();

  TEST_LOOP (Identifier id, idhelper.reg_range()) {
    assert (idhelper.is_supercell(id) == supercell);
    IdentifierHash hashId = idhelper.region_hash (id);

    printf ("Region %-18s eta: %8.6f %8.6f %1d %3d phi: %3.1f %8.6f %1d %3d\n",
            idhelper.show_to_string(id).c_str(),
            idhelper.eta0(hashId),
            idhelper.etaGranularity(hashId),
            idhelper.eta_min(id),
            idhelper.eta_max(id),
            idhelper.phi0(hashId),
            idhelper.phiGranularity(hashId),
            idhelper.phi_min(id),
            idhelper.phi_max(id));

    int side = idhelper.pos_neg(id);
    int samp = idhelper.sampling(id);
    int reg  = idhelper.region(id);
    Identifier id2 = idhelper.region_id (side, samp, reg);
    assert (id == id2);

    ExpandedIdentifier exp_id;
    LArHEC_ID_Test* idhelper_test = (LArHEC_ID_Test*)&idhelper;
    exp_id << idhelper_test->lar_field_value()
      	   << idhelper_test->lar_hec_field_value()
	   << idhelper.pos_neg(id)
	   << idhelper.sampling(id)
	   << idhelper.region(id);
    assert (idhelper.region_id (exp_id) == id);
  }
}


void test_disco (const LArHEC_Base_ID& idhelper)
{
  std::cout << "test_disco\n";

  // disconnected channels
  std::vector<Identifier>::const_iterator itId = idhelper.disc_hec_begin();
  std::vector<Identifier>::const_iterator itIdEnd = idhelper.disc_hec_end();
  CellCounter counts;

  std::cout << "  nchan " << itIdEnd - itId << "\n";

  assert (idhelper.channel_hash_max() == idhelper.disc_channel_hash_min());
  std::vector<bool> hashvec(idhelper.disc_channel_hash_max() - idhelper.disc_channel_hash_min());

  int hashsum = 0;
  for (; itId != itIdEnd; ++itId) {
    Identifier id = *itId;
    assert (idhelper.is_lar_hec (id));
    assert (!idhelper.is_connected (id));

    IdentifierHash hashId = idhelper.disc_channel_hash (id) ;
    assert (!idhelper.is_connected (hashId));
    Identifier id2 = idhelper.disc_channel_id(hashId);
    assert (id == id2);
    assert (hashId >= idhelper.channel_hash_max());
    assert (!hashvec[hashId - idhelper.disc_channel_hash_min()]);
    hashvec[hashId - idhelper.disc_channel_hash_min()] = true;

    hashsum += hashId;

    int side = idhelper.pos_neg(id);
    int samp = idhelper.sampling(id);
    int reg  = idhelper.region(id);
    counts.count (reg, samp);

    assert (idhelper.is_supercell(id) == false);

    assert (idhelper.eta(id) == -999);
    assert (idhelper.phi(id) == -999);
    int eta  = idhelper.disc_eta(id);
    int phi  = idhelper.disc_phi(id);

    assert (!idhelper.is_connected (side, samp, reg, eta, phi));
    assert (idhelper.is_disconnected (side, samp, reg, eta, phi));
      
    id2 = idhelper.disc_channel_id (side, samp, reg, eta, phi);
    assert (id == id2);

    Identifier reg_id = idhelper.disc_region_id (side, samp, reg);
    id2 = idhelper.channel_id (reg_id, eta, phi);
    assert (id == id2);

    ExpandedIdentifier exp_id;
    LArHEC_ID_Test* idhelper_test = (LArHEC_ID_Test*)&idhelper;
    exp_id << idhelper_test->lar_field_value()
      	   << idhelper_test->lar_hec_field_value()
	   << idhelper.pos_neg(id)
	   << idhelper.sampling(id)
	   << idhelper.region(id)
           << idhelper.disc_eta(id)
           << idhelper.disc_phi(id)
           << (unsigned)idhelper.is_supercell(id);
    assert (idhelper.disc_channel_id (exp_id) == id);
  }
  for (size_t i = 0; i < hashvec.size(); i++)
    assert (hashvec[i]);

  TEST_LOOP (Identifier id, idhelper.disc_hec_range()) {
    hashsum -= idhelper.disc_channel_hash (id);
  }
  assert (hashsum == 0);

  hashsum = 0;
  BOOST_FOREACH (Identifier id, idhelper.disc_reg_range()) {
    ExpandedIdentifier exp_id;
    LArHEC_ID_Test* idhelper_test = (LArHEC_ID_Test*)&idhelper;
    exp_id << idhelper_test->lar_field_value()
      	   << idhelper_test->lar_hec_field_value()
	   << idhelper.pos_neg(id)
	   << idhelper.sampling(id)
	   << idhelper.region(id);
    assert (idhelper.disc_region_id (exp_id) == id);
    ++hashsum;
  }

  itId = idhelper.disc_reg_begin();
  itIdEnd = idhelper.disc_reg_end();
  for (; itId != itIdEnd; ++itId)
    --hashsum;
  assert (hashsum == 0);

  counts.report();
}


void test_exceptions (const LArHEC_Base_ID& idhelper)
{
  std::cout << "test_exceptions\n";

  bool caught = false;
  try {
    /*Identifier wrongRegionId =*/ idhelper.region_id (0,99,0); 
  }
  catch(LArID_Exception & except){
    caught = true;
    std::cout << "Exception 1: " << (std::string)except << "\n";
  }
  assert (caught);

  caught = false;
  try {
    /*Identifier wrongChannelId =*/ idhelper.channel_id (0,99,0,0,0); 
  }
  catch(LArID_Exception & except){
    caught = true;
    std::cout << "Exception 2: " << (std::string)except << "\n";
  }
  assert (caught);

  caught = false;
  try {
    Identifier goodRegionId = idhelper.region_id (2,0,0); 
    /*Identifier wrongChannelId =*/ idhelper.channel_id (goodRegionId,0,-99); 
  }
  catch(LArID_Exception & except){
    caught = true;
    std::cout << "Exception 3: " << (std::string)except << "\n";
  }
  assert (caught);
}


// hec-specific addressing.
void test_hec (const LArHEC_Base_ID& idhelper)
{
  std::cout << "test_hec\n";

  assert (idhelper.channel_id (2, 0, 0, 5, 1) ==
          idhelper.channel_id (2, 0,    5, 1));
  assert (idhelper.channel_id (2, 0, 1, 3, 1) ==
          idhelper.channel_id (2, 0,   13, 1));

  assert (idhelper.channel_id (2, 0, 0, 5, 1) ==
          idhelper.channel_id (2, 0, 0, 0, 5, 1));
  assert (idhelper.channel_id (2, 0, 0, 5, 10) ==
          idhelper.channel_id (2, 0, 5, 0, 5, 0));
  assert (idhelper.channel_id (2, 0, 0, 5, 11) ==
          idhelper.channel_id (2, 0, 5, 0, 5, 1));
}
