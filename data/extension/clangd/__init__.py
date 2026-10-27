from cloudforest import language

from pythonscripts.lsp.lsp_client_class import LspClient

def language_used(lang: language.Language):
    command = ["clangd"]
    client = LspClient(command, lang, False)
    if not client.start():
        print("cannot find command")

lang : language.Language | None = language.find("C++")

if lang:
    lang.connect("first-used", language_used)
