from extension.CloudForestBuiltIn.LSPClientClass import LSPServer
from extension.CloudForestPy.EditAreaMod import EditArea

working = False


def start():
    global working
    working = True
    global Server
    Server = LSPServer("typescript-language-server --stdio", "ts")
    Server.Read()
    # Server.Start()


def Listen(ea: EditArea):
    global working
    if not working:
        start()

    ea.setlanguage("typescript")
    ea.addcallback("TEXTCHANGED", textchanged)
    ea.addcallback("REQUESTCOMPLETION", completionrequested)
    pass


def textchanged(ea: EditArea):
    # get the text for didOpen message
    text = ea.getcontent()
    global Server
    filepath = ea.getfilepath()
    Server.ChangeText(filepath, text)


def completionrequested(ea: EditArea, cursorline: int, cursorpos: int):
    Server.AutoComplete(ea, cursorline - 1, cursorpos)
