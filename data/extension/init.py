# language server cannot find the real CloudForest since
# the module is only available inside CloudForest

# Make the app directory visible
import json
import os
import sys

currentdirectory = os.path.dirname(os.path.abspath(__file__))
parentdir = os.path.dirname(currentdirectory)
sys.path.insert(0, parentdir)

with open("data/extension/enabled.json") as enabled_json:
    enabled_extensions = json.load(enabled_json)
    for extension_name in enabled_extensions:
        extension = enabled_extensions[extension_name]
        print()
        with open(extension["location"]) as entry:
            exec(entry.read())
