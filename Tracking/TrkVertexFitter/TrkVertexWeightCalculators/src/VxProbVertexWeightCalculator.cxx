/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkVertexWeightCalculators/VxProbVertexWeightCalculator.h"
#include "VxVertex/VxCandidate.h"
#include "VxVertex/VxTrackAtVertex.h"
#include "TrkParameters/TrackParameters.h"
#include "GaudiKernel/ITHistSvc.h"
#include <TH1.h>
#include <TMath.h>

#include "xAODTracking/Vertex.h"
#include "xAODTracking/TrackParticle.h"
 
 namespace Trk{
 
   StatusCode VxProbVertexWeightCalculator::initialize()
   {
     StatusCode sc = AthAlgTool::initialize();
  
     //initializing the AlgTool itself
     if(sc.isFailure())
     {
       msg(MSG::ERROR)<<" Unable to initialize the AlgTool"<<endreq;
       return StatusCode::FAILURE;
     }
     
     if (!service("THistSvc",m_iTHistSvc , true).isSuccess()) 
     {
       msg(MSG::ERROR)<< "Unable to locate THistSvc" << endreq;
       return StatusCode::FAILURE;
     }
     sc = m_iTHistSvc->regHist(m_HistoFileLocation); 
     
     if (sc.isFailure())
     {
       
       msg(MSG::ERROR) << "Unable to locate THistSvc" << endreq;
       return StatusCode::FAILURE;
     }
    
     sc = m_iTHistSvc->getHist(m_HistoFileLocation,m_hMinBiasPt );
     if (sc.isFailure()) 
     {
       msg(MSG::ERROR) << "Unable to locate THistSvc" << endreq;
       return StatusCode::FAILURE;
     }
    
     msg(MSG::ERROR) <<"Initialization successfull"<<endreq;
     return StatusCode::SUCCESS;
  }//end of initialize method
 
 
  StatusCode VxProbVertexWeightCalculator::finalize()
  {
    msg(MSG::ERROR) << "Finalize successful" << endreq;
    return StatusCode::SUCCESS;
  }
 
   //class constructor implementation
   VxProbVertexWeightCalculator::VxProbVertexWeightCalculator(const std::string& t, const std::string& n, const IInterface*  p):
           AthAlgTool(t,n,p),
           m_hMinBiasPt(0),
           m_HistoFileLocation("/VxProbHisto/h_sTrkPdfminBias"),
           m_iTHistSvc()
   {
     declareProperty("HistogramPath",m_HistoFileLocation );
     declareInterface<IVertexWeightCalculator>(this);
   }
   
   //destructor
   VxProbVertexWeightCalculator::~VxProbVertexWeightCalculator(){}
   
   double  VxProbVertexWeightCalculator::estimateSignalCompatibility(const VxCandidate& vertex)
   { 
     const std::vector<Trk::VxTrackAtVertex*>* tracks = vertex.vxTrackAtVertex();
     std::vector<Trk::VxTrackAtVertex*>::const_iterator begintracks=tracks->begin();
     std::vector<Trk::VxTrackAtVertex*>::const_iterator endtracks=tracks->end();

     double P0 = 1.;
     bool pTtooBig = false;
     
     for(std::vector<Trk::VxTrackAtVertex*>::const_iterator i = begintracks; i!=endtracks; i++) {
       
       const Trk::TrackParameters* perigee(0);
       if ((*i)->perigeeAtVertex()!=0) 
       {
         perigee=(*i)->perigeeAtVertex();
       } 
       else 
       {
         perigee=(*i)->initialPerigee();
       }     
       double p_T = std::fabs(1./perigee->parameters()[Trk::qOverP])*sin(perigee->parameters()[Trk::theta])/1000.;
       
       TH1F* myHisto = dynamic_cast<TH1F*>(m_hMinBiasPt);

       if (myHisto == 0)
       {
         msg(MSG::ERROR) << "VxProbHisto is an empty pointer!!!" << endreq;
         return 0;
         
       }
       
       double IntPt = (myHisto->Integral(myHisto->FindBin(p_T),myHisto->GetNbinsX() + 1)
                    + myHisto->Integral(myHisto->FindBin(p_T)+1,myHisto->GetNbinsX() + 1))/2.;
       //if (IntPt == 0)
       //      pTtooBig=true;
       // --- Markus Elsing: fake high pt tracks, limit the integral
       if (IntPt < 0.0001)
	 IntPt = 0.0001;
         
       P0 = P0 *IntPt;
     }

     double VxProb = 0.;
    
     if (tracks->size()>0)
     {
        
       if (pTtooBig)
           VxProb = 1e99;
       else
       {
         for (int j = 0; j < (int)tracks->size(); j++)
         {
           VxProb = VxProb + (TMath::Power(-TMath::Log(P0),j)/TMath::Factorial(j));
         }
         VxProb = -TMath::Log(VxProb*P0);
         
       }
       
     }
     return  VxProb;
   }


   double  VxProbVertexWeightCalculator::estimateSignalCompatibility(const xAOD::Vertex& vertex) 
   {
     double P0 = 1.;
     bool pTtooBig = false;

    for(const auto& elTrackParticle : vertex.trackParticleLinks()) {
      
      if (not elTrackParticle.isValid()) {
	ATH_MSG_WARNING("No valid link to tracks in xAOD::Vertex object. Skipping track for signal compatibility (may be serious).");
	continue;
      }

      const Trk::Perigee& perigee = (*elTrackParticle.cptr())->perigeeParameters();
     
      double p_T = std::fabs(1./perigee.parameters()[Trk::qOverP])*sin(perigee.parameters()[Trk::theta])/1000.;
       
      TH1F* myHisto = dynamic_cast<TH1F*>(m_hMinBiasPt);

      if (myHisto == 0)	{
	ATH_MSG_WARNING("VxProbHisto is an empty pointer. Signal compatibility failed and returning 0.0");
         return 0;         
      }
       
      double IntPt = (myHisto->Integral(myHisto->FindBin(p_T),myHisto->GetNbinsX() + 1)
		      + myHisto->Integral(myHisto->FindBin(p_T)+1,myHisto->GetNbinsX() + 1))/2.;
      // --- Markus Elsing: fake high pt tracks, limit the integral
      if (IntPt < 0.0001)
	IntPt = 0.0001;
         
      P0 = P0 *IntPt;
    }

    double VxProb = 0.;
    
    int sizeOfTPColl = vertex.trackParticleLinks().size();
    if (sizeOfTPColl > 0) {        
      if (pTtooBig)
	VxProb = 1e99;
      else {
	for (int j = 0; j < sizeOfTPColl; j++) {
	  VxProb = VxProb + (TMath::Power(-TMath::Log(P0),j)/TMath::Factorial(j));
	}
	VxProb = -TMath::Log(VxProb*P0);        
      }       
    }
    return  VxProb;
   }

 }//Namespace

