#Get the list of plot file names in a directory and make a LaTex script-ready list of them

import os
import sys

cumulative = ""
for file in os.listdir("../Macros/standardizedPlots/Unmixed/Negative"):
    comps = file.split("_")
    str = ""
    for i in range(len(comps)-1):
        str += comps[i] + "\_"
    str += comps[len(comps) - 1][:-4]
    if str.find("cut") >= 0:
        continue
    cumulative += str + ", "
        
print cumulative[:-2]
