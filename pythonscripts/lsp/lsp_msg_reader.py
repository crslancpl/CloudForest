import json

from cloudforest import editarea


def read(message: str) -> dict:
    content = json.loads(message)
    if content.get("method"):
        find_method_processor(content.get("method"), content.get("params"))
    if content.get("error"):
        read_as_error(content.get("error"))
    return content


def read_as_error(content: dict) -> None:
    code: int | None = content.get("code")
    msg: str | None = content.get("message")

    # print(f"lsp error: code {code} message {msg}")
    return


"""
messages with "method"
"""


def find_method_processor(method: str, params: dict):
    match method:
        case "window/showMessage":
            read_as_show_message(params)
        case "textDocument/publishDiagnostics":
            read_as_publish_diagnostics(params)
        case "textDocument/completion":
            read_as_completion(params)


def read_as_show_message(params: dict) -> None:
    msg: str | None = params.get("message")
    print(f"lsp show message: {msg}")


def read_as_completion(params: dict) -> None:
    result: None | dict = params.get("result")
    if result:
        print(result)
    else:
        print("no value for item")
    return


def read_as_publish_diagnostics(params: dict) -> None:
    diagnostics: list | None = params.get("diagnostics")
    uri: str | None = params.get("uri")
    version = params.get("version")

    # caomparing variables and None because version 0 will be treadted as false.
    if diagnostics is None:
        return
    if uri is None:
        return
    if version is None:
        return

    path = str(uri).removeprefix("file:///")
    # print(f"diagnostics: {path} version {version}")

    ea = editarea.find_by_file_path(path)

    for diagnostic in diagnostics:
        range = diagnostic.get("range")
        start = range.get("start")
        end = range.get("end")
        code = diagnostic.get("code")
        if code is None:
            code = "none"

        print(
            f"diagnostic: range {range} code '{code}' diagnostic: '{diagnostic.get('message')}'"
        )

        ea.add_diagnostic(
            code,
            diagnostic.get("message"),
            start.get("line"),
            start.get("character"),
            end.get("line"),
            end.get("character"),
        )

    return
