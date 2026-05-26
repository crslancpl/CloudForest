# Lsp Client check list
> [Language Server Protocal Offical Specification](https://microsoft.github.io/language-server-protocol/specifications/lsp/3.17/specification/)

The LSP support is done by python. The scripts for language server clients are under *pythonscripts/lsp* directory. The *lsp_client_class.py* defined a `LspClient` class. `LspClient` provides the basic funtionalities of lsp including opens the server and listen for events in `EditArea`. Messages are written by *lsp_msg_writer.py* and read by *lsp_msg_reader.py*

- [x] run clients parallelly/concurrently
- [ ] getting server capabilities
- [ ] readding errors
- [x] diagnostics
- [ ] completion on text changed


## Requests
- [x] initialize message
textDocument/
- [ ] declaration
- [ ] definition
- [ ] typeDefinition
- [ ] inplementation
- [ ] references
- [ ] hover
- [ ] completion

## Responses
- [ ] initializeResult

## Notifications
- [x] initialized
- [x] shutdown
- [x] exit

textDocument/
- [x] didOpen
- [x] didChange
- [ ] willSave
- [ ] didSave
- [x] didClose

## LSP Client Capabilities
Here is the list of Client Capabilities in the [official LSP specification](https://microsoft.github.io/language-server-protocol/specifications/lsp/3.17/specification/#clientCapabilities).
- [x] workspace
- [ ] textDocument
- [ ] notebookDocument
- [ ] window
- [ ] general
- [ ] experimental
