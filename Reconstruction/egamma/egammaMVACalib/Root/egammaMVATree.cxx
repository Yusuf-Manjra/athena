/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <exception>
#include <stdexcept>

#include <TTreeFormula.h>

#include "xAODEgamma/Egamma.h"
#include "xAODEgamma/EgammaDefs.h"
#include "xAODEgamma/Electron.h"
#include "xAODEgamma/EgammaxAODHelpers.h"
#include "xAODEgamma/Photon.h"
#include "xAODCaloEvent/CaloCluster.h"

#include "xAODTracking/Vertex.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackingPrimitives.h"

#include "egammaMVACalib/egammaMVATree.h"


using namespace egammaMVATreeHelpers;

//ClassImp(egammaMVATreeEgamma)
//#ifdef ROOTCORE
//ClassImp(egammaMVATreeElectron)
//#endif

egammaMVATreeEgamma::egammaMVATreeEgamma(const std::string& name,
                                         const std::string& prefix,
                                         const std::set<std::string>& variables)
: TTree(name.c_str(), name.c_str()),
  asg::AsgMessaging(name),
  m_prefix(prefix)
{
  SetCacheSize(0);  // this is to avoid creation of (big) cache for the tree
  init_variables(variables);
}

void egammaMVATreeEgamma::init_variables(const std::set<std::string>& variables)
{
  const std::map<std::string, std::function<float(const xAOD::CaloCluster&)>> map_float_CaloCluster {
    {m_prefix + "_cl_eta", compute_cl_eta},
    {m_prefix + "_cl_phi", compute_cl_phi},
    {m_prefix + "_rawcl_Es0", compute_rawcl_Es0},
    {m_prefix + "_rawcl_Es1", compute_rawcl_Es1},
    {m_prefix + "_rawcl_Es2", compute_rawcl_Es2},
    {m_prefix + "_rawcl_Es3", compute_rawcl_Es3},
    {m_prefix + "_cl_E", [](const xAOD::CaloCluster& cl) { return cl.e(); }},
    {m_prefix + "_cl_etaCalo", compute_cl_etaCalo},
    {m_prefix + "_cl_phiCalo", compute_cl_phiCalo},
    {m_prefix + "_rawcl_calibHitsShowerDepth", compute_rawcl_calibHitsShowerDepth},
    {m_prefix + "_cl_E_TileGap3", [](const xAOD::CaloCluster& cl) { return cl.eSample(CaloSampling::TileGap3); }}
  };

  create_structure<float>(variables, map_float_CaloCluster, m_functions_float_from_calo_cluster);
}

template<> const float* egammaMVATreeEgamma::get_ptr(const std::string& var_name) const
{
  for (const auto& item : m_functions_float_from_calo_cluster) {
    if (std::get<0>(item) == var_name) { return &std::get<2>(item); }
  }
  return nullptr;
}

template<> const int* egammaMVATreeEgamma::get_ptr(const std::string& /*var_name*/) const
{
  // there is no int variables
  return nullptr;
}

void egammaMVATreeEgamma::update(const xAOD::Egamma* egamma)
{
  update(egamma, egamma->caloCluster());
}

void egammaMVATreeEgamma::update(const xAOD::Egamma*, const xAOD::CaloCluster* cluster)
{
  ATH_MSG_DEBUG("updating egamma");
  ATH_MSG_DEBUG(m_functions_float_from_calo_cluster.size() << " float functions");
  if (!cluster and m_functions_float_from_calo_cluster.size() > 0) { ATH_MSG_FATAL("egamma cluster pointer is null"); }
  for (auto& var_function : m_functions_float_from_calo_cluster) {
    auto& var = std::get<2>(var_function);
    const auto& f = std::get<1>(var_function);
    var = f(*cluster);
    ATH_MSG_DEBUG(std::get<0>(var_function) << " = " << var << " == " << std::get<2>(var_function));
  }
}

egammaMVATreeElectron::egammaMVATreeElectron(const std::string& name, const std::set<std::string>& vars)
:egammaMVATreeEgamma(name, "el", vars)
{
  init_variables(vars);
}

void egammaMVATreeElectron::init_variables(const std::set<std::string>& variables)
{
  ATH_MSG_DEBUG("init variable for electron");
  const std::map<std::string, std::function<float(const xAOD::Electron&)>> map_float_Electron {
    {"el_charge", compute_el_charge},
    {"el_tracketa", compute_el_tracketa},
    {"el_trackpt", compute_el_trackpt},
    {"el_trackz0", compute_el_trackz0},
    {"el_refittedTrack_qoverp", compute_el_refittedTrack_qoverp}
  };

  const std::map<std::string, std::function<int(const xAOD::Electron&)>> map_int_Electron {
    {"el_author", compute_el_author}
  };

  create_structure<float>(variables, map_float_Electron, m_functions_float_from_electron);
  create_structure<int>(variables, map_int_Electron, m_functions_int_from_electron);
}

void egammaMVATreeElectron::update(const xAOD::Electron* electron)
{
  update(electron, electron->caloCluster());
}

void egammaMVATreeElectron::update(const xAOD::Electron* electron, const xAOD::CaloCluster* cluster)
{
  egammaMVATreeEgamma::update(electron, cluster);
  ATH_MSG_DEBUG("updating electron");

  ATH_MSG_DEBUG(m_functions_float_from_electron.size() << " float functions");
  if (!electron and !m_functions_float_from_electron.empty()) { ATH_MSG_FATAL("electron pointer is null"); }
  for (auto& var_function : m_functions_float_from_electron) {
    auto& var = std::get<2>(var_function);
    const auto& f = std::get<1>(var_function);
    var = f(*electron);
    ATH_MSG_DEBUG(std::get<0>(var_function) << " = " << var << " == " << std::get<2>(var_function) << " at " << &var);
  }

  ATH_MSG_DEBUG(m_functions_int_from_electron.size() << " int functions");
  if (!electron and !m_functions_int_from_electron.empty()) { ATH_MSG_FATAL("electron pointer is null"); }
  for (auto& var_function : m_functions_int_from_electron) {
    auto& var = std::get<2>(var_function);
    const auto& f = std::get<1>(var_function);
    var = f(*electron);
    ATH_MSG_DEBUG(std::get<0>(var_function) << " = " << var << " == " << std::get<2>(var_function) << " at " << &var);
  }
}

template<> const float* egammaMVATreeElectron::get_ptr(const std::string& var_name) const
{
  const float* var = egammaMVATreeEgamma::get_ptr<float>(var_name);
  if (var) { return var; }
  for (const auto& item : m_functions_float_from_electron) {
    if (std::get<0>(item) == var_name) { return &std::get<2>(item); }
  }
  return nullptr;
}

template<> const int* egammaMVATreeElectron::get_ptr(const std::string& var_name) const
{
  const int* var = egammaMVATreeEgamma::get_ptr<int>(var_name);
  if (var) { return var; }
  for (const auto& item : m_functions_int_from_electron) {
    if (std::get<0>(item) == var_name) { return &std::get<2>(item); }
  }
  return nullptr;
}

//#ifdef ROOTCORE
//ClassImp(egammaMVATreePhoton)
//#endif

egammaMVATreePhoton::egammaMVATreePhoton(const std::string& name,
                                         const std::set<std::string>& vars,
                                         bool force_conversion_to_zero_when_null_photon/*=false*/)
: egammaMVATreeEgamma(name, "ph", vars), m_force_conversion_to_zero_when_null_photon(force_conversion_to_zero_when_null_photon)
{
  init_variables(vars);
}

void egammaMVATreePhoton::init_variables(const std::set<std::string>& variables)
{
  ATH_MSG_DEBUG("init variable for photon");
  const std::map<std::string, std::function<float(const xAOD::Photon&)>> map_float_Photon {
  };

  const std::map<std::string, std::function<int(const xAOD::Photon&)>> map_int_Photon {
    {"ph_convFlag", compute_ph_convFlag}
  };

  const std::map<std::string, std::function<float(const ConversionHelper&)>> map_float_ConversionHelper {
    {"ph_Rconv", [](const ConversionHelper& ch) { return ch.ph_Rconv(); }},
    {"ph_zconv", [](const ConversionHelper& ch) { return ch.ph_zconv(); }},
    {"ph_pt1conv", [](const ConversionHelper& ch) { return ch.ph_pt1conv(); }},
    {"ph_pt2conv", [](const ConversionHelper& ch) { return ch.ph_pt2conv(); }},
    {"ph_ptconv", [](const ConversionHelper& ch) { return ch.ph_ptconv(); }}
  };

  const std::map<std::string, std::function<int(const ConversionHelper&)>> map_int_ConversionHelper {
    {"ph_convtrk1nPixHits", [](const ConversionHelper& ch) { return ch.ph_convtrk1nPixHits(); }},
    {"ph_convtrk2nPixHits", [](const ConversionHelper& ch) { return ch.ph_convtrk2nPixHits(); }},
    {"ph_convtrk1nSCTHits", [](const ConversionHelper& ch) { return ch.ph_convtrk1nSCTHits(); }},
    {"ph_convtrk2nSCTHits", [](const ConversionHelper& ch) { return ch.ph_convtrk2nSCTHits(); }}
  };

  create_structure<float>(variables, map_float_Photon, m_functions_float_from_photon);
  create_structure<int>(variables, map_int_Photon, m_functions_int_from_photon);
  create_structure<float>(variables, map_float_ConversionHelper, m_functions_float_from_ConversionHelper);
  create_structure<int>(variables, map_int_ConversionHelper, m_functions_int_from_ConversionHelper);
}

void egammaMVATreePhoton::update(const xAOD::Photon* photon)
{
  update(photon, photon->caloCluster());
}

void egammaMVATreePhoton::update(const xAOD::Photon* photon, const xAOD::CaloCluster* cluster)
{
  egammaMVATreeEgamma::update(photon, cluster);
  ATH_MSG_DEBUG("updating photon");

  ATH_MSG_DEBUG(m_functions_float_from_photon.size() << " float functions");
  if (!photon and !m_functions_float_from_photon.empty()) { ATH_MSG_FATAL("photon pointer is null"); }
  for (auto& var_function : m_functions_float_from_photon) {
    auto& var = std::get<2>(var_function);
    const auto& f = std::get<1>(var_function);
    var = f(*photon);
    ATH_MSG_DEBUG(std::get<0>(var_function) << " = " << var << " == " << std::get<2>(var_function) << " at " << &var);
  }

  if (!photon and !m_functions_int_from_photon.empty()) { ATH_MSG_FATAL("photon pointer is null"); }
  ATH_MSG_DEBUG(m_functions_int_from_photon.size() << " int functions");
  for (auto& var_function : m_functions_int_from_photon) {
    auto& var = std::get<2>(var_function);
    const auto& f = std::get<1>(var_function);
    var = f(*photon);
    ATH_MSG_DEBUG(std::get<0>(var_function) << " = " << var << " == " << std::get<2>(var_function) << " at " << &var);
  }

  if (m_functions_int_from_ConversionHelper.size() > 0 or
      m_functions_float_from_ConversionHelper.size() > 0)
  {
    if (!photon and !m_force_conversion_to_zero_when_null_photon) { ATH_MSG_FATAL("photon pointer is null"); }
    ATH_MSG_DEBUG("computing function with Conversion Helper");

    if (!photon and m_force_conversion_to_zero_when_null_photon) {
      // this is for the trigger calibration. For historical reason it needs
      // the radius of conversion to be present and equal to zero. This is
      // used to distinguish between convertend and unconverted. In this case
      // all photon will be threated as unconverted, but the tool expect two
      // files (converted / unconverted), the converted is never used for
      // trigger calibration. TODO: remove when improved support for multiple
      // MVAs (converted / unconverted / ...)
      ATH_MSG_DEBUG("forcing conversion variables to zero since photon is null");
      for (auto& var_function : m_functions_float_from_ConversionHelper) {
        auto& var = std::get<2>(var_function);
        var = 0;
      }
      for (auto& var_function : m_functions_int_from_ConversionHelper) {
        auto& var = std::get<2>(var_function);
        var = 0;
      }
    }
    else {  // this is the normal case
      const ConversionHelper conv_helper(photon);  // something predigested

      ATH_MSG_DEBUG(m_functions_float_from_ConversionHelper.size() << " int functions");
      for (auto& var_function : m_functions_float_from_ConversionHelper) {
        auto& var = std::get<2>(var_function);
        const auto& f = std::get<1>(var_function);
        var = f(conv_helper);
        ATH_MSG_DEBUG(std::get<0>(var_function) << " = " << var << " == " << std::get<2>(var_function) << " at " << &var);
      }

      ATH_MSG_DEBUG(m_functions_int_from_ConversionHelper.size() << " int functions");
      for (auto& var_function : m_functions_int_from_ConversionHelper) {
        auto& var = std::get<2>(var_function);
        const auto& f = std::get<1>(var_function);
        var = f(conv_helper);
        ATH_MSG_DEBUG(std::get<0>(var_function) << " = " << var << " == " << std::get<2>(var_function) << " at " << &var);
      }
    }
  }
}

template<> const float* egammaMVATreePhoton::get_ptr(const std::string& var_name) const
{
  const float* var = egammaMVATreeEgamma::get_ptr<float>(var_name);
  if (var) { return var; }
  for (const auto& item : m_functions_float_from_photon) {
    if (std::get<0>(item) == var_name) { return &std::get<2>(item); }
  }
  for (const auto& item : m_functions_float_from_ConversionHelper) {
    if (std::get<0>(item) == var_name) { return &std::get<2>(item); }
  }
  return nullptr;
}

template<> const int* egammaMVATreePhoton::get_ptr(const std::string& var_name) const
{
  const int* var = egammaMVATreeEgamma::get_ptr<int>(var_name);
  if (var) { return var; }
  for (const auto& item : m_functions_int_from_photon) {
    if (std::get<0>(item) == var_name) { return &std::get<2>(item); }
  }
  for (const auto& item : m_functions_int_from_ConversionHelper) {
    if (std::get<0>(item) == var_name) { return &std::get<2>(item); }
  }
  return nullptr;
}
