/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETUNCERTAINTIES_CONFIGHELPER_H
#define JETUNCERTAINTIES_CONFIGHELPER_H

#include "JetUncertainties/UncertaintyEnum.h"
#include "ParticleJetTools/LargeRJetLabelEnum.h"

#include "AsgMessaging/AsgMessaging.h"
#include "AsgMessaging/StatusCode.h"

#include "TString.h"
class TEnv;

namespace jet
{

class ComponentHelper
{
    public:
        ComponentHelper(const TString name = "");
        ComponentHelper(TEnv& settings, const TString& compPrefix, const TString& MCtype, const float energyScaleVal);
        virtual ~ComponentHelper() {}
        
        // Values fixed by arguments
        const float energyScale;

        // Raw component values to read from a file
        TString name;
        TString desc;
        TString param;
        TString massDefStr;
        TString scale;
        TString topologyStr;
        TString interpolStr;
        TString special;
        TString uncNameList;
        TString validName;
        TString subCompList;
        int     splitNum;
        int     groupNum;
        TString combMassStr;
        TString caloMassTerm;
        TString TAMassTerm;
        TString caloMassDef;
        TString TAMassDef;
        TString truthLabelStr;
        TString constrainZresponseStr;
        TString constrainZresponseFunc;
        TString LargeRJetTruthLabelName;
        TString LargeRJetTruthLabelStr;
        TString LargeRJetTruthLabelsForSFstr;
        TString RegionForSFstr;
        TString ResultName;

        // Derived values to parse from the raw values
        CompParametrization::TypeEnum parametrization;
        CompMassDef::TypeEnum massDef;
        CompScaleVar::TypeEnum scaleVar;
        JetTopology::TypeEnum topology;
        bool isSpecial;
        PileupComp::TypeEnum pileupType;
        FlavourComp::TypeEnum flavourType;
        CombMassComp::TypeEnum combMassType;
        Interpolate::TypeEnum interpolate;
        std::vector<TString> uncNames;
        std::vector<TString> subComps;
        std::vector<int> truthLabels;
        bool constrainZresponse;
        std::vector<TString> LargeRJetTruthLabelStrs;
        std::vector<LargeRJetTruthLabel::TypeEnum> LargeRJetTruthLabels;
        std::vector<TString> LargeRJetTruthLabelsForSFstrs;
        std::vector<CompFlavorLabelVar::TypeEnum> LargeRJetTruthLabelsForSF;
        CompTaggerRegionVar::TypeEnum RegionForSF;
};

class GroupHelper
{
    public:
        GroupHelper(const TString name = "");
        GroupHelper(TEnv& settings, const TString& groupPrefix, const TString& MCtype);
        virtual ~GroupHelper() {}

        // Raw component values to read from a file
        TString name;
        TString desc;
        TString cat;
        TString corr;
        TString isRed;
        int     groupNum;
        int     subgroupNum;

        // Derived values to parse from the raw values
        CompCategory::TypeEnum category;
        CompCorrelation::TypeEnum correlation;
        bool reducible;
};

class ConfigHelper : asg::AsgMessaging
{
    public:
        ConfigHelper(const TString& confPrefix, const TString& MCtype, const float energyScaleVal);
        virtual StatusCode initialize(TEnv& settings);
        virtual ~ConfigHelper();

        // Classification support
        bool isCompGroup() const;
        bool isComponent() const;
        bool isGroup()     const;

        // Information retrieval
        const ComponentHelper* getComponentInfo() const;
        const GroupHelper*     getGroupInfo()     const;

        // Modification methods
        void enforceGroupNamePrefix(const std::string& prefix);
        void enforceGroupNamePrefix(const TString& prefix);
        void setComponentJetDefSuffix(const std::string& suffix);
        void setComponentJetDefSuffix(const TString& suffix);
        void addValidityHistogram(const std::string& histName);
        void addValidityHistogram(const TString& histName);

    private: 
        ConfigHelper();

        bool m_isInit;
        
        // Values fixed by arguments
        const TString m_confPrefix;
        const TString m_MCtype;
        const float m_energyScale;
        
        // Values filled through initialize
        ComponentHelper* m_cInfo;
        GroupHelper*     m_gInfo;
};

} // end jet namespace

#endif
