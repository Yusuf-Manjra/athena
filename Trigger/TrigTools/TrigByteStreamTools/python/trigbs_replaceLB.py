#!/usr/bin/env python

# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""\
Increment the LB number of the event. The modify() method will only be executed once
per event. Hence this module can be used by several other modules to increase lumiblocks.

Can be used as event modifier with athenaHLT -Z ... or as a standalone script.
"""

import eformat
from eformat import helper
import logging
import sys
from TrigByteStreamTools import CTPfragment

class Config:
  """Configuration options for this module"""
  eventsPerLB = 10            # events per LB
  
class Store:
  """Global variables"""
  eventCounter = 0
  currentLB = 1
  currentL1ID = None

log = logging.getLogger(__name__)
log.info('Increasing LB every %d events', Config.eventsPerLB)

def modify(event):

  if event.lvl1_id()==Store.currentL1ID:
    log.debug('Already executed once on this event')
    return event  
      
  Store.eventCounter += 1
  Store.currentL1ID = event.lvl1_id()
  if (Store.eventCounter % Config.eventsPerLB)==0:
    Store.currentLB +=1

  newevt = event if isinstance(event,eformat.write.FullEventFragment) else eformat.write.FullEventFragment(event)

  # Find CTP ROB
  ctp_robs = [rob for rob in newevt.children()
              if rob.source_id().subdetector_id() == helper.SubDetector.TDAQ_CTP]

  # Modify LB in CTP fragmnet
  for ctp_rob in ctp_robs:
    CTPfragment.setLumiBlock(ctp_rob, Store.currentLB)
    
  # Set LB in event object header
  newevt.lumi_block(Store.currentLB)
  
  log.info("Event %4d, L1ID %10d assigned LB = %d", Store.eventCounter, event.lvl1_id(), Store.currentLB)

  return newevt if isinstance(event,eformat.write.FullEventFragment) else newevt.readonly()


def main():
  from optparse import OptionParser

  parser = OptionParser(description=__doc__,
                        usage='%prog infile outfile')
  parser.add_option('-n', '--events', action='store', type='int', default=-1,
                    help='number of evnets to process')

  (opt, args) = parser.parse_args()
  
  if len(args)!=2:
    parser.print_help()
    return 1
    
  dr = eformat.EventStorage.pickDataReader(args[0])
  ostr = eformat.ostream(core_name = dr.fileNameCore(),
                         run_number = dr.runNumber(),
                         trigger_type = dr.triggerType(),
                         detector_mask = dr.detectorMask(),
                         beam_type = dr.beamType(),
                         beam_energy = dr.beamEnergy())

  bsfile = eformat.istream([args[0]])
  i = 0
  for event in bsfile:
    i +=1
    if opt.events>0 and i>opt.events:
      break
    ro_event = modify(event)
    rw_event = eformat.write.FullEventFragment(ro_event)
    ostr.write(rw_event)

  return

if __name__ == "__main__":
  sys.exit(main())
