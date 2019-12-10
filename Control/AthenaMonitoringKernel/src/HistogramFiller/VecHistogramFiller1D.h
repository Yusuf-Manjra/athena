/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/  
  
#ifndef AthenaMonitoringKernel_HistogramFiller_VecHistogramFiller1D_h
#define AthenaMonitoringKernel_HistogramFiller_VecHistogramFiller1D_h

#include "HistogramFiller1D.h"

namespace Monitored {
  class VecHistogramFiller1D : public HistogramFiller1D {
  public:
    VecHistogramFiller1D(const HistogramDef& definition, std::shared_ptr<IHistogramProvider> provider)
      : HistogramFiller1D(definition, provider) {}

    virtual VecHistogramFiller1D* clone() override {
      return new VecHistogramFiller1D( *this );
    }

    virtual unsigned fill() override {
      if (m_monVariables.size() != 1) {
        return 0;
      }

      auto histogram = this->histogram<TH1>();
      auto valuesVector = m_monVariables[0].get().getVectorRepresentation();
      std::lock_guard<std::mutex> lock(*(this->m_mutex));

      for (unsigned i = 0; i < std::size(valuesVector); ++i) {
        auto value = valuesVector[i];
        histogram->AddBinContent(i+1, value);
        histogram->SetEntries(histogram->GetEntries() + value);
      }

      return std::size(valuesVector);  
    }
  };
}

#endif /* AthenaMonitoringKernel_HistogramFiller_VecHistogramFiller1D_h */
