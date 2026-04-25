from cloudforest import editarea, language
from data.extension.lsp.lsp_client_class import LspClient

CLIENT = None


def start():
    global CLIENT
    CLIENT = LspClient("clangd", "cpp")
    CLIENT.start()


def editarea_text_changed(ea: editarea.EditArea):
    CLIENT.change_text(ea.get_file_path(), ea.get_content())


def editarea_created(ea: editarea.EditArea):
    global CLIENT
    if not CLIENT:
        start()
    CLIENT.open_file(ea.get_file_path(), ea.get_content())
    print(ea.get_file_path())


language.listen("C++", editarea_created)
