import CloudForestPy
# from extension import testlsp
# language server cannot find the real CloudForest since
# the module is only available inside CloudForest

# Make the app directory visible
import sys
import os

currentdirectory = os.path.dirname(os.path.abspath(__file__))
parentdir = os.path.dirname(currentdirectory)
sys.path.insert(0, parentdir)

def NewEACreated(ea:CloudForestPy.EditArea):
    text = ea.getcontent()
    print(text)

CloudForestPy.AddCallback("NEWEDITAREA",NewEACreated)
