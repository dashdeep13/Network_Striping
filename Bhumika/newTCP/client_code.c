#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>

#define PORT 60021

#define CHUNKS_TOTAL 10

int main(int argc, char *argv[])
{
    // for taking the port numbers
    int portNum1;
    int portNum2;                                                                        
    // for taking the IP addresses
    char *addressIP1;                                                                     
    char *addressIP2;

   if( argc > 5 )
   {
      printf("Too many arguments supplied.\n");
   }
   else if (argc < 2)
   {
      printf("Port number and Ip addresses expected.\n");
   }
   else if (argc == 3)
   {
        addressIP1 = argv[1];                                                            // get the server IP address as input from user    
        portNum1 = atoi(argv[2]);                                                        // get the server port number as input from the user
   }
   else if (argc == 5)
   {
	addressIP1 = argv[1];
	addressIP2 = argv[2];
	portNum1 = atoi(argv[3]);
	portNum2 = atoi(argv[4]);
   }
   else if( argc != 5 && argc!=3) {
      printf("Usage: %s <IP address1> <IPaddress2> <Port Num1> <Port Num2>\n", argv[0]);
      exit ( 1 );
   }

  
  int clientSocket;
  char buffer[1024];
  char buffer1[1024];
  char bufferr[1024];
  struct sockaddr_in serverAddr;
  socklen_t addr_size1, addr_size2;
  int res2;
  int i = 0;

  clientSocket = socket(PF_INET, SOCK_STREAM, 0);                                       // create the client socket

  /*---- Configure settings of the server address struct ----*/
  // Remote System - on which to connect - that is the server
  serverAddr.sin_family = AF_INET;                                                      /* Address family = Internet */
  serverAddr.sin_port = htons(portNum1);                                                 /* Set port number, using htons function to use proper byte order */
  serverAddr.sin_addr.s_addr = inet_addr(addressIP1);                                    /* Set IP address to server IP */
  memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);                        /* Set all bits of the padding field to 0 */

  /*---- Connect the socket to the server using the address struct ----*/
  addr_size2 = sizeof serverAddr;
  res2 = connect(clientSocket, (struct sockaddr *) &serverAddr, addr_size2);

  /*---- Read the message from the server into the buffer ----*/
  recv(clientSocket, bufferr, 1024, 0);
  printf(" %s \n",bufferr);

  /*---- Send a lot of messages to the server ----*/
  for(i=0; i<CHUNKS_TOTAL/2; i++){
	strcpy(buffer1, "message \n");
	send(clientSocket, buffer1, 1024, 0); 
	printf("Message sent. \n");
  }

  close(clientSocket);
  return 0;
}
