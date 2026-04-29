from typing import Callable


def clear_data():
    pass


def add_language(langname: str, id: str, syntaxfile: str, fileextension: str):
    pass


def Listen(langname: str, callback: Callable):
    pass


def stop_listen(langname: str, callback: Callable):
    pass
