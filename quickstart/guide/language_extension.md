```py
from cloudforest import language, editarea

def callback(ea:editarea.EditArea):
    # do something
    return

# start listening
language.listen("mylang", callback)

# stop listening
language.stop_listen("mylang", callback)
```
