# this is the template function definition for
# EditArea's module.
from typing import Optional
from enum import Enum

class EditArea():
    def test(self):
        pass

    def getcontent(self)->str:
        content = "content"
        return content

    def setlanguage(self, language:str):
        pass

    # evoke callback when the event occurs
    # for the edit area that is editing this file

    def addcallback(self, type:str, funcname:str):
        # type should be one of the Event
        # "OPENNEW", "TEXTCHANGED"
        pass

    def addsuggestion(self, text, label, startline, startpos, endline, endpos):
        pass



    def clearsuggestion(self):
        pass


def AddCallback(type:str, function):
    pass
