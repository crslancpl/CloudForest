"""
The EXTENSION_DIR is the directory "CloudForest/data/extension"
"""

# Make the app directory visible
import importlib
import json
import os
import shutil
import sys
from io import TextIOWrapper

import cloudforest
from cloudforest import setting

USER_SETTING_FILE = "data/setting/user/extension.json"
DEFAULT_SETTING_FILE = "data/setting/extension-default.json"
EXTENSION_DIR = "data/extension"

sys.path.insert(0, EXTENSION_DIR)

enabled_extension_list: list[str] = []
disabled_extension_list: list[str] = []


def save_extension_setting():
    with open(USER_SETTING_FILE, "w") as extension_list_file:
        data = {"enabled": enabled_extension_list, "disabled": disabled_extension_list}

        json.dump(data, extension_list_file)


def extension_enabled(ext: str):
    disabled_extension_list.remove(ext)
    enabled_extension_list.append(ext)
    save_extension_setting()


def extension_disabled(ext: str):
    enabled_extension_list.remove(ext)
    disabled_extension_list.append(ext)
    save_extension_setting()


def read_extension(folder_name: str, is_enable: bool):
    try:
        with open(f"{EXTENSION_DIR}/{folder_name}/manifest.json") as manifest_file:
            manifest = json.load(manifest_file)
            setting.add_extension(
                manifest.get("name"),
                folder_name,
                manifest.get("description"),
                is_enable,
            )

    except OSError:
        print(f"extension folder {folder_name} does not have a manifest.json")


def read_setting_file(setting_file: TextIOWrapper):
    extension_list = json.load(setting_file)

    enabled_extensions: list[str] = extension_list.get("enabled")
    for extension_folder in enabled_extensions:
        enabled_extension_list.append(extension_folder)
        read_extension(extension_folder, True)

    disabled_extensions: list[str] = extension_list.get("disabled")
    for extension_folder in disabled_extensions:
        disabled_extension_list.append(extension_folder)
        read_extension(extension_folder, False)


def load_setting():
    setting.add_callback("extension-enabled", extension_enabled)
    setting.add_callback("extension-disabled", extension_disabled)

    try:
        with open(USER_SETTING_FILE) as extension_list_file:
            read_setting_file(extension_list_file)

    except OSError:
        print("copy default extension settings to data/setting/user/extension.json")
        shutil.copy(DEFAULT_SETTING_FILE, USER_SETTING_FILE)
        with open(USER_SETTING_FILE) as extension_list_file:
            read_setting_file(extension_list_file)
