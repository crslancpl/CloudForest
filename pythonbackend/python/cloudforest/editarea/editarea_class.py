from typing import Callable


class EditArea:
    def get_file_path(self):
        pass

    def get_content(self):
        pass

    def add_callback(self, event: str, callback: Callable):
        pass

    def rm_callback(self, event: str, callback: Callable):
        pass

    def highlight(self, tagname: str, line: int, offset: int, length: int):
        pass

    def set_language(self, language: str):
        pass

    def get_language(self):
        pass

    # def add_suggestion(self, )

    def clear_suggestion(self):
        pass

    def hide_suggestion(self):
        pass

    def show_suggestion(self):
        pass
