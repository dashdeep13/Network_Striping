#include "stdio.h"
#include "string.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "strings.h"
#include "symlist.h"
#include "defs.h"

/* added the #includes for thread */
#include<pthread.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<time.h>



extern struct sym_list Head;	/* head of singly-linked list */

/* declarations of functions go here */
extern char
	*lookup(),
	*strsave();

extern struct sym_list
	*s_lookup(),
	*new_sym();

