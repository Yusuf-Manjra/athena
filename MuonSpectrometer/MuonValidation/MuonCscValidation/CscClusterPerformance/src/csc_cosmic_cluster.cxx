/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// csc_cosmic_cluster.cxx

// Woochun Park
// Nov 1, 2007
//
// Main program to loop over entries in a cluster tree to find clusters from
// cosmic muon tracks.
//
// The classes ClusterAccessor used to access the trees
// are generated by root. If changes are made, run this program with option
// -g and copy the headers to the include directory CscClusterPerformance.

#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <map>
#include <vector>
#include "CxxUtils/no_bitwise_op_warning.h"
#include "TTree.h"
#include "TTreeIndex.h"
#include "SimposAccessor.h"
#include "ClusterAccessor.h"
#include <cmath>

// To build a exe that only generates the ROOTT interface classes.
#undef  GENERATE_ONLY

typedef std::vector<int> EntryList;
typedef std::vector<std::string> NameList;

namespace {

// Class to hold event number.
  class Runevt {
    int m_run;
    int m_evt;
  public:
    Runevt(int run, int evt) : m_run(run), m_evt(evt) { }
    int run() const { return m_run; }
    int evt() const { return m_evt; }
    int operator<(const Runevt& rhs) const {
      if ( run() == rhs.run() ) return evt() < rhs.evt();
      return run() < rhs.run();
    }
    int operator==(const Runevt& rhs) const {
      return run() == rhs.run() && evt() == rhs.evt();
    }
  };

  std::ostream& operator<<(std::ostream& str, const Runevt& rhs) {
    str << rhs.run() << ":" << rhs.evt();
    return str;
  }

  class Cluster {
    float m_pos;
    float m_dpos;
    int m_phisec;
    int m_wlay;
    float m_z;
  public:
    Cluster(float pos, float dpos, float z)
      : m_pos(pos), m_dpos(dpos), m_phisec(-1), m_wlay(-1),
        m_z(z) { }
    float z()    const { return m_z; }
    float pos()    const { return m_pos; }
    float dpos()   const { return m_dpos; }
    float weight() const { return 1./m_dpos/m_dpos; }
    float zpos()   const { return (m_phisec-1)*12.55 + (m_wlay-1)*2.05;}
  };
}  // end unnamed namespace
//typedef std::vector<Cluster*> Clusters;



////////////////////////////

#if 0
void fit_segment(const std::vector<Cluster*>& clus, float& s0, float& s1, float& d0, float& d1, float& d01, float& chsq ) {
  float q0 = 0.0;
  float q1 = 0.0;
  float q2 = 0.0;
  float q01 = 0.0;
  float q11 = 0.0;
  float q02 = 0.0;
  s0=0.;
  s1=0.;
  d0=0.;
  d1=0.;
  d01=0.;
  chsq =0.;

  for ( std::vector<Cluster*>::const_iterator iclu=clus.begin(); iclu!=clus.end(); ++iclu ) {
    const Cluster& clu = **iclu;
    float y = clu.pos();
    float z = clu.z();
    float d = clu.dpos();
    float w = 1.0/(d*d);
    q0 += w;
    q1 += w*z;
    q2 += w*z*z;
    q01 += w*y;
    q11 += w*z*y;
    q02 += w*y*y;
  }
  float r00 = q2/(q2*q0-q1*q1);
  float r01 = q1/(q1*q1-q2*q0);
  float r10 = q1/(q1*q1-q0*q2);
  float r11 = q0/(q0*q2-q1*q1);
  s0 = r01*q11 + r00*q01;
  s1 = r11*q11 + r10*q01;
  d0 = sqrt(r01*r01*q2 + 2.0*r00*r01*q1 + r00*r00*q0);
  d1 = sqrt(r11*r11*q2 + 2.0*r10*r11*q1 + r10*r10*q0);
  d01 = r01*r11*q2 + (r01*r10+r00*r11)*q1 + r00*r10*q0;
  chsq = q02 + s1*s1*q2 + 2*s0*s1*q1 + s0*s0*q0 - 2*s0*q01 - 2*s1*q11;
}
#endif

void fit_segment_eqwgt(const std::vector<Cluster*>& clus, float& s0, float& s1, float& d0, float& d1, float& d01, float& chsq ) {
  float q0 = 0.0;
  float q1 = 0.0;
  float q2 = 0.0;
  float q01 = 0.0;
  float q11 = 0.0;
  float q02 = 0.0;
  s0=0.;
  s1=0.;
  d0=0.;
  d1=0.;
  d01=0.;
  chsq =0.;

  for ( std::vector<Cluster*>::const_iterator iclu=clus.begin(); iclu!=clus.end(); ++iclu ) {
    const Cluster& clu = **iclu;
    float y = clu.pos();
    float z = clu.z();
    //    float d = clu.dpos();
    float w = 1.0;
    q0 += w;
    q1 += w*z;
    q2 += w*z*z;
    q01 += w*y;
    q11 += w*z*y;
    q02 += w*y*y;
  }
  float r00 = q2/(q2*q0-q1*q1);
  float r01 = q1/(q1*q1-q2*q0);
  float r10 = q1/(q1*q1-q0*q2);
  float r11 = q0/(q0*q2-q1*q1);
  s0 = r01*q11 + r00*q01;
  s1 = r11*q11 + r10*q01;
  d0 = sqrt(r01*r01*q2 + 2.0*r00*r01*q1 + r00*r00*q0);
  d1 = sqrt(r11*r11*q2 + 2.0*r10*r11*q1 + r10*r10*q0);
  d01 = r01*r11*q2 + (r01*r10+r00*r11)*q1 + r00*r10*q0;
  chsq = q02 + s1*s1*q2 + 2*s0*s1*q1 + s0*s0*q0 - 2*s0*q01 - 2*s1*q11;
}


//////////////
int main(int narg, char* argv[]) {
  bool help = false;
  bool generate = false;
  int error = 0;
  int ndump = 0;
  std::string arg1;
  std::cout << narg << "  " << argv[0] << std::endl;
  // Read option flags.
  int iarg = 0;
  while ( ++iarg<narg && argv[iarg][0] == '-' ) {
    std::string opt = argv[iarg] + 1;
    if ( opt == "h" ) {
      help = true;
    } else if ( opt == "g" ) {
      generate = true;
    } else if ( opt == "d" ) {
      std::string sdump = argv[++iarg];
      std::istringstream ssdump(sdump);
      ssdump >> ndump;
    } else {
      std::cerr << "Uknown option: -" << opt << std::endl;
      error = 3;
    }
  }
  // Read the file names.
  if ( !error && !help ) {
    if ( iarg < narg ) {
      arg1 = argv[iarg++];
    } else {
      std::cout << "First file name not found" << std::endl;
      error = 1;
    }
  }

  if ( help ) {
    std::cout << "Usage: " << argv[0] << " [-g] simpos_file cluster_file" << std::endl;
    return error;
  }

  if ( error ) return error;

  // int x = (int);
  // char*chX='a';
  // int x = (int)*chX;

  // Open simpos file.
  TFile* psfile = new TFile(arg1.c_str(), "READ");
  TTree* pctree = dynamic_cast<TTree*>(psfile->Get("csc_cluster"));
  if ( pctree == nullptr ) {
    std::cout << "Unable to retrieve simpos tree" << std::endl;
    std::cerr << "  File: " << arg1 << std::endl;
    psfile->Print();
    return 3;
  }
  std::cout << "Cluster tree has " << pctree->GetEntries() << " entries." << std::endl;
  if ( generate ) {
    std::cout << "Generating class ClusterAccessor" << std::endl;
    pctree->MakeClass("ClusterAccessor");
  } 

#ifndef GENERATE_ONLY
#define MAXENT 200
#define MAXSTRIP 10
#define MAXPOS 10
#define MAXEXTRA 20

  // Create result tree.
  int run;
  int evt;
  //  int nentry;                // Number of entries for each event.

  ClusterAccessor cluster(pctree);
  pctree->BuildIndex("run", "evt");

  int nsample =1000;
  //  int nsample =pctree->GetEntries();

  // Loop over events.
  for ( int icevt=1; icevt<nsample; ++icevt ) {
    cluster.GetEntry(icevt);
    run = cluster.run;
    evt = cluster.evt;
    Runevt re(run, evt);
    std::cout << "Processing run:event " << re << std::endl;
    int nclu = cluster.nentry;

    // Loop over clusters.
    float ref_value[2][4][4];
    int pid[2][4][4];
    for (int i=0; i<2; ++i)
      for (int j=0; j<4; ++j)
        for (int k=0; k<4; ++k) {
          ref_value[i][j][k] =0.;
          pid[i][j][k] =0;
        }

    for (int iphi=0; iphi<4; ++iphi)
      for (int ilay=0; ilay<4; ++ilay)
        for ( int iclu=0; iclu<nclu; ++iclu ){
          if (cluster.phisec[iclu]==iphi+1 && cluster.wlay[iclu]==ilay+1 )
            if (cluster.qpeak[iclu]>ref_value[cluster.measphi[iclu]][iphi][ilay]) {
              ref_value[cluster.measphi[iclu]][iphi][ilay] =cluster.qpeak[iclu];
              pid[cluster.measphi[iclu]][iphi][ilay] = iclu;
            }
        }

    std::vector<Cluster*> global_clus;
    for (Int_t iphi=3; iphi>=0; --iphi) {
      std::vector<Cluster*> clus;
      for (Int_t ilay=3; ilay>=0; --ilay) {
        float clupos =cluster.pos[pid[0][iphi][ilay]];
        float cludpos = cluster.error[pid[0][iphi][ilay]];
        float cluz = cluster.z[pid[0][iphi][ilay]];
        Cluster* clu = new Cluster(clupos, cludpos, cluz);
        clus.push_back(clu);
        global_clus.push_back(clu);
      }
      float s0, s1, d0, d1, d01, chsq;
      fit_segment_eqwgt(clus, s0, s1, d0, d1, d01, chsq);
      std::cout << "LOCAL  " <<  std::setw(7)  << setiosflags (std::ios::fixed) << std::setprecision(0) <<chsq << "  "
           << std::setw(7)  << setiosflags (std::ios::fixed) << std::setprecision(1) << cluster.pos[pid[0][iphi][3]] << " "
           << std::setw(7)  << setiosflags (std::ios::fixed) << std::setprecision(1) << cluster.pos[pid[0][iphi][2]] << " "
           << std::setw(7)  << setiosflags (std::ios::fixed) << std::setprecision(1) << cluster.pos[pid[0][iphi][1]] << " "
           << std::setw(7)  << setiosflags (std::ios::fixed) << std::setprecision(1) << cluster.pos[pid[0][iphi][0]] << " "
           <<std::endl;
    }
    float s0, s1, d0, d1, d01, chsq;
    fit_segment_eqwgt(global_clus, s0, s1, d0, d1, d01, chsq);
    std::cout << "GLOBAL "
         << std::setw(7)  << setiosflags (std::ios::fixed) << std::setprecision(0) << chsq
         << std::setw(7)  << setiosflags (std::ios::fixed) << std::setprecision(1) << cluster.pos[pid[0][3][3]] << " "
         << std::setw(7)  << setiosflags (std::ios::fixed) << std::setprecision(1) << cluster.pos[pid[0][3][2]] << " "
         << std::setw(7)  << setiosflags (std::ios::fixed) << std::setprecision(1) << cluster.pos[pid[0][3][1]] << " "
         << std::setw(7)  << setiosflags (std::ios::fixed) << std::setprecision(1) << cluster.pos[pid[0][3][0]] << " "
           << std::setw(7)  << setiosflags (std::ios::fixed) << std::setprecision(1) << cluster.pos[pid[0][2][3]] << " "
           << std::setw(7)  << setiosflags (std::ios::fixed) << std::setprecision(1) << cluster.pos[pid[0][2][2]] << " "
           << std::setw(7)  << setiosflags (std::ios::fixed) << std::setprecision(1) << cluster.pos[pid[0][2][1]] << " "
           << std::setw(7)  << setiosflags (std::ios::fixed) << std::setprecision(1) << cluster.pos[pid[0][2][0]] << " "
           << std::setw(7)  << setiosflags (std::ios::fixed) << std::setprecision(1) << cluster.pos[pid[0][1][3]] << " "
           << std::setw(7)  << setiosflags (std::ios::fixed) << std::setprecision(1) << cluster.pos[pid[0][1][2]] << " "
           << std::setw(7)  << setiosflags (std::ios::fixed) << std::setprecision(1) << cluster.pos[pid[0][1][1]] << " "
           << std::setw(7)  << setiosflags (std::ios::fixed) << std::setprecision(1) << cluster.pos[pid[0][1][0]] << " "
           << std::setw(7)  << setiosflags (std::ios::fixed) << std::setprecision(1) << cluster.pos[pid[0][0][3]] << " "
           << std::setw(7)  << setiosflags (std::ios::fixed) << std::setprecision(1) << cluster.pos[pid[0][0][2]] << " "
           << std::setw(7)  << setiosflags (std::ios::fixed) << std::setprecision(1) << cluster.pos[pid[0][0][1]] << " "
           << std::setw(7)  << setiosflags (std::ios::fixed) << std::setprecision(1) << cluster.pos[pid[0][0][0]] << " "
         << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
  }  // end loop over events
#endif
  std::cout << "Done." << std::endl;
  return 0;
}

//******************************************************************************

// Fit a segment residual with a list of clusters.
// Segment is fit excluding cluster irclu and then the residual is calculated
// as the difference between the cluster position and that predicted by
// the segment. The error in the residual includes those of the segment and the
// excluded cluster.

// Build the skeleton functions.
#define ClusterAccessor_cxx
#include "ClusterAccessor.h"
void ClusterAccessor::Loop() { }
