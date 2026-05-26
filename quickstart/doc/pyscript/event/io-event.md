# IOEvent class
> in *pythonscripts/event.py*
## Initializer
`IOEvent(io: IO)`
The `io` can be stdin, stdout, and stderr

## Methods
`end() -> None`
Set the `self.terminate` variable to true

`skip_line()-> None `
Skip the line

`read_line() -> str`
Read the line from IO and return it as string

`read_chars(length: int) -> str`
Read the specific number of characters from the IO and return it as string 

`add_listener(event: str, cb: Callable)`
events:
* "line" : Triggered when a line is written to the buffer. The event passes the line to the callback function.
