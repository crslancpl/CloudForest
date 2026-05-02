from cloudforest import editarea, language

from pythonscripts.lsp.lsp_client_class import LspClient, create_lsp_client

client: None | LspClient = None


def editarea_text_changed(ea: editarea.EditArea):
    client.change_text(ea.get_file_path(), ea.get_content())


def editarea_created(ea: editarea.EditArea):
    global client
    if not client:
        client = create_lsp_client("clangd", "cpp")

    if client:
        client.open_file(ea.get_file_path(), ea.get_content())
        ea.a
    else:
        language.stop_listen("C++", editarea_created)


language.listen("C++", editarea_created)
