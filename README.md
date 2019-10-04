#Personal Web Server

**Note: this project is still in development**

##Summary
This is a web server written entirely in C intended to serve as a server for Jonah Feldman's personal webpage.  However, the server is also intended to be fully functional for standard HTTP tasks beyond simply serving static content, and is modular enough that with minimal changes it could be used as a web server performing a wide variety of tasks.

##Technologies
###C
The backend is written entirely in C, with no special HTTP libraries.  SELECT is used to handle multiplexing

###Jekyll
Jekyll is used to generate the static content served by the server.  However, Jekyll's built-in Ruby server is not used in any way

##Current Modules
###Socket-io
A suite of functions to abstract away key socket programming functions, including opening and binding a socket, writing an arbitrary number of bytes, and writing a file

###HTTP-io
A suite of functions to handle receiving and sending HTTP requests and responses.

###HTTP-parser
A suite of functions instead to simplify and abstract parsing HTTP request headers and generating correctly formatted response headers

##Future Modules and features

###Partial-message-handler
A module to deal with TCP streams that are not transmitted all at one time due to inconsistent internet connections

###SSL
encrypting all TCP connections and moving the operation to port 443

###Logging
A module intended to log and aggregate data from incoming requests


**Created by Jonah Feldman, all rights reserved**