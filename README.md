# CS214 Project 3: Basic Data Sorter – server/client

## Assumptions

* Rows of CSV files must be delimited by newline characters.

## Server

### Compilation - Server

    make server

### Usage - Server

    ./server -p Port

*Port* is the port to run the server on. **Required**

### Design - Server

This is a WACK server spawns a thread for each connected socket. Esch socket stays open as long as it receives propper initialize, sort, and retrieve requests. This server does the following on a request:

#### Initialize - Design - Server

Generates a unique ID, creates a memory space identified by the ID and saves the given column header in that memory space. Responds with the generated ID.

#### Sort - Design - Server

Sorts the given CSV and saves it to the memory space identified by the given ID. If this is the first sort request with this ID, as long as the CSV has the column header associated with the ID, the sort will be successful. All subsequent sort requested CSVs with this ID will be accepted only if they have the same headers as the CSV stored in the appropriate memory space. If a subsequent CSV is accepted, it is merged with the current CSV and this newly merged CSV is now the CSV in the memory space identified by the given ID. Responds with a sort code:

* 0: The sort was successful.
* 1: The column header associated with the given ID was not found in the given CSV.
* 2: The given ID was not found.
* 3: The column headers in the given CSV do not match the column headers of the CSV associated with the given ID.

#### Retrieve - Design - Server

Sends the CSV associated with the given ID, then wipes the memory space associated with this ID. If the ID was not found or no CSV has been sorted by this ID, the response's CSV size will be 0.

#### Done - Design - Server

Closes the socket this request was made on.

### Extra Credit - Server

For ID generation, I simply made a thread-safe function that increments a static variable on each invocation. I created a Linked list that stores the ID on and other useful information on a node on an initialize request. For sort and retrieve request, the server traverses the linked list to find the ID and its associated data to continue with the requested operation.

## Client

### Compilation - Client

    make client

### Usage - Client

    ./client -h Hostname -p Port -c ColumnHeader -d InputDirectory -o OutputDirectory -s SocketPoolSize

*Hostname* is the hostname of the WACK server used to sort the CSV files. **Required**

*Port* is the port the WACK server is running on. **Required**

*ColumnHeader* is the column header to sort the found CSVs on. **Required**

*InputDirectory* is the directory to search for CSVs. Set to the current directory if not specified.

*OutputDirectory* is the directory to save the sorted and merged CSV. Set to the current directory if not specified.

*SocketPoolSize* is the number connections to make for all communication to the server. Set to 1 if not specified.

### Design - Client

After parsing the command line arguments, the program pool of sockets with the quantity specified by the user. Then it gets a socket from the pool, recieves an ID from the server using the initialize request, and returns the socket back to the pool. This is the only ID used throughout the remiander of this instance the program. The clinet then procedes to search for CSVs in the given input directory, spawning threads for each subdirectory and file found. the thread spawned on a directory recursively searches the directroy while the process spawned on a file checks if it has the ".csv" extension. If the file is not a CSV, the thread prints this information. If the file is a CSV, the thread gets a socket form the pool, makes a sort request using this CSV and the ID obtained earlier, waits for the sort code from the server, returns the socket to the pool, and prints an appropiate message based on the sort code. Once all directories and files have been processed, the program gets a socket from the pool, makes a retrieve request, saves the response CSV data to a newly created CSV in the specified output directory, and returns the socket to the pool. Finally, the program sends a done request before closing each socket in the pool.

### Extra Credit - Client

The pool of sockets is saved using a queue. There is a semaphore associated with the queue to keep track of the number of sockets currently available for use. There is also a mutex to keep the push and pop operations atomic. I also made the done request in the WACK protocol to support this kind of functionality.

## WACK Protocol

The WACK protocol is a protocol made for sorting and merging CSV files. There are four types of WACK requests:

* Initialize
* Sort
* Retrieve
* Done

### Initialize - WACK Protocol

#### Request - Initialize - WACK Protocol

1. The first 4 bytes must be the ASCII representation of **init**.
2. The next 4 bytes must specify the size in bytes of the remaining request in network byte order.
3. The remaining request must specify the column header.

#### Response - Initialize - WACK Protocol

Must consist of 4 bytes specifying the ID in network byte order.

### Sort - WACK Protocol

#### Request - Sort - WACK Protocol

1. The first 4 bytes must be the ASCII representation of **sort**.
2. The next 4 bytes must specify the size in bytes of the remaining request in network byte order.
3. The remaining request must be a CSV file.

#### Response - Sort - WACK Protocol

Must consist of 1 byte, specifying the completion of the request.

### Retrieve - WACK Protocol

#### Request - Retrieve - WACK Protocol

1. The first 4 bytes must be the ASCII representation of **retr**.
2. The next and final 4 bytes must specify an ID in network byte order.

#### Response - Retrieve - WACK Protocol

1. The first 4 bytes must specify the size in bytes of the remaining request in network byte order.
2. The remaining request must be a CSV file.

### Done - WACK Protocol

#### Request - Done - WACK Protocol

Only 4 bytes that must be the ASCII representation of **done**.