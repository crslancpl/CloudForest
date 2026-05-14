# Notes
Important notes for bugs

## Python Backend bottleneck
Once the program returns to executing C++ code, all running Python threads will stop or pause, probably due to Python's GIL. Currently, the `LspClient` has to wait for `stdout` input using `timeoutpoll`, which results in latency while typing. The current goal is to keep the thread running in background.
```py
'''
my_function() is called by C++ with Python C API.

C++ code -> my_function() -> C++ code
            my_loop()     -> stop here
'''

def my_loop():
    while True:
        time.sleep(1)
        print("my loop is running")

def my_function():
    t = threading.Thread(target=my_loop)
    t.start()
    '''
    main thread returns to run C++ code after starting the thread
    '''
```
## EditArea text changed bottleneck
EditArea text changed event doesn't show the difference, The `LspClient` has to post the entire file in `didChange` message, which slows the server's response.
