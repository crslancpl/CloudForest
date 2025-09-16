from os import path
from multilspy import SyncLanguageServer
from multilspy.multilspy_config import MultilspyConfig
from multilspy.multilspy_logger import MultilspyLogger

from pathlib import Path


def startlsp(file: str, lang: str):
    filepath = Path(file)
    parentpath = filepath.parent.absolute()
    config = MultilspyConfig.from_dict({"code_language": lang })
    logger = MultilspyLogger()
    server = SyncLanguageServer.create(config, logger,str(parentpath))
    with server.start_server():
        completitonresult = server.language_server.request_completions(filepath.name, 2, 2, False)
        print(completitonresult)
