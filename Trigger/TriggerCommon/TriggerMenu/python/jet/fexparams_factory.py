# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

""" factory function to return the objects that carry fex args
according to the fex class. """

def fexparams_factory(selector, kargs):

    if selector == 'antikt':
        return _AntiKtParams(kargs)

    if selector == 'cone':
        return _ConeParams(kargs)

    if selector == 'jetrec':
        return _JetRecParams(kargs)

    if selector == 'jetrec_recluster':
        return _JetRecReclusterParams(**kargs)

    else:
        raise RuntimeError('fexarfs_factory: unknow fex selector %s' % selector)


class _JetFexParams(object):
    """Common code for Fexes: check sanity of input parameters.
    Provide an alg factory parameter.

    26/2/2014: with the current simple list (in constrast with 2012)
    the fexes are so similar the class heirarchy is not yet useful.
    It may become so if the variety of Fex algorithms increases."""

    def __init__(self,
                 merge_param,
                 jet_calib,
                 fex_label,
                 data_type,
                 fex_alg_name,
                 **kargs):

        self._check_args(merge_param)
        self.merge_param = merge_param
        self.data_type = data_type  # input type: TT, tc etc
        # jet_calib:
        # string used by offline to determine which calibration to perform
        self.jet_calib = jet_calib 
        self.fex_label = fex_label
        self.fex_alg_name = fex_alg_name  # from input dictionary
        
    def __str__(self):
        s = ['%s: %s\n' % (k, str(v)) for k, v in self.__dict__.items()]
        return '\n'.join(s)

    def _check_args(self, merge_param):
        """check the constructor args"""

        try:
            merge_param = int(merge_param)
        except ValueError:
            m = '%s._check_args: non integer merge_param %s' % (
                self.__class__.__name__,
                str(merge_param))
            raise RuntimeError(m)

        if not merge_param in (2, 4, 10):
            m = '%s._check_args: unsupported merge_param %d' % (
                self.__class__.__name__,
                merge_param)
            raise RuntimeError(m)


class _AntiKtParams(_JetFexParams):
    """ Argument checking class that holds the  parameters for a
    AntiKt python alg"""

    fex_type = 'antikt'

    def __init__(self, kargs):
        _JetFexParams.__init__(self, **kargs)


class _ConeParams(_JetFexParams):
    """ Argument checking class that holds the  parameters for a
    Cone python alg"""

    fex_type = 'cone'

    def __init__(self, kargs):
        _JetFexParams.__init__(self, **kargs)


class _JetRecParams(_JetFexParams):
    """ Argument checking class that holds the  parameters for JetRec"""

    fex_type = 'jetrec'

    def __init__(self, kargs):
        _JetFexParams.__init__(self, **kargs)


class _JetRecReclusterParams(_JetFexParams):
    """ Argument checking class that holds the  parameters for JetRec"""

    fex_type = 'jetrec_recluster'

    def __init__(self, ptMinCut, etaMaxCut, **kargs):
        _JetFexParams.__init__(self, **kargs)
        self.ptMinCut = ptMinCut
        self.etaMaxCut = etaMaxCut
