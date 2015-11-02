/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/StatusCode.h"

#include "TrkAlgebraUtils/AlMat.h"
#include "TrkAlgebraUtils/AlVec.h"
#include "TrkAlgebraUtils/AlSymMat.h"
#include "TrkAlgebraUtils/AlSpaMat.h"

#include <algorithm>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <stdint.h>

namespace Trk {

AlMat::AlMat() {
  _ncol = 0; _nrow = 0; _nele = 0;
  ptr_data = 0;  // set pointer to null
  transpose = false;
  m_pathbin="./";
  m_pathtxt="./";
}

AlMat::AlMat(int N, int M) {
  _nrow = N;
  _ncol = M;
  _nele = N*M;
  ptr_data = new double[_nele];
  transpose = false;
  m_pathbin="./";
  m_pathtxt="./";

  double*  p = ptr_data + _nele;
  while (p > ptr_data) *(--p) = 0.;

}

AlMat::AlMat(const AlMat& m) {
  _nrow = m.nrow(); _ncol = m.ncol(); _nele = _nrow*_ncol;
  ptr_data = new double[_nele];
  transpose = false;
  m_pathbin = m.m_pathbin;
  m_pathtxt = m.m_pathtxt;
  copy(m);
}

AlMat::~AlMat()
{if( ptr_data != NULL) delete [] ptr_data;}

void AlMat::copy(const AlMat&  m) {
  int  nr = nrow();
  int  nc = ncol();
  if( nr != m.nrow() || nc != m.ncol() ) {
    std::cerr << "AlMat::copy: sizes do not match!" << std::endl;
    return; }

  for( int i=0; i<nr; i++ ) {
    for( int j=0; j<nc; j++ ) {
      elemr(i, j) = m.elemc(i, j);
    }
  }
}

void AlMat::copy(const AlSymMatBase&  m) {
  long int  n = m.size();
  if( nrow() != n || ncol() != n ) {
    std::cerr << "AlMat::copy: sizes do not match!" << std::endl;
    return; }

  for( int i=0; i<n; i++ ) {
    for( int j=0; j<n; j++ ) {
      *(ptr_data+i*_ncol+j) = m.elemc(i, j);
    }
  }
}

double& AlMat::elemr(long int i,long int j) {
#ifdef _DEBUG
  if( i<0 )     { std::cerr << "AlMat::elemr: Index 1 < zero! " << i << std::endl;  return *(ptr_data); };
  if( i>=size ) { std::cerr << "AlMat::elemr: Index 1 too large! " << i << std::endl;  return *(ptr_data); };
  if( j<0 )     { std::cerr << "AlMat::elemr: Index 2 < zero! " << j << std::endl;  return *(ptr_data);  };
  if( j>=size ) { std::cerr << "AlMat::elemr: Index 2 too large! " << j << std::endl;  return *(ptr_data); };
#endif

  if( transpose )   return  *(ptr_data+j*_ncol+i);
  return  *(ptr_data+i*_ncol+j);
}

double AlMat::elemc(long int i,long int j) const {
#ifdef _DEBUG
  if( i<0 )     { std::cerr << "AlMat::elemc: Index 1 < zero! " << i << std::endl;  return *(ptr_data); };
  if( i>=size ) { std::cerr << "AlMat::elemc: Index 1 too large! " << i << std::endl;  return *(ptr_data); };
  if( j<0 )     { std::cerr << "AlMat::elemc: Index 2 < zero! " << j << std::endl;  return *(ptr_data);  };
  if( j>=size ) { std::cerr << "AlMat::elemc: Index 2 too large! " << j << std::endl;  return *(ptr_data); };
#endif

  if( transpose )   return  *(ptr_data+j*_ncol+i);
  return  *(ptr_data+i*_ncol+j);
}

long int AlMat::elem(long int i,long int j) const {
#ifdef _DEBUG
  if( i<0 )     { std::cerr << "AlMat::elem: Index 1 < zero! " << i << std::endl;  return *(ptr_data); };
  if( i>=size ) { std::cerr << "AlMat::elem: Index 1 too large! " << i << std::endl;  return *(ptr_data); };
  if( j<0 )     { std::cerr << "AlMat::elem: Index 2 < zero! " << j << std::endl;  return *(ptr_data);  };
  if( j>=size ) { std::cerr << "AlMat::elem: Index 2 too large! " << j << std::endl;  return *(ptr_data); };
#endif

  if( transpose )   return  (j*_ncol+i);
  return  (i*_ncol+j);
}


AlMat&  AlMat::operator=(const double& d) {

  double*  p = ptr_data + _nele;
  while (p > ptr_data) *(--p) = d;

  return *this;
}

AlMat& AlMat::operator=(const AlMat& m) {
  if (this==&m) return *this;

  if(( _nrow!=0 || _ncol!=0) && (_nrow != m.nrow() || _ncol != m.ncol() ))
    { std::cerr << "AlMat=AlMat Assignment: size do not match!" << std::endl; return *this; }

  if ( ptr_data != m.ptr_data ) {
    reSize(m.nrow(), m.ncol());
    copy(m);
  };
  return (*this);
}

AlMat&  AlMat::operator=(const AlSymMat& m) {
  if( ( _nrow!=0 || _ncol!=0) && (_nrow != m.size() || _ncol != m.size() ))
    { std::cerr << "AlMat=AlSymMatBase Assignment: size do not match!" << std::endl; return *this; }

  if ( ptr_data != m.ptrData() ) {
    reSize(m.size(), m.size());
    copy(m);
  }
  return (*this);
}

AlMat AlMat::operator+(const AlMat& m) const {
  if( _nrow != m._nrow || _ncol != m._ncol )
    { std::cerr << "AlMat: operator+: size do not match!" << std::endl; return *this; }

  AlMat b( _nrow, _ncol );

  double*  p = ptr_data + _nele;
  double*  q = m.ptr_data + _nele;
  double*  r = b.ptr_data + _nele;
  while (p > ptr_data) *(--r) = (*(--p))+(*(--q));

  return b;
}

AlMat&  AlMat::operator+=(const AlMat& m) {

  if( _nrow != m._nrow || _ncol != m._ncol )
    { std::cerr << "AlMat: operator+=: size do not match!" << std::endl; return *this; }

  double*  p = ptr_data + _nele;
  double*  q = m.ptr_data + _nele;
  while (p > ptr_data) *(--p) += *(--q);

  return *this;
}

AlMat   AlMat::operator-(const AlMat& m) const {
  if( _nrow != m._nrow || _ncol != m._ncol )
    { std::cerr << "AlMat: operator-: size do not match!" << std::endl; return *this; }

  AlMat b( _nrow, _ncol );

  double*  p = ptr_data + _nele;
  double*  q = m.ptr_data + _nele;
  double*  r = b.ptr_data + _nele;
  while (p > ptr_data) *(--r) = (*(--p))-(*(--q));

  return b;
}

AlMat&  AlMat::operator-=(const AlMat& m) {
  if( _nrow != m._nrow || _ncol != m._ncol )
    { std::cerr << "AlMat: operator-=: size do not match!" << std::endl; return *this; }

  double*  p = ptr_data + _nele;
  double*  q = m.ptr_data + _nele;
  while (p > ptr_data) *(--p) -= *(--q);

  return *this;
}

AlMat AlMat::operator*(const AlMat& m) const {
  if( ncol() != m.nrow() ) {  std::cerr << "AlMat: operator*: size do not match!" << std::endl;
  return m; }

  int k(nrow());
  int l(m.ncol());
  int f(ncol());

  AlMat b( k, l);
  for( int i=0; i<k; i++ ) {
    for( int j=0; j<l; j++ ) {
      for( int n=0; n<f; n++) b.elemr(i, j) += elemc(i, n)*m.elemc(n, j);
    }
  }
  return b;
}

AlMat AlMat::operator*(const AlSymMatBase& m) const {
  if( ncol() != m.size()) {  std::cerr << "AlMat: operator*: size do not match!" << std::endl;
  return *this; }

  int k(nrow());
  int l(m.size());

  AlMat b( k, l);
  for( int i=0; i<k; i++ ) {
    for( int j=0; j<l; j++ ) {
      for( int n=0; n<l; n++) b.elemr(i, j) += elemc(i, n)*m.elemc(n, j);
    }
  }
  return b;
}


AlVec AlMat::operator*(const AlVec& v) const {
  if( ncol() != v.size() )
    { std::cerr << "AlMat: operator*: size do not match! " << std::endl; return v; }

  int k(nrow());
  int l(ncol());
  double* p;
  double* q;

  AlVec b(k);
  for( int i=0; i<k; i++ ) {
    p = b.ptrData()+i;
    q = ptr_data+i*l;
    for( int j=0; j<l; j++ )  *p += (*(q+j))*(*(v.ptrData()+j));
  }

  return b;
}

AlMat&   AlMat::operator*=(const double& d) {
  double*  p = ptr_data+_nele;
  while (p > ptr_data)
    *(--p) *= d;

  return *this;
}

// transposition
AlMat AlMat::T() const {
  AlMat b(_ncol,_nrow);
  for( int i=0; i<b._nrow; i++ ) {
    for( int j=0; j<b._ncol; j++ ) {
      b[i][j]= *(ptr_data+j*_ncol+i);
    }
  }
  return b;
}

// transposition
AlMat& AlMat::Transpose() {

  transpose = true;

  return *this;
}

// normal position
AlMat& AlMat::Normal() {

  transpose = false;

  return *this;
}


//invert sym matrix declared as non-symetric for convenience
void AlMat::invertS(int& ierr, double Norm = 1.) {
  if(_nrow!=_ncol) {
    std::cerr << "AlMat invertS: non-square matrix!" << std::endl;
    return;
  }

  AlSymMat b(_nrow);
  for( int i=0; i<b.size(); i++ ) {
    for( int j=0; j<=i; j++ ) {
      b.elemr(i, j) = elemc(i, j);
    }
  }

  b*=Norm;
  ierr = b.invert();
  b*=Norm;

  if (ierr==0) (*this).copy(b);

}

// reSize
void AlMat::reSize(int Nnew, int Mnew) {
  if ( Nnew != _nrow || Mnew != _ncol ) {

    double*  p = ptr_data;
    _nele = Nnew*Mnew;
    ptr_data = new double[_nele];
    int _nrow_old = _nrow;
    int _ncol_old = _ncol;
    transpose = false;

    _nrow = Nnew;
    _ncol = Mnew;
    int k = _nrow <= _nrow_old ? _nrow : _nrow_old;
    int l = _ncol <= _ncol_old ? _ncol : _ncol_old;

    for( int i=0; i<k; i++ ) {
      for( int j=0; j<l; j++ ) {
        *(ptr_data+i*_ncol+j) = *(p+i*_ncol_old+j);
      }
    }

    delete [] p;
  }
}


StatusCode AlMat::ReadScalaPack(const std::string &filename){
  std::ifstream inmat(filename.c_str(), std::ios::binary);
  if(inmat.fail())
    return StatusCode::FAILURE;

  int32_t mNrow = _nrow;
  inmat.read((char*)&mNrow, sizeof (mNrow));
  int32_t mNcol = _ncol;
  inmat.read((char*)&mNcol, sizeof (mNcol));

  _nrow=abs(mNrow);
  _ncol=abs(mNcol);
  _nele=_ncol*_nrow;
  transpose = false;

  // printf("ALMat::nrow: %d \n",_nrow);
  // printf("ALMat::ncol: %d \n",_ncol);
  // printf("ALMat::nele: %d \n",_nele);

  double melem=0;
  for(int i=0; i<_nrow; i++) {
    for(int j=0; j<_ncol; j++) {
      inmat.read((char*)&melem, sizeof (melem));
      *(ptr_data+i*_ncol+j) = melem;
      // printf("(%d,%d) = %.16lf \n",i,j,melem);
    }
  }

  inmat.close();
  return StatusCode::SUCCESS;
}

//______________________________________________________________________________
void AlMat::SetPathBin(const std::string &path)
{
  m_pathbin.assign(path);
}

//______________________________________________________________________________
void AlMat::SetPathTxt(const std::string &path)
{
  m_pathtxt.assign(path);
}

//______________________________________________________________________________
StatusCode AlMat::Write(const std::string &filename, bool binary, unsigned int precision)
{
  std::ofstream outmat;

  if(binary) {
    outmat.open((m_pathbin+filename).c_str(), std::ios::binary);
    if(outmat.fail())
      return StatusCode::FAILURE;

    int32_t mNrow = _nrow;
    outmat.write((char*)&mNrow, sizeof (mNrow));
    int32_t mNcol = _ncol;
    outmat.write((char*)&mNcol, sizeof (mNcol));
  }
  else {
    outmat.open((m_pathtxt+filename).c_str());
    if(outmat.fail())
      return StatusCode::FAILURE;
    outmat.setf(std::ios::fixed);
    if(precision>0)
      outmat.setf(std::ios::showpoint);
    outmat.precision(precision);
  }

  double melem=0;

  for( int i=0; i<_nrow; i++) {
    for( int j=0; j<_ncol; j++) {
      melem =  *(ptr_data+i*_ncol+j);
      if(binary)
        outmat.write((char*)&(melem), sizeof (melem));
      else
        outmat << " " << std::setw(12) << melem;
    }
    if(!binary)
      outmat << std::endl;
  }

  outmat.close();
  return StatusCode::SUCCESS;
}

//______________________________________________________________________________
const std::string AlMat::Print(const int NColsPerSet)
{
  std::ostringstream textmatrix;
  int FirstCol = 0;
  int LastCol = NColsPerSet;

  int NSets = (ncol()/NColsPerSet);
  if (ncol()%NColsPerSet>0) NSets++;
  textmatrix << std::endl;
  textmatrix << " ++++++++++++++ AlMat ++ "
        << nrow()
        << " x "
        << ncol()
        << " +++++++++++++++++++++++++++++"<< std::endl;
  for (int set=0; set < NSets; set++) {
    FirstCol = set*NColsPerSet;
    LastCol = (set+1)*NColsPerSet;
    if (LastCol>ncol()) LastCol = ncol();
    textmatrix << " |row\\col| ";
    for (int col=FirstCol; col < LastCol; col++) {
      textmatrix << std::setw(6) << col << " | ";
    }
    textmatrix << std::endl;
    textmatrix << " |-------|";
    for (int col=FirstCol; col < LastCol; col++) {
      textmatrix << "--------|";
    }
    textmatrix << std::endl;
    for (int row=0; row<nrow(); row++) {
      textmatrix << " |" << std::setw(6) << row << " | ";
      for (int col=FirstCol; col<LastCol; col++) {
    double melem =  *(ptr_data+row*ncol()+col);
    textmatrix << std::setprecision(5) << std:: setw(6) << melem << " | ";
      }
      textmatrix << std::endl;
    }
    if (set != (NSets-1)) textmatrix << std::endl;
  }
  textmatrix << " +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"<< std::endl;
  return textmatrix.str();
}

} // end namespace Trk

