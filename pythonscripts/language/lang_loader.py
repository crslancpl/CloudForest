import json

from cloudforest import editarea, language


def load_language_settings():
    with open("data/setting/language.json") as languagefile:
        data = json.load(languagefile)

        for langname in data:
            lang = data[langname]

            language.add(
                langname, lang["id"], lang["syntax-file"], lang["file-extensions"]
            )
