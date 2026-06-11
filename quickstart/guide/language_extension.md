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
First, we need `cloudforest.lanugage` module to interact with the application. And we import `pythonscripts.lsp.lsp_client_class` for `LspClient` and `create_lsp_client()`.
```py
from cloudforest import language
from pythonscripts.lsp.lsp_client_class import LspClient
```
## Writing callback and create a client
`language.listen()` function takes two parameter: `language-name` (string) and `callback` (function with parameter type `EditArea`). When the language is used by EditArea for the first time, the `language` module runs your callback with one parameter which is the language's name.
```py
# create a callback with parameter type EditArea
def language_used(lang: str):
    # do something
    return

# start listening
language.listen_language_used("MyLang", language_used)
```
Now, create a client. The first parameter is the command to start Language Server. The second parameter is the id of language( ==NOT LANGUAGE NAME== ). After the construction. Run `LspClient.start()` to to start the server. `LspClient.start()` returns `False` if the program cannot be found.
> [!IMPORTANT]
> `LspClient.start()` checks the program with `shutil.which()`. Aliases will not be found.

Parameters for `LspClient` constructor:
1. command `list[str]`
2. language name `str`
3. language id `str`
4. read stderr `bool`: for debugging
 
```py
client: None | LspClient = None

def language_used(lang: str):
    command = ["my-server"]
    client = LspClient(command, "MyLang", "mylang", False)
    if not client.start():
        print("cannot find command")


language.listen_language_used("C++", language_used)

```
