# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

def createAlgorithm( typeName, instanceName ):
    """Create an algorithm configurable

    This function is used to create an algorithm "configurable" in a dual-use
    way, either returning an actual Athena configurable, or an appropriately
    configured EL::AnaAlgorithmConfig instance.

    Keyword arguments:
      typeName     -- The C++ type name of the algorithm
      instanceName -- The instance name of the algorithm to create
    """

    try:
        # Try to get a configurable for this C++ class "from Athena".
        # If this succeeds, we're obviously in an Athena environment.

        # First off, construct a "python type name" for the class, replacing the
        # '::' namespace delimeters with '__'.
        pythonTypeName = typeName.replace( '::', '__' )

        # Now look up the Athena configurable of this algorithm:
        from AthenaCommon import CfgMgr
        algClass = getattr( CfgMgr, pythonTypeName )

        # Return the object:
        return algClass( instanceName )

    except ImportError:
        # If that didn't work, then apparently we're in an EventLoop
        # environment, so we need to use AnaAlgorithmConfig as the base class
        # for the user's class.
        from AnaAlgorithm.AnaAlgorithmConfig import AnaAlgorithmConfig
        return AnaAlgorithmConfig( '%s/%s' % ( typeName, instanceName ) )

    pass


def createPublicTool( typeName, toolName ):
    """Helper function for setting up a public tool for a dual-use algorithm

    This function is meant to be used in the analysis algorithm sequence
    configurations for setting up public tools on the analysis algorithms.
    Public tools that could then be configured with a syntax shared between
    Athena and EventLoop.

    Keyword arguments:
      typeName -- The C++ type name of the private tool
      toolName -- The property name with which the tool handle was declared on
                  the algorithm. Also the instance name of the tool.
    """

    try:
        # Try to set up a public tool of this type for Athena. If this succeeds,
        # we're obviously in an Athena environment.

        # First off, construct a "python type name" for the class, replacing the
        # '::' namespace delimeters with '__'.
        pythonTypeName = typeName.replace( '::', '__' )

        # Now look up the Athena configurable of this tool:
        from AthenaCommon import CfgMgr
        toolClass = getattr( CfgMgr, pythonTypeName )

        # Add an instance of the tool to the ToolSvc:
        from AthenaCommon.AppMgr import ToolSvc
        if not hasattr( ToolSvc, toolName ):
            ToolSvc += toolClass( toolName )
            pass

        # Return the member on the ToolSvc:
        return getattr( ToolSvc, toolName )

    except ImportError:
        # If that didn't work, then apparently we're in an EventLoop
        # environment, so let's use the EventLoop specific formalism.
        from AnaAlgorithm.AnaAlgorithmConfig import AnaAlgorithmConfig
        tool = AnaAlgorithmConfig( '%s/%s' % ( typeName, toolName ) )
        tool.setIsPublicTool( True )
        return tool


def addPrivateTool( alg, toolName, typeName ):
    """Helper function for declaring a private tool for a dual-use algorithm

    This function is meant to be used in the analysis algorithm sequence
    configurations for setting up private tools on the analysis algorithms.
    Private tools that could then be configured with a syntax shared between
    Athena and EventLoop.

    Keyword arguments:
      alg      -- The algorithm to set up the private tool on
      toolName -- The property name with which the tool handle was declared on
                  the algorithm. Also the instance name of the tool.
      typeName -- The C++ type name of the private tool
    """

    try:

        # First try to set up the private tool in an "Athena way".

        # Tokenize the tool's name. In case it is a subtool of a tool, or
        # something possibly even deeper.
        toolNames = toolName.split( '.' )

        # Look up the component that we need to set up the private tool on:
        component = alg
        for tname in toolNames[ 0 : -1 ]:
            component = getattr( component, tname )
            pass

        # Let's replace all '::' namespace delimeters in the type name
        # with '__'. Just because that's how the Athena code behaves...
        pythonTypeName = typeName.replace( '::', '__' )

        # Now look up the Athena configurable describing this tool:
        from AthenaCommon import CfgMgr
        toolClass = getattr( CfgMgr, pythonTypeName )

        # Finally, set up the tool handle property:
        setattr( component, toolNames[ -1 ], toolClass( toolNames[ -1 ] ) )

    except ( ImportError, AttributeError ) as e:

        # If that failed, then we should be in an EventLoop environment. So
        # let's rely on the standalone specific formalism for setting up the
        # private tool.
        alg.addPrivateTool( toolName, typeName )
        pass

    return
