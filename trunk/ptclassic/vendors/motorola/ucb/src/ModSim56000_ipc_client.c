/* $Id$ */
/* sim56_ipc_client.c
 * ModSim56000 side of IPC 
 *
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/times.h>
#include <netinet/in.h>
#include <signal.h>
#include <errno.h>
#include <netdb.h>

#define NUM_PINS 74


receive_pin_vals(fipc, pin_vals)  /* receive pin values from Thor over IPC */
int  fipc;
char pin_vals[NUM_PINS];
{
/* read into array with buffering */
  char buffer;
  int  n_read;
  int i = 0; 

  while (i < NUM_PINS) {
	n_read = read(fipc, &buffer, 1);
	if (n_read == -1) {
      dsplendwin();
      fprintf(stderr,"\nModSim56000_ipc_client: ERROR: receive_pin_vals: read() failed.\n");
      exit(-1);
      }
	             if (n_read == 0) {
                        dsplendwin();
                        fprintf(stderr,
        "\nModSim56000_ipc_client: receive_pin_vals: THOR shutdown socket.\n");
/*                        sleep(5); */
                        exit(-1);
                }
      pin_vals[i++] = buffer; 
    }
  return(0);
} /* receive_pin_vals */


int
send_pin_vals(fipc, pin_vals) /* send pin values to Thor over IPC */
int  fipc;
char pin_vals[NUM_PINS];
{
  int  n_written;

  if ((n_written = write(fipc, pin_vals, NUM_PINS)) != NUM_PINS ) {
    dsplendwin();
    fprintf(stderr,"\nModSim56000: ERROR: send_pin_vals: write() failed.\n");
    exit(-1);
    }

} /* send_pin_vals */


/*
 * make a connection to a server
 *  connect to a remote port
 *
 * Arguments:
 *   host	name of the remote host
 *   port	port to connect to
 *   protocol   protocol to use ("decnet" or "inet")
 *
 * Returns:
 *   RPC_OK	if the connection was made
 *   RPC_ERROR	if anything went wrong
 */
RPCConnectToServer(host, port, protocol)
char *host;
int port;
char *protocol;
{
#ifdef DNET
    char object[1024];
#endif DNET
    int s;


    if (strcmp(protocol, "inet") == 0) {
	
	if ((s = inet_conn(host, port, SOCK_STREAM)) < 0) {
	    (void) fprintf(stderr, "RPC Error: bad inet connect\n");
	    return(-1);
	}
	
#ifdef DNET
    } else if (strcmp(protocol, "decnet") == 0) {
	
	(void) sprintf(object, "X%d-%d", (int) port / 100000,
		(int) port -  100000 * (port / 100000)); 
	if ((s = dnet_conn(host, object, SOCK_STREAM, 0, 0, 0, 0)) < 0) {
	    (void) fprintf(stderr, "RPC Error: bad decnet connect\n");
	    nerror("decnet connection");
	    return(-1);
	}
#endif DNET	

    } else {
	(void) fprintf(stderr, "RPC Error: protocol not supported: %s\n", protocol);
	return(-1);
    }

    return(s);
}



inet_conn(host, port, option)
char *host;
int port;
int option;
/*
 * make an inet connection, just like dnet_conn
 *
 * host is the name of the host to connect to
 * option is SOCK_STREAM
 *
 * returns socket (>= 0) on success and -1 on failure
 *
 */
{
    int s;
    struct hostent *hp;
    struct sockaddr_in sin;

#ifdef BSD_RPC
    extern char *bzero();
    (void) bzero((char *) &sin, sizeof(struct sockaddr_in));
#else
    extern char *memset();
    (void) memset((char *) &sin, 0, sizeof(struct sockaddr_in));
#endif    
    if ((hp = gethostbyname(host)) == 0) {
	(void) fprintf(stderr, "RPC Error: bad host: %s\n", host);
	return(-1);
    }
    
    sin.sin_family = hp->h_addrtype;  /* better always be AF_INET */

	sin.sin_port = port;

    sin.sin_addr.s_addr = ((struct in_addr *)(hp->h_addr))->s_addr;
    /* memcpy(hp->h_addr, (char *) &sin.sin_addr, hp->h_length); */
    
    if ((s = socket(AF_INET, option, 0)) < 0) {
	(void) fprintf(stderr, "RPC Error: bad inet socket return\n");
	return(-1);
    }
    
    if (connect(s, (struct sockaddr *) &sin, sizeof (sin)) < 0) {
	(void) fprintf(stderr, "RPC Error: bad inet connect\n");
	perror("inet connection");
	return(-1);
    }

    return(s);
}


