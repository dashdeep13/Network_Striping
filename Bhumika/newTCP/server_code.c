#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>

// for the threads
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>

#define PORT 60021

#define CHUNKS_TOTAL 10

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


pthread_t tid[2]; 		// creating 2 threads

// function executed by the first thread
void* firstThread(void *arg)
{

	 struct timeval  tv1, tv2;                                   // Get the current time at the start of the program 
	 gettimeofday(&tv1, NULL);

   	 unsigned long i = 0;
   	 pthread_t id = pthread_self();                              // calculating the id of the thread

   	 if(pthread_equal(id,tid[0]))                                // calculated id and thread id of 1st thread
   	 {
       	    printf("\n First thread processing\n");
            printf("\n The id of the first thread is %u \n", id );
   	 }


	/*---- Send a connection message ----*/
          strcpy(bufferr,"Client is now connected\n");
          send(newSocket,bufferr,sizeof(bufferr),0);


        /*---- Receive a lot of messages together and display all of them ----*/
        for(i=0; i<CHUNKS_TOTAL/2; i++){
                recv(newSocket, buffer1, 1024, 0);
                strcpy(newMesg,buffer1);
                printf("The received message is: %s \n",newMesg);
        } 

	 gettimeofday(&tv2, NULL);                                    // Get the time at the end

	 double t1 = (double) (tv1.tv_sec);
  	 double t2 = (double) (tv2.tv_usec)/1000000+(double)(tv2.tv_sec);


  	 double t;
  	 t = t2 - t1;                                                 // calculate the time elapsed from the start to the end
  	 printf ("Total time for the first thread is = %f seconds\n", t );

	pthread_exit(NULL);                                             // exit from this thread at the end


} // 1st thread end


// function executed by the second thread
void* secondThread(void *arg)
{
} //2nd thread ends here

int main()
{


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

  serverSocket = socket(PF_INET, SOCK_STREAM, 0);                                       // Create the socket
  if (serverSocket < 0)
      {
      perror("ERROR opening socket");
      exit(1);
      }

  /*---- Configure settings of the server address struct ----*/
  serverAddr.sin_family = AF_INET;                                                      /* Address family = Internet */
  serverAddr.sin_port = htons(PORT);                                                    /* Set port number, using htons function to use proper byte order */
  serverAddr.sin_addr.s_addr = INADDR_ANY;                                      /* Set IP address */
  memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);                        /* Set all bits of the padding field to 0 */

  /*---- Bind the address struct to the socket ----*/
  if (bind(serverSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) < 0)
      {
      perror("ERROR on binding");
      exit(1);
      }

  /*---- Listen on the socket, with 30 max connection requests queued ----*/
  if(listen(serverSocket,30)==0)
    printf("Listening\n");
  else
    printf("Error\n");

  /*---- Accept call creates a new socket for the incoming connection ----*/
  addr_size = sizeof serverStorage;
  while(1)
  {
	newSocket = accept(serverSocket, (struct sockaddr *) &serverStorage, &addr_size);
          if (newSocket < 0)
              {
              perror("ERROR on accept");
              exit(1);
              }

          printf("server port = %d\n", ntohs(serverAddr.sin_port));
	  
          // creating thread 1
	  err =  pthread_create(&(tid[0]), NULL, &firstThread, NULL);
          if (err != 0)
              printf("\ncan't create thread :[%s]", strerror(err));
          else
              printf("\n First Thread created successfully\n");

	/*
          // creating thread 2 .. 2nd child thread
          err1 = pthread_create(&(tid[1]), NULL, &doSecondThing, NULL);
          if (err1 != 0)
              printf("\ncan't create thread :[%s]", strerror(err));
          else
              printf("\n Second Thread created successfully\n");
	*/

	  (void) pthread_join(tid[0], NULL);
         // (void) pthread_join(tid[1], NULL);                  // to avoid the main process from interrupting the execution of either child threads

  }
  return 0;

}//main
