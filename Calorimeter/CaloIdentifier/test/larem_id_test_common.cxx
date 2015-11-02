/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file  CaloIdentifier/test/larem_id_test_common.cxx
 * @author scott snyder
 * @date Aug 2012
 * @brief Code common between LArEM_ID_test and LArEM_SuperCell_ID test.
 *        This file is meant to be included in another file,
 *        not compiled separately.
 */


#include "boost/foreach.hpp"


// This because test differencing ignored lines containing `0x...'
std::string munghex (const std::string& h)
{
  if (h.size() > 2 && h[0] == '0' && h[1] == 'x')
    return "hex(" + h.substr (2, std::string::npos) + ")";
  return h;
}
void basic_print_id (const LArEM_Base_ID& idhelper, const Identifier& id)
{
  std::cout << idhelper.show_to_string (id) << " "
            << munghex(id.getString()) << "\n";
}


class LArEM_ID_Test
  : public LArEM_Base_ID
{
public:
  using LArEM_Base_ID::lar_field_value;
  using LArEM_Base_ID::lar_em_field_value;
};


class CellCounter
{
public:
  CellCounter();
  void count(int bec, int samp);
  void report();


private:
  enum {
    N_BEC = 3,
    N_SAMP = 4
  };
  unsigned m_counts[N_BEC][N_SAMP];
};


CellCounter::CellCounter()
{
  for (unsigned i=0; i < N_BEC; i++)
    for (unsigned j=0; j < N_SAMP; j++)
      m_counts[i][j] = 0;
}


void CellCounter::count (int bec, int samp)
{
  bec = std::abs(bec) - 1;
  if (bec < 0 || bec >= N_BEC) std::abort();
  if (samp < 0 || samp >= N_SAMP) std::abort();
  ++m_counts[bec][samp];
}


void CellCounter::report()
{
  unsigned tot[N_BEC] = {0};
  printf ("Sampling   Barrel   Outer wheel   Inner wheel\n");
  for (unsigned s=0; s < N_SAMP; s++) {
    printf ("%1d          %6d        %6d        %6d\n",
            s, m_counts[0][s], m_counts[1][s], m_counts[2][s]);
    for (unsigned b=0; b < N_BEC; b++)
      tot[b] += m_counts[b][s];
  }
  printf ("Total      %6d        %6d        %6d   Grand total: %6d\n",
          tot[0], tot[1], tot[2],
          tot[0]+tot[1]+tot[2]);
}


void region_test (const LArEM_Base_ID& em_id, const Identifier& ch_id)
{
  int bec  = em_id.barrel_ec(ch_id);
  switch (bec) {
  case 1:
  case -1:
    assert (em_id.is_em_barrel (ch_id));
    assert (!em_id.is_em_endcap (ch_id));
    assert (!em_id.is_em_endcap_inner (ch_id));
    assert (!em_id.is_em_endcap_outer (ch_id));
    break;
  case 2:
  case -2:
    assert (!em_id.is_em_barrel (ch_id));
    assert (em_id.is_em_endcap (ch_id));
    assert (!em_id.is_em_endcap_inner (ch_id));
    assert (em_id.is_em_endcap_outer (ch_id));
    break;
  case 3:
  case -3:
    assert (!em_id.is_em_barrel (ch_id));
    assert (em_id.is_em_endcap (ch_id));
    assert (em_id.is_em_endcap_inner (ch_id));
    assert (!em_id.is_em_endcap_outer (ch_id));
    break;
  default:
    abort();
  }
}


void test_connected (const LArEM_Base_ID& em_id, bool supercell = false)
{
  std::cout << "test_connected\n";

  // channels
  IdContext channelContext = em_id.channel_context();
  std::vector<Identifier>::const_iterator itId = em_id.em_begin();
  std::vector<Identifier>::const_iterator itIdEnd = em_id.em_end();
  std::vector<bool> hashvec(em_id.channel_hash_max());
  CellCounter counts;

  std::cout << "  nchan " << itIdEnd - itId << "\n";

  const std::vector<Identifier>& ch_ids = em_id.channel_ids();

  int hashsum = 0;
  for(; itId!=itIdEnd; ++itId) {
    Identifier ch_id = *itId;
    assert (em_id.is_lar_em (ch_id));
    assert (em_id.is_connected (ch_id));

    IdentifierHash hashId;
    assert (em_id.get_hash (ch_id, hashId, &channelContext) == 0);
    assert (em_id.is_connected (hashId));
    Identifier ch_id2;
    assert (em_id.get_id (hashId, ch_id2, &channelContext) == 0);
    assert (ch_id == ch_id2);
    assert (ch_id == em_id.channel_id (hashId));
    assert (hashId == em_id.channel_hash (ch_id));
    assert (hashId == em_id.channel_hash_binary_search (ch_id));
    assert (hashId < hashvec.size());
    assert (!hashvec[hashId]);
    hashvec[hashId] = true;
    assert (ch_ids[hashId] == ch_id);

    hashsum += hashId;

    int bec  = em_id.barrel_ec(ch_id);
    int samp = em_id.sampling(ch_id);
    int reg  = em_id.region(ch_id);
    int eta  = em_id.eta(ch_id);
    int phi  = em_id.phi(ch_id);
    region_test (em_id, ch_id);

    assert (em_id.is_supercell(ch_id) == supercell);
      
    assert (em_id.is_connected (bec, samp, reg, eta, phi));
    assert (!em_id.is_disconnected (bec, samp, reg, eta, phi));

    ch_id2 = em_id.channel_id (bec, samp, reg, eta, phi);
    assert (ch_id == ch_id2);

    Identifier reg_id = em_id.region_id (bec, samp, reg);
    ch_id2 = em_id.channel_id (reg_id, eta, phi);
    assert (ch_id == ch_id2);
    assert (reg_id == em_id.region_id (ch_id));

    counts.count (bec, samp);

    ExpandedIdentifier exp_id;
    LArEM_ID_Test* em_id_test = (LArEM_ID_Test*)&em_id;
    exp_id << em_id_test->lar_field_value()
      	   << em_id_test->lar_em_field_value()
	   << em_id.barrel_ec(ch_id)
	   << em_id.sampling(ch_id)
	   << em_id.region(ch_id)
           << em_id.eta(ch_id)
           << em_id.phi(ch_id)
           << (unsigned)em_id.is_supercell(ch_id);
    assert (em_id.channel_id (exp_id) == ch_id);
  }
  for (size_t i = 0; i < hashvec.size(); i++)
    assert (hashvec[i]);

#if __cplusplus > 201100
  for (Identifier ch_id : em_id.em_range()) {
#else
  BOOST_FOREACH (Identifier ch_id, em_id.em_range()) {
#endif
    hashsum -= em_id.channel_hash (ch_id);
  }
  assert (hashsum == 0);


  // regions
  itId = em_id.reg_begin();
  itIdEnd = em_id.reg_end();
  hashvec.clear();
  hashvec.resize (em_id.region_hash_max());
  IdContext region_context = em_id.region_context();
  
  std::cout << "  nreg " << itIdEnd - itId << "\n";

  for(; itId!=itIdEnd; ++itId) {
    Identifier reg_id = *itId;
    
    assert (em_id.is_supercell(reg_id) == supercell);

    IdentifierHash regHash = em_id.region_hash(reg_id);
    hashsum += regHash;
#if 1
    printf ("Region %-18s eta: %8.6f %8.6f %1d %3d phi: %3.1f %8.6f %1d %3d\n",
            em_id.show_to_string(reg_id).c_str(),
            em_id.eta0(regHash),
            em_id.etaGranularity(regHash),
            em_id.eta_min(reg_id),
            em_id.eta_max(reg_id),
            em_id.phi0(regHash),
            em_id.phiGranularity(regHash),
            em_id.phi_min(reg_id),
            em_id.phi_max(reg_id));
#endif
    Identifier reg_id2 = em_id.region_id(regHash);
    assert (reg_id == reg_id2);

    IdentifierHash regHash2;
    assert (em_id.get_hash (reg_id, regHash2, &region_context) == 0);

    assert (!hashvec[regHash]);
    hashvec[regHash] = true;

    ExpandedIdentifier exp_id;
    LArEM_ID_Test* em_id_test = (LArEM_ID_Test*)&em_id;
    exp_id << em_id_test->lar_field_value()
      	   << em_id_test->lar_em_field_value()
	   << em_id.barrel_ec(reg_id)
	   << em_id.sampling(reg_id)
	   << em_id.region(reg_id);
    assert (em_id.region_id (exp_id) == reg_id);
  }
  for (size_t i = 0; i < hashvec.size(); i++)
    assert (hashvec[i]);

#if __cplusplus > 201100
  for (Identifier ch_id : em_id.reg_range()) {
#else
  BOOST_FOREACH (Identifier ch_id, em_id.reg_range()) {
#endif
    hashsum -= em_id.region_hash (ch_id);
  }
  assert (hashsum == 0);

  counts.report();
}


void test_disco (const LArEM_Base_ID& em_id)
{
  std::cout << "test_disco\n";

  // disconnected channels
  //IdContext channelContext = em_id.channel_context();
  std::vector<Identifier>::const_iterator itId = em_id.disc_em_begin();
  std::vector<Identifier>::const_iterator itIdEnd = em_id.disc_em_end();
  CellCounter counts;

  std::cout << "  nchan " << itIdEnd - itId << "\n";

  assert (em_id.channel_hash_max() == em_id.disc_channel_hash_min());
  std::vector<bool> hashvec(em_id.disc_channel_hash_max() - em_id.disc_channel_hash_min());

  int hashsum = 0;
  for (; itId!=itIdEnd; ++itId) {
    Identifier ch_id = *itId;
    assert (em_id.is_lar_em (ch_id));
    assert (!em_id.is_connected (ch_id));

    IdentifierHash hashId = em_id.disc_channel_hash (ch_id) ;
    assert (!em_id.is_connected (hashId));
    Identifier ch_id2 = em_id.disc_channel_id(hashId);
    assert (ch_id == ch_id2);
    assert (hashId >= em_id.channel_hash_max());
    assert (!hashvec[hashId - em_id.disc_channel_hash_min()]);
    hashvec[hashId - em_id.disc_channel_hash_min()] = true;

    hashsum += hashId;

    int bec  = em_id.barrel_ec(ch_id);
    int samp = em_id.sampling(ch_id);
    int reg  = em_id.region(ch_id);
    region_test (em_id, ch_id);
    counts.count (bec, samp);

    assert (em_id.is_supercell(ch_id) == false);

    assert (em_id.eta(ch_id) == -999);
    assert (em_id.phi(ch_id) == -999);
    int eta  = em_id.disc_eta(ch_id);
    int phi  = em_id.disc_phi(ch_id);

    assert (!em_id.is_connected (bec, samp, reg, eta, phi));
    assert (em_id.is_disconnected (bec, samp, reg, eta, phi));
      
    ch_id2 = em_id.disc_channel_id (bec, samp, reg, eta, phi);
    assert (ch_id == ch_id2);

    Identifier reg_id = em_id.disc_region_id (bec, samp, reg);
    ch_id2 = em_id.channel_id (reg_id, eta, phi);
    assert (ch_id == ch_id2);

    ExpandedIdentifier exp_id;
    LArEM_ID_Test* em_id_test = (LArEM_ID_Test*)&em_id;
    exp_id << em_id_test->lar_field_value()
      	   << em_id_test->lar_em_field_value()
	   << em_id.barrel_ec(ch_id)
	   << em_id.sampling(ch_id)
	   << em_id.region(ch_id)
           << em_id.disc_eta(ch_id)
           << em_id.disc_phi(ch_id)
           << (unsigned)em_id.is_supercell(ch_id);
    assert (em_id.disc_channel_id (exp_id) == ch_id);
  }
  for (size_t i = 0; i < hashvec.size(); i++)
    assert (hashvec[i]);

#if __cplusplus > 201100
  for (Identifier ch_id : em_id.disc_em_range()) {
#else
  BOOST_FOREACH (Identifier ch_id, em_id.disc_em_range()) {
#endif
    hashsum -= em_id.disc_channel_hash (ch_id);
  }
  assert (hashsum == 0);

  counts.report();
}


void test_exceptions (const LArEM_Base_ID& em_id, bool /*supercell*/ = false)
{
  std::cout << "test_exceptions\n";

  bool caught = false;
  try {
    /*Identifier wrongRegionId =*/ em_id.region_id (0,99,0); 
  }
  catch(LArID_Exception & except){
    caught = true;
    std::cout << "Exception 1: " << (std::string)except << "\n";
  }
  assert (caught);

  caught = false;
  try {
    /*Identifier wrongChannelId =*/ em_id.channel_id (0,99,0,0,0); 
  }
  catch(LArID_Exception & except){
    caught = true;
    std::cout << "Exception 2: " << (std::string)except << "\n";
  }
  assert (caught);

  caught = false;
  try {
    Identifier goodRegionId = em_id.region_id (1,0,0); 
    /*Identifier wrongChannelId =*/ em_id.channel_id (goodRegionId,0,-99); 
  }
  catch(LArID_Exception & except){
    caught = true;
    std::cout << "Exception 3: " << (std::string)except << "\n";
  }
  assert (caught);
}


