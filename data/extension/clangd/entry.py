from cloudforest import editarea, language

# from pythonscripts.lsp.lsp_client_class import LspClient, create_lsp_client
from pythonscripts.lsp.lsp_client_class import LspClient, create_lsp_client

client: None | LspClient = None
pending_ea: editarea.EditArea


def server_started():
    global client, pending_ea
    if not client:
        return

    print(f"clangd running version {client.version}")
    client.listen_editarea(pending_ea)


def editarea_created(ea: editarea.EditArea):
    global client, pending_ea
    if client:
        client.listen_editarea(ea)
    else:
        pending_ea = ea
        client = create_lsp_client("clangd", "C++", "cpp", False)
        if client:
            client.start(server_started)
        else:
            language.stop_listen("C++", editarea_created)


language.listen("C++", editarea_created)
