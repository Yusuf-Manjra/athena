/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MuonSmearer.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_PARSIMTOOLS_MUONSMEARER_H
#define ISF_PARSIMTOOLS_MUONSMEARER_H

// ISF
#include "ISF_ParSimInterfaces/IChargedSmearer.h"

// Gaudi
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/RndmGenerators.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ITHistSvc.h"

// ROOT
#include "TFile.h"
#include "TLorentzVector.h"
#include "TVectorD.h"
#include "TMatrixDSym.h"
#include "TVector3.h"

// CLHEP
#include "CLHEP/Random/RandomEngine.h"
#include "CLHEP/RandomObjects/RandMultiGauss.h"
#include "CLHEP/Matrix/Vector.h"
#include "CLHEP/Matrix/Matrix.h"

class TTree;

namespace ISF {
    class ISFParticle;
}

namespace iParSim {
      
  /** 
   @class MuonSmearer

   Creates xAOD::TrackParticles from ISFParticles
   this is a smearer for muons

   @author Miha.Muskinja -at- cern.ch 
   */
      
  class MuonSmearer : public extends<AthAlgTool, iParSim::IChargedSmearer>
  {
    public:

      /**Constructor */
      MuonSmearer(const std::string&,const std::string&,const IInterface*);
      
      /**Destructor*/
      virtual ~MuonSmearer();
      
      /** AlgTool initialize method.*/
      virtual StatusCode initialize() override;
      
      /** AlgTool finalize method */
      virtual StatusCode finalize() override;

      /** Smear the existing xAOD::TrackParticle */
      virtual
      bool smear(xAOD::TrackParticle* xaodTP, CLHEP::HepRandomEngine *randomEngine) const override;
      
      /** Return the pdg code of the smearer */
      virtual
      unsigned int pdg() const override;

    private:
      /**MC12 Muon Smearer files */
      std::map<unsigned int, std::string>  m_filenamesMC12MuonSmearer; //!< reordered in a map for look-up
      std::string m_filenameMC12MuonPtBins;
      std::string m_filenameMC12MuonEtaBins;

      bool getBinsFromFile(const std::string& ptFile, const std::string& etaFile);
      
      int getEtaBin(const double) const;
      
      int getPtBin(const double) const;

      std::vector<double>  m_pt_min;
      std::vector<double>  m_pt_max;
      std::vector<double>  m_eta_min;
      std::vector<double>  m_eta_max;
      
      /** Mode of the muon smearer: 1 for ID only and 2 for MS only */
      int m_para_code;
      
      /** Root file with the muon smearing parameters */
      TFile* m_file_para;

      /** Random Engines */
      std::unique_ptr<CLHEP::RandMultiGauss>  m_randMultiGauss;                 //!< MultiGauss Random
  };

} // end of namespace

#endif 

