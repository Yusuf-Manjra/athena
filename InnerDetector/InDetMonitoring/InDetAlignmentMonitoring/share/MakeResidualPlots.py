#--
#
# Basic Barrel Residual distributions
#
# Pixel
PIXX = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b_residualx","noFitWithStats",rootFiles,nFiles,normaliseHistos)#,unitArea)
DrawPlots(PIXX, outputDir+"/"+"PIXX."+oFext, "Pixel barrel + IBL", "Hits on tracks / 12 #mum",
          "Local x residual [mm]", 0.18, 0.88, "#mum",canvasText,makeOutput,0.59, 0.87)

PIXY = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b_residualy","noFitWithStats",rootFiles,nFiles,normaliseHistos)#,unitArea)
DrawPlots(PIXY, outputDir+"/"+"PIXY."+oFext, "Pixel barrel + IBL", "Hits on tracks / 16 #mum",
          "Local y residual [mm]", 0.18, 0.88, "#mum",canvasText,makeOutput,0.59, 0.87)

# SCT
SCTX = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_b_residualx","noFitWithStats",rootFiles,nFiles,normaliseHistos)#,unitArea)
DrawPlots(SCTX, outputDir+"/"+"SCTX."+oFext, "SCT barrel", "Hits on tracks / 8 #mum",
          "Local x residual [mm]", 0.18, 0.88, "#mum",canvasText,makeOutput,0.59, 0.87)

#SCTY = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_b_residualy","noFitWithStats",rootFiles,nFiles,normaliseHistos)#,unitArea)
#DrawPlots(SCTY, outputDir+"/"+"SCTY."+oFext, "SCT barrel", "Hits on tracks / 10 #mum",
#          "Local y residual [mm]", 0.18, 0.88, "#mum",canvasText,makeOutput,0.59, 0.87)

# TRT
TRTR = MakePlots(trtResidualsDir,legendTitles,markerColors,markerStyles,"trt_b_residualR","noFitWithStats",rootFiles,nFiles,normaliseHistos)#,unitArea)
DrawPlots(TRTR, outputDir+"/"+"TRTR."+oFext, "TRT barrel", "Hits on tracks / 22 #mum",
          "residual [mm]", 0.18, 0.88, "#mum",canvasText,makeOutput,0.59, 0.87)

## Individual Barrel Layer Residual Distributions

PIXX0 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b0_residualx","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXX0, outputDir+"/"+"PIXIBL_X."+oFext, "IBL", "Hits on tracks / 12 #mum",
          "Local x residual [mm]", 0.18, 0.88, "#mum",canvasText,makeOutput,0.59, 0.87)

PIXX1 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b1_residualx","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXX1, outputDir+"/"+"PIXX1."+oFext, "Pixel Barrel L0", "Hits on tracks / 12 #mum",
          "Local x residual [mm]", 0.18, 0.88, "#mum",canvasText,makeOutput,0.59, 0.87)

PIXX2 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b2_residualx","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXX2, outputDir+"/"+"PIXX2."+oFext, "Pixel Barrel L1", "Hits on tracks / 12 #mum",
          "Local x residual [mm]", 0.18, 0.88, "#mum",canvasText,makeOutput,0.59, 0.87)

PIXX3 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b3_residualx","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXX3, outputDir+"/"+"PIXX3."+oFext, "Pixel Barrel L2", "Hits on tracks / 12 #mum",
          "Local x residual [mm]", 0.18, 0.88, "#mum",canvasText,makeOutput,0.59, 0.87)


# pixel barrel local y
PIXY0 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b0_residualy","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXY0, outputDir+"/"+"PIXIBL_Y."+oFext, "IBL", "Hits on tracks / 16 #mum",
          "Local y residual [mm]", 0.18, 0.88, "#mum",canvasText,makeOutput,0.59, 0.87)

PIXY1 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b1_residualy","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXY1, outputDir+"/"+"PIXY1."+oFext, "Pixel Barrel L0", "Hits on tracks / 16 #mum",
          "Local y residual [mm]", 0.18, 0.88, "#mum",canvasText,makeOutput,0.59, 0.87)

PIXY2 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b2_residualy","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXY2, outputDir+"/"+"PIXY2."+oFext, "Pixel Barrel L1", "Hits on tracks / 16 #mum",
          "Local y residual [mm]", 0.18, 0.88, "#mum",canvasText,makeOutput,0.59, 0.87)

PIXY3 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b3_residualy","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXY3, outputDir+"/"+"PIXY3."+oFext, "Pixel Barrel L2", "Hits on tracks / 16 #mum",
          "Local y residual [mm]", 0.18, 0.88, "#mum",canvasText,makeOutput,0.59, 0.87)


# Pixel L1 L2 L3                                                                                                                                                                              
OLDPIXX = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"oldpix_b_residualx","noFitWithStats",rootFiles,nFiles,normaliseHistos)#,unitArea)                                    
DrawPlots(OLDPIXX, outputDir+"/"+"OLDPIXX."+oFext, "Pixel Barrel", "Hits on tracks / 12 #mum",
          "Local x residual [mm]", 0.18, 0.88, "#mum",canvasText,makeOutput,0.59, 0.87)

OLDPIXY = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"oldpix_b_residualy","noFitWithStats",rootFiles,nFiles,normaliseHistos)#,unitArea)                                    
DrawPlots(OLDPIXY, outputDir+"/"+"OLDPIXY."+oFext, "Pixel Barrel", "Hits on tracks / 16 #mum",
          "Local y residual [mm]", 0.18, 0.88, "#mum",canvasText,makeOutput,0.59, 0.87)


# sct barrel
SCTX0 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_b0_residualx","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(SCTX0, outputDir+"/"+"SCTX0."+oFext, "SCT Barrel L0", "Hits on tracks / 8 #mum",
          "Local x residual [mm]", 0.18, 0.88, "#mum",canvasText,makeOutput,0.59, 0.87)

SCTX1 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_b1_residualx","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(SCTX1, outputDir+"/"+"SCTX1."+oFext, "SCT Barrel L1", "Hits on tracks / 8 #mum",
          "Local x residual [mm]", 0.18, 0.88, "#mum",canvasText,makeOutput,0.59, 0.87)

SCTX2 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_b2_residualx","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(SCTX2, outputDir+"/"+"SCTX2."+oFext, "SCT Barrel L2", "Hits on tracks / 8 #mum",
          "Local x residual [mm]", 0.18, 0.88, "#mum",canvasText,makeOutput,0.59, 0.87)

SCTX3 = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_b3_residualx","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(SCTX3, outputDir+"/"+"SCTX3."+oFext, "SCT Barrel L3", "Hits on tracks / 8 #mum",
          "Local x residual [mm]", 0.18, 0.88, "#mum",canvasText,makeOutput,0.59, 0.87)

#
# Residual mean and width distributions as a function of layer, ring, stave in barrel 
#

PixelXEta = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b_xresvsmodeta","noFit",rootFiles,nFiles,False)
DrawPlots(PixelXEta, outputDir+"/"+"PixelXvsModEta."+oFext, "Pixel Barrel", "residual mean [mm]",
          "Module Eta", 0.18, 0.88, "#mum",
          canvasText,makeOutput,0.59, 0.87,False) #False
'''
PixelXEtaW = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b_xresvsmodeta_width","noFit",rootFiles,nFiles,False)
DrawPlots(PixelXEtaW, outputDir+"/"+"PixelXvsModEtaWidth."+oFext, "Pixel Barrel", "residual width [mm]",
          "Module Eta", 0.18, 0.88, "#mum",
          canvasText,makeOutput,0.59, 0.87,False)

PixelXPhi = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b_xresvsmodphi","noFit",rootFiles,nFiles,False)
DrawPlots(PixelXPhi, outputDir+"/"+"PixelXvsModPhi."+oFext, "Pixel Barrel", "residual mean [mm]",
          "Module Phi", 0.18, 0.88, "#mum",
          canvasText,makeOutput,0.59, 0.87,False)

PixelXPhiW = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b_xresvsmodphi_width","noFit",rootFiles,nFiles,False)
DrawPlots(PixelXPhiW, outputDir+"/"+"PixelXvsModPhiWidth."+oFext, "Pixel Barrel", "residual width [mm]",
          "Module Phi", 0.18, 0.88, "#mum",
          canvasText,makeOutput,0.59, 0.87,False)
'''
SiBarrelResXMean = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"si_barrel_resX_mean","noFit",rootFiles,nFiles,False)
DrawPlots(SiBarrelResXMean, outputDir+"/"+"SiBarrelResXMean."+oFext, "Pixel/SCT Barrel", "residual mean [mm]",
          "", 0.18, 0.88, "#mum",
          canvasText,makeOutput,0.59, 0.87,False)

SiBarrelResXWidth = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"si_barrel_resX_rms","noFit",rootFiles,nFiles,False)
DrawPlots(SiBarrelResXWidth, outputDir+"/"+"SiBarrelResXWidth."+oFext, "Pixel/SCT Barrel", "residual width [mm]",
          "", 0.18, 0.88, "#mum",
          canvasText,makeOutput,0.59, 0.87,False)

# Priscilla's add
SCTXPhi_eca = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_eca_xresvsmodphi","noFit",rootFiles,nFiles,False)
DrawPlots(SCTXPhi_eca, outputDir+"/"+"SCTXvsModPhi_eca."+oFext, "SCT Endcap A", "residual mean [mm]",
          "Module Phi", 0.18, 0.88, "#mum",
          canvasText,makeOutput,0.59, 0.87,False)

# SCTXPhiW_eca = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_eca_xresvsmodphi_width","noFit",rootFiles,nFiles,False)
# DrawPlots(SCTXPhiW_eca, outputDir+"/"+"SCTXvsModPhiWidth_eca."+oFext, "SCT Endcap A", "residual width [mm]",
#           "Module Phi", 0.18, 0.88, "#mum",
#           canvasText,makeOutput,0.59, 0.87,False)

SCTXPhi_ecc = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_ecc_xresvsmodphi","noFit",rootFiles,nFiles,False)
DrawPlots(SCTXPhi_ecc, outputDir+"/"+"SCTXvsModPhi_ecc."+oFext, "SCT Endcap C", "residual mean [mm]",
          "Module Phi", 0.18, 0.88, "#mum",
          canvasText,makeOutput,0.59, 0.87,False)

# SCTXPhiW_ecc = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_ecc_xresvsmodphi_width","noFit",rootFiles,nFiles,False)
# DrawPlots(SCTXPhiW_ecc, outputDir+"/"+"SCTXvsModPhiWidth_ecc."+oFext, "SCT Endcap C", "residual width [mm]",
#           "Module Phi", 0.18, 0.88, "#mum",
#           canvasText,makeOutput,0.59, 0.87,False)


## Basic Endcap Residual distributions

# draws combined ECA and ECC residual distribution
PIXECX = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_ec_residualx","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXECX, outputDir+"/"+"PIXECX."+oFext, "Pixel end-caps", "Hits on tracks / 12 #mum",
          "Local x residual [mm]", 0.18, 0.88, "#mum",canvasText,makeOutput,0.59, 0.87)

# Priscilla, for approval plot like in https://atlas.web.cern.ch/Atlas/GROUPS/PHYSICS/CONFNOTES/ATLAS-CONF-2011-012/fig_14b.png
PIXECY = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_ec_residualy","noFitWithStats",rootFiles,nFiles,normaliseHistos)#,unitArea)
DrawPlots(PIXECY, outputDir+"/"+"PIXECY."+oFext, "Pixel end-caps", "Hits on tracks / 16 #mum",
          "Local y residual [mm]", 0.18, 0.88, "#mum",canvasText,makeOutput,0.59, 0.87)

PIXECAX = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_eca_residualx","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXECAX, outputDir+"/"+"PIXECAX."+oFext, "PIX Endcap A", "Hits on tracks / 12 #mum",
          "Local x residual [mm]", 0.18, 0.88, "#mum",
          canvasText,makeOutput)

PIXECCX = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_ecc_residualx","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXECCX, outputDir+"/"+"PIXECCX."+oFext, "PIX Endcap C", "Hits on tracks / 12 #mum",
          "Local x residual [mm]", 0.18, 0.88, "#mum",
          canvasText,makeOutput)

PIXECAY = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_eca_residualy","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXECAY, outputDir+"/"+"PIXECAY."+oFext, "PIX Endcap A", "Hits on tracks / 16 #mum",
          "Local y residual [mm]", 0.18, 0.88, "#mum",
          canvasText,makeOutput)

PIXECCY = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_ecc_residualy","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(PIXECCY, outputDir+"/"+"PIXECCY."+oFext, "PIX Endcap C", "Hits on tracks / 16 #mum",
          "Local y residual [mm]", 0.18, 0.88, "#mum",
          canvasText,makeOutput)


# draws combined ECA and ECC residual distribution
SCTECX = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_ec_residualx","noFitWithStats",rootFiles,nFiles,normaliseHistos)#,unitArea)
DrawPlots(SCTECX, outputDir+"/"+"SCTECX."+oFext, "SCT Endcap", "Hits on tracks / 8 #mum",
          "Local x residual [mm]", 0.18, 0.88, "#mum",canvasText,makeOutput,0.59, 0.87)


SCTECAX = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_eca_residualx","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(SCTECAX, outputDir+"/"+"SCTECAX."+oFext, "SCT Endcap A", "Hits on tracks / 8 #mum",
          "x residual [mm]", 0.18, 0.88, "#mum",
          canvasText,makeOutput,0.59, 0.87)

SCTECCX = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_ecc_residualx","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(SCTECCX, outputDir+"/"+"SCTECCX."+oFext, "SCT Endcap C", "Hits on tracks / 8 #mum",
          "x residual [mm]", 0.18, 0.88, "#mum",
          canvasText,makeOutput,0.59, 0.87)

TRTECA = MakePlots(trtResidualsDir,legendTitles,markerColors,markerStyles,"trt_ec_residualR_Endcap_A","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(TRTECA, outputDir+"/"+"TRTECA."+oFext, "TRT Endcap A", "Hits on tracks / 22 #mum",
          "residual [mm]", 0.18, 0.88, "#mum",canvasText,makeOutput)

TRTECC = MakePlots(trtResidualsDir,legendTitles,markerColors,markerStyles,"trt_ec_residualR_Endcap_C","noFitWithStats",rootFiles,nFiles,normaliseHistos,unitArea)
DrawPlots(TRTECC, outputDir+"/"+"TRTECC."+oFext, "TRT Endcap C", "Hits on tracks / 22 #mum",
          "residual [mm]", 0.18, 0.88, "#mum",canvasText,makeOutput)

TRTEC = MakePlots(trtResidualsDir,legendTitles,markerColors,markerStyles,"trt_ec_residualR","noFitWithStats",rootFiles,nFiles,normaliseHistos)#,unitArea)
DrawPlots(TRTEC, outputDir+"/"+"TRTEC."+oFext, "TRT end-caps", "Hits on tracks / 22 #mum",
          "residual [mm]", 0.18, 0.88, "#mum",canvasText,makeOutput)


TRTBprof = MakePlots(trtResidualsDir,legendTitles,markerColors,markerStyles,"trt_b_aveResOverPhiVsStrawLayer","noFit",rootFiles,nFiles,False)
DrawPlots(TRTBprof, outputDir+"/"+"TRTBprof."+oFext, "TRT Barrel", "residual [#mum]", "wire number", 
          0.18, 0.88, "#mum", canvasText, makeOutput, 0.59, 0.87, False)

TRTB0R = MakePlots(trtResidualsDir,legendTitles,markerColors,markerStyles,"trt_b_aveRes_l0","noFit",rootFiles,nFiles,False)
DrawPlots(TRTB0R, outputDir+"/"+"TRTB0R."+oFext, "TRT Barrel layer 0", "residual [#mum]", "sector", 
          0.18, 0.88, "#mum", canvasText, makeOutput, 0.59, 0.87, False)

TRTB1R = MakePlots(trtResidualsDir,legendTitles,markerColors,markerStyles,"trt_b_aveRes_l1","noFit",rootFiles,nFiles,False)
DrawPlots(TRTB1R, outputDir+"/"+"TRTB1R."+oFext, "TRT Barrel layer 1", "residual [#mum]", "sector", 
          0.18, 0.88, "#mum", canvasText, makeOutput, 0.59, 0.87, False)

TRTB2R = MakePlots(trtResidualsDir,legendTitles,markerColors,markerStyles,"trt_b_aveRes_l2","noFit",rootFiles,nFiles,False)
DrawPlots(TRTB2R, outputDir+"/"+"TRTB2R."+oFext, "TRT Barrel layer 2", "residual [#mum]", "sector", 
          0.18, 0.88, "#mum", canvasText, makeOutput, 0.59, 0.87, False)
#
# -- Extended plots
#
if (userExtended or True):
    # IBL split per 3D and planar sensors
    # -- local x
    PIXIBL_3DC_xRES = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"IBL_residualx_3DCSensors","noFitWithStats",rootFiles,nFiles,normaliseHistos)#,unitArea)
    DrawPlots(PIXIBL_3DC_xRES, outputDir+"/"+"PIXIBL_3DC_xRES."+oFext, "IBL 3D sensors. C side", "Hits on track", "Local x residual [mm]",0.18, 0.88, "#mum",
              canvasText,makeOutput,0.59, 0.87)

    PIXIBL_3DA_xRES = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"IBL_residualx_3DASensors","noFitWithStats",rootFiles,nFiles,normaliseHistos)#,unitArea)
    DrawPlots(PIXIBL_3DA_xRES, outputDir+"/"+"PIXIBL_3DA_xRES."+oFext, "IBL 3D sensors. A side", "Hits on track", "Local x residual [mm]",0.18, 0.88, "#mum",
              canvasText,makeOutput,0.59, 0.87)

    PIXIBL_3D_xRES = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"IBL_residualx_3DSensors","noFitWithStats",rootFiles,nFiles,normaliseHistos)#,unitArea)
    DrawPlots(PIXIBL_3D_xRES, outputDir+"/"+"PIXIBL_3D_xRES."+oFext, "IBL 3D sensors", "Hits on track", "Local x residual [mm]",0.18, 0.88, "#mum",
              canvasText,makeOutput,0.59, 0.87)
    
    PIXIBL_Planar_xRES = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"IBL_residualx_PlanarSensors","noFitWithStats",rootFiles,nFiles,normaliseHistos)#,unitArea)
    DrawPlots(PIXIBL_Planar_xRES, outputDir+"/"+"PIXIBL_Planar_xRES."+oFext, "IBL Planar sensors", "Hits on track", "Local x residual [mm]",0.18, 0.88, "#mum",
              canvasText,makeOutput,0.59, 0.87)
    
    # -- local y
    PIXIBL_3D_yRES = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"IBL_residualy_3DSensors","noFitWithStats",rootFiles,nFiles,normaliseHistos)#,unitArea)
    DrawPlots(PIXIBL_3D_yRES, outputDir+"/"+"PIXIBL_3D_yRES."+oFext, "IBL 3D sensors", "Hits on track", "Local y residual [mm]",0.18, 0.88, "#mum",
              canvasText,makeOutput,0.59, 0.87)
    
    PIXIBL_Planar_yRES = MakePlots(residualsDir,legendTitles,markerColors,markerStyles,"IBL_residualy_PlanarSensors","noFitWithStats",rootFiles,nFiles,normaliseHistos)#,unitArea)
    DrawPlots(PIXIBL_Planar_yRES, outputDir+"/"+"PIXIBL_Planar_yRES."+oFext, "IBL Planar sensors", "Hits on track", "Local y residual [mm]",0.18, 0.88, "#mum",
              canvasText,makeOutput,0.59, 0.87)
    

if (userExtended):
    PIXX0vsINCANG = MakeProfPlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b0_residualx_inciphi","noFit",rootFiles,nFiles,True)
    DrawPlots(PIXX0vsINCANG,outputDir+"/"+"PIXIBL_XvsINCANG."+oFext,"IBL","Local x residual [mm]","Incident angle [radians]",0.15,0.89,"#mum", canvasText,makeOutput, 0.59, 0.87, False);
    
    PIXX1vsINCANG = MakeProfPlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b1_residualx_inciphi","noFit",rootFiles,nFiles,True)
    DrawPlots(PIXX1vsINCANG,outputDir+"/"+"PIXX1vsINCANG."+oFext,"PIXEL Layer 0","Local x residual [mm]","Incident angle [radians]",0.15,0.88,"#mum", canvasText,makeOutput, 0.59, 0.87, False);
    
    PIXX2vsINCANG = MakeProfPlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b2_residualx_inciphi","noFit",rootFiles,nFiles,True)
    DrawPlots(PIXX2vsINCANG,outputDir+"/"+"PIXX2vsINCANG."+oFext,"PIXEL Layer 1","Local x residual [mm]","Incident angle [radians]",0.15,0.88,"#mum", canvasText,makeOutput, 0.59, 0.87, False);
    
    PIXX3vsINCANG = MakeProfPlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b3_residualx_inciphi","noFit",rootFiles,nFiles,True)
    DrawPlots(PIXX3vsINCANG,outputDir+"/"+"PIXX3vsINCANG."+oFext,"PIXEL Layer 2","Local x residual [mm]","Incident angle [radians]",0.15,0.88,"#mum", canvasText,makeOutput, 0.59, 0.87, False);
    
    PIXX0vsCLUSIZ = MakeProfPlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b0_residualx_clustersizePhi","noFit",rootFiles,nFiles,True)
    DrawPlots(PIXX0vsCLUSIZ,outputDir+"/"+"PIXX0vsCLUSIZE."+oFext,"IBL","Local x residual [mm]","Cluster size",0.15,0.88,"#mum", canvasText,makeOutput, 0.59, 0.87, False);

    PIXX1vsCLUSIZ = MakeProfPlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b1_residualx_clustersizePhi","noFit",rootFiles,nFiles,True)
    DrawPlots(PIXX1vsCLUSIZ,outputDir+"/"+"PIXX1vsCLUSIZE."+oFext,"PIXEL Layer 0","Local x residual [mm]","Cluster size",0.15,0.88,"#mum", canvasText,makeOutput, 0.59, 0.87, False);
    
    PIXX2vsCLUSIZ = MakeProfPlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b2_residualx_clustersizePhi","noFit",rootFiles,nFiles,True)
    DrawPlots(PIXX2vsCLUSIZ,outputDir+"/"+"PIXX2vsCLUSIZE."+oFext,"PIXEL Layer 1","Local x residual [mm]","Cluster size",0.15,0.88,"#mum", canvasText,makeOutput, 0.59, 0.87, False);
    
    PIXX3vsCLUSIZ = MakeProfPlots(residualsDir,legendTitles,markerColors,markerStyles,"pix_b3_residualx_clustersizePhi","noFit",rootFiles,nFiles,True)
    DrawPlots(PIXX3vsCLUSIZ,outputDir+"/"+"PIXX3vsCLUSIZE."+oFext,"PIXEL Layer 2","Local x residual [mm]","Cluster size",0.15,0.88,"#mum", canvasText,makeOutput, 0.59, 0.87, False);

	# SCT 
    SCTX0vsCLUSIZ = MakeProfPlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_b0_residualx_clustersizePhi","noFit",rootFiles,nFiles,True)
    DrawPlots(SCTX0vsCLUSIZ,outputDir+"/"+"SCTX0vsCLUSIZE."+oFext,"SCT Barrel layer 0","Local x residual [mm]","Cluster size",0.15,0.89,"#mum", canvasText,makeOutput, 0.59, 0.87, False);

    SCTX1vsCLUSIZ = MakeProfPlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_b1_residualx_clustersizePhi","noFit",rootFiles,nFiles,True)
    DrawPlots(SCTX1vsCLUSIZ,outputDir+"/"+"SCTX1vsCLUSIZE."+oFext,"SCT Barrel layer 1","Local x residual [mm]","Cluster size",0.15,0.89,"#mum", canvasText,makeOutput, 0.59, 0.87, False);

    SCTX2vsCLUSIZ = MakeProfPlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_b2_residualx_clustersizePhi","noFit",rootFiles,nFiles,True)
    DrawPlots(SCTX2vsCLUSIZ,outputDir+"/"+"SCTX2vsCLUSIZE."+oFext,"SCT Barrel layer 2","Local x residual [mm]","Cluster size",0.15,0.89,"#mum", canvasText,makeOutput, 0.59, 0.87, False);

    SCTX3vsCLUSIZ = MakeProfPlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_b3_residualx_clustersizePhi","noFit",rootFiles,nFiles,True)
    DrawPlots(SCTX3vsCLUSIZ,outputDir+"/"+"SCTX3vsCLUSIZE."+oFext,"SCT Barrel layer 3","Local x residual [mm]","Cluster size",0.15,0.89,"#mum", canvasText,makeOutput, 0.59, 0.87, False);

    # SCT vs incident angle across strips
    SCTXvsINCANG = MakeProfPlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_b0_residualsx_incidentAnglePhi_s0","noFit",rootFiles,nFiles,True)
    DrawPlots(SCTXvsINCANG,outputDir+"/"+"SCTX0s0vsIncAng."+oFext,"SCT Barrel layer 0 side 0","Local x residual [mm]","Incident angle [rad]",
              0.15,0.89,"#mum", canvasText,makeOutput, 0.59, 0.87, False);

    SCTXvsINCANG = MakeProfPlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_b1_residualsx_incidentAnglePhi_s0","noFit",rootFiles,nFiles,True)
    DrawPlots(SCTXvsINCANG,outputDir+"/"+"SCTX1s0vsIncAng."+oFext,"SCT Barrel layer 1 side 0","Local x residual [mm]","Incident angle [rad]",
              0.15,0.89,"#mum", canvasText,makeOutput, 0.59, 0.87, False);
    
    SCTXvsINCANG = MakeProfPlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_b2_residualsx_incidentAnglePhi_s0","noFit",rootFiles,nFiles,True)
    DrawPlots(SCTXvsINCANG,outputDir+"/"+"SCTX2s0vsIncAng."+oFext,"SCT Barrel layer 2 side 0","Local x residual [mm]","Incident angle [rad]",
              0.15,0.89,"#mum", canvasText,makeOutput, 0.59, 0.87, False);
    
    SCTXvsINCANG = MakeProfPlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_b3_residualsx_incidentAnglePhi_s0","noFit",rootFiles,nFiles,True)
    DrawPlots(SCTXvsINCANG,outputDir+"/"+"SCTX3s0vsIncAng."+oFext,"SCT Barrel layer 3 side 0","Local x residual [mm]","Incident angle [rad]",
              0.15,0.89,"#mum", canvasText,makeOutput, 0.59, 0.87, False);

    SCTXvsINCANG = MakeProfPlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_b0_residualsx_incidentAnglePhi_s1","noFit",rootFiles,nFiles,True)
    DrawPlots(SCTXvsINCANG,outputDir+"/"+"SCTX0s1vsIncAng."+oFext,"SCT Barrel layer 0 side 1","Local x residual [mm]","Incident angle [rad]",
              0.15,0.89,"#mum", canvasText,makeOutput, 0.59, 0.87, False);

    SCTXvsINCANG = MakeProfPlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_b1_residualsx_incidentAnglePhi_s1","noFit",rootFiles,nFiles,True)
    DrawPlots(SCTXvsINCANG,outputDir+"/"+"SCTX1s1vsIncAng."+oFext,"SCT Barrel layer 1 side 1","Local x residual [mm]","Incident angle [rad]",
              0.15,0.89,"#mum", canvasText,makeOutput, 0.59, 0.87, False);
    
    SCTXvsINCANG = MakeProfPlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_b2_residualsx_incidentAnglePhi_s1","noFit",rootFiles,nFiles,True)
    DrawPlots(SCTXvsINCANG,outputDir+"/"+"SCTX2s1vsIncAng."+oFext,"SCT Barrel layer 2 side 1","Local x residual [mm]","Incident angle [rad]",
              0.15,0.89,"#mum", canvasText,makeOutput, 0.59, 0.87, False);
    
    SCTXvsINCANG = MakeProfPlots(residualsDir,legendTitles,markerColors,markerStyles,"sct_b3_residualsx_incidentAnglePhi_s1","noFit",rootFiles,nFiles,True)
    DrawPlots(SCTXvsINCANG,outputDir+"/"+"SCTX3s1vsIncAng."+oFext,"SCT Barrel layer 3 side 1","Local x residual [mm]","Incident angle [rad]",
              0.15,0.89,"#mum", canvasText,makeOutput, 0.59, 0.87, False);

    # SCT barrel layer 0 Upper sectors (useful for cosmics)
    SCTUpper = MakePlotsFrom3D(residualsDir,legendTitles,markerColors,markerStyles, "sct_b0_xresvsmodetaphi_3d", "noFitWithStats", rootFiles, nFiles, False, 7, 10) 
    DrawPlots(SCTUpper, outputDir+"/"+"SCT_upper."+oFext, "SCT Layer 0 upper", "Average local x residual [mm]",
              "local x residual [mm]", 0.17, 0.89, "#mum",canvasText,makeOutput,0.65,0.89, False)
    
    SCTSides = MakePlotsFrom3D(residualsDir,legendTitles,markerColors,markerStyles, "sct_b0_xresvsmodetaphi_3d", "noFitWithStats", rootFiles, nFiles, False, 0, 4) 
    DrawPlots(SCTSides, outputDir+"/"+"SCT_sides."+oFext, "SCT Layer 0 sides", "Average local x residual [mm]",
              "local x residual [mm]", 0.17, 0.89, "#mum",canvasText,makeOutput,0.65,0.89, False)

print " -- MakeResidualPlots -- completed "
