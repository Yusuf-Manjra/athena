/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetUncertainties/JetUncertaintiesTool.h"
#include "JetUncertainties/Helpers.h"

#include "xAODJet/Jet.h"
#include "xAODJet/JetContainer.h"
#include "xAODJet/JetAuxContainer.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODEventInfo/EventInfoContainer.h"
#include "xAODEventInfo/EventInfoAuxContainer.h"

#include "../testingMacros/atlasstyle/AtlasStyle.C"
#include "../testingMacros/atlasstyle/AtlasLabels.C"

#include "TString.h"
#include "TCanvas.h"
#include "TH1D.h"
#include "TGraphErrors.h"
#include "TColor.h"
#include "TBox.h"
#include "math.h"
#include <vector>

#define TESTLINE printf("Reached line %d\n",__LINE__);

// Kate's comparison plot colours
TColor blue1(1000,13.0/255.0,159.0/255.0,216.0/255.0);
TColor mauve(1001,133.0/255.0,105.0/255,207.0/255.0);
TColor green(1002,124.0/255.0,194.0/255.0,66.0/255.0);
TColor darkred(1003,248.0/255.0,14.0/255.0,39.0/255.0);
TColor mango(1004,238.0/255.0, 206.0/255.0, 0.0/255.0);

static TBox * myTBox = new TBox();

float maxYuser = -1.;

// Options helper
std::vector<TString> globalOptions;
TString getOptionValue(const TString optionName)
{
    for (size_t iOpt = 0; iOpt < globalOptions.size(); ++iOpt)
    {
        std::vector<TString> option = jet::utils::vectorize<TString>(globalOptions.at(iOpt),"=");
        if (option.size() != 2)
        {
            printf("Bad option, check formatting: %s\n",globalOptions.at(iOpt).Data());
            exit(-1);
        }
        if (!option.at(0).CompareTo(optionName,TString::kIgnoreCase))
            return option.at(1);
    }
    return "";
}
bool IsDijet()
{
    const TString isDijet = getOptionValue("isDijet");
    return isDijet != "" && jet::utils::getTypeObjFromString<bool>(isDijet);
}

bool IsLargeR()
{
    const TString isLargeR = getOptionValue("isLargeR");
    return isLargeR != "" && jet::utils::getTypeObjFromString<bool>(isLargeR);
}

bool IsJER()
{
    const TString isJER = getOptionValue("isJER");
    return isJER != "" && jet::utils::getTypeObjFromString<bool>(isJER);
}

TString DoCompare()
{
    const TString doCompare = getOptionValue("doCompare");
    return doCompare;

}

bool CompareOnly()
{

    const TString compareOnly = getOptionValue("compareOnly");
    bool returnval = ( compareOnly != "" && jet::utils::getTypeObjFromString<bool>(compareOnly));
    return returnval;

}

std::vector<TString> GetCompareVals()
{

   std::vector<TString> returnVal;
   for (int i=1; i> 0; i++) {
     TString optname = TString::Format("compareConfig%i",i);
     const TString thisname = getOptionValue(optname);
     if (thisname != "") returnVal.push_back(thisname);
     else break;
   }
   return returnVal;

}

TString GetNamePrefix()
{
    return getOptionValue("prefix");
}

TString GetATLASLabel()
{
    const TString label = getOptionValue("ATLASLabel");
    return label != "" ? (label != "WIP" ? label : "Work in Progress") : "Internal";
}

TString GetBunchSpacing()
{
    const TString spacing = getOptionValue("bunchSpacing");
    return spacing;
}

bool DoATLASLabel()
{
    const TString doLabel = getOptionValue("DoATLASLabel");
    return doLabel == "" || jet::utils::getTypeObjFromString<bool>(doLabel);
}

bool IsPublicFormat()
{
    const TString isPublic = getOptionValue("isPublic");
    return isPublic != "" && jet::utils::getTypeObjFromString<bool>(isPublic);
}

void applyPublicFormat(TGraph& graph)
{
    const TString name = graph.GetName();
    if (name.Contains("Absolute") && name.Contains("JES"))
    {
        graph.SetLineColor(kRed);
        graph.SetLineStyle(1);
    }
    else if (name.Contains("Relative") && name.Contains("JES"))
    {
        graph.SetLineColor(kMagenta);
        graph.SetLineStyle(6);
    }
    else if (name.Contains("Flav. composition"))
    {
        graph.SetLineColor(kBlue);
        graph.SetLineStyle(2);
        if (!name.CompareTo("Flav. composition"))
            graph.SetName(Form("%s, %s",name.Data(),IsDijet()?"inclusive jets":"unknown composition"));
    }
    else if (name.Contains("Flav. response"))
    {
        graph.SetLineColor(kGreen+1);
        graph.SetLineStyle(3);
        if (!name.CompareTo("Flav. response"))
            graph.SetName(Form("%s, %s",name.Data(),IsDijet()?"inclusive jets":"unknown composition"));
    }
    else if (name.Contains("Pileup"))
    {
        graph.SetLineColor(kGray+1);
        graph.SetLineStyle(4);
    }
    else if (name.Contains("Punch-through"))
    {
        graph.SetLineColor(kOrange+7);
        graph.SetLineStyle(5);
    }
    else if (name.Contains("nonclosure") || name.Contains("NonClosure"))
    {
        graph.SetLineColor(kCyan);
        graph.SetLineStyle(7);
    }
    else
        printf("Unexpected name for public format, not changing style of graph: %s\n",name.Data());
}

bool IsEtaDepPileup()
{
    const TString isEtaDepPileup = getOptionValue("isEtaDepPileup");
    return isEtaDepPileup != "" && jet::utils::getTypeObjFromString<bool>(isEtaDepPileup);
}

bool DrawTotal()
{
    const TString drawTotal = getOptionValue("drawTotal");
    return drawTotal == "" || jet::utils::getTypeObjFromString<bool>(drawTotal);
}

TString TotalUncName()
{
    const TString totalUncName = getOptionValue("totalUncName");
    return totalUncName == "" ? "Total uncertainty" : TString(totalUncName).ReplaceAll("__"," ");
}

bool IgnorePT()
{
    const TString ignorePT = getOptionValue("ignorePT");
    return ignorePT != "" || jet::utils::getTypeObjFromString<bool>(ignorePT);
}








void DrawText(TString txt, int col=kBlack, double y=0.88, double x=0.15, int align=12, double ts=0.042)
{
  static TLatex *tex = new TLatex();
  tex->SetNDC();
  tex->SetTextFont(42);
  tex->SetTextSize(ts);
  tex->SetTextAlign(align);
  tex->SetTextColor(col);
  tex->DrawLatex(x,y,txt); 
}

void DrawLineLabel(TString txt, double x, double y, int col, int ls, int lw) {
  static TLine *l = new TLine(); l->SetLineWidth(lw); l->SetLineColor(col); l->SetLineStyle(ls);
  l->DrawLineNDC(x-0.02,y,x+0.02,y); DrawText(txt,kBlack,y,x+0.032,12,0.036);
}

void DrawLineLabel(TString txt, double x, double y, const TGraphErrors *h) {
  DrawLineLabel(txt,x,y,h->GetLineColor(),h->GetLineStyle(),h->GetLineWidth());
}

void DrawLineLabel(TString txt, double x, double y, const TH1 *h) {
  DrawLineLabel(txt,x,y,h->GetLineColor(),h->GetLineStyle(),h->GetLineWidth());
}

void DrawFillLabel(TString txt, double x, double y, TH1 *h, bool logX = true, bool logY = false) {

/*  static TPave *box = new TPave();
//  box->SetFillColor(590);
  box->SetFillColor(h->GetFillColor());
  box->SetLineColor(h->GetLineColor());
  box->SetLineWidth(1);
  box->SetLineColor(kBlack);
  box->SetShadowColor(0);
  box->SetX1NDC(x-0.02);
  box->SetY1NDC(y-0.015);
  box->SetX2NDC(x+0.02);
  box->SetY2NDC(y+0.015);
  box->Draw();
*/
  x = x + 0.011; // Boxes in an ugly place otherwise.
  y = y + 0.003; // ""

  // Margin = (0.12,0.04,0.15,0.04)
  double xlow = h->GetXaxis()->GetXmin();
  double xhigh = h->GetXaxis()->GetXmax();
  double ylow = 0.0;  // I hate root and I am hard-coding this. Apologies future user.
  double yhigh = maxYuser; // ""
  // Now calculate user coordinates from sensible ratios given in function call :(
  double fracxaxislow = ((x-0.02)-0.12)/(1.0-0.12-0.04); 
  double fracxaxishigh = ((x+0.02)-0.12)/(1.0-0.12-0.04);
  double userxlow, userxhigh;
  if (logX) {
    userxlow = exp(log(xlow) + fracxaxislow*(log(xhigh) - log(xlow)));
    userxhigh = exp(log(xlow) + fracxaxishigh*(log(xhigh) - log(xlow)));
  } else {
    userxlow = xlow + fracxaxislow*(xhigh - xlow);
    userxhigh = xlow + fracxaxishigh*(xhigh - xlow);
  }
  double fracyaxislow = ((y-0.015)-0.15)/(1.0-0.15-0.04);
  double fracyaxishigh = ((y+0.015)-0.15)/(1.0-0.15-0.04);
  double userylow, useryhigh;
  if (logY) {
    userylow = exp(log(ylow) + fracyaxislow*(log(yhigh) - log(ylow)));
    useryhigh = exp(log(ylow) + fracyaxishigh*(log(yhigh) - log(ylow)));
  } else {
    userylow = ylow + fracyaxislow*(yhigh - ylow);
    useryhigh = ylow + fracyaxishigh*(yhigh - ylow);
  }

  myTBox->SetFillColor(h->GetFillColor());
  myTBox->SetLineColor(h->GetLineColor());
  myTBox->SetLineWidth(2);
  myTBox->SetX1(userxlow); //(80);
  myTBox->SetY1(userylow); //(0.1*y-0.005);
  myTBox->SetX2(userxhigh); //(110.0);
  myTBox->SetY2(useryhigh); //(0.1*y);
  myTBox->DrawClone("l");

  x = x - 0.011;
  y = y - 0.003;
/*
  static TPave *box = new TPave(); 
  box->SetFillColor(590); 
  box->SetLineColor(h->GetLineColor()); box->SetLineWidth(1); box->SetLineColor(kBlack);
  //box->SetFillStyle(1001);
  //box->SetShadowColor(h->GetFillColor());
  //box->SetFillColor(2);
  //cout << box->GetFillColor() << endl;
  box->DrawPave(x-0.02,y-0.015,x+0.02,y+0.015,1,"ndc");//h->GetLineWidth(),"ndc");
  //box->DrawBox(100,0.04,200,0.05);
  //  double xc[4] = {x-0.02,x+0.02,x+0.02,x-0.02};
  //double yc[4] = {y-0.015,x-0.015,y+0.015,x+0.015};
  //gPad->SetFillColor(590);
  //gPad->PaintFillArea(4,xc,yc);
  //double x1 = gPad->GetX1(), xden=(gPad->GetX2()-gPad->GetX1());
  //double y1 = gPad->GetY1(), yden=(gPad->GetY2()-gPad->GetY1());
  //box->DrawBox((x-0.02)*xden+x1,(y-0.015)*yden+y1,(x+0.02)*xden+x1,(y+0.015)*yden+y1);
  //cout << (x-0.02)*xden+x1 << endl;
*/


  /*
    No, we do not have such functions, but the conversion is easy
      double u = (x-gPad->GetX1())/(gpad->GetX2()-gPad->GetX1());
    where
    x is your user coordinate
    u will be the corresponding NDC coordinate
  */
  DrawText(txt,kBlack,y,x+0.032,12,0.036);
}



void DrawAtlasLabel(const TString& label = GetATLASLabel(), const bool right=true)
{
    double xPos = 0.15;
//    double yPos = 0.850;
    double yPos = 0.890;
    
    if (label.CompareTo("Work in Progress",TString::kIgnoreCase))
    {
        if (right)
        {
            if (!label.CompareTo("Internal"))
                xPos = 0.7;
            else if (!label.CompareTo("Preliminary"))
                xPos = 0.67;
            else if (label == "")
                xPos = 0.75;
            else
                xPos = 0.7;
        }
        ATLASLabel(xPos,yPos,label.Data(),kBlack);
    }
    else
    {
        ATLASLabel(right?0.75:xPos,yPos,"",kBlack);
        DrawText(label,kBlack,yPos-0.038,right?0.75:xPos);
    }
  
}
  
std::vector<TString> GetJetDesc(const TString& jetAlgoIn)
{
    TString jetAlgo = jetAlgoIn;
    std::vector<TString> returnvals;

    TString calib = "";
    if (jetAlgo.Contains("EMTopo") || jetAlgo.Contains("TopoEM"))
    {
        calib = "EM+JES";
        jetAlgo.ReplaceAll("EMTopo","");
        jetAlgo.ReplaceAll("TopoEM","");
    }
    else if (jetAlgo.Contains("LCTopo") || jetAlgo.Contains("TopoLC"))
    {
        calib = "LCW+JES";
        jetAlgo.ReplaceAll("LCTopo","");
        jetAlgo.ReplaceAll("TopoLC","");
    }
    if (calib == "")
    {
        printf("Failed to parse calib for: %s\n",jetAlgoIn.Data());
        returnvals.push_back("");
        return returnvals;
    }

    TString type = "";
    if (jetAlgo.BeginsWith("AntiKt"))
    {
        type = "anti-k_{t}";
        jetAlgo.ReplaceAll("AntiKt","");
    }
    else if (jetAlgo.BeginsWith("CamKt"))
    {
        type = "Cambridge-Aachen k_{t}";
        jetAlgo.ReplaceAll("CamKt","");
    }
    else if (jetAlgo.BeginsWith("Kt"))
    {
        type = "k_{t}";
        jetAlgo.ReplaceAll("Kt","");
    }
    if (type == "")
    {
        printf("Failed to parse type for: %s\n",jetAlgoIn.Data());
        returnvals.push_back("");
        return returnvals;
    }

    // AntiKt4LCTopo --> AntiKt4 --> 4
    // Hopefully all that's left is the radius parameter
    int jetR = 0;
    if (!jet::utils::getTypeObjFromString<int>(jetAlgo,jetR))
    {
        printf("Failed to get radius parameter for: %s\n",jetAlgoIn.Data());
        returnvals.push_back("");
        return returnvals;
    }
    if (jetR <= 0)
    {
        printf("Got unexpected radius parameter of %d for: %s\n",jetR,jetAlgoIn.Data());
        returnvals.push_back("");
        return returnvals;
    }

    returnvals.push_back(Form("%s %s ",type.Data(),calib.Data()));
    returnvals.push_back( Form("#font[52]{R} = %s",jetR>9?Form("%.1f",jetR/10.):Form("0.%d",jetR)));
//    return Form("%s #font[52]{R} = %s, %s",type.Data(),jetR>9?Form("%.1f",jetR/10.):Form("0.%d",jetR),calib.Data());
    return returnvals;
}

TString GetLargeJetDesc(const TString& jetAlgoIn)
{
    // First get the substructure techniques information
    // Example: AntiKt10LCTopoTrimmedPtFrac5SmallR30
    // Want to split into "AntiKt10LCTopo" and "TrimmedPtFrac5SmallR30"
    TString jetAlgo = jetAlgoIn;
    jetAlgo.ReplaceAll("Trimmed",";Trimmed").ReplaceAll("Filtered",";Filtered");
    
    // Split it
    std::vector<TString> jetAlgoStrings = jet::utils::vectorize<TString>(jetAlgo,";");
    if (jetAlgoStrings.size() != 2)
    {
        printf("Failed to split large-R jet definition: %s\n",jetAlgoIn.Data());
        return "";
    }

    // Get the pure jet part
    TString jetDefPart = GetJetDesc(jetAlgoStrings.at(0)).at(0);
    if (jetDefPart == "")
        return "";
    // Overwrite the "+JES" as this is not applicable for large-R
    jetDefPart.ReplaceAll("+JES","");

    // Now get the substructure part
    TString jetSubPart = "";
    TString trimOrFilter = "";
    if (jetAlgoStrings.at(1).Contains("Trimmed"))
        trimOrFilter = "Trimmed";
    else if (jetAlgoStrings.at(1).Contains("Filtered"))
        trimOrFilter = "Filtered";

    // Get trimming info
    // TODO make this more general
    if (trimOrFilter == "Trimmed")
    {
        jetAlgoStrings.at(1).ReplaceAll("Trimmed","");
        TString pTfrac = "";
        if (jetAlgoStrings.at(1).BeginsWith("PtFrac"))
        {
            jetAlgoStrings.at(1).ReplaceAll("PtFrac","");
            const char* temp = jetAlgoStrings.at(1).Data();
            for (int index = 0; index < jetAlgoStrings.at(1).Length(); ++index)
            {
                if (TString(temp[index]).IsDigit())
                    pTfrac += TString(temp[index]);
                else
                    break;
            }
            if (pTfrac == "")
            {
                printf("Found PtFrac but not an associated value: %s\n",jetAlgoIn.Data());
                return "";
            }
            jetAlgoStrings.at(1).Replace(0,pTfrac.Length(),"");
        }

        TString smallR = "";
        if (jetAlgoStrings.at(1).BeginsWith("SmallR"))
        {
            jetAlgoStrings.at(1).ReplaceAll("SmallR","");
            const char* temp = jetAlgoStrings.at(1).Data();
            for (int index = 0; index < jetAlgoStrings.at(1).Length(); ++index)
            {
                if (TString(temp[index]).IsDigit())
                    smallR += TString(temp[index]);
                else
                    break;
            }
            if (smallR == "")
            {
                printf("Found SmallR but not an associated value: %s\n",jetAlgoIn.Data());
                return "";
            }
            jetAlgoStrings.at(1).Replace(0,smallR.Length(),"");
        }

        if (jetAlgoStrings.at(1) != "")
        {
            printf("Substructure information remains (%s): %s\n",jetAlgoStrings.at(1).Data(),jetAlgoIn.Data());
            return "";
        }

        jetSubPart = Form("f_{cut}=%.2f, R_{sub}=%.1f",float(jet::utils::getTypeObjFromString<int>(pTfrac))/100.,float(jet::utils::getTypeObjFromString<int>(smallR))/100.);
    }

    if (jetSubPart == "")
    {
        printf("Failed to parse substructure information: %s\n",jetAlgoIn.Data());
        return "";
    }

    return Form("#splitline{%s}{%s}",jetDefPart.Data(),jetSubPart.Data());
}

void DrawJetLabel(const JetUncertaintiesTool* provider, const double yPos)
{
    if (IsLargeR())
        DrawText(GetLargeJetDesc(provider->getJetDef().c_str()),kBlack,yPos);
    else
        //DrawText(GetJetDesc(provider->getJetDef().c_str())+" + #it{in situ} correction",kBlack,yPos);
        DrawText(GetJetDesc(provider->getJetDef().c_str()).at(0)+" + #it{in situ}, "+GetJetDesc(provider->getJetDef().c_str()).at(1),kBlack,yPos);
}

void DrawYearLabel(const JetUncertaintiesTool* provider, const double yPos)
{
    const TString config = provider->getConfigFile().c_str();
    
    TString type = "";
    TString sqrtS = "";
    if (config.Contains("_2011/"))
    {
        type = "Data 2011";
        sqrtS = "7 TeV";
    }
    else if (config.Contains("_2012/"))
    {
        type = "Data 2012";
        sqrtS = "8 TeV";
    }
    else if (config.Contains("_2015/"))
    {
        type = "Prerec 2015";
        sqrtS = "13 TeV";
    }
    if (type == "" || sqrtS == "")
    {
        printf("Could not parse year information from config: %s\n",config.Data());
        exit(-1);
    }
   //DrawText(Form("%s, #sqrt{#it{s}} = %s",type.Data(),sqrtS.Data()),kBlack,yPos);
   const TString bunchspacing = GetBunchSpacing();
   if (bunchspacing != "") {
     DrawText(Form("#sqrt{#it{s}} = %s, %s ns",sqrtS.Data(),bunchspacing.Data()),kBlack,yPos);
   } else 
     DrawText(Form("#sqrt{#it{s}} = %s",sqrtS.Data()),kBlack,yPos);

}

void DrawScenarioLabel(const JetUncertaintiesTool* provider, const double yPos)
{
    const TString config = provider->getConfigFile().c_str();

    TString scenario = "Nominal";
    if (config.Contains("StrongerCorrelations"))
        scenario = "Stronger Correlations";
    else if (config.Contains("WeakerCorrelations"))
        scenario = "Weaker Correlations";
    else if (config.Contains("_3NP_"))
    {
        TString scenarioNum = "UNKNOWN";
        std::vector<TString> tokens = jet::utils::vectorize<TString>(config,"_./");
        for (size_t iToken = 0; iToken < tokens.size(); ++iToken)
            if (tokens.at(iToken).BeginsWith("Scenario"))
                scenarioNum = tokens.at(iToken).ReplaceAll("Scenario","");
                
        scenario = Form("Rep_{str.red}^{%s,JES}",scenarioNum.Data());
    }

    if (scenario != "Nominal")
        DrawText(scenario,kBlack,config.Contains("_3NP")?yPos-0.01:yPos);
}










double GetPunchthroughProb(const JetUncertaintiesTool* provider, const xAOD::Jet& jet)
{
    static TH1* PThisto = NULL;
    static TString jetType = "";

    if (PThisto == NULL || jetType == "" || jetType != provider->getJetDef())
    {
        if (PThisto)
        {
            delete PThisto;
            PThisto = NULL;
        }

        const TString config = provider->getConfigFile();
        TString filename;
        if (config.Contains("_2015")) filename = "PTprob.root";
        else filename = "PTprob_2012.root";

        jetType = provider->getJetDef();
        TString histName = "";
        if (jetType == "AntiKt4LCTopo")
            histName = "h_PT_pT_LCJES4_norm";
        else if (jetType == "AntiKt6LCTopo")
            histName = "h_PT_pT_LCJES6_norm";
        else if (jetType == "AntiKt4EMTopo")
            histName = "h_PT_pT_EMJES4_norm";
        else if (jetType == "AntiKt6EMTopo")
            histName = "h_PT_pT_EMJES6_norm";
        else
        {
            printf("Unrecognized jet type for PT fraction: %s\n",jetType.Data());
            exit(100);
        }
        TFile* PTfile = new TFile(filename,"READ");
        if (!PTfile || PTfile->IsZombie())
        {
            printf("Failed to open PT fraction file\n");
            exit(101);
        }
        PThisto = dynamic_cast<TH1*>(PTfile->Get(histName));
        if (!PThisto)
        {
            printf("Failed to retrieve PT fraction histo: %s\n",histName.Data());
            exit(102);
        }
        PThisto->SetDirectory(0);
    }

    return PThisto->Interpolate(jet.pt()/1.e3);
}

void setPileupShiftsForYear(const JetUncertaintiesTool* provider, xAOD::EventInfo* eInfo, const xAOD::Jet* jet = NULL)
{
    static SG::AuxElement::Accessor<float> mu("averageInteractionsPerCrossing");
    static SG::AuxElement::Accessor<float> NPV("NPV");

    float    sigmaMu  = 0;
    float    sigmaNPV = 0;

    const TString config = provider->getConfigFile();
    if (config.Contains("_2011/"))
    {
        // Dag, night of Febuary 4/5, 2013
        sigmaMu  = 3.0;
        sigmaNPV = 3.0;
    }
    else if (config.Contains("_2012/") or config.Contains("_2015/Prerec"))
    {
        // Craig Sawyer, Jan 22 2013
        sigmaMu  = 5.593*1.11;
        sigmaNPV = 3.672;
    }
    else
    {
        printf("Unexpected year for setPileupShiftsForYear in config: %s\n",config.Data());
        exit(-1);
    }
    
    mu(*eInfo)  = (jet?provider->getRefMu(*jet):provider->getRefMu())+sigmaMu;
    NPV(*eInfo) = (jet?provider->getRefNPV(*jet):provider->getRefNPV())+sigmaNPV;
}

double getQuadratureSumUncertainty(const JetUncertaintiesTool* provider, const std::vector<size_t>& compIndices,const xAOD::Jet& jet,const xAOD::EventInfo& eInfo,const size_t PTindex)
{
    double unc = 0;
    for (size_t iComp = 0; iComp < compIndices.size(); ++iComp)
    {
        const double factor = compIndices.at(iComp) != PTindex ? 1 : GetPunchthroughProb(provider,jet);
        unc += pow(factor*provider->getUncertainty(compIndices.at(iComp),jet,eInfo),2);
    }
    unc = sqrt(unc);
    return unc;
}

std::vector<size_t> getComponentIndicesFromName(const JetUncertaintiesTool* provider,const TString& compName)
{
    // Map of name to index won't work as we need to worry about wildcard component names
    // So build a vector
    std::vector<TString> provNames;
    for (size_t iComp = 0; iComp < provider->getNumComponents(); ++iComp)
        provNames.push_back(provider->getComponentName(iComp));

    // Get the name prefix, if it exists
    const TString namePrefix = GetNamePrefix();

    // Now parse component name to index/indices
    const std::vector<TString> subComponents = jet::utils::vectorize<TString>(compName,",");
    std::vector<size_t> indices;
    for (size_t iSubComp = 0; iSubComp < subComponents.size(); ++iSubComp)
    {
        // Check for wildcards
        const bool beginWild = subComponents.at(iSubComp).BeginsWith("#");
        const bool endWild   = subComponents.at(iSubComp).EndsWith("#");
        const bool midWild   = !beginWild && !endWild && subComponents.at(iSubComp).Contains("#");
        const bool ALLCOMP   = !subComponents.at(iSubComp).CompareTo("total",TString::kIgnoreCase) || !subComponents.at(iSubComp).CompareTo("#");
        const TString toFind = midWild ? subComponents.at(iSubComp) : TString(subComponents.at(iSubComp)).ReplaceAll("#","");

        // Treat the more difficult mid-wild case
        std::vector<TString> tokensToFind;
        if (midWild) tokensToFind = jet::utils::vectorize<TString>(toFind,"#");
        if (midWild && tokensToFind.size() != 2)
        {
            printf("Only one middle-wildcard is currently supported\n");
            printf("Cannot handle string: %s\n",toFind.Data());
            exit(-1);
        }

        // Find the component index
        bool foundIndex = false;
        for (size_t iProvComp = 0; iProvComp < provNames.size(); ++iProvComp)
        {
            // Skip components which are not 4-vec scales or pt-scales
            if (!provider->getComponentScalesFourVec(iProvComp) && !provider->getComponentScalesPt(iProvComp))
                continue;
            
            if (ALLCOMP)
            {
                foundIndex = true;
                indices.push_back(iProvComp);
            }
            else if (midWild)
            {
                // Check if the string starts with the first token and ends with the last token
                // We already ensured there are only two tokens
                if ( ( provNames.at(iProvComp).BeginsWith(tokensToFind.at(0),TString::kIgnoreCase) || (namePrefix != "" && provNames.at(iProvComp).BeginsWith(namePrefix+tokensToFind.at(0),TString::kIgnoreCase)) ) && provNames.at(iProvComp).EndsWith(tokensToFind.at(1),TString::kIgnoreCase))
                {
                    foundIndex = true;
                    indices.push_back(iProvComp);
                    break;
                }
            }
            else if (!beginWild && !endWild)
            {
                if (!provNames.at(iProvComp).CompareTo(toFind,TString::kIgnoreCase) || ( namePrefix != "" && !provNames.at(iProvComp).CompareTo(namePrefix+toFind,TString::kIgnoreCase) ) )
                {
                    foundIndex = true;
                    indices.push_back(iProvComp);
                    break;
                }
            }
            else if (beginWild && !endWild)
            {
                if (provNames.at(iProvComp).EndsWith(toFind,TString::kIgnoreCase))
                {
                    foundIndex = true;
                    indices.push_back(iProvComp);
                }
            }
            else if (!beginWild && endWild)
            {
                if (provNames.at(iProvComp).BeginsWith(toFind,TString::kIgnoreCase) || ( namePrefix != "" && provNames.at(iProvComp).BeginsWith(namePrefix+toFind,TString::kIgnoreCase) ) )
                {
                    foundIndex = true;
                    indices.push_back(iProvComp);
                }
            }
            else
            {
                if (provNames.at(iProvComp).Contains(toFind,TString::kIgnoreCase))
                {
                    foundIndex = true;
                    indices.push_back(iProvComp);
                }
            }
        }
        if (!foundIndex)
            printf("WARNING: Failed to find match for sub/component: %s\n",toFind.Data());
    }

    if (!indices.size())
    {
        printf("Failed to find any indices for component: %s\n",compName.Data());
        exit(-1);
    }

    return indices;
}

std::vector< std::vector<size_t> > getComponentIndicesFromNames(const JetUncertaintiesTool* provider,const std::vector<TString>& compNames)
{
    std::vector< std::vector<size_t> > indices;
    
    // Now parse all of the names to provider indices
    for (size_t iComp = 0; iComp < compNames.size(); ++iComp)
        indices.push_back(getComponentIndicesFromName(provider,compNames.at(iComp)));

    return indices;
}



void MakeUncertaintyPlots(const TString& outFile,TCanvas* canvas,const std::vector<JetUncertaintiesTool*>& providers,const std::vector< std::vector< std::vector<size_t> > >& compSetIndices,const std::vector< std::vector<TString> >& labelNames,TH1D* frame,const double fixedValue,const std::vector<double>& scanBins,const bool fixedIsEta, const float mOverPt, const bool doComparison, const bool doCompareOnly)
{
    static SG::AuxElement::Accessor<int> Nsegments("GhostMuonSegmentCount");
    static SG::AuxElement::Accessor<bool> IsBjet("IsBjet");
    
    // Make TGraphs per provider per component
    // Also make total TGraph and TH1D per provider
    std::vector<TH1D*> totalHists;
    std::vector<TGraphErrors*> totalGraphs;
    std::vector< std::vector<TGraphErrors*> > compGraphs;
        
    // Build a jet container and a jet for us to manipulate later
    xAOD::TEvent event;
    xAOD::TStore store;
    xAOD::JetContainer* jets = new xAOD::JetContainer();
    jets->setStore(new xAOD::JetAuxContainer());
    jets->push_back(new xAOD::Jet());
    xAOD::Jet* jet = jets->at(0);

    // Add Nsegments information
    // 25 segments is about average for jets receiving a correction
    // This is modulated by the probability of punchthrough
    Nsegments(*jet) = IgnorePT()?0:25;
    IsBjet(*jet) = false;
    
    // Build an EventInfo object for us to manipulate later
    xAOD::EventInfoContainer* eInfos = new xAOD::EventInfoContainer();
    eInfos->setStore(new xAOD::EventInfoAuxContainer());
    eInfos->push_back(new xAOD::EventInfo());
    xAOD::EventInfo* eInfo = eInfos->at(0);

    // Get the provider
    for (size_t iProv = 0; iProv < providers.size(); ++iProv)
    {
        const JetUncertaintiesTool* provider = providers.at(iProv);
        
        // Set the pileup values for the year (so long as this isn't the special jet-dependent case)
        if (!IsEtaDepPileup())
            setPileupShiftsForYear(provider,eInfo);
        // Now catch the special case, where we want to set with respect to the central region for all jets
        // (specifically early 2015 data where eta-intercalibration done with mu=0 data for forward region)
        else
        {
            jet->setJetP4(xAOD::JetFourMom_t(100.e3,0.,0.,0.));
            setPileupShiftsForYear(provider,eInfo,jet);
        }
        
        // One totalHist per provider
        totalHists.push_back(new TH1D(Form("Total_%zu_hist",iProv),"",scanBins.size()-1,&scanBins[0]));
        TH1D* totalHist = totalHists.back();
        totalHist->SetFillColor(590);
        totalHist->SetLineWidth(0);
        
        // One totalGraph per provider
        totalGraphs.push_back(new TGraphErrors());
        TGraphErrors* totalGraph = totalGraphs.back();
        totalGraph->SetName(Form("Total_%zu_graph",iProv));
        totalGraph->SetLineStyle(1);
        totalGraph->SetLineWidth(2);
        totalGraph->SetLineColor(kBlack);

        // One list of all indices per provider
        const std::vector<size_t> allIndices = getComponentIndicesFromName(provider,"total");

        // Find the punch-through index, if applicable
        size_t PTindex = allIndices.size();
        for (size_t iComp = 0; iComp < provider->getNumComponents(); ++iComp)
            if (TString(provider->getComponentName(iComp).c_str()).Contains("PunchThrough",TString::kIgnoreCase))
            {
                PTindex = iComp;
                break;
            }

        // One vector of comp graphs per provider
        std::vector<TGraphErrors*> provCompGraphs;

        // Now get the components
        for (size_t iComp = 0; iComp < compSetIndices.at(iProv).size(); ++iComp)
        {
            // One graph per component per provider
            provCompGraphs.push_back(new TGraphErrors());
            TGraphErrors* compGraph = provCompGraphs.back();
            compGraph->SetName(labelNames.at(iProv).at(iComp));
            compGraph->SetLineStyle(iComp+1);
            compGraph->SetLineColor(iComp+2 < 5 ? iComp+2 : iComp+3);
            if (iComp+3 == 10) compGraph->SetLineColor(kYellow-9);
            compGraph->SetLineWidth(4);
            if (IsPublicFormat())
                applyPublicFormat(*compGraph);
        }

        // Now run over the scan grid
        for (int iScan = 1; iScan < frame->GetNbinsX()+2; ++iScan)
        {
            const double binValue = iScan == 1 ? frame->GetBinLowEdge(iScan)+1.e-3 : iScan == frame->GetNbinsX()+1 ? frame->GetBinLowEdge(iScan)-1.e-3 : frame->GetBinLowEdge(iScan);

            // Build the jet to work with
            const double pt   = (fixedIsEta ? binValue : fixedValue)*1.e3;
            const double eta  = (fixedIsEta ? fixedValue : binValue);
            const double mass = mOverPt*pt;
            jet->setJetP4(xAOD::JetFourMom_t(pt,eta,0,mass));


            // Fill components
            for (size_t iComp = 0; iComp < compSetIndices.at(iProv).size(); ++iComp)
            {

                // Get the uncertainty for the component, quad-sum of sub-components if applicable
                TGraphErrors* compGraph = provCompGraphs.at(iComp);
                const double compUnc = getQuadratureSumUncertainty(provider,compSetIndices.at(iProv).at(iComp),*jet,*eInfo,PTindex);
                compGraph->SetPoint(iScan-1,binValue,compUnc);
            }
              
            // Fill the total uncertainty graph
            const double totalUnc = getQuadratureSumUncertainty(provider,allIndices,*jet,*eInfo,PTindex);
            totalGraph->SetPoint(iScan-1,binValue,totalUnc);

        }

        // Add the vector of comp graphs to the total vector
        compGraphs.push_back(provCompGraphs);

        // Now build the total uncertainty histo (bin centers instead of edges)
        for (int iScan = 1; iScan < frame->GetNbinsX()+1; ++iScan)
        {
            const double binValue = frame->GetBinCenter(iScan);

            // Build the jet to work with
            const double pt   = (fixedIsEta ? binValue : fixedValue)*1.e3;
            const double eta  = (fixedIsEta ? fixedValue : binValue);
            const double mass = mOverPt*pt;
            jet->setJetP4(xAOD::JetFourMom_t(pt,eta,0,mass));

            totalHist->SetBinContent(iScan,getQuadratureSumUncertainty(provider,allIndices,*jet,*eInfo,PTindex));
        }
    }
    
    // Clean up the jet and EventInfo
    //delete eInfo;
    //eInfos->clear();
    delete eInfos;
    //delete jet;
    //jets->clear();
    delete jets;

    // Convert total graph to finer binning if this is an eta scan
    if (!fixedIsEta)
    {
        for (size_t iProv = 0; iProv < providers.size(); ++iProv)
        {
            TH1D* totalHist = totalHists.at(iProv);

            std::vector<double> rebinnedBins = jet::utils::getUniformBins(720,-4.5,4.5);
            TH1D* rebinnedTotal = new TH1D(Form("Rebinned_%s",totalHist->GetName()),"",rebinnedBins.size()-1,&rebinnedBins[0]);
            rebinnedTotal->SetFillColor(590);
            rebinnedTotal->SetLineWidth(0);
            for(int iBin = 1; iBin < rebinnedTotal->GetNbinsX()+1; ++iBin)
                rebinnedTotal->SetBinContent(iBin,totalHist->Interpolate(rebinnedTotal->GetBinCenter(iBin)));
            delete totalHists[iProv];
            totalHists[iProv] = rebinnedTotal;
        }
    }

    // Make the plot
    canvas->cd();
    canvas->Clear();
    if (fixedIsEta) canvas->SetLogx(true);
    else            canvas->SetLogx(false);
    frame->Draw("");
    
    if      (IsLargeR()) frame->GetXaxis()->SetRangeUser(200,2.e3);
    else if (fixedIsEta) frame->GetXaxis()->SetRangeUser(17,2.5e3);
    else                 frame->GetXaxis()->SetRangeUser(-4.2,4.2);
    
    if (IsLargeR()) frame->GetYaxis()->SetRangeUser(0,0.2);
    else
    {
        maxYuser = 0.1;//!(IsJER()&&((fixedIsEta&&fixedValue>2.5)||(!fixedIsEta)))?0.1:0.25;
        frame->GetYaxis()->SetRangeUser(0,maxYuser);
    }
  
    // If we want a comparison, that config was stored as last
    // item in the providers. Pop it off and store it.
    TH1D* comparisonHist = 0;
    TGraphErrors * comparisonGraph = 0;
    if (doComparison) {
        comparisonHist = totalHists.at(providers.size()-1);
        totalHists.pop_back();
        comparisonGraph = totalGraphs.at(providers.size()-1);
        totalGraphs.pop_back();
    }
    TH1D * backHist = 0; TH1D * frontHist = 0;
    TGraphErrors * backGraph = 0; TGraphErrors * frontGraph = 0;

    if (doCompareOnly) {
        for (size_t index=0; index < providers.size(); index++) {
          totalGraphs.at(index)->SetLineColor(1000+index);
          totalGraphs.at(index)->SetLineStyle(1);
          totalGraphs.at(index)->SetLineWidth(2);
          totalHists.at(index)->SetLineColor(1000+index);
          totalHists.at(index)->SetLineStyle(1);
          totalHists.at(index)->SetLineWidth(2);
          totalGraphs.at(index)->Draw("c same");
        } 
    } else if (providers.size() == 1 && DrawTotal())
        totalHists.at(0)->Draw("histF same");
    else if ((providers.size() == 2) && doComparison) {
      if (comparisonHist->GetMaximum() > totalHists.at(0)->GetMaximum()) {
        backHist = comparisonHist;
        frontHist = totalHists.at(0);
        backGraph = comparisonGraph;
        frontGraph = totalGraphs.at(0);
      } else {
        backHist = totalHists.at(0);
        frontHist = comparisonHist;
        backGraph = totalGraphs.at(0);
        frontGraph = comparisonGraph;
      }
      backHist->SetFillColor(kTeal-9);
      backHist->SetLineColor(kTeal+5);
      backHist->Draw("histF same");
      frontHist->Draw("histF same");

    }

    if (!doCompareOnly) {
      for (size_t iProv = 0; iProv < providers.size(); ++iProv)
      {
         if (doComparison) {
             if (iProv == providers.size()-1) continue;
             backGraph->SetLineColor(kTeal+5);
             backGraph->Draw("c same");
             frontGraph->Draw("c same");
         } else if (DrawTotal()) totalGraphs.at(iProv)->Draw("c same");
        for (size_t iComp = 0; iComp < compGraphs.at(iProv).size(); ++iComp)
            compGraphs.at(iProv).at(iComp)->Draw("l same");
      }
    }

    // Add labels
    if (DoATLASLabel()) DrawAtlasLabel();
 
    bool sign = true; 
    for (size_t i=0; i<providers.size(); i++) {
      const TString thisconfname = providers.at(i)->getConfigFile().c_str();
      if (!(thisconfname.Contains("_2012/"))) sign = false;
    }
    if (IsLargeR())
    {
       DrawJetLabel(providers.at(0),0.89);
       if (providers.size() == 1 || (providers.size()==2 && doComparison) || sign)
            DrawYearLabel(providers.at(0),0.81);
        DrawText(Form("|#eta| = %.1f, M/#it{p}_{T}^{jet} = %.1f",fixedValue,mOverPt),kBlack,0.76);
    }
    else
    {
/*        DrawJetLabel(providers.at(0),0.905);
        if (providers.size() == 1 || (providers.size()==2 && doComparison))
            DrawYearLabel(providers.at(0),0.860);
        DrawText(fixedIsEta?Form("#eta = %.1f",fixedValue):Form("#it{p}_{T}^{jet} = %.0f GeV",fixedValue),kBlack,0.815);
        DrawScenarioLabel(providers.at(0),0.77); */
        
        
        DrawJetLabel(providers.at(0),0.855);//0.905);
        if (providers.size() == 1 || (providers.size()==2 && doComparison) || sign)
            DrawYearLabel(providers.at(0),0.910);//0.860);
        DrawText(fixedIsEta?Form("#eta = %.1f",fixedValue):Form("#it{p}_{T}^{jet} = %.0f GeV",fixedValue),kBlack,0.81);
        DrawScenarioLabel(providers.at(0),0.77); // 0.77

    }

    // Add legend
    double legx=0.41, legy=0.78, dy = 0.045; //dy=0.070;
    if (doCompareOnly) {
       for (size_t i=0; i < totalGraphs.size(); i++) {
          const TString title = labelNames.at(i).at(0);
          DrawLineLabel(title,legx,legy-=dy,totalHists.at(i));
       }
    }
    if ((providers.size() == 1 || (providers.size() == 2 && doComparison)) && DrawTotal()) {
        if (fixedIsEta) DrawFillLabel(TotalUncName(),legx,legy,totalHists.at(0));
        else DrawFillLabel(TotalUncName(),legx,legy,totalHists.at(0),false);
    }
    if (doComparison) {
        const TString title = labelNames.at(providers.size()-1).at(0);
        if (fixedIsEta) DrawFillLabel(title,legx,legy-=dy,comparisonHist);
        else DrawFillLabel(title,legx,legy-=dy,comparisonHist, false);
    }
    for (size_t iProv = 0; iProv < providers.size(); ++iProv)
        for (size_t iComp = 0; iComp < compGraphs.at(iProv).size(); ++iComp)
        {
            const TGraphErrors* compGraph = compGraphs.at(iProv).at(iComp);
            const TString label = compGraph->GetName(); //labelNames.at(iProv).at(iComp);
            if (label.Contains("#splitline"))
            {
                legy -= 0.35*dy;
                DrawLineLabel(label,legx,legy-=dy,compGraph);
                legy -= 0.35*dy;
            }
            else
                DrawLineLabel(label,legx,legy-=dy,compGraph);
        }
   
    // Write the plot
    frame->Draw("axis same");
    if (!outFile.EndsWith(".eps") && !outFile.EndsWith(".png"))
        canvas->Print(outFile);
    else
        canvas->Print(TString(outFile).ReplaceAll(outFile.EndsWith(".eps")?".eps":".png",Form("/fig_%s_%s.%s",fixedIsEta?"eta":"pt",fixedIsEta?Form("%.1f",fixedValue):Form("%.0f",fixedValue),outFile.EndsWith(".eps")?"eps":"png")));

    // Free graphs/hists
    for (size_t iProv = 0; iProv < providers.size(); ++iProv)
    {
        if (doComparison && iProv == providers.size()-1) continue;
        else {
          delete totalHists.at(iProv);
          delete totalGraphs.at(iProv);
        }
        for (size_t iComp = 0; iComp < compGraphs.at(iProv).size(); ++iComp)
            delete compGraphs.at(iProv).at(iComp);
    }
    delete comparisonHist;
    delete comparisonGraph;

}

void MakeUncertaintyPlots(const TString& outFile,TCanvas* canvas,const std::vector<JetUncertaintiesTool*>& providers,const std::vector< std::vector<TString> >& compSetComponents,const std::vector< std::vector<TString> >& labelNames, const bool doComparison, const bool doCompareOnly)
{
    // Central values for scans
    const std::vector<double> ptValuesForEtaScan = jet::utils::vectorize<double>("25,40,60,80,120",",");
    const std::vector<double> etaValuesForPtScan = IsLargeR() ? jet::utils::vectorize<double>("0",",") : IsJER() ? jet::utils::vectorize<double>("0,0.8,1.2,2.1,2.8,3.2,3.6",",") : jet::utils::vectorize<double>("0,0.5,1,2,3,4",",");
    const std::vector<double> mOverPtValuesForPtScan = !IsLargeR() ? jet::utils::vectorize<double>("0",",") : jet::utils::vectorize<double>("0.2,0.4,0.6",",");

    // Scan ranges
    const std::vector<double> etaScanValues = jet::utils::getUniformBins(180,-4.5,4.5);
    const std::vector<double> ptScanValues  = !IsLargeR() ? jet::utils::getLogBins(200,15,2500) : jet::utils::getLogBins(200,200,2000);

    // Create the frames
    TH1D* frameEtaScan = new TH1D("frameEtaScan","",etaScanValues.size()-1,&etaScanValues[0]);
    TH1D* framePtScan  = new TH1D("framePtScan","",ptScanValues.size()-1,&ptScanValues[0]);
    frameEtaScan->GetXaxis()->SetTitleOffset(1.4);
    frameEtaScan->GetYaxis()->SetTitleOffset(!IsJER()?1.25:1.1);
    frameEtaScan->GetXaxis()->SetTitle("#eta");
    frameEtaScan->GetYaxis()->SetTitle(!IsJER()?"Fractional JES uncertainty":"Uncertainty on #sigma(#it{p}_{T})/#it{p}_{T}");
    framePtScan->GetXaxis()->SetTitleOffset(1.4);
    framePtScan->GetYaxis()->SetTitleOffset(!IsJER()?1.25:1.1);
    framePtScan->GetXaxis()->SetTitle("#it{p}_{T}^{jet} [GeV]");
    framePtScan->GetYaxis()->SetTitle(!IsJER()?"Fractional JES uncertainty":"Uncertainty on #sigma(#it{p}_{T})/#it{p}_{T}");
    framePtScan->GetXaxis()->SetMoreLogLabels();

    // Fill frame histograms with -1 so line is drawn outside of frame
    for (int iBin = 1; iBin <= frameEtaScan->GetNbinsX(); ++iBin)
        frameEtaScan->SetBinContent(iBin,-1);
    for (int iBin = 1; iBin <= framePtScan->GetNbinsX(); ++iBin)
        framePtScan->SetBinContent(iBin,-1);


    // Convert components to indices
    std::vector< std::vector< std::vector<size_t> > > compSetIndices;
    for (size_t iProv = 0; iProv < providers.size(); ++iProv)
        compSetIndices.push_back(getComponentIndicesFromNames(providers.at(iProv),compSetComponents.at(iProv)));

    // Create a plot per fixed value
    if (!IsLargeR())
    {
        for (size_t iFixed = 0; iFixed < etaValuesForPtScan.size(); ++iFixed)
            MakeUncertaintyPlots(outFile,canvas,providers,compSetIndices,labelNames,framePtScan,etaValuesForPtScan.at(iFixed),ptScanValues,true,mOverPtValuesForPtScan.at(0),doComparison, doCompareOnly);
        for (size_t iFixed = 0; iFixed < ptValuesForEtaScan.size(); ++iFixed)
            MakeUncertaintyPlots(outFile,canvas,providers,compSetIndices,labelNames,frameEtaScan,ptValuesForEtaScan.at(iFixed),etaScanValues,false,mOverPtValuesForPtScan.at(0),doComparison, doCompareOnly);
    }
    else
    {
        for (size_t iFixed = 0; iFixed < mOverPtValuesForPtScan.size(); ++iFixed)
            MakeUncertaintyPlots(outFile,canvas,providers,compSetIndices,labelNames,framePtScan,etaValuesForPtScan.at(0),ptScanValues,true,mOverPtValuesForPtScan.at(iFixed),doComparison, doCompareOnly);
    }


    delete framePtScan;
    delete frameEtaScan;
}


int main (int argc, char* argv[])
{
    if (argc != 7 && argc != 8)
    {
        printf("Expected arguments:\n");
        printf("\t1. Output file (ps, pdf, eps)\n");
        printf("\t2. Jet definition(s), semicolon delimited\n");
        printf("\t3. MC type(s), semicolon delimited (one for all configs, or one per config)\n");
        printf("\t4. Config file(s), semicolon delimited\n");
        printf("\t5. Component list, special formatting (one for all configs, or one per config):\n");
        printf("\t\tComma-delimited means combine components\n");
        printf("\t\tSemicolon-delimited means new component\n");
        printf("\t\tAmpersand-delimited means new config\n");
        printf("\t\tTOTAL to get the total uncertainty as one term\n");
        printf("\t\t# is wildcard, so InSitu# is everything starting with InSitu\n");
        printf("\t6. Component labels, special formatting (one for all configs, or one per config)\n");
        printf("\t\tSemicolon-delimited for each component label\n");
        printf("\t\tAmpersand-delimited for each config\n");
        printf("\t7. Options (optional argument), semi-colon delimited, examples:\n");
        printf("\t\tisDijet=false\n");
        printf("\t\tisLargeR=false\n");
        exit(1);
    }
    TString outFile = argv[1];
    std::vector<TString> jetDefs   = jet::utils::vectorize<TString>(argv[2],";");
    std::vector<TString> mcTypes   = jet::utils::vectorize<TString>(argv[3],";");
    std::vector<TString> configs   = jet::utils::vectorize<TString>(argv[4],";");
    std::vector<TString> compSets  = jet::utils::vectorize<TString>(argv[5],"@");
    std::vector<TString> labelSets = jet::utils::vectorize<TString>(argv[6],"@");
    if (argc == 8) globalOptions = jet::utils::vectorize<TString>(argv[7],";");
    
    StatusCode::enableFailure();
  
    bool doComparison = false;
    TString compareMCType = "";
    TString totalLabel = "";
    TString doCompConfig = DoCompare();
    if (doCompConfig != "") {
      doComparison = true;
      std::vector<TString> comparisons = jet::utils::vectorize<TString>(doCompConfig,"&");
      doCompConfig = comparisons.at(0);
      compareMCType = comparisons.at(1);
      totalLabel = comparisons.at(2);
      configs.push_back(doCompConfig);
      mcTypes.push_back(compareMCType);
      compSets.push_back("");
      labelSets.push_back(totalLabel);
    }

    bool doCompareOnly = CompareOnly();
    if (doCompareOnly) {
       // If we are doing only the comparison we don't want anything with individual components
       // just three big lines. So clear anything that is here now.
       configs.clear();
       mcTypes.clear();
       compSets.clear();
       labelSets.clear();
       std::vector<TString> compareconfigs = GetCompareVals();
       for (size_t item = 0; item < compareconfigs.size(); item++) {
         std::vector<TString> comparisons = jet::utils::vectorize<TString>(compareconfigs.at(item),"&");
         doCompConfig = comparisons.at(0);
         compareMCType = comparisons.at(1);
         totalLabel = comparisons.at(2);
         configs.push_back(doCompConfig);
         mcTypes.push_back(compareMCType);
         compSets.push_back("");
         labelSets.push_back(totalLabel);
       }

    }
    
    // One MC type for all configs or one MC type per config
    if (mcTypes.size() != 1 && mcTypes.size() != configs.size())
    {
        printf("Expected either 1 MC type for all configs, or 1 MC type per config\n");
        printf("Instead, got %zu MC types and %zu configs\n",mcTypes.size(),configs.size());
        exit(2);
    }
    // One MC type for all configs, then duplicate
    else if (mcTypes.size() == 1 && configs.size() != 1)
        for (size_t iConfig = 1; iConfig < configs.size(); ++iConfig)
            mcTypes.push_back(mcTypes.at(0));

    // Equal numbers of component lists and labels
    if (compSets.size() != labelSets.size())
    {
        printf("Expected matching numbers of component lists and labels\n");
        printf("Instead, got %zu component lists and %zu labels\n",compSets.size(),labelSets.size());
        exit(4);
    }
    
    // One component list for all configs or one component list per config
    if (compSets.size() != 1 && compSets.size() != configs.size())
    {
        printf("Expected either 1 component list for all configs, or 1 component list per config\n");
        printf("Instead, got %zu component lists and %zu configs\n",compSets.size(),configs.size());
        exit(3);
    }
    // One component list for all configs, then duplicate
    else if (compSets.size() == 1 && configs.size() != 1)
        for (size_t iConfig = 1; iConfig < configs.size(); ++iConfig)
        {
            compSets.push_back(compSets.at(0));
            labelSets.push_back(labelSets.at(0));
        }


    // Split the component sets and labels further
    std::vector< std::vector<TString> > compSetComponents;
    std::vector< std::vector<TString> > labelNames;
    for (size_t iSet = 0; iSet < compSets.size(); ++iSet)
    {
        compSetComponents.push_back(jet::utils::vectorize<TString>(compSets.at(iSet),";"));
        labelNames.push_back(jet::utils::vectorize<TString>(labelSets.at(iSet),";"));
    }

    
    // Create the canvas
    SetAtlasStyle();
    gStyle->SetPalette(1);
    TCanvas* canvas = new TCanvas("canvas");
    canvas->SetMargin(0.12,0.04,0.15,0.04);
    canvas->SetFillStyle(4000);
    canvas->SetFillColor(0);
    canvas->SetFrameBorderMode(0);
    canvas->cd();

    // If this is not an eps, start the output
    if (!outFile.EndsWith(".eps") && !outFile.EndsWith(".png"))
        canvas->Print(outFile+"[");
    // Otherwise, make a folder for the eps files
    else
        system(Form("mkdir -p %s",TString(outFile).ReplaceAll(outFile.EndsWith(".eps")?".eps":".png","").Data()));


    // Run once per jet type
    for (size_t iJetDef = 0; iJetDef < jetDefs.size(); ++iJetDef)
    {
        const TString jetDef = jetDefs.at(iJetDef);

        // Create the providers
        std::vector<JetUncertaintiesTool*> providers;
        for (size_t iConfig = 0; iConfig < configs.size(); ++iConfig)
        {
            // Make a new provider
            providers.push_back(new JetUncertaintiesTool(Form("%s_%zu",jetDef.Data(),iConfig)));
            
            //Set properties
            if (providers.back()->setProperty("JetDefinition",jetDef.Data()).isFailure())
            {
                printf("Failed to set JetDefinition to %s\n",jetDef.Data());
                exit(4);
            }
            if (providers.back()->setProperty("MCType",mcTypes.at(iConfig).Data()).isFailure())
            {
                printf("Failed to set MCType to %s\n",mcTypes.at(iConfig).Data());
                exit(5);
            }
            if (providers.back()->setProperty("ConfigFile",configs.at(iConfig).Data()).isFailure())
            {
                printf("Failed to set ConfigFile to %s\n",configs.at(iConfig).Data());
                exit(6);
            }

            // Check if we want to change topology from unknown to dijet
            if (IsDijet())
            {
                const TString dijetAnalysisFile = "../testingMacros/random_stuff/MJESForInclusiveJets_MC11b.root";
                if (providers.back()->setProperty("AnalysisFile",dijetAnalysisFile.Data()).isFailure())
                {
                    printf("Failed to set AnalysisFile to %s\n",dijetAnalysisFile.Data());
                    exit(7);
                }
            }

            // Done setting properties, initialize the tool
            if (providers.back()->initialize().isFailure())
            {
                printf("Failed to initialize tool for config: %s\n",configs.at(iConfig).Data());
                exit(8);
            }
        }

        // Make the plots
        MakeUncertaintyPlots(outFile,canvas,providers,compSetComponents,labelNames,doComparison,doCompareOnly);

        // Clean up
        for (size_t iProv = 0; iProv < providers.size(); ++iProv)
            delete providers.at(iProv);
        providers.clear();
    }

    // If this is not an eps, end the output
    if (!outFile.EndsWith(".eps") && !outFile.EndsWith(".png"))
        canvas->Print(outFile+"]");

    return 0;
}
