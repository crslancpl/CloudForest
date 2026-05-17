# language server cannot find the real CloudForest since
# the module is only available inside CloudForest

# Make the app directory visible
import importlib
import json
import os
import sys

currentdirectory = os.path.dirname(os.path.abspath(__file__))
parentdir = os.path.dirname(currentdirectory)
sys.path.insert(0, parentdir)


def run_extension(path: str):
    # import the Py file as module
    mod = path.removesuffix(".py").replace("/", ".")
    extension_module = importlib.import_module(mod)


def load_enabled_extensions():
    with open("data/extension/enabled.json") as enabled_json:
        enabled_extensions = json.load(enabled_json)
        for extension_name in enabled_extensions:
            extension: dict = enabled_extensions[extension_name]
            path = str(extension.get("location"))
            run_extension(path)


load_enabled_extensions()
