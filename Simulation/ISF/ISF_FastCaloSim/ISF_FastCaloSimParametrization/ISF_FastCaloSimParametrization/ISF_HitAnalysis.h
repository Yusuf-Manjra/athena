/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_HIT_ANALYSIS_H
#define ISF_HIT_ANALYSIS_H

#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/ObjectVector.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "StoreGate/StoreGateSvc.h"
#include "AthenaKernel/IOVSvcDefs.h"

#include "AthenaBaseComps/AthAlgorithm.h"
#include "LArElecCalib/ILArfSampl.h"
#include "CaloDetDescr/CaloDetDescrManager.h"

//#####################################
#include "CaloDetDescr/ICaloCoordinateTool.h"
#include "ISF_FastCaloSimParametrization/FSmap.h"
#include "HepMC/GenParticle.h"
#include "HepPDT/ParticleData.hh"
#include "GaudiKernel/IPartPropSvc.h"
#include "TrkParameters/TrackParameters.h"
//#####################################

#include "ISF_FastCaloSimParametrization/FastCaloSimCaloExtrapolation.h"
#include "ISF_FastCaloSimParametrization/IFastCaloSimGeometryHelper.h"
#include "ISF_FastCaloSimEvent/FastCaloSim_CaloCell_ID.h"

namespace Trk {
  class TrackingVolume;
}
#include "TrkExInterfaces/ITimedExtrapolator.h" 
#include "TrkEventPrimitives/PdgToParticleHypothesis.h"
class ICaloSurfaceHelper;

#include <string>
#include <Rtypes.h>
//#include "TH1.h"

/* *************************************************************
 This is a modified copy of Simulation/Tools/CaloHitAnalysis
 Aug 27, 2013 Zdenek Hubacek (CERN)
************************************************************** */

class TileID;
class TileDetDescrManager;
class TTree;
class ITHistSvc;
class TileInfo;
class LArEM_ID;
class LArFCAL_ID;
class LArHEC_ID;
class IGeoModelSvc;

//############################
class ICaloSurfaceBuilder;
class ICaloCoordinateTool;
class IExtrapolateToCaloTool;
class CaloDepthTool;
namespace Trk {
	  class IExtrapolator;
}

class ISF_HitAnalysis : public AthAlgorithm {

 public:

   ISF_HitAnalysis(const std::string& name, ISvcLocator* pSvcLocator);
   ~ISF_HitAnalysis();

   virtual StatusCode initialize();
   virtual StatusCode finalize();
   virtual StatusCode execute();
   virtual StatusCode geoInit(IOVSVC_CALLBACK_ARGS);
   virtual StatusCode updateMetaData(IOVSVC_CALLBACK_ARGS);

   //bool get_calo_etaphi(std::vector<Trk::HitInfo>* hitVector,CaloCell_ID_FCS::CaloSample sample);
   bool get_calo_etaphi(std::vector<Trk::HitInfo>* hitVector,int sample,int subpos=SUBPOS_MID);
   bool get_calo_surface(std::vector<Trk::HitInfo>* hitVector);
   bool rz_cylinder_get_calo_etaphi(std::vector<Trk::HitInfo>* hitVector, double cylR, double cylZ, Amg::Vector3D& pos, Amg::Vector3D& mom);  
   
   IFastCaloSimGeometryHelper* GetCaloGeometry() const {return &(*m_CaloGeometryHelper);};

 private:

   /** a handle on Store Gate for access to the Event Store */
   //StoreGateSvc* m_storeGate;
   //StoreGateSvc* m_detStore;

   const IGeoModelSvc *m_geoModel;
   const TileInfo *m_tileInfo;
   const LArEM_ID *m_larEmID;
   const LArFCAL_ID *m_larFcalID;
   const LArHEC_ID *m_larHecID;
   const TileID * m_tileID;
   const TileDetDescrManager * m_tileMgr;
   const DataHandle<ILArfSampl>   m_dd_fSampl;
      
   /** Simple variables by Ketevi */
   std::vector<float>* m_hit_x;
   std::vector<float>* m_hit_y;
   std::vector<float>* m_hit_z;
   std::vector<float>* m_hit_energy;
   std::vector<float>* m_hit_time;
   std::vector<Long64_t>* m_hit_identifier;
   std::vector<Long64_t>* m_hit_cellidentifier;
   std::vector<bool>*  m_islarbarrel;
   std::vector<bool>*  m_islarendcap;
   std::vector<bool>*  m_islarhec;
   std::vector<bool>*  m_islarfcal;
   std::vector<bool>*  m_istile;
   std::vector<int>*   m_hit_sampling;
   std::vector<float>* m_hit_samplingfraction;

   std::vector<float>* m_truth_energy;
   std::vector<float>* m_truth_px;
   std::vector<float>* m_truth_py;
   std::vector<float>* m_truth_pz;
   std::vector<int>*   m_truth_pdg;
   std::vector<int>*   m_truth_barcode;
   std::vector<int>*   m_truth_vtxbarcode; //production vertex barcode

   std::vector<Long64_t>* m_cell_identifier;
   std::vector<float>*       m_cell_energy;
   std::vector<int>*         m_cell_sampling;

   std::vector<float>*       m_g4hit_energy;
   std::vector<float>*       m_g4hit_time;
   std::vector<Long64_t>* m_g4hit_identifier;
   std::vector<Long64_t>* m_g4hit_cellidentifier;
   std::vector<float>*       m_g4hit_samplingfraction;
   std::vector<int>*         m_g4hit_sampling;
   //Ok, this won't work, ROOT won't let me save a custom object which it doesn't know about
   //std::vector<zh_matchedcell>* m_matched_cells;

   TTree * m_tree;
   std::string m_ntupleFileName; 
   std::string m_ntupleDirName; 
   std::string m_ntupleTreeName; 
   std::string m_metadataTreeName; 
   std::string m_geoFileName; 
   int m_NtruthParticles;
   ITHistSvc * m_thistSvc;
   const CaloDetDescrManager* m_calo_dd_man;

   //####################################################
   double m_eta_calo_surf;
   double m_phi_calo_surf;
   double m_d_calo_surf;
   double m_ptruth_eta;
   double m_ptruth_phi;
   double m_ptruth_e;
   double m_ptruth_et;
   double m_ptruth_pt;
   double m_ptruth_p;
   int m_pdgid;
   
   std::vector<std::vector<double> >* m_TTC_entrance_eta;
   std::vector<std::vector<double> >* m_TTC_entrance_phi;
   std::vector<std::vector<double> >* m_TTC_entrance_r;
   std::vector<std::vector<double> >* m_TTC_entrance_z;
   std::vector<std::vector<double> >* m_TTC_back_eta;
   std::vector<std::vector<double> >* m_TTC_back_phi;
   std::vector<std::vector<double> >* m_TTC_back_r;
   std::vector<std::vector<double> >* m_TTC_back_z;
   std::vector<double>* m_TTC_IDCaloBoundary_eta;
   std::vector<double>* m_TTC_IDCaloBoundary_phi;
   std::vector<double>* m_TTC_IDCaloBoundary_r;
   std::vector<double>* m_TTC_IDCaloBoundary_z;
   std::vector<double>* m_TTC_Angle3D;
   std::vector<double>* m_TTC_AngleEta;

   /** The new Extrapolator setup */
   ToolHandle<Trk::ITimedExtrapolator>   m_extrapolator;          
   ToolHandle<ICaloSurfaceHelper>   m_caloSurfaceHelper;
   mutable const Trk::TrackingVolume*  m_caloEntrance;
   std::string                         m_caloEntranceName; 
   // extrapolation through Calo
   std::vector<Trk::HitInfo>* caloHits(const HepMC::GenParticle& part ) const;
   Trk::PdgToParticleHypothesis        m_pdgToParticleHypothesis;
   ICaloCoordinateTool*           m_calo_tb_coord;
   /** End new Extrapolator setup */

   //IExtrapolateToCaloTool*      m_etoCalo;
   //IExtrapolateToCaloTool*      m_etoCaloEntrance;
   //CaloDepthTool*               m_calodepth;
   //CaloDepthTool*               m_calodepthEntrance;
   ///ICaloSurfaceBuilder*           m_calosurf;
   //ICaloSurfaceBuilder*           m_calosurf_entrance;
   //Trk::IExtrapolator*            m_extrapolator;
   //std::string                    m_extrapolatorName;
   //std::string                    m_extrapolatorInstanceName;
   //std::string                    m_calosurf_InstanceName;
   //std::string                    m_calosurf_entrance_InstanceName;
   
   CaloCell_ID_FCS::CaloSample    m_sample_calo_surf;
   std::vector< CaloCell_ID_FCS::CaloSample > m_surfacelist;
   
   //CaloGeometryFromCaloDDM* m_CaloGeometry;
   /** The FastCaloSimGeometryHelper tool */
   ToolHandle<IFastCaloSimGeometryHelper> m_CaloGeometryHelper;

   bool   isCaloBarrel(int sample) const;
   double deta(int sample,double eta) const;
   void   minmaxeta(int sample,double eta,double& mineta,double& maxeta) const;
   double rzmid(int sample,double eta) const;
   double rzent(int sample,double eta) const;
   double rzext(int sample,double eta) const;
   double rmid(int sample,double eta) const;
   double rent(int sample,double eta) const;
   double rext(int sample,double eta) const;
   double zmid(int sample,double eta) const;
   double zent(int sample,double eta) const;
   double zext(int sample,double eta) const;
   double rpos(int sample,double eta,int subpos = CaloSubPos::SUBPOS_MID) const;
   double zpos(int sample,double eta,int subpos = CaloSubPos::SUBPOS_MID) const;
   double rzpos(int sample,double eta,int subpos = CaloSubPos::SUBPOS_MID) const;
   
   bool   m_layerCaloOK[CaloCell_ID_FCS::MaxSample][3];
   double m_letaCalo[CaloCell_ID_FCS::MaxSample][3];
   double m_lphiCalo[CaloCell_ID_FCS::MaxSample][3];
   double m_lrCalo[CaloCell_ID_FCS::MaxSample][3];
   double m_lzCalo[CaloCell_ID_FCS::MaxSample][3];
   double m_dCalo[CaloCell_ID_FCS::MaxSample][3];
   double m_distetaCaloBorder[CaloCell_ID_FCS::MaxSample][3];
   
   
   void extrapolate(const HepMC::GenParticle* part,std::vector<Trk::HitInfo>* hitVector);
   void extrapolate_to_ID(const HepMC::GenParticle* part,std::vector<Trk::HitInfo>* hitVector);
   
   HepPDT::ParticleDataTable*     m_particleDataTable;
   
   double m_CaloBoundaryR;
   double m_CaloBoundaryZ;
   double m_calomargin;
   
   std::string m_MC_DIGI_PARAM; 
   std::string m_MC_SIM_PARAM;

   //###################################################################
 	  

};

#endif // ISF_HIT_ANALYSIS_H

