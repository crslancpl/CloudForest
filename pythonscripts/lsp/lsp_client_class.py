import asyncio
import re
import select
import shutil
import subprocess
import threading

import cloudforest
from cloudforest import editarea

from . import lsp_msg


class LspClient:
    def __init__(self, lspcommand: str, languageId: str):
        self.LSP = subprocess.Popen(
            lspcommand,
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
        )
        self.lsp_command = lspcommand
        self.language_id: str = languageId
        self.read_number: int = 0

        if self.LSP.stdin:
            self.stdin_thread = self.LSP.stdin
        if self.LSP.stdout:
            self.stdout_thread = self.LSP.stdout
        if self.LSP.stderr:
            self.stderr_thread = self.LSP.stderr

        cloudforest.add_callback("app-closed", self.exit)
        self.start()

    def start(self):
        self.send(lsp_msg.initialize_message())
        self.read()
        self.send(lsp_msg.initialized_notification())
        self.read()

    def exit(self):
        print("ending subprocess")
        self.send(lsp_msg.shut_down_message())
        self.read()
        self.send(lsp_msg.exit_notification())
        self.read()
        self.LSP.terminate()

    # callbacks
    def editarea_completion_requested(
        self, ea: editarea.EditArea, line: int, column: int
    ):
        self.current_editarea = ea
        self.send(lsp_msg.completion_message(ea.get_file_path(), line, column - 1))
        self.read()

    def editarea_text_changed(self, ea: editarea.EditArea):
        message = lsp_msg.did_change_message(
            ea.get_file_path(),
            ea.get_content(),
            ea.get_file_version(),
            self.language_id,
        )
        self.send(message)
        self.read()

    def listen_editarea(self, ea: editarea.EditArea):
        message = lsp_msg.did_open_message(
            ea.get_file_path(), ea.get_content(), self.language_id
        )

        self.send(message)
        self.read()
        ea.add_callback("text-changed", self.editarea_text_changed)

    def send(self, message: str):
        # self.stop_reading()
        if self.LSP.stdin is None:
            return
        ContentLengthHeader = lsp_msg.content_length_header(message)

        # print("message: " + message)
        self.LSP.stdin.flush()
        _ = self.LSP.stdin.write(ContentLengthHeader.encode("utf-8"))
        self.LSP.stdin.flush()
        _ = self.LSP.stdin.write(message.encode("utf-8"))
        self.LSP.stdin.flush()

    def read(self):
        self.err_thread = threading.Thread(
            target=self.read_err(),
        )
        self.out_thread = threading.Thread(
            target=self.read_out(),
        )
        self.err_thread.run()
        self.out_thread.run()

    def stop_reading(self):
        if self.err_thread:
            self.err_thread.join()
        if self.out_thread:
            self.out_thread.join()

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
            waitforin = timeoutpoll.poll(60)  # wait shorter
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
                # print(f"lsp_client_class stdin:{msg}")
                contentlength = re.findall(r"\d+", str(msg))
                _ = self.LSP.stdout.readline()  # this will be \r\n\r\n
                message = self.LSP.stdout.read(int(contentlength[0])).decode()
                print(f"lsp_client_class stdin: {message}")
                # self.process_message(message)

            else:
                return

    def process_message(self, message: str):
        content = lsp_msg.read_lsp_message(message)
        if content is None:
            return
        elif content[0] == 1:
            return
        elif content[0] == 2:
            self.read_completion(content[1])
            return
        else:
            return

    def read_completion(self, items) -> None:
        self.current_editarea.clear_suggestion()
        if items == []:
            return
        # print(items)
        for item in items:
            range = item.get("textEdit").get("range")
            self.current_editarea.add_suggestion(
                item.get("insertText"),
                item.get("label"),
                range.get("start").get("line"),
                range.get("start").get("character"),
                range.get("end").get("line"),
                range.get("end").get("character"),
            )

        self.current_editarea.show_suggestion()


def create_lsp_client(lspcommand: str, languageId: str) -> LspClient | None:
    if not shutil.which(lspcommand):
        # executable or file not found
        print(f'lsp_client_class: Language server "{lspcommand}" not found.')
        return None

    return LspClient(lspcommand, languageId)
