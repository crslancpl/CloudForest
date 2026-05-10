import threading
from typing import IO, Callable


class IOEvent:
    def __init__(self, io: IO):
        self.callbacks: list = []
        self.io = io
        self.terminate = False
        self.thread = threading.Thread(target=self.read_thread, daemon=True)
        self.thread.start()

    def end(self):
        self.terminate = True

    def read_thread(self):
        for line in self.io:
            if self.terminate:
                return

            text = line.decode()
            for cb in self.callbacks:
                cb(text)

    def skip_line(self) -> None:
        self.io.readline()

    def read_line(self) -> str:
        return self.io.readline().decode()

    def read_chars(self, length: int) -> str:
        return self.io.read(length).decode()

    def add_listener(self, cb: Callable):
        self.callbacks.append(cb)
