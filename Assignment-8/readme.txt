In code for the client in the client server chat server, I started editing it so that we can put a for loop where the client will request to connect on the 
ports ranging between 0-1024 on the server i.e IP 10.100.76.78 in this case. 

We know that apache2 is active at port number 80 so we enable the service and run our code from client.

The  code ouputs PORT number 80 on execution.

Thereafter, we run zenmap at 10.100.76.78 to confirm the output of our C code.
The Zenmapa also output port 80 is open.

Hence, our results are verified.