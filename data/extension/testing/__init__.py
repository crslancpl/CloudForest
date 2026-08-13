import sys

from cloudforest import editarea

from . import test_import

print("this is from 'testing'")
test_import.call_test_import("test1")


def on_new(ea: editarea.EditArea):
    global path
    path = ea.get_file_path()
    print(f"editarea {path} open")


editarea.add_callback("new-editarea", on_new)
