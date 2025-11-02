# for writing and parsing LSP message

from ast import Num
import json

InitMessage = {"jsonrpc": "2.0", "id": 1, "method": "initialize", "params": {}}

ExitMessage = {"jsonrpc": "2.0", "id": 1, "method": "exit"}

DidOpenMessage = {
    "jsonrpc": "2.0",
    "method": "textDocument/didOpen",
    "params": {
        "textDocument": {
            "uri": "file:///",
            "languageId": "",
            "version": 1,
            "text": "",
        }
    },
}

DidChangeMessage = {
    "jsonrpc": "2.0",
    "method": "textDocument/didOpen",
    "params": {
        "textDocument": {
            "uri": "file:///",
            "languageId": "",
            "version": 1,
            "text": "",
        }
    },
}

AutoCompleteMessage = {
    "jsonrpc": "2.0",
    "id": 2,
    "method": "textDocument/completion",
    "params": {"textDocument": {}, "context": {"triggerKind": 1}},
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
    copied = DidOpenMessage.copy()
    copied["params"]["textDocument"]["languageId"] = langid
    copied["params"]["textDocument"]["uri"] = "file:///" + fileuri
    copied["params"]["textDocument"]["text"] = content
    message = json.dumps(copied)
    return message


def GetDidChangeMessage(fileuri: str, content: str, langid: str):
    copied = DidChangeMessage.copy()
    copied["params"]["textDocument"]["languageId"] = langid
    copied["params"]["textDocument"]["uri"] = "file:///" + fileuri
    copied["params"]["textDocument"]["text"] = content
    message = json.dumps(copied)
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
