import subprocess, select, time, re, json
from CloudForestPy import EditArea
from extension import CloudForestPy
from extension.CloudForestBuiltIn import LSPMsg

class LSPServer():
    def __init__(self, lspname:str) -> None:
        self.LSP  = subprocess.Popen(lspname,stdin=subprocess.PIPE, stdout=subprocess.PIPE)

    def Start(self):
        message = LSPMsg.GetInitMessage()
        self.Send(message)
        self.Read()

    def End(self):
        message = LSPMsg.GetExitMessage()
        self.Send(message)

    def ChangeText(self, file, content):
        message = LSPMsg.GetDidOpenMessage(file, content)
        self.Send(message)

    def AutoComplete(self, file, line, pos):
        self.Filepath = file
        message = LSPMsg.GetAutoCompMessage(file, line,pos)
        self.Send(message)
        self.Read()

    def Send(self, message:str):
        if(self.LSP is None):
            return
        if(self.LSP.stdin is None or self.LSP.stdout is None):
            return
        ContentLengthHeader = LSPMsg.GetContentLengthHeader(message)
        # print("message: " + message)
        self.LSP.stdout.flush()
        self.LSP.stdin.write(ContentLengthHeader.encode('utf-8'))
        self.LSP.stdin.flush()
        self.LSP.stdin.write(message.encode('utf-8'))
        self.LSP.stdin.flush()


    def Read(self):
        # [!NOTE]
        # We cannot guarantee how long is the message from
        # LSP. There may be a lots of messages one after another.
        # Thereby, we have to set a timeout for the readline()
        # or it will block the program

        if(self.LSP.stdout is None or self.LSP.stdin is None):
            print("read error")
            return


        timeoutpoll = select.poll()
        timeoutpoll.register(self.LSP.stdout, select.POLLIN)

        time.sleep(0.1) # let the lsp print the message
        print("\n\n\n[content:]")
        while True:
            # The "Content-Length: ...\r\n" message
            waitforin = timeoutpoll.poll(50)
            if not waitforin:
                print("[content ended: nothing to poll]\n\n")
                return

            msgbytes = self.LSP.stdout.readline()
            msg = msgbytes.decode()
            print("\n\n")
            if msg.startswith("Content-Length:"):
                # get content length
                # The header looks like this
                # Content-Length: 100\r\n\r\n
                contentlength = re.findall(r'\d+',str(msg))
                self.LSP.stdout.readline()# this will be \r\n\r\n
                content = self.LSP.stdout.read(int(contentlength[0])).decode()
                print(content)
                items = LSPMsg.ReadLSPMessage(content)
                if items:
                    EditArea.clearsuggestion(self.Filepath)
                    for item in items:
                        range = item.get('textEdit').get('range')
                        EditArea.addsuggestion(
                            self.Filepath,
                            item.get('insertText'),
                            item.get('label'),
                            range.get('start').get('line'),
                            range.get('start').get('character'),
                            range.get('end').get('line'),
                            range.get('end').get('character'),
                        )




def NewEACreated(file):
    EditArea.addcallback(file, "TEXTCHANGED", "textchanged")

Server = None

a = 0

def textchanged(file, cursorline, cursorpos):
    # get the text for didOpen message
    text = EditArea.getcontent(file)
    global Server
    if(Server is None):
        return
    Server.ChangeText(file,text)
    Server.AutoComplete(file, cursorline-1, cursorpos)


def EAOpen(ea):
    print("new edit area opened")

def StartListenEditAreas():
    global Server

    Server = LSPServer("clangd")
    Server.Start()
    CloudForestPy.AddCallback("OPENNEW", EAOpen)

StartListenEditAreas()
