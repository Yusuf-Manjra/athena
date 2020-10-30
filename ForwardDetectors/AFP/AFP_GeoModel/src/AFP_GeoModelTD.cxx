/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "AFP_GeoModel/AFP_GeoModelFactory.h"
#include "GeoModelInterfaces/AbsMaterialManager.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoTubs.h"
#include "GeoModelKernel/GeoShapeSubtraction.h"
#include "GeoModelKernel/GeoShapeIntersection.h"
#include "GeoModelKernel/GeoShapeUnion.h"
#include "GeoModelKernel/GeoShapeShift.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoAlignableTransform.h"
#include "CLHEP/GenericFunctions/AbsFunction.hh"
#include "CLHEP/GenericFunctions/Variable.hh"
#include "CLHEP/GenericFunctions/Sin.hh"
#include "CLHEP/GenericFunctions/Cos.hh"
#include "StoreGate/StoreGateSvc.h"
#include "GeoModelUtilities/GeoModelTool.h"

#include "GeoModelInterfaces/StoredMaterialManager.h"
#include "GeoModelUtilities/GeoOpticalPhysVol.h"
#include "GeoModelUtilities/GeoOpticalSurface.h"
#include "GeoModelUtilities/GeoBorderSurfaceContainer.h"
#include "GeoModelUtilities/GeoMaterialPropertiesTable.h"

#include <iostream>
#include <fstream>
#include <string>

#include <math.h>
#include <algorithm>

#include <list>
#include <map>

#include <stdlib.h>

void AFP_GeoModelFactory::initializeTDParameters()
{
    // Surface definition (preliminary)
    m_pOpticalSurface = new GeoOpticalSurface("TDQuarticOpticalSurface", GeoOpticalSurface::unified, GeoOpticalSurface::polished, GeoOpticalSurface::dielectric_dielectric, GeoOpticalSurface::polished);

    m_pReflectionOptSurface=new GeoOpticalSurface("AirLightGuideSurface", GeoOpticalSurface::unified, GeoOpticalSurface::polished, GeoOpticalSurface::dielectric_metal, GeoOpticalSurface::polished);
    GeoMaterialPropertiesTable* pMPT=new GeoMaterialPropertiesTable();
    double pfEnergy1[2]={ 1.62*CLHEP::eV, 6.20*CLHEP::eV };
    double pfReflectivity1[2]={ 0.9, 0.9};
    pMPT->AddProperty("REFLECTIVITY",pfEnergy1,pfReflectivity1,2);
    m_pReflectionOptSurface->SetMaterialPropertiesTable(pMPT);
}

StatusCode AFP_GeoModelFactory::addTimingDetector(const char* pszStationName, GeoOpticalPhysVol* pPhysMotherVol, HepGeom::Transform3D& TransInMotherVolume, GeoBorderSurfaceContainer* bsContainer)
{
	int i,j,nPixelID;
	double fXShift,fYShift,fZShift;
	AFPTOF_LBARDIMENSIONS LQBarDims;
    HepGeom::Transform3D TotTransform;

	eAFPStation eStation=m_pGeometry->parseStationName(pszStationName);
	AFP_TDCONFIGURATION TofCfg=m_CfgParams.tdcfg[eStation];
	AFPTOF_LBARDIMENSIONS BarDims11=TofCfg.mapBarDims[11];
	fXShift=-73.3*CLHEP::mm;
	fYShift=(BarDims11.fRadLength+TofCfg.mapTrainInfo[BarDims11.nTrainID].fPerpShiftInPixel-0.5*(BarDims11.fLGuideWidth-TofCfg.mapTrainInfo[BarDims11.nTrainID].fTaperOffset)-0.5*BarDims11.fLBarThickness/tan(TofCfg.fAlpha))*sin(TofCfg.fAlpha);
	fZShift=fabs(fYShift)/tan(TofCfg.fAlpha)+0.5*BarDims11.fLBarThickness/sin(TofCfg.fAlpha);
	HepGeom::Transform3D TofTransform=TransInMotherVolume*HepGeom::Translate3D(fXShift,fYShift,fZShift)*HepGeom::RotateX3D(90.0*CLHEP::deg-TofCfg.fAlpha)*HepGeom::RotateZ3D(-90.0*CLHEP::deg);

	for(i=0;i<m_CfgParams.tdcfg[eStation].nX1PixCnt;i++)
	{
		for(j=0;j<m_CfgParams.tdcfg[eStation].nX2PixCnt;j++)
		{
			nPixelID=10*(i+1)+(j+1);
			addSepRadLBar(pszStationName,1,nPixelID,pPhysMotherVol,TofTransform,bsContainer);
		}
	}

	addSensor(pszStationName,1,pPhysMotherVol,TofTransform,bsContainer);

    return StatusCode::SUCCESS;
}

void AFP_GeoModelFactory::addSepRadLBar(const char* pszStationName, const int nQuarticID, const int nBarID, GeoOpticalPhysVol* pPhysMotherVolume, HepGeom::Transform3D& TransInMotherVolume, GeoBorderSurfaceContainer* bsContainer)
{
	double fX1Pos,fX2Pos,falpha,fd;
	HepGeom::Vector3D<double> vecA1, vecA2, vecX;
	CLHEP::Hep3Vector vecCutShift;
	HepGeom::Transform3D TransCut;
	GeoShapeShift* pMoveCut;
	GeoBox* pSolAux;
	char szlabel[64];

	CLHEP::HepRotation Rot1,Rot2,Rot3;

	eAFPStation eStation=m_pGeometry->parseStationName(pszStationName);
	AFP_TDCONFIGURATION TofCfg=m_CfgParams.tdcfg[eStation];
	AFPTOF_LBARDIMENSIONS BarDims=TofCfg.mapBarDims[nBarID];

	double fTaperOffset=TofCfg.mapTrainInfo[BarDims.nTrainID].fTaperOffset;
	m_pGeometry->getPixelLocalPosition(eStation,nBarID,&fX1Pos,&fX2Pos);

	fX1Pos+=TofCfg.mapTrainInfo[BarDims.nTrainID].fPerpShiftInPixel;

	//Light guide
	HepGeom::Transform3D TotTransform=TransInMotherVolume*HepGeom::Translate3D(fX1Pos,0.5*BarDims.fLGuideLength,fX2Pos);
	GeoShape* pSolVolume=new GeoBox(0.5*BarDims.fLGuideWidth,0.5*BarDims.fLGuideLength,0.5*BarDims.fLBarThickness);

	if(TofCfg.mapTrainInfo[BarDims.nTrainID].bUseTaper)
	{
		falpha=TofCfg.mapTrainInfo[BarDims.nTrainID].fTaperAngle;
		fd=fTaperOffset/sin(falpha);
		vecA1=-fd*cos(falpha)*CLHEP::Hep3Vector(0.0,1.0,0.0);
		vecA2=+0.5*fd*sqrt(2.0)*(CLHEP::HepRotationZ(+(45*CLHEP::deg-falpha))*CLHEP::Hep3Vector(0.0,1.0,0.0)).unit();
		vecX=vecA1+vecA2;
		vecCutShift=CLHEP::Hep3Vector(-0.5*BarDims.fLGuideWidth,0.5*BarDims.fLGuideLength,0.0)+CLHEP::Hep3Vector(vecX);
		Rot1.rotateZ(-falpha);

		pSolAux=new GeoBox(0.5*fd,0.5*fd,0.5*BarDims.fLBarThickness+SLIMCUT);
		TransCut=HepGeom::Transform3D(Rot1,vecCutShift);
		pMoveCut=new GeoShapeShift(pSolAux, TransCut);
		pSolVolume=new GeoShapeSubtraction(pSolVolume, pMoveCut);
	}

	sprintf(szlabel,"%s_Q%i_LGuide[%i]",pszStationName,nQuarticID,nBarID);
	GeoLogVol* pLogLGuide=new GeoLogVol(szlabel,pSolVolume,m_MapMaterials[std::string("Quartz")]);
	GeoOpticalPhysVol* pPhysLGuide=new GeoOpticalPhysVol(pLogLGuide);
	sprintf(szlabel,"%s_Q%i_LGuide[%i]",pszStationName,nQuarticID,nBarID);
	pPhysMotherVolume->add(new GeoNameTag(szlabel));
	pPhysMotherVolume->add(new GeoTransform(TotTransform));
	pPhysMotherVolume->add(pPhysLGuide);
	sprintf(szlabel,"%s_Q%i_LGuideSurface[%i]",pszStationName,nQuarticID,nBarID);
	bsContainer->push_back(GeoBorderSurface(szlabel, pPhysLGuide, pPhysMotherVolume, m_pOpticalSurface));

	//Radiator elbow
	double fRadYDim=BarDims.fRadYDim;
	double fElbowXDim=BarDims.fRadYDim;
	TotTransform=TransInMotherVolume*HepGeom::Translate3D(fX1Pos-0.5*BarDims.fLGuideWidth+0.5*fElbowXDim+fTaperOffset,BarDims.fLGuideLength+0.5*fRadYDim,fX2Pos);
	pSolVolume=new GeoBox(0.5*fElbowXDim,0.5*fRadYDim,0.5*BarDims.fLBarThickness);

	falpha=45.0*CLHEP::deg;
	fd=fRadYDim/sin(falpha);//sqrt(2.0)*fRadYDim;
	vecA1=-fd*cos(falpha)*CLHEP::Hep3Vector(0.0,1.0,0.0);
	vecA2=+0.5*fd*sqrt(2.0)*(CLHEP::HepRotationZ(+(45*CLHEP::deg-falpha))*CLHEP::Hep3Vector(0.0,1.0,0.0)).unit();
	vecX=vecA1+vecA2;
	vecCutShift=CLHEP::Hep3Vector(-0.5*fElbowXDim,0.5*fRadYDim,0.0)+CLHEP::Hep3Vector(vecX);
	Rot2.rotateZ(-falpha);

	pSolAux=new GeoBox(0.5*fd,0.5*fd,0.5*BarDims.fLBarThickness+SLIMCUT);
	TransCut=HepGeom::Transform3D(Rot2,vecCutShift);
	pMoveCut=new GeoShapeShift(pSolAux, TransCut);
	pSolVolume=new GeoShapeSubtraction(pSolVolume, pMoveCut);

	sprintf(szlabel,"%s_Q%i_LogRadiator[%i]",pszStationName,nQuarticID,nBarID);
	GeoLogVol* pLogRadiator=new GeoLogVol(szlabel,pSolVolume,m_MapMaterials[std::string("Quartz")]);
	GeoOpticalPhysVol* pPhysRadiator=new GeoOpticalPhysVol(pLogRadiator);
	sprintf(szlabel,"%s_Q%i_Radiator[%i]",pszStationName,nQuarticID,nBarID);
	pPhysMotherVolume->add(new GeoNameTag(szlabel));
	pPhysMotherVolume->add(new GeoTransform(TotTransform));
	pPhysMotherVolume->add(pPhysRadiator);
	sprintf(szlabel,"%s_Q%i_RadiatorElbowSurface[%i]",pszStationName,nQuarticID,nBarID);
	bsContainer->push_back(GeoBorderSurface(szlabel, pPhysRadiator, pPhysMotherVolume, m_pReflectionOptSurface));

	//Radiator
	double fRadLength=BarDims.fRadLength-(BarDims.fLGuideWidth-fTaperOffset);
	TotTransform=TransInMotherVolume*HepGeom::Translate3D(fX1Pos-0.5*BarDims.fLGuideWidth+fElbowXDim+fTaperOffset+0.5*fRadLength,BarDims.fLGuideLength+0.5*fRadYDim,fX2Pos);
	pSolVolume=new GeoBox(0.5*fRadLength,0.5*fRadYDim,0.5*BarDims.fLBarThickness);

	if(TofCfg.bApplyBottomCut)
	{
		falpha=48.0*CLHEP::deg;
		fd=BarDims.fLBarThickness/sin(falpha);
		vecA1=-fd*cos(falpha)*CLHEP::Hep3Vector(1.0,0.0,0.0);
		vecA2=+0.5*fd*sqrt(2.0)*(CLHEP::HepRotationY(-(45*CLHEP::deg-falpha))*CLHEP::Hep3Vector(1.0,0.0,0.0)).unit();
		vecX=vecA1+vecA2;
		vecCutShift=CLHEP::Hep3Vector(0.5*fRadLength,0.0,0.5*BarDims.fLBarThickness)+CLHEP::Hep3Vector(vecX);
		Rot3.rotateY(falpha);

		pSolAux=new GeoBox(0.5*fd,0.5*fRadYDim+SLIMCUT,0.5*fd);
		TransCut=HepGeom::Transform3D(Rot3,vecCutShift);
		pMoveCut=new GeoShapeShift(pSolAux, TransCut);
		pSolVolume=new GeoShapeSubtraction(pSolVolume, pMoveCut);
	}

	sprintf(szlabel,"%s_Q%i_LogRadiator[%i]",pszStationName,nQuarticID,nBarID);
	pLogRadiator=new GeoLogVol(szlabel,pSolVolume,m_MapMaterials[std::string("Quartz")]);
	pPhysRadiator=new GeoOpticalPhysVol(pLogRadiator);
	sprintf(szlabel,"%s_Q%i_Radiator[%i]",pszStationName,nQuarticID,nBarID);
	pPhysMotherVolume->add(new GeoNameTag(szlabel));
	pPhysMotherVolume->add(new GeoTransform(TotTransform));
	pPhysMotherVolume->add(pPhysRadiator);
	sprintf(szlabel,"%s_Q%i_RadiatorSurface[%i]",pszStationName,nQuarticID,nBarID);
	bsContainer->push_back(GeoBorderSurface(szlabel, pPhysRadiator, pPhysMotherVolume, m_pOpticalSurface));
}

void AFP_GeoModelFactory::addSensor(const char* pszStationName, const int nQuarticID, GeoOpticalPhysVol* pPhysMotherVolume,
									HepGeom::Transform3D &TransInMotherVolume, GeoBorderSurfaceContainer* bsContainer)
{
	int i,j,nPixelID;
	char szlabel[64];
	double fX1Pos, fX2Pos;
	HepGeom::Transform3D TotTransform;

	eAFPStation eStation=m_pGeometry->parseStationName(pszStationName);
	AFP_TDCONFIGURATION TofCfg=m_CfgParams.tdcfg[eStation];
	double fSensor2BarDistance=(TofCfg.bEmulateImmersion)? 0.0:m_AfpConstants.ToF_Sensor2BarDist;

	GeoBox* pSolSensor=new GeoBox(0.5*TofCfg.fPixelX1Dim,0.5*m_AfpConstants.ToF_SensorThickness,0.5*TofCfg.fPixelX2Dim);

	for(i=0;i<TofCfg.nX1PixCnt;i++)
	{
		for(j=0;j<TofCfg.nX2PixCnt;j++)
		{
			nPixelID=10*(i+1)+(j+1);
			fX1Pos=-(0.0+i)*TofCfg.fPixelX1Dim;//-(0.5+i)*m_TofCfg.fPixelX1Dim
			fX2Pos=(0.0+j)*TofCfg.fPixelX2Dim;//(0.5+j)*m_TofCfg.fPixelX2Dim;

			sprintf(szlabel,"%s_Q%i_LogTDSensor[%i][%02i]",pszStationName,nQuarticID,nPixelID,nPixelID);
			TotTransform=TransInMotherVolume*HepGeom::Translate3D(fX1Pos,-0.5*m_AfpConstants.ToF_SensorThickness-fSensor2BarDistance,fX2Pos);
			GeoLogVol* pLogSensor=new GeoLogVol(szlabel,pSolSensor,m_MapMaterials["SiliconPMT"]);
			GeoOpticalPhysVol* pPhysSensor=new GeoOpticalPhysVol(pLogSensor);
			sprintf(szlabel,"%s_Q%i_TDSensor[%i][%02i]",pszStationName,nQuarticID,nPixelID,nPixelID);
			pPhysMotherVolume->add(new GeoNameTag(szlabel));
			pPhysMotherVolume->add(new GeoTransform(TotTransform));
			pPhysMotherVolume->add(pPhysSensor);
			sprintf(szlabel,"%s_Q%i_SensorSurface[%i]",pszStationName,nQuarticID,nPixelID);
			bsContainer->push_back(GeoBorderSurface(szlabel, pPhysSensor, pPhysMotherVolume, m_pOpticalSurface));
			//m_mapTransToSensorCS[nPixelID]=TransInMotherVolume.inverse();

			pPhysSensor=NULL;
		}
	}

	/*
    int i;
    double fFirstSensorZPos;
    HepGeom::Transform3D TotTransform;
    char szLabel[64];

    double fSensorLength=LQBarDims.fLBarZDim/LQBarDims.nNumOfSensors;
    bool bIsOddSensorNumber=(LQBarDims.nNumOfSensors%2==0)? false:true;
    GeoBox* pSolSensor=new GeoBox(0.5*TD_SENSORTHICKNESS,0.5*LQBarDims.fHorzBarYDim,0.5*fSensorLength);

    fFirstSensorZPos=bIsOddSensorNumber? -((LQBarDims.nNumOfSensors-1)>>1)*fSensorLength:-((LQBarDims.nNumOfSensors>>1)-0.5)*fSensorLength;

    for(i=0;i<LQBarDims.nNumOfSensors;i++)
    {
        TotTransform=TransInMotherVolume*HepGeom::TranslateZ3D(fFirstSensorZPos+i*fSensorLength);
        sprintf(szLabel,"%s_Q%i_LogTDSensor[%i][%02i]",pszStationName,nQuarticID,nLQBarID,(m_CfgParams.tdcfg.nColsCnt>1)? nLQBarID:i);
        GeoLogVol* pLogSensor=new GeoLogVol(szLabel,pSolSensor,m_MapMaterials["SiliconPMT"]);
        GeoOpticalPhysVol* pPhysSensor=new GeoOpticalPhysVol(pLogSensor);
        //sprintf(szLabel,"%s_Q%i_TDSensor[%i]",pszStationName,nQuarticID,nRowsCnt*i+j);
        sprintf(szLabel,"%s_Q%i_TDSensor[%i][%02i]",pszStationName,nQuarticID,nLQBarID,(m_CfgParams.tdcfg.nColsCnt>1)? nLQBarID:i);
        pPhysMotherVolume->add(new GeoNameTag(szLabel));
        pPhysMotherVolume->add(new GeoTransform(TotTransform));
        pPhysMotherVolume->add(pPhysSensor);
        //TotTransform=TransInMotherVolume*HepGeom::TranslateZ3D(fFirstSensorZPos+i*fSensorLength);
        //sprintf(szLabel,"%s_Q%i_LogSensor[%i][%02i]",pszStationName,nQuarticID,nLQBarID,(m_LBarDimensions.nNumOfLBars>1)? nLQBarID:i);
        //G4LogicalVolume* pLogSensor=new G4LogicalVolume(pSolSensor,G4Material::GetMaterial("BoroGlass"),szbuff,NULL,0,0);
        //sprintf(szbuff,"Sensor%02i",i);
        //G4VPhysicalVolume* pPhysSensor=new G4PVPlacement(TotTransform,pLogSensor,szbuff,pLogMotherVolume,false,0);
        //m_mapTransToSensorCS[i]=TransInMotherVolume.inverse();

        pPhysSensor=NULL;
    }
*/
}
