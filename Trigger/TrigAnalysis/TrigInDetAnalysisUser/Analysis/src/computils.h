// emacs: this is -*- c++ -*-
//
//   @file    computils.h         
//   
//
//   @author M.Sutton
// 
//   Copyright (C) 2012 M.Sutton (sutt@cern.ch)    
//
//   $Id: comparitor.cxx, v0.0   Sat Aug 30 2014 14:38:03 CEST  sutt $

#ifndef COMPUTILS_H
#define COMPUTILS_H

#include <iostream>
#include <string>
#include <vector>

#include "label.h"
#include "DrawLabel.h" 

#include "TStyle.h"
#include "TPad.h"
#include "TH1D.h"
#include "TH1.h"
#include "TGraphAsymmErrors.h"

#include "TLegend.h"
#include "TrigInDetAnalysis/Efficiency.h"

void ATLASFORAPP_LABEL( double x, double y, int color, double size=0.06 ); 

void myText( Double_t x, Double_t y, Color_t color, const std::string& text, Double_t tsize);


/// return the current data and time
std::string stime();
static std::string release;



/// does a string contain the substring
bool contains( const std::string& s, const std::string& p);

/// does a string contain the substring at the beginning of the string
bool fcontains( const std::string& s, const std::string& p);

/// does a file exist
bool exists( const std::string& filename );

/// tail of a string 
std::string tail( std::string s, const std::string& pattern );

/// match a file name
std::string globbed( const std::string& s );

void contents( std::vector<std::string>& keys, 
	       TDirectory* td, 
	       const std::string& directory="", 
	       const std::string& pattern="", 
	       const std::string& path="" );

double realmax( TH1* h, bool include_error=true, double lo=0, double hi=0 );
double realmin( TH1* h, bool include_error=true, double lo=0, double hi=0 );


double plotable( TH1* h );



template<typename T>
std::ostream& operator<<( std::ostream& s, std::vector<T>& v) { 
  for ( unsigned i=0 ; i<v.size() ; i++ ) s << "\t" << v[i];
  return s;
}


/// automatically set the xrange on a histogram
std::vector<int>    findxrange(TH1* h, bool symmetric=false );
std::vector<double> findxrangeuser(TH1* h, bool symmetric=false );

void xrange(TH1* h, bool symmetric=false );
void xrangeuser(TH1* h, bool symmetric=false );



/// slightly more convenient legend class
class Legend { 

public:

  Legend() : mleg(0) { }  

  Legend(double x1, double x2, double y1, double y2) { 
    mleg = new TLegend(x1, y1, x2, y2);
    mleg->SetBorderSize(0);
    mleg->SetTextFont(42);
    mleg->SetTextSize(0.04);
    mleg->SetFillStyle(3000);
    mleg->SetFillColor(0);
    mleg->SetLineColor(0);
  } 


  // Legend( const Legend& leg ) : mleg((TLegend*)leg.mleg->Clone()) { } 

 Legend(const Legend& legend) : mleg(legend.mleg) { } 

  ~Legend() { } 

  TLegend* legend() { return mleg; } 

  TLegend* operator->() { return mleg; }

  size_t size() const { 
    if ( mleg ) return mleg->GetNRows();
    else        return 0;
  }  

private:

  TLegend* mleg;

}; 


static const int colours[6] = { 1, 2, kBlue-4, 6, kCyan-2, kGreen+2 };
static const int markers[6] = { 20, 24, 25, 26, 21, 22 };


template<typename T>
void setParameters( T* h, TGraphAsymmErrors* tg ) { 
  tg->SetLineColor( h->GetLineColor() );
  tg->SetMarkerColor( h->GetMarkerColor() );
  tg->SetMarkerStyle( h->GetMarkerStyle() );
  tg->SetMarkerSize( h->GetMarkerSize() );
  tg->SetLineWidth( h->GetLineWidth() );
  tg->SetLineStyle( h->GetLineStyle() );
}


template<typename T>
void zeroErrors( T* h ) {
  for ( int i=1 ; i<=h->GetNbinsX() ; i++ ) h->SetBinError( i, 1e-100 ); 
} 



/// generic plotter class - better to have one of these - make 
/// sure it can be configured however you like, line styles, 
/// marker types, legends etc 
/// now a template so can be used for TH1D and TH2D etc
template<typename T>
class tPlotter { 

public: 

  tPlotter(T* _htest=0, T* _href=0, const std::string& s="", TGraphAsymmErrors* _tgtest=0, TGraphAsymmErrors* _tgref=0 ) : 
    m_htest(_htest), m_href(_href),
    m_tgtest(_tgtest), m_tgref(_tgref),
    m_plotfilename(s) { 
  }

  
  tPlotter(const tPlotter& p) : 
    m_htest(p.m_htest),   m_href(p.m_href), 
    m_tgtest(p.m_tgtest), m_tgref(p.m_tgref), 
    m_plotfilename(p.m_plotfilename){     
  }


  /// sadly, root manages all the histograms (does it really? 
  /// who can tell) so we mustn't delete anything just in case
  // ~tPlotter() { if ( m_tg ) delete m_tg; }
  /// NO, NO, NO, don't delete the objects, root need them because 
  /// it is TOO STUPID to allow objects to be used as actual objects
  ~tPlotter() { } 

  std::string plotfilename() const { return m_plotfilename; }

  //  void Draw(const std::string& chain, unsigned int i, Legend& leg ) { 
  void Draw( int i, Legend& _leg, bool mean=false, bool first=true ) { 

    Legend leg = _leg;

    if ( htest() ) {
      gStyle->SetOptStat(0);
      if ( href() ) { 
	href()->SetLineColor(colours[i%6]);
	href()->SetLineStyle(2);
	href()->SetMarkerStyle(0);
      }
      //      href()->SetMarkerColor(href()->GetLineColor());
      htest()->SetLineColor(colours[i%6]);
      htest()->SetLineStyle(1);
      htest()->SetMarkerColor(htest()->GetLineColor());
      htest()->SetMarkerStyle(markers[i%6]);

      if(first)  { 
	if ( plotref && href() ) {
	  href()->GetYaxis()->SetMoreLogLabels(true);
	  href()->Draw("hist][");
	  //	  if ( tgref() ) { 
	  //	    setParameters( href(), tgref() );
	  //	  }
	}
	else    {
	  if ( tgtest() ) { 
	    zeroErrors(htest());
	    htest()->Draw("ep");
	    setParameters( htest(), tgtest() );
	    // tgtest()->Draw("p1same");
	    tgtest()->Draw("esame");
	  }
	  else htest()->Draw("ep");
	}
      }

#if 0
      static bool _dbg = true;

      if ( _dbg && htest() && tgtest() ) { 
	TFile f("dbg.root","recreate");
	htest()->Write();
	tgtest()->Write();
	f.Close();
	
	_dbg = false;
      }
#endif

      // if(i==0)  htest()->Draw("ep");
     
      //      if ( contains(href()->GetName(),"sigma") ) href()->SetMinimum(0);

      if ( plotref && href() ) { 
	if ( contains(href()->GetName(),"_vs_")  || 
	     contains(href()->GetName(),"sigma") || 
	     contains(href()->GetName(),"_eff") ) href()->Draw("hist same][");
	else                                      href()->Draw("hist same");
      }


      if ( tgtest() ) { 
	zeroErrors(htest());
	setParameters( htest(), tgtest() );
	tgtest()->Draw("e1same");
      }
      
#if 0
      /// solid white centres for the marker types
      if ( htest()->GetMarkerStyle()>23 ) { 
	TH1D* hnull = (TH1D*)htest()->Clone("duff"); hnull->SetDirectory(0);
	zeroErrors( hnull );
	hnull->SetLineColor(kWhite);
	hnull->SetMarkerStyle( htest()->GetMarkerStyle()-4 );
	//	hnull->SetMarkerStyle( 0 ); 
	hnull->SetMarkerColor(kWhite);
	hnull->SetMarkerSize( htest()->GetMarkerSize()*0.75 );
	// hnull->SetMarkerSize( 0 );
	hnull->DrawCopy("l same");
	delete hnull;
      }
#endif

      htest()->Draw("ep same");


      // href()->Draw("lhistsame");
      // htest()->Draw("lhistsame");

      std::string key = m_plotfilename;

      char _mean[64];
      std::sprintf( _mean, " <t> = %3.2f ms", htest()->GetMean() );


      //      std::cout << "adding key " << key << std::endl; 

      // if      ( contains( key, "FastTrack" ) )  leg->AddEntry( htest(), "  Run 2: Fast Tracking", "p" );
      // else if ( contains( key, "EFID_" ) )      leg->AddEntry( htest(), "  Run 1: EFID Tracking", "p" );
      // else if ( contains( key, "_EFID" ) )      leg->AddEntry( htest(), "  Run 2: Precision Tracking", "p" );
      
      //      std::cout << "cost size " << key.size() << std::endl;

      static TH1D* hnull = new TH1D("hnull", "", 1, 0, 1);
      hnull->SetMarkerColor(kWhite);
      hnull->SetLineColor(kWhite);
      hnull->SetMarkerStyle(0);
      hnull->SetLineStyle(0);
      hnull->SetLineWidth(0);
      hnull->SetMarkerSize(0);

      
      if ( mean ) { 
	key += std::string(" : ");
       	if ( key.size()<47 ) { 
	  key += _mean;
	  leg->AddEntry( htest(), key.c_str(), "p" );
	}
	else { 
	  leg->AddEntry( htest(), key.c_str(), "p" );
	  leg->AddEntry( hnull, _mean, "p" );
	}
      }
      else leg->AddEntry( htest(), key.c_str(), "p" );

      leg->Draw();

    }
  }
  

  /// print the output 
  void Print(const std::string& s="") {
    if ( s!="" ) gPad->Print(s.c_str());
    else         gPad->Print(m_plotfilename.c_str());
  } 

  T* htest() { return m_htest; }
  T* href()  { return m_href; }


  TGraphAsymmErrors* tgtest() { return m_tgtest; }
  TGraphAsymmErrors* tgref()  { return m_tgref; }


public:

  static void setplotref( bool b ) { plotref=b; }

private:

  /// actual histograms 
  T* m_htest;
  T* m_href;

  TGraphAsymmErrors* m_tgtest;
  TGraphAsymmErrors* m_tgref;

  std::string m_plotfilename;

  static bool plotref;

};

template<typename T>
bool tPlotter<T>::plotref = true;




typedef tPlotter<TH1F> Plotter;


inline void hminus(TH1* h) { 
  std::cout << __FUNCTION__ << std::endl; 
  for ( int i=1 ; i<=h->GetNbinsX() ; i++ ) { 
    double duff = h->GetBinContent(i);
    if (  duff<0 ) { 
      std::cout<< "\t\t" << __FUNCTION__ << " " << h->GetName() << "  " << i << " " << h->GetBinContent(i) << " " << (duff*1e6) << std::endl;  
    }
  }
  h->DrawCopy();
  gPad->Print( (std::string("duff-")+h->GetName()+".pdf").c_str() );
}


/// set of generic plots
class Plots : public std::vector<Plotter> { 
  
public:
  
  Plots(const std::string& s="") : 
    m_name(s), 
    m_logx(false), m_logy(false), 
    m_maxset(false), m_minset(false), 
    m_rangeset(false) 
  { }

  double realmin( double lo=0, double hi=0 ) {
    bool first = true;
    double _min = 1000;
    for ( unsigned i=0 ; i<size() ; i++ ) {
      double rmtest = ::realmin( at(i).htest(), false, lo, hi );
      if ( first || ( rmtest!=0 && _min>rmtest ) ) _min = rmtest;
      if ( rmtest!=0 ) first = false;
    }
    return _min;
  }

  double realmax(double lo=0, double hi=0) {
    bool first = true;
    double _max = 0;
    for ( unsigned i=0 ; i<size() ; i++ ) {
      // double rmref  = realmin( at(i).href(), false );
      double rmtest = ::realmax( at(i).htest(), false, lo, hi );
      if ( first || _max<rmtest ) _max = rmtest;
      if ( rmtest!=0 ) first = false;
    }
    return _max;
  }


  
  void MaxScale(double scale=1.1, double lo=0, double hi=0) {  
    
    if ( size()<1 ) return;

    double tmax = realmax(lo,hi);
    double tmin = realmin(lo,hi);
    
    //    std::cout << "\tname: " << m_name << "\ttmin: " << tmin << "\ttmax: " << tmax << "\tlog: " << m_logy << std::endl; 

    m_max = scale*tmin;
    
    if ( m_logy ) m_min = tmin;

    for ( unsigned i=0 ; i<size() ; i++ ) {
      at(i).href()->SetMaximum(scale*tmax);
      at(i).htest()->SetMaximum(scale*tmax);

      //      if ( m_logy ) {
      //	at(i).href()->SetMinimum(tmin);
      //	at(i).htest()->SetMinimum(tmin);
      //      }

      //   std::cout << "refmax  : " << at(i).href()->GetMaximum()  << std::endl;
      //   std::cout << "testmax : " << at(i).htest()->GetMaximum() << std::endl;
    } 

  }


  void MinScale( double scale, double lo=0, double hi=0 ) { 

    if ( size()<1 ) return;

    if ( scale==0 ) { 
      for ( unsigned i=0 ; i<size() ; i++ ) {
	at(i).href()->SetMinimum(0);
	at(i).htest()->SetMinimum(0);
      }  
      return;
    }

    
    double tmin = 0;

    if ( lo!=hi ) tmin = realmin( lo, hi );
    else          tmin = realmin();

    m_min = scale*tmin;

    for ( unsigned i=0 ; i<size() ; i++ ) {
      at(i).href()->SetMinimum(scale*tmin);
      at(i).htest()->SetMinimum(scale*tmin);

      //      if ( m_logy ) { 
      //	if ( at(i).href()->GetMinimum()<=0 )  at(i).href()->GetMinimum(1e-4);
      //	if ( at(i).htest()->GetMinimum()<=0 ) at(i).htest()->GetMinimum(1e-4);
      //      }

      //      if ( contains( at(i).href()->GetName(), "sigma" ) ) { 
      //	std::cout << "refmin  : " << at(i).href()->GetMinimum()  << std::endl;
      //	std::cout << "testmin : " << at(i).htest()->GetMinimum() << std::endl;
      //      } 
    }
  }
 

  void Min( double scale ) { 
    m_minset = true;
    for ( unsigned i=0 ; i<size() ; i++ ) {
      at(i).href()->SetMinimum(scale);
      at(i).htest()->SetMinimum(scale);
      if ( m_logy ) { 
	if ( at(i).href()->GetMinimum()<=0 )  at(i).href()->GetMinimum(1e-4);
	if ( at(i).htest()->GetMinimum()<=0 ) at(i).htest()->GetMinimum(1e-4);
      }
      
      //      if ( contains( at(i).href()->GetName(), "sigma" ) ) { 
      //	std::cout << "refmin  : " << at(i).href()->GetMinimum()  << std::endl;
      //	std::cout << "testmin : " << at(i).htest()->GetMinimum() << std::endl;
      //      } 
    }
  }
  

  void Max( double scale ) { 
    m_maxset = true;
    for ( unsigned i=0 ; i<size() ; i++ ) {
      at(i).href()->SetMaximum(scale);
      at(i).htest()->SetMaximum(scale);
    }
  }
  


  void xrange(bool symmetric=false) { 
    m_rangeset = false;
    for ( unsigned i=0 ; i<size() ; i++ ) { 
      ::xrange( at(i).href(), symmetric );
      ::xrange( at(i).htest(), symmetric );
      // ::xrange( at(i).htest(), symmetric );
    }
  }

  void SetRangeUser( double lo, double hi ){ 
    m_rangeset = true;
    m_lo = lo;
    m_hi = hi;
    for ( unsigned i=0 ; i<size() ; i++ ) { 
      at(i).href()->GetXaxis()->SetRangeUser( m_lo, m_hi );
      at(i).htest()->GetXaxis()->SetRangeUser( m_lo, m_hi );
    }
  }
  

  void limits() { 
    double rmax = realmax();
    double rmin = realmin();    
    if ( rmin<0 ) { 
      std::cout << "\tlimits \t" << m_name << "\tmin " << rmin << "\tmax " << rmax << std::endl; 
      std::cout << "\tlimits \t" << m_name << "\tmin " << rmin << "\tmax " << rmax << std::endl; 
    }
  }

   
  void Draw( Legend& leg, bool means=false ) {  
    //   Max();

    /// plotting range options etc  
    //    std::cout << "Plotter::Draw() " << m_name << "\txaxis: log: " << m_logx;

    //    if ( m_rangeset ) std::cout << "\trange: " << m_lo << " - " << m_hi;

    //    std::cout << "\tyaxis: log:" << m_logy;
    //    if ( m_minset )   std::cout << "\tymin: " << m_min;
    //    if ( m_maxset )   std::cout << "\tymax: " << m_max;
    //    std::cout << std::endl;
    
    bool first = true;
    //  for ( unsigned i=size() ; i-- ; first=false ) at(i).Draw( i, leg, means, first );
    for ( unsigned i=0 ; i<size() ; i++, first=false ) at(i).Draw( i, leg, means, first );
    if ( watermark ) DrawLabel(0.1, 0.02, "built on "+stime()+release, kBlack, 0.03 );
    gPad->SetLogy(m_logy);
    gPad->SetLogx(m_logx);
  }

  void SetLogx( bool b=true ) { m_logx=b; } 
  void SetLogy( bool b=true ) { m_logy=b; } 

  std::string GetXaxisTitle() { 
    if ( size()>0 ) return at(0).htest()->GetXaxis()->GetTitle();
    return "";
  }

  std::string GetYaxisTitle() { 
    if ( size()>0 ) return at(0).htest()->GetYaxis()->GetTitle();
    return "";
  }

  void SetXaxisTitle(std::string s) { 
    if ( size()>0 ) at(0).htest()->GetXaxis()->SetTitle(s.c_str());
  }

  void SetYaxisTitle(std::string s) { 
    if ( size()>0 ) at(0).htest()->GetYaxis()->SetTitle(s.c_str());
  }


  /// this is so that we can update the stats as we go along, 
  /// but no updating isn't done at the moment 
  void push_back(const Plotter& val) {
    std::vector<Plotter>::push_back( val );
  }

public:

  static void setwatermark(bool b) { watermark = b; }

private:
  
  std::string m_name;

  /// canvas log setting
  bool m_logx;
  bool m_logy;
  
  /// yaxis range setting
  bool   m_maxset;
  double m_max;
  
  bool   m_minset;
  double m_min;

  /// xaxis range setting
  bool   m_rangeset;
  double m_lo;
  double m_hi;

private:

  static bool watermark;

};



#endif // COMPUTILS_H
