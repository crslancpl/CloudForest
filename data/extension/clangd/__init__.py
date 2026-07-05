from cloudforest import language

from pythonscripts.lsp.lsp_client_class import LspClient


def language_used(lang: str):
    command = ["clangd"]
    client = LspClient(command, "C++", "cpp", False)
    if not client.start():
        print("cannot find command")


language.listen_language_used("C++", language_used)
