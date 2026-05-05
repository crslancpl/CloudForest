from cloudforest import editarea, language

# from pythonscripts.lsp.lsp_client_class import LspClient, create_lsp_client
from pythonscripts.lsp.lsp_client_class import LspClient, create_lsp_client

client: None | LspClient = None


def editarea_created(ea: editarea.EditArea):
    global client
    if not client:
        client = create_lsp_client("rust-analyzer", "rust")

    if client:
        client.listen_editarea(ea)

    else:
        language.stop_listen("Rust", editarea_created)


language.listen("Rust", editarea_created)
