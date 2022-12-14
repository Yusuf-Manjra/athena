# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from D3PDMakerCoreComps.D3PDObject import D3PDObject
import D3PDMakerCoreComps

from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
from AthenaPython import PyAthena

from D3PDMakerReader.D3PDMakerReaderConf import D3PD__MultiReaderAlg

from AthenaCommon.Logging import logging
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

##
# A class very similar to D3PDMakerCoreComps.MakerAlg, but it creates a configured
# version of the D3PD::MultiReaderAlg. Can be used directly as if it were a MakerAlg
# object.
class MultiReaderAlg( D3PD__MultiReaderAlg ):

    def __init__( self,
                  name,
                  seq = topSequence,
                  tuplename = None,
                  preD3PDAlgSeqName = D3PDMakerFlags.PreD3PDAlgSeqName(),
                  **kwargs ):

        self.__logger = logging.getLogger( "MultiReaderAlg" )

        # Work around initialization order issue.
        seq.__iadd__( D3PDMakerCoreComps.DummyInitAlg( name + 'DummyInit' ),
                      index = 0 )

        # tuple name defaults to the algorithm name.
        if tuplename is None:
            tuplename = name

        # Create the algorithm Configurable.
        D3PD__MultiReaderAlg.__init__ ( self, name,
                                        TupleName = tuplename,
                                        **kwargs )

        # Add to the supplied sequence.
        if seq:
            # But first, add a sequence for algorithms that should run
            # before D3PD making, if it's not already there.
            preseq = AlgSequence( preD3PDAlgSeqName )
            if not hasattr( seq, preD3PDAlgSeqName ):
                seq += [ preseq ]

            # We don't want to do filtering in the presequence.
            preseq.StopOverride = True
            # Now set up another sequence for filtering.
            # Unlike the presequence, there should be a unique one of these
            # per algorithm.  We also need to break out an additional
            # sequence to which users can add, and to wrap the whole
            # thing in a sequence to prevent a failed filter
            # decision from stopping other algorithms.
            # Like this:
            #
            #   ALG_FilterAlgorithmsWrap (StopOverride = True)
            #     ALG_FilterAlgorithmsHolder
            #       ALG_FilterAlgorithms
            #       ALG
            #     Dummy alg, to reset filter flag
            suffix = D3PDMakerFlags.FilterAlgSeqSuffix()
            wrap = AlgSequence( name + suffix + 'Wrap',
                                StopOverride = True )
            holder = AlgSequence( name + suffix + 'Holder' )
            self.filterSeq = AlgSequence( name + suffix )
            holder += self.filterSeq
            holder += self
            wrap += holder
            wrap += PyAthena.Alg( name + 'Dummy' )
           
            seq += wrap

        # Create a unique collection getter registry tool for this tree.
        from AthenaCommon.AppMgr import ToolSvc
        self._registry = \
           D3PDMakerCoreComps.CollectionGetterRegistryTool (self.name() +
                                                   '_CollectionGetterRegistry')
        ToolSvc += self._registry

        return

    def __iadd__( self, config ):
        """Add a new IObjFillerTool to a tree."""

        nchild = len( self )
        if not isinstance( config, list ):
            # Check if this is a duplicate:
            if hasattr( self, config.getName() ):
                self.__logger.warning( "Duplicate D3PDObject with name \"%s\" ignored", config.ObjectName )
                return self
            # If not then add it to the algorithm:
            self.__logger.info( "Adding D3PDObject with name \"%s\" and prefix \"%s\"",
                                config.ObjectName, config.Prefix )
            self.Tools      += [ config ]
            self.Prefixes   += [ config.Prefix ]
            self.ClassNames += [ config.ObjectName ]
            config = [ config ]
        else:
            for c in config:
                self.__iadd__( c )
                pass
            return self

        super( MultiReaderAlg, self ).__iadd__( config )

        # Rescan all children to set the proper collection getter registry.
        self._setRegistry( self )

        # Execute the hooks as well. They are needed after all...
        for c in self.getChildren()[ nchild : ]:
            D3PDObject.runHooks( c )
            pass

        return self

    def _setRegistry( self, conf ):
        """Scan CONF and all children to set the proper
        collection getter registry for this tree.
        """
       
        if 'CollectionGetterRegistry' in conf.properties():
            conf.CollectionGetterRegistry = self._registry
        for c in conf.getAllChildren():
            self._setRegistry( c )

        return
