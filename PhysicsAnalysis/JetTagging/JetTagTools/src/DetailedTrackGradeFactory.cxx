/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetTagTools/DetailedTrackGradeFactory.h"
#include "GaudiKernel/MsgStream.h"

#include "JetTagInfo/TrackGrade.h"

#include "JetTagInfo/TrackGradesDefinition.h"

#include "xAODTracking/TrackParticle.h"

namespace Analysis
{

DetailedTrackGradeFactory::DetailedTrackGradeFactory( const std::string& t, const std::string& n, const IInterface* p ) :
        AthAlgTool( t, n, p )
{
    declareProperty("hitBLayerGrade", m_hitBLayerGrade = false);

    declareProperty("useSharedHitInfo", m_useSharedHitInfo = false);
    declareProperty("useDetailSharedHitInfo",m_useDetailSharedHitInfo=false);

    declareProperty("nSharedBLayer", m_nSharedBLayer = 0);
    declareProperty("nSharedPix", m_nSharedPix = 0);
    declareProperty("nSharedSct", m_nSharedSct = 1);
    declareProperty("nSharedSi", m_nSharedSi = 999);

    // Flag to toggle track gradation according to Run2 pixel layout
    declareProperty("useRun2TrackGrading", m_useRun2TrackGrading = false);
    // And the properties for it:
    declareProperty("hitInnerLayersGrade", m_hitInnerLayersGrade = true);
    declareProperty("nSharedInnermostPixelLayer", m_nSharedInnermostPixelLayer = 0);
    declareProperty("nSharedNextToInnermostPixelLayer", m_nSharedNextToInnermostPixelLayer = 0);
    //
    declareProperty("ptFracGrade", m_ptFracGrade = false);
    declareProperty("ptFracCut", m_ptFracCut = 0.04);

    declareProperty("ptEtaGrades", m_ptEtaGrades = false);
    declareProperty("ptLowerCuts", m_ptLowerCuts);
    declareProperty("etaLowerCuts", m_etaLowerCuts);

    m_ptLowerCuts.push_back(1000.);
    m_ptLowerCuts.push_back(4000.);
    m_ptLowerCuts.push_back(10000.);
    
    m_etaLowerCuts.push_back(0.);
    m_etaLowerCuts.push_back(0.5);
    m_etaLowerCuts.push_back(1.5);

    declareInterface<ITrackGradeFactory>( this );
}

DetailedTrackGradeFactory::~DetailedTrackGradeFactory()
{}

StatusCode DetailedTrackGradeFactory::initialize()
{
  
  std::vector<TrackGrade> myGrades;
  int nbGrades=0;

  if (!m_useRun2TrackGrading){
    if (m_hitBLayerGrade)
      {
	myGrades.push_back(TrackGrade(nbGrades,std::string("0HitBLayer")));
	nbGrades++;
      }
    if (m_useSharedHitInfo)
      {
	myGrades.push_back(TrackGrade(nbGrades,std::string("Shared")));
	nbGrades++;
      }
    if(m_useDetailSharedHitInfo) //Add By Bai Yu Nov 28,2011
      {
	myGrades.push_back(TrackGrade(nbGrades,std::string("BlaShared")));
	nbGrades++;
	myGrades.push_back(TrackGrade(nbGrades,std::string("PixShared")));
	nbGrades++;
	myGrades.push_back(TrackGrade(nbGrades,std::string("SctShared")));
	nbGrades++;
      }
  }
  else if (m_useRun2TrackGrading){
    if (m_hitInnerLayersGrade)
      {
	myGrades.push_back(TrackGrade(nbGrades,std::string("0HitInnermostPixelLayer")));
	nbGrades++;
	myGrades.push_back(TrackGrade(nbGrades,std::string("0HitNextToInnermostPixelLayer")));
	nbGrades++;
	myGrades.push_back(TrackGrade(nbGrades,std::string("0Hit2InnermostPixelLayers")));
	nbGrades++;
      }
    if (m_useSharedHitInfo)
      {
	myGrades.push_back(TrackGrade(nbGrades,std::string("Shared")));
	nbGrades++;
      }
    if(m_useDetailSharedHitInfo)
      {
	myGrades.push_back(TrackGrade(nbGrades,std::string("InnermostShared")));
	nbGrades++;
	myGrades.push_back(TrackGrade(nbGrades,std::string("NextToInnermostShared")));
	nbGrades++;
	myGrades.push_back(TrackGrade(nbGrades,std::string("PixShared")));
	nbGrades++;
	myGrades.push_back(TrackGrade(nbGrades,std::string("SctShared")));
	nbGrades++;
      }
  }

  if (m_ptFracGrade)
    {
      myGrades.push_back(TrackGrade(nbGrades,std::string("PtFrac")));
      nbGrades++;
    }

  int ptGradeNumber=m_ptLowerCuts.size();
  int etaGradeNumber=m_etaLowerCuts.size();

  if (m_ptEtaGrades)
    {

      if (ptGradeNumber==0)
      {
	ATH_MSG_ERROR("Vector with pt categories has 0 size!!! You need at least 1 lower cut...");
      }

      if (etaGradeNumber==0)
      {
	ATH_MSG_ERROR("Vector with eta categories has 0 size!!! You need at least 1 lower cut...");
      }
      

      for (int i=0;i<ptGradeNumber;i++){
	for (int j=0;j<etaGradeNumber;j++){

          std::string nameGrade("Good");

          char namePt[30];
          if (i<ptGradeNumber-1)
          {
            sprintf(namePt,"Pt%.1f-%.1f",m_ptLowerCuts[i]*1e-3,m_ptLowerCuts[i+1]*1e-3);
          }
          else
          {
            sprintf(namePt,"Pt%.1f-inf",m_ptLowerCuts[i]*1e-3);
          }
          

          char nameEta[30];
          if (j<etaGradeNumber-1)
          {
            sprintf(nameEta,"Eta%.1f-%.1f",m_etaLowerCuts[j],m_etaLowerCuts[j+1]);
          }
          else
          {
            sprintf(nameEta,"Eta%.1f-2.5",m_etaLowerCuts[j]);
          }
  
          nameGrade+=namePt;
          nameGrade+=nameEta;
          
	  ATH_MSG_DEBUG("Adding cat. pt "<<i<<" eta "<<j<<" name of Grade: "<<nameGrade);

	  myGrades.push_back(TrackGrade(nbGrades,nameGrade));
	  nbGrades++;
	}
      }
    }
  else
    {
      myGrades.push_back(TrackGrade(nbGrades,std::string("Good")));
    }    

  m_trackGradesDefinition=TrackGradesDefinition(myGrades);



  return StatusCode::SUCCESS;
}

StatusCode DetailedTrackGradeFactory::finalize()
{
    return StatusCode::SUCCESS;
}


const TrackGradesDefinition & DetailedTrackGradeFactory::getTrackGradesDefinition() const
{
  return m_trackGradesDefinition;
}




TrackGrade* DetailedTrackGradeFactory::getGrade(const xAOD::TrackParticle & track,
						const xAOD::IParticle::FourMom_t & jetMomentum) const
{


  bool nohitBLayer(false);
  bool nohitInnermostLayer(false);
  bool nohitNextToInnermostLayer(false);

  bool sharedClass(false);  
  bool blasharedClass(false);
  bool innermostsharedClass(false);
  bool nexttoinnermostsharedClass(false);

  bool pixsharedClass(false);
  bool sctsharedClass(false);
  bool ptFrac(false);

  if (m_hitBLayerGrade)
  {
    //check if no hit in BLayer
    uint8_t nbs;
    if (!track.summaryValue(nbs, xAOD::numberOfBLayerHits)){
      ATH_MSG_ERROR("#BTAG# Cannot retrieve numberOfBLayerHists for TrackGrade!");
    }
    if(nbs < 0) nbs = 0;
    if (nbs==0) {
      nohitBLayer = true;
    }
  }
  
  if (m_useSharedHitInfo)
  {
    if (!m_useRun2TrackGrading){
      //check if shared
      uint8_t nbs, nps, nss;
      if (!track.summaryValue(nbs, xAOD::numberOfBLayerSharedHits)){
	ATH_MSG_ERROR("#BTAG# Cannot retrieve numberOfBLayerSharedHists for TrackGrade!");
      }
      if(nbs < 0) nbs = 0;

      if (!track.summaryValue(nps, xAOD::numberOfPixelSharedHits)){
	ATH_MSG_ERROR("#BTAG# Cannot retrieve numberOfPixelSharedHists for TrackGrade!");
      }
      if(nps < 0) nps = 0;

      if (!track.summaryValue(nss, xAOD::numberOfSCTSharedHits)){
	ATH_MSG_ERROR("#BTAG# Cannot retrieve numberOfSCTSharedHists for TrackGrade!");
      }
      if(nss < 0) nss = 0;
      int nsht =  nps+nss ;
      if(nbs>m_nSharedBLayer) {
	sharedClass = true;
	blasharedClass = true;
      }
      if(nps>m_nSharedPix) {
	sharedClass = true;
	pixsharedClass = true;
      }
      if(nss>m_nSharedSct) {
	sharedClass = true;
	sctsharedClass = true;
      }
      if(nsht>m_nSharedSi) {
	sharedClass = true;
      }
    }
    else if (m_useRun2TrackGrading){
      uint8_t nips, nnips, nps, nss;
      if (!track.summaryValue(nips, xAOD::numberOfInnermostPixelLayerSharedHits)){
	ATH_MSG_ERROR("#BTAG# Cannot retrieve numberOfInnermostPixelLayerSharedHits for TrackGrade!");
      }
      if(nips < 0) nips = 0;

      if (!track.summaryValue(nnips, xAOD::numberOfNextToInnermostPixelLayerSharedHits)){
	ATH_MSG_ERROR("#BTAG# Cannot retrieve numberOfNextToInnermostPixelLayerSharedHits for TrackGrade!");
      }
      if(nnips < 0) nnips = 0;
      if (!track.summaryValue(nps, xAOD::numberOfPixelSharedHits)){
	ATH_MSG_ERROR("#BTAG# Cannot retrieve numberOfPixelSharedHits for TrackGrade!");
      }
      if(nps < 0) nps = 0;

      if (!track.summaryValue(nss, xAOD::numberOfSCTSharedHits)){
	ATH_MSG_ERROR("#BTAG# Cannot retrieve numberOfSCTSharedHits for TrackGrade!");
      }
      if(nss < 0) nss = 0;
      int nsht =  nps+nss ;
      if(nips>m_nSharedInnermostPixelLayer) {
	sharedClass = true;
	innermostsharedClass = true;
      }
      if(nnips>m_nSharedNextToInnermostPixelLayer) {
	sharedClass = true;
	nexttoinnermostsharedClass = true;
      }
      if(nps>m_nSharedPix) {
	sharedClass = true;
	pixsharedClass = true;
      }
      if(nss>m_nSharedSct) {
	sharedClass = true;
	sctsharedClass = true;
      }
      if(nsht>m_nSharedSi) {
	sharedClass = true;
      }
    }
  }


  double ptTrack = track.pt();  
  double etaTrack = fabs(track.eta());

  //check if ptFrac < cut value  
  if (m_ptFracGrade)
  {

    double ptJet = jetMomentum.Pt();
    if ((ptTrack/ptJet) < m_ptFracCut) ptFrac = true;
  }

  
  std::string ptEtaCategoryName;

  if (m_ptEtaGrades)
  {

    int ptGradeNumber=m_ptLowerCuts.size();
    int etaGradeNumber=m_etaLowerCuts.size();
  
    //check pt
    int ptBin(-1);

    for (int i=0;i<ptGradeNumber;i++){

      if (i<ptGradeNumber-1)
      {
        if (ptTrack>m_ptLowerCuts[i]&& ptTrack<=m_ptLowerCuts[i+1])
        {
          ptBin=i;
          break;
        }
      }
      else
      {
        if (ptTrack>m_ptLowerCuts[i])
        {
          ptBin=i;
          break;
        }
      }
    }

    
    int etaBin(-1);

    for (int i=0;i<etaGradeNumber;i++){

      if (i<etaGradeNumber-1)
      {
        if (etaTrack>m_etaLowerCuts[i]&& etaTrack<=m_etaLowerCuts[i+1])
        {
          etaBin=i;
          break;
        }
      }
      else
      {
        if (etaTrack>m_etaLowerCuts[i])
        {
          etaBin=i;
          break;
        }
      }
    }
    
    //now choose the right grade

    std::string nameGrade("Good");

    char namePt[30];
    if (ptBin<ptGradeNumber-1)
    {
      sprintf(namePt,"Pt%.1f-%.1f",m_ptLowerCuts[ptBin]*1e-3,m_ptLowerCuts[ptBin+1]*1e-3);
    }
    else
    {
      sprintf(namePt,"Pt%.1f-inf",m_ptLowerCuts[ptBin]*1e-3);
    }

    char nameEta[30];
    if (etaBin<etaGradeNumber-1)
    {
      sprintf(nameEta,"Eta%.1f-%.1f",m_etaLowerCuts[etaBin],m_etaLowerCuts[etaBin+1]);
    }
    else
    {
      sprintf(nameEta,"Eta%.1f-2.5",m_etaLowerCuts[etaBin]);
    }
  
    nameGrade+=namePt;
    nameGrade+=nameEta;
    
    ptEtaCategoryName=nameGrade;
    
  }
  
  
  const TrackGrade * gradeToReturn(0);

  if (!m_useRun2TrackGrading){
    if (nohitBLayer)
      {
	gradeToReturn=m_trackGradesDefinition.getGrade(std::string("0HitBLayer"));
      }
    else if (sharedClass)
      {
	if(!m_useDetailSharedHitInfo)
	  {
	    gradeToReturn=m_trackGradesDefinition.getGrade(std::string("Shared"));
	    //gradeToReturn=m_trackGradesDefinition.getGrade(1);
	  }
	else
	  {
	    if(blasharedClass)
	      {
		gradeToReturn=m_trackGradesDefinition.getGrade(std::string("BlaShared"));
	      }
	    if(pixsharedClass&&(!blasharedClass))
	      {
		gradeToReturn=m_trackGradesDefinition.getGrade(std::string("PixShared"));
	      } 
	    if(sctsharedClass)
	      {
		gradeToReturn=m_trackGradesDefinition.getGrade(std::string("SctShared"));
	      }
	  }
      }
    else if (ptFrac)
      {
	gradeToReturn=m_trackGradesDefinition.getGrade(std::string("PtFrac"));
      }
    else
      {
	if (m_ptEtaGrades)
	  {
	    ATH_MSG_VERBOSE(" category "<<ptEtaCategoryName);
	    gradeToReturn=m_trackGradesDefinition.getGrade(ptEtaCategoryName);
	  }
	else
	  {
	    gradeToReturn=m_trackGradesDefinition.getGrade(std::string("Good"));
	  }
      }
  }
  else if (m_useRun2TrackGrading){
    if (nohitInnermostLayer)
      {
	if (nohitNextToInnermostLayer)
	  {
	    gradeToReturn=m_trackGradesDefinition.getGrade(std::string("0Hit2InnermostPixelLayers"));
	  }
	else
	  {
	    gradeToReturn=m_trackGradesDefinition.getGrade(std::string("0HitInnermostPixelLayer"));
	  }
      }
    else if (nohitNextToInnermostLayer)
      {
	gradeToReturn=m_trackGradesDefinition.getGrade(std::string("0HitNextToInnermostPixelLayer"));
      }
    else if (sharedClass)
      {
	if(!m_useDetailSharedHitInfo)
	  {
	    gradeToReturn=m_trackGradesDefinition.getGrade(std::string("Shared"));
	  }
	else
	  {
	    if(innermostsharedClass)
	      {
		gradeToReturn=m_trackGradesDefinition.getGrade(std::string("InnermostShared"));
	      }
	    else if(nexttoinnermostsharedClass)
	      {
		gradeToReturn=m_trackGradesDefinition.getGrade(std::string("NextToInnermostShared"));
	      }
	    else if(pixsharedClass)
	      {
		gradeToReturn=m_trackGradesDefinition.getGrade(std::string("PixShared"));
	      }
	    if (sctsharedClass)
	      {
		gradeToReturn=m_trackGradesDefinition.getGrade(std::string("SctShared"));
	      }
	  }
      }
    else if (ptFrac)
      {
	gradeToReturn=m_trackGradesDefinition.getGrade(std::string("PtFrac"));
      }
    else
      {
	if (m_ptEtaGrades)
	  {
	    ATH_MSG_VERBOSE(" category "<<ptEtaCategoryName);
	    gradeToReturn=m_trackGradesDefinition.getGrade(ptEtaCategoryName);
	  }
	else
	  {
	    gradeToReturn=m_trackGradesDefinition.getGrade(std::string("Good"));
	  }
      }
  }
      
  if (gradeToReturn==0)
  {
    ATH_MSG_ERROR("Grade not found. Internal error... ");

  }

  return new TrackGrade(*gradeToReturn);
  
}
 
}

