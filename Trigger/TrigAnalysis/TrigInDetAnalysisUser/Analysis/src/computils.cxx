//
//   @file    computils.cxx         
//   
//
//   @author M.Sutton
// 
//   Copyright (C) 2012 M.Sutton (sutt@cern.ch)    
//
//   $Id: comparitor.cxx, v0.0  Sat Aug 30 2014 14:38:03 CEST  sutt $


#include <stdlib.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h> 
#include <glob.h>

#include <iostream>
#include <string>
#include <vector>

#include "label.h"
#include "DrawLabel.h" 

#include "TFile.h"
#include "TKey.h"
#include "TObject.h"
#include "TDirectory.h"
#include "TH1D.h"

#include "TLegend.h"
#include "TColor.h"

#include "computils.h"


bool Plots::watermark = true;



void ATLASFORAPP_LABEL( double x, double  y, int color, double size ) 
{
  TLatex* lat  = new TLatex(); //lat.SetTextAlign(12); lat.SetTextSize(tsize); 
  lat->SetNDC();
  lat->SetTextFont(72);
  lat->SetTextColor(color);
  lat->SetTextSize(size);
  lat->DrawLatex(x,y,"ATLAS");

  TLatex* lat2 = new TLatex(); //lat.SetTextAlign(12); lat.SetTextSize(tsize);   
  lat2->SetNDC();
  lat2->SetTextFont(52);
  lat2->SetTextColor(color);
  lat2->SetTextSize(size);  // this 0.13 should really be calculated as a ratio of the width of the canvas
  lat2->DrawLatex(x+0.13,y,"For Approval"); 
}

void myText( Double_t x, Double_t y, Color_t color, const std::string& text, Double_t tsize) {

  //Double_t tsize=0.05;
  TLatex lat; lat.SetTextAlign(12); lat.SetTextSize(tsize); 
  lat.SetNDC();
  lat.SetTextColor(color);
  lat.DrawLatex(x,y,text.c_str());
}


std::string stime() { 
  time_t t;
  time(&t);
  return label("%s", ctime(&t) );
}


/// contains a string
bool contains( const std::string& s, const std::string& p) { 
  return (s.find(p)!=std::string::npos);
}


/// contains a string at the *beginning* of the string
bool fcontains( const std::string& s, const std::string& p) { 
  return (s.find(p)==0);
}


bool exists( const std::string& filename ) { 
  struct stat sb;
  if ( stat( filename.c_str(), &sb)==0 ) return true; // && S_ISREG(sb.st_mode ))
  else return false;
}



std::string globbed( const std::string& s ) { 
  /// glob for a file based on the pattern, then return the name 
  /// of the first matching file
  
  glob_t glob_result;
  glob( s.c_str(), GLOB_TILDE, 0, &glob_result );

  std::vector<std::string> ret;
  for( unsigned i=0 ; i<glob_result.gl_pathc ; i++ ){
    ret.push_back( std::string(glob_result.gl_pathv[i]) );
  }
  globfree(&glob_result);

  
  if ( ret.empty() ) { 
    std::cerr << "no matching file: " << s << std::endl;
    return "";
  }  

  if ( ret.size()>1 ) { 
    for ( unsigned i=0 ; i<ret.size() ; i++ ) { 
      std::cout << "matched " << ret[i] << std::endl;
    }
  }    

  return ret[0];
}




double integral( TH1* h ) { 
  double n=0;
  for ( int i=h->GetNbinsX() ; i>0 ; i-- ) n += h->GetBinContent(i);
  return n;
}


std::string tail( std::string s, const std::string& pattern ) { 
  size_t pos = s.find(pattern);
  while ( pos != std::string::npos ) { 
    s.erase( 0, pos+1 ); 
    pos = s.find(pattern);
  }
  return s;
}

void contents( std::vector<std::string>&  keys, TDirectory* td, 
	       const std::string& directory, const std::string& pattern, const std::string& path ) { 
  
  TList* tl  = td->GetListOfKeys();
  
  bool print = true;

  for ( int i=tl->GetSize() ; i-- ; ) {

    TKey* tobj = (TKey*)tl->At(i);

    if ( tobj==0 ) continue;
    
    if ( std::string(tobj->GetClassName()).find("TDirectory")!=std::string::npos ) { 
      
      TDirectory* tnd = (TDirectory*)tobj->ReadObj();
      
      /// directory, cd to it ...
      std::string dname = tnd->GetName();
      
      std::string newpath = path+dname+"/";
      contents( keys, tnd, directory, pattern, newpath );
    
    }
    else { 
      /// test to see whether we are searhing for a specific directory name
      /// not a directory so include this ...
      if ( directory == "" || contains( path, directory ) ) {
	
	if ( print ) std::cout << "will process " << tobj->GetName() << std::endl;
	print = false;
	
	if ( pattern == "" || contains(std::string(tobj->GetName()), pattern ) ) { 
	  keys.push_back( path+tobj->GetName() );
	  // keys.push_back( tobj->GetName() );
	  //	std::cout << "object:    " << tobj->GetName() << " : \t" << path+tobj->GetName() << std::endl;
	}
      }
    }
  }
}





double realmax( TH1* h, bool include_error, double lo, double hi ) { 
  double rm = 0;
  if ( h->GetNbinsX()==0 )  return 0; 

  bool first = 0;
  if ( include_error ) rm += h->GetBinError(1);

  for ( int i=1 ; i<=h->GetNbinsX() ; i++ ) { 

    if ( lo!=hi ) { 
      double c = h->GetBinCenter(i);
      if ( lo>c || hi<c ) continue;
    }

    double re = h->GetBinContent(i);
    if ( include_error ) re += h->GetBinError(i);
    if ( first || rm<re ) { 
      rm = re;
      first = false;
    }
  }

  return rm;
}


// double realmin( TH1* h, bool include_error, double lo, double hi ) { 
double realmin( TH1* h, bool , double lo, double hi ) { 

  if ( h->GetNbinsX()==0 )  return 0; 

  double rm = 0;

  bool first = true;
  for ( int i=1 ; i<=h->GetNbinsX() ; i++ ) { 

    if ( lo!=hi ) { 
      double c = h->GetBinCenter(i);
      if ( lo>c || hi<c ) continue;
    }

    double re = h->GetBinContent(i);

    if ( re!=0 ) {
      //      if ( include_error ) re -= h->GetBinError(i);
      if ( first || rm>re ) rm = re;
      first = false;
    } 
  }

  return rm;
}


double hmean( TH1* h ) {
  double N = integral(h);
  double n=0;
  for ( int i=h->GetNbinsX() ; i>0 ; i-- ) { 
    n += h->GetBinContent(i);
    if ( 2*n>N ) return h->GetBinCenter(i);
  }
  return 0;
}




std::vector<int>  findxrange(TH1* h, bool symmetric ) { 

  int ilo = 1;
  int ihi = h->GetNbinsX();

  h->GetXaxis()->SetRange( ilo, ihi );

  std::vector<int> limits(2,0);
  limits[0] = ilo;
  limits[1] = ihi;


  double content = integral(h);

  if ( content == 0 ) return limits;

#if 1

  /// zoom on non-empty bins
  for ( ; ilo<=ihi ; ilo++ ) if ( h->GetBinContent(ilo)>0 ) break; 
  for ( ; ihi>=ilo ; ihi-- ) if ( h->GetBinContent(ihi)>0 ) break;

#else

  /// experimental zooming ...

  /// zoom on the central region containg more than some fraction X
 
  double icont = 1/content;
  
  double flo = 0;
  double fhi = 0;
  for ( ; ilo<=ihi ; ilo++ ) { 
    flo += h->GetBinContent(ilo);
    if ( (flo*icont)>0.0005 )  break;
  }
 
  for ( ; ihi>=ilo  ; ihi-- ) { 
    fhi += h->GetBinContent(ihi);
    if ( (fhi*icont)>0.0005 )  break;
  }

#endif 

  int delta_lo = ilo-1;
  int delta_hi = h->GetNbinsX()-ihi;


  if ( symmetric ) { 
    if ( delta_hi<delta_lo ) { 
      limits[0] = 1+delta_hi; 
      limits[1] = ihi; 
    }
    else { 
      limits[0] = 1+delta_lo; 
      limits[1] = h->GetNbinsX()-delta_lo;
    }
  }
  else { 
    
    if ( ilo>1 ) ilo--;
    if ( ihi<h->GetNbinsX() ) ihi++;
    
    limits[0] = ilo;
    limits[1] = ihi;
  }

  std::cout << "::xrange " << h->GetName() << "\t" << limits[0] << " " << limits[1] << std::endl;

  return limits; 

}



void xrange(TH1* h, bool symmetric ) { 
  std::vector<int> limits = findxrange( h, symmetric );
  h->GetXaxis()->SetRange( limits[0], limits[1] );
}




std::vector<double>  findxrangeuser(TH1* h, bool symmetric ) { 
  std::vector<int> limits = findxrange( h, symmetric );

  std::vector<double> dlimits(2,0);

  double dx = h->GetBinLowEdge(limits[1]+1)-h->GetBinLowEdge(limits[1]);  

  dlimits[0] = h->GetBinLowEdge(limits[0]);  
  dlimits[1] = h->GetBinLowEdge(limits[1]+1)-dx*1e-11;  
  
  return dlimits;
}


void xrangeuser(TH1* h, bool symmetric ) { 
  std::vector<double> limits = findxrangeuser( h, symmetric );
  h->GetXaxis()->SetRangeUser( limits[0], limits[1] );
}
