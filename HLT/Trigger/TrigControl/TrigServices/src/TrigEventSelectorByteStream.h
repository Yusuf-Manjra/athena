/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGEVENTSELECTORBYTESTREAM_H
#define TRIGEVENTSELECTORBYTESTREAM_H

#include "GaudiKernel/IEvtSelector.h"
#include "GaudiKernel/Service.h"

// Forward declarations
class ByteStreamInputSvc;

/**
 * @class  TrigEventSelectorByteStream
 * @brief  online implementation of IEvtSelector for ByteStream
 */

class TrigEventSelectorByteStream : public Service,
                                    virtual public IEvtSelector {
public:
  /**
   * @class TrigEventSelectorByteStream::Context
   * @brief Event Selector context for TrigEventSelectorByteStream
   */
  class Context : public IEvtSelector::Context {
  public:
    /// Constructor from a selector
    Context(const IEvtSelector* selector);
    /// Copy constructor
    Context(const TrigEventSelectorByteStream::Context& other);
    /// Default destructor
    virtual ~Context();
    /// Implementation of IEvtSelector::Context::identifier
    virtual void* identifier() const override;
  private:
    /// pointer to the event selector
    const IEvtSelector* m_evtSelector;
  };

  /// Standard constructor
  TrigEventSelectorByteStream(const std::string& name, ISvcLocator* svcLoc);
  /// Standard destructor
  virtual ~TrigEventSelectorByteStream();

  // ------------------------- IInterface methods -----------------------------
  virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface) override;

  // ------------------------- Service methods --------------------------------
  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;

  // ------------------------- IEvtSelector methods ---------------------------
  virtual StatusCode createContext(IEvtSelector::Context*& c) const override;
  virtual StatusCode next(IEvtSelector::Context& c) const override;
  virtual StatusCode next(IEvtSelector::Context& c, int jump) const override;
  virtual StatusCode previous(IEvtSelector::Context& c) const override;
  virtual StatusCode previous(IEvtSelector::Context& c, int jump) const override;
  virtual StatusCode last(IEvtSelector::Context& refContext) const override;
  virtual StatusCode rewind(IEvtSelector::Context& c) const override;
  virtual StatusCode createAddress(const IEvtSelector::Context& c, IOpaqueAddress*& iop) const override;
  virtual StatusCode releaseContext(IEvtSelector::Context*& c) const override;
  virtual StatusCode resetCriteria(const std::string& cr, IEvtSelector::Context& c) const override;

private:
  // @property Reference to the event source (ByteStreamInputSvc)
  ServiceHandle<ByteStreamInputSvc> m_eventSource;
};

#endif // TRIGEVENTSELECTORBYTESTREAM_H