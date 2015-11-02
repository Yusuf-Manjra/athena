/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// BeamCondSvc.cxx - service giving beamspot data
// Richard Hawkings, started 16/6/05

#include "GaudiKernel/SvcFactory.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "CoralBase/AttributeListException.h"
#include "EventPrimitives/EventPrimitives.h"
#include "BeamCondSvc.h"

// name of the folder for beamspot information
#define INDET_BEAMPOS "/Indet/Beampos"


BeamCondSvc::BeamCondSvc(const std::string& name, ISvcLocator* svc) :
  AthService(name,svc),
  p_detstore("DetectorStore",name),
  par_usedb(true),
  par_status(1),
  par_posx(0.),
  par_posy(0.),
  par_posz(0.),
  par_sigx(0.015),
  par_sigy(0.015),
  par_sigz(53.),
  par_sigxy(0.),
  par_tiltx(0.),
  par_tilty(0.),
  m_status(0)
{
  // declare properties
  declareProperty("useDB",par_usedb);
  declareProperty("status",par_status);
  declareProperty("posX",par_posx);
  declareProperty("posY",par_posy);
  declareProperty("posZ",par_posz);
  declareProperty("sigmaX",par_sigx);
  declareProperty("sigmaY",par_sigy);
  declareProperty("sigmaZ",par_sigz);
  declareProperty("sigmaXY",par_sigxy);
  declareProperty("tiltX",par_tiltx);
  declareProperty("tiltY",par_tilty);
}

BeamCondSvc::~BeamCondSvc() {}

const InterfaceID& BeamCondSvc::type() const
{ 
return IBeamCondSvc::interfaceID();
}

StatusCode BeamCondSvc::queryInterface(const InterfaceID& riid, void** ppvInterface)
{
  if (IBeamCondSvc::interfaceID().versionMatch(riid)) {
    *ppvInterface=(IBeamCondSvc*)this;
  } else {
    return AthService::queryInterface(riid,ppvInterface);
  }
  return StatusCode::SUCCESS;
}

StatusCode BeamCondSvc::initialize()
{
  // service initialisation - get parameters, setup default cache
  // and register for condDB callbacks if needed
  msg(MSG::INFO) << "in initialize()" << endreq;

  // get detector store
  if (p_detstore.retrieve().isFailure()) {
    msg(MSG::FATAL) << "Detector store not found" << endreq; 
    return StatusCode::FAILURE;
  }
  // always init cache to joboption values in case CondDB read fails
  initCache(par_status,par_posx,par_posy,par_posz,par_sigx,par_sigy,par_sigz,
	      par_sigxy,par_tiltx,par_tilty);
  if (par_usedb) {
    // register callback function for cache updates
    const DataHandle<AthenaAttributeList> aptr;
    if (StatusCode::SUCCESS==p_detstore->regFcn(&BeamCondSvc::update,this,
						aptr, INDET_BEAMPOS )) {
      msg(MSG::DEBUG) << "Registered callback for beam position" << endreq;
    } else {
      msg(MSG::ERROR) << "Callback registration failed" << endreq;
    }
  } else {
    msg(MSG::INFO) << 
     "Default beamspot parameters will be used (from jobopt) " << 
      endreq << "Beamspot status " << par_status << 
      endreq << "Beamspot position  (" << par_posx << "," << par_posy << "," <<
      par_posz << ")" << endreq << "RMS size (" << par_sigx << "," << par_sigy
	<< "," << par_sigz << ")" << endreq << "Tilt xz yz/radian (" << 
      par_tiltx << "," << par_tilty << ")" << endreq;
  }
  return StatusCode::SUCCESS;
}

StatusCode BeamCondSvc::finalize() {
  return StatusCode::SUCCESS;
}

void BeamCondSvc::initCache(int status, float x, float y, float z, 
			    float sx, float sy, float sz, float sxy,
			    float tx, float ty) {
  // set cache values from given parameters (comes from jobopt or condDB)
  m_status=status;
  m_beampos=Amg::Vector3D(x,y,z);
  m_errpar[0]=sx;
  m_errpar[1]=sy;
  m_errpar[2]=sz;
  m_errpar[3]=tx;
  m_errpar[4]=ty;
  m_errpar[5]=sxy;
  // calculate corresponding error matrix
  float xx=tan(tx)*sz;
  float yy=tan(ty)*sz;
  float six=sqrt(sx*sx+xx*xx);
  float siy=sqrt(sy*sy+yy*yy);
  Amg::MatrixX beamerr(3,3);
  beamerr.setZero();
  beamerr.fillSymmetric(0,0,six*six);
  beamerr.fillSymmetric(0,1,sxy);
  beamerr.fillSymmetric(1,1,siy*siy);
  beamerr.fillSymmetric(0,2,tan(tx)*sz*sz);
  beamerr.fillSymmetric(1,2,tan(ty)*sz*sz);
  beamerr.fillSymmetric(2,2,sz*sz);
  // fill in RecVertex information, ndof and chisq are set to 0 by default
  m_vertex=Trk::RecVertex(m_beampos,beamerr);
}

bool BeamCondSvc::fillRec() const {
  // fill beamspot information from jobOptions into TDS AttributeList
  // for registration into conditions database
  // for use in updating database only

 // create spec for Beampos object
  coral::AttributeListSpecification* aspec=
    new coral::AttributeListSpecification();
  aspec->extend("status","int");
  aspec->extend("posX","float");
  aspec->extend("posY","float");
  aspec->extend("posZ","float");
  aspec->extend("sigmaX","float");
  aspec->extend("sigmaY","float");
  aspec->extend("sigmaZ","float");
  aspec->extend("tiltX","float");
  aspec->extend("tiltY","float");
  aspec->extend("sigmaXY","float");
  if (!aspec->size()) msg(MSG::ERROR) << 
			"Attribute list specification is empty!" << endreq;
  AthenaAttributeList* alist=new AthenaAttributeList(*aspec);
  // set status to 1 for now - for future use
  (*alist)["status"].setValue(par_status);
  (*alist)["posX"].setValue(par_posx);
  (*alist)["posY"].setValue(par_posy);
  (*alist)["posZ"].setValue(par_posz);
  (*alist)["sigmaX"].setValue(par_sigx);
  (*alist)["sigmaY"].setValue(par_sigy);
  (*alist)["sigmaZ"].setValue(par_sigz);
  (*alist)["tiltX"].setValue(par_tiltx);
  (*alist)["tiltY"].setValue(par_tilty);
  (*alist)["sigmaXY"].setValue(par_sigxy);
  // record Beampos object in TDS
  if (StatusCode::SUCCESS==p_detstore->record(alist, INDET_BEAMPOS )) {
    if (msgLvl(MSG::INFO)) msg() << "Recorded Beampos object in TDS" << endreq;
  } else {
    msg(MSG::ERROR) << "Could not record Beampos object" << endreq;
    return false;
  }
  return true;
}

StatusCode BeamCondSvc::update( IOVSVC_CALLBACK_ARGS_P(I,keys) ) {
  // callback function to update beamspot cache when condDB data changes
  if (msgLvl(MSG::DEBUG)) 
    msg() << "update callback invoked for I=" << I << " keys: ";
  for (std::list<std::string>::const_iterator itr=keys.begin();
       itr!=keys.end(); ++itr) msg() << " " << *itr;
  msg() << endreq;
  // read the Beampos object
  const AthenaAttributeList* atrlist=0;
  if (StatusCode::SUCCESS==p_detstore->retrieve(atrlist, INDET_BEAMPOS ) &&
    atrlist!=0) {
    int status;
    float posx,posy,posz,sigx,sigy,sigz,sigxy,tiltx,tilty;
    status=(*atrlist)["status"].data<int>();
    posx=(*atrlist)["posX"].data<float>();
    posy=(*atrlist)["posY"].data<float>();
    posz=(*atrlist)["posZ"].data<float>();
    sigx=(*atrlist)["sigmaX"].data<float>();
    sigy=(*atrlist)["sigmaY"].data<float>();
    sigz=(*atrlist)["sigmaZ"].data<float>();
    tiltx=(*atrlist)["tiltX"].data<float>();
    tilty=(*atrlist)["tiltY"].data<float>();
    // sigmaXY was added later - use zero if not present in DB
    try {
      sigxy=(*atrlist)["sigmaXY"].data<float>();
    }
    catch (coral::AttributeListException& e) {
      sigxy=0.;
      if (msgLvl(MSG::DEBUG))
	msg() << "No sigmaXY retrieved from conditions DB, assume 0" << endreq;
    }
    if (msgLvl(MSG::INFO))
      msg() << "Read from condDB"
            << " status " << status
            << " pos (" << posx << "," << posy << "," << posz << ")"
            << " sigma (" << sigx << "," << sigy << "," << sigz << ")"
            << " tilt (" << tiltx << "," << tilty << ")"
            << " sigmaXY " << sigxy
            << endreq;
    initCache(status,posx,posy,posz,sigx,sigy,sigz,sigxy,tiltx,tilty);
  } else {
    msg(MSG::ERROR) << "Problem reading condDB object" << endreq;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}
