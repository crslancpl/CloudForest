# Creating a language server extension
> [!NOTE]
> Details of Language Server can be found in [Microsoft-Language Server Protocal Specification](https://microsoft.github.io/language-server-protocol/specifications/lsp/3.17/specification/) 

The `LspClient` class is not finished yet. You also can create a Language Client from scratch.
## Createing files
Extensions are placed in *data/extension*. Create a folder in *data/extension*. Then create a *manifest.json* and a `__init__.py` file.
```
# in data/extension

my-server
|- __init__.py
|- manifest.json
```
Add the folder of your extension to enabled *data/setting/user/extension.json*.
```json
/* data/extension/enabled.json */
{
  "enabled" : ["my-server"]
}
```
Write the information in *manifest.json*
```json
{
  "name": "My Language Server",
  "description": "Language server for my language"
}

```

## Import modules
First, we need `cloudforest.lanugage` and `cloudforest.editarea` to interact with the application. And we import `pythonscripts.lsp.lsp_client_class` for `LspClient` and `create_lsp_client()`.
```py
from cloudforest import language, editarea
from pythonscripts.lsp.lsp_client_class import LspClient, create_lsp_client()
```
## Writing callback and create a client
`language.listen()` function takes two parameter: `language-name` (string) and `callback` (function with parameter type `EditArea`). When the language of an EditArea is set to your choosen language, the `language` module runs your callback with one parameter which is an `EditArea`.
```py
# create a callback with parameter type EditArea
def editarea_created(ea:editarea.EditArea):
    # do something
    return

# start listening
language.listen("MyLang", editarea_created)
```
Now, create a client with `create_lsp_client()`. The first parameter is the command to start Language Server. The second parameter is the id of language(NOT LANGUAGE NAME). `create_lsp_client()` checks whether the program exists. If it does, it executes the command in a subprocess.
> [!IMPORTANT]
> `create_lsp_client()` checks the program with `shutil.which()`. Aliases will not be found.

```py
client: None | LspClient = None
pending_ea: editarea.EditArea

def server_started():
    global client, pending_ea
    if not client:
        return

    print(f"my server running version {client.version}")
    client.listen_editarea(pending_ea)


def editarea_created(ea: editarea.EditArea):
    global client, pending_ea
    if client:
        client.listen_editarea(ea)
    else:
        pending_ea = ea
        client = create_lsp_client("my-server", "MyLang", "mylang")
        # client will be None if my-server cannot be found  so check it again
    
        if client:
            # Creating a client doesn't send the initialize message automatically, Use client.start()
            # to send the initialize notification
            client.start(server_started)
        else:
            # my-server not found, stop listening
            language.stop_listen("MyLang", editarea_created)
```
