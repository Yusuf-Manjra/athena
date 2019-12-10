/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#include <algorithm>
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/Property.h"
#include "AthenaInterprocess/Incidents.h"
#include "DecisionHandling/HLTIdentifier.h"
#include "TrigSignatureMoniMT.h"

TrigSignatureMoniMT::TrigSignatureMoniMT( const std::string& name, 
			  ISvcLocator* pSvcLocator ) : 
  base_class( name, pSvcLocator )
{}

StatusCode TrigSignatureMoniMT::initialize() {

  ATH_CHECK( m_l1DecisionsKey.initialize() );
  ATH_CHECK( m_finalDecisionKey.initialize() );
  ATH_CHECK( m_HLTMenuKey.initialize() );
  ATH_CHECK( m_L1MenuKey.initialize() );
  ATH_CHECK( m_collectorTools.retrieve() );
  ATH_CHECK( m_histSvc.retrieve() );

  ATH_CHECK( m_incidentSvc.retrieve() );
  m_incidentSvc->addListener(this, AthenaInterprocess::UpdateAfterFork::type());

  m_timeDivider = std::make_unique<TimeDivider>(m_intervals, m_duration, TimeDivider::seconds);

  return StatusCode::SUCCESS;
}

StatusCode TrigSignatureMoniMT::start() {
  SG::ReadHandle<TrigConf::HLTMenu>  hltMenuHandle = SG::makeHandle( m_HLTMenuKey );
  ATH_CHECK( hltMenuHandle.isValid() );

  SG::ReadHandle<TrigConf::L1Menu>  l1MenuHandle = SG::makeHandle( m_L1MenuKey );
  ATH_CHECK( l1MenuHandle.isValid() );

  for ( const TrigConf::Chain& chain: *hltMenuHandle ){
    for ( auto group : chain.groups() ){
      if ( group.find("RATE") == 0 ){
        m_groupToChainMap[group].insert( HLT::Identifier(chain.name()) );
      }
    }

    for ( auto stream : chain.streams() ){
      m_streamToChainMap[stream.getAttribute("name")].insert( HLT::Identifier(chain.name()) );
    }

    auto item = std::find_if(l1MenuHandle->begin(), l1MenuHandle->end(),
      [&](const TrigConf::L1Item& item){ return item.name()==chain.l1item();});

    if ( item != l1MenuHandle->end() ){
      for ( auto group : (*item).getList("bunchgroups", true) ){
        if ( group.getValue() != "BGRP0" ){
          m_chainIDToBunchMap[HLT::Identifier(chain.name())].insert(group.getValue());
        }
      }
    }
  }

  const int x = nBinsX(hltMenuHandle);
  const int xb = nBunchBinsX(hltMenuHandle);
  const int y = nBinsY();
  const int yr = nRateBinsY();
  const int yb = nBunchBinsY(l1MenuHandle);
  ATH_MSG_DEBUG( "Histogram " << x << " x " << y << " bins");

  std::string outputName ("Rate" + std::to_string(m_duration) + "s");
  std::unique_ptr<TH2> hSA = std::make_unique<TH2I>("SignatureAcceptance", "Raw acceptance of signatures in;chain;step", x, 1, x + 1, y, 1, y + 1);
  std::unique_ptr<TH2> hDC = std::make_unique<TH2I>("DecisionCount", "Positive decisions count per step;chain;step", x, 1, x + 1, y, 1, y + 1);
  std::unique_ptr<TH2> hR = std::make_unique<TH2F>(outputName.c_str(), "Rate of positive decisions", x, 1, x + 1, yr, 1, yr + 1);
  std::unique_ptr<TH2> hRB = std::make_unique<TH2I>("RateCountBuffer", "Rate of positive decisions buffer", x, 1, x + 1, yr, 1, yr + 1);
  std::unique_ptr<TH2> hBG = std::make_unique<TH2I>("BunchGroupCount", "Bunch group count per chain", xb, 1, xb + 1, yb, 1, yb + 1);
  

  ATH_CHECK( initHist( hSA, hltMenuHandle ) );
  ATH_CHECK( initHist( hDC, hltMenuHandle ) );
  ATH_CHECK( initHist( hR, hltMenuHandle, false ) );
  ATH_CHECK( initHist( hRB, hltMenuHandle, false ) );
  ATH_CHECK( initBunchHist( hBG, hltMenuHandle , l1MenuHandle ) );

  ATH_CHECK( m_histSvc->regShared( m_bookingPath + "/" + name() + "/SignatureAcceptance", std::move(hSA), m_passHistogram));
  ATH_CHECK( m_histSvc->regShared( m_bookingPath + "/" + name() + "/DecisionCount", std::move(hDC), m_countHistogram));
  ATH_CHECK( m_histSvc->regShared( m_bookingPath + "/" + name() + "/BunchGroupCount", std::move(hBG), m_bunchHistogram));
  ATH_CHECK( m_histSvc->regShared( m_bookingPath + "/" + name() + '/' + outputName.c_str(), std::move(hR), m_rateHistogram));
  
  m_rateBufferHistogram.set(hRB.release(), &m_bufferMutex);
  m_rateBufferHistogram->SetDirectory(0);

  return StatusCode::SUCCESS;
}

StatusCode TrigSignatureMoniMT::stop() {
  //publish final rate histogram
  if (m_timer) {
    m_timer->stop();
    time_t t = time(0);
    unsigned int interval;
    unsigned int duration = m_timeDivider->forcePassed(t, interval);
    updatePublished(duration); //divide by time that really passed not by interval duration
  }
  m_timer.reset();
  delete m_rateBufferHistogram.get();

  if (m_chainIDToBinMap.empty()) {
    ATH_MSG_INFO( "No chains configured, no counts to print" );
    return StatusCode::SUCCESS;
  }
  
  auto collToString = [&]( int xbin, const LockedHandle<TH2>& hist, int startOfset=0, int endOffset=0){ 
    std::string v;
    for ( int ybin = 1; ybin <= hist->GetYaxis()->GetNbins()-endOffset; ++ybin ) {
      if ( ybin > startOfset ) {
        v += std::to_string( int(hist->GetBinContent( xbin, ybin )) );
        v += std::string( 10*ybin - v.size(),  ' ' ); // fill with spaces
      } else {
        v += std::string( 10, ' ');
      }
    }
    
    return v;
  };
  
  auto fixedWidth = [](const std::string& s, size_t sz) {
    return s.size() < sz ?
    s+ std::string(sz - s.size(), ' ') : s; };

  std::string v;
  for ( int bin = 1; bin <= m_passHistogram->GetYaxis()->GetNbins()-3; ++bin ) {
    v += "step";
    v += std::to_string(bin);
    v += std::string( 10*bin - v.size(),  ' ' );
  }
  
  ATH_MSG_INFO( "Chains passing step (1st row events & 2nd row decision counts):" );  
  ATH_MSG_INFO( "Chain name                   L1,      AfterPS,  "<<v<<"Output"  );

  /*
    comment for future dev:
    for combined chains we find x2 the number of decisions, because we count both the HypoAlg and the combo Alg decisions
  */
  
  for ( int bin = 1; bin <= (*m_passHistogram)->GetXaxis()->GetNbins(); ++bin ) {
    const std::string chainName = m_passHistogram->GetXaxis()->GetBinLabel(bin);
    if ( chainName.find("HLT") == 0 ) { // print only for chains
      ATH_MSG_INFO( fixedWidth(chainName, 30)  << collToString( bin, m_passHistogram) );
      ATH_MSG_INFO(fixedWidth(chainName +" decisions", 30) << collToString( bin, m_countHistogram , 2, 1 ) );
    }
    if ( chainName.find("All") == 0 ){
      ATH_MSG_INFO( fixedWidth(chainName, 30)  << collToString( bin, m_passHistogram) );
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode TrigSignatureMoniMT::fillPassEvents(const TrigCompositeUtils::DecisionIDContainer& dc, int row, LockedHandle<TH2>& histogram) const {
  for ( auto id : dc )  {
    auto id2bin = m_chainIDToBinMap.find( id );
    if ( id2bin == m_chainIDToBinMap.end() && HLT::Identifier(id).name().find("leg") != 0 ) {
      ATH_MSG_WARNING( "HLT chain " << HLT::Identifier(id) << " not configured to be monitored" );
    } else {
      histogram->Fill( id2bin->second, double(row) );
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode TrigSignatureMoniMT::fillDecisionCount(const std::vector<TrigCompositeUtils::DecisionID>& dc, int row) const {
  for ( auto id : dc )  {
    auto id2bin = m_chainIDToBinMap.find( id );
    if ( id2bin == m_chainIDToBinMap.end() ) {
    } else {
      m_countHistogram->Fill( id2bin->second, double(row) );
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode TrigSignatureMoniMT::fillBunchGroups(const TrigCompositeUtils::DecisionIDContainer& dc ) const {
  for ( auto id : dc )  {
    auto id2bin = m_chainIDToBinMap.find( id );
    auto bunchGroups = m_chainIDToBunchMap.find( id );
    if ( id2bin != m_chainIDToBinMap.end() && bunchGroups != m_chainIDToBunchMap.end() ) {
      for (auto group : bunchGroups->second){
        m_bunchHistogram->Fill( id2bin->second, double(m_nameToBinMap.at(group)) );
        m_bunchHistogram->Fill( 1, double(m_nameToBinMap.at(group)) );
      }
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode TrigSignatureMoniMT::fillRate(const TrigCompositeUtils::DecisionIDContainer& dc, int row) const {
  return fillPassEvents(dc, row, m_rateBufferHistogram); 
}

StatusCode TrigSignatureMoniMT::fillStreamsAndGroups(const std::map<std::string, TrigCompositeUtils::DecisionIDContainer>& map, const TrigCompositeUtils::DecisionIDContainer& dc) const {
  const double row = nBinsY();
  const double rateRow = nRateBinsY();
  for ( auto m : map ) {
    for ( auto id : dc ) {
      if ( m.second.find(id) != m.second.end() ){
        double bin = m_nameToBinMap.at(m.first);
        m_countHistogram->Fill( bin, row );
        m_rateBufferHistogram->Fill( bin, rateRow );
        m_passHistogram->Fill( bin, row );
        break;
      }
    }
  }
  return StatusCode::SUCCESS;
}

void TrigSignatureMoniMT::updatePublished(unsigned int duration) const {

  ATH_MSG_DEBUG( "Publishing Rate Histogram and Reset" );

  m_rateHistogram->Reset("ICES");
  m_rateHistogram->Add(m_rateBufferHistogram.get(), 1./duration);
  m_rateBufferHistogram->Reset("ICES");
}

void TrigSignatureMoniMT::callback() const {
  //ask time divider if we need to switch to new interval
  time_t t = time(0);
  unsigned int newinterval;
  unsigned int oldinterval;
 
  if ( m_timeDivider->isPassed(t, newinterval, oldinterval) ) {
    updatePublished(m_duration);
  }  

  //schedule itself in another 1/20 of the integration period in milliseconds
  m_timer->start(m_duration*50);
}

void TrigSignatureMoniMT::handle( const Incident& incident ) {
  // Create and start timer after fork
  if (incident.type() == AthenaInterprocess::UpdateAfterFork::type()) {
    if (m_timer) {
      ATH_MSG_WARNING("Timer is already running. UpdateAfterFork incident called more than once?");
      return;
    }
    m_timer = std::make_unique<Athena::AlgorithmTimer>(m_duration*50, boost::bind(&TrigSignatureMoniMT::callback, this), Athena::AlgorithmTimer::DELIVERYBYTHREAD);
    ATH_MSG_DEBUG("Started rate timer");
  }
}

StatusCode TrigSignatureMoniMT::execute( const EventContext& context ) const {

  auto l1Decisions = SG::makeHandle( m_l1DecisionsKey, context );

  const TrigCompositeUtils::Decision* l1SeededChains = nullptr; // Activated by L1
  const TrigCompositeUtils::Decision* unprescaledChains = nullptr; // Activated and passed prescale check
  for (const TrigCompositeUtils::Decision* d : *l1Decisions) {
    if (d->name() == "l1seeded") {
      l1SeededChains = d;
    } else if (d->name() == "unprescaled") {
      unprescaledChains = d;
    }
  }

  if (l1SeededChains == nullptr || unprescaledChains == nullptr) {
    ATH_MSG_ERROR("Unable to read in the summary from the L1Decoder.");
    return StatusCode::FAILURE;
  }

  auto fillL1 = [&]( int index ) -> StatusCode {    
    TrigCompositeUtils::DecisionIDContainer ids;    
    TrigCompositeUtils::decisionIDs( l1Decisions->at( index ), ids );
    ATH_MSG_DEBUG( "L1 " << index << " N positive decisions " << ids.size()  );
    ATH_CHECK( fillPassEvents( ids, index + 1, m_passHistogram ));
    ATH_CHECK( fillRate( ids, index + 1) );
    if ( not ids.empty() ){
      m_passHistogram->Fill( 1, double(index + 1) );
      m_rateBufferHistogram->Fill( 1, double(index + 1) );
    }
    return StatusCode::SUCCESS;
  };

  ATH_CHECK( fillL1(0) );
  ATH_CHECK( fillL1(1) );

  int step = 0;
  for ( auto& ctool: m_collectorTools ) {
    std::vector<TrigCompositeUtils::DecisionID> stepSum;
    ctool->getDecisions( stepSum );
    ATH_MSG_DEBUG( " Step " << step << " decisions " << stepSum.size() );
    TrigCompositeUtils::DecisionIDContainer stepUniqueSum( stepSum.begin(), stepSum.end() );
    ATH_CHECK( fillPassEvents( stepUniqueSum, 3+step, m_passHistogram ) );
    ATH_CHECK( fillDecisionCount( stepSum, 3+step ) );
    ++step;
  }

  const int row = nBinsY();
  const int rateRow = nRateBinsY();
  auto finalDecisionsHandle = SG::makeHandle( m_finalDecisionKey, context );
  ATH_CHECK( finalDecisionsHandle.isValid() );
  TrigCompositeUtils::DecisionIDContainer finalIDs;
  for (const TrigCompositeUtils::Decision* decisionObject : *finalDecisionsHandle) {
    if (decisionObject->name() == "HLTPassRaw") {
      TrigCompositeUtils::decisionIDs(decisionObject, finalIDs);
      break;
    }
  }
  
  ATH_CHECK( fillStreamsAndGroups( m_streamToChainMap, finalIDs ) );
  ATH_CHECK( fillStreamsAndGroups( m_groupToChainMap, finalIDs ) );
  ATH_CHECK( fillPassEvents( finalIDs, row, m_passHistogram ) );
  ATH_CHECK( fillRate( finalIDs, rateRow ) );
  ATH_CHECK( fillBunchGroups( finalIDs ) );

  if ( not finalIDs.empty() ) {
    m_passHistogram->Fill( 1, double( row ) );
    m_rateBufferHistogram->Fill( 1, double( rateRow ) );
  }

  return StatusCode::SUCCESS;
}

int TrigSignatureMoniMT::nBinsX(SG::ReadHandle<TrigConf::HLTMenu>& hltMenuHandle) const {
  return hltMenuHandle->size() + m_groupToChainMap.size() + m_streamToChainMap.size() + 1;
}

int TrigSignatureMoniMT::nBunchBinsX(SG::ReadHandle<TrigConf::HLTMenu>& hltMenuHandle) const {
  return hltMenuHandle->size() + 1; //only chains
}

int TrigSignatureMoniMT::nBinsY() const {
  return m_collectorTools.size() + 3; // in, after ps, out
}

int TrigSignatureMoniMT::nRateBinsY() const {
  return 3; // in, after ps, out
}

int TrigSignatureMoniMT::nBunchBinsY(SG::ReadHandle<TrigConf::L1Menu>& l1MenuHandle) const {
  return l1MenuHandle->getObject("bunchGroups").getKeys().size() - 1;
}


StatusCode TrigSignatureMoniMT::initHist(std::unique_ptr<TH2>& hist, SG::ReadHandle<TrigConf::HLTMenu>& hltMenuHandle, bool steps) {
  TAxis* x = hist->GetXaxis();
  x->SetBinLabel(1, "All");
  int bin = 2; // 1 is for total count, (remember bins numbering in ROOT start from 1)

  std::vector<std::string> sortedChainsList;
  for ( const TrigConf::Chain& chain: *hltMenuHandle ) {
    sortedChainsList.push_back( chain.name() );
  }
  std::sort( sortedChainsList.begin(), sortedChainsList.end() );
  
  for ( auto chainName: sortedChainsList ) {
    x->SetBinLabel( bin, chainName.c_str() );
    m_chainIDToBinMap[ HLT::Identifier( chainName ).numeric() ] = bin;
    bin++;
  }

  for ( auto stream : m_streamToChainMap){
    x->SetBinLabel( bin, ("str_"+stream.first).c_str());
    m_nameToBinMap[ stream.first ] = bin;
    bin++;
  }

  for ( auto group : m_groupToChainMap){
    x->SetBinLabel( bin, ("grp_"+group.first.substr(group.first.find(':')+1)).c_str() );
    m_nameToBinMap[ group.first ] = bin;
    bin++;
  }


  TAxis* y = hist->GetYaxis();
  y->SetBinLabel( 1, steps ? "L1" : "Input" );
  y->SetBinLabel( 2, "AfterPS" );
  for ( size_t i = 0; steps && i < m_collectorTools.size(); ++i ) {
    y->SetBinLabel( 3+i, ("Step "+std::to_string(i)).c_str() );
  }
  y->SetBinLabel( y->GetNbins(), "Output" ); // last bin

  return StatusCode::SUCCESS;
}

StatusCode TrigSignatureMoniMT::initBunchHist(std::unique_ptr<TH2>& hist, SG::ReadHandle<TrigConf::HLTMenu>& hltMenuHandle, SG::ReadHandle<TrigConf::L1Menu>& l1MenuHandle) {
  TAxis* x = hist->GetXaxis();
  x->SetBinLabel(1, "All");
  int bin = 2;

  std::vector<std::string> sortedChainsList;
  for ( const TrigConf::Chain& chain: *hltMenuHandle ) {
    sortedChainsList.push_back( chain.name() );
  }
  std::sort( sortedChainsList.begin(), sortedChainsList.end() );

  for ( auto chainName: sortedChainsList ) {
    x->SetBinLabel( bin, chainName.c_str() );
    ++bin;
  }

  TAxis* y = hist->GetYaxis();
  std::vector<std::string> sortedBunchGroups = l1MenuHandle->getObject("bunchGroups").getKeys();
  std::sort( sortedBunchGroups.begin(), sortedBunchGroups.end() );
  sortedBunchGroups.erase(std::remove(sortedBunchGroups.begin(), sortedBunchGroups.end(), "BGRP0"), sortedBunchGroups.end());

  bin = 1;
  for ( auto group : sortedBunchGroups ){
    y->SetBinLabel( bin, l1MenuHandle->getAttribute( "bunchGroups." + group + ".name", true).c_str() );
    m_nameToBinMap[group] = bin;
    ++bin;
  }

  return StatusCode::SUCCESS;
}
