import json
from typing import Callable

from cloudforest import editarea

from .lsp_request_method import LspRequestMethod


class LspReader:
    def __init__(self):
        self.request_dict: dict[str, tuple] = {}

    def add_request(self, id: str, type: LspRequestMethod, data: dict | None):
        req: tuple[LspRequestMethod, dict | None] = (type, data)
        self.request_dict[id] = req

    def on_initialize(self, callback: Callable):
        self.initialize_callback = callback

    def read(self, message: str):
        content: dict = json.loads(message)
        id: int | str | None = content.get("id")
        result: dict = {}
        if id:
            # response
            match id:
                case 1000:
                    # response for initialize message
                    result = content.get("result", {})
                    # print(f"result {result}\n")
                    self.__as_initialize(result)

                case _:
                    tup: tuple[LspRequestMethod, dict | None] | None = (
                        self.request_dict.get(id)
                    )
                    if tup:
                        match tup[0]:
                            case LspRequestMethod.COMPLETION:
                                result = content.get("result", {})
                                req_data = tup[1]
                                self.__as_completion(result, req_data)
                                pass

            return

        elif content.get("method"):
            method = content.get("method", "")
            params = content.get("params", {})

            match method:
                case "window/showMessage":
                    self.__as_show_message(params)
                case "textDocument/publishDiagnostics":
                    self.__as_publish_diagnostics(params)
            pass
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
        ea = editarea.find_by_file_path(path)
        if not ea:
            return

        ea.clear_diagnostics()

        for diagnostic in diagnostics:
            range = diagnostic.get("range")
            start = range.get("start")
            end = range.get("end")
            code = diagnostic.get("code")
            if code is None:
                code = "none"

            ea.add_diagnostic(
                code,
                diagnostic.get("message"),
                start.get("line"),
                start.get("character"),
                end.get("line"),
                end.get("character"),
                diagnostic.get("severity"),
            )

        ea.process_diagnostics(version)

    def __as_show_message(self, params: dict):
        msg: str = params.get("message", "")
        print(f"lsp show message: {msg}")
