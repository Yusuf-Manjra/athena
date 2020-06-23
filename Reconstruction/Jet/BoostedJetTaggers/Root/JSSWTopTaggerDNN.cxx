/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "BoostedJetTaggers/JSSWTopTaggerDNN.h"

#include "PathResolver/PathResolver.h"

#include "TEnv.h"
#include "TF1.h"
#include "TSystem.h"

#define APP_NAME "JSSWTopTaggerDNN"

JSSWTopTaggerDNN::JSSWTopTaggerDNN( const std::string& name ) :
  JSSTaggerBase( name ),
  m_name(name),
  m_APP_NAME(APP_NAME),
  m_lwnn(nullptr),
  m_dec_mcutL("mcutL"),
  m_dec_mcutH("mcutH"),
  m_dec_scoreCut("scoreCut"),
  m_dec_scoreValue("scoreValue"),
  m_dec_weight("weightdec")
{

    declareProperty( "JetEtaMax",             m_jetEtaMax = 2.0);

    // tagging scale factors
    declareProperty( "WeightDecorationName",      m_weightdecorationName = "SF");
    declareProperty( "WeightFile",                m_weightFileName = "");
    declareProperty( "WeightHistogramName",       m_weightHistogramName = "");
    declareProperty( "WeightFlavors",             m_weightFlavors = "");
}

JSSWTopTaggerDNN::~JSSWTopTaggerDNN() {}

StatusCode JSSWTopTaggerDNN::initialize(){

  /* Initialize the DNN tagger tool */
  ATH_MSG_INFO( (m_APP_NAME+": Initializing JSSWTopTaggerDNN tool").c_str() );
  ATH_MSG_INFO( (m_APP_NAME+": Using config file :"+m_configFile).c_str() );

  if( ! m_configFile.empty() ) {
    ATH_MSG_INFO( "Using config file : "<< m_configFile );
    // check for the existence of the configuration file
    std::string configPath;

    if ( m_calibArea.compare("Local") == 0 ){
      configPath = PathResolverFindCalibFile(("$WorkDir_DIR/data/BoostedJetTaggers/"+m_configFile).c_str());
    } else if ( m_calibArea.find("eos") != std::string::npos) {
      configPath = PathResolverFindCalibFile((m_calibArea+"/"+m_configFile).c_str());
    } else {
      configPath = PathResolverFindCalibFile(("BoostedJetTaggers/"+m_calibArea+"/"+m_configFile).c_str());
    }

    /* https://root.cern.ch/root/roottalk/roottalk02/5332.html */
    FileStat_t fStats;
    int fSuccess = gSystem->GetPathInfo(configPath.c_str(), fStats);
    if(fSuccess != 0){
      ATH_MSG_ERROR("Recommendations file could not be found : ");
      return StatusCode::FAILURE;
    }
    else {
      ATH_MSG_DEBUG("Recommendations file was found : "<<configPath);
    }


    TEnv configReader;
    if(configReader.ReadFile( configPath.c_str(), EEnvLevel(0) ) != 0 ) {
      ATH_MSG_ERROR( "Error while reading config file : "<< configPath );
      return StatusCode::FAILURE;
    }

    // get tagger type
    m_tagType = configReader.GetValue("TaggerType" ,"");

    // get the CVMFS calib area where stuff is stored
    // if this is set to "Local" then it will look for the config file in the share space
    m_kerasCalibArea = configReader.GetValue("CalibAreaKeras" ,"");

    // get the name/path of the JSON config
    m_kerasConfigFileName = configReader.GetValue("KerasConfigFile" ,"");

    // get the name of the Keras output node
    m_kerasConfigOutputName = configReader.GetValue("KerasOutput" ,"");

    // get the configured cut values
    m_strMassCutLow  = configReader.GetValue("MassCutLow" ,"");
    m_strMassCutHigh = configReader.GetValue("MassCutHigh" ,"");
    m_strScoreCut    = configReader.GetValue("ScoreCut" ,"");

    // get min and max jet pt
    m_jetPtMin = configReader.GetValue("pTCutLow", 350.0);
    m_jetPtMax = configReader.GetValue("pTCutHigh", 4000.0);

    // get the decoration name
    m_decorationName = configReader.GetValue("DecorationName" ,"");

    // get the scale factor configuration
    m_calcSF = configReader.GetValue("CalcSF", false);
    if(m_calcSF){
      m_weightdecorationName = configReader.GetValue("WeightDecorationName", "");
      m_weightFileName = configReader.GetValue("WeightFile", "");
      m_weightHistogramName = configReader.GetValue("WeightHistogramName", "");
      m_weightFlavors = configReader.GetValue("WeightFlavors", "");
    
      // get truth label name information
      m_truthLabelName = configReader.GetValue("TruthLabelName" , "R10TruthLabel_R21Consolidated");
    }

    // print out the configuration parameters for viewing
    ATH_MSG_INFO( "Configurations Loaded :");
    ATH_MSG_INFO( "tagType               : " << m_tagType );
    ATH_MSG_INFO( "calibarea_keras       : " << m_kerasCalibArea );
    ATH_MSG_INFO( "kerasConfigFileName   : " << m_kerasConfigFileName );
    ATH_MSG_INFO( "kerasConfigOutputName : " << m_kerasConfigOutputName );
    ATH_MSG_INFO( "strMassCutLow         : " << m_strMassCutLow );
    ATH_MSG_INFO( "strMassCutHigh        : " << m_strMassCutHigh );
    ATH_MSG_INFO( "pTCutLow              : " << m_jetPtMin );
    ATH_MSG_INFO( "pTCutHigh             : " << m_jetPtMax );
    ATH_MSG_INFO( "strScoreCut           : " << m_strScoreCut );
    ATH_MSG_INFO( "decorationName        : " << m_decorationName );
    if(m_calcSF){
      ATH_MSG_INFO( "weightdecorationName  : " << m_weightdecorationName );
      ATH_MSG_INFO( "weightFile            : " << m_weightFileName );
      ATH_MSG_INFO( "weightHistogramName   : " << m_weightHistogramName );
      ATH_MSG_INFO( "weightFlavors         : " << m_weightFlavors );
      ATH_MSG_INFO( "TruthLabelName        : " << m_truthLabelName );
    }
  }
  else { // no config file
    // Assume the cut functions have been set through properties.
    // check they are non empty
    if((m_kerasConfigFileName.empty() ||
        m_kerasConfigOutputName.empty() ||
        m_strScoreCut.empty() ||
        m_strMassCutLow.empty() ||
        m_strMassCutHigh.empty() ||
        m_decorationName.empty() || 
	m_weightFileName.empty()) ||
       ((m_weightdecorationName.empty() ||
	 m_weightHistogramName.empty() ||
	 m_weightFlavors.empty()) && m_calcSF) ) {
      ATH_MSG_ERROR( "No config file provided OR you haven't manually specified all needed parameters" ) ;
      ATH_MSG_ERROR( "Please read the TWiki for this tool" );
      return StatusCode::FAILURE;
    }

  }

  // initialize decorators as decorationName+_decorator
  ATH_MSG_INFO( "Decorators that will be attached to jet :" );
  std::string dec_name;

  dec_name = m_decorationName+"_Cut_mlow";
  ATH_MSG_INFO( "  "<<dec_name<<" : lower mass cut for tagger choice" );
  m_dec_mcutL      = SG::AuxElement::Decorator<float>((dec_name).c_str());
  dec_name = m_decorationName+"_Cut_mhigh";
  ATH_MSG_INFO( "  "<<dec_name<<" : upper mass cut for tagger choice" );
  m_dec_mcutH      = SG::AuxElement::Decorator<float>((dec_name).c_str());
  dec_name = m_decorationName+"_Cut_score";
  ATH_MSG_INFO( "  "<<dec_name<<" : MVA score cut for tagger choice" );
  m_dec_scoreCut   = SG::AuxElement::Decorator<float>((dec_name).c_str());
  dec_name = m_decorationName+"_Score";
  ATH_MSG_INFO( "  "<<dec_name<<" : evaluated MVA score" );
  m_dec_scoreValue = SG::AuxElement::Decorator<float>((dec_name).c_str());
  if(m_calcSF){
    dec_name = m_decorationName+"_"+m_weightdecorationName;
    ATH_MSG_INFO( "  "<<dec_name<<" : tagging SF" );
    m_dec_weight     = SG::AuxElement::Decorator<float>((dec_name).c_str());
    m_acc_truthLabel = std::make_unique< SG::AuxElement::ConstAccessor<int> >((m_truthLabelName).c_str());
  }

  // transform these strings into functions
  m_funcMassCutLow   = new TF1("strMassCutLow",  m_strMassCutLow.c_str(),  0, 14000);
  m_funcMassCutHigh  = new TF1("strMassCutHigh", m_strMassCutHigh.c_str(), 0, 14000);
  m_funcScoreCut     = new TF1("strScoreCut",      m_strScoreCut.c_str(),      0, 14000);

  ATH_MSG_INFO( "DNN Tagger tool initialized" );
  ATH_MSG_INFO( "  Mass cut low   : "<< m_strMassCutLow );
  ATH_MSG_INFO( "  Mass cut High  : "<< m_strMassCutHigh );
  ATH_MSG_INFO( "  Score cut low    : "<< m_strScoreCut );

  // if the calibarea is specified to be "Local" then it looks in the same place as the top level configs
  if( m_kerasCalibArea.empty() ){
    ATH_MSG_INFO( (m_APP_NAME+": You need to specify where the calibarea is as either being Local or on CVMFS") );
    return StatusCode::FAILURE;
  }
  else if(m_kerasCalibArea.compare("Local")==0){
    std::string localCalibArea = "BoostedJetTaggers/JSSWTopTaggerDNN/";
    ATH_MSG_INFO( (m_APP_NAME+": Using Local calibarea "+localCalibArea ));
    // convert the JSON config file name to the full path
    m_kerasConfigFilePath = PathResolverFindCalibFile(localCalibArea+m_kerasConfigFileName);
    if(m_calcSF)
      m_weightConfigPath = PathResolverFindCalibFile(localCalibArea+m_weightFileName);
  }
  else{
    ATH_MSG_INFO( (m_APP_NAME+": Using CVMFS calibarea") );
    // get the config file from CVMFS
    // necessary because xml files are too large to house on the data space
    m_kerasConfigFilePath = PathResolverFindCalibFile( (m_kerasCalibArea+m_kerasConfigFileName).c_str() );
    if(m_calcSF)
      m_weightConfigPath = PathResolverFindCalibFile( (m_kerasCalibArea+m_weightFileName).c_str());
  }

  // read json file for DNN weights
  ATH_MSG_INFO( (m_APP_NAME+": DNN Tagger configured with: "+m_kerasConfigFilePath.c_str() ));

  std::ifstream input_cfg( m_kerasConfigFilePath.c_str() );

  if(input_cfg.is_open()==false){
    ATH_MSG_INFO( (m_APP_NAME+": Error openning config file : "+m_kerasConfigFilePath.c_str() ));
    ATH_MSG_INFO( (m_APP_NAME+": Are you sure that the file exists at this path?" ));
    return StatusCode::FAILURE;
  }

  lwt::JSONConfig cfg = lwt::parse_json( input_cfg );

  ATH_MSG_INFO( (m_APP_NAME+": Keras Network NLayers : "+cfg.layers.size() ));

  m_lwnn = std::unique_ptr<lwt::LightweightNeuralNetwork>
              (new lwt::LightweightNeuralNetwork(cfg.inputs, cfg.layers, cfg.outputs) );


  // set internal tagger type
  size_t pos1;
  size_t pos2;

  pos1 = m_tagType.find("Top");
  pos2 = m_tagType.find("W");

  if(pos1!=std::string::npos){
    ATH_MSG_DEBUG( "This is a top quark tagger" );
    m_TagClass = TAGCLASS::TopQuark;
  }
  else if(pos2!=std::string::npos){
    ATH_MSG_DEBUG( "This is a W boson tagger" );
    m_TagClass = TAGCLASS::WBoson;
  }
  else{
    ATH_MSG_ERROR( "Your configuration is invalid because from it, I can't tell if it is for W tagging or top quark tagging." );
    return StatusCode::FAILURE;
  }


  //setting the possible states that the tagger can be left in after the JSSTaggerBase::tag() function is called
  m_accept.addCut( "ValidPtRangeHigh"    , "True if the jet is not too high pT"  );
  m_accept.addCut( "ValidPtRangeLow"     , "True if the jet is not too low pT"   );
  m_accept.addCut( "ValidEtaRange"       , "True if the jet is not too forward"     );
  m_accept.addCut( "ValidJetContent"     , "True if the jet is alright technically (e.g. all attributes necessary for tag)"        );

  if(m_tagType.compare("WBoson")==0 || m_tagType.compare("ZBoson")==0){
    m_accept.addCut( "PassMassLow"         , "mJet > mCutLow"       );
    m_accept.addCut( "PassMassHigh"        , "mJet < mCutHigh"      );
    m_accept.addCut( "PassScore"           , "ScoreJet > ScoreCut"         );
  }
  if(m_tagType.compare("TopQuark")==0){
    m_accept.addCut( "PassMassLow"         , "mJet > mCutLow"       );
    m_accept.addCut( "PassScore"           , "ScoreJet > ScoreCut"         );
  }
  //loop over and print out the cuts that have been configured
  ATH_MSG_INFO( "After tagging, you will have access to the following cuts as a Root::TAccept : (<NCut>) <cut> : <description>)" );
  showCuts();

  // setup scale factors
  if(m_calcSF){
    std::unique_ptr<TFile> weightConfig(TFile::Open( m_weightConfigPath.c_str()));
    m_weightConfig = std::move(weightConfig);
    if( !m_weightConfig ) {
      ATH_MSG_INFO( (m_APP_NAME+": Error openning config file : "+m_weightConfigPath.c_str()) );
      return StatusCode::FAILURE;
    }

    // install histograms for tagging SF
    std::stringstream ss{m_weightFlavors};
    std::string flavor;
    while(std::getline(ss, flavor, ',')){
      m_weightHistograms.insert( std::make_pair( flavor, (TH2D*)m_weightConfig->Get((m_weightHistogramName+"_"+flavor).c_str()) ) );
      ATH_MSG_INFO( (m_APP_NAME+"Tagging SF histogram for "+flavor+" is installed.") );
    }

    ASG_SET_ANA_TOOL_TYPE( m_JetTruthLabelingTool, JetTruthLabelingTool);
    m_JetTruthLabelingTool.setName("JetTruthLabelingTool");
    ATH_CHECK( m_JetTruthLabelingTool.setProperty("TruthLabelName", m_truthLabelName) );
    ATH_CHECK( m_JetTruthLabelingTool.setProperty("UseTRUTH3", m_truthLabelUseTRUTH3) );
    ATH_CHECK( m_JetTruthLabelingTool.setProperty("TruthParticleContainerName", m_truthParticleContainerName) );
    ATH_CHECK( m_JetTruthLabelingTool.setProperty("TruthBosonContainerName", m_truthBosonContainerName) );
    ATH_CHECK( m_JetTruthLabelingTool.setProperty("TruthTopQuarkContainerName", m_truthTopQuarkContainerName) );
    ATH_CHECK( m_JetTruthLabelingTool.retrieve() );
  }

  ATH_MSG_INFO( (m_APP_NAME+": DNN Tagger tool initialized").c_str() );

  return StatusCode::SUCCESS;
}

Root::TAccept& JSSWTopTaggerDNN::tag(const xAOD::Jet& jet) const{

  ATH_MSG_DEBUG( ": Obtaining DNN result" );

  //clear all accept values
  m_accept.clear();

  // set the jet validity bits to 1 by default
  m_accept.setCutResult( "ValidPtRangeHigh", true);
  m_accept.setCutResult( "ValidPtRangeLow" , true);
  m_accept.setCutResult( "ValidEtaRange"   , true);
  m_accept.setCutResult( "ValidJetContent" , true);

  // counter for pt range warnings
  const static int maxNWarn = 10;
  static int nWarn = 0;

  // check basic kinematic selection
  if (std::fabs(jet.eta()) > m_jetEtaMax) {
    ATH_MSG_DEBUG("Jet does not pass basic kinematic selection (|eta| < " << m_jetEtaMax << "). Jet eta = " << jet.eta());
    m_accept.setCutResult("ValidEtaRange", false);
  }
  if (jet.pt()/1.e3 < m_jetPtMin) {
    ATH_MSG_DEBUG("Jet does not pass basic kinematic selection (pT > " << m_jetPtMin << "). Jet pT = " << jet.pt()/1.e3);
    m_accept.setCutResult("ValidPtRangeLow", false);
  }
  if (jet.pt()/1.e3 > m_jetPtMax) {
    if(nWarn++ < maxNWarn) ATH_MSG_WARNING("Jet does not pass basic kinematic selection (pT < " << m_jetPtMax << "). Jet pT = " << jet.pt()/1.e3);
    else ATH_MSG_DEBUG("Jet does not pass basic kinematic selection (pT < " << m_jetPtMax << "). Jet pT = " << jet.pt()/1.e3);
    m_accept.setCutResult("ValidPtRangeHigh", false);
  }

  // get the relevant attributes of the jet
  // mass and pt - note that this will depend on the configuration of the calibration used
  float jet_pt   = jet.pt()/1000.0;
  float jet_mass = jet.m()/1000.0;

  // get DNN score for the jet
  float jet_score = getScore(jet);

  // evaluate the values of the upper and lower mass bounds and the d2 cut
  float cut_mass_low  = m_funcMassCutLow ->Eval(jet_pt);
  float cut_mass_high = m_funcMassCutHigh->Eval(jet_pt);
  float cut_score     = m_funcScoreCut   ->Eval(jet_pt);

  // decorate truth label for SF provider
  float jet_weight=1.0;
  if ( m_calcSF && (!m_acc_truthLabel->isAvailable(jet) || LargeRJetTruthLabel::intToEnum((*m_acc_truthLabel)(jet))==LargeRJetTruthLabel::UNKNOWN) ){
    if ( m_IsMC ){
      if ( m_JetTruthLabelingTool->modifyJet(jet) == StatusCode::FAILURE){
        ATH_MSG_FATAL("Failed to decorate jet truth label. Please check truth container names");
      }
    }
  }

  if( (jet_score > cut_score) && m_calcSF) {
    if ( m_IsMC ){
      jet_weight = getWeight(jet);
    }else{
      jet_weight = 1.0;
    }
  }

  // decorate the cut value if needed;
  if(m_decorate){
    ATH_MSG_DEBUG("Decorating with score");
    decorateJet(jet, cut_mass_high, cut_mass_low, cut_score, jet_score, jet_weight);
  }

  // evaluate the cut criteria on mass and score
  ATH_MSG_VERBOSE(": CutsValues : MassWindow=["<<cut_mass_low<<","<<cut_mass_high<<"]  ,  scoreCut="<<cut_score );
  ATH_MSG_VERBOSE(": JetValues  : JetMass="<<jet_mass<<"  ,  score="<<jet_score<<"  ,  SF="<<jet_weight );

  //set the TAccept depending on whether it is a W/Z or a top tag
  if(m_tagType.compare("WBoson")==0 || m_tagType.compare("ZBoson")==0){
    ATH_MSG_VERBOSE("Determining WZ tag return");
    if( jet_mass>cut_mass_low )
      m_accept.setCutResult( "PassMassLow"  , true );
    if( jet_mass<cut_mass_high )
      m_accept.setCutResult( "PassMassHigh" , true );
    if( jet_score > cut_score )
      m_accept.setCutResult( "PassScore"    , true );
  }
  else if(m_tagType.compare("TopQuark")==0){
    ATH_MSG_VERBOSE("Determining TopQuark tag return");
    if( jet_mass>cut_mass_low )
      m_accept.setCutResult( "PassMassLow"  , true );
    if( jet_score > cut_score )
      m_accept.setCutResult( "PassScore"    , true );
  }

  // return the TAccept object that you created and filled
  return m_accept;
}

double JSSWTopTaggerDNN::getScore(const xAOD::Jet& jet) const{

    // create input dictionary map<string,double> for argument to lwtnn
    std::map<std::string,double> DNN_inputValues = getJetProperties(jet);

    // evaluate the network
    lwt::ValueMap discriminant = m_lwnn->compute(DNN_inputValues);

    // obtain the output associated with the single output node
    double DNNscore(-666.);

    if(m_undefInput){
      ATH_MSG_WARNING("One (or more) tagger input variable has an undefined value (NaN), setting score to -666");      
      return DNNscore;
    }

    DNNscore = discriminant[m_kerasConfigOutputName];

    return DNNscore;
}

double JSSWTopTaggerDNN::getWeight(const xAOD::Jet& jet) const {
    if ( jet.pt()*0.001 < m_jetPtMin ||
	 jet.pt()*0.001 > m_jetPtMax ||
	 fabs(jet.eta())>m_jetEtaMax ) return 1.0;

    std::string truthLabelStr;
    LargeRJetTruthLabel::TypeEnum jetContainment=LargeRJetTruthLabel::intToEnum((*m_acc_truthLabel)(jet));
    if( m_weightHistograms.count("t_qqb") ) {
      // full-contained top tagger
      if( jetContainment==LargeRJetTruthLabel::tqqb ){
	truthLabelStr="t_qqb";
      }else if( jetContainment==LargeRJetTruthLabel::notruth || jetContainment==LargeRJetTruthLabel::qcd ) {
	truthLabelStr="q";
      }
    }else{
      // W/Z tagger or inclusive top tagger
      if( jetContainment==LargeRJetTruthLabel::tqqb || jetContainment==LargeRJetTruthLabel::other_From_t ){
	truthLabelStr="t";
      }else if( jetContainment==LargeRJetTruthLabel::Wqq || jetContainment==LargeRJetTruthLabel::Zqq || jetContainment==LargeRJetTruthLabel::Wqq_From_t ){
	truthLabelStr="V_qq";
      }else if( jetContainment==LargeRJetTruthLabel::notruth || jetContainment==LargeRJetTruthLabel::qcd ) {
	truthLabelStr="q";
      }
    }

    double SF=1.0;
    if( m_weightHistograms.count(truthLabelStr.c_str()) ){
      int pt_mPt_bin=(m_weightHistograms.find(truthLabelStr.c_str())->second)->FindBin(jet.pt()*0.001, log(jet.m()/jet.pt()));
      SF=(m_weightHistograms.find(truthLabelStr.c_str())->second)->GetBinContent(pt_mPt_bin);
    } else {
      ATH_MSG_DEBUG("SF for truth label for "+truthLabelStr+" is not available. Just return 1.0");
      return 1.0;      
    }
    if ( SF < 1e-3 ) {
      ATH_MSG_DEBUG("(pt, m/pt) is out of range for SF calculation. Just return 1.0");
      return 1.0;
    } else return SF;
}

void JSSWTopTaggerDNN::decorateJet(const xAOD::Jet& jet, float mcutH, float mcutL, float scoreCut, float scoreValue, float weightValue) const{
    /* decorate jet with attributes */

    m_dec_mcutH(jet)      = mcutH;
    m_dec_mcutL(jet)      = mcutL;
    m_dec_scoreCut(jet)   = scoreCut;
    m_dec_scoreValue(jet) = scoreValue;
    m_dec_weight(jet)     = weightValue;
}

std::map<std::string,double> JSSWTopTaggerDNN::getJetProperties(const xAOD::Jet& jet) const{

  // flag for if the input is nonsense
  m_undefInput = false;

  // Update the jet substructure variables for this jet
  std::map<std::string,double> DNN_inputValues;


  if(m_TagClass == TAGCLASS::WBoson){
    ATH_MSG_DEBUG("Loading variables for W boson tagger");

    //mass and pT
    //it is assumed that these are the combined and calibrated mass and pT
    DNN_inputValues["CaloTACombinedMassUncorrelated"] = jet.m();
    DNN_inputValues["JetpTCorrByCombinedMass"] = jet.pt();

    // Splitting Scales
    DNN_inputValues["Split12"] = jet.getAttribute<float>("Split12");

    // Energy Correlation Functions
    float ecf1(0.0);
    float ecf2(0.0);
    float ecf3(0.0);
    jet.getAttribute("ECF1", ecf1);
    jet.getAttribute("ECF2", ecf2);
    jet.getAttribute("ECF3", ecf3);
    if (!jet.isAvailable<float>("C2"))
        DNN_inputValues["C2"] = ecf3 * ecf1 / pow(ecf2, 2.0);
    else
        DNN_inputValues["C2"] = jet.getAttribute<float>("C2");
    if (!jet.isAvailable<float>("D2"))
        DNN_inputValues["D2"] = ecf3 * pow(ecf1, 3.0) / pow(ecf2, 3.0);
    else
        DNN_inputValues["D2"] = jet.getAttribute<float>("D2");

    // N-subjettiness
    float tau1wta = jet.getAttribute<float>("Tau1_wta");
    float tau2wta = jet.getAttribute<float>("Tau2_wta");

    if(!jet.isAvailable<float>("Tau21_wta")){
      double tau21wta = tau2wta / tau1wta;
      if(std::isnan(tau21wta)){
        m_undefInput = true;
      }
      DNN_inputValues["Tau21_wta"] = tau21wta;
    }
    else{
      DNN_inputValues["Tau21_wta"] = jet.getAttribute<float>("Tau21_wta");
    }

    // Qw observable for top tagging

    DNN_inputValues["FoxWolfram20"] = jet.getAttribute<float>("FoxWolfram2") / jet.getAttribute<float>("FoxWolfram0");
    DNN_inputValues["PlanarFlow"] = jet.getAttribute<float>("PlanarFlow");
    DNN_inputValues["Angularity"] = jet.getAttribute<float>("Angularity");
    DNN_inputValues["Aplanarity"] = jet.getAttribute<float>("Aplanarity");
    DNN_inputValues["ZCut12"] = jet.getAttribute<float>("ZCut12");
    DNN_inputValues["KtDR"] = jet.getAttribute<float>("KtDR");
  }
  else if(m_TagClass == TAGCLASS::TopQuark){
    ATH_MSG_DEBUG("Loading variables for top quark tagger");

    //mass and pT
    //it is assumed that these are the combined and calibrated mass and pT
    DNN_inputValues["CaloTACombinedMassUncorrelated"] = jet.m();
    DNN_inputValues["JetpTCorrByCombinedMass"] = jet.pt();

    // Splitting Scales
    DNN_inputValues["Split12"] = jet.getAttribute<float>("Split12");
    DNN_inputValues["Split23"] = jet.getAttribute<float>("Split23");

    // Energy Correlation Functions
    float ecf1(0.0);
    float ecf2(0.0);
    float ecf3(0.0);
    jet.getAttribute("ECF1", ecf1);
    jet.getAttribute("ECF2", ecf2);
    jet.getAttribute("ECF3", ecf3);
    if (!jet.isAvailable<float>("C2"))
        DNN_inputValues["C2"] = ecf3 * ecf1 / pow(ecf2, 2.0);
    else
        DNN_inputValues["C2"] = jet.getAttribute<float>("C2");
    if (!jet.isAvailable<float>("D2"))
        DNN_inputValues["D2"] = ecf3 * pow(ecf1, 3.0) / pow(ecf2, 3.0);
    else
        DNN_inputValues["D2"] = jet.getAttribute<float>("D2");

    // e3 := normalized ECF3/ECF1**3
    float e3(0.0);
    e3 = ecf3/pow(ecf1,3.0);
    DNN_inputValues["e3"] = e3;

    // N-subjettiness
    float tau1wta = jet.getAttribute<float>("Tau1_wta");
    float tau2wta = jet.getAttribute<float>("Tau2_wta");
    float tau3wta = jet.getAttribute<float>("Tau3_wta");
    DNN_inputValues["Tau1_wta"] = tau1wta;
    DNN_inputValues["Tau2_wta"] = tau2wta;
    DNN_inputValues["Tau3_wta"] = tau3wta;

    m_undefInput = false;

    if(!jet.isAvailable<float>("Tau21_wta")){
      double tau21wta = tau2wta / tau1wta;
      if(std::isnan(tau21wta)){
        m_undefInput = true;
      }
      DNN_inputValues["Tau21_wta"] = tau21wta;
    }
    else{
      DNN_inputValues["Tau21_wta"] = jet.getAttribute<float>("Tau21_wta");
    }

    if(!jet.isAvailable<float>("Tau32_wta")){
      double tau32wta = tau3wta / tau2wta;
      if(std::isnan(tau32wta)){
        m_undefInput = true;
      }
      DNN_inputValues["Tau32_wta"] = tau32wta;
    }
    else{
      DNN_inputValues["Tau32_wta"] = jet.getAttribute<float>("Tau32_wta");
    }

    // Qw observable for top tagging
    DNN_inputValues["Qw"] = jet.getAttribute<float>("Qw");
  }
  else{
    ATH_MSG_ERROR( "Loading variables failed because the tagger you are using is not clear to me");
  }

  return DNN_inputValues;
}

StatusCode JSSWTopTaggerDNN::finalize(){
    // Delete or clear anything
    return StatusCode::SUCCESS;
}



