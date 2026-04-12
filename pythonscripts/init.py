# add this directory to system path make it visible
import os
import sys

currentdirectory = os.path.dirname(os.path.abspath(__file__))
parentdir = os.path.dirname(currentdirectory)
sys.path.insert(0, parentdir)
