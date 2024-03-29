#!/usr/bin/env python
# Copyright (C) 2002-2023 CERN for the benefit of the ATLAS collaboration
#
# art-description: athenaHLT test of the Dev_pp_run3_v1 menu imitating partition with preloaded data at P1
# art-type: build
# art-include: master/Athena
# art-include: 23.0/Athena

from TrigP1Test.PreloadTest import test_trigP1_preload
import sys

sys.exit(test_trigP1_preload('Dev_pp_run3_v1').run())
