from CloudForestPy import EditAreaMod

from extension import ClangdClient, PyrightClient, TypescriptClient
from extension.CloudForestBuiltIn.LSPClientClass import LSPServer


def NewEACreated(ea: EditAreaMod.EditArea):
    pass


def EAOpen(ea: EditAreaMod.EditArea):
    print("open")
    filepath = ea.getfilepath()
    print(filepath)
    if filepath.endswith(".cpp") or filepath.endswith(".h"):
        ClangdClient.Listen(ea)
    elif filepath.endswith(".py"):
        ea.setlanguage("py")
        # PyrightClient.
        pass
    elif filepath.endswith(".js") or filepath.endswith(".ts"):
        # TypescriptClient.Listen(ea)
        pass
    elif filepath.endswith(".rs"):
        pass
    else:
        print("unknown")


def StartListenEditAreas():
    EditAreaMod.addcallback("NEWEDITAREA", EAOpen)


# StartListenEditAreas() is called by init.py
