### Socket Server ###

## Overview ##

This program runs a simple server using TCP/IP sockets. 

The server currently runs on local host and can accept sockets from browsers via HTTP requests.

Requests that address "Resources" endpoint are seen as asking for data available on the server.

Resources requested are downloaded from the browser if the client has permissions, otherwise a pin must be sent in the URL to access private resources.

The client program simulates a client request to the server. 

## TODO ##

Clients can upload data to "Resources" with the right credentials.

Have a list of usernames and passwords with permissions.

