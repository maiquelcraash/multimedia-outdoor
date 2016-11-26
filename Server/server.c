//
//  server.c
//
//
//  Created by Maiquel Jardel Ludwig on 12/11/16.
//
//



/*
 * tcpserver.c - A simple TCP echo server
 * usage: tcpserver <port>
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFSIZE 1024

#if 0
/*
 * Structs exported from in.h
 */

/* Internet address */
struct in_addr {
    unsigned int s_addr;
};

/* Internet style socket address */
struct sockaddr_in  {
    unsigned short int sin_family; /* Address family */
    unsigned short int sin_port;   /* Port number */
    struct in_addr sin_addr;	 /* IP address */
    unsigned char sin_zero[...];   /* Pad to size of 'struct sockaddr' */
};

/*
 * Struct exported from netdb.h
 */

/* Domain name service (DNS) host entry */
struct hostent {
    char    *h_name;        /* official name of host */
    char    **h_aliases;    /* alias list */
    int     h_addrtype;     /* host address type */
    int     h_length;       /* length of address */
    char    **h_addr_list;  /* list of addresses */
}
#endif

/*
 * error - wrapper for perror
 */
void error(char *msg) {
    perror(msg);
    exit(1);
}

int main(int argc, char **argv) {
    int parentfd; /* parent socket */
    int connection = -1; /* child socket */
    int portno; /* port to listen on */
    int clientlen; /* byte size of client's address */
    struct sockaddr_in serveraddr; /* server's addr */
    struct sockaddr_in clientaddr; /* client addr */
    struct hostent *hostp; /* client host info */
    char buf[BUFSIZE]; /* message buffer */
    char *hostaddrp; /* dotted decimal host addr string */
    int optval; /* flag value for setsockopt */
    int n; /* message byte size */
    char *reply =
            "HTTP/1.0 200 OK\n"
                    "Content-type: application/json\n"
                    "Upgrade: websocket\n"
                    "Connnection: upgrade\n"
                    "Sec-WebSocket-Key: {see RFC6455 Section 11.3.1}\n"
                    "Sec-WebSocket-Version: {see RFC6455 Section 11.3.5}\n"
                    "Access-Control-Allow-Origin: *\n"
                    "\n"
                    "{\"images\":[{\"url\":\"../img/1.png\",\"title\":\"Natura\",\"caption\":\"Shampoo\",\"color\":\"rgb(255,0,0)\"},{\"url\":\"http://s3-us-west-2.amazonaws.com/s.cdpn.io/4273/happy-bot-tinkerbot.jpg\",\"title\":\"Robô\",\"caption\":\"Feliz\",\"color\":\"#00F\"},{\"url\":\"http://s3-us-west-2.amazonaws.com/s.cdpn.io/4273/dada-voltaire-tinkerbot.jpg\",\"title\":\"Outro robo\",\"caption\":\"Triste pq n demos nome\",\"color\":\"green\"}],\"messages\":[\"Outro robo lindo e querido\",\"O Pritsch n acreditva que podia ter mais mensagens\"],\"city\":1399}";

    /*
     * check command line arguments
     */
    if (argc != 2) {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(1);
    }
    portno = atoi(argv[1]);

    /*
     * socket: create the parent socket
     */
    parentfd = socket(AF_INET, SOCK_STREAM, 0);
    if (parentfd < 0)
        error("ERROR opening socket");

    /* setsockopt: Handy debugging trick that lets
     * us rerun the server immediately after we kill it;
     * otherwise we have to wait about 20 secs.
     * Eliminates "ERROR on binding: Address already in use" error.
     */
    optval = 1;
    setsockopt(parentfd, SOL_SOCKET, SO_REUSEADDR,
               (const void *) &optval, sizeof(int));

    /*
     * build the server's Internet address
     */
    bzero((char *) &serveraddr, sizeof(serveraddr));

    /* this is an Internet address */
    serveraddr.sin_family = AF_INET;

    /* let the system figure out our IP address */
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);

    /* this is the port we will listen on */
    serveraddr.sin_port = htons((unsigned short) portno);

    /*
     * bind: associate the parent socket with a port
     */
    if (bind(parentfd, (struct sockaddr *) &serveraddr,
             sizeof(serveraddr)) < 0)
        error("ERROR on binding");

    /*
     * listen: make this socket ready to accept connection requests
     */
    if (listen(parentfd, 5) < 0) /* allow 5 requests to queue up */
        error("ERROR on listen");

    /*
     * main loop: wait for a connection request, echo input line,
     * then close connection.
     */
    clientlen = sizeof(clientaddr);
    while (1) {
        /*
         * accept: wait for a connection request
         */
        connection = accept(parentfd, (struct sockaddr *) &clientaddr, &clientlen);
        if (connection < 0) {
            error("ERROR on accept");
        }
        /*
         * gethostbyaddr: determine who sent the message
         */
        hostp = gethostbyaddr((const char *) &clientaddr.sin_addr.s_addr,
                              sizeof(clientaddr.sin_addr.s_addr), AF_INET);
        if (hostp == NULL) {
            error("ERROR on gethostbyaddr");
        }
        hostaddrp = inet_ntoa(clientaddr.sin_addr);

        if (hostaddrp == NULL) {
            error("ERROR on inet_ntoa\n");
        }

        printf("server established connection with %s (%s)\n",
               hostp->h_name, hostaddrp);

        /*
         * read: read input string from the client
         */
        bzero(buf, BUFSIZE);
        n = read(connection, buf, BUFSIZE);
        if (n < 0) {
            error("ERROR reading from socket");
        }
        printf("server received %d bytes: %s", n, buf);

        /*
         * write: echo the input string back to the client
         */
        n = 0;
        bzero(buf, BUFSIZE);
        buf[0] = "9";
        buf[1] = "8";
        //n = write(connection, reply, strlen(reply));
        n = send(connection, reply, strlen(reply),0);
        if (n < 0) {
            error("ERROR writing to socket");
        }

        close(connection);
    }
}
