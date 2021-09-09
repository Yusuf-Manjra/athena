// Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

#ifndef JLARGERJETTOB_H
#define JLARGERJETTOB_H

#include <iostream>

#include "L1TopoEvent/BaseTOB.h"
#include "L1TopoEvent/Heap.h"


// TODO implement sizecheck lile in ClusterTOB

namespace TCS {
   
   class GenericTOB;
   
   class jLargeRJetTOB : public BaseTOB {
   public:
      
      // default constructor
      jLargeRJetTOB(uint32_t roiWord = 0, const std::string& tobName = "jLargeRJetTOB");

      // copy constructor
      jLargeRJetTOB(const jLargeRJetTOB & jet);

      // constructor with initial values
      jLargeRJetTOB(unsigned int Et, int eta, unsigned phi, uint32_t roiWord = 0, const std::string& tobName = "jLargeRJetTOB" );

      // destructor
      virtual ~jLargeRJetTOB();
      
      // accessors
      unsigned int energy() const { return m_Et; }
      unsigned int Et() const { return m_Et; }
      
      // accessors
      int eta() const { return m_eta; }
      unsigned phi() const { return m_phi; }
      
      double EtDouble() const { return m_EtDouble; }
      double etaDouble() const { return m_etaDouble; }
      double phiDouble() const { return m_phiDouble; }
      
      // setters
      void setEt(int energy) { m_Et = energy; }
      void setEta(int eta) { m_eta = eta; }
      void setPhi(int phi) { m_phi = phi; }
      
      void setEtDouble(double et) { m_EtDouble = et; }
      void setEtaDouble(double eta) { m_etaDouble = eta; }
      void setPhiDouble(double phi) { m_phiDouble = phi; }

      inputTOBType_t tobType() const { return JET; }

      static jLargeRJetTOB* createOnHeap(const jLargeRJetTOB& jet);
      static void clearHeap();

      static const Heap<TCS::jLargeRJetTOB>& heap() { return fg_heap; }

   private:

      static const unsigned int g_nBitsEt;
      static const unsigned int g_nBitsIsolation;
      static const unsigned int g_nBitsEta;
      static const unsigned int g_nBitsPhi;

      unsigned int m_Et { 0 };
      int m_eta { 0 };
      unsigned m_phi { 0 };

      double m_EtDouble { 0 };
      double m_etaDouble { 0 };
      double m_phiDouble { 0 };

      virtual void print(std::ostream &o) const;

      static thread_local Heap<TCS::jLargeRJetTOB> fg_heap;
   };
   
}

#endif
