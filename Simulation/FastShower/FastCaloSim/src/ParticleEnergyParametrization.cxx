/*
  Copyright (C) 2002-2023 CERN for the benefit of the ATLAS collaboration
*/

#include "FastCaloSim/ParticleEnergyParametrization.h"
#include "TMath.h"
#include "TBuffer.h"

#include "FastCaloSimAthenaPool/FastShowerInfo.h"
#include <exception>
#include <stdexcept> // out_of_range exception
#include "CxxUtils/checker_macros.h"

using namespace std;


namespace {


const Double_t* GetIntegral (const TH1& h)
{
  // Ok as long as h.ComputeIntegral() has already been called.
  TH1& h_nc ATLAS_THREAD_SAFE = const_cast<TH1&>(h);
  return h_nc.GetIntegral();
}


// TH2::GetRandom2 is not MT-safe, since it's non-const and uses gRandom.
// Copy it here, making it const and taking the random generator
// as an argument rather than using gRandom.
// ComputeIntegral must have already been called.
void GetRandom2 (TRandom& rand, const TH2& h, Double_t &x, Double_t &y)
{
  Int_t nbinsx = h.GetNbinsX();
  Int_t nbinsy = h.GetNbinsY();
  Int_t nbins  = nbinsx*nbinsy;
  const Double_t* integral_arr = GetIntegral (h);
  Double_t integral = integral_arr[nbins];
  // compute integral checking that all bins have positive content (see ROOT-5894)
  if (integral == 0 ) { x = 0; y = 0; return;}
  // case histogram has negative bins
  if (integral == TMath::QuietNaN() ) { x = TMath::QuietNaN(); y = TMath::QuietNaN(); return;}

  Double_t r1 = rand.Rndm();
  Int_t ibin = TMath::BinarySearch(nbins,integral_arr,(Double_t) r1);
  Int_t biny = ibin/nbinsx;
  Int_t binx = ibin - nbinsx*biny;
  x = h.GetXaxis()->GetBinLowEdge(binx+1);
  if (r1 > integral_arr[ibin]) x +=
                                 h.GetXaxis()->GetBinWidth(binx+1)*(r1-integral_arr[ibin])/(integral_arr[ibin+1] - integral_arr[ibin]);
  y = h.GetYaxis()->GetBinLowEdge(biny+1) + h.GetYaxis()->GetBinWidth(biny+1)*rand.Rndm();
}


// TH1::GetRandom2 is not MT-safe, since it's non-const and uses gRandom.
// Copy it here, making it const and taking the random generator
// as an argument rather than using gRandom.
// ComputeIntegral must have already been called.
Double_t GetRandom (TRandom& rand, const TH1& h)
{
  Int_t nbinsx = h.GetNbinsX();
  const Double_t* integral_arr = GetIntegral (h);
  Double_t integral = integral_arr[nbinsx];
  // compute integral checking that all bins have positive content (see ROOT-5894)
  if (integral == 0) return 0;
  // return a NaN in case some bins have negative content
  if (integral == TMath::QuietNaN() ) return TMath::QuietNaN();

  Double_t r1 = rand.Rndm();
  Int_t ibin = TMath::BinarySearch(nbinsx,integral_arr,r1);
  Double_t x = h.GetBinLowEdge(ibin+1);
  if (r1 > integral_arr[ibin]) x +=
                                 h.GetBinWidth(ibin+1)*(r1-integral_arr[ibin])/(integral_arr[ibin+1] - integral_arr[ibin]);
  return x;
}


} // anonymous namespace


ParticleEnergyParametrization::ParticleEnergyParametrization(int id,double E,double eta):TNamed(Form("ParticleShape%d_E%d_eta%d",id,(int)(E+0.1),(int)(eta*100+0.1)),Form("ParticleShape pdgid=%d E=%1.1f eta=%4.2f",id,E,eta)),m_id(id),m_E(E),m_eta(eta),
  m_weights_err()
{
  m_Ecal_vs_dist=nullptr;
  m_h_layer_d_fine=nullptr;
  for(int i=CaloCell_ID_FCS::FirstSample;i<CaloCell_ID_FCS::MaxSample;++i) {
    m_weights[i]=1;
//    m_Elayermean_vs_dist[i]=0;
  }
}

ParticleEnergyParametrization::~ParticleEnergyParametrization() {
  m_DistPara.Delete();
  delete m_Ecal_vs_dist;
  delete m_h_layer_d_fine;
}

void  ParticleEnergyParametrization::set_Ecal_vs_dist(TH2* h) {
  m_Ecal_vs_dist=h;
  h->ComputeIntegral(true);
  m_DistPara.Expand(h->GetNbinsX()+2);
  for(int distbin=0;distbin<h->GetNbinsX()+2;++distbin) m_DistPara.AddAt(new ParticleEnergyParametrizationInDistbin(),distbin);
}

void ParticleEnergyParametrization::MakeCumulativeX(int calosample,TH2* h) {
  if(!h) return;
  
  for(int distbin=1;distbin<=h->GetNbinsX();++distbin) {
    TH1D* h1=h->ProjectionY("_py",distbin,distbin);
    h1->SetDirectory(nullptr);
    h1->ComputeIntegral();
    if(!DistPara(distbin)) {
      cout<<"ParticleEnergyParametrization::MakeCumulativeX : calosample="<<calosample<<" distbin="<<distbin<<" size="<<m_DistPara.GetSize()<<" m_Ecal_vs_dist->GetNbinsX()="<<m_Ecal_vs_dist->GetNbinsX()<<endl;
      m_Ecal_vs_dist->Dump();      
    }  
    DistPara(distbin)->m_ElayerProp[calosample]=h1;
  }
}

void ParticleEnergyParametrization::CorelatedGausRandom_corset(const TMatrixD& v,TMatrixD& c) {
/*
Code from cernlib corset, 
Method from : Monte Carlo theory and practice, F James 1980 Rep. Prog. Phys. 43 1145-1189

      SUBROUTINE CORSET (V,C,NP)
C         Correlated Gaussian random number package
C
C         CORSET sets up the generation
C         CORGEN actually generates a set of NP random numbers
C                Gaussian-distributed with covariance matrix V
C                and mean values zero.
C
C         NP is dimension of vectors and matrices
C         V(NP,NP) is covariance matrix 
C         C(NP,NP) is working space used internally to hold the
C                  'square root' of V
C         X(NP) is the vector of random numbers generated by CORGEN
C
      DIMENSION V(NP,NP), C(NP,NP)
C
C               Compute C = square root of matrix V
      DO 40 J=1,NP
C               Diagonal terms
      CK = 0.
         DO 10 K= 1, J-1
   10    CK = CK + C(J,K)*C(J,K)
      C(J,J) = SQRT(ABS(V(J,J) - CK))
C
C               Off-diagonal terms
      DO 30 I= J+1, NP
         CK = 0.
         DO 20 K= 1, J-1
   20    CK = CK + C(I,K)*C(J,K)
      C(I,J) = (V(I,J) - CK) /C(J,J)
   30 CONTINUE
   40 CONTINUE
C
      RETURN
      END
*/

//  if(v.GetNrows()!=v.GetNcols()) {
  if(v.GetRowLwb()!=v.GetColLwb() || v.GetRowUpb()!=v.GetColUpb()) {
    cout<<"ParticleEnergyParametrization::CorelatedGausRandom_corset : covariance matrix v is not a square matrix"<<endl;
    return;
  }
//  int np=v.GetNrows();
  int Lwb=v.GetRowLwb();
  int Upb=v.GetRowUpb();
  
  c.ResizeTo(0,0);
  c.ResizeTo(Lwb,Upb,Lwb,Upb);
  for(int j=Lwb;j<=Upb;++j) {
    double ck=0;
    for(int k=Lwb;k<=j-1;++k) ck+=c(j,k)*c(j,k);
    
    c(j,j)=TMath::Sqrt(TMath::Abs(v(j,j)-ck));
    if(c(j,j)==0) c(j,j)=1.0;
    
    for(int i=j+1;i<=Upb;++i) {
      ck=0;
      for(int k=Lwb;k<=j-1;++k) ck+=c(i,k)*c(j,k);
      c(i,j)=(v(i,j)-ck)/c(j,j);
    }
  }
//  c.Print();
}

void ParticleEnergyParametrization::CorelatedGausRandom_corgen(const TMatrixD& c  ,TVectorD& x,TRandom& rand,int startidx) {
/*
Code from cernlib corset, 
Method from : Monte Carlo theory and practice, F James 1980 Rep. Prog. Phys. 43 1145-1189

      SUBROUTINE CORGEN(C,X,NP)
C
C         CORSET sets up the generation by calculating C from V.
C         CORGEN generates a set of NP random numbers
C                Gaussian-distributed with covariance matrix V
C                (V = C*C') and mean values zero.
C
      DIMENSION C(NP,NP), X(NP)
      PARAMETER (NMAX=100)
      DIMENSION Z(NMAX)
C
      IF (NP .GT. NMAX)  GO TO 120
C
      CALL RNORML(Z,NP)
C
      DO 100 I= 1, NP
         X(I) = 0.
         DO 90 J= 1, I
         X(I) = X(I) + C(I,J)*Z(J)
   90    CONTINUE
  100 CONTINUE
C
      RETURN
C                Error return
  120 CONTINUE
      WRITE (6,121) NP,NMAX
  121 FORMAT (' ERROR IN CORGEN. VECTOR LENGTH NP=',I5,
     1   ', BUT MAXIMUM ALLOWED IS',I5)
      RETURN
      END
*/

  if(c.GetRowLwb()!=c.GetColLwb() || c.GetRowUpb()!=c.GetColUpb()) {
    cout<<"ParticleEnergyParametrization::CorelatedGausRandom_corset : covariance matrix c is not a square matrix"<<endl;
    return;
  }
//  int np=v.GetNrows();
  int Lwb=c.GetRowLwb();
  int Upb=c.GetRowUpb();
  if(startidx<Lwb) startidx=Lwb;
  if(startidx>Upb) startidx=Upb;
  
  x.ResizeTo(Lwb,Upb);
  TVectorD z(Lwb,Upb);
  for(int i=Lwb;i<startidx;++i) z(i)=x(i);
  for(int i=startidx;i<=Upb;++i) z(i)=rand.Gaus();
  for(int i=startidx;i<=Upb;++i) {
    x(i)=0;
    for(int j=Lwb;j<=i;++j) x(i)+=c(i,j)*z(j);
  }
}

void ParticleEnergyParametrization::DiceParticle(ParticleEnergyShape& p,TRandom& rand) const {
  if(!m_Ecal_vs_dist) {
    p.E=0;
    p.Ecal=0;
    p.dist_in=0;
    p.dist000=0;
    return;
  }
  GetRandom2 (rand, *m_Ecal_vs_dist, p.dist_in, p.Ecal);
  int distbin=m_Ecal_vs_dist->FindFixBin(p.dist_in);
  if(distbin<1) distbin=1;
  if(distbin>m_Ecal_vs_dist->GetNbinsX()) distbin=m_Ecal_vs_dist->GetNbinsX();
  
  double xmin= std::as_const(m_Ecal_vs_dist)->GetXaxis()->GetBinLowEdge(distbin);
  double xmax= std::as_const(m_Ecal_vs_dist)->GetXaxis()->GetBinUpEdge(distbin);

  //p.dist_in = GetRandomInBinRange(rand, xmin,xmax ,m_h_layer_d_fine);
  if(m_h_layer_d_fine) {
    //cout<<" fine hist ptr="<<m_h_layer_d_fine<<endl;
    //cout<<" fine hist="<<m_h_layer_d_fine->GetName()<<" : "<<m_h_layer_d_fine->GetTitle()<<endl;
    p.dist_in = GetRandomInBinRange(rand, xmin,xmax , m_h_layer_d_fine);
  }

  const ParticleEnergyParametrizationInDistbin* shapeindist=DistPara(distbin);
  if(!shapeindist) {
    cout<<"shapeindist not found : dist_in="<<p.dist_in<<" ECal="<<p.Ecal<<" distbin="<<distbin<<endl;
    return;
  }
  
  p.fcal_tot=0;
  
  if(shapeindist->m_corr.GetNoElements()>0) {
    TVectorD x(-2,CaloCell_ID_FCS::LastSample);
    x(-2)=p.dist_in;
    x(-2)-=shapeindist->m_mean(-2);
    if(shapeindist->m_RMS(-2)>0) x(-2)/=shapeindist->m_RMS(-2);
    x(-1)=p.Ecal;
    x(-1)-=shapeindist->m_mean(-1);
    if(shapeindist->m_RMS(-1)>0) x(-1)/=shapeindist->m_RMS(-1);
    
    CorelatedGausRandom_corgen(shapeindist->m_corr,x,rand,0);
    
    for(int i=CaloCell_ID_FCS::FirstSample;i<CaloCell_ID_FCS::MaxSample;++i) {
      p.fcal_layer[i]=0;
      if(::isnan(x(i))) {
        cout<<" WARNING: encountered nan in "<<GetName()<<", distbin="<<distbin<<" : x("<<i<<")="<<x(i)<<" n="<<shapeindist->m_corr.GetNoElements()<<" elements:"<<endl;
        x.Print();
        shapeindist->m_mean.Print();
        shapeindist->m_RMS.Print();
        shapeindist->m_corr.Print();
      }
      TH1* h1=shapeindist->m_ElayerProp[i];
      if(h1) {
        double* fIntegral=h1->GetIntegral();
        if(!fIntegral) h1->ComputeIntegral();
        fIntegral=h1->GetIntegral();
        if(fIntegral) {
          double prop=TMath::Freq(x(i));
  //        cout<<"cs="<<i<<" prop="<<prop<<endl;
          int nbinsx = h1->GetNbinsX();
          int ibin = (Int_t)TMath::BinarySearch(nbinsx,fIntegral,prop);

          double xdist;
          if(fIntegral[ibin+1]<=fIntegral[ibin]) xdist=h1->GetBinCenter(ibin+1);
           else xdist=h1->GetBinLowEdge(ibin+1)+h1->GetBinWidth(ibin+1)*(prop-fIntegral[ibin])/(fIntegral[ibin+1] - fIntegral[ibin]);

          double f=xdist+shapeindist->m_mean(i);
          if(f<0) f=0;
          p.fcal_layer[i]=f;
          p.fcal_tot+=f;
        } else cout<<"cs="<<i<<" no fIntegral"<<endl; 
      }
    }
  } else {
    for(int i=CaloCell_ID_FCS::FirstSample;i<CaloCell_ID_FCS::MaxSample;++i) {
      p.fcal_layer[i]=0;
      TH1* h1=shapeindist->m_ElayerProp[i];
      if(h1) {
        double f=GetRandom(rand, *h1)+shapeindist->m_mean(i);
        if(f<0) f=0;
        p.fcal_layer[i]=f;
        p.fcal_tot+=f;
      }
    }
  }

  p.fcal_tot_uncor=0;
  for(int i=CaloCell_ID_FCS::FirstSample;i<CaloCell_ID_FCS::MaxSample;++i) {
//    p.fcal_layer[i]=0;
    TH1* h1=shapeindist->m_ElayerProp[i];
    if(h1) {
      double f=GetRandom(rand, *h1)+shapeindist->m_mean(i);
      if(f<0) f=0;
//        p.fcal_layer[i]=f;
      p.fcal_tot_uncor+=f;
    }
  }


  if(p.fcal_tot>0) for(int i=CaloCell_ID_FCS::FirstSample;i<CaloCell_ID_FCS::MaxSample;++i) {
    p.fcal_layer[i]/=p.fcal_tot;
  }  

  p.E=0;
  p.dist000=0;
  p.dist_rec=0;
  for(int i=CaloCell_ID_FCS::FirstSample;i<CaloCell_ID_FCS::MaxSample;++i) {
    double Ecal=p.fcal_layer[i]*p.Ecal;
    double E=Ecal/m_weights[i];
    
    p.E_layer[i]=E;
    p.E+=E;
  }
}

void ParticleEnergyParametrization::RepeatDiceParticles(ParticleEnergyShape* p,int n) const {
  TRandom rand;
  for(int ip=0;ip<n;++ip) {
    if(ip%1000==0) cout<<"simul particle "<<ip<<endl;
    DiceParticle(p[ip],rand);
  }
}

double ParticleEnergyParametrization::GetRandomInBinRange(TRandom& rand, double xmin_in1, double xmax_in1 , const TH1* in2) {

 // cout << " enter GetRandomInBinRange " << endl;
  int NumOfBin_in2 = in2->GetNbinsX();

  // find bins in "in2" and NumOfBins
  int firstbin_in2 = in2->FindFixBin(xmin_in1);
  if(firstbin_in2>0) --firstbin_in2;
  int lastbin_in2  = in2->FindFixBin(xmax_in1);
  if(in2->GetNbinsX()< lastbin_in2) lastbin_in2  = in2->GetNbinsX();

// Get the number of bins in "in2"
//  int nbinsx = lastbin_in2 - firstbin_in2 +1 ;
// dice random number flat in [0,1] and map it to interval [fIntegral(firstbin_in2),fIntegral(lastbin_in2)]
  double r1,r;
  double x;
  do{
     r=rand.Rndm();
     // For calling GetIntegral().
     // TH1::GetIntegral() is non-const because the first time it is called,
     // it computes and memoizes the integral.  After that, it simply returns
     // what was calculated earlier.
     // We take care to call GetIntegral when we're given the histogram.
     // If that's done, then it should be safe to call GetIntegral() in MT.
     TH1* in2_nc ATLAS_THREAD_SAFE = const_cast<TH1*>(in2);
     r1= in2_nc->GetIntegral()[firstbin_in2] +r*( in2_nc->GetIntegral()[lastbin_in2]-in2_nc->GetIntegral()[firstbin_in2]);

     Int_t ibin = TMath::BinarySearch(NumOfBin_in2,&in2_nc->GetIntegral()[0],r1);
     x = in2->GetXaxis()->GetBinLowEdge(ibin+1);
  
     if (r1 > in2_nc->GetIntegral()[ibin]){ 
         x += in2->GetBinWidth(ibin+1)*(r1-in2_nc->GetIntegral()[ibin])/(in2_nc->GetIntegral()[ibin+1] - in2_nc->GetIntegral()[ibin]);
     }
   } while (x<  xmin_in1|| x> xmax_in1);
  
return x;
}

void ParticleEnergyParametrization::SetNoDirectoryHisto()
{
  if(m_Ecal_vs_dist) m_Ecal_vs_dist->SetDirectory(nullptr);
  if(m_h_layer_d_fine) m_h_layer_d_fine->SetDirectory(nullptr);
  for(int distbin=0;distbin<m_DistPara.GetSize();++distbin) if(DistPara(distbin)) DistPara(distbin)->SetNoDirectoryHisto();
}


//______________________________________________________________________________
void ParticleEnergyParametrization::Streamer(TBuffer &R__b)
{
   // Stream an object of class ParticleEnergyParametrization.

   UInt_t R__s, R__c;
   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(&R__s, &R__c); if (R__v) { }
      TNamed::Streamer(R__b);
      R__b >> m_id;
      R__b >> m_E;
      R__b >> m_eta;
      R__b.ReadFastArray((double*)m_weights,CaloCell_ID_FCS::MaxSample);
      R__b.ReadFastArray((double*)m_weights_err,CaloCell_ID_FCS::MaxSample);
      R__b >> m_Ecal_vs_dist;
      if(R__v>=3) { 
        R__b >> m_h_layer_d_fine;
        if(m_h_layer_d_fine) {
          m_h_layer_d_fine->SetDirectory(nullptr);
          m_h_layer_d_fine->GetIntegral();
        }
      }
      m_DistPara.Streamer(R__b);
      int R__i;
      for (R__i = 0; R__i < 24; R__i++)
         m_Elayer_vs_dist[R__i].Streamer(R__b);
      R__b.CheckByteCount(R__s, R__c, ParticleEnergyParametrization::IsA());
      
      if(m_Ecal_vs_dist) {
        double integ=m_Ecal_vs_dist->Integral();
        if(integ<=0) {
          delete m_Ecal_vs_dist;
          m_Ecal_vs_dist=nullptr;
        } else {
          double* fIntegral=m_Ecal_vs_dist->GetIntegral();
          if(!fIntegral) m_Ecal_vs_dist->ComputeIntegral();
          fIntegral=m_Ecal_vs_dist->GetIntegral();
          if(!fIntegral) {
            delete m_Ecal_vs_dist;
            m_Ecal_vs_dist=nullptr;
          }
        }  
      }
   } else {
      R__c = R__b.WriteVersion(ParticleEnergyParametrization::IsA(), kTRUE);
      TNamed::Streamer(R__b);
      R__b << m_id;
      R__b << m_E;
      R__b << m_eta;
      R__b.WriteArray(m_weights, 24);
      R__b.WriteArray(m_weights_err, 24);
      R__b << (TObject*)m_Ecal_vs_dist;
      m_DistPara.Streamer(R__b);
      int R__i;
      for (R__i = 0; R__i < 24; R__i++)
         m_Elayer_vs_dist[R__i].Streamer(R__b);
      R__b.SetByteCount(R__c, kTRUE);
   }
}

void ParticleEnergyParametrization::CopyDebugInfo(FastShowerInfo* fsi) const
{
	if(!fsi)
	{
		cout << "ParticleEnergyParametrization \tERROR \tCopyDebugInfo called with NULL pointer for FastShowerInfo object" << endl;
		return;
	}
	
	fsi->m_pep_id = (int) id();
	fsi->m_pep_e = (float) E();
	fsi->m_pep_eta = (float) eta();
}

ClassImp(ParticleEnergyParametrization)
