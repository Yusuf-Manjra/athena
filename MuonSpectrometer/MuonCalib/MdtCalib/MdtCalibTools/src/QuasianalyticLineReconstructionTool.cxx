/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MdtCalibTools/QuasianalyticLineReconstructionTool.h"
#include "GaudiKernel/MsgStream.h"
#include "MdtCalibFitters/QuasianalyticLineReconstruction.h"

using namespace MuonCalib;

//::::::::::::::::::::::::::::::::::::::::::::::::::::
//:: IMPLEMENTATION OF METHODS DEFINED IN THE CLASS ::
//::       QuasianalyticLineReconstructionTool      ::
//::::::::::::::::::::::::::::::::::::::::::::::::::::

//*****************************************************************************

//:::::::::::::::::
//:: CONSTRUCTOR ::
//:::::::::::::::::

QuasianalyticLineReconstructionTool::QuasianalyticLineReconstructionTool(
	const std::string & type, const std::string & name, 
	const IInterface *parent) : AthAlgTool(type, name, parent) {

	declareInterface< IMdtSegmentFitterTool>(this);

	m_road_width = 0.5;
	declareProperty("roadWidth", m_road_width);

	m_refit = false;
	declareProperty("refit", m_refit);

	return;

}

//*****************************************************************************

//:::::::::::::::::::::::
//:: METHOD initialize ::
//:::::::::::::::::::::::

StatusCode QuasianalyticLineReconstructionTool::initialize() {

	MsgStream log(msgSvc(), name());
	log << MSG::INFO << "initialize     " << endmsg;

// create an instance of QuasianalyticLineReconstruction //
	m_implementation = new QuasianalyticLineReconstruction(m_road_width);

	if (m_refit) {
		static_cast<QuasianalyticLineReconstruction*>(m_implementation
							)->switchOnRefit();
	} else {
		static_cast<QuasianalyticLineReconstruction*>(m_implementation
							)->switchOffRefit();
	}
  
	return StatusCode::SUCCESS;

}

//*****************************************************************************

//:::::::::::::::::::::
//:: METHOD finalize ::
//:::::::::::::::::::::

StatusCode QuasianalyticLineReconstructionTool::finalize() {

	MsgStream log(msgSvc(), name());
	log << MSG::INFO << "finalize     " << endmsg;

	delete m_implementation;

	return StatusCode::SUCCESS;

}
