/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// Build detailed stave support : face plate + carbon foam + cable flex + cooling pipe + end blocks
// This is built one time per layer. 

#include "BarrelExtendedRef/GeoPixelStaveSupportExtRef.h"
#include "BarrelExtendedRef/PixelExtRefStaveXMLHelper.h"

#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoSimplePolygonBrep.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoNameTag.h"

#include "InDetGeoModelUtils/InDetMaterialManager.h" 
 
#include "GeoModelKernel/GeoTransform.h"
#include "PathResolver/PathResolver.h"

#include <algorithm>
using std::max;

GeoPixelStaveSupportExtRef::GeoPixelStaveSupportExtRef(const PixelGeoBuilderBasics* basics, int iLayer,  const GeoDetModulePixel& barrelModule):
  PixelGeoBuilder(basics),								     
  m_layer(iLayer), m_barrelModule(barrelModule)
{
  preBuild();
}

GeoVPhysVol* GeoPixelStaveSupportExtRef::Build() {
  return m_physVol;
}

void GeoPixelStaveSupportExtRef::preBuild() {

  // build parallelepipedic stave support (data read from stave xml file)
  
  // Access XML file
  PixelExtRefStaveXMLHelper staveDBHelper(m_layer, getBasics());

  double thickness = staveDBHelper.getStaveSupportThick();
  double width     = staveDBHelper.getStaveSupportWidth();
  double length    = staveDBHelper.getStaveSupportLength();
  std::string matName = staveDBHelper.getStaveSupportMaterial();
  double xOffset = staveDBHelper.getServiceOffsetX();
  double yOffset = staveDBHelper.getServiceOffsetY();
  if(width<0.01) width = m_barrelModule.Width();
  std::string routing = staveDBHelper.getSvcRoutingPos();

  msg(MSG::DEBUG)<<"Stave sizes LxWxT: "<<length<<"  "<<width<<"  "<<thickness<<"   "<<matName<<endmsg;

  GeoBox * shape = new GeoBox(0.5*thickness, 0.5*width, 0.5*length);
  const GeoMaterial* material = matMgr()->getMaterialForVolume(matName,shape->volume());
  GeoLogVol* logVol = new GeoLogVol("StaveSupport",shape,material);


  m_thicknessP =  xOffset + 0.5*thickness;
  m_thicknessN =  -xOffset + 0.5*thickness;
  m_length = length;
  m_width = width;
  m_routing = routing;

  if (m_routing=="inner") xOffset = -xOffset;
  m_transform = HepGeom::Translate3D(xOffset,yOffset,0);

  m_physVol = new GeoPhysVol(logVol);

}
