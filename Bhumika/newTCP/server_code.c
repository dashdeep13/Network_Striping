#include "defs.h"

  int serverSocket[2];                                                                     // Creating the server socket
  int newSocket[2];                                                                        // new socket for the client
  char buffer[MSG_SIZE];
  char newMesg[MSG_SIZE];

  struct sockaddr_in serverAddr[2];
  struct sockaddr_in clientAddr;
  struct sockaddr_storage serverStorage[2];

  socklen_t addr_size;
  int clilen, inAddr;
  int temp;
  char nameRecv[1024];
  int result;
  int i = 0;
  int count_thread1,count_thread2;	


pthread_t tid[2]; 		// creating 2 threads

// function executed by the first thread
void* firstThread(void *arg)
{
   	 unsigned long i = 0;
  	 char bufferr[MSG_SIZE];
	 char buffer1[MSG_SIZE];
   	 pthread_t id = pthread_self();                              // calculating the id of the thread

   	 if(pthread_equal(id,tid[0]))                                // calculated id and thread id of 1st thread
   	 {
       	    printf("\nFirst thread processing\n");
   	 }


	/*---- Send a connection message ----*/
          strcpy(bufferr,"\nClient is now connected\n");
          send(newSocket[0],bufferr,sizeof(bufferr),0);


        /*---- Receive a lot of messages together and display all of them ----*/
        for(i=0; i<count_thread1; i++){
                recv(newSocket[0], buffer1, 1024, 0);
                strcpy(newMesg,buffer1);
                printf("The received message is: %s \n",newMesg);
        } 

	close(newSocket[0]);
	pthread_exit(NULL);                                             // exit from this thread at the end


} // 1st thread end


// function executed by the second thread
void* secondThread(void *arg)
{
	unsigned long i = 0;
  	char bufferr[MSG_SIZE];
	char buffer1[MSG_SIZE];
         pthread_t id = pthread_self();                              // calculating the id of the thread

         if(pthread_equal(id,tid[0]))                                // calculated id and thread id of 1st thread
         {
            printf("\n Second thread processing\n");
         }


        /*---- Send a connection message ----*/
          strcpy(bufferr,"Client is now connected\n");
          send(newSocket[1],bufferr,sizeof(bufferr),0);
	

        /*---- Receive a lot of messages together and display all of them ----*/
        for(i=0; i<count_thread2; i++){
                recv(newSocket[1], buffer1, sizeof(buffer1), 0);
                strcpy(newMesg,buffer1);
                printf("The received message is: %s \n",newMesg);
        }

	close(newSocket[1]);
        pthread_exit(NULL);




} //2nd thread ends here

int main(int argc, char *argv[])
{
    // store port numbers
    int portNum1 = -1;
    int portNum2 = -1;                                                                        
    // store IP addresses
    char *addressIP1;                                                                     
    char *addressIP2;


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

/*
  int serverSocket;                                                                     // Creating the server socket
  int newSocket;                                                                        // new socket for the client
  char buffer[1024];
  char bufferr[1024];
  char buffer1[1024];
  char newMesg[1024];

  struct sockaddr_in serverAddr;
  struct sockaddr_storage serverStorage;

  socklen_t addr_size;
  int clilen, inAddr;
  int temp;
  char nameRecv[1024];
  int result;
  int i = 0;
*/


  // for thread 
  int err, err1;  
  // to store the time after the join operation
  struct timeval  tv1;

  //creating socket for first conn

  serverSocket[0] = socket(AF_INET, SOCK_STREAM, 0);                                       // Create the socket
  if (serverSocket[0] < 0)
      {
      perror("ERROR opening socket");
      exit(1);
      }

  /*---- Configure settings of the server address struct ----*/
  serverAddr[0].sin_family = AF_INET;                                                      /* Address family = Internet */
  serverAddr[0].sin_port = htons(portNum1);                                                    /* Set port number, using htons function to use proper byte order */
 /* convert address to the right format */
  if( inet_pton( AF_INET, addressIP1, &serverAddr[0].sin_addr ) <= 0 ) {
      perror( "Cannot convert to correct address" );
      exit(1);
  }

  memset(serverAddr[0].sin_zero, '\0', sizeof serverAddr[0].sin_zero);                        /* Set all bits of the padding field to 0 */

  /*---- Bind the address struct to the socket ----*/
  if (bind(serverSocket[0], (struct sockaddr *) &serverAddr, sizeof(serverAddr)) < 0)
      {
      perror("ERROR on binding");
      exit(1);
      }

  /*---- Listen on the socket, with 30 max connection requests queued ----*/
  if(listen(serverSocket[0],30)==0)
    printf("Listening\n");
  else
    printf("Error\n");

  
  //creating socket for second conn
  
  if(portNum2 != -1) {

    serverSocket[1] = socket(AF_INET, SOCK_STREAM, 0);                                       // Create the socket
    if (serverSocket[1] < 0)
        {
        perror("ERROR opening socket");
        exit(1);
        }

    /*---- Configure settings of the server address struct ----*/
    serverAddr[1].sin_family = AF_INET;                                                      /* Address family = Internet */
    serverAddr[1].sin_port = htons(portNum2);                                                    /* Set port number, using htons function to use proper byte order */
   /* convert address to the right format */
    if( inet_pton( AF_INET, addressIP2, &serverAddr[1].sin_addr ) <= 0 ) {
        perror( "Cannot convert to correct address" );
        exit(1);
    }

    memset(serverAddr[1].sin_zero, '\0', sizeof serverAddr[1].sin_zero);                        /* Set all bits of the padding field to 0 */

    /*---- Bind the address struct to the socket ----*/
    if (bind(serverSocket[1], (struct sockaddr *) &serverAddr, sizeof(serverAddr)) < 0)
        {
        perror("ERROR on binding");
        exit(1);
        }

    /*---- Listen on the socket, with 30 max connection requests queued ----*/
    if(listen(serverSocket[1],30)==0)
      printf("Listening\n");
    else
      printf("Error\n");
 
  } 

  /*---- Accept call creates a new socket for the incoming connection ----*/
  //addr_size = sizeof serverStorage;
  addr_size = sizeof(clientAddr);
  	
  while(1)
  {
	newSocket[0] = accept(serverSocket[0], (struct sockaddr *) &clientAddr, &addr_size);
          if (newSocket[0] < 0)
              {
              perror("ERROR on accept");
              exit(1);
              }

	//second conn
	if(portNum2 != -1) {
	  newSocket[1] = accept(serverSocket[1], (struct sockaddr *) &clientAddr, &addr_size);
            if (newSocket[1] < 0)
              {
                perror("ERROR on accept");
                exit(1);
              }


	}


          printf("server port = %d\n", ntohs(serverAddr[0].sin_port));
	  
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
/*
         // calculate the time at this point
         gettimeofday(&tv1, NULL);
         double t1 = (double) (tv1.tv_sec);
         // calculate the time difference
         double tt;
         tt = t12 - t11;                                     //calculate the time elapsed
         printf ("Total time for execution is  %f seconds\n", tt );
*/
	
	struct timeval  tv;
	gettimeofday(&tv, NULL);

	unsigned long time_in_micro = (tv.tv_sec) * 1000000 + (tv.tv_usec)  ; 
        printf ("Time after Receiving complete  %lu micro seconds\n", time_in_micro );
	break;
  }
  close(serverSocket[0]);
  if(portNum2 !=-1) close(serverSocket[1]); 
  return 0;

}//main
