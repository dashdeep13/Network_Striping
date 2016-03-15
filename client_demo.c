
/*
 * demo client
 * It provides an associative memory for strings
 * strings are named with an assignment, e.g.,
 * a=mary had a little lamb
 * They are retrieved by used of their name prepended by a '$' character
 * presented to the prompt ('>'), e.g.,
 * >$a
 * mary had a little lamb
 * >
 *
 * The client passes the strings input to a server, the IP address of
 * which is specified on the command line, e.g..
 * $ client_demo 127.0.0.1
 * The server stores the assigned values into an internal
 * data structure, which it makes persistent using a file.
 *
 * There's a certain amount of overhead in using stdio and
 * sockets, particularly buffer management with fflush() and so forth.
 */

#include "demo.h"

int NUM_CONN = 0;
void create_connection(CONN_PAIR addrs[]) {

  int count = 0;
  while(count<NUM_CONN) {
    int sockfd;
    struct sockaddr_in servaddr;

    /* Open up a TCP/IP socket */
    if( (sockfd = socket(AF_INET, SOCK_STREAM, 0 )) < 0 )
      {
        perror( "socket open" );
        exit( ERR_SOCKET );
      }

    /* initialize and prepare for connections to a server */
    bzero( &servaddr, sizeof(servaddr ));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons( addrs[count].port );

    /* convert address, e.g., 127.0.0.1, to the right format */
    if( inet_pton( AF_INET, addrs[count].addr, &servaddr.sin_addr ) <= 0 )
      {
        perror( "inet_pton for address" );
        exit( 99 );
      }

    /* attempt to make the actual connection */
    if( connect(sockfd, (SA *) &servaddr, sizeof(servaddr)) < 0 )
      {
        perror( "connect to associative memory at server" );
        exit( 100 );
      }
    addrs[count].fd = sockfd;
    count++;
   }

}

int
main( int argc, char *argv[] )
{
  char buf[BUFSIZE];
  FILE *server_request, *server_reply;
  extern int close();

  if( argc != 5 && argc!=3) {
      printf("Usage: %s <IP address1> <IPaddress2> <Port Num1> <Port Num2>\n", argv[0]);
      exit ( 1 );
    }


  CONN_PAIR addrs[2];
  if(argc==3) {
    NUM_CONN = 1;
    addrs[0].port = atoi(argv[2]);
    strcpy(addrs[0].addr,argv[1]);

  } else {
    NUM_CONN = 2;
    addrs[0].port = atoi(argv[2]);
    strcpy(addrs[0].addr,argv[1]);
    addrs[1].port = atoi(argv[3]);
    strcpy(addrs[1].addr,argv[4]);

  }

  create_connection(addrs);
  int sockfd = addrs[0].fd;

  /* setup the interfaces between the new socket and stdio system */
  server_request = fdopen( sockfd, "w" );
  if( server_request == (FILE *) NULL )
    {
      perror( "fdopen of stream for server requests" );
      exit( 2 );
    }
  server_reply = fdopen( sockfd, "r" );
  if( server_reply == (FILE *) NULL )
    {
      perror( "fdopen of stream for server replies" );
      exit( 3 );
    }

  /* The main interactive loop, getting input from the user and 
   * passing to the server, and presenting replies from the server to
   * the user, as appropriate. Lots of opportunity to generalize
   * this primitive user interface...
   */
  for( putchar('>');
       (fgets( buf, BUFSIZE, stdin ) != NULL );
       putchar('>'))
    {
      if( fputs( buf, server_request ) == EOF )
	{
	  perror( "write failure to associative memory at server" );
	}
      fflush( server_request );  /* buffering everywhere.... */

      /* user wants value */
      if( (find_equals( buf ) == NULL) && (find_dollar( buf ) != NULL) )
	{
	  if( fgets( buf, BUFSIZE, server_reply ) == NULL )
	    {
	      perror( "read failure from associative memory at server");
	    }
	  fputs( buf, stdout );
	}

      /* user sets value */
      if( (find_equals( buf ) != NULL) && (find_dollar( buf ) == NULL) )
	{
	  if( fgets( buf, BUFSIZE, server_reply ) == NULL )
	    {
	      perror( "read failure from associative memory at server");
	    }
	}
    }

  /* shut things down */
  fclose( server_request );
  fclose( server_reply );
  close( sockfd); 

  exit( 0 );
}
