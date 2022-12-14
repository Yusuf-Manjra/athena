# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#
# File: normphi.py
# Created: sss, 2004.
# Purpose: Normalize an angle to within the range -pi...pi.
#
# This is intended for use only when the initial angle is already
# close to being within the range.
#


from math import pi

def normphi (phi):
    """Normalize an angle to within the range -pi...pi.

This is intended for use only when the initial angle is already
close to being within the range.
"""
    
    while phi < -pi:
        phi += 2*pi
    while phi >  pi:
        phi -= 2*pi
    return phi


