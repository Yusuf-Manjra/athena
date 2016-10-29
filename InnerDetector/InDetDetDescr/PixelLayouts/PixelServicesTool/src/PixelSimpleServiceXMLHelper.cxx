/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

 
#include "PixelServicesTool/PixelSimpleServiceXMLHelper.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "PathResolver/PathResolver.h"
#include "PixelLayoutUtils/DBXMLUtils.h"

PixelSimpleServiceXMLHelper::PixelSimpleServiceXMLHelper(IRDBRecordset_ptr table, const InDetDD::ServiceVolumeSchema & schema, const PixelGeoBuilderBasics* basics):
  GeoXMLUtils(),
  PixelGeoBuilder(basics),
  m_schema(schema),
  m_bXMLdefined(true)
{
  std::string nodeName = table->nodeName();
  for_each(nodeName.begin(), nodeName.end(), [](char& in){ in = ::toupper(in); });
  std::string envName = "PIXEL_"+nodeName+"_GEO_XML";

  std::string fileName;
  if(const char* env_p = std::getenv(envName.c_str())) fileName = std::string(env_p);
  if(fileName.size()==0){
    m_bXMLdefined = false;
    return;
  }


  bool readXMLfromDB = getBasics()->ReadInputDataFromDB();
  bool bParsed=false;
  if(readXMLfromDB)
    {
      msg(MSG::INFO)<<"XML input : DB CLOB "<<fileName<<"  (DB flag : "<<readXMLfromDB<<")"<<endmsg;
      DBXMLUtils dbUtils(getBasics());
      std::string XMLtext = dbUtils.readXMLFromDB(fileName);
      InitializeXML();
      bParsed = ParseBuffer(XMLtext,std::string(""));
    }
  else
    {
      msg(MSG::DEBUG)<<"XML input : from file "<<fileName<<"  (DB flag : "<<readXMLfromDB<<")"<<endmsg;
      std::string file = PathResolver::find_file (fileName, "DATAPATH");
      InitializeXML();
      bParsed = ParseFile(file);
    }
  
  if(!bParsed){
    m_bXMLdefined = false;
    msg(MSG::WARNING)<<"XML file "<<fileName<<" not found"<<endmsg;
    return;
    }
}

PixelSimpleServiceXMLHelper::PixelSimpleServiceXMLHelper(std::string envFileName, const PixelGeoBuilderBasics* basics):
  GeoXMLUtils(),
  PixelGeoBuilder(basics),
  m_bXMLdefined(true)
{

  msg(MSG::DEBUG)<<"XML helper - PixelSimpleServiceXMLHelper"<<endmsg;
    
  std::string envName = envFileName;
  msg(MSG::DEBUG)<<"SimpleServiceVolumeMakerMgr : env name "<<envName<<endmsg;
  
  std::string fileName;
  if(const char* env_p = std::getenv(envName.c_str())) fileName = std::string(env_p);
 
  bool readXMLfromDB = getBasics()->ReadInputDataFromDB();
  msg(MSG::DEBUG)<<"Build material table from XML  (DB XML file : "<<readXMLfromDB<<" )"<<endmsg;

  bool bParsed=false;
  if(readXMLfromDB)
    {
      DBXMLUtils dbUtils(getBasics());
      std::string XMLtext = dbUtils.readXMLFromDB(fileName);
      InitializeXML();
      bParsed = ParseBuffer(XMLtext,std::string(""));
    }
  else
    {
      std::string file = PathResolver::find_file (fileName, "DATAPATH");
      msg(MSG::DEBUG)<< " PixelServices : "<<file<<endmsg;
      InitializeXML();
      bParsed = ParseFile(file);
    }
  
  if(!bParsed){
    m_bXMLdefined = false;
    msg(MSG::WARNING)<<"XML file "<<fileName<<" not found"<<endmsg;
    return;
    }
}

PixelSimpleServiceXMLHelper::~PixelSimpleServiceXMLHelper()
{
  TerminateXML();
}

double PixelSimpleServiceXMLHelper::rmin(int index) const
{
  return getDouble("SimpleService", index, m_schema.rmin().c_str());
}


double PixelSimpleServiceXMLHelper::rmax(int index) const
{
  return getDouble("SimpleService", index, m_schema.rmax().c_str());
}


double PixelSimpleServiceXMLHelper::rmin2(int index) const
{
  return getDouble("SimpleService", index, m_schema.rmin2().c_str());
}

double PixelSimpleServiceXMLHelper::rmax2(int index) const
{
  return getDouble("SimpleService", index, m_schema.rmax2().c_str());
}

double PixelSimpleServiceXMLHelper::zmin(int index) const
{
  return getDouble("SimpleService", index, m_schema.zmin().c_str());
}

double PixelSimpleServiceXMLHelper::zmax(int index) const
{
  return getDouble("SimpleService", index, m_schema.zmax().c_str());
}

double PixelSimpleServiceXMLHelper::phiDelta(int index) const
{
  return getDouble("SimpleService", index, m_schema.phiDelta().c_str());
}

double PixelSimpleServiceXMLHelper::width(int index) const
{
  if (m_schema.has_width()) {
    return getDouble("SimpleService", index, m_schema.width().c_str());
  }
  return 0;
}

double PixelSimpleServiceXMLHelper::phiStart(int index) const
{
  return getDouble("SimpleService", index, m_schema.phiStart().c_str());
}

double PixelSimpleServiceXMLHelper::phiStep(int index) const
{
  if (m_schema.has_phiStep()) {
    return getDouble("SimpleService", index, m_schema.phiStep().c_str());
  } 
  return 0;
}

bool PixelSimpleServiceXMLHelper::zsymm(int index) const
{
  return getBoolean("SimpleService", index, m_schema.zsymm().c_str());
}


int PixelSimpleServiceXMLHelper::repeat(int index) const
{
  return getInt("SimpleService", index, m_schema.repeat().c_str());
}

int PixelSimpleServiceXMLHelper::radialDiv(int index) const
{
  if (m_schema.has_radial()) {  
    return getInt("SimpleService", index, m_schema.radialDiv().c_str());
  } else { 
    return 0;
  }
}

std::string PixelSimpleServiceXMLHelper::shapeType(int index) const
{
  if (m_schema.has_shapeType()) {  
    std::string tmp=getString("SimpleService", index, m_schema.shapeType().c_str(),0,"TUBE");
    tmp.erase(std::remove(tmp.begin(),tmp.end(),' '),tmp.end());
    return tmp;
    }
  return "UNKNOWN";
}

std::string PixelSimpleServiceXMLHelper::volName(int index) const
{
  std::string tmp=getString("SimpleService", index, m_schema.volName().c_str(),0,"");
  tmp.erase(std::remove(tmp.begin(),tmp.end(),' '),tmp.end());
  return tmp;
}

std::string PixelSimpleServiceXMLHelper::materialName(int index) const
{
  std::string tmp=getString("SimpleService", index, m_schema.materialName().c_str());
  tmp.erase(std::remove(tmp.begin(),tmp.end(),' '),tmp.end());
  return tmp;

}

int PixelSimpleServiceXMLHelper::volId(int index) const
{
  int volNumber= getInt("SimpleService", index, m_schema.volId().c_str(),0,-1);

  if(volNumber<0)return index;
  return volNumber;
}

unsigned int PixelSimpleServiceXMLHelper::numElements() const
{
  if (!m_bXMLdefined) return 0;
  return getChildCount("PixelServices", 0, "SimpleService");
}


int PixelSimpleServiceXMLHelper::getServiceIndex( std::string srvName) const
{
  int srvIndex = getChildValue_Index("SimpleService",
				     m_schema.volName().c_str(),
				     -1,
				     srvName);
  return srvIndex;
}



bool PixelSimpleServiceXMLHelper::SupportTubeExists(std::string srvName) const
{
  int index=getServiceIndex(srvName);
  if(index<0) return false;

  return true;
}

double PixelSimpleServiceXMLHelper::SupportTubeRMin(std::string srvName) const
{

  int index=getServiceIndex(srvName);
  if(index<0) return -1;

  return rmin(index);
}

double PixelSimpleServiceXMLHelper::SupportTubeRMax(std::string srvName) const
{

  int index=getServiceIndex(srvName);
  if(index<0) return -1;

  return rmax(index);
}

double PixelSimpleServiceXMLHelper::SupportTubeZMin(std::string srvName) const
{

  int index=getServiceIndex(srvName);
  if(index<0) return -1;

  return zmin(index);
}

double PixelSimpleServiceXMLHelper::SupportTubeZMax(std::string srvName) const
{

  int index=getServiceIndex(srvName);
  if(index<0) return -1;

  return zmax(index);
}


