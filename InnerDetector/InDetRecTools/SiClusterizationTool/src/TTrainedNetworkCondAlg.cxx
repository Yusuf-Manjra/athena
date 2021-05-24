/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
/*
 *   */

#include "TTrainedNetworkCondAlg.h"

#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "FileCatalog/IFileCatalog.h"

#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TObject.h"

// for error messages
#include <typeinfo>

namespace InDet {

  TTrainedNetworkCondAlg::TTrainedNetworkCondAlg (const std::string& name, ISvcLocator* pSvcLocator)
    : ::AthReentrantAlgorithm( name, pSvcLocator )
  {}

  StatusCode TTrainedNetworkCondAlg::initialize() {
    ATH_CHECK( m_condSvc.retrieve() );
    ATH_CHECK( m_poolsvc.retrieve() );

    // Condition Handles
    ATH_CHECK( m_readKey.initialize() );
    ATH_CHECK( m_writeKey.initialize() );

    // Register write handle
    if (m_condSvc->regHandle(this, m_writeKey).isFailure()) {
      ATH_MSG_ERROR("Unable to register WriteCondHandle " << m_writeKey.fullKey() << " with CondSvc");
      return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
  }

  StatusCode TTrainedNetworkCondAlg::finalize()
  {
    return StatusCode::SUCCESS;
  }

  namespace {
    template <class T> T *getObject(TFile &a_file, const std::string &path) {
      std::unique_ptr<TObject> obj_raw(a_file.Get(path.c_str()));
      T  *obj = dynamic_cast<T *>(obj_raw.get());
      if (!obj) {
        std::stringstream msg;
        if (obj_raw != nullptr) {
          msg << "Failed object "  << path << " from File " << a_file.GetName()
              << " is not of type " << typeid(T).name() << " but " << typeid(obj_raw).name() << ".";
        }
        else {
          msg << "Failed to get object "  << path << " from File " << a_file.GetName();
        }
        throw std::runtime_error( msg.str() );
      }
      obj_raw.release();
      return obj;
    }
  }

  TTrainedNetwork* TTrainedNetworkCondAlg::retrieveNetwork(TFile &input_file, const std::string& folder) const
  {
    std::vector<TH1*> retrievedHistos;
    const unsigned int layer_info = retrievedHistos.size();
    // the information about the layers
    retrievedHistos.push_back( getObject<TH1>(input_file, folder+m_layerInfoHistogram.value()) );

    if(!m_getInputsInfo){
      // the info about the input nodes 
      retrievedHistos.push_back( getObject<TH2>(input_file, folder+"InputsInfo") );
    }

    // retrieve the number of hidden layers from the LayerInfo histogram
    unsigned int  n_hidden = retrievedHistos.at(layer_info)->GetNbinsX()-2;
    ATH_MSG_VERBOSE(" Retrieving calibration: " << folder  << " for NN with: " << n_hidden << " hidden layers.");

    retrievedHistos.reserve( retrievedHistos.size() + n_hidden*2 );
    for (unsigned int  i=0; i<=n_hidden; ++i) {
      std::stringstream folder_name; 
      folder_name << folder <<  m_layerPrefix.value() << i;
      retrievedHistos.push_back( getObject<TH2>(input_file, folder_name.str()+m_weightIndicator.value() ) );
      retrievedHistos.push_back( getObject<TH1>(input_file, folder_name.str()+m_thresholdIndicator.value() ) );
    }

    std::unique_ptr<TTrainedNetwork> a_nn(m_networkToHistoTool->fromHistoToTrainedNetwork(retrievedHistos));
    if (!a_nn) {
      ATH_MSG_ERROR( "Failed to create NN from " << retrievedHistos.size() << " histograms read from " << folder);
    }
    else {
      ATH_MSG_VERBOSE(   folder << " " <<  a_nn->getnInput()     );
    }

    return a_nn.release();
  }


  StatusCode TTrainedNetworkCondAlg::execute(const EventContext& ctx) const {

    SG::WriteCondHandle<TTrainedNetworkCollection > NnWriteHandle{m_writeKey, ctx};
    if (NnWriteHandle.isValid()) {
      ATH_MSG_DEBUG("Write CondHandle "<< NnWriteHandle.fullKey() << " is already valid");
      return StatusCode::SUCCESS;
    }

    SG::ReadCondHandle<CondAttrListCollection> readHandle{m_readKey, ctx};
    if(!readHandle.isValid()) {
      ATH_MSG_ERROR("Invalid read handle " << m_readKey.key());
      return StatusCode::FAILURE;
    }
    const CondAttrListCollection* atrcol{*readHandle};
    assert( atrcol != nullptr);

    EventIDRange cdo_iov;
    if(!readHandle.range(cdo_iov)) {
      ATH_MSG_ERROR("Failed to get valid validaty range from  " << readHandle.key());
      return StatusCode::FAILURE;
    }

    unsigned int channel=1; //Always 1 in old version with CoolHistSvc
    CondAttrListCollection::const_iterator channel_iter = atrcol->chanAttrListPair(channel);
    if (channel_iter==atrcol->end()) {
      ATH_MSG_ERROR("Conditions data " << readHandle.key() << " misses channel " << channel);
      return StatusCode::FAILURE;
    }

    // @TODO store NN parameters in a different way than as a set of histograms in a root file.
    const std::string coolguid=channel_iter->second["fileGUID"].data<std::string>();

    std::unique_ptr<TTrainedNetworkCollection> writeCdo{std::make_unique<TTrainedNetworkCollection>()};
    {
      std::string pfname;
      std::string tech;
      m_poolsvc->catalog()->getFirstPFN(coolguid, pfname, tech );
      ATH_MSG_VERBOSE("Get NNs from file " << pfname.c_str() << " [" << coolguid << " <- " << readHandle.key() << "]." );
      std::unique_ptr<TFile> a_file( TFile::Open(pfname.c_str(),"READ") );
      if (!a_file || !a_file->IsOpen()) {
        ATH_MSG_ERROR("Failed to open  file " << pfname << " referenced by " << readHandle.key() << " GUID " << coolguid);
        return StatusCode::FAILURE;
      }

      writeCdo->reserve(m_nnOrder.size());
      for (const std::string &folder: m_nnOrder) {
	ATH_MSG_VERBOSE( "Retrieve NN " << writeCdo->size() << ": " << folder );
        writeCdo->push_back( std::unique_ptr<TTrainedNetwork>( retrieveNetwork(*a_file, folder) ) );
      }
      writeCdo->setNames(m_nnOrder);
    }

    if(NnWriteHandle.record(cdo_iov,std::move(writeCdo)).isFailure()) {
      ATH_MSG_ERROR("Failed to record Trained network collection to " 
                    << NnWriteHandle.key()
                    << " with IOV " << cdo_iov );
      return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;
  }

}
