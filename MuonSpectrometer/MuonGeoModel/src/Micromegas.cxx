/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


#include "MuonGeoModel/Micromegas.h"
#include "MuonAGDDDescription/MM_Technology.h"
#include "AGDDKernel/AGDDDetectorStore.h"
#include "MuonGeoModel/Station.h"
#include "MuonGeoModel/MicromegasComponent.h"
#include "MuonGeoModel/Cutout.h"
#include "GeoModelKernel/GeoTrd.h"
#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoSerialDenominator.h"
#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoSerialIdentifier.h"
#include "GeoModelKernel/GeoIdentifierTag.h"
#include "GeoModelKernel/GeoDefinitions.h"
// for cutouts:
#include "GeoModelKernel/GeoShapeSubtraction.h"
#include "GeoModelKernel/GeoShapeIntersection.h"
#include "GeoModelKernel/GeoShapeShift.h"

namespace MuonGM {

Micromegas::Micromegas(Component* ss): DetectorElement(ss->name)
{
  MicromegasComponent* s = (MicromegasComponent*)ss;
  m_component = s;
  width = s->dx1;
  longWidth = s->dx2;
  length = s->dy;
  name=s->name;
  index = s->index;
}

GeoFullPhysVol* Micromegas::build(int minimalgeo)
{
  std::vector<Cutout*> vcutdef;
  int cutoutson = 0;
  return build(minimalgeo, cutoutson, vcutdef);
}

GeoFullPhysVol* Micromegas::build(int minimalgeo, int , std::vector<Cutout*> )
{
  AGDDDetectorStore* ds = AGDDDetectorStore::GetDetectorStore();
  MM_Technology* t = (MM_Technology*) ds->GetTechnology(name);
  thickness = t->Thickness();
  double gasTck=t->gasThickness;
  double pcbTck=t->pcbThickness;
  double roTck=t->roThickness;
  double f1=t->f1Thickness;
  double f2=t->f2Thickness;
  double f3=t->f3Thickness;


  minimalgeo=t->geoLevel;

  // Build Micromegas mother volume out of G10
  const GeoShape* strd = new GeoTrd(thickness/2, thickness/2, width/2,
                                    longWidth/2, length/2);

  logVolName=name;
  if (!(m_component->subType).empty()) logVolName+=("-"+m_component->subType);
  const GeoMaterial* mtrd = getMaterialManager()->getMaterial("sct::PCB");
  GeoLogVol* ltrd = new GeoLogVol(logVolName, strd, mtrd);
  GeoFullPhysVol* ptrd = new GeoFullPhysVol(ltrd);

  if (!minimalgeo) return ptrd;

  double newpos = -thickness/2.;
  ptrd->add(new GeoSerialIdentifier(0));

  int igl = 0;
  int iSenLyr = 0;

  // Loop over Micromegas layers
  for (int i = 0; i < t->nlayers; i++) {
    // std::cout<<"loop over layers"<<std::endl;
    double widthActive;
    double longWidthActive;
    double lengthActive;


    // sensitive volume
    igl++;
    ptrd->add(new GeoIdentifierTag(igl));

    widthActive = width;
    longWidthActive = longWidth;
    lengthActive = length;

    if(i==0) newpos += gasTck/2.;
    else newpos += gasTck;
    if( (i+1) % 2 ) newpos += pcbTck;
    else newpos += roTck;
    double newXPos=newpos;

    const GeoShape* sGasVolume = new GeoTrd(gasTck/2, gasTck/2, widthActive/2,
                                            longWidthActive/2, lengthActive/2);



    GeoLogVol* ltrdtmp = new GeoLogVol("MM_Sensitive", sGasVolume,
                                       getMaterialManager()->getMaterial("muo::ArCO2"));
    GeoPhysVol* ptrdtmp = new GeoPhysVol(ltrdtmp);
    GeoNameTag* ntrdtmp = new GeoNameTag(name+"muo::ArCO2");
    GeoTransform* ttrdtmp = new GeoTransform(GeoTrf::TranslateX3D(newXPos));

    // Place gas volume inside G10 mother volume so that
    // subtractions from gas volume now become G10

    ptrd->add(ntrdtmp);
    ptrd->add(ttrdtmp);
    ptrd->add(ptrdtmp);


    double lW=longWidth/2.-((longWidth-width)/2.)*f1/length;
    double W=width/2.+((longWidth-width)/2.)*f2/length;
    const GeoShape* trd1 = new GeoTrd(gasTck/2,gasTck/2, width/2,
                                      longWidth/2, length/2);
    const GeoShape* trd2 = new GeoTrd(gasTck,gasTck, W-f3,
                                      lW-f3, length/2-(f1+f2)/2.);
    GeoTrf::Translate3D c(0,0,(f2-f1)/2.);
    trd1= &(trd1->subtract( (*trd2) << c ));
    GeoLogVol* ltrdframe = new GeoLogVol("MM_Frame", trd1,
                                         getMaterialManager()->getMaterial("std::Aluminium"));
    GeoPhysVol* ptrdframe = new GeoPhysVol(ltrdframe);

    ptrdtmp->add(ptrdframe);

    iSenLyr++;
  } // Loop over tgc layers

  return ptrd;
}

void Micromegas::print()
{
  MsgStream log(Athena::getMessageSvc(), "MuonGM::Micromegas");
  log << MSG::INFO << " Micromegas " << name << " :" << endmsg;
}

} // namespace MuonGM
