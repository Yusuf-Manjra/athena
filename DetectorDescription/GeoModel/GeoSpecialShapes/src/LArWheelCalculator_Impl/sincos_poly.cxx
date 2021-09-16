/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoSpecialShapes/LArWheelCalculator.h"

#include "CxxUtils/sincos.h"

#include "CLHEP/Units/SystemOfUnits.h"

#include "TDecompLU.h"
#include "TDecompSVD.h"
#include "TMath.h"
#include "TMatrixD.h"
#include "TMatrixDLazy.h"
#include "TVectorD.h"

#include <iomanip>
#include <iostream>
#include <math.h>
#include <mutex>
#include <sys/time.h>

#define DEBUGPRINT 0

template<typename T>
std::ostream&
operator<<(std::ostream& ostr, const TVectorT<T>& v)
{
  std::ios_base::fmtflags save_flags(ostr.flags());
  ostr << '[';
  ostr << std::scientific;
  for (Int_t idx = v.GetLwb(); idx < v.GetUpb(); idx++) {
    ostr << v[idx] << ", ";
  }
  ostr << v[v.GetUpb()];
  ostr << ']';
  ostr.flags(save_flags);
  return ostr;
}

// find best approximation of y values using linear combination of basis
// functions in bf
static TVectorD
findLinearApproximation(const Int_t dataLen,
                        const Int_t nBasisFuntions,
                        const TVectorD& y,
                        const TMatrixD& bf)
{
  TMatrixDSym A(nBasisFuntions);
  TVectorD vY(nBasisFuntions);

  for (Int_t j = 0; j < nBasisFuntions; ++j) {
    for (Int_t k = 0; k < nBasisFuntions; ++k) {
      Double_t Ajk = 0.0;
      for (Int_t i = 0; i < dataLen; ++i) {
        Ajk += bf(j, i) * bf(k, i);
      }
      A(j, k) = Ajk;
    }
  }

  for (Int_t k = 0; k < nBasisFuntions; ++k) {
    Double_t vYk = 0.0;
    for (Int_t i = 0; i < dataLen; ++i) {
      vYk += y[i] * bf(k, i);
    }
    vY[k] = vYk;
  }

  TMatrixDSym Ainv(A);
  Ainv.Invert();
  return Ainv * vY;
}

using namespace CLHEP;

void
LArWheelCalculator::fill_sincos_parameterization()
{
  const Int_t nrPolyDegree = LARWC_SINCOS_POLY;
#if LARWC_SINCOS_POLY > 4 && DEBUGPRINT
  std::cout << "LARWC_SINCOS_POLY: " << LARWC_SINCOS_POLY << std::endl;
#endif
  const Int_t nBasisFunctions = nrPolyDegree + 1;

  // We compute the polynomial approximations once per side, and store them in
  // the static variables below for reuse in successive calculator instances.
  // For multi-threading, then, this code needs to be mutex locked.
  // FIXME: this could done in a cleaner way.
  static std::mutex fillParamMutex;
  std::lock_guard<std::mutex> lock(fillParamMutex);

  static bool filled[2] = { false, false };
  static double sin_parametrization[2][nBasisFunctions];
  static double cos_parametrization[2][nBasisFunctions];

  // Reuse the computation if already performed
  size_t S = m_isInner ? 0 : 1;
  if (filled[S]) {
    for (Int_t i = 0; i < nBasisFunctions; ++i) {
      m_sin_parametrization[i] = sin_parametrization[S][i];
      m_cos_parametrization[i] = cos_parametrization[S][i];
    }
    // To Miha not  sure this is the right order just assuem here
    // s4, s5, c4, c5
    // s2, s3, c2, c3
    // s0, s1, c0, c1
    m_vsincos_par.sincos_parametrization[0][0] = m_sin_parametrization[4];
    m_vsincos_par.sincos_parametrization[0][1] = m_sin_parametrization[5];
    m_vsincos_par.sincos_parametrization[0][2] = m_cos_parametrization[4];
    m_vsincos_par.sincos_parametrization[0][3] = m_cos_parametrization[5];
    m_vsincos_par.sincos_parametrization[1][0] = m_sin_parametrization[2];
    m_vsincos_par.sincos_parametrization[1][1] = m_sin_parametrization[3];
    m_vsincos_par.sincos_parametrization[1][2] = m_cos_parametrization[2];
    m_vsincos_par.sincos_parametrization[1][3] = m_cos_parametrization[3];
    m_vsincos_par.sincos_parametrization[2][0] = m_sin_parametrization[0];
    m_vsincos_par.sincos_parametrization[2][1] = m_sin_parametrization[1];
    m_vsincos_par.sincos_parametrization[2][2] = m_cos_parametrization[0];
    m_vsincos_par.sincos_parametrization[2][3] = m_cos_parametrization[1];
    return;
  }

  // const Double_t Rmin = m_isInner? 290.*mm: 600.*mm;
  // const Double_t Rmax = m_isInner? 710.*mm: 2050.*mm;
  const Double_t Rmin = m_isInner ? 250. * mm : 560. * mm;
  const Double_t Rmax = m_isInner ? 750. * mm : 2090. * mm;
  // const Double_t Rmin = m_isInner? 220.*mm: 530.*mm;
  // const Double_t Rmax = m_isInner? 780.*mm: 2120.*mm;
  const Double_t Rstep = 1. * mm;
  const Int_t nrPoints = (Rmax - Rmin) * (1. / Rstep);
  const Int_t dataLen = nrPoints + 1;

  TVectorD x(dataLen); // angle points
  TVectorD ysin(
    dataLen); // to be approximated function values at angle points - sin
  TVectorD ycos(
    dataLen); // to be approximated function values at angle points - cos
  TMatrixD bf(nBasisFunctions,
              dataLen); // Matrix of values of basis functions at angle points

  for (Int_t i = 0; i < dataLen; ++i) {
    const Double_t a = Rmin + i * Rstep;
    x[i] = a;
    CxxUtils::sincos scalpha(parameterized_slant_angle(a));
    ysin[i] = scalpha.sn;
    ycos[i] = scalpha.cs;
    for (Int_t n = 0; n < nBasisFunctions; ++n) {
      bf(n, i) = pow(a, n);
    }
  }

  TVectorD params_sin =
    findLinearApproximation(dataLen, nBasisFunctions, ysin, bf);
  TVectorD params_cos =
    findLinearApproximation(dataLen, nBasisFunctions, ycos, bf);

  for (Int_t i = 0; i < nBasisFunctions; ++i) {
    m_sin_parametrization[i] = params_sin[i];
    m_cos_parametrization[i] = params_cos[i];
    sin_parametrization[S][i] = params_sin[i];
    cos_parametrization[S][i] = params_cos[i];
  }

  // To Miha not  sure this is the right order just assuem here
  // s4, s5, c4, c5
  // s2, s3, c2, c3
  // s0, s1, c0, c1
  m_vsincos_par.sincos_parametrization[0][0] = m_sin_parametrization[4];
  m_vsincos_par.sincos_parametrization[0][1] = m_sin_parametrization[5];
  m_vsincos_par.sincos_parametrization[0][2] = m_cos_parametrization[4];
  m_vsincos_par.sincos_parametrization[0][3] = m_cos_parametrization[5];
  m_vsincos_par.sincos_parametrization[1][0] = m_sin_parametrization[2];
  m_vsincos_par.sincos_parametrization[1][1] = m_sin_parametrization[3];
  m_vsincos_par.sincos_parametrization[1][2] = m_cos_parametrization[2];
  m_vsincos_par.sincos_parametrization[1][3] = m_cos_parametrization[3];
  m_vsincos_par.sincos_parametrization[2][0] = m_sin_parametrization[0];
  m_vsincos_par.sincos_parametrization[2][1] = m_sin_parametrization[1];
  m_vsincos_par.sincos_parametrization[2][2] = m_cos_parametrization[0];
  m_vsincos_par.sincos_parametrization[2][3] = m_cos_parametrization[1];
  filled[S] = true;

  // FIXME: nothing below is needed unless debug printing

#if DEBUGPRINT
  std::cout << "sin params:" << params_sin << std::endl;
  std::cout << "cos params:" << params_cos << std::endl;

  double dsinr = 0., dcosr = 0.;
  double dtrigr = 0;
#endif

  double dsin = 0., dcos = 0.;
  double dtrig = 0.;
  for (double r = Rmin + 40.; r < Rmax - 40.; r += Rstep / 10.) {
    CxxUtils::sincos scalpha(parameterized_slant_angle(r));
    double sin_a, cos_a;
    double sin_a_v, cos_a_v;
    parameterized_sincos(r, sin_a, cos_a);
    m_vsincos_par.eval(r, sin_a_v, cos_a_v);
    std::cout << "default: " << r << " " << sin_a << " " << cos_a << std::endl;
    std::cout << "vec: " << r << " " << sin_a_v << " " << cos_a_v << std::endl;
    double ds = fabs(scalpha.sn - sin_a);
    if (ds > dsin) {
      dsin = ds;
#if DEBUGPRINT
      dsinr = r;
#endif
    }
    double dc = fabs(scalpha.cs - cos_a);
    if (dc > dcos) {
      dcos = dc;
#if DEBUGPRINT
      dcosr = r;
#endif
    }
    double dt = fabs(sin_a * sin_a + cos_a * cos_a - 1.);
    if (dt > dtrig) {
      dtrig = dt;
#if DEBUGPRINT
      dtrigr = r;
#endif
    }
  }

#if DEBUGPRINT
  std::cout << "Max. difference: " << std::endl
            << "\tsin: " << dsin << " at " << dsinr << std::endl
            << "\tcos: " << dcos << " at " << dcosr << std::endl
            << "\tsin^2+cos^2: " << dtrig << " at " << dtrigr << std::endl;
#endif

#ifdef HARDDEBUG
  TVectorD y_test(dataLen);
  const Int_t nIter = 10000;
  std::cout << "Perfomance test started, " << nIter << " iterations"
            << std::endl;

  double y_testsin[dataLen];
  double y_testcos[dataLen];
  struct timeval tvsincos_start, tvsincos_stop;
  gettimeofday(&tvsincos_start, 0);
  for (Int_t iIter = 0; iIter < nIter; iIter++) {
    for (Int_t i = 0; i < dataLen; i++) {
      sincos(parameterized_slant_angle(x[i]), &y_testsin[i], &y_testcos[i]);
    }
  }
  gettimeofday(&tvsincos_stop, 0);
  double timeSinCos =
    (tvsincos_stop.tv_sec - tvsincos_start.tv_sec +
     1E-6 * (tvsincos_stop.tv_usec - tvsincos_start.tv_usec)) /
    nIter;

  std::cout.unsetf(std::ios::fixed | std::ios::scientific);
  std::cout << "Time to fill 2x" << dataLen
            << " elements using sincos function: " << timeSinCos << std::endl;

  struct timeval tvpoly_start, tvpoly_stop;
  gettimeofday(&tvpoly_start, 0);
  for (Int_t iIter = 0; iIter < nIter; iIter++) {
    for (Int_t i = 0; i < dataLen; i++) {
      parameterized_sincos(x[i], y_testsin[i], y_testcos[i]);
    }
  }
  gettimeofday(&tvpoly_stop, 0);
  double timePoly = (tvpoly_stop.tv_sec - tvpoly_start.tv_sec +
                     1E-6 * (tvpoly_stop.tv_usec - tvpoly_start.tv_usec)) /
                    nIter;
  std::cout << "Time to fill 2x" << dataLen
            << " elements using approximation sin&cos: " << timePoly
            << std::endl;
  std::cout.unsetf(std::ios::fixed | std::ios::scientific);
  std::cout << "Approximation is " << timeSinCos / timePoly << " faster "
            << std::endl;
#endif
}
