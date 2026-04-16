# Programming languages
During the initialization, script *pythonscripts/lang_loader.py* reads the Json file *data/setting/language.json* and loads the data to language manager.

You can find a list of language id from [Microsoft Language Server Protocal Specification #text document item](https://microsoft.github.io/language-server-protocol/specifications/lsp/3.17/specification/#textDocumentItem)

```json
// in data/language.json
{
  "LangName": {
    "id": "langid",
    "file-extensions": ["ext1", "ext2"],
    "syntax-file": "file/path.txt"
  }
}
```
For example
```json
{
  "Text": {
    "id": "txt",
    "file-extensions": ["txt"],
    "syntax-file": "data/syntax/text.txt"
  }
}
```
