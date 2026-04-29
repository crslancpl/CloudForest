from cloudforest import editarea, language

from pythonscripts.lsp.lsp_client_class import LspClient

client = None


def start():
    global client
    client = LspClient("clangd", "cpp")
    client.start()


def editarea_text_changed(ea: editarea.EditArea):
    client.change_text(ea.get_file_path(), ea.get_content())


def editarea_created(ea: editarea.EditArea):
    global client
    if not client:
        start()
    client.open_file(ea.get_file_path(), ea.get_content())
    print(ea.get_file_path())


language.listen("C++", editarea_created)
