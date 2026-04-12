from cloudforest.editarea import EditArea

from ..lsp_client_class import LspClient

Server = None
IsRunning = False


def Start():
    global IsRunning
    IsRunning = True
    global Server
    Server = LspClient("clangd", "cpp")
    Server.start()


def Listen(ea: EditArea):
    global IsRunning
    if not IsRunning:
        Start()
