/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonRecValidationNtuples/MuonInsideOutValidationNtuple.h"


namespace Muon {

  void MuonValidationBlockBase::clear() {
    
    // clear entries
    for( auto entry : intBlocks ) entry.first.get()->clear();
    for( auto entry : floatBlocks ) entry.first.get()->clear();
    for( auto entry : subBlocks )   entry.first->clear();

  }

  MuonValidationBlockBase::~MuonValidationBlockBase() {
    // delete entries
    //for( auto entry : intBlocks )   delete entry.first.get();
    //for( auto entry : floatBlocks ) delete entry.first.get();
  }


  void MuonValidationBlockBase::init( std::string prefix, TTree* tree, bool write ) {
    for( auto entry : intBlocks ) {
      if( tree ){
        if( write ){
          entry.first.get() = new std::vector<int>();
          tree->Branch( (prefix + entry.second).c_str(),           entry.first.get());
        }else{
          tree->SetBranchAddress( (prefix + entry.second).c_str(), &entry.first.get());
        }
      }
    }

    for( auto entry : floatBlocks ) {
      if( tree ){
        if( write ){
          entry.first.get() = new std::vector<float>();
          tree->Branch( (prefix + entry.second).c_str(),           entry.first.get());
        }else{
          tree->SetBranchAddress( (prefix + entry.second).c_str(), &entry.first.get());
        }
      }
    }

    for( auto entry : subBlocks ) {
      entry.first->init(prefix + entry.second, tree, write );
    }

  }

  MuonValidationTrackParticleBlock::MuonValidationTrackParticleBlock() {
    addBlock(pt,"pt");
    addBlock(p,"p");
    addBlock(eta,"eta");
    addBlock(phi,"phi");
    addBlock(&truth,"");
  }

  MuonValidationTimeBlock::MuonValidationTimeBlock() {
    addBlock(&id,"");
    addBlock(&track,"");
    addBlock(&truth,"");
    addBlock(type,"type");
    addBlock(gasgapId,"gasgapId");
    addBlock(r,"r");
    addBlock(z,"z");
    addBlock(d,"d");
    addBlock(time,"time");
    addBlock(err,"err");
    addBlock(timeProp,"timeProp");
    addBlock(avTimeProp,"avTimeProp");
    addBlock(tof,"tof");
    addBlock(avTof,"avTof");
    addBlock(timeCor,"timeCor");
  }


  MuonValidationResidualBlock::MuonValidationResidualBlock() {
    addBlock(residual,"res");
    addBlock(pull,"pull");
    addBlock(pos,"pos");
    addBlock(err,"err");
    addBlock(expos,"expos");
    addBlock(expos_err,"expos_err");
    addBlock(expos_errstatus,"expos_errstatus");
  }

  MuonValidationIdBlock::MuonValidationIdBlock() {
    addBlock(sector,"sector");
    addBlock(chIndex,"chIndex");
  }

  MuonValidationTrackBlock::MuonValidationTrackBlock() {
    addBlock(trkid,"trkid");
  }

  MuonValidationTruthBlock::MuonValidationTruthBlock() {
    addBlock(pdg,"pdg");
    addBlock(barcode,"barcode");
  }

  MuonValidationSegmentBlock::MuonValidationSegmentBlock() {
    addBlock(stage,"stage");
    addBlock(quality,"quality");
    addBlock(nmdtHits,"nmdtHits");
    addBlock(ntrigEtaHits,"ntrigEtaHits");
    addBlock(ntrigPhiHits,"ntrigPhiHits");
    addBlock(r,"r");
    addBlock(z,"z");
    addBlock(t0,"t0");
    addBlock(t0Error,"t0Error");
    addBlock(t0Trig,"t0Trig");
    addBlock(t0TrigError,"t0TrigError");
    addBlock(&id,"");
    addBlock(&track,"");
    addBlock(&truth,"");
    addBlock(&xresiduals,"x");
    addBlock(&yresiduals,"y");
    addBlock(&angleXZ,"angleXZ");
    addBlock(&angleYZ,"angleYZ");
    addBlock(&combinedYZ,"combYZ");
  }

  MuonValidationHoughBlock::MuonValidationHoughBlock() {
    addBlock(maximum,"max");
    addBlock(&residuals,"");
    addBlock(&id,"");
    addBlock(&track,"");
    addBlock(&truth,"");
  }


  MuonValidationHitBlock::MuonValidationHitBlock() {
    addBlock(&id,"");
    addBlock(&track,"");
    addBlock(&truth,"");
    addBlock(&residuals,"");
  }

  MuonInsideOutValidationNtuple::MuonInsideOutValidationNtuple() {
    addBlock(&trackParticleBlock,"trk_");
    addBlock(&segmentBlock,"seg_");
    addBlock(&houghBlock,"hou_");
    addBlock(&hitBlock,"hit_");
    addBlock(&timeBlock,"time_");
  }

}
