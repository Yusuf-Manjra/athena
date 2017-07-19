/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "EndcapRingRef/PixelRingSupportXMLHelper.h"
#include "PathResolver/PathResolver.h"
#include "PixelLayoutUtils/DBXMLUtils.h"

PixelRingSupportXMLHelper::PixelRingSupportXMLHelper(const PixelGeoBuilderBasics* basics):
  GeoXMLUtils(),
  PixelGeoBuilder(basics),
  m_bXMLfileExist(false)
{

  std::string fileName="GenericRingSupport.xml";
  if(const char* env_p = std::getenv("PIXEL_PIXELDISCSUPPORT_GEO_XML")) fileName = std::string(env_p);
  //  std::cout<<"XML disc support : "<<fileName<<std::endl;

  bool readXMLfromDB = getBasics()->ReadInputDataFromDB();
  bool bParsed=false;
  if(readXMLfromDB)
    {
      basics->msgStream()<<"XML input : DB CLOB "<<fileName<<"  (DB flag : "<<readXMLfromDB<<")"<<endreq;
      DBXMLUtils dbUtils(getBasics());
      std::string XMLtext = dbUtils.readXMLFromDB(fileName);
      InitializeXML();
      bParsed = ParseBuffer(XMLtext,std::string(""));
    }
  else
    {
      basics->msgStream()<<"XML input : from file "<<fileName<<"  (DB flag : "<<readXMLfromDB<<")"<<endreq;
      std::string file = PathResolver::find_file (fileName, "DATAPATH");
      InitializeXML();
      bParsed = ParseFile(file);
    } 

  if(!bParsed){
    //    std::cout<<"XML file "<<fileName<<" not found"<<std::endl;
    return;
  }

  m_bXMLfileExist = true;

}

PixelRingSupportXMLHelper::~PixelRingSupportXMLHelper()
{
  TerminateXML();
}

int PixelRingSupportXMLHelper::getNbSupport(int layer) 
{
  if(!m_bXMLfileExist) return 0;

  int layerIndex = getChildValue_Index("PixelRingSupport", "Layer", layer);
  std::string ringGeoName = getString("PixelRingSupport", layerIndex, "RingSupportGeo");
  m_ringGeoIndex = (ringGeoName!="None")? getChildValue_Index("PixelRingSupportGeo", "name", -1, ringGeoName) : -1;

  if(m_ringGeoIndex<0) return 0;
  std::vector<double> v = getVectorDouble("PixelRingSupportGeo",m_ringGeoIndex,"rmin");
  return (int)v.size();
}

double PixelRingSupportXMLHelper::getRingSupportRMin(int iSupport) const
{
  std::vector<double> v = getVectorDouble("PixelRingSupportGeo",m_ringGeoIndex,"rmin");
  return v[iSupport];
}

double PixelRingSupportXMLHelper::getRingSupportRMax(int iSupport) const
{
  std::vector<double> v = getVectorDouble("PixelRingSupportGeo",m_ringGeoIndex,"rmax");
  return v[iSupport];
}

double PixelRingSupportXMLHelper::getRingSupportThickness(int iSupport) const
{
  std::vector<double> v = getVectorDouble("PixelRingSupportGeo",m_ringGeoIndex,"thickness");
  int index = (v.size()==1)? 0 : iSupport;
  return v[index];
}


std::string PixelRingSupportXMLHelper::getRingSupportMaterial(int iSupport) const
{
  std::vector<std::string> v = getVectorString("PixelRingSupportGeo",m_ringGeoIndex,"material");
  int index = (v.size()==1)? 0 : iSupport;
  return v[index];
}

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

int PixelRingSupportXMLHelper::getNbLayerSupport(int layer)
{
  if(!m_bXMLfileExist) return 0;

  int layerIndex = getChildValue_Index("PixelLayerSupport", "Layer", layer);
  std::string ringGeoName = getString("PixelLayerSupport", layerIndex, "LayerSupportGeo");
  m_ringGeoIndex = (ringGeoName!="None")? getChildValue_Index("PixelLayerSupportGeo", "name", -1, ringGeoName) : -1;

  if(m_ringGeoIndex<0) return 0;
  return 1;
}

std::vector<int> PixelRingSupportXMLHelper::getNbLayerSupportIndex(int layer)
{
  std::vector<int> layers;
  
  if(!m_bXMLfileExist) return layers;

  int layerIndex = getChildValue_Index("PixelLayerSupport", "Layer", layer);
  std::string ringGeoName = getString("PixelLayerSupport", layerIndex, "LayerSupportGeo");
   
  // using the first name support to get the layer index
  std::stringstream ss(ringGeoName);
  std::string item;
  
  while (ss>>item) {
    if(item.size()==0) continue;
       
    m_ringGeoIndex = (ringGeoName!="None")? getChildValue_Index("PixelLayerSupportGeo", "name", -1, item) : -1;
    layers.push_back(m_ringGeoIndex);
  }
  return layers;
}

std::vector<double> PixelRingSupportXMLHelper::getLayerSupportRadius(int /*iSupport*/) const
{
  std::vector<double> v = getVectorDouble("PixelLayerSupportGeo",m_ringGeoIndex,"r");
  return v;
}

std::vector<double> PixelRingSupportXMLHelper::getLayerSupportZ(int /*iSupport*/) const
{
  std::vector<double> v = getVectorDouble("PixelLayerSupportGeo",m_ringGeoIndex,"z");
  return v;
}

std::string PixelRingSupportXMLHelper::getLayerSupportMaterial(int iSupport) const
{
  std::vector<std::string> v = getVectorString("PixelLayerSupportGeo",m_ringGeoIndex,"material");
  int index = (v.size()==1)? 0 : iSupport;
  return v[index];
}

std::vector<double> PixelRingSupportXMLHelper::getLayerSupportRadiusAtIndex(int index) const
{
  std::vector<double> v = getVectorDouble("PixelLayerSupportGeo",index,"r");
  return v;
}

std::vector<double> PixelRingSupportXMLHelper::getLayerSupportZAtIndex(int index) const
{
  std::vector<double> v = getVectorDouble("PixelLayerSupportGeo",index,"z");
  return v;
}

std::string PixelRingSupportXMLHelper::getLayerSupportMaterialAtIndex(int index) const
{
  std::vector<std::string> v = getVectorString("PixelLayerSupportGeo",index,"material");
  int myIndex = (v.size()==1)? 0 : index;
  return v[myIndex];
}

