# this is the template function definition for
# EditArea's module.
from typing import Optional
from typing_extensions import Callable


class EditArea():
    def test(self):
        pass

    def getfilepath(self)->str:
        return "filepath"

    def getcontent(self)->str:
        return "content"

    def setlanguage(self, language:str):
        pass

    # evoke callback when the event occurs
    # for the edit area that is editing this file

    def addcallback(self, type:str, func:Callable):
        # type should be one of the Events:
        # "REQUESTCOMPLETION", "TEXTCHANGED"
        pass

    def addsuggestion(self, text, label, startline, startpos, endline, endpos):
        pass

    def hidesuggestion(self):
        pass

    def showsuggestion(self):
        pass

    def clearsuggestion(self):
        pass


def addcallback(type:str, func:Callable):
    pass
