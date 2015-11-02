/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKALGS_ALVEC_H
#define TRKALGS_ALVEC_H

// PBdR (17Apr2007)

#include <iostream>
#include <map>
#include <vector>

class StatusCode;

namespace Trk {

class AlSymMatBase;
class AlSymMat;
class AlMat;
class AlSpaMat;

class AlVec {
 public:
  AlVec(int N);
  AlVec();
  AlVec(const AlVec& v);

  ~AlVec();

  inline double& operator[](int i);
  inline const double& operator[](int i) const;

  AlVec& operator=(const AlVec& v);
  AlVec& operator=(const double& v);
  AlVec  operator+(const AlVec&) const;
  AlVec& operator+=(const AlVec&);
  AlVec  operator-(const AlVec&) const;
  AlVec& operator-=(const AlVec&);
  double operator*(const AlVec&) const;
  AlVec  operator*(const AlMat&) const;
  AlVec  operator*(const AlSymMatBase&) const;
  AlVec  operator*(const double&) const;
  AlVec& operator*=(const double&);

  double norm() const;

  void reSize(int);
  void RemoveModule(int);
  void RemoveAlignPar(int, int);
  int  RemoveElements(std::vector<int>);
  void SetPathBin(const std::string&);
  void SetPathTxt(const std::string&);
  StatusCode Write(const std::string&, bool, double, std::map<int,unsigned long long>, float);
  StatusCode WritePartial(const std::string&, bool, double, std::map<int,unsigned long long>, float);
  StatusCode WritePartial(const std::string&, bool, double, std::map<int,std::string>, float);
  StatusCode InitializeOutputVector(const std::string&, bool, double, float, std::ofstream&);

  StatusCode CheckVecVersion(const std::string, bool&);
  StatusCode Read(const std::string&, double&, std::map<int,unsigned long long>&, float&);
  StatusCode ReadPartial(const std::string&, double&, std::map<int,unsigned long long>&, float&);
  StatusCode ReadProjected(const std::string&, double&, std::map<int,unsigned long long>&, float&);
  StatusCode ReadScalaPack(const std::string&);
  StatusCode WriteEigenvalueVec(const std::string &, bool);
  inline int size() const;
  inline double* ptrData() const;

 protected:
  int _size;
  double* ptr_data;
  std::string m_pathbin;
  std::string m_pathtxt;

  void copy(const AlVec&);

  //friend class AlSymMatBase;
  //friend class AlSymMat;
  //friend class AlSpaMat;
  //friend class AlMat;
};

// inline methods:

inline int AlVec::size() const {
  return _size;
}

inline double* AlVec::ptrData() const {
  return ptr_data;
}

inline double& AlVec::operator[](int i) {
  if( i < 0 ) {
    std::cerr << "AlVec: Index < zero! " << std::endl;
    return ptr_data[0];
  }

  if( i >= _size ) {
    std::cerr << "AlVec: Index too large! " << std::endl;
    return ptr_data[0];
  }

  return *(ptr_data+i);
}

inline const double& AlVec::operator[](int i) const {
  if( i < 0 ) {
    std::cerr << "AlVec: Index < zero! " << std::endl;
    return ptr_data[0];
  }

  if( i >= _size ) {
    std::cerr << "AlVec: Index too large! " << std::endl;
    return ptr_data[0];
  }

  return *(ptr_data+i);
}

} // end namespace Trk

#endif // TRKALGS_ALVEC_H
