# for writing and parsing LSP message
# https://microsoft.github.io/language-server-protocol/specifications/lsp/3.17/specification/

import json

import cloudforest
from cloudforest import editarea

BaseMessage = {"jsonrpc": "2.0"}

ClientCapabilities = {"workspace": {"workspaceFolders": True}}

InitMessage = {
    **BaseMessage,
    "id": 1000,
    "method": "initialize",
    "params": {"capabilities": ClientCapabilities, "workspaceFolders": []},
}

ExitMessage = {**BaseMessage, "id": 1001, "method": "exit"}

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

DidSaveMessage = {
    **BaseMessage,
    "method": "textDocument/didSave",
    "params": None,
}

DidSaveParams = {
    "textDocument": None,
    "text": "",
}

# DidCloseMessage =

ContentChange = {
    "text": "",
}

AutoCompleteMessage = {
    **BaseMessage,
    "id": None,
    "method": "textDocument/completion",
    "params": {"textDocument": None, "context": {"triggerKind": 1}},
}


def content_length_header(content: str) -> str:
    header = "Content-Length: " + str(len(content)) + "\r\n\r\n"
    return header


def initialize_message() -> str:
    workspaces: list[dict[str, str]] = cloudforest.get_workspaces()
    # cloudforest.get_workspaces() returns a list of dictionaries. The URI already contains the "file://" schema
    # {name: "", "uri": ""}
    # print(workspaces)
    msg = InitMessage.copy()
    if len(workspaces) != 0:
        msg["params"]["workspaceFolders"] = workspaces
    message = json.dumps(InitMessage)
    # print(message)
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


def did_open_message(ea: editarea.EditArea, langid: str) -> str:
    did_open = {
        "jsonrpc": "2.0",
        "method": "textDocument/didOpen",
        "params": {
            "textDocument": {
                "uri": "file://" + ea.get_file_path(),
                "languageId": langid,
                "version": ea.get_file_version(),
                "text": ea.get_content(),
            },
        },
    }

    return json.dumps(did_open)


def did_save_notification(ea: editarea.EditArea):
    did_save = {
        "jsonrpc": "2.0",
        "method": "textDocument/didSave",
        "params": {
            "textDocument": {
                "uri": f"file://{ea.get_file_path()}",
            },
            "text": ea.get_content(),
        },
    }
    return json.dumps(did_save)


def did_close_notification(ea: editarea.EditArea):
    did_close = {
        "jsonrpc": "2.0",
        "method": "textDocument/didClose",
        "params": {
            "textDocument": {
                "uri": f"file://{ea.get_file_path()}",
            },
        },
    }

    return json.dumps(did_close)


def did_change_message(
    path: str, range: dict, changed_text: str, version: int, langid: str
) -> str:
    did_change = {
        "jsonrpc": "2.0",
        "method": "textDocument/didChange",
        "params": {
            "textDocument": {
                "uri": "file://" + path,
                "languageId": langid,
                "version": version,
            },
            "contentChanges": [{"range": range, "text": changed_text}],
        },
    }
    return json.dumps(did_change)


def completion_message(path: str, line: int, char: int) -> str:
    copied = AutoCompleteMessage.copy()
    copied["id"] = path
    copied["params"]["textDocument"] = {"uri": "file://" + path}
    copied["params"]["position"] = {"line": line, "character": char}

    message = json.dumps(copied)
    return message


def new_workspace_notification(name: str, path: str):
    msg = {
        "jsonrpc": "2.0",
        "method": "workspace/didChangeWorkspaceFolders",
        "params": {
            "event": {
                "added": [{"uri": f"file://{path}", "name": name}],
                "removed": [],
            }
        },
    }
    return json.dumps(msg)
