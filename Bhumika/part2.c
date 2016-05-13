#include<stdio.h>
#include<string.h>
#include<pthread.h>
#include<stdlib.h>
#include<unistd.h>

#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<time.h>

#define OUTPUTNAME "write.out"
#define OUTPUTNAMENEW "fprint.out"

pthread_t tid[2];				 		// creating 2 threads



void* doFirstThing(void *arg)
{

    struct timeval  tv1, tv2;					// Get the current time at the start of the program 
    gettimeofday(&tv1, NULL);

    unsigned long i = 0;
    pthread_t id = pthread_self();				// calculating the id of the thread

    if(pthread_equal(id,tid[0]))				// calculated id and thread id of 1st thread
    {
        printf("\n First thread processing\n");
	printf("\n The id of the first thread is %u \n", id );
    }

        /* syscall_writer.c -­‐ write 50,000 characters with write*/
	
        int fd;
   	if ((fd=open(OUTPUTNAME,O_WRONLY|O_CREAT,0644)) < 0){
		fprintf(stderr,"Can't open %s. Bye.\n",OUTPUTNAME);
		exit(1);
       }

	for (i=0; i<50000; i++) {
		/* write 50,000 Ys with write */
 		 if (write(fd,"Y",1) < 1) {
			fprintf(stderr,"Can't write. Bye\n");
			exit(1);
  		 }
	}

	close(fd);

   gettimeofday(&tv2, NULL);					// Get the time at the end

   double t1 = (double) (tv1.tv_sec);					
   double t2 = (double) (tv2.tv_usec)/1000000+(double)(tv2.tv_sec);


   double t;
   t = t2 - t1;							// calculate the time elapsed from the start to the end
   printf ("Total time for the first thread is = %f seconds\n", t );

pthread_exit(NULL);						// exit from this thread at the end
}


//function that is executed by the second thread
void* doSecondThing(void *arg)
{
    struct timeval  ts1, ts2;					// Get the current time at the start of this thread
    gettimeofday(&ts1, NULL);

    unsigned long i = 0;
    pthread_t id = pthread_self();				// calculating the id of the thread

    if(pthread_equal(id,tid[1]))				// calculated id and thread id of 2nd thread
    {
        printf("\n Second thread processing\n");
	printf("\n The id of the second thread is %d \n", id );
    }

    /* stdlibrary_writer.c -­‐ write 400,000 characters with fprintf */
   
   FILE *fp;
   if ((fp=fopen(OUTPUTNAMENEW,"w")) == NULL) {
	fprintf(stderr,"Can't open %s. Bye.\n",OUTPUTNAMENEW);
	exit(1);
   }

   for (i=0; i<400000; i++) {
	/* write 400,000 Xs with fprintf */
  	if (fprintf(fp,"X") < 1) {
		fprintf(stderr,"Can't write. Bye\n");
		exit(1);
   	}
    }

    fclose(fp);
    
    gettimeofday(&ts2, NULL);				// Get the time at the end

    double t11 = (double) (ts1.tv_sec);					
    double t12 = (double) (ts2.tv_usec)/1000000+(double)(ts2.tv_sec);

    double tt;
    tt = t12 - t11;					//calculate the time elapsed
    printf ("Total time for the second thread is = %f seconds\n", tt );


pthread_exit(NULL);					// exit from this thread

}


//main program 
int main(void)
{
    int i;
    int err;
    int err1;

	// creating thread 1 .. 1st child thread
        err = pthread_create(&(tid[0]), NULL, &doFirstThing, NULL);			
        if (err != 0)
            printf("\ncan't create thread :[%s]", strerror(err));
        else
            printf("\n First Thread created successfully\n");
        
   
	// creating thread 2 .. 2nd child thread
        err1 = pthread_create(&(tid[1]), NULL, &doSecondThing, NULL);		
        if (err1 != 0)
            printf("\ncan't create thread :[%s]", strerror(err));
        else
            printf("\n Second Thread created successfully\n");
        
 

    sleep(5);

    (void) pthread_join(tid[0], NULL);
    (void) pthread_join(tid[1], NULL);			// to avoid the main process from interrupting the execution of either child threads

    return 0;
}


