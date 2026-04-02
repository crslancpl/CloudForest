import json


def load_language_settings():
    #
    with open("data/languages.json") as languagefile:
        data = json.load(languagefile)
        # print(data)
