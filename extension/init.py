from CloudForestPy import EditAreaMod
# from extension import testlsp
# language server cannot find the real CloudForest since
# the module is only available inside CloudForest

# Make the app directory visible
import sys
import os

currentdirectory = os.path.dirname(os.path.abspath(__file__))
parentdir = os.path.dirname(currentdirectory)
sys.path.insert(0, parentdir)

from extension import LSPClient

def EATextChanged(ea:EditAreaMod.EditArea, cursorline, cursorpos):
    text = ea.getcontent()
    print(text)

def NewEACreated(ea:EditAreaMod.EditArea):
    text = ea.getcontent()
    print(text)
    ea.addcallback("TEXTCHANGED",EATextChanged)

LSPClient.StartListenEditAreas()
#EditAreaMod.addcallback("NEWEDITAREA", NewEACreated)
