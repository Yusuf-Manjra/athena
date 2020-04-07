/*
Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#include "MbtsFexMT.h"

MbtsFexMT::MbtsFexMT(const std::string& name, ISvcLocator* pSvcLocator):
AthReentrantAlgorithm(name, pSvcLocator)
{

}

MbtsFexMT::~MbtsFexMT()
{
}
StatusCode MbtsFexMT::initialize()
{
  ATH_CHECK( m_emScaleKey.initialize() );
  ATH_CHECK( m_badChannelsKey.initialize() );
  ATH_CHECK( m_TileHelperKey.initialize() );
  ATH_CHECK( m_dataAccessSvc.retrieve() );
  ATH_CHECK(m_t2MbtsBitsKey.initialize());
  if (! m_monTool.empty() ) ATH_CHECK( m_monTool.retrieve() );

  return StatusCode::SUCCESS;
}

StatusCode MbtsFexMT::execute(const EventContext& context) const
{

  TileCellCollection mbtsContainer (SG::VIEW_ELEMENTS); // testing
  ATH_CHECK(m_dataAccessSvc->loadMBTS( context, mbtsContainer));
  ATH_MSG_DEBUG ("Successfully retrieved mbtsContainer collection of size " << mbtsContainer.size());

  SG::ReadHandle<TileTBID> TileHelper(m_TileHelperKey, context );
  std::vector<float> triggerEnergies(xAOD::TrigT2MbtsBits::NUM_MBTS,0.);
  std::vector<float> triggerTimes(xAOD::TrigT2MbtsBits::NUM_MBTS,0.);

  for(auto mbtsTile: mbtsContainer ){
    Identifier id=mbtsTile->ID();

    // MBTS Id type is  "side"  +/- 1
    int type_id = TileHelper->type(id);

    // MBTS Id channel is "eta"  0-1   zero is closer to beam pipe
    unsigned int channel_id = TileHelper->channel(id);

    // MBTS Id module is "phi"  0-7
    unsigned int module_id = TileHelper->module(id);

    ATH_MSG_DEBUG("type_id = " << type_id<<" channel_id = " << channel_id<<" module_id = " << module_id);

    // Catch errors
    if(abs(type_id) != 1) {
      ATH_MSG_WARNING("MBTS identifier type is out of range");
      continue;
    }
    if( channel_id > 1 ){
      ATH_MSG_WARNING("MBTS identifier channel is out of range");
      continue;
    }
    if( module_id > 7 ){
      ATH_MSG_WARNING("MBTS identifier module is out of range");
      continue;
    }

    unsigned int bit_pos = 0; // The position of the bit

    if(type_id == -1) { // C-side
      bit_pos += 16;
    }

    bit_pos += channel_id*8;
    bit_pos += module_id;

    ATH_CHECK(bit_pos < triggerEnergies.size());
    triggerEnergies[bit_pos] = mbtsTile->energy();
    ATH_MSG_DEBUG("Counter id = " << bit_pos << ", energy = " << triggerEnergies[bit_pos] << " pC");


    ATH_CHECK(bit_pos < triggerTimes.size());
    triggerTimes[bit_pos] = mbtsTile->time();
    ATH_MSG_DEBUG("Counter id = " << bit_pos << ", time = " << triggerTimes[bit_pos] << " ns");

  }
  auto mon_triggerEnergies = Monitored::Collection("triggerEnergy",triggerEnergies);
  auto mon_triggerEta = Monitored::Collection("eta",mbtsContainer,[](auto &cellptr){return cellptr->eta();});
  auto mon_triggerPhi = Monitored::Collection("phi",mbtsContainer,[](auto &cellptr){return cellptr->phi();});
  auto mon_triggerTimes = Monitored::Collection("triggerTime ",triggerTimes);
  Monitored::Group(m_monTool,mon_triggerEnergies,mon_triggerEta,mon_triggerPhi,mon_triggerTimes);

  SG::WriteHandle<xAOD::TrigT2MbtsBitsContainer> mbtsHandle (m_t2MbtsBitsKey, context);

  auto trigMbtsContainer = std::make_unique< xAOD::TrigT2MbtsBitsContainer>();
  auto trigMbtsAuxContainer = std::make_unique< xAOD::TrigT2MbtsBitsAuxContainer>();
  trigMbtsContainer->setStore(trigMbtsAuxContainer.get());

  xAOD::TrigT2MbtsBits * mbtsObj = new xAOD::TrigT2MbtsBits();
  trigMbtsContainer->push_back(mbtsObj);

  mbtsObj->setTriggerEnergies(triggerEnergies);
  mbtsObj->setTriggerTimes(triggerTimes);

  ATH_CHECK(mbtsHandle.record( std::move(trigMbtsContainer), std::move( trigMbtsAuxContainer ) ) );
  return StatusCode::SUCCESS;
}
