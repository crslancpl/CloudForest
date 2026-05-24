import json
import re
import select
import shutil
import subprocess
from concurrent.futures import ThreadPoolExecutor

import cloudforest
from cloudforest import editarea

from . import lsp_msg_reader, lsp_msg_writer
from .lsp_msg_reader import find_method_processor, read_as_error


class LspClient:
    def __init__(self, lspcommand: str, language: str, languageId: str):
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
            self.stdout_thread = self.LSP.stdout
            """
            # GIL should be disabled to run the listener
            self.out_event: IOEvent = IOEvent(self.stdout_thread)
            self.out_event.add_listener(self.read_text)
            """

        if self.LSP.stderr:
            self.stderr_thread = self.LSP.stderr

        self.read()
        cloudforest.add_callback("app-closed", self.exit)
        self.start()

    def start(self):
        self.send(lsp_msg_writer.initialize_message())
        self.read()
        self.send(lsp_msg_writer.initialized_notification())
        self.read()

    def exit(self):
        print("ending subprocess")
        self.send(lsp_msg_writer.shut_down_message())
        self.read()
        self.send(lsp_msg_writer.exit_notification())
        self.read()
        self.LSP.terminate()

    def add_workspace(self, name: str, path: str):
        self.send(lsp_msg_writer.new_workspace_notification(name, path))
        self.read()
        pass

    # callbacks
    def editarea_completion_requested(
        self, ea: editarea.EditArea, line: int, column: int
    ):
        self.current_editarea = ea
        self.send(
            lsp_msg_writer.completion_message(ea.get_file_path(), line, column - 1)
        )
        self.read()

    def editarea_lang_changed(self, ea: editarea.EditArea):
        if ea.get_language() == self.language:
            return

        self.send(lsp_msg_writer.did_close_notification(ea.get_file_path()))
        self.read()
        ea.rm_callback("text-changed", self.editarea_text_changed)
        ea.rm_callback("lang-changed", self.editarea_lang_changed)

    def editarea_text_changed(
        self, ea: editarea.EditArea, range, changed_text, version
    ):

        message = lsp_msg_writer.did_change_message(
            ea.get_file_path(),
            range,
            changed_text,
            version,
            self.language_id,
        )

        self.send(message)
        self.read()

    def listen_editarea(self, ea: editarea.EditArea):
        message = lsp_msg_writer.did_open_message(
            ea.get_file_path(), ea.get_content(), self.language_id
        )
        # print(f"listening editarea {ea.get_file_path()}")
        self.send(message)
        self.read()
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

    def read(self):
        with ThreadPoolExecutor(max_workers=1) as TPExecutor:
            TPExecutor.submit(self.read_out)
            # TPExecutor.submit(self.read_err)

    def read_msg(self, message: str):
        content = json.loads(message)
        if content.get("id"):
            # response
            match content.get("id"):
                case 1:
                    # response for initialize message
                    self.load_server_info(content.get("result"))

        elif content.get("method"):
            find_method_processor(content.get("method"), content.get("params"))
        elif content.get("error"):
            read_as_error(content.get("error"))
        elif content.get("result"):
            pass
        else:
            print(f"other message: {message}")
        return content

    def read_err(self):
        if self.LSP.stderr is None:
            print("read error")
            return

        timeoutpoll = select.poll()
        timeoutpoll.register(self.LSP.stderr, select.POLLIN)

        while True:
            waitforin = timeoutpoll.poll(60)
            if not waitforin:
                return
            # print("reading err")
            msgbytes = self.LSP.stderr.readline()
            msg = msgbytes.decode()
            if msg == "":
                return
            print(f"lsp_client_class stderr: {msg}", end="")

    def read_out(self):
        # [!NOTE]
        # We cannot guarantee how long is the message from
        # LSP. There may be a lot of messages one after another.
        # Thereby, we have to set a timeout for the readline()
        # or it will block the program

        if self.LSP.stdout is None:
            print("read error")
            return

        timeoutpoll = select.poll()
        timeoutpoll.register(self.LSP.stdout, select.POLLIN)
        self.read_number += 1
        # print(f"reading out num {self.read_number}")

        while True:
            waitforin = timeoutpoll.poll(50)  # wait shorter
            # The "Content-Length: ...\r\n" message
            if not waitforin:
                # print(f"stop reading out {self.read_number}")
                self.stdout_thread.flush()
                return

            msgbytes = self.LSP.stdout.readline()
            msg = msgbytes.decode()
            if msg.startswith("Content-Length:"):
                # get content length. The header looks like this
                # Content-Length: 100\r\n\r\n

                contentlength = re.findall(r"\d+", str(msg))
                _ = self.LSP.stdout.readline()  # this will be \r\n\r\n
                message = self.LSP.stdout.read(int(contentlength[0])).decode()
                # print(f"lsp_client_class stdout: {message}")
                self.read_msg(message)
            else:
                return


def create_lsp_client(
    lspcommand: str, language: str, languageId: str
) -> LspClient | None:
    if not shutil.which(lspcommand):
        # executable or file not found
        print(f'lsp_client_class: Language server "{lspcommand}" not found.')
        return None
    # print(f"create client for {lspcommand}")
    return LspClient(lspcommand, language, languageId)
