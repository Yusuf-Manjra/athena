# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from MuonRecExample.MuonRecUtils import logMuon
logMuon.warning("MuonRecExample.MuonCablingConfig is deprecated. Use instead: import MuonCnvExample.MuonCablingConfig")
import MuonCnvExample.MuonCablingConfig  # noqa: F401
