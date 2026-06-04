"""
The EXTENSION_DIR is the directory "CloudForest/data/extension"
"""

# Make the app directory visible
import importlib
import json
import os
import sys

EXTENSION_DIR = "data/extension"  # os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, EXTENSION_DIR)


def run_extension(folder_name: str):
    # import the Py file as module
    try:
        with open(f"{EXTENSION_DIR}/{folder_name}/manifest.json") as manifest_file:
            manifest = json.load(manifest_file)
            mod = manifest.get("entry-file").removesuffix(".py").replace("/", ".")
            try:
                importlib.import_module(f"{folder_name}.{mod}")
            except Exception:
                print(
                    f"extension folder {folder_name} does not have a correct entry file"
                )

    except OSError:
        print(f"extension folder {folder_name} does not have manifest.json")


def load_extensions():
    with open("data/setting/extension-list.json") as extension_list_file:
        extension_list = json.load(extension_list_file)
        enabled_extensions: list[str] = extension_list.get("enabled")
        for extension_folder in enabled_extensions:
            run_extension(extension_folder)
