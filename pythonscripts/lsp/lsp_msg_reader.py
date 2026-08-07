import json
import sys
from threading import Lock
from typing import Callable

from cloudforest import editarea

from .lsp_request_method import LspRequestMethod


class LspReader:
    def __init__(self):
        self.thread_lock = Lock()
        self.request_dict: dict[str, tuple] = {}

    def add_request(self, id: str, type: LspRequestMethod, data: dict | None):
        req: tuple[LspRequestMethod, dict | None] = (type, data)
        self.request_dict[id] = req

    def on_initialize(self, callback: Callable):
        self.initialize_callback = callback

    def read(self, message: str):
        content: dict = {}
        content = json.loads(message)
        id: int | str | None = content.get("id")
        # print(message)
        if id:
            # response
            match id:
                case 1000:
                    # response for initialize message
                    init_result: dict = content.get("result", {})
                    # print(f"result {result}\n")
                    self.__as_initialize(init_result)

                case _:
                    tup: tuple[LspRequestMethod, dict | None] | None = (
                        self.request_dict.get(id)
                    )
                    if tup:
                        match tup[0]:
                            case LspRequestMethod.COMPLETION:
                                comp_result: dict | None = content.get("result")
                                req_data = tup[1]
                                if comp_result:
                                    self.__as_completion(comp_result, req_data)
                                else:
                                    comp_error: dict | None = content.get("error")
                                    if comp_error:
                                        self.__as_completion_error(comp_error, req_data)

            return

        elif content.get("method"):
            method = content.get("method", "")
            params = content.get("params", {})
            match method:
                case "window/showMessage":
                    self.__as_show_message(params)
                case "textDocument/publishDiagnostics":
                    self.__as_publish_diagnostics(params)

            # self.__find_method_processor(content.get("method"), content.get("params"))
        elif content.get("error"):
            self.__as_error(content.get("error", {}))
        else:
            print(f"other message: {message}\n")
        return content

    def __as_completion(self, result: dict, req_data: dict | None):
        if req_data:
            ea: editarea.EditArea | None = req_data.get("EditArea")
            if ea:
                ea.clear_completion()
                ea.show_completion(result)

        req_data = None

    def __as_completion_error(self, error: dict, req_data: dict | None):
        print("lsp: completion error")
        self.__as_error(error)
        req_data = None

    def __as_error(self, params: dict):
        code: int | None = params.get("code")
        msg: str | None = params.get("message")
        print(f"lsp error: code {code} message {msg}")

    def __as_initialize(self, result: dict):
        self.initialize_callback(result)

    def __as_publish_diagnostics(self, params: dict):
        diagnostics: list = params.get("diagnostics", [])
        uri: str = params.get("uri", "file://")
        version = params.get("version", 0)
        path = str(uri).removeprefix("file://")
        # print(f"diagnostics: {path} version {version}")
        ea: editarea.EditArea | None = editarea.find_by_file_path(path)

        print(f"publish diag ref count {sys.getrefcount(ea)}")
        if not ea or not isinstance(ea, editarea.EditArea):
            return
        ea.process_diagnostic(diagnostics, version)

    def __as_show_message(self, params: dict):
        msg: str = params.get("message", "")
        print(f"lsp show message: {msg}")
