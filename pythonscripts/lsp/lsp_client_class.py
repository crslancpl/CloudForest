import re
import select
import subprocess

from cloudforest import editarea

from . import lsp_msg


class LspClient:
    def __init__(self, lspcommand: str, languageId: str) -> None:
        print("aldha")
        self.LSP = subprocess.Popen(
            lspcommand.split(), stdin=subprocess.PIPE, stdout=subprocess.PIPE
        )
        print("sta")
        self.languageId: str = languageId

    def start(self):
        message = lsp_msg.init_message()
        self.send(message)
        self.read()

    def end(self):
        message = lsp_msg.exit_message()
        self.send(message)

    def open_file(self, file: str, content: str):
        message = lsp_msg.did_open_message(file, content, self.languageId)
        self.send(message)
        self.read()

    def change_text(self, file: str, content: str):
        message = lsp_msg.did_change_message(file, content, self.languageId)
        self.send(message)
        self.read()

    def get_completion(self, ea: editarea.EditArea, line: int, pos: int):
        self.currentEditArea = ea
        message = lsp_msg.completion_message(ea.get_filepath(), line, pos - 1)
        self.send(message)
        self.read()

    def send(self, message: str):
        if self.LSP.stdin is None or self.LSP.stdout is None:
            return
        ContentLengthHeader = lsp_msg.content_length_header(message)

        # print("message: " + message)
        self.LSP.stdout.flush()
        _ = self.LSP.stdin.write(ContentLengthHeader.encode("utf-8"))
        self.LSP.stdin.flush()
        _ = self.LSP.stdin.write(message.encode("utf-8"))
        self.LSP.stdin.flush()

    def read(self):
        # [!NOTE]
        # We cannot guarantee how long is the message from
        # LSP. There may be a lots of messages one after another.
        # Thereby, we have to set a timeout for the readline()
        # or it will block the program

        if self.LSP.stdout is None or self.LSP.stdin is None:
            print("read error")
            return

        timeoutpoll = select.poll()
        timeoutpoll.register(self.LSP.stdout, select.POLLIN)
        while True:
            # The "Content-Length: ...\r\n" message
            waitforin = timeoutpoll.poll(7)
            if not waitforin:
                # print("[content ended: nothing to poll]\n\n")
                return

            msgbytes = self.LSP.stdout.readline()
            msg = msgbytes.decode()

            if msg.startswith("Content-Length:"):
                # get content length
                # The header looks like this
                # Content-Length: 100\r\n\r\n
                contentlength = re.findall(r"\d+", str(msg))
                _ = self.LSP.stdout.readline()  # this will be \r\n\r\n
                message = self.LSP.stdout.read(int(contentlength[0])).decode()

                # print("lsp message: ", message)
                content = lsp_msg.read_lsp_message(message)
                if content is None:
                    pass
                elif content[0] == 1:
                    pass
                elif content[0] == 2:
                    self.read_completion(content[1])
                    pass
                else:
                    pass

    def read_completion(self, items) -> None:
        self.currentEditArea.clear_suggestion()
        if items == []:
            return
        # print(items)
        for item in items:
            range = item.get("textEdit").get("range")
            self.currentEditArea.add_suggestion(
                item.get("insertText"),
                item.get("label"),
                range.get("start").get("line"),
                range.get("start").get("character"),
                range.get("end").get("line"),
                range.get("end").get("character"),
            )

        self.currentEditArea.show_suggestion()
