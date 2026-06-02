import json
import re
import shutil
import subprocess
import uuid
from typing import Dict

# from concurrent.futures import ThreadPoolExecutor
import cloudforest
from cloudforest import editarea

from pythonscripts.event import IOEvent

from . import lsp_msg_writer
from .lsp_msg_reader import (
    read_as_completion,
    read_as_error,
    read_as_publish_diagnostics,
    read_as_show_message,
)


class LspClient:
    file_version_dict: Dict[str, int] = {}

    def __init__(
        self,
        lspcommand: list[str],
        language: str,
        languageId: str,
        read_stderr: bool,
    ):
        self.LSP = subprocess.Popen(
            lspcommand,
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
        )
        self.lsp_command = lspcommand
        self.language = language
        self.language_id: str = languageId
        self.read_number: int = 0

        if self.LSP.stdin:
            self.stdin_thread = self.LSP.stdin
        if self.LSP.stdout:
            self.out_event: IOEvent = IOEvent(self.LSP.stdout)
            self.out_event.add_listener("line", self.read_out)
        if read_stderr and self.LSP.stderr:
            self.err_event: IOEvent = IOEvent(self.LSP.stderr)
            self.err_event.add_listener("line", self.read_err)

        cloudforest.add_callback("app-closed", self.exit)

    def read_out(self, text: str):
        # [!NOTE]
        # We cannot guarantee how long is the message from
        # LSP. There may be a lot of messages one after another.
        # Thereby, we have to set a timeout for the readline()
        # or it will block the program
        if text.startswith("Content-Length:"):
            # get content length. The header looks like this
            # Content-Length: 100\r\n\r\n

            contentlength = re.findall(r"\d+", str(text))
            self.out_event.skip_line()  # this will be \r\n\r\n
            message = self.out_event.read_chars(int(contentlength[0]))
            # print(f"lsp_client_class stdout: {message}")
            self.read_msg(message)
        else:
            return

    def start(self, init_callback):
        self.send(lsp_msg_writer.initialize_message())
        self.send(lsp_msg_writer.initialized_notification())
        self.init_callback = init_callback

    def exit(self):
        print("ending subprocess")
        self.send(lsp_msg_writer.shut_down_message())
        self.send(lsp_msg_writer.exit_notification())
        self.LSP.terminate()

    def add_workspace(self, name: str, path: str):
        self.send(lsp_msg_writer.new_workspace_notification(name, path))
        pass

    # callbacks
    def editarea_completion_requested(
        self, ea: editarea.EditArea, line: int, column: int
    ):
        self.current_editarea = ea
        self.send(
            lsp_msg_writer.completion_message(ea.get_file_path(), line, column - 1)
        )

    def editarea_lang_changed(self, ea: editarea.EditArea):
        if ea.get_language() == self.language:
            return

        self.send(lsp_msg_writer.did_close_notification(ea.get_file_path()))
        ea.rm_callback("text-changed", self.editarea_text_changed)
        ea.rm_callback("lang-changed", self.editarea_lang_changed)

    def editarea_text_changed(
        self, ea: editarea.EditArea, range, changed_text, version
    ):
        path = ea.get_file_path()
        self.file_version_dict[path] = version
        message = lsp_msg_writer.did_change_message(
            path,
            range,
            changed_text,
            version,
            self.language_id,
        )

        self.send(message)

    def find_method_processor(self, method: str, params: dict):
        match method:
            case "window/showMessage":
                read_as_show_message(params)
            case "textDocument/publishDiagnostics":
                read_as_publish_diagnostics(params, self.file_version_dict)
            case "textDocument/completion":
                read_as_completion(params)

    def listen_editarea(self, ea: editarea.EditArea):
        path = ea.get_file_path()
        version = ea.get_file_version()
        message = lsp_msg_writer.did_open_message(
            path, ea.get_content(), version, self.language_id
        )
        self.file_version_dict[path] = version
        # print(f"listening editarea {ea.get_file_path()}")
        self.send(message)
        ea.add_callback("text-changed", self.editarea_text_changed)
        ea.add_callback("lang-changed", self.editarea_lang_changed)

    def load_server_info(self, result: dict):
        server_info = result.get("serverInfo")
        if not server_info:
            return
        self.name = server_info.get("name")
        self.version = server_info.get("version")

        capability = result.get("capabilities")
        if not capability:
            return
        ws_info = capability.get("workspace")
        if ws_info:
            self.load_server_workspace_capability(ws_info)

    def load_server_workspace_capability(self, ws_info: dict):
        ws_folders_info = ws_info.get("workspaceFolders")
        if not ws_folders_info:
            return
        if ws_folders_info.get("supported") and ws_folders_info.get(
            "changeNotifications"
        ):
            cloudforest.add_callback("new-workspace", self.add_workspace)

    def send(self, message: str):
        # self.stop_reading()
        if self.LSP.stdin is None:
            return
        ContentLengthHeader = lsp_msg_writer.content_length_header(message)
        # print("message: " + message)
        self.LSP.stdin.flush()
        _ = self.LSP.stdin.write(ContentLengthHeader.encode("utf-8"))
        self.LSP.stdin.flush()
        _ = self.LSP.stdin.write(message.encode("utf-8"))
        self.LSP.stdin.flush()

    def read_msg(self, message: str):
        # print(message)
        content = json.loads(message)
        if content.get("id"):
            # response
            match content.get("id"):
                case 1:
                    # response for initialize message
                    result = content.get("result")
                    # print(f"result {result}")

                    self.load_server_info(result)
                    self.init_callback()

        elif content.get("method"):
            self.find_method_processor(content.get("method"), content.get("params"))
        elif content.get("error"):
            read_as_error(content.get("error"))
        elif content.get("result"):
            pass
        else:
            print(f"other message: {message}")
        return content

    def read_err(self, text: str):
        print(f"lsp_client_class stderr: {text}", end="")


def create_lsp_client(
    lspcommand: list[str], language: str, languageId: str, read_stderr: bool
) -> LspClient | None:
    if not shutil.which(lspcommand[0]):
        # executable or file not found
        print(f'lsp_client_class: Language server "{lspcommand[0]}" not found.')
        return None
    return LspClient(lspcommand, language, languageId, read_stderr)
