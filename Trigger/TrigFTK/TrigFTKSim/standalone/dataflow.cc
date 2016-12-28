/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "dataflow.h"
#include "boost/program_options.hpp"
#include "boost/filesystem.hpp"
#include <iomanip>
#include <TFile.h>

int SIGDIGITS=2;

int main(int argc, char **argv) {
  events = -1;
  ntower = MAXTOWER;

  try {
    std::string appName = boost::filesystem::basename(argv[0]);
    // Define and parse the program options
    namespace po = boost::program_options;
    po::options_description desc("Options");
    desc.add_options()
      ("help,h", "Print this help message")
      ("output,o", po::value<std::string>(&output)->default_value("./output.txt"), "The name of the output file")
      ("outputTeX,T", po::value<std::string>(&outputTeX)->default_value("./output.tex"), "The name of the output LaTeX file")
      ("outputRoot,R", po::value<std::string>(&outputRoot)->default_value("./output.root"), "The name of the output root file")
      ("description,d", po::value<std::string>(&description)->default_value(""), "Information to describe this dataflow")
      ("events,e", po::value<int>(&events)->default_value(-1), "The number of events to run over. Set to -1 to use all events")
      ("ntower,n", po::value<int>(&ntower)->default_value(MAXTOWER), "The number of towers")
      ("files", po::value<std::vector<std::string> >(&files)->multitoken(), "FTK NTUP files")
      ("sigdigits,s", po::value<int>(&SIGDIGITS)->default_value(2), "The number of significant digits");

    po::variables_map vm;
    try
    {
      po::store(po::command_line_parser(argc, argv).options(desc).run(), vm);
      // --help option
      if (vm.count("help"))
      {
        cout << desc << "\n";
        return 1;
      }
      po::notify(vm);
    }
    catch(std::exception& e)
    {
      std::cerr << "ERROR: " << e.what() << std::endl << std::endl;
      return 1;
    }

    Init();
    if(ch) {
       for (unsigned int ientry = 0; ientry < nloop; ++ientry) {
          Process(ientry);
       }
    } else {
       // loop over indifidual files (towers spread over files
       vector<unsigned> processed(ntower);
       for (size_t ifile=0; ifile < files.size(); ++ifile) {
          unsigned requiredEvents=0;
          for(int itower=0;itower<ntower;itower++) {
             activeTower[itower]=false;
             if(towersPerFile[ifile].first &&
                towersPerFile[ifile].second[itower] &&
                processed[itower]<nloop) {
                activeTower[itower]=true;
                unsigned n=nloop- processed[itower];
                if(n>towersPerFile[ifile].first) n=towersPerFile[ifile].first;
                if(n>requiredEvents) requiredEvents=n;
             }
          }
          if(requiredEvents>0) {
             ch=new TChain("ftkdata");
             ch->Add(files[ifile].c_str());
             for(int itower=0;itower<ntower;itower++) {
                if(towersPerFile[ifile].second[itower] &&
                   processed[itower]<nloop) {
                   ch->SetBranchAddress
                      (Form("FTKTracksStream%d.",itower),&trackstream[itower]);
                   ch->SetBranchAddress
                      (Form("FTKRoadsStream%d.",itower),&stream[itower]);
                }
             }
             for(unsigned int ientry = 0; ientry < requiredEvents; ++ientry) {
                Process(ientry);
                for(int itower=0;itower<ntower;itower++) {
                   if(towersPerFile[ifile].second[itower]) {
                      ++processed[itower];
                      // disable this tower
                      if(processed[itower]>=nloop) {
                         activeTower[itower]=false;
                      }
                   }
                }
             }
             delete ch;
             ch=0;
          }
       }
    }

    Terminate();

  } // end try
  catch(std::exception& e)
  {
    std::cerr << "Unhandled Exception reached the top of main: "
              << e.what() << ", application will now exit" << std::endl;
    return 1;
  }
  return 0;
}

void Init() {


   outputRootFile=new TFile(outputRoot.c_str(),"RECREATE");
   outputHistAvg=new TH2D("hist_dataflow_avg",";tower;entries",
                           MAXTOWER,-0.5,MAXTOWER-0.5,
                           HIST_MAXBINS,-0.5,HIST_MAXBINS-0.5);
   outputHistAvg->Sumw2();
   outputHistMin=new TH2D("hist_dataflow_min",";tower;entries",
                           MAXTOWER,-0.5,MAXTOWER-0.5,
                           HIST_MAXBINS,-0.5,HIST_MAXBINS-0.5);
   outputHistMin->Sumw2();
   outputHistMax=new TH2D("hist_dataflow_max",";tower;entries",
                          MAXTOWER,-0.5,MAXTOWER-0.5,
                          HIST_MAXBINS,-0.5,HIST_MAXBINS-0.5);
   outputHistMax->Sumw2();

  myfile.open (output);
  myfileTeX.open (outputTeX);
  std::cout.precision (1) ;

  myfileTeX << "\\documentclass[12pt]{article}" << std::endl;
  myfileTeX << "\\usepackage[margin=1in]{geometry}" << std::endl;
  myfileTeX << "\\usepackage{graphicx}" << std::endl;
  myfileTeX << "\\usepackage{multirow}" << std::endl;
  myfileTeX << "\\usepackage{multicol}" << std::endl;
  myfileTeX << "\\begin{document}" << std::endl;
  myfileTeX << "\\begin{table}" << std::endl;
  myfileTeX << "\\centering" << std::endl;
  myfileTeX << "\\begin{tabular}{|c|c|c|c|c|}" << std::endl;
  myfileTeX << "\\hline" << std::endl;
  myfileTeX << "& \\multicolumn{2}{|c|}{Barrel} & \\multicolumn{2}{|c|}{Endcap} \\\\" << std::endl;
  myfileTeX << "\\hline" << std::endl;
  myfileTeX << "& Average & Max Tower & Average & Max Tower\\\\ " << std::endl;
  myfileTeX << "\\hline" << std::endl;

  myfile << description << endl;

  for (int i=0; i < MAXTOWER; i++) {
    stream[i] = 0;
    trackstream[i] = 0;
    nRoad[i] = 0;
    nFit[i] = 0;
    nFitRecovery[i] = 0;
    nFitRecoveryI[i] = 0;
    nFitMajority[i] = 0;
    nConn[i] = 0;
    nExtrapAUX[i]=0;
    nFitMajorityI[i] = 0;
    nFitMajorityPix[i] = 0;
    nFitMajorityPixI[i] = 0;
    nFitMajoritySCT[i] = 0;
    nFitMajoritySCTI[i] = 0;
    nTrack[i] = 0;
    nFitI[i] = 0;
    nTrackI[i] = 0;
    nTrackBeforeHW[i] = 0;
    for (int k=0; k<8; k++) { nCluster[k][i]=0; nSSID[k][i] = 0;}
  }
  
  ch = new TChain(TTREE_NAME);

  // add the input files
  std::vector<std::string>::const_iterator files_it = files.begin();
  for (; files_it != files.end(); ++files_it) {
    ch->Add((*files_it).c_str());
  }
  nloop = ch->GetEntries();

  if (events > 0) {
    nloop = (unsigned int) events;
  }
  if (events > ch->GetEntries()) {
    nloop = ch->GetEntries();
  }

  int error=0;
  activeTower.resize(ntower);
  for (int itower = 0; itower < ntower; itower++) {
     if((ch->SetBranchAddress
         (Form("FTKMergedTracksStream%d",itower),&trackstream[itower])!=0)||
        (ch->SetBranchAddress
         (Form("FTKMergedRoadsStream%d",itower),&stream[itower])!=0)) {
        // not all towers present
        // enable loop over non-merged files
        error++;
     } else {
        activeTower[itower]=true;
     }

  }
  if(error) {
     delete ch;
     ch = 0;
     towersPerFile.resize(files.size());
     vector<size_t> neventPerTower(ntower);
     for (size_t ifile=0; ifile < files.size(); ++ifile) {
        TFile *file=new TFile((files[ifile]).c_str());
        if(!file) {
           cerr<<"WARNING: skipping file "<<files[ifile]<<" TFile() failed\n";
           continue;
        }
        TTree *tree;
        file->GetObject(TTREE_NAME,tree);
        if(!tree) {
           cerr<<"WARNING: skipping file "<<files[ifile]<<" TTree "<<TTREE_NAME
               <<" not found\n";
           continue;
        }
        towersPerFile[ifile].first=tree->GetEntries();
        towersPerFile[ifile].second.resize(ntower);
        for(int itower=0;itower<ntower; itower++) {
           towersPerFile[ifile].second[itower]=
              (tree->SetBranchAddress
               (Form("FTKTracksStream%d.",itower),&trackstream[itower])==0) &&
              (tree->SetBranchAddress
            (Form("FTKRoadsStream%d.",itower),&stream[itower])==0);
           if(towersPerFile[ifile].second[itower]) {
              neventPerTower[itower]+= towersPerFile[ifile].first;
           }
        }
        delete file;
     }
     for (size_t ifile=0; ifile < files.size(); ++ifile) {
        cout<<"file: "<<files[ifile]<<" events: "<<towersPerFile[ifile].first
            <<" towers: [";
        for(int itower=0;itower<ntower; itower++) {
           if(towersPerFile[ifile].second[itower]) cout<<" "<<itower;
        }
        cout<<"]\n";
     }
     // determine number of events to process (from "smallest" tower)
     nloop=0;
     for(int itower=0;itower<ntower; itower++) {
        if((nloop==0)||
           ((neventPerTower[itower]>0)&&(nloop>neventPerTower[itower]))) {
           nloop=neventPerTower[itower];
        }
     }
     cout<<"nloop="<<nloop<<"\n";
  }
  divide = 1./nloop;

}

void Process(unsigned int ientry) {
  
  ch->GetEntry(ientry);
  Int_t thisrun=-1,thisevt=-1;
  for (int itower = 0; itower < ntower; itower++) {
     if(!activeTower[itower]) continue;
     thisrun = stream[itower]->runNumber();
     thisevt = stream[itower]->eventNumber();
     break;
  }
  if (ientry % 50 == 0) cerr << "ientry = " << ientry << " and run = " << thisrun << " and event = " << thisevt << endl;

  static int first=true;

#define ADD_TO_HIST(x,name)                                             \
  outputHistAvg->Fill((double)itower,(double)ibin,(double)x); ++ibin;   \
  if(x>outputHistMax->GetBinContent(itower+1,ibin))                     \
    outputHistMax->SetBinContent(itower+1,ibin,x);                      \
  if((outputHistMin->GetBinContent(itower+1,ibin)<=0.)||                \
     (outputHistMin->GetBinContent(itower+1,ibin)>x))                   \
    outputHistMin->SetBinContent(itower+1,ibin,x);                      \
  if(first) { outputHistAvg->GetYaxis()->SetBinLabel(ibin,name);        \
              outputHistMin->GetYaxis()->SetBinLabel(ibin,name);        \
              outputHistMax->GetYaxis()->SetBinLabel(ibin,name); }

  for (int itower = 0; itower < ntower; itower++) {
     if(!activeTower[itower]) continue;
    if (trackstream[itower] == 0) { cerr << "ERROR: Track stream " << itower << " = 0 " << std::endl; return;}
    if (stream[itower] == 0) { cerr << "ERROR: Road stream " << itower << " = 0 " << std::endl; return;}

    int ibin=0;
    ADD_TO_HIST(1,"nevent");
    ADD_TO_HIST( stream[itower]->naoGetNroadsAM(),"nRoad");
    ADD_TO_HIST( trackstream[itower]->getNFits(),"nFit");
    ADD_TO_HIST( trackstream[itower]->getNFitsI(),"nFitI");
    ADD_TO_HIST( trackstream[itower]->getNTracks(),"nTrack");
    ADD_TO_HIST( trackstream[itower]->getNTracks(),"nTrackI");
    
      nRoad[itower] += stream[itower]->naoGetNroadsAM()*divide;
      nFit[itower] += trackstream[itower]->getNFits()*divide;
      nFitI[itower] += trackstream[itower]->getNFitsI()*divide;
      nTrack[itower] += trackstream[itower]->getNTracks()*divide;
      nTrackI[itower] += trackstream[itower]->getNTracksI()*divide;

      Int_t ntrackBeforeHW = trackstream[itower]->getNTracks() + trackstream[itower]->getNFitsHWRejected();

   ADD_TO_HIST(ntrackBeforeHW,"nTrackBeforeHW");

      nTrackBeforeHW[itower] += ntrackBeforeHW*divide;

      nConn[itower] += trackstream[itower]->getNConn()*divide;
      nExtrapAUX[itower] += trackstream[itower]->getNExtrapolatedTracks()*divide;
   ADD_TO_HIST( trackstream[itower]->getNConn(),"NConn");
   ADD_TO_HIST( trackstream[itower]->getNExtrapolatedTracks(),"NExtrapAUX");

      nFitRecoveryI[itower] += trackstream[itower]->getNFitsRecoveryI()*divide;
      nFitRecovery[itower] += trackstream[itower]->getNFitsRecovery()*divide;

   ADD_TO_HIST( trackstream[itower]->getNFitsRecoveryI(),"nFitRecoveryI");
   ADD_TO_HIST( trackstream[itower]->getNFitsRecovery(),"nFitRecovery");

      nFitMajorityI[itower] += trackstream[itower]->getNFitsMajorityI()*divide;
      nFitMajority[itower] += trackstream[itower]->getNFitsMajority()*divide;

   ADD_TO_HIST( trackstream[itower]->getNFitsMajorityI(),"nFitMajorityI");
   ADD_TO_HIST( trackstream[itower]->getNFitsMajority(),"nFitMajority");

      nFitMajoritySCTI[itower] += trackstream[itower]->getNFitsMajorityI_SCT()*divide;
      nFitMajoritySCT[itower] += trackstream[itower]->getNFitsMajority_SCT()*divide;

   ADD_TO_HIST( trackstream[itower]->getNFitsMajorityI_SCT(),"nFitMajoritySCTI");
   ADD_TO_HIST( trackstream[itower]->getNFitsMajority_SCT(),"nFitMajoritySCT");

      nFitMajorityPixI[itower] += trackstream[itower]->getNFitsMajorityI_pix()*divide;
      nFitMajorityPix[itower] += trackstream[itower]->getNFitsMajority_pix()*divide;

   ADD_TO_HIST( trackstream[itower]->getNFitsMajorityI_pix(),"nFitMajorityPixI");
   ADD_TO_HIST( trackstream[itower]->getNFitsMajority_pix(),"nFitMajorityPix");

      for (int il = 0; il < 8; il++) {
         nCluster[il][itower] += stream[itower]->naoGetNclus(il)*divide;
         nSSID[il][itower] += stream[itower]->naoGetNss(il)*divide;
      }
   ADD_TO_HIST( stream[itower]->naoGetNclus(0),"nCluster0");
   ADD_TO_HIST( stream[itower]->naoGetNclus(1),"nCluster1");
   ADD_TO_HIST( stream[itower]->naoGetNclus(2),"nCluster2");
   ADD_TO_HIST( stream[itower]->naoGetNclus(3),"nCluster3");
   ADD_TO_HIST( stream[itower]->naoGetNclus(4),"nCluster4");
   ADD_TO_HIST( stream[itower]->naoGetNclus(5),"nCluster5");
   ADD_TO_HIST( stream[itower]->naoGetNclus(6),"nCluster6");
   ADD_TO_HIST( stream[itower]->naoGetNclus(7),"nCluster7");

   ADD_TO_HIST( stream[itower]->naoGetNss(0),"nSSID0");
   ADD_TO_HIST( stream[itower]->naoGetNss(1),"nSSID1");
   ADD_TO_HIST( stream[itower]->naoGetNss(2),"nSSID2");
   ADD_TO_HIST( stream[itower]->naoGetNss(3),"nSSID3");
   ADD_TO_HIST( stream[itower]->naoGetNss(4),"nSSID4");
   ADD_TO_HIST( stream[itower]->naoGetNss(5),"nSSID5");
   ADD_TO_HIST( stream[itower]->naoGetNss(6),"nSSID6");
   ADD_TO_HIST( stream[itower]->naoGetNss(7),"nSSID7");

  }
  first=false;
}

void Terminate() {

  myfile << "Type\t\tbarrelmean\t\tbarrelmax\t\tendcapmean\t\tendcapmax" << endl;
  myfile << "--------------" << endl;

  // kludge, can do this better but works for now
  float temp[MAXTOWER], temp2[MAXTOWER];
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < MAXTOWER; j++) {
      temp[j] = nCluster[i][j];
      temp2[j] = nSSID[i][j];
    }
    printinfo (temp, Form("NClusterL%d",i));
    printinfo (temp2, Form("NSSIDL%d",i));
  }
  AddBreak();
  printinfo (nRoad, "NRoads");
  AddBreak();
  printinfo (nFitI, "NFitAux (8/8 + 7/8)");
  printinfo (nFitRecoveryI, "NFitAux Recovery");
  printinfo (nTrackI, "NTrackAux");
  AddBreak();
  printinfo (nFitMajorityI, "NFitAux Majority");
  printinfo (nFitMajoritySCTI, "NFitAux Majority missing SCT");
  printinfo (nFitMajorityPixI, "NFitAux Majority missing Pix");
  AddBreak();
  printinfo (nFit, "NFitSSB (12/12 + Recovery)");
  printinfo (nFitRecovery, "NFitSSB Recovery");
  printinfo (nTrack, "NTrackSSB");
  printinfo (nTrackBeforeHW, "NTrackSSB Before HW");
  printinfo (nConn, "N Total Connections");
  printinfo (nExtrapAUX, "N successful extrapolated AUX tracks");
  AddBreak();
  printinfo (nFitMajority, "NFitSSB Majority");
  printinfo (nFitMajoritySCT, "NFitSSB Majority missing SCT");
  printinfo (nFitMajorityPix, "NFitSSB Majority missing Pix");
  myfile.close();

  myfileTeX << "\\hline" << std::endl;
  myfileTeX << "\\end{tabular} " << std::endl;
  myfileTeX << "\\caption{Data flow for " << nloop << " events, using " << ntower << " towers. " << description << "} " << std::endl;
  myfileTeX << "\\end{table}" << std::endl;
  myfileTeX << "\\end{document}" << std::endl;
  myfileTeX.close();

  outputRootFile->cd();
  outputHistAvg->Write();
  outputHistMin->Write();
  outputHistMax->Write();
  delete outputRootFile;
  outputRootFile=0;
}


void printinfo(float towers[MAXTOWER], TString text) {

  float barrelmean(0), endcapmean(0), barrelmax(0), endcapmax(0);
  for (int i = 0; i<ntower; i++) {
    if (i < (ntower/4) || i >= 3*(ntower/4.)) { // kludge not always (?) guaranteed to work
      if (towers[i] > endcapmax) endcapmax = towers[i];
      endcapmean += towers[i]/(ntower/2.);
    }
    else {
      if (towers[i] > barrelmax) barrelmax = towers[i];
      barrelmean += towers[i]/(ntower/2.);
    }
  }

  barrelmean = ([barrelmean](int SIGDIGITS)->double{
      std::stringstream lStream;
      lStream << std::setprecision(SIGDIGITS) << barrelmean; return std::stod(lStream.str());
    })(SIGDIGITS);
  barrelmax = ([barrelmax](int SIGDIGITS)->double{
      std::stringstream lStream;
      lStream << std::setprecision(SIGDIGITS) << barrelmax; return std::stod(lStream.str());
    })(SIGDIGITS);
  endcapmean = ([endcapmean](int SIGDIGITS)->double{
      std::stringstream lStream;
      lStream << std::setprecision(SIGDIGITS) << endcapmean; return std::stod(lStream.str());
    })(SIGDIGITS);
  endcapmax = ([endcapmax](int SIGDIGITS)->double{
      std::stringstream lStream;
      lStream << std::setprecision(SIGDIGITS) << endcapmax; return std::stod(lStream.str());
    })(SIGDIGITS);


  myfile << text << "\t\t" << barrelmean << "\t\t" << barrelmax << "\t\t" << endcapmean << "\t\t" << endcapmax << endl;
  myfileTeX << text << "&" << barrelmean << "&" << barrelmax << "&" << endcapmean << "&" << endcapmax << " \\\\" << endl;
}

void AddBreak(int n) {
  for (int i = 0; i < n; i++)  {
    myfile << "--------------" << endl;
    myfileTeX << "\\hline" << std::endl;
  }
}
