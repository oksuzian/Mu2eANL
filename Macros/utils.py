#!/usr/bin/env python
# -*- coding: utf-8 -*-
import math
import pandas

class Utils:
  
  def __init__(self):
    self.LIVETIME_LO=1.36E8+5.09E7
    self.LIVETIME_HI=5.49E6 
    self.LIVETIME_MU2E=3.46E6  

  def getNormBackground(self, df, cuts, scale):

  # scale - to account for a larger momentum window 

    raw_count_hi = len(df.query('is_high==1 & %s' % cuts))
    raw_count_lo = len(df.query('is_high==0 & %s' % cuts))

    ave_hi = raw_count_hi/self.LIVETIME_HI*self.LIVETIME_MU2E/scale
    ave_lo = raw_count_lo/self.LIVETIME_LO*self.LIVETIME_MU2E/scale
    ave = ave_hi + ave_lo
  
    err_hi = math.sqrt(raw_count_hi)/self.LIVETIME_HI*self.LIVETIME_MU2E/scale
    err_lo = math.sqrt(raw_count_lo)/self.LIVETIME_LO*self.LIVETIME_MU2E/scale
    err = math.sqrt(err_hi**2 + err_lo**2)

    return (ave, err, ave_hi, ave_lo, err_hi, err_lo)
  
  def pritnLiveTimes(self):
    print("LIVETIME_MU2E: %e" % self.LIVETIME_MU2E)
    print("LIVETIME_HI: %e" % self.LIVETIME_HI)
    print("LIVETIME_LO: %e" % self.LIVETIME_LO)