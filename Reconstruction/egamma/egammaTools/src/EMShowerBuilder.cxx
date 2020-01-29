/*
   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

// INCLUDE HEADER FILES:
#include "GaudiKernel/IChronoStatSvc.h"
#include "GaudiKernel/EventContext.h"
#include "StoreGate/ReadHandle.h"

#include "EMShowerBuilder.h"
#include "egammaInterfaces/IegammaShowerShape.h"
#include "egammaInterfaces/IegammaIso.h"

#include "CaloUtils/CaloCellList.h"
#include "CaloEvent/CaloCellContainer.h"
#include "xAODEgamma/Egamma.h"
#include "xAODEgamma/Electron.h"
#include "xAODEgamma/Photon.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODTracking/TrackParticleContainer.h" 
#include "xAODTracking/VertexContainer.h" 

#include "FourMomUtils/P4Helpers.h"
#include <algorithm> 
#include <cmath>

//  END OF HEADER FILES INCLUDE

// ===========================================================================
EMShowerBuilder::EMShowerBuilder(const std::string& type,
        const std::string& name,
        const IInterface* parent)
: egammaBaseTool(type, name, parent)
{
    //
    // constructor
    //

    // declare interface
    declareInterface<IEMShowerBuilder>(this);
}

// ===============================================================
EMShowerBuilder::~EMShowerBuilder()
{
    //
    // destructor
    // 
}

// ==================================================================
StatusCode EMShowerBuilder::initialize()
{
    //
    // initialize method
    //

    ATH_MSG_DEBUG(" Initializing EMShowerBuilder, m_cellKey = " << m_cellsKey.key());

    ATH_CHECK(m_cellsKey.initialize((m_UseShowerShapeTool || m_UseCaloIsoTool) && 
                m_cellsKey.key() != ""));

    
    if (m_UseShowerShapeTool) { 
        if ((RetrieveShowerShapeTool()).isFailure()) { 
            return StatusCode::FAILURE; 
        } else {
            m_ShowerShapeTool.disable();
        }
    }
    // 
    // call calorimeter isolation tool only if needed
    //
    if (m_UseCaloIsoTool) {
        if ((RetrieveHadronicLeakageTool()).isFailure()) {
            return StatusCode::FAILURE; 
        } else {
            m_HadronicLeakageTool.disable();
        }
    }
    // for measuring the timing

    return StatusCode::SUCCESS;
}

StatusCode EMShowerBuilder::RetrieveShowerShapeTool()
{
    //
    // retrieve shower shape tool
    //

    if (m_ShowerShapeTool.empty()) {
        ATH_MSG_INFO("ShowerShape is empty");
        return StatusCode::SUCCESS;
    } 

    // retrieve egammaShowerShape Tool
    if(m_ShowerShapeTool.retrieve().isFailure()) {
        ATH_MSG_ERROR("Unable to retrieve "<<m_ShowerShapeTool);
        return StatusCode::FAILURE;
    } 
    else ATH_MSG_DEBUG("Retrieved Tool " << m_ShowerShapeTool); 
    return StatusCode::SUCCESS;
}


StatusCode EMShowerBuilder::RetrieveHadronicLeakageTool()
{
    //
    // retrieve EMCaloIsolationTool
    //

    if (m_HadronicLeakageTool.empty()) {
        ATH_MSG_INFO("HadronicLeakageTool is empty");
        return StatusCode::SUCCESS;
    } 

    if(m_HadronicLeakageTool.retrieve().isFailure()) {
        ATH_MSG_ERROR("Unable to retrieve "<<m_HadronicLeakageTool);
        return StatusCode::FAILURE;
    } 
    else ATH_MSG_DEBUG("Retrieved Tool "<<m_HadronicLeakageTool);

    return StatusCode::SUCCESS;
}

StatusCode EMShowerBuilder::finalize(){
    return StatusCode::SUCCESS;
}

StatusCode EMShowerBuilder::execute(const EventContext& ctx, xAOD::Egamma* eg) const { 
    // 
    // execute method as used in offline reconstruction
    // 
    ATH_MSG_DEBUG("Executing EMShowerBuilder::execute");
    // protection against bad pointers
    if (eg==nullptr) return StatusCode::SUCCESS;

    // retrieve the cell containers
    SG::ReadHandle<CaloCellContainer> cellcoll(m_cellsKey, ctx);
    // check is only used for serial running; remove when MT scheduler used
    if(!cellcoll.isValid()) {
        ATH_MSG_ERROR("Failed to retrieve cell container: "<< m_cellsKey.key());
        return StatusCode::FAILURE;
    }
    if(recoExecute(eg,cellcoll.cptr()).isFailure()) {
        ATH_MSG_WARNING("execute(egamma* eg) returns failure on recoExecute"); 
        return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;
}


StatusCode EMShowerBuilder::recoExecute(xAOD::Egamma* eg, const CaloCellContainer* cellcoll) const
{ 
    // 
    // execute method as used by online reconstruction
    // 
    ATH_MSG_DEBUG("Executing recoExecute");
    // calculate shower shapes
    CHECK(CalcShowerShape(eg,cellcoll));
    ATH_MSG_DEBUG("Exiting recoExecute");
    return StatusCode::SUCCESS;
}

StatusCode EMShowerBuilder::CalcShowerShape(xAOD::Egamma* eg,const CaloCellContainer* cellcoll) const
{ 
    // 
    // Estimate shower shapes and fill the EMShower object associated to eg
    // 
    ATH_MSG_DEBUG("Executing CalcShowerShape");
    // protection against bad pointers
    if (eg==nullptr) {return StatusCode::SUCCESS;}
    StatusCode sc = StatusCode::SUCCESS;
    // retrieve the cluster 
    const xAOD::CaloCluster* clus = eg->caloCluster(); 
    // Protect against non-existent structures.
    if (clus == nullptr) {
        ATH_MSG_WARNING("No Cluster");
        return StatusCode::SUCCESS; 
    }
    if (cellcoll== nullptr) {
        ATH_MSG_WARNING("No cells");
        return StatusCode::SUCCESS; 
    }
    // call calorimeter isolation tool only if needed
    if (m_UseCaloIsoTool){
           CHECK(CalcHadronicLeakage(eg,clus,cellcoll));
    } 

    // Calculate shower shapes in all samplings
    if (m_UseShowerShapeTool) {
        // protection in case tool does not exist 
        IegammaShowerShape::Info info;
        if (!m_ShowerShapeTool.empty()) {
            sc = m_ShowerShapeTool->execute(*clus,*cellcoll,info);
            if ( sc.isFailure() ) {
                ATH_MSG_WARNING("call to ShowerShape returns failure ");
            } else {
                CHECK(FillEMShowerShape(eg,info));
            }
        }
    }
    return sc;
}

// ==========================================================================
StatusCode EMShowerBuilder::CalcHadronicLeakage(xAOD::Egamma* eg,const xAOD::CaloCluster* clus,
        const CaloCellContainer* cellcoll) const {
    //
    // Call calorimeter isolation tool
    //
    ATH_MSG_DEBUG("Executing CalcHadronicLeakage");
    if ( m_caloNums.size() <3 ) {
        ATH_MSG_DEBUG("Less than 3 subCalos, skipping");
        return StatusCode::SUCCESS;
    }
    // protection in case tool does not exist
    if (m_HadronicLeakageTool.empty()) {
        return StatusCode::SUCCESS;
    }
    // define a new Calo Cell list corresponding to HAD Calo
    // retrieve the corresponding CaloCell_ID for LarHec and TILE
    static const std::vector<CaloCell_ID::SUBCALO> theVecCalo={CaloCell_ID::LARHEC,CaloCell_ID::TILE};
   // define a new Calo Cell list
    CaloCellList HADccl(cellcoll,theVecCalo);
    // calculate information concerning just the hadronic leakage
    IegammaIso::Info info;
    StatusCode sc =  m_HadronicLeakageTool->execute(*clus,HADccl,info);
    if ( sc.isFailure() ) {
        ATH_MSG_WARNING("call to Iso returns failure for execute");
        return sc;
    }
    float value=0;
    /// @brief ethad/et
    const double et = eg->caloCluster()->et();
    value=static_cast<float>(info.ethad1);
    eg->setShowerShapeValue(value, xAOD::EgammaParameters::ethad1);
    eg->setShowerShapeValue( et != 0. ? value/et : 0., xAOD::EgammaParameters::Rhad1);
    value=static_cast<float>(info.ethad);
    eg->setShowerShapeValue(value, xAOD::EgammaParameters::ethad);
    eg->setShowerShapeValue( et != 0. ? value/et : 0., xAOD::EgammaParameters::Rhad);
    value=static_cast<float>(info.ehad1);
    eg->setShowerShapeValue(value, xAOD::EgammaParameters::ehad1);
    return StatusCode::SUCCESS;
}

StatusCode EMShowerBuilder::FillEMShowerShape(xAOD::Egamma* eg, const IegammaShowerShape::Info& info) const { 
    
  // protection in case Tool does not exist
    if (m_ShowerShapeTool.empty()) {
        return StatusCode::SUCCESS;
    }
    // information in the presampler
    
    // E in 1x1 cells in pre sampler
    float value=0;
    // E in 1x1 cells in pre sampler
    value=static_cast<float>(info.e011);
    eg->setShowerShapeValue(value, xAOD::EgammaParameters::e011);
    // E in 3x3 cells in pre sampler
    value=static_cast<float>(info.e033);
    eg->setShowerShapeValue(value, xAOD::EgammaParameters::e033);
    // E in 3x2 cells in S1
    value=static_cast<float>(info.e132);
    eg->setShowerShapeValue(value, xAOD::EgammaParameters::e132);
    // E in 15x2 cells in S1 
    value=static_cast<float>(info.e1152);
    eg->setShowerShapeValue(value, xAOD::EgammaParameters::e1152);
    // fraction of E in S1
    value=static_cast<float>(info.f1);
    eg->setShowerShapeValue(value, xAOD::EgammaParameters::f1);
    // fraction of E in the core(e132) in S1
    value=static_cast<float>(info.f1core);
    eg->setShowerShapeValue(value, xAOD::EgammaParameters::f1core);
    // corr width with 3 strips 
    value=static_cast<float>(info.ws3c);
    eg->setShowerShapeValue(value, xAOD::EgammaParameters::weta1);
    // energy in second max
    value=static_cast<float>(info.esec);
    eg->setShowerShapeValue(value, xAOD::EgammaParameters::e2ts1);
    // energy strip of second max
    value=static_cast<float>(info.esec1);
    eg->setShowerShapeValue(value, xAOD::EgammaParameters::e2tsts1);
    // fraction of E outside core in S1
    value=static_cast<float>(info.fside);
    eg->setShowerShapeValue(value, xAOD::EgammaParameters::fracs1);
    // width with 5 strips
    value=static_cast<float>(info.widths5);
    eg->setShowerShapeValue(value, xAOD::EgammaParameters::widths1);
    //eta pos within cell in S1
    value=static_cast<float>(info.poscs1);
    eg->setShowerShapeValue(value, xAOD::EgammaParameters::poscs1);
    // asymmetry with 3 strips
    value=static_cast<float>(info.asymmetrys3);
    eg->setShowerShapeValue(value, xAOD::EgammaParameters::asy1);
    // diff position +/- 1 cells
    value=static_cast<float>(info.deltaEtaTrackShower);
    eg->setShowerShapeValue(value, xAOD::EgammaParameters::pos);
    // diff position +/- 7 cells
    value=static_cast<float>(info.deltaEtaTrackShower7);
    eg->setShowerShapeValue(value, xAOD::EgammaParameters::pos7);
    // E of strip with min E 
    value=static_cast<float>(info.emins1);
    eg->setShowerShapeValue(value, xAOD::EgammaParameters::emins1);
    // E of strip with max E
    value=static_cast<float>(info.emaxs1);
    eg->setShowerShapeValue(value, xAOD::EgammaParameters::emaxs1);
    // barycentre in eta in S1
    value=static_cast<float>(info.etas3);
    eg->setShowerShapeValue(value, xAOD::EgammaParameters::barys1);
    // total width in strips
    value=static_cast<float>(info.wstot);
    eg->setShowerShapeValue(value, xAOD::EgammaParameters::wtots1);
    //
    // information in the 2nd sampling
    //
    // E in 3x3 cells in S2
    value=static_cast<float>(info.e233);
    eg->setShowerShapeValue(value, xAOD::EgammaParameters::e233);
    // E in 3x5 cells in S2
    value=static_cast<float>(info.e235);
    eg->setShowerShapeValue(value, xAOD::EgammaParameters::e235);
    // E in 3x7 cells in S2
    value=static_cast<float>(info.e237);
    eg->setShowerShapeValue(value, xAOD::EgammaParameters::e237);
    // E in 5x5 cells in S2
    value=static_cast<float>(info.e255);
    eg->setShowerShapeValue(value, xAOD::EgammaParameters::e255);
    // E in 7x7 cells in S2
    value=static_cast<float>(info.e277);
    eg->setShowerShapeValue(value, xAOD::EgammaParameters::e277);
    // corr width in S2
    value=static_cast<float>(info.etaw);
    eg->setShowerShapeValue(value, xAOD::EgammaParameters::weta2);
    // uncorr width in S2
    value=static_cast<float>(info.width);
    eg->setShowerShapeValue(value, xAOD::EgammaParameters::widths2);
    // position in eta within cell in S2
    value=static_cast<float>(info.poscs2);
    eg->setShowerShapeValue(value, xAOD::EgammaParameters::poscs2);
    //
    // information in the 3rd sampling
    //
    // fraction of E in S3 
    value=static_cast<float>(info.f3);
    eg->setShowerShapeValue(value, xAOD::EgammaParameters::f3);
    // fraction of E in the core (e333) in S3 
    value=static_cast<float>(info.f3core);
    eg->setShowerShapeValue(value, xAOD::EgammaParameters::f3core);
    // E in 3x3 cells in S3
    value=static_cast<float>(info.e333);
    eg->setShowerShapeValue(value, xAOD::EgammaParameters::e333);
    // E in 3x5 cells in S3
    value=static_cast<float>(info.e335);
    eg->setShowerShapeValue(value, xAOD::EgammaParameters::e335);
    // E in 3x7 cells in S3
    value=static_cast<float>(info.e337);
    eg->setShowerShapeValue(value, xAOD::EgammaParameters::e337);
    // E in 7x7 cells in S3
    value=static_cast<float>(info.e377);
    eg->setShowerShapeValue(value, xAOD::EgammaParameters::e377);
    //
    // information combining all samplings
    //
    // ratio of energy in 3x3/3x7 cells
    value=static_cast<float>(info.reta3337_allcalo);
    eg->setShowerShapeValue(value, xAOD::EgammaParameters::r33over37allcalo);
    // core energy
    value=static_cast<float>(info.ecore);
    eg->setShowerShapeValue(value, xAOD::EgammaParameters::ecore);
    //
    // information combining different shower shape
    //
    float valueSecond=0;
    /// @brief  e237/e277
    value=static_cast<float>(info.e277);
    valueSecond=static_cast<float>(info.e237);
    eg->setShowerShapeValue( value != 0 ? valueSecond/value : 0., xAOD::EgammaParameters::Reta);
    /// @brief  e233/e237
    value=static_cast<float>(info.e233);
    valueSecond=static_cast<float>(info.e237);
    eg->setShowerShapeValue( valueSecond != 0 ? value/valueSecond : 0., xAOD::EgammaParameters::Rphi);
    /// @brief (emaxs1-e2tsts1)/(emaxs1+e2tsts1)
    value=static_cast<float>(info.emaxs1);
    valueSecond=static_cast<float>(info.esec1);
    eg->setShowerShapeValue(fabs(value+valueSecond)>0. ? (value-valueSecond)/(value+valueSecond) : 0., xAOD::EgammaParameters::Eratio);

    value=static_cast<float>(info.emins1);
    eg->setShowerShapeValue( (valueSecond-value), xAOD::EgammaParameters::DeltaE);
    return StatusCode::SUCCESS;
}

