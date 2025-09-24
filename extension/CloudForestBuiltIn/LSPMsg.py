# for writing and parsing LSP message

import json

InitMessage= {
    "jsonrpc":"2.0",
    "id": 1,
    "method":"initialize",
    "params":{
    }
}

ExitMessage = {
    "jsonrpc":"2.0",
    "id":1,
    "method":"exit"
}

DidOpenMessage ={
    "jsonrpc": "2.0",
    "method": "textDocument/didOpen",
    "params": {
        "textDocument": {
            "uri": "file:///",
            "languageId": "cpp",
            "version": 1,
            "text": ""
        }
    }
}

AutoCompleteMessage = {
    "jsonrpc":"2.0",
    "id": 2,
    "method":"textDocument/completion",
    "params":{
        "textDocument":{},
        "context":{
            "triggerKind":1
        }
    }
}

def GetInitMessage()->str:
    message = json.dumps(InitMessage)
    return message

def GetExitMessage()->str:
    message = json.dumps(ExitMessage)
    return message

def GetDidOpenMessage(fileuri:str, content:str):
    copied = DidOpenMessage.copy()
    copied['params']['textDocument']['uri'] = "file:///" + fileuri
    copied['params']['textDocument']['text'] = content
    message = json.dumps(copied)
    return message


def GetAutoCompMessage(fileuri:str, line:int, char:int)->str:
    copied = AutoCompleteMessage.copy()
    copied['params']['textDocument'] = {
        "uri":"file:///" +fileuri
    }
    copied['params']['position'] = {
        "line":line,
        "character": char
    }

    message = json.dumps(copied)
    return message
