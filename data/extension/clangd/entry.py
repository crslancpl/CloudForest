from cloudforest import editarea, language

# from pythonscripts.lsp.lsp_client_class import LspClient, create_lsp_client
from pythonscripts.lsp.lsp_client_class import LspClient, create_lsp_client


def editarea_created(ea: editarea.EditArea):
    global client
    if not client:
        client = create_lsp_client("clangd", "C++", "cpp")

    if client:
        print(f"clangd running version {client.version}")
        client.listen_editarea(ea)

    else:
        language.stop_listen("C++", editarea_created)


client: None | LspClient = None
language.listen("C++", editarea_created)
