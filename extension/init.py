# language server cannot find the real CloudForest since
# the module is only available inside CloudForest

# Make the app directory visible
import os
import sys

currentdirectory = os.path.dirname(os.path.abspath(__file__))
parentdir = os.path.dirname(currentdirectory)
sys.path.insert(0, parentdir)
