# for writing and parsing LSP message
# https://microsoft.github.io/language-server-protocol/specifications/lsp/3.17/specification/

import copy
import json
from pydoc import text

from typing_extensions import Text

BaseMessage = {"jsonrpc": "2.0"}

InitMessage = {
    **BaseMessage,
    "id": 1,
    "method": "initialize",
    "params": {"capabilities": {}},
}

ExitMessage = {**BaseMessage, "id": 1, "method": "exit"}

PositionProperty = {"line": 0, "character": 0}

RangeProperty = {
    "start": PositionProperty,
    "end": PositionProperty,
}

TextDocument = {
    "uri": "file:///",
    "languageId": "",
    "version": 0,
    "text": "",
}

DidOpenMessage = {
    **BaseMessage,
    "method": "textDocument/didOpen",
    "params": None,
}

DidOpenParams = {"textDocument": None}

DidChangeMessage = {
    **BaseMessage,
    "method": "textDocument/didChange",
    "params": None,
}

DidChangeParams = {
    "textDocument": None,
    "contentChanges": [],
}

ContentChange = {
    "text": "",
}

AutoCompleteMessage = {
    **BaseMessage,
    "id": 2,
    "method": "textDocument/completion",
    "params": {"textDocument": None, "context": {"triggerKind": 1}},
}


def content_length_header(content: str) -> str:
    header = "Content-Length: " + str(len(content)) + "\r\n\r\n"
    return header


def initialize_message() -> str:
    message = json.dumps(InitMessage)
    return message


def initialized_notification() -> str:
    message = {"jsonrpc": "2.0", "method": "initialized", "params": {}}
    return json.dumps(message)


def shut_down_message() -> str:
    shut_down = {"jsonrpc": "2.0", "method": "shutdown", "params": None}
    return json.dumps(shut_down)


def exit_notification() -> str:
    message = json.dumps(ExitMessage)
    return message


def did_open_message(fileuri: str, content: str, langid: str) -> str:
    did_open = {
        "jsonrpc": "2.0",
        "method": "textDocument/didOpen",
        "params": {
            "textDocument": {
                "uri": "file:///" + fileuri,
                "languageId": langid,
                "version": 0,
                "text": content,
            },
        },
    }

    return json.dumps(did_open)


def did_change_message(fileuri: str, content: str, version: int, langid: str) -> str:
    did_change = {
        "jsonrpc": "2.0",
        "method": "textDocument/didChange",
        "params": {
            "textDocument": {
                "uri": "file:///" + fileuri,
                "languageId": langid,
                "version": version,
                "text": content,
            },
            "contentChanges": [{"text": content}],
        },
    }
    return json.dumps(did_change)


def completion_message(fileuri: str, line: int, char: int) -> str:
    copied = AutoCompleteMessage.copy()
    copied["params"]["textDocument"] = {"uri": "file:///" + fileuri}
    copied["params"]["position"] = {"line": line, "character": char}

    message = json.dumps(copied)
    return message


def read_completion(message: str):
    msg = json.loads(message)

    result = msg.get("result")
    if result:
        print(result)
    else:
        print("no value for item")

    for items in result:
        print(items)
    # print("id: " + msg['id'])


def read_lsp_message(message: str) -> list | None:
    # returns a [id, data] pair
    msg = json.loads(message)

    id = msg.get("id")
    data = None
    print(f"lsp_message method: {msg.get('method')}")
    if id == 2:
        # Code completion
        data = msg.get("result").get("items")
    elif id == 1:
        data = "initmessage"
    return [id, data]
