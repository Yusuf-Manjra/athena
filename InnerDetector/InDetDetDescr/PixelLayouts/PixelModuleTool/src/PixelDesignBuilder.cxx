/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelModuleTool/PixelDesignBuilder.h"

#include "PixelGeoModel/PixelGeoBuilder.h"

#include "InDetReadoutGeometry/PixelModuleDesign.h"
#include "InDetReadoutGeometry/PixelDiodeMatrix.h"
#include "InDetReadoutGeometry/SiCommonItems.h"
#include "InDetReadoutGeometry/PixelDetectorManager.h"

#include "StoreGate/StoreGate.h"
#include "GeoModelInterfaces/IGeoModelSvc.h"

#include "PathResolver/PathResolver.h"
#include "PixelLayoutUtils/DBXMLUtils.h"

#include <algorithm>

using namespace InDetDD;

std::vector<PixelModuleDesign*> PixelDesignBuilder::s_designs;

PixelDesignBuilder::PixelDesignBuilder( const std::string& name, ISvcLocator* pSvcLocator ) : 
  AthService(name, pSvcLocator),
  GeoXMLUtils()
{
  s_designs.clear();
  m_moduleMap = GeoDetModulePixelMap();
}

PixelDesignBuilder::~PixelDesignBuilder()
{
  for(int i=0; i<(int)s_designs.size(); i++) delete s_designs[i];
}


StatusCode PixelDesignBuilder::initialize()
{ 
  return StatusCode::SUCCESS;
}


StatusCode PixelDesignBuilder::finalize()
{
  return StatusCode::SUCCESS;
}

StatusCode PixelDesignBuilder::queryInterface(const InterfaceID& riid, void** ppvInterface)
{
  if ( IPixelDesignSvc::interfaceID().versionMatch(riid) ) {
    *ppvInterface = dynamic_cast<IPixelDesignSvc *>(this);
  }  else  {
    // Interface is not directly available: try out a base class
    return Service::queryInterface(riid, ppvInterface);
  }
  addRef();

  return StatusCode::SUCCESS;
}

StatusCode PixelDesignBuilder::callBack(IOVSVC_CALLBACK_ARGS_P(/*I*/,/*keys*/))
{  
  return StatusCode::SUCCESS;
}




PixelModuleDesign* PixelDesignBuilder::getDesign( const PixelGeoBuilderBasics* basics, int moduleIndex)
{

  //  std::cout<<"PIXELDESIGNBUILDER per index : "<<moduleIndex<<std::endl;

  //  std::cout<<"PixelModuleDesign* PixelDesignBuilder::getDesign( const OraclePixGeoAccessor& geoAccessor, int moduleIndex)"<<endmsg;
  if (moduleIndex > (int) s_designs.size()-1) {
    s_designs.resize( moduleIndex+1, 0); // prefill with zeros
  }

  if (s_designs[ moduleIndex] == 0) {
    PixelModuleDesign* newDesign = build( basics, moduleIndex);
    s_designs[ moduleIndex] = newDesign;
    //? newDesign->ref();
  }
  return s_designs[ moduleIndex];
}

PixelModuleDesign* PixelDesignBuilder::getDesign(const PixelGeoBuilderBasics* basics, std::string moduleType)
{
  int moduleIndex = m_moduleMap.getModuleIndex(moduleType);
  
  //  std::cout<<"PIXELDESIGNBUILDER per type : "<<moduleType<<" "<<moduleIndex<<std::endl;

  return getDesign(basics, moduleIndex);
}



PixelModuleDesign* PixelDesignBuilder::build( const PixelGeoBuilderBasics* basics, int moduleIndex)
{

  std::string fileName="PixelModules.xml";
  if(const char* env_p = std::getenv("PIXEL_SILICONMODULES_GEO_XML")) fileName = std::string(env_p);

  bool readXMLfromDB = basics->ReadInputDataFromDB();
  bool bParsed=false;
  if(readXMLfromDB)
    {
      msg(MSG::DEBUG)<<"XML input : DB CLOB "<<fileName<<"  (DB flag : "<<readXMLfromDB<<")"<<endmsg;
      DBXMLUtils dbUtils(basics);
      std::string XMLtext = dbUtils.readXMLFromDB(fileName);
      InitializeXML();
      bParsed = ParseBuffer(XMLtext,std::string(""));
    }
  else
    {
      msg(MSG::INFO)<<"XML input : from file "<<fileName<<"  (DB flag : "<<readXMLfromDB<<")"<<endmsg;
      std::string file = PathResolver::find_file (fileName, "DATAPATH");
      InitializeXML();
      bParsed = ParseFile(file);
    }

  if(!bParsed){
    msg(MSG::WARNING)<<"XML file "<<fileName<<" not found"<<endmsg;
    return 0;
  }

  std::string moduleName = getChildValue("Module", moduleIndex, "moduleName");
  std::string chipName = getChildValue("Module", moduleIndex, "chipName");

  int lengthChip = getInt("Module", moduleIndex, "lengthInChips");
  int circuitsEta = lengthChip;

  //  int widthChip = getInt("Module", moduleIndex, "widthInChips", 0, -1);
  int widthChipMax = getInt("Module", moduleIndex, "widthMaxInChips", 0, -1);
  //  int widthChipMin = getInt("Module", moduleIndex , "widthMinInChips", 0, -1);
  //  widthChip = std::max(widthChip, widthChipMax);
  //   widthChip = std::max(widthChip, widthChipMin);
  int widthChip=widthChipMax;
  int circuitsPhi = widthChip;

  msg(MSG::DEBUG)<<"Readout geo parameters - ChipWidth : "<<moduleName<<"  "<<widthChipMax<<endmsg;

  double thick = getDouble("Module", moduleIndex, "sensorThickness");

  //  basics->msgStream()<<MSG::INFO<<"PixelDesignBuilder sizes : "<<width<<" "<<length<<" "<<thick<<"  module/readout -> "<<moduleIndex<<" "<<readoutIndex<<endmsg;

  int readoutIndex = getChildValue_Index("FrontEndChip","name",-1,chipName);
  double phiPitch = getDouble( "FrontEndChip", readoutIndex,"pitchPhi");
  double etaPitch = getDouble( "FrontEndChip",  readoutIndex,"pitchEta");

//   double etaPitchLong = getDouble( "FrontEndChip", readoutIndex,"pitchEtaLong",0,-1.);
//   if(etaPitchLong<0) etaPitchLong = etaPitch;
//   double etaPitchLongEnd = getDouble( "FrontEndChip", readoutIndex,"pitchEtaEnd",0,0.);

  double etaPitchLong = etaPitch;
  double etaPitchLongEnd = etaPitch;

  // rowsPerSensor = DesignNumChipsPhi() * (DesignNumRowsPerChip()+DesignNumEmptyRowsInGap()) - DesignNumEmptyRowsInGap();
  int rowsPerChip = getInt( "FrontEndChip",  readoutIndex,"rows"); 
  int emptyRows = 0;
  int rowsPerSensor = circuitsPhi*rowsPerChip + (circuitsPhi-1)*emptyRows; // FIXME check that the matrix does the right thing

  int colsPerChip = getInt( "FrontEndChip",  readoutIndex,"columns");
  int readoutSide = 1;
  
  // Add the gap between adjacent chips for RD53 chips
  chipName.erase(std::remove(chipName.begin(),chipName.end(),' '),chipName.end());
  if(chipName=="RD53"){
    if(circuitsPhi>0){
      rowsPerChip+=2*(circuitsPhi-1);
      rowsPerSensor = circuitsPhi*rowsPerChip + (circuitsPhi-1)*emptyRows;
    }
    if(circuitsEta>0){
      colsPerChip+=2*(circuitsEta-1);
    }
  }

  msg(MSG::DEBUG)<<"readout geo : ------------------------------------------------------------------------"<<endmsg;
  msg(MSG::DEBUG)<<"readout geo : "<<chipName<<endmsg;
  msg(MSG::DEBUG)<<"readout geo : "<<moduleName<<" phi : "<<circuitsPhi<<" "<<rowsPerChip<<" empty "<<emptyRows<<endmsg;

  msg(MSG::DEBUG)<<"readout geo : "<<moduleName<<" eta : "<<circuitsEta<<" "<<colsPerChip<<endmsg;
  
  msg(MSG::DEBUG)<<"readout geo : "<< moduleName<<" "<< phiPitch<<" "<< etaPitch<<" "<< etaPitchLong<<" "<< etaPitchLongEnd<<" "<<
    circuitsPhi<<" "<< circuitsEta<<" "<< rowsPerSensor<<" "<< colsPerChip<<" *"<<circuitsEta<<endmsg;
  
  msg(MSG::DEBUG)<<"readout geo : ------------------------------------------------------------------------"<<endmsg;

  double cellRowPerCirc = circuitsPhi*rowsPerChip;


  // -----------------------------------------------------------------------------------------------------------------
  // For compatibility with LoI/ATLAS geometries ( etaPithLong, ganged pixels, ...)

  TerminateXML();

  fileName="PixelModules.xml";
  if(const char* env_p = std::getenv("PIXEL_SILICONREADOUT_GEO_XML")) fileName = std::string(env_p);
  msg(MSG::DEBUG)<<"Readout geometry file name : "<<fileName<<endmsg;

  bParsed=false;
  if(readXMLfromDB)
    {
      msg(MSG::INFO)<<"XML input : DB CLOB "<<fileName<<"  (DB flag : "<<readXMLfromDB<<")"<<endmsg;
      DBXMLUtils dbUtils(basics);
      std::string XMLtext = dbUtils.readXMLFromDB(fileName);
      InitializeXML();
      bParsed = ParseBuffer(XMLtext,std::string(""));
    }
  else
    {
      msg(MSG::INFO)<<"XML input : from file "<<fileName<<"  (DB flag : "<<readXMLfromDB<<")"<<endmsg;
      InitializeXML();
      std::string file = PathResolver::find_file (fileName, "DATAPATH");
      bParsed = ParseFile(file);
    }

  if(!bParsed){
    msg(MSG::WARNING)<<"XML file "<<fileName<<" not found"<<endmsg;
    return 0;
  }

  int readoutIndexGeo = getChildValue_Index("FrontEndChipGeo","moduleName",-1,moduleName);
  if(readoutIndexGeo>-1) {

    msg(MSG::DEBUG)<<"Readout module : "<<moduleName<<"  "<<chipName<<"  "<<readoutIndexGeo<<endmsg;

    etaPitchLong = getDouble( "FrontEndChipGeo", readoutIndexGeo,"pitchEtaLong");
    etaPitchLongEnd = getDouble( "FrontEndChipGeo",  readoutIndexGeo,"pitchEtaEnd");
    emptyRows = getInt( "FrontEndChipGeo",readoutIndexGeo,"emptyRows");
    rowsPerSensor = circuitsPhi*rowsPerChip + (circuitsPhi-1)*emptyRows; // FIXME check that the matrix does the right thing
    
    readoutSide = getInt( "FrontEndChipGeo", readoutIndexGeo,"readoutSide");

    msg(MSG::DEBUG)<<"readout geo - old geo : "<< moduleName<<" "<< phiPitch<<" "<< etaPitch<<" "<< etaPitchLong<<" "<< etaPitchLongEnd<<" "<<
      circuitsPhi<<" "<< circuitsEta<<" "<< rowsPerSensor<<" "<< colsPerChip<<" *"<<circuitsEta<<"   empty "<<emptyRows<<endmsg;

  }
  
  msg(MSG::DEBUG)<<"readout geo : ------------------------------------------------------------------------"<<endmsg;

  // see PixelGeoModel/OraclePixGeoManager 
  // This should be (*pdch)[0]->getDouble("NRPCHIP"), but in the current
  // design we prefer to have one chip in the rphi direction
  // and define the connections for the pixels in the gap
  circuitsPhi =1;


  msg(MSG::DEBUG)<<"readout geo - matrix : "<< phiPitch<<" "<< etaPitch<<" "<< etaPitchLong<<" "<< etaPitchLongEnd<<" "<<
    circuitsPhi<<" "<< circuitsEta<<" "<< rowsPerSensor<<" "<< colsPerChip<<endmsg;

  PixelDiodeMatrix * fullMatrix = buildMatrix( phiPitch, etaPitch, etaPitchLong, etaPitchLongEnd,
					       circuitsPhi, circuitsEta, rowsPerSensor, colsPerChip);


  msg(MSG::DEBUG)<<"readout geo - design " << thick<<" "<<
    circuitsPhi<<" "<<
    circuitsEta<<" "<<
    colsPerChip<<" "<<
    cellRowPerCirc<<" "<<
    colsPerChip<<" "<<
    rowsPerSensor<<" "<<
    electrons<<" "<<
    readoutSide<<endmsg;

  PixelModuleDesign* design = new PixelModuleDesign( thick,
						     circuitsPhi,
						     circuitsEta,
						     colsPerChip,
						     cellRowPerCirc,
						     colsPerChip,
						     rowsPerSensor,
						     fullMatrix,
						     electrons,
						     readoutSide);

  msg(MSG::DEBUG)<<"readout geo - design : "<<design->width()<<" "<<design->length()<<" "<<design->thickness()<<"    "<<design->rows()<<" "<<design->columns()<<endmsg;

  // Multiple connections (ganged pixels)
  if (emptyRows>0){
    
    int gangedType = getInt( "FrontEndChipGeo",  readoutIndexGeo,"gangedType");
    int gangedIndex = getChildValue_Index("GangedType", "type", gangedType);
    std::vector<int> v_emptyrows = getVectorInt("GangedType",gangedIndex,"emptyrow");
    std::vector<int> v_connectrows = getVectorInt("GangedType",gangedIndex,"connectrow");

    msg(MSG::DEBUG)<<"readout geo - emptyrows>0 : "<< gangedType<<" "<< gangedIndex<<endmsg;
    
    int minRow = v_emptyrows[0];
    int maxRow = minRow;
    
    for (int iConnect = 0; iConnect < emptyRows; iConnect++){
      minRow = std::min(minRow, v_emptyrows[iConnect]);
      minRow = std::min(minRow, v_connectrows[iConnect]);
      maxRow = std::max(maxRow, v_emptyrows[iConnect]);
      maxRow = std::max(maxRow, v_connectrows[iConnect]);
    }
    
    std::vector <int> connections(maxRow-minRow+1);
    
    // We fill them all with a one to one correspondence first.
    for (unsigned int iRow = 0; iRow < connections.size(); iRow++){
      connections[iRow] = iRow +  minRow;
    }
    
    // Now make the connections.
    for (int iConnect = 0; iConnect < emptyRows; iConnect++){
      connections[v_emptyrows[iConnect]-minRow] = v_connectrows[iConnect];
    }
    
//     basics->msgStream() << "empty rows : MinRow = " << minRow << endmsg;
//     basics->msgStream() << "empty rows : MaxRow = " << maxRow << endmsg;
//     basics->msgStream() << "empty rows : ";
//     for (unsigned int iRow = 0; iRow < connections.size(); iRow++){
//       basics->msgStream() << iRow << " " << connections[iRow] << " / ";
//     }
//     msg(MSG::DEBUG)<<endmsg;
    
    design->addMultipleRowConnection(minRow, connections);
  }
  
  basics->getDetectorManager()->addDesign(design);

  return design;
}


// Copied  from GeoPixelSiCrystal::makeMatrix release 17.3.1 
// FIXME avoid duplication!
PixelDiodeMatrix *  PixelDesignBuilder::buildMatrix( double phiPitch, double etaPitch, 
						     double etaPitchLong, double etaPitchLongEnd,
						     int circuitsPhi, int circuitsEta,
						     int diodeRowPerCirc, int diodeColPerCirc)
{
  // There are several different cases. Not all are used at the time of wrtiting the code but I
  // have tried to consider all possible cases for completeness. 
  //
  // end cell : middle cells : between chip 
  // --------------------------------------
  // long:normal:long (standard ATLAS case)
  // normal:normal:normal
  // normal:normal:long (> 2 chips)
  // normal:normal:long (2 chips)
  // end:normal:long    (not likely)
  // end:normal:normal  (not likely)
  // end:normal:end  (if single chip)

  PixelDiodeMatrix * fullMatrix = 0;
  
  if (etaPitchLongEnd == etaPitchLong && etaPitchLong != etaPitch) {
    // long:normal:long (standard ATLAS case)
    //if (gmt_mgr->msgLvl(MSG::DEBUG)) gmt_mgr->msg(MSG::DEBUG) <<  "GeoPixelSiCrystal: Making matrix (long:normal:long, Standard ATLAS case)" << endmsg;

    PixelDiodeMatrix * normalCell = new PixelDiodeMatrix(phiPitch, etaPitch); 
    PixelDiodeMatrix * bigCell = new PixelDiodeMatrix(phiPitch, etaPitchLong); 
    
    PixelDiodeMatrix * singleChipRow = new PixelDiodeMatrix(PixelDiodeMatrix::etaDir,
							    bigCell, 
							    normalCell, 
							    diodeColPerCirc-2,
							    bigCell);

    PixelDiodeMatrix * singleRow = new PixelDiodeMatrix(PixelDiodeMatrix::etaDir,
							0, singleChipRow, circuitsEta, 0);

    fullMatrix = new PixelDiodeMatrix(PixelDiodeMatrix::phiDir,
				      0, singleRow, circuitsPhi*diodeRowPerCirc, 0);
  } else if (etaPitchLongEnd == etaPitchLong && (etaPitchLong == etaPitch || circuitsEta == 1)) {
    // normal:normal:normal
    //if (gmt_mgr->msgLvl(MSG::DEBUG)) gmt_mgr->msg(MSG::DEBUG) <<  "GeoPixelSiCrystal: Making matrix (normal:normal:normal)" << endmsg;
    PixelDiodeMatrix * normalCell = new PixelDiodeMatrix(phiPitch, etaPitch); 
    PixelDiodeMatrix * singleRow = new PixelDiodeMatrix(PixelDiodeMatrix::etaDir,
							0, normalCell, circuitsEta*diodeColPerCirc, 0);
    fullMatrix = new PixelDiodeMatrix(PixelDiodeMatrix::phiDir,
				      0, singleRow, circuitsPhi*diodeRowPerCirc, 0);
  } else if (etaPitchLongEnd == etaPitch &&  etaPitchLong != etaPitch && circuitsEta > 2) {
    //if (gmt_mgr->msgLvl(MSG::DEBUG)) gmt_mgr->msg(MSG::DEBUG) <<  "GeoPixelSiCrystal: Making matrix (normal:normal:long, > 2 chips)" << endmsg;
    // normal:normal:long: > 2 chips
    PixelDiodeMatrix * normalCell = new PixelDiodeMatrix(phiPitch, etaPitch); 
    PixelDiodeMatrix * bigCell = new PixelDiodeMatrix(phiPitch, etaPitchLong); 
    
    PixelDiodeMatrix * lowerSingleChipRow = new PixelDiodeMatrix(PixelDiodeMatrix::etaDir,
								 0, 
								 normalCell, 
								 diodeColPerCirc-1,
								 bigCell);
    PixelDiodeMatrix * middleSingleChipRow = new PixelDiodeMatrix(PixelDiodeMatrix::etaDir,
								  bigCell, 
								  normalCell, 
								  diodeColPerCirc-2,
								  bigCell);
    PixelDiodeMatrix * upperSingleChipRow = new PixelDiodeMatrix(PixelDiodeMatrix::etaDir,
								 bigCell, 
								 normalCell, 
								 diodeColPerCirc-1,
								 0);
    PixelDiodeMatrix * singleRow = new PixelDiodeMatrix(PixelDiodeMatrix::etaDir,
							lowerSingleChipRow, middleSingleChipRow, circuitsEta-2, upperSingleChipRow);
    fullMatrix = new PixelDiodeMatrix(PixelDiodeMatrix::phiDir,
				      0, singleRow, circuitsPhi*diodeRowPerCirc, 0);
  } else if (etaPitchLongEnd == etaPitch &&  etaPitchLong != etaPitch && circuitsEta == 2) {
    // normal:normal:long: 2 chips (current SLHC case)
    //if (gmt_mgr->msgLvl(MSG::DEBUG)) gmt_mgr->msg(MSG::DEBUG) <<  "GeoPixelSiCrystal: Making matrix (normal:normal:long, 2 chips)" << endmsg;
    PixelDiodeMatrix * normalCell = new PixelDiodeMatrix(phiPitch, etaPitch); 
    PixelDiodeMatrix * bigCell = new PixelDiodeMatrix(phiPitch, etaPitchLong); 
    
    PixelDiodeMatrix * lowerSingleChipRow = new PixelDiodeMatrix(PixelDiodeMatrix::etaDir,
								 0, 
								 normalCell, 
								 diodeColPerCirc-1,
								 bigCell);
    PixelDiodeMatrix * upperSingleChipRow = new PixelDiodeMatrix(PixelDiodeMatrix::etaDir,
								 bigCell, 
								 normalCell, 
								 diodeColPerCirc-1,
								 0);
    PixelDiodeMatrix * singleRow = new PixelDiodeMatrix(PixelDiodeMatrix::etaDir,
							lowerSingleChipRow, upperSingleChipRow, 1, 0);
    fullMatrix = new PixelDiodeMatrix(PixelDiodeMatrix::phiDir,
				      0, singleRow, circuitsPhi*diodeRowPerCirc, 0);
  } else if (circuitsEta == 1 ||  (etaPitchLongEnd != etaPitch &&  etaPitchLong == etaPitch )){ // etaPitchLongEnd != etaPitch at this stage
    // end:normal:end  (for single chip)
    // end:normal:normal  (not likely)
    //if (gmt_mgr->msgLvl(MSG::DEBUG)) gmt_mgr->msg(MSG::DEBUG) <<  "GeoPixelSiCrystal: Making matrix (end:normal:end, single chips or end:normal:normal)" << endmsg;
    PixelDiodeMatrix * normalCell = new PixelDiodeMatrix(phiPitch, etaPitch); 
    PixelDiodeMatrix * bigCell = new PixelDiodeMatrix(phiPitch, etaPitchLongEnd); 
    
    PixelDiodeMatrix * singleRow = new PixelDiodeMatrix(PixelDiodeMatrix::etaDir,
							    bigCell, 
							    normalCell, 
							    circuitsEta*diodeColPerCirc-2,
							    bigCell);
    fullMatrix = new PixelDiodeMatrix(PixelDiodeMatrix::phiDir,
				      0, singleRow, circuitsPhi*diodeRowPerCirc, 0);
  } else {
    // end:normal:long    (not likely)
    //if (gmt_mgr->msgLvl(MSG::DEBUG)) gmt_mgr->msg(MSG::DEBUG) <<  "GeoPixelSiCrystal: Making matrix (end:normal:long)" << endmsg;
    PixelDiodeMatrix * normalCell = new PixelDiodeMatrix(phiPitch, etaPitch); 
    PixelDiodeMatrix * bigCell = new PixelDiodeMatrix(phiPitch, etaPitchLong); 
    PixelDiodeMatrix * endCell = new PixelDiodeMatrix(phiPitch, etaPitchLongEnd); 
    
    PixelDiodeMatrix * lowerSingleChipRow = new PixelDiodeMatrix(PixelDiodeMatrix::etaDir,
								 endCell, 
								 normalCell, 
								 diodeColPerCirc-2,
								 bigCell);
    PixelDiodeMatrix * middleSingleChipRow = new PixelDiodeMatrix(PixelDiodeMatrix::etaDir,
								  bigCell, 
								  normalCell, 
								  diodeColPerCirc-2,
								  bigCell);
    PixelDiodeMatrix * upperSingleChipRow = new PixelDiodeMatrix(PixelDiodeMatrix::etaDir,
								 bigCell, 
								 normalCell, 
								 diodeColPerCirc-2,
								 endCell);
    PixelDiodeMatrix * singleRow = new PixelDiodeMatrix(PixelDiodeMatrix::etaDir,
							lowerSingleChipRow, middleSingleChipRow, circuitsEta-2, upperSingleChipRow);
    fullMatrix = new PixelDiodeMatrix(PixelDiodeMatrix::phiDir,
				      0, singleRow, circuitsPhi*diodeRowPerCirc, 0);
    
  }

  return fullMatrix;
}
