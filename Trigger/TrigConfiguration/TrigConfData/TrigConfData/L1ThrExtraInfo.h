/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCONFDATA_L1THREXTRAINFO_H
#define TRIGCONFDATA_L1THREXTRAINFO_H

#include "TrigConfData/DataStructure.h"
#include "TrigConfData/L1ThresholdBase.h"

#include <map>
#include <vector>
#include <iostream>

namespace TrigConf {

   class L1ThrExtraInfoBase;
   class L1ThrExtraInfo_EMTAULegacy;
   class L1ThrExtraInfo_JETLegacy;
   class L1ThrExtraInfo_XSLegacy;
   class L1ThrExtraInfo_eEM;
   class L1ThrExtraInfo_eTAU;
   class L1ThrExtraInfo_jTAU;
   class L1ThrExtraInfo_cTAU;
   class L1ThrExtraInfo_jJ;
   class L1ThrExtraInfo_gXE;
   class L1ThrExtraInfo_MU;

   class L1ThrExtraInfo {
   public:
      static std::unique_ptr<L1ThrExtraInfoBase> createExtraInfo(const std::string & thrTypeName, const boost::property_tree::ptree & data);

      std::weak_ptr<TrigConf::L1ThrExtraInfoBase> addExtraInfo(const std::string & thrTypeName, const boost::property_tree::ptree & data);

      const L1ThrExtraInfo_EMTAULegacy & EM() const;
      const L1ThrExtraInfo_EMTAULegacy & TAU() const;
      const L1ThrExtraInfo_JETLegacy & JET() const;
      const L1ThrExtraInfo_XSLegacy & XS() const;
      const L1ThrExtraInfo_eEM & eEM() const;
      const L1ThrExtraInfo_eTAU & eTAU() const;
      const L1ThrExtraInfo_jTAU & jTAU() const;
      const L1ThrExtraInfo_cTAU & cTAU() const;
      const L1ThrExtraInfo_jJ & jJ() const;
      const L1ThrExtraInfo_gXE & gXE() const;
      const L1ThrExtraInfo_MU & MU() const;

      const L1ThrExtraInfoBase & thrExtraInfo(const std::string & thrTypeName) const;

      bool hasInfo(const std::string & typeName) const;

      void clear();

   private:
      // map from threshold type to L1ThrExtraInfo object
      std::map<std::string, std::shared_ptr<TrigConf::L1ThrExtraInfoBase>> m_thrExtraInfo{};
      std::shared_ptr<TrigConf::L1ThrExtraInfoBase> m_emptyInfo{ nullptr };
   };



   class L1ThrExtraInfo_EMTAULegacy final : public L1ThrExtraInfoBase {
   public:
      L1ThrExtraInfo_EMTAULegacy(const std::string & thrTypeName, const ptree & data) :
         L1ThrExtraInfoBase(thrTypeName, data) { load(); }
      virtual ~L1ThrExtraInfo_EMTAULegacy() = default;
      virtual std::string className() const { return "L1ThrExtraInfo_EMTAULegacy"; }
      unsigned int emScale() const { return 1000 / resolutionMeV(); }
      float ptMinToTopo() const { return m_ptMinToTopoMeV/1000.0f; }
      unsigned int ptMinToTopoMeV() const { return m_ptMinToTopoMeV; }
      unsigned int ptMinToTopoCounts() const { return energyInCounts( m_ptMinToTopoMeV, resolutionMeV() ); }
      const IsolationLegacy & isolation(const std::string & thrType, size_t bit) const;
   private:
      /** Update the internal members */
      void load();
      /** EM specific data */
      unsigned int m_ptMinToTopoMeV{0}; // in MeV
      std::map<std::string,std::vector<IsolationLegacy>> m_isolation{};
   };


   class L1ThrExtraInfo_JETLegacy final : public L1ThrExtraInfoBase {
   public:
      L1ThrExtraInfo_JETLegacy(const std::string & thrTypeName, const ptree & data) :
         L1ThrExtraInfoBase(thrTypeName, data) { load(); }
      virtual ~L1ThrExtraInfo_JETLegacy() = default;
      virtual std::string className() const { return "L1ThrExtraInfo_JETLegacy"; }
      unsigned int jetScale() const { return 1000 / resolutionMeV(); }
      double ptMinToTopoLargeWindow() const { return m_ptMinToTopoLargeWindowMeV / 1000.0; }
      double ptMinToTopoSmallWindow() const { return m_ptMinToTopoSmallWindowMeV / 1000.0; }
      unsigned int ptMinToTopoLargeWindowMeV() const { return m_ptMinToTopoLargeWindowMeV; }
      unsigned int ptMinToTopoSmallWindowMeV() const { return m_ptMinToTopoSmallWindowMeV; }
      unsigned int ptMinToTopoLargeWindowCounts() const { return energyInCounts( m_ptMinToTopoLargeWindowMeV, resolutionMeV() ); }
      unsigned int ptMinToTopoSmallWindowCounts() const { return energyInCounts( m_ptMinToTopoSmallWindowMeV, resolutionMeV() ); }
   private:
      /** Update the internal members */
      void load();
      /** JET specific data */
      unsigned int m_ptMinToTopoLargeWindowMeV{0};
      unsigned int m_ptMinToTopoSmallWindowMeV{0};
   };


   class L1ThrExtraInfo_XSLegacy final : public L1ThrExtraInfoBase {
   public:
      L1ThrExtraInfo_XSLegacy(const std::string & thrTypeName, const ptree & data) :
         L1ThrExtraInfoBase(thrTypeName, data) { load(); }
      virtual ~L1ThrExtraInfo_XSLegacy() = default;
      virtual std::string className() const { return "L1ThrExtraInfo_XSLegacy"; }
      unsigned int xeMin() const { return m_xeMin; };
      unsigned int xeMax() const { return m_xeMax; };
      unsigned int teSqrtMin() const { return m_teSqrtMin; };
      unsigned int teSqrtMax() const { return m_teSqrtMax; };
      unsigned int xsSigmaScale() const { return m_xsSigmaScale; };
      unsigned int xsSigmaOffset() const { return m_xsSigmaOffset; };
   private:
      /** Update the internal members */
      void load();
      /** XS specific data */
      unsigned int m_xeMin{0};
      unsigned int m_xeMax{0};
      unsigned int m_teSqrtMin{0};
      unsigned int m_teSqrtMax{0};
      unsigned int m_xsSigmaScale{0}; 
      unsigned int m_xsSigmaOffset{0};
   };


   /***********************************
    * Extra info for new thresholds
    ***********************************/
   class L1ThrExtraInfo_eEM final : public L1ThrExtraInfoBase {
   public:
      class WorkingPoints_eEM {
      public:
         WorkingPoints_eEM() = default;
         WorkingPoints_eEM( const boost::property_tree::ptree & );
         bool isDefined() const { return m_isDefined; } 
         int reta_fw()       const { return m_reta_fw; } 
         int wstot_fw()      const { return m_wstot_fw; }
         int rhad_fw()       const { return m_rhad_fw; }
         unsigned int maxEt()  const { return m_maxEt; }
         double reta_d()       const { return m_reta_d; } 
         double wstot_d()     const { return m_wstot_d; }
         double rhad_d()       const { return m_rhad_d; }
      private:
         bool m_isDefined { false };
         double m_reta_d { 0 };
         double m_wstot_d { 0 };
         double m_rhad_d { 0 };
         int   m_reta_fw { 0 };
         int   m_wstot_fw { 0 };
         int   m_rhad_fw { 0 };
         unsigned int m_maxEt { 0 };
      };
      L1ThrExtraInfo_eEM(const std::string & thrTypeName, const ptree & data) :
         L1ThrExtraInfoBase(thrTypeName, data) { load(); }
      virtual ~L1ThrExtraInfo_eEM() = default;
      virtual std::string className() const { return "L1ThrExtraInfo_eEM"; }
      float ptMinToTopo() const { return m_ptMinToTopoMeV/1000.0f; }
      unsigned int ptMinToTopoMeV() const { return m_ptMinToTopoMeV; }
      unsigned int ptMinToTopoCounts() const { return energyInCounts( m_ptMinToTopoMeV, resolutionMeV() ); }
      const WorkingPoints_eEM & isolation(TrigConf::Selection::WP wp, int eta) const { return m_isolation.at(wp).at(eta); }
      const ValueWithEtaDependence<WorkingPoints_eEM> & isolation(TrigConf::Selection::WP wp) const { return m_isolation.at(wp); }
   private:
      /** Update the internal members */
      void load();
      /** eEM specific data */
      unsigned int m_ptMinToTopoMeV{0};
      std::map<TrigConf::Selection::WP, ValueWithEtaDependence<WorkingPoints_eEM>> m_isolation{};
   };
   std::ostream & operator<<(std::ostream & os, const TrigConf::L1ThrExtraInfo_eEM::WorkingPoints_eEM & iso);


   class L1ThrExtraInfo_eTAU final : public L1ThrExtraInfoBase {
   public:
      class WorkingPoints_eTAU {
      public:
         WorkingPoints_eTAU( const boost::property_tree::ptree & );
         bool isDefined() const { return m_isDefined; }
         int isoConeRel_fw() const { return m_isoConeRel_fw; }
         double isoConeRel_d() const { return m_isoConeRel_d; }
         int fEM_fw() const { return m_fEM_fw; }
         double fEM_d() const { return m_fEM_d; }
         unsigned int maxEt() const { return m_maxEt; } 
      private:
         bool m_isDefined { false };
         int m_isoConeRel_fw {0};
         int m_fEM_fw {0};
         double m_isoConeRel_d { 0 };
         double m_fEM_d { 0 };      
         unsigned int m_maxEt { 0 };
      };
      L1ThrExtraInfo_eTAU(const std::string & thrTypeName, const ptree & data) :
         L1ThrExtraInfoBase(thrTypeName, data) { load(); }
      virtual ~L1ThrExtraInfo_eTAU() = default;
      virtual std::string className() const { return "L1ThrExtraInfo_eTAU"; }
      float ptMinToTopo() const { return m_ptMinToTopoMeV/1000.0f; }
      unsigned int ptMinToTopoMeV() const { return m_ptMinToTopoMeV; }
      unsigned int ptMinToTopoCounts() const { return energyInCounts( m_ptMinToTopoMeV, resolutionMeV() ); }
      const WorkingPoints_eTAU & isolation(TrigConf::Selection::WP wp, int eta) const { return m_isolation.at(wp).at(eta); }
      const ValueWithEtaDependence<WorkingPoints_eTAU> & isolation(TrigConf::Selection::WP wp) const  { return m_isolation.at(wp); }
   private:
      /** Update the internal members */
      void load();
      /** eTAU specific data */
      unsigned int m_ptMinToTopoMeV{0};
      std::map<TrigConf::Selection::WP, ValueWithEtaDependence<WorkingPoints_eTAU>> m_isolation{};
   };
   std::ostream & operator<<(std::ostream & os, const TrigConf::L1ThrExtraInfo_eTAU::WorkingPoints_eTAU & iso);

   class L1ThrExtraInfo_jTAU final : public L1ThrExtraInfoBase {
   public:
      class WorkingPoints_jTAU {
      public:
         WorkingPoints_jTAU( const boost::property_tree::ptree & );
         bool isDefined() const { return m_isDefined; }
         int isolation_fw() const { return m_isolation_fw; }
         double isolation_d() const { return m_isolation_d; }
         unsigned int maxEt() const { return m_maxEt; }
      private:
         bool m_isDefined { false };
         int m_isolation_fw {0};
         double m_isolation_d { 0 };
         unsigned int m_maxEt { 0 };
      };
      L1ThrExtraInfo_jTAU(const std::string & thrTypeName, const ptree & data) :
         L1ThrExtraInfoBase(thrTypeName, data) { load(); }
      virtual ~L1ThrExtraInfo_jTAU() = default;
      virtual std::string className() const { return "L1ThrExtraInfo_jTAU"; }
      float ptMinToTopo() const { return m_ptMinToTopoMeV/1000.0f; }
      unsigned int ptMinToTopoMeV() const { return m_ptMinToTopoMeV; }
      unsigned int ptMinToTopoCounts() const { return energyInCounts( m_ptMinToTopoMeV, resolutionMeV() ); }
      const WorkingPoints_jTAU & isolation(TrigConf::Selection::WP wp, int eta) const { return m_isolation.at(wp).at(eta); }
      const ValueWithEtaDependence<WorkingPoints_jTAU> & isolation(TrigConf::Selection::WP wp) const  { return m_isolation.at(wp); }
   private:
      /** Update the internal members */
      void load();
     /** jTAU specific data */
      unsigned int m_ptMinToTopoMeV{0};
      std::map<TrigConf::Selection::WP, ValueWithEtaDependence<WorkingPoints_jTAU>> m_isolation{};
   };
   std::ostream & operator<<(std::ostream & os, const TrigConf::L1ThrExtraInfo_jTAU::WorkingPoints_jTAU & iso);

   class L1ThrExtraInfo_cTAU final : public L1ThrExtraInfoBase {
   public:
      class WorkingPoints_cTAU {
      public:
         WorkingPoints_cTAU( const boost::property_tree::ptree & );
         bool isDefined() const { return m_isDefined; }
         int isolation_fw() const { return m_isolation_fw; }
         double isolation_d() const { return m_isolation_d; }
         unsigned int maxEt() const { return m_maxEt; }
      private:
         bool m_isDefined { false };
         int m_isolation_fw {0};
         double m_isolation_d { 0 };
         unsigned int m_maxEt { 0 };
      };
      L1ThrExtraInfo_cTAU(const std::string & thrTypeName, const ptree & data) :
         L1ThrExtraInfoBase(thrTypeName, data) { load(); }
      virtual ~L1ThrExtraInfo_cTAU() = default;
      virtual std::string className() const { return "L1ThrExtraInfo_cTAU"; }
      const WorkingPoints_cTAU & isolation(TrigConf::Selection::WP wp, int eta) const { return m_isolation.at(wp).at(eta); }
      const ValueWithEtaDependence<WorkingPoints_cTAU> & isolation(TrigConf::Selection::WP wp) const  { return m_isolation.at(wp); }
   private:
      /** Update the internal members */
      void load();
     /** cTAU specific data */
      std::map<TrigConf::Selection::WP, ValueWithEtaDependence<WorkingPoints_cTAU>> m_isolation{};
   };
   std::ostream & operator<<(std::ostream & os, const TrigConf::L1ThrExtraInfo_cTAU::WorkingPoints_cTAU & iso);

   class L1ThrExtraInfo_jJ final : public L1ThrExtraInfoBase {
   public:
      L1ThrExtraInfo_jJ(const std::string & thrTypeName, const ptree & data) :
         L1ThrExtraInfoBase(thrTypeName, data) { load(); }
      virtual ~L1ThrExtraInfo_jJ() = default;
      virtual std::string className() const { return "L1ThrExtraInfo_jJ"; }
      double ptMinToTopo(int eta = 0) const { return ptMinToTopoMeV(eta) / 1000.0; }
      unsigned int ptMinToTopoMeV(int eta = 0) const { return m_ptMinToTopoMeV.at(eta); }
      unsigned int ptMinToTopoCounts(int eta = 0) const { return energyInCounts( ptMinToTopoMeV(eta), resolutionMeV() ); }
      const ValueWithEtaDependence<int> & ptMinToTopoEtaMeV() const { return m_ptMinToTopoMeV; }
   private:
      /** Update the internal members */
      void load();
      /** jJ specific data */
      ValueWithEtaDependence<int> m_ptMinToTopoMeV{"jJptMinTopo"};
   };

   class L1ThrExtraInfo_gXE final : public L1ThrExtraInfoBase {
   public:
      L1ThrExtraInfo_gXE(const std::string & thrTypeName, const ptree & data) :
         L1ThrExtraInfoBase(thrTypeName, data) { load(); }
      virtual ~L1ThrExtraInfo_gXE() = default;
      virtual std::string className() const { return "L1ThrExtraInfo_gXE"; }
   private:
      /** Update the internal members */
      void load();
      /** gXE specific data */
   };


   class L1ThrExtraInfo_MU final : public L1ThrExtraInfoBase {
   public:
      L1ThrExtraInfo_MU(const std::string & thrTypeName, const ptree & data) :
         L1ThrExtraInfoBase(thrTypeName, data) { load(); }
      virtual ~L1ThrExtraInfo_MU() = default;
      virtual std::string className() const { return "L1ThrExtraInfo_MU"; }
      unsigned int rpcIdxForPt(unsigned int pt) const;
      unsigned int tgcIdxForPt(unsigned int pt) const;
      std::vector<unsigned int> knownRpcPtValues() const;
      std::vector<unsigned int> knownTgcPtValues() const;
      std::vector<std::string> exclusionListNames() const;
      const std::map<std::string, std::vector<unsigned int>> & exclusionList(const std::string & listName) const;
   private:
      /** Update the internal members */
      void load();
      /** MU specific data */
      std::map<unsigned int, unsigned int> m_rpcPtMap; // map of pt value (1..~20) to road index (0..5)
      std::map<unsigned int, unsigned int> m_tgcPtMap; // map of pt value (1..~30) to road index (0..15)
      std::map<std::string, std::map<std::string, std::vector<unsigned int>>> m_roiExclusionLists; // named lists of excluded rois
   };

}

#endif
