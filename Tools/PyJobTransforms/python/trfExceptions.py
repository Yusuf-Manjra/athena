# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @Package PyJobTransforms.trfExceptions
#  @brief Standard exceptions thrown by transforms
#  @author atlas-comp-transforms-dev@cern.ch
#  @version $Id: trfExceptions.py 570543 2013-11-14 21:52:16Z graemes $

import unittest

import logging
msg = logging.getLogger(__name__)



## Base class for transform exceptions
class TransformException(Exception):
    def __init__(self, errCode, errMsg):
        self._errCode = errCode
        self._errMsg = errMsg
                
    def __str__(self):
        return "%s (Error code %d)" % (self._errMsg, self._errCode)
    
    @property
    def errCode(self):
        return self._errCode
    
    @property
    def errMsg(self):
        return self._errMsg
    
    ## @brief Have a setter for @c errMsg as sometimes we want to update this with more
    #  information
    @errMsg.setter
    def errMsg(self, value):
        self._errMsg = value


## Group of argument based exceptions
class TransformArgException(TransformException):
    pass

## Setup exceptions
class TransformSetupException(TransformException):
    pass

## Logfile  exceptions
class TransformLogfileException(TransformException):
    pass

## Group of validation exceptions
class TransformValidationException(TransformException):
    pass

## Group of validation exceptions
class TransformReportException(TransformException):
    pass

## Exception class for validation failures detected by parsing logfiles
class TransformLogfileErrorException(TransformValidationException):
    pass

## Base class for execution exceptions
class TransformExecutionException(TransformException):
    pass

## Base class for file merging exceptions
class TransformMergeException(TransformExecutionException):
    pass

## Exception for problems finding the path through the graph
class TransformGraphException(TransformExecutionException):
    pass

## Transform internal errors
class TransformInternalException(TransformException):
    pass

## Exception used by time limited executions
class TransformTimeoutException(TransformException):
    pass

## Exception used by metadata functions
class TransformMetadataException(TransformException):
    pass

## Exception used by configuration via AMI tags
class TransformAMIException(TransformException):
        pass

    
