///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// SignalStateHelper.cxx 
/////////////////////////////////////////////////////////////////// 

// EventKernel includes
#include "EventKernel/SignalStateHelper.h"

//*****************************************************************
//
// SignalStateHelper
//
//*****************************************************************


/** Empty Constructor */
SignalStateHelper::SignalStateHelper():
  m_originalState(P4SignalState::UNKNOWN),
  m_currentState(P4SignalState::UNKNOWN),
  m_object(0)
{}

/** Constructor with initialization */
SignalStateHelper::SignalStateHelper(ISignalState* theObject):
  m_originalState(P4SignalState::UNKNOWN),
  m_currentState(P4SignalState::UNKNOWN),
  m_object(0)
{  
  controlObject(theObject);
}

SignalStateHelper::SignalStateHelper(P4SignalState::State s):
  m_originalState(P4SignalState::UNKNOWN),
  m_currentState(s),
  m_object(0)
{  
  setSignalState(s);
}

SignalStateHelper::SignalStateHelper(ISignalState* theObject, P4SignalState::State s):
  m_originalState(P4SignalState::UNKNOWN),
  m_currentState(s),
  m_object(theObject)
{  
  if( theObject != NULL ){
    m_originalState = theObject->signalState();  
    m_object->setSignalState(m_currentState);
  }
}

SignalStateHelper::~SignalStateHelper()
{
  releaseObject();
}

/** Change the object controlled */
bool SignalStateHelper::controlObject(ISignalState* theObject)
{
  releaseObject();
  m_originalState = theObject->signalState();
  m_object = theObject;
  if(m_currentState!=P4SignalState::UNKNOWN)
    m_object->setSignalState(m_currentState);
  return true;
}

bool SignalStateHelper::setSignalState(P4SignalState::State s)
{
  m_currentState = s;
  // If we do have an object attached, switch its signal state
  if(m_object != 0 && m_currentState!=P4SignalState::UNKNOWN)
    {
      m_object->setSignalState(m_currentState);
      return true;
    }
  return false;
}

bool SignalStateHelper::releaseObject()
{
  // If we have an object attached, revert and remove
  if(m_object!=0)
    {
      m_object->setSignalState(m_originalState);
      m_object=0;
      return true;
    }
  return false;
}

bool SignalStateHelper::resetSignalState()
{
  if(m_object!=0)
    {
      m_currentState = m_originalState;
      m_object->setSignalState(m_originalState);
      return true;
    }
  return false;
}

//*****************************************************************
//
// SignalStateConstHelper
//
//*****************************************************************


/** Empty Constructor */
SignalStateConstHelper::SignalStateConstHelper():
  m_originalState(P4SignalState::UNKNOWN),
  m_currentState(P4SignalState::UNKNOWN),
  m_object(0)
{}

/** Constructor with initialization */
SignalStateConstHelper::SignalStateConstHelper(const ISignalState* theObject):
  m_originalState(P4SignalState::UNKNOWN),
  m_currentState(P4SignalState::UNKNOWN),
  m_object(0)
{  
  controlObject(theObject);
}

SignalStateConstHelper::SignalStateConstHelper(P4SignalState::State s):
  m_originalState(P4SignalState::UNKNOWN),
  m_currentState(s),
  m_object(0)
{  
  setSignalState(s);
}

SignalStateConstHelper::SignalStateConstHelper(const ISignalState* theObject, P4SignalState::State s):
  m_originalState(P4SignalState::UNKNOWN),
  m_currentState(s),
  m_object(const_cast<ISignalState*>(theObject))
{
  if( theObject != NULL ){
    m_originalState = theObject->signalState();  
    m_object->setSignalState(m_currentState);
  }
}

SignalStateConstHelper::~SignalStateConstHelper()
{
  releaseObject();
}

/** Change the object controlled */
bool SignalStateConstHelper::controlObject(const ISignalState* theObject)
{
  releaseObject();
  m_originalState = theObject->signalState();
  m_object = const_cast<ISignalState*>(theObject);
  if(m_currentState!=P4SignalState::UNKNOWN)
    m_object->setSignalState(m_currentState);
  return true;
}

bool SignalStateConstHelper::setSignalState(P4SignalState::State s)
{
  m_currentState = s;
  // If we do have an object attached, switch its signal state
  if(m_object != 0 && m_currentState!=P4SignalState::UNKNOWN)
    {
      m_object->setSignalState(m_currentState);
      return true;
    }
  return false;
}

bool SignalStateConstHelper::releaseObject()
{
  // If we have an object attached, revert and remove
  if(m_object!=0)
    {
      m_object->setSignalState(m_originalState);
      m_object=0;
      return true;
    }
  return false;
}

bool SignalStateConstHelper::resetSignalState()
{
  if(m_object!=0)
    {
      m_currentState = m_originalState;
      m_object->setSignalState(m_originalState);
      return true;
    }
  return false;
}

