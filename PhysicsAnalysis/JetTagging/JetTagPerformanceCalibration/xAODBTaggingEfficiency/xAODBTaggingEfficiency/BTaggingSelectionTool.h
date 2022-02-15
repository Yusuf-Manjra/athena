// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// BTaggingSelectionTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
/**
  @class BTaggingSelectionTool
  Tool to apply flavour-tagging requirements on jets
  @author C. Lüdtke, M. Ughetto
  @contact cluedtke@cern.ch, mughetto@cern.ch
*/

#ifndef CPBTAGGINGSELECTIONTOOL_H
#define CPBTAGGINGSELECTIONTOOL_H

#include "FTagAnalysisInterfaces/IBTaggingSelectionTool.h"
#include "xAODBTagging/BTagging.h"

#include "AsgTools/AsgTool.h"
#include "PATCore/IAsgSelectionTool.h"

#include "TFile.h"
#include "TSpline.h"
#include "TVector.h"
#include <string>
#include <set>
#include <vector>
#include <map>

class BTaggingSelectionTool: public asg::AsgTool,
			     public virtual IBTaggingSelectionTool,
			     public virtual IAsgSelectionTool  {
  typedef double (xAOD::BTagging::* tagWeight_member_t)() const;

  /// Create a proper constructor for Athena
  ASG_TOOL_CLASS2( BTaggingSelectionTool , IAsgSelectionTool, IBTaggingSelectionTool )

  public:
  /// Create a constructor for standalone usage
  BTaggingSelectionTool( const std::string& name );
  StatusCode initialize();
  /// Get an object describing the "selection steps" of the tool
  virtual const Root::TAccept& getTAccept() const;

  /// Get the decision using a generic IParticle pointer
  virtual const Root::TAccept& accept( const xAOD::IParticle* p ) const;
  virtual const Root::TAccept& accept( const xAOD::Jet& jet ) const;

  /// Get the decision using thet jet's pt and mv2c20 weight values
  virtual const Root::TAccept& accept(double /* jet pt */, double /* jet eta */, double /* tag_weight */ ) const;
  virtual const Root::TAccept& accept(double /* jet pt */, double /* jet eta*/, double /* veto_taggerWeight */, double /* veto_taggerWeight */) const;
  virtual const Root::TAccept& accept(double /* jet pt */, double /* jet eta */, double /* dl1pb */, double /* dl1pc  */ , double /* dl1pu  */) const;

  /// Decide in which quantile of the MV2c20 weight distribution the jet belongs (continuous tagging)
  /// The return value represents the bin index of the quantile distribution
  virtual int getQuantile( const xAOD::IParticle* ) const;
  virtual int getQuantile( const xAOD::Jet& ) const;
  virtual int getQuantile( double /* jet pt */, double /* jet eta */, double /* tag weight */  ) const;
  virtual int getQuantile(double /*pT*/, double /*eta*/, double /*tag_weight*/, double /*tag_weight_veto*/ ) const;

  virtual CP::CorrectionCode getCutValue(double /* jet pt */, double & cutval, bool useVetoWP = false) const;
  virtual CP::CorrectionCode getTaggerWeight( const xAOD::Jet& jet, double & weight ,bool useVetoWP = false) const;
  virtual CP::CorrectionCode getTaggerWeight( double /* dl1pb */, double /* dl1pc  */ , double /* dl1pu  */ , double & weight,bool useVetoWP = false) const;

private:
  /// Helper function that decides whether a jet belongs to the correct jet selection for b-tagging
  virtual bool checkRange( double /* jet pt */, double /* jet eta */ ) const;
  //fill the spline or vector that store the cut values for a particular working point
  void InitializeTaggerVariables(std::string taggerName,std::string OP, TSpline3 *spline, TVector *constcut, double &fraction);

  bool m_initialised;

  bool m_ErrorOnTagWeightFailure;

   /// Object used to store the last decision
  mutable Root::TAccept m_accept;

  double m_maxEta;
  double m_minPt;
  double m_maxRangePt;

  std::string m_CutFileName;
  std::string m_taggerName;
  std::string m_OP;
  std::string m_jetAuthor;

  TFile *m_inf;
  double m_continuouscuts[6];

  //optional "Veto" working point defined in the working point name
  //the user can specify for example:
  //tool->setProperty("TaggerName", "MV2c10");
  //tool->setProperty("OperatingPoint", "FixedCutBEff_77-Veto-DL1-FixedCutBEff_60");
  //and the tool->accept() methods will require the jet to pass MV2c10 FixedCutBEff_77 but fail DL1 FixedCutBEff_60
  bool m_useVeto;
  std::string m_taggerName_Veto;
  std::string m_OP_Veto;

  enum Tag{
    BTag, ///< b-tagging
    CTag, ///< c-tagging
    LTag  ///< light-tagging (okay, not definied now)
  };

  struct taggerproperties{
    std::string name;
    double fraction;
    TSpline3* spline;
    TVector* constcut;
    Tag tagging = Tag::BTag; //b-tagging or c-tagging. b-tagging by default
  };

  taggerproperties m_tagger;
  taggerproperties m_vetoTagger;

  //get from the CDI file the taggers cut object(that holds the definition of cut values)
  //and flaovur fraction (for DL1 tagger) and store them in the right taggerproperties struct
  void ExtractTaggerProperties(taggerproperties& tagger, std::string taggerName, std::string OP);

};

#endif // CPBTAGGINGSELECTIONTOOL_H
