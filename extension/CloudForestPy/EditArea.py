# this is the template function definition for
# EditArea's module.
from typing import Optional
from enum import Enum

def test():
    pass


def getcontent(filename:str):
    pass

def setlanguage(filename:str, language:str):
    pass

# evoke callback when the event occurs
# for the edit area that is editing this file

def addcallback(filename:Optional[str], type:str, funcname:str):
    # type should be one of the Event
    # "OPENNEW", "TEXTCHANGED"
    pass
