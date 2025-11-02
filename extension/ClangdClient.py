from extension.CloudForestBuiltIn.LSPClientClass import LSPServer
from extension.CloudForestPy.EditAreaMod import EditArea

working = False


def start():
    global working
    working = True
    global Server
    Server = LSPServer("clangd", "cpp")
    Server.Start()


def Listen(ea: EditArea):
    global working
    if not working:
        start()

    ea.setlanguage("cpp")
    filepath = ea.getfilepath()
    text = ea.getcontent()
    Server.OpenFile(filepath, text)
    ea.addcallback("TEXTCHANGED", textchanged)
    ea.addcallback("REQUESTCOMPLETION", completionrequested)
    pass


def textchanged(ea: EditArea):
    # get the text for didChange message
    text = ea.getcontent()
    global Server

    filepath = ea.getfilepath()
    Server.ChangeText(filepath, text)


def completionrequested(ea: EditArea, cursorline, cursorpos):
    global Server
    if Server is None:
        return
    Server.AutoComplete(ea, cursorline - 1, cursorpos)
