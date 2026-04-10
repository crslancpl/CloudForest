import json

from cloudforest import editarea, language


def load_language_settings():
    #
    with open("data/languages.json") as languagefile:
        data = json.load(languagefile)
        # print(data)
        for langname in data:
            lang = data[langname]
            language.add_language(
                langname, lang["id"], lang["syntax-file"], str(lang["file-extensions"])
            )
