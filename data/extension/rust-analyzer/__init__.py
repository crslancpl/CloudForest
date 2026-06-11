from cloudforest import language

from pythonscripts.lsp.lsp_client_class import LspClient


def language_used(lang: str):
    command = ["rust-analyzer", "--verbose"]
    client = LspClient(command, "Rust", "rust", False)
    if not client.start():
        print("cannot find command")


language.listen_language_used("Rust", language_used)
