/* emacs: this is -*- c++ -*- */
/**
 **     @file    TIDAVertex.h
 **
 **     @author  mark sutton
 **     @date    Thu 22 Apr 2010 15:32:23 BST 
 **
 **     Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 **/


#ifndef TIDA_VERTEX_H
#define TIDA_VERTEX_H

#include <iostream>

#include "TObject.h"


namespace TIDA { 

class Vertex : public TObject {

public:

  Vertex(double  x=0, double  y=0, double  z=0,
	 double dx=0, double dy=0, double dz=0, 
	 int Ntracks=0, double c2=0, int dof=0 ) { 
    m_x[0] = x;
    m_x[1] = y;
    m_x[2] = z;

    m_dx[0] = dx;
    m_dx[1] = dy;
    m_dx[2] = dz;

    m_Ntracks=Ntracks;
    m_chi2 = c2;
    m_ndof = dof;
  } 

  Vertex(const TIDA::Vertex& v) : TObject(v) { 
    for ( int i=3 ; i-- ; ) m_x[i]  = v.m_x[i];
    for ( int i=3 ; i-- ; ) m_dx[i] = v.m_dx[i];
    m_Ntracks=v.m_Ntracks;
    m_chi2=v.m_chi2;
    m_ndof=v.m_ndof;
  }


  virtual ~Vertex();

  const double* position() const { return m_x; }
  double*       position()       { return m_x; }

  double x() const { return m_x[0]; }
  double y() const { return m_x[1]; }
  double z() const { return m_x[2]; }

  const double* error() const { return m_dx; }
  double*       error()       { return m_dx; }

  double dx() const { return m_dx[0]; }
  double dy() const { return m_dx[1]; }
  double dz() const { return m_dx[2]; }

  int Ntracks() const  { return m_Ntracks; }

  double chi2()    const  { return m_chi2; }
  int    ndof()    const  { return m_ndof; }

protected:

  void setNtracks( int nTracks ) { m_Ntracks = nTracks; }

private:

  double m_x[3];
  double m_dx[3];
  
  int    m_Ntracks;
  double m_chi2;
  int    m_ndof;

  ClassDef(TIDA::Vertex,2)

};


}

inline std::ostream& operator<<( std::ostream& s, const TIDA::Vertex& t ) { 
  return s <<  "[ x=" << t.position()[0]  << " +- " << t.error()[0]
	   <<  "\ty=" << t.position()[1]  << " +- " << t.error()[1]
	   <<  "\tz=" << t.position()[2]  << " +- " << t.error()[2]
	   << ";\tchi2="   << t.chi2()    << "/" << t.ndof()
	   << "\tNtracks=" << t.Ntracks() << "\t]"; 
}


#endif  // TIDA_VERTEX_H 










