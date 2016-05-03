#include "defs.h"
  
int clientSocket[2];
char buffer[MSG_SIZE];
char bufferr[MSG_SIZE];
struct sockaddr_in serverAddr[2];
socklen_t addr_size1, addr_size2;
int count_thread1 = 0;
int count_thread2 = 0;

//should be less than the data packet sent
char data_to_send[] = "Message is the best way to check for the best thing possible\n";

pthread_t tid[2]; 		// creating 2 threads

// function executed by the first thread
void* firstThread(void *arg)
{
	int i=0;
	char buffer1[MSG_SIZE];
	strcpy(buffer1,data_to_send);
  	/*---- Send a lot of messages to the server ----*/
	  for(i=0; i<count_thread1; i++){
	        send(clientSocket[0], buffer1, sizeof(buffer1), 0);
        	//printf("Message sent. \n");
	  }

        close(clientSocket[0]);

        pthread_exit(NULL);                                             // exit from this thread at the end


} // 1st thread end


// function executed by the first thread
void* secondThread(void *arg)
{
	int i=0;
	char buffer1[MSG_SIZE];
	strcpy(buffer1,data_to_send);
        /*---- Send a lot of messages to the server ----*/
          for(i=0; i<count_thread2; i++){
                send(clientSocket[1], buffer1, sizeof(buffer1), 0);
                //printf("Message sent. \n");
          }

        close(clientSocket[0]);

        pthread_exit(NULL);
} // 2nd thread end





int main(int argc, char *argv[])
{
    // for taking the port numbers
    int portNum1=-1;
    int portNum2=-1;                                                                        
    // for taking the IP addresses
    char *addressIP1;                                                                     
    char *addressIP2;
    int err,err1;

   if( argc > 5 )
   {
      printf("Too many arguments supplied.\n");
   }
   else if (argc < 2)
   {
     printf("Usage: %s <IP address1> <Port Num1> <IPaddress2> <Port Num2>\n", argv[0]);
      exit ( 1 );
   }
   else if (argc == 3)
   {
        addressIP1 = argv[1];                                                            // get the server IP address as input from user    
        portNum1 = atoi(argv[2]);                                                        // get the server port number as input from the user
   }
   else if (argc == 5)
   {
	addressIP1 = argv[1];
	addressIP2 = argv[3];
	portNum1 = atoi(argv[2]);
	portNum2 = atoi(argv[4]);
   }
   else if( argc != 5 && argc!=3) {
      printf("Usage: %s <IP address1> <Port Num1> <IPaddress2> <Port Num2>\n", argv[0]);
      exit ( 1 );
   }

  count_thread1 = CHUNKS_TOTAL;

  // we have 2 connections
  if(portNum2 != -1) {
	count_thread1 = CHUNKS_TOTAL/2;
        if(CHUNKS_TOTAL%2 !=0 ) count_thread2++;	
	
	count_thread2 = CHUNKS_TOTAL/2;
  }

 
  int res2;
  int i = 0;

  //for first conn
  clientSocket[0] = socket(AF_INET, SOCK_STREAM, 0);                                       // create the client socket
  
  /*---- Configure settings of the server address struct ----*/
  // Remote System - on which to connect - that is the server
  serverAddr[0].sin_family = AF_INET;                                                      /* Address family = Internet */
  serverAddr[0].sin_port = htons(portNum1);                                                 /* Set port number, using htons function to use proper byte order */
  /* convert address to the right format */
  if( inet_pton( AF_INET, addressIP1, &serverAddr[0].sin_addr ) <= 0 ) {
      perror( "Cannot convert to correct address" );
      exit(1);
  }
	
  memset(serverAddr[0].sin_zero, '\0', sizeof serverAddr[0].sin_zero);                        /* Set all bits of the padding field to 0 */

  /*---- Connect the socket to the server using the address struct ----*/
  addr_size1 = sizeof serverAddr[0];
  res2 = connect(clientSocket[0], (struct sockaddr *) &serverAddr[0], addr_size1);
  /*---- Read the message from the server into the buffer ----*/
  memset(&bufferr,0, sizeof(bufferr));
  //printf("Trying to Confirm Connection\n");
  recv(clientSocket[0], bufferr, sizeof(bufferr), 0);
  printf(" %s \n",bufferr);

  if(portNum2 != -1) {
    //for second conn
    clientSocket[1] = socket(AF_INET, SOCK_STREAM, 0);                                       // create the client socket

    /*---- Configure settings of the server address struct ----*/
    // Remote System - on which to connect - that is the server
    serverAddr[1].sin_family = AF_INET;                                                      /* Address family = Internet */
    serverAddr[1].sin_port = htons(portNum2);                                                 /* Set port number, using htons function to use proper byte order */
    if( inet_pton( AF_INET, addressIP2, &serverAddr[1].sin_addr ) <= 0 ) {
      perror( "Cannot convert to correct address" );
      exit(1);
    }
    memset(serverAddr[1].sin_zero, '\0', sizeof serverAddr[1].sin_zero);                        /* Set all bits of the padding field to 0 */

    /*---- Connect the socket to the server using the address struct ----*/
    addr_size1 = sizeof serverAddr[1];
    res2 = connect(clientSocket[1], (struct sockaddr *) &serverAddr[1], addr_size1);
    /*---- Read the message from the server into the buffer ----*/
    memset(&bufferr,0, sizeof(bufferr));
    recv(clientSocket[1], bufferr, sizeof(bufferr), 0);
    printf(" %s \n",bufferr);
  }

  // print time of operation start
  struct timeval  tv;
  gettimeofday(&tv, NULL);
  printf("Will start to send data\n");
  sleep(2);
	
  unsigned long time_in_micro = (tv.tv_sec) * 1000000 + (tv.tv_usec)  ;
  printf ("Time just before Sending data  %lu micro seconds\n", time_in_micro );

  // creating thread 1
  err =  pthread_create(&(tid[0]), NULL, &firstThread, NULL);
  if (err != 0)
       printf("\ncan't create thread :[%s]", strerror(err));
  else
       printf("\nFirst Thread created successfully\n");

  if(portNum2 != -1) {
     // creating thread 2 .. 2nd child thread
     err1 = pthread_create(&(tid[1]), NULL, &secondThread, NULL);
     if (err1 != 0)
          printf("\ncan't create thread :[%s]", strerror(err));
     else
          printf("\nSecond Thread created successfully\n");
  }

  (void) pthread_join(tid[0], NULL);
  if(portNum2 != -1) {
     (void) pthread_join(tid[1], NULL);                  // to avoid the main process from interrupting the execution of either child threads
  }


  return 0;
}
