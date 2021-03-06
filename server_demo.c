#include "demo.h"

struct sym_list Head;	/* head of singly-linked list */

/*
 * Daemon provides an interactive associative memory
 * via a socket-based interface. Clients either set
 * values with an assignment statement or access
 * values with a $ preface. When the value is
 * accessed, we write it onto the client's socket.
 * We currently do this as an iterative server for
 * reasons of queuing and serialization. If the
 * server is made concurrent, the database will have
 * to have serialized access and copy control - this
 * is not necessary yet.
 *
 * Program notes:
 * Parsing is done
 * with find_dollar() and find_equals().
 * Storage management is carried out by the insert()
 * and lookup() routines.
 * save() and restore() routines added to
 * use disk storage to maintain memory across
 * invocations.
 * Iterative server code is copied from Stevens, "UNIX Network
 * Programming: Networking APIs: Sockets and XTI," p. 101
 *
 */


int NUM_CONN = 0;

int
main( argc, argv, env )
	int argc;
	char *argv[], *env[];
{
  int server_fd, connection_fd;
  void service() ,create_service(),save(), restore();
  socklen_t len;
  struct sockaddr_in cliaddr;
  char buf[BUFSIZE];
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
  	
  create_service(addrs);

  while( HELL_NOT_FROZEN )
    {
      len = sizeof( cliaddr );
      connection_fd = accept( addrs[0].fd, (SA *) &cliaddr, &len );

      if( connection_fd < 0 ){
	  perror( "accept on server_fd" );
	  exit( ERR_ACCEPT );
	}

	restore( DATABASE );
	service(connection_fd);
	save( DATABASE );

	close( connection_fd );
    }
}

void
service( int fd )
{
  FILE *client_request, *client_reply, *fdopen();
  char buf[BUFSIZE];
  extern  void fix_tcl(), insert();

  /* interface between socket and stdio */
  client_request = fdopen( fd, "r" );
  if( client_request == (FILE *) NULL )
    {
      perror( "fdopen of client_request" );
      exit( 1 );
    }
  client_reply = fdopen( fd, "w" );
  if( client_reply == (FILE *) NULL )
    {
      perror( "fdopen of client_reply" );
      exit( 1 );
    }

  while( fgets( buf, BUFSIZE, client_request ) != NULL ){
      char *ptr, *name, *value;

      fix_tcl( buf ); /* hack to interface with tcl scripting language */

      /* ASSIGN */
      if( (ptr = find_equals( buf )) != (char *) NULL )
	{
#ifdef EBUG
fprintf( stderr, "ASSIGN: %s\n", buf );
dump( buf );
#endif
	  *ptr = EOS;
	  name = strsave( buf ); 
	  value = strsave( ++ptr );
	  insert( name, value );
          fputs( "\n", client_reply );
	  fflush( client_reply );
#ifdef EBUG
fprintf( stderr, "REPLY: <>\n" );
#endif
	}
      else if ((ptr = find_dollar( buf )) != (char *) NULL) /* RETRIEVE */
	{
	  char *reply, *find_newline; 

#ifdef EBUG
	  dump( ptr );
#endif
	  /* removes trailing newline if found */ 
	  if( (find_newline = strrchr( ptr, NEWLINE )) != NULL )
	    *find_newline = EOS;

	  if( (reply = lookup( ++ptr )) != NULL )
	    {
	      fputs( reply, client_reply );
	      fflush( client_reply );
#ifdef EBUG
fprintf( stderr, "REPLY: <%s>\n", reply );
#endif
	    }
	  else
	    {
	      fputs( "\n", client_reply );
	      fflush( client_reply );
#ifdef EBUG
fprintf( stderr, "REPLY: <>\n" );
#endif
	    }
	}
      else
	{
#ifdef EBUG
fprintf( stderr, "GARBAGE\n" );
#endif
	}
    }
  return;

}

void create_service(CONN_PAIR addrs[])
{
  int count = 0;
  while(count < NUM_CONN) {
    int listenfd;
    struct sockaddr_in servaddr;

    listenfd = socket(AF_INET, SOCK_STREAM, 0 );
    if( listenfd < 0 )
    {
      perror( "creating socket for listenfd" );
      exit( ERR_SOCKET );
    }
  
    bzero( &servaddr, sizeof(servaddr) );
    servaddr.sin_family = AF_INET;
    /* convert address to the right format */
    if( inet_pton( AF_INET, addrs[count].addr, &servaddr.sin_addr ) <= 0 ) {
      perror( "Cannot convert to correct address" );
      exit(1);
    }

    servaddr.sin_port = htons( addrs[count].port );

    if( bind( listenfd, (SA *) &servaddr, sizeof(servaddr) ) < 0 )
    {
      perror( "bind on listenfd");
      exit( ERR_BIND );
    }

    if( listen( listenfd, LISTENQ ) < 0 )
      {
        perror( "listen on listenfd" );
        exit( ERR_LISTEN );
      }
	
    addrs[count].fd = listenfd; 
    count++;	
  }
 
}

void
fix_tcl( char *buf )
{
  char *ptr;

#define CARRIAGE_RETURN '\r'
  if( (ptr = strrchr( buf, CARRIAGE_RETURN )) != NULL )
    *ptr = EOS;
  return;

}
 
void
dump( char *buf )
{
  fprintf( stderr, "strlen(buf)=%d, buf=<%s>\n", (int) strlen(buf), buf );
  {
    int i;

    for( i=0; buf[i] != EOS; i++ )
      fprintf( stderr, "%d:%c:%x\n", i, buf[i], buf[i] );
  }
}
