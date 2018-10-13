**StreamLabs Challenge**

**Execution:**

\[Build Path\]/\[debug/release\]/server.exe

\[Build Path\]/\[debug/release\]/Client.exe

The exe files can also be found in the **binaries** folder of the
project.

**Project Explanation:**

**Utilities:**

Utilities header for now contains only a split string function which is
being used to serialize the data. In real life scenarios, it would
contain a serialization class

For this assignment, I have assumed a unique delimiter to break the
strings and simulate serialization.

The delimiter is "\$\#\^"

**CustomClass:**

CustomClass is a custom library which contains the class of a custom
object. It will be used to create objects and send them over pipes.

**Server:**

The server is single threaded, it uses overlapped operations to service
the client (Only one client for now, can be extended to service multiple
clients by creating multiple instances).

The pipe server creates a Pipe instance which can be connected to the
client.

The Overlapped structure is specified as a parameter in each read, write
and connectnamedpipe operation. It avoids simultaneous operations on the
same pipe instance by using the event object in the Overlapped structure

The event handle to the pipe instance is passed to WaitForSingleObject
function. This function waits for the event to be signaled and returns
the array index of the event that caused the wait operation to complete.
The fIOPending member of the structure keeps track of whether the most
recent I/O operation was pending or completed.

The start function is responsible for creating an instance of the pipe
and waiting for a connection to establish.

The run function is responsible to reading and writing to the pipe. It
runs in a while loop and only breaks when the connection is terminated.

If fIOPending is true, this means that I/O has not yet been completed
and it waits for FIOPending to become false.

There are three states, namely:

-   **Connecting State**: This state is returned when a client has
    successfully connected to the server, when this happens we simply
    change the state to reading state because the server can now start
    reading client requests from the client.

-   **Reading State**: When this state is returned, it means that the
    read operation was completed, and so we can just change the state to
    writing state.

-   **Writing State:** This means that the writing operation is
    completed, and so we just change the state to read state.

-   **If I/O is pending in any of these states, we don't do anything and
    wait to I/O operation to finish.**

**Storage:** The server has 3 unordered maps to stores the data sent by
the client. The keys to each map are integers.

-   Map to store int

-   Map to store strings

-   Map to store Custom Objects

Each message is broken down at the server and the appriopriate reply is sent . 
In this case , the client is responsible for choosing the key for each item being sent to the server. Another possible way of implementing this is to have the server generate a key for each item stored and reply back to the server with the key . 

**Client:**

Client can read and write both synchronously and asynchronously to the
server.

It first connects to the pipe using the createFile method defined in the
windows API. The pipe is open with both GENERIC\_READ and GENERIC\_WRITE
attributes as well as FILE\_FLAG\_OVERLAPPED.

For async write, we must use the LPOVERLAPPED struct as parameter to the
readFile and writeFile function.

The serialized message is sent using the SendMsg function . This
function first checks if the message is async or sync ( -a at the end of
the message is used for async) and calls the sendAsync or sendSync
function appriopriately.

**Sync Send:** The serialized msg is sent to the writeFile function
which will block since the LPOVERLAPPED parameter is NULL. After the
whole message is written, the readFile method is called which blocks
again because LPOVERLAPPED is NULL. If the response is a success, the
message received is sent for processing by calling the processReply
function. If the response is an error, an error msg is printed. There is
no retry functionality included in this project, but one can be easily
implemented by running a loop with a counter to count the number of
retries.

**Send Async:** For async send , we create two OVERLAPPED objects (one
for read and one for write) and pass them to readFile and writeFile
functions to preform async send/receive to the pipe . After calling the
writeFile/readFile function to write/read to the pipe if success is true
, this means that the write operation was successful. If success returns
false , we need to check a few flags and error msgs . Error messages are
retrieved using getLastError function . If the error message is
ERROR\_IO\_PENDING , it means that the operation is still happening and
we call waitforSingleObject to wait for the operation to finish . If the
response is WAIT\_OBJECT\_0 , the operation was a success and we call
getOverlappedResult to retrieve the msg. For any other response , we
just call cancelIO function , which cancels the operation.

*USAGE:*

**The following is the way to construct the message and use client. Any
message being sent through async calls should have "-a" appended at the
end.**

-   **Send Integer : addint\$\#\^\[Key\]\$\#\^\[Integer\]**

-   **Send String : addstr\$\#\^\[Key\]\$\#\^\[String\]**

-   **SendObject : addobj\$\#\^\[Key\]\$\#\^\[uid\]\$\#\^\[name\]** uid
    and name are specific to our customclass , in general you would have
    proper serialization of objects.

-   **Get Integer : getint\$\#\^\[Key\]**

-   **Get String : getstr\$\#\^\[Key\]**

-   **Get Object : getobj\$\#\^\[Key\]**

-   **Call function :
    callfunction\$\#\^\[ObjectKey\]\$\#\^\[FunctionName\]**

**Any message mentioned above can be sent asynchronously by adding "-a"
at the end of the message ( without any spaces) .**

**Example : addint\$\#\^1\$\#\^45-a , this sends an add integer request
to server with key as 1 and the integer as 45 . The send is done
asynchronously.**

**Instead of using a serialization library , it has been simulated using
splitstring function and a unique delimiter which doesn't usually occur
("\$\#\^").**


**References**
1.https://docs.microsoft.com/en-us/windows/desktop/ipc/named-pipes
2.https://docs.microsoft.com/en-us/windows/desktop/ipc/synchronous-and-overlapped-input-and-output
3.https://docs.microsoft.com/en-us/windows/desktop/ipc/named-pipe-server-using-overlapped-i-o
4.https://docs.microsoft.com/en-us/windows/desktop/ipc/named-pipe-client
