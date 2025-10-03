import re
import asyncio
import threading
import time
import subprocess
from CloudForestPy import EditArea
from extension.CloudForestBuiltIn import LSPMsg

class LSPServer():
    def __init__(self, lspname:str) -> None:
        self.loop = asyncio.new_event_loop()
        self.thread = threading.Thread(target=self.event_loop, daemon=True)
        self.thread.start()

    def event_loop(self):
        asyncio.set_event_loop(self.loop)
        self.LSP  = self.loop.run_until_complete(
            asyncio.create_subprocess_exec("clangd",stdin=subprocess.PIPE, stdout=subprocess.PIPE))

    async def Start(self):

        message = LSPMsg.GetInitMessage()
        self.Send(message)

    def End(self):
        message = LSPMsg.GetExitMessage()
        self.Send(message)

    def ChangeText(self, file, content):
        message = LSPMsg.GetDidOpenMessage(file, content)
        self.Send(message)

    def AutoComplete(self, file, line, pos):
        message = LSPMsg.GetAutoCompMessage(file, line,pos)
        self.Send(message)
        asyncio.run_coroutine_threadsafe(self.Read(), self.loop)

    def Send(self, message:str):
        if(self.LSP is None):
            return
        if(self.LSP.stdin is None):
            return
        ContentLengthHeader = LSPMsg.GetContentLengthHeader(message)
        # print("message: " + message)
        self.LSP.stdin.write(ContentLengthHeader.encode('utf-8'))
        self.LSP.stdin.write(message.encode('utf-8'))

    async def Read(self):
        # [!NOTE]
        # We cannot guarantee how long is the message from
        # LSP. There may be a lots of messages one after another.
        # Hence, we have to set a timeout for the readline()
        # or it will block the program
        if(self.LSP.stdout is None or self.LSP.stdin is None):
            print("read error")
            return



        time.sleep(0.1) # let the lsp print the message
        print("read")

        while True:
            # The "Content-Length: ...\r\n" message
            msg = ""
            try:
                msgbytes = self.LSP.stdout.readline()
                msg = str(msgbytes)
                print("message: " + msg)
            except asyncio.TimeoutError:
                print("timeout")


            if not msg:
                print("no message")
                break

            if msg.startswith("Content-Length:"):
                # get content length
                # The header looks like this
                # Content-Length: 100
                contentlength = re.findall(r'\d+',str(msg))
                await self.LSP.stdout.readline()# this will be \r\n\r\n
                content = await self.LSP.stdout.read(int(contentlength[0]))

                print(content)
                LSPMsg.ReadAutoComplete(str(content))


def NewEACreated(file):
    print(file+" created")
    EditArea.addcallback(file, "TEXTCHANGED", "textchanged")
    if(file != "New File"):
        # testlsp.startlsp(file, "python")
        return

Server = None

a = 0

def textchanged(file, cursorline, cursorpos):
    text = EditArea.getcontent(file)
    global Server
    if(Server is None):
        return
    Server.ChangeText(file,text)
    Server.AutoComplete(file, cursorline-1, cursorpos-1)

    # get the text for didOpen message


def StartListenEditAreas():
    global Server

    Server = LSPServer("clangd")
    asyncio.run(Server.Start())
    print("addcallback")
    EditArea.addcallback(None, "OPENNEW", "NewEACreated")

StartListenEditAreas()
