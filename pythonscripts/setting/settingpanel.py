import json

from cloudforest import setting_panel


def initialize_setting_panel():
    with open("data/setting/setting.json") as settingfile:
        settings = json.load(settingfile)
        for setting in settings:
            setting_panel.add_tab(setting)
