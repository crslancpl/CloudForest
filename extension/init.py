from CloudForestPy import EditArea
# from extension import testlsp
# language server cannot find the real CloudForest since
# the module is only available inside CloudForest

import sys
import os

currentdirectory = os.path.dirname(os.path.abspath(__file__))

parentdir = os.path.dirname(currentdirectory)
sys.path.insert(0, parentdir)

from extension import testlsp

import os
print(os.getcwd())

EditArea.test()

def NewEACreated(file):
    print(file+" created")
    EditArea.addcallback(file, "TEXTCHANGED", "textchanged")
    if(file != "New File"):
        # testlsp.startlsp(file, "python")
        return

def textchanged(file, cursorline, cursorpos):
    text = EditArea.getcontent(file)
    print(str(cursorline) + " " + str(cursorpos))
    print(text)

EditArea.addcallback(None, "OPENNEW", "NewEACreated")
