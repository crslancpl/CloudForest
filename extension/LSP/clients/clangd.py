from extension.CloudForestMod.EditAreaMod import EditArea
from extension.LSP.LSPClient import LSPServer

Server = None
IsRunning = False


def Start():
    global IsRunning
    IsRunning = True
    global Server
    Server = LSPServer("clangd", "cpp")
    Server.Start()


def Listen(ea: EditArea):
    global IsRunning
    if not IsRunning:
        Start()
