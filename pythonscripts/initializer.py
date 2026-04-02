# add this directory to system path make it visible
import os
import sys


def initialize():
    currentdirectory = os.path.dirname(os.path.abspath(__file__))
    sys.path.insert(0, currentdirectory)
