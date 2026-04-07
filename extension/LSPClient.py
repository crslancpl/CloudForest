from cloudforest import EditAreaMod

from extension.CloudForestBuiltIn.LSPClientClass import LSPServer


def new_ea_created(ea: EditAreaMod.EditArea):
    pass


def ea_open(ea: EditAreaMod.EditArea):
    filepath = ea.get_file_path()
    # print(filepath)
    if filepath.endswith(".cpp") or filepath.endswith(".h"):
        print("C++ open")
        # ClangdClient.Listen(ea)
    elif filepath.endswith(".py"):
        ea.set_language("py")
        # PyrightClient.
        pass
    elif filepath.endswith(".js") or filepath.endswith(".ts"):
        # TypescriptClient.Listen(ea)
        pass
    elif filepath.endswith(".rs"):
        pass
    else:
        print("unknown")


def start_listen_editareas():
    EditAreaMod.add_callback("NEWEDITAREA", ea_open)


# StartListenEditAreas() is called by init.py
