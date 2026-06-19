import json

from cloudforest import editarea


def read_as_response(content: dict) -> None:
    for key in content:
        print(f"{key} {content[key]}")
        pass


def read_as_error(content: dict) -> None:
    code: int | None = content.get("code")
    msg: str | None = content.get("message")

    # print(f"lsp error: code {code} message {msg}")
    return


"""
messages with "method"
"""


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


def read_as_publish_diagnostics(params: dict, version_dict: dict) -> None:
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

    path = str(uri).removeprefix("file://")
    v = version_dict.get(path)

    if version < v:
        # version too old
        return
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
        """
        print(
            f"diagnostic severity {diagnostic.get('severity')}: range {range} code '{code}' diagnostic: '{diagnostic.get('message')}'"
        )
        """
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
