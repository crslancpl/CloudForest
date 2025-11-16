# for writing and parsing LSP message
# https://microsoft.github.io/language-server-protocol/specifications/lsp/3.17/specification/

import json

BaseMessage = {"jsonrpc": "2.0"}

InitMessage = {**BaseMessage, "id": 1, "method": "initialize", "params": {}}

ExitMessage = {**BaseMessage, "id": 1, "method": "exit"}

PositionProperty = {"line": 0, "character": 0}

RangeProperty = {
    "start": PositionProperty,
    "end": PositionProperty,
}

TextDocumentProperty = {"uri": "file:///", "languageId": "", "version": 0, "text": ""}

DidOpenMessage = {
    **BaseMessage,
    "method": "textDocument/didOpen",
    "params": {"textDocument": TextDocumentProperty},
}

DidChangeMessage = {
    **BaseMessage,
    "method": "textDocument/didChange",
    "params": {"textDocument": TextDocumentProperty, "contentChanges": [{"text": ""}]},
}

AutoCompleteMessage = {
    **BaseMessage,
    "id": 2,
    "method": "textDocument/completion",
    "params": {"textDocument": TextDocumentProperty, "context": {"triggerKind": 1}},
}


def GetContentLengthHeader(content: str) -> str:
    header = "Content-Length: " + str(len(content)) + "\r\n\r\n"
    return header


def GetInitMessage() -> str:
    message = json.dumps(InitMessage)
    return message


def GetExitMessage() -> str:
    message = json.dumps(ExitMessage)
    return message


def GetDidOpenMessage(fileuri: str, content: str, langid: str):
    copiedTextDoc = TextDocumentProperty.copy()
    copiedTextDoc["uri"] = "file:///" + fileuri
    copiedTextDoc["text"] = content
    copiedTextDoc["languageId"] = langid

    copiedDidOpenMsg = DidOpenMessage.copy()
    copiedDidOpenMsg["params"]["textDocument"] = copiedTextDoc

    message = json.dumps(copiedDidOpenMsg)
    return message


def GetDidChangeMessage(fileuri: str, content: str, langid: str):
    copiedTextDoc = TextDocumentProperty.copy()
    copiedTextDoc["uri"] = "file:///" + fileuri
    copiedTextDoc["text"] = ""
    copiedTextDoc["languageId"] = langid

    copiedDidChangeMsg = DidChangeMessage.copy()
    copiedDidChangeMsg["params"]["textDocument"] = copiedTextDoc
    copiedDidChangeMsg["params"]["contentChanges"][0]["text"] = content

    message = json.dumps(copiedDidChangeMsg)

    return message


def GetAutoCompMessage(fileuri: str, line: int, char: int) -> str:
    copied = AutoCompleteMessage.copy()
    copied["params"]["textDocument"] = {"uri": "file:///" + fileuri}
    copied["params"]["position"] = {"line": line, "character": char}

    message = json.dumps(copied)
    return message


def ReadAutoComplete(message: str):
    msg = json.loads(message)

    result = msg.get("result")
    if result:
        print(result)
    else:
        print("no value for item")

    for items in result:
        print(items)
    # print("id: " + msg['id'])


def ReadLSPMessage(message: str) -> list | None:
    # returns a [id, data] pair
    msg = json.loads(message)

    id = msg.get("id")
    data = None
    if id == 2:
        # Code completion
        data = msg.get("result").get("items")
    elif id == 1:
        data = "initmessage"
    return [id, data]
