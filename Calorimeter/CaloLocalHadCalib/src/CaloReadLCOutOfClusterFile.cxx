/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloLocalHadCalib/CaloReadLCOutOfClusterFile.h"
#include "CaloConditions/CaloLocalHadDefs.h"
#include "GaudiKernel/MsgStream.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "StoreGate/StoreGateSvc.h"

#include "PathResolver/PathResolver.h"
#include "TFile.h"
#include "TAxis.h"
#include "TProfile2D.h"
#include <iostream>
#include <sstream>


CaloReadLCOutOfClusterFile::CaloReadLCOutOfClusterFile(const std::string & name, 
						       ISvcLocator * pSvcLocator) : 
  AthAlgorithm(name,pSvcLocator) {
  declareProperty("LCOutOfClusterFileName",m_LCOutOfClusterFileName);
  declareProperty("CorrectionKey",m_key="OOC");

  m_data = new CaloLocalHadCoeff();
}


CaloReadLCOutOfClusterFile::~CaloReadLCOutOfClusterFile() {}


StatusCode CaloReadLCOutOfClusterFile::initDataFromFile(std::string theLCOutOfClusterFileName)
{
  MsgStream log(msgSvc(), name());
  
  // Find the full path to filename:
  std::string file = PathResolver::find_file (theLCOutOfClusterFileName, "DATAPATH");
  log << MSG::INFO << "Reading file  " << file << endreq;
  TFile* theLCOutOfClusterFile = new TFile(file.c_str());
  if ( !theLCOutOfClusterFile ) {
    return StatusCode::FAILURE;
  }
  
  CaloLocalHadCoeff::LocalHadArea theArea("OutOfCluster",0,3);

  TList * theKeyList = theLCOutOfClusterFile->GetListOfKeys();
  while ( theKeyList->GetEntries() ) {
    TProfile2D * prof = (TProfile2D *)theLCOutOfClusterFile->Get(theKeyList->First()->GetTitle());
    // parse histogram title to find all dimensions and bins
    std::string sTitle(prof->GetTitle());
    bool allValid(true);

    std::vector<std::string> keys;
    keys.push_back(std::string("_iside_"));
    keys.push_back(std::string("_iphi_"));
    keys.push_back(std::string("_ilogE_"));
	  
    std::vector<std::string> names;
    names.push_back(std::string("side"));
    names.push_back(std::string("phi"));
    names.push_back(std::string("log10(E_clus (MeV))"));
    names.push_back(std::string(prof->GetXaxis()->GetTitle()));
    names.push_back(std::string(prof->GetYaxis()->GetTitle()));
	  
    std::vector<int> types;
    types.push_back(CaloLocalHadDefs::DIM_EQUI);	  
    types.push_back(CaloLocalHadDefs::DIM_EQUI);	  
    types.push_back(CaloLocalHadDefs::DIM_LOG|CaloLocalHadDefs::DIM_EQUI);	  
    types.push_back(CaloLocalHadDefs::DIM_EQUI);	  
    types.push_back(CaloLocalHadDefs::DIM_LOG|CaloLocalHadDefs::DIM_EQUI);	  
    
    std::vector<int> ibin(names.size(),-1);
    std::vector<double> rmin(names.size(),-1);
    std::vector<double> rmax(names.size(),-1);
    std::vector<int> nbin(names.size(),-1);
    unsigned int idim;
	  
    for (idim=0;idim<keys.size();idim++) {
      size_t found = sTitle.find(keys[idim]);
      if ( found == std::string::npos ) {
	log << MSG::ERROR << "Could not find key " << keys[idim] << " in current histogram." << endreq;
	allValid = false;
      }
      else {
	char c;
	std::istringstream tstr(sTitle.substr(found+keys[idim].length()));
	tstr >> ibin[idim] >> c >> c >> rmin[idim] >> c >> rmax[idim] >> c >> nbin[idim];
	if ( ibin[idim] < 0 || ibin[idim] >= nbin[idim] ) {
	  log << MSG::ERROR << "Found invalid bin number " << ibin[idim] << " not in valid range [0," << nbin[idim] << " in current histogram." << endreq;
	  allValid = false;
	}
      }
    }
    if ( allValid ) {
      // final 2 dimensions are from TProfile2D itself
      nbin[names.size()-2] = prof->GetNbinsX();
      rmin[names.size()-2] = prof->GetXaxis()->GetXmin();
      rmax[names.size()-2] = prof->GetXaxis()->GetXmax();
      nbin[names.size()-1] = prof->GetNbinsY();
      rmin[names.size()-1] = prof->GetYaxis()->GetXmin();
      rmax[names.size()-1] = prof->GetYaxis()->GetXmax();
      // book new area from the first histo found 
      if ( theArea.getNdim() == 0 ) {
	for (idim = 0;idim<names.size();idim++) {
	  CaloLocalHadCoeff::LocalHadDimension theDim(names[idim].c_str(),types[idim],nbin[idim],rmin[idim],rmax[idim]);
          log << MSG::INFO << "adding dimension " << names[idim].c_str() << " " << types[idim]<< " " << nbin[idim]<< " " << rmin[idim]<< " " << rmax[idim] << endreq;
	  theArea.addDimension(theDim);
	}
	log << MSG::INFO << "adding Area with nDim = " << theArea.getNdim() << endreq;
	m_data->addArea(theArea);
      }
      // now fill all data for current histogram
      TAxis * xax = prof->GetXaxis();
      TAxis * yax = prof->GetYaxis();
      for (ibin[names.size()-2]=0;ibin[names.size()-2]<prof->GetNbinsX();ibin[names.size()-2]++) {
	for (ibin[names.size()-1]=0;ibin[names.size()-1]<prof->GetNbinsY();ibin[names.size()-1]++) {
	  float abseta = xax->GetBinCenter(ibin[names.size()-2]+1);
	  float loglambda = yax->GetBinCenter(ibin[names.size()-1]+1);
	  int iBin = prof->FindBin(abseta,loglambda);
		  
	  CaloLocalHadCoeff::LocalHadCoeff theData(3,0);
	  theData[CaloLocalHadDefs::BIN_WEIGHT]  = prof->GetBinContent(iBin);
	  theData[CaloLocalHadDefs::BIN_ENTRIES] = prof->GetBinEntries(iBin);
	  theData[CaloLocalHadDefs::BIN_ERROR]   = prof->GetBinError(iBin);
		  
	  log << MSG::INFO << "Now set data for bins: ";
	  for(unsigned int ii=0;ii<ibin.size();ii++)
	    log << ibin[ii] << " ";
	  log << endreq;
	  m_data->setCoeff(m_data->getBin(0,ibin),theData);
	}
      }
    }
    theKeyList->RemoveFirst();
  }

  return StatusCode::SUCCESS;
}

StatusCode CaloReadLCOutOfClusterFile::initialize() {
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << " Building CaloLocalHadCoeff object " << endreq;
  StatusCode sc;
  StoreGateSvc* detStore;
  sc=service("DetectorStore",detStore);
  if (sc.isFailure()) {
     log << MSG::ERROR << "Unable to get the DetectorStore" << endreq;
     return sc;
  }
  sc=initDataFromFile(m_LCOutOfClusterFileName);
  if (sc.isFailure()) {
     log << MSG::ERROR << "Unable to read input Data File" << endreq;
     return sc;
  }
  sc=detStore->record(m_data,m_key);
  if (sc.isFailure()) {
    log << MSG::ERROR << "Unable to record CaloLocalHadCoeff" << endreq;
    return sc;
  }
  sc=detStore->setConst(m_data);
  if (sc.isFailure()) {
    log << MSG::ERROR << "Unable to lock CaloLocalHadCoeff" << endreq;
    return sc;
  }
  return StatusCode::SUCCESS;
}

StatusCode  CaloReadLCOutOfClusterFile::execute() 
{ 
  return StatusCode::SUCCESS;
}

StatusCode  CaloReadLCOutOfClusterFile::finalize() 
{ 
  return StatusCode::SUCCESS;
}
