import re
import select
import subprocess

from extension.CloudForestBuiltIn import LSPMsg
from extension.CloudForestPy import EditAreaMod


class LSPServer:
    def __init__(self, lspcommand: str, languageId: str) -> None:
        self.LSP = subprocess.Popen(
            lspcommand.split(), stdin=subprocess.PIPE, stdout=subprocess.PIPE
        )
        self.languageId: str = languageId

    def Start(self):
        message = LSPMsg.GetInitMessage()
        self.Send(message)
        self.Read()

    def End(self):
        message = LSPMsg.GetExitMessage()
        self.Send(message)

    def OpenFile(self, file: str, content: str):
        message = LSPMsg.GetDidOpenMessage(file, content, self.languageId)
        self.Send(message)
        self.Read()

    def ChangeText(self, file: str, content: str):
        message = LSPMsg.GetDidChangeMessage(file, content, self.languageId)
        self.Send(message)
        self.Read()

    def AutoComplete(self, ea: EditAreaMod.EditArea, line: int, pos: int):
        self.currentEditArea = ea
        message = LSPMsg.GetAutoCompMessage(ea.getfilepath(), line, pos - 1)
        self.Send(message)
        self.Read()

    def Send(self, message: str):
        if self.LSP.stdin is None or self.LSP.stdout is None:
            return
        ContentLengthHeader = LSPMsg.GetContentLengthHeader(message)

        print("message: " + message)
        self.LSP.stdout.flush()
        _ = self.LSP.stdin.write(ContentLengthHeader.encode("utf-8"))
        self.LSP.stdin.flush()
        _ = self.LSP.stdin.write(message.encode("utf-8"))
        self.LSP.stdin.flush()

    def Read(self):
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
                content = LSPMsg.ReadLSPMessage(message)
                if content is None:
                    pass
                elif content[0] == 1:
                    pass
                elif content[0] == 2:
                    self.ReadAutoComplete(content[1])
                    pass
                else:
                    pass

    def ReadAutoComplete(self, items) -> None:
        self.currentEditArea.clearsuggestion()
        if items == []:
            return
        # print(items)
        for item in items:
            range = item.get("textEdit").get("range")
            self.currentEditArea.addsuggestion(
                item.get("insertText"),
                item.get("label"),
                range.get("start").get("line"),
                range.get("start").get("character"),
                range.get("end").get("line"),
                range.get("end").get("character"),
            )

        self.currentEditArea.showsuggestion()
