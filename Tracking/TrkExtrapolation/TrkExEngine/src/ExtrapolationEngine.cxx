/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ExtrapolationEngine.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// STL
#include <sstream>
// Trk include
#include "TrkExEngine/ExtrapolationEngine.h"
#include "TrkDetDescrUtils/GeometrySignature.h"

// constructor
Trk::ExtrapolationEngine::ExtrapolationEngine(const std::string& t, const std::string& n, const IInterface* p)
: AthAlgTool(t,n,p),
  m_trackingGeometry(0),
  m_trackingGeometrySvc("AtlasTrackingGeometrySvc", n),
  m_trackingGeometryName("AtlasTrackingGeometry"),
  m_forceSearchInit(false)
{
    declareInterface<Trk::IExtrapolationEngine>(this);
    
    // Geometry retrieval
    declareProperty("TrackingGeometrySvc"                   , m_trackingGeometrySvc);
    // Extrapolation Engine retrieval 
    declareProperty("ExtrapolationEngines"                  , m_extrapolationEngines);
    // Material Updator Engine retrival
    
    // steering of the screen outoput (SOP)
    declareProperty("OutputPrefix"                          , m_sopPrefix);
    declareProperty("OutputPostfix"                         , m_sopPostfix);
    // the properties to be given 
    declareProperty("ForceSearchAtInit"                     , m_forceSearchInit);
}

// destructor
Trk::ExtrapolationEngine::~ExtrapolationEngine()
{}


// the interface method initialize
StatusCode Trk::ExtrapolationEngine::initialize()
{

            
    EX_MSG_INFO( "init", " starting initialize()" );

    // get the TrackingGeometrySvc
    if (m_trackingGeometrySvc.retrieve().isSuccess()){
        EX_MSG_INFO( "init", "Successfully retrieved " << m_trackingGeometrySvc );
        m_trackingGeometryName = m_trackingGeometrySvc->trackingGeometryName();
    } else {
        EX_MSG_WARNING( "init", "Couldn't retrieve " << m_trackingGeometrySvc << ". " );
        EX_MSG_WARNING( "init", " -> Trying to retrieve default '" << m_trackingGeometryName << "' from DetectorStore. Abort." );
        return StatusCode::FAILURE;
    }
    
    // retrieve the ExtrapolationEngines
    if (m_extrapolationEngines.retrieve().isFailure()){
        
    } else {
        EX_MSG_INFO( "init", "Successfully retrieved " << m_extrapolationEngines.size() << " ExtrapolationEngines. Ordering them now." );
        m_eeAccessor = std::vector<const Trk::IExtrapolationEngine*>(int(Trk::NumberOfGeometryTypes), (const Trk::IExtrapolationEngine*)NULL);
        for (auto& ee : m_extrapolationEngines){
            EX_MSG_DEBUG( "init", "Registering " << ee->name() << " - for GeometryType : "  << ee->geometryType() );
            m_eeAccessor[ee->geometryType()] = (&*ee);
        }
    }
    
    
    
    return StatusCode::SUCCESS;
}    

// the interface method finalize
StatusCode Trk::ExtrapolationEngine::finalize()
{    
    EX_MSG_INFO( "fini", "finalize() successful" );    
    return StatusCode::SUCCESS;
}


/** charged extrapolation */
Trk::ExtrapolationCode Trk::ExtrapolationEngine::extrapolate(ExCellCharged& ecCharged,
                                                        const Surface* sf,
                                                        PropDirection dir,
                                                        BoundaryCheck bcheck) const
{ return extrapolateT<TrackParameters>(ecCharged,sf,dir,bcheck); }


/** neutral extrapolation */
Trk::ExtrapolationCode Trk::ExtrapolationEngine::extrapolate(ExCellNeutral& ecNeutral,
                                                        const Surface* sf,
                                                        PropDirection dir,
                                                        BoundaryCheck bcheck) const
{ return extrapolateT<NeutralParameters>(ecNeutral,sf,dir,bcheck); }
                                           

StatusCode Trk::ExtrapolationEngine::updateTrackingGeometry() const {
    // retrieve the TrackingGeometry from the detector store 
    if (detStore()->retrieve(m_trackingGeometry, m_trackingGeometryName).isFailure()){
        EX_MSG_FATAL( "tgeo", "Could not retrieve TrackingGeometry '" << m_trackingGeometryName << "' from DetectorStore." );
        EX_MSG_FATAL( "tgeo", "  - probably the chosen layout is not supported / no cool tag exists. "                     );
        return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;
}


