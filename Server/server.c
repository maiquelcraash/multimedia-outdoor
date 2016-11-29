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
#include <arpa/inet.h>
#include <dirent.h>

#define BUFSIZE 1024

void getData();

void getSlidesCount();

/*
 * error - wrapper for perror
 */
void error(char *msg) {
    perror(msg);
    exit(1);
}

char reply[50000000] =                                                       //2 MB
        "HTTP/1.0 200 OK\n"
                "Content-type: application/json\n"
                "Upgrade: websocket\n"
                "Connnection: upgrade\n"
                "Sec-WebSocket-Key: {see RFC6455 Section 11.3.1}\n"
                "Sec-WebSocket-Version: {see RFC6455 Section 11.3.5}\n"
                "Access-Control-Allow-Origin: *\n"
                "\n"
                //"{\"images\":[{\"url\":\"../img/1.png\",\"title\":\"Natura\",\"caption\":\"Shampoo\",\"color\":\"rgb(255,0,0)\"},{\"url\":\"http://s3-us-west-2.amazonaws.com/s.cdpn.io/4273/happy-bot-tinkerbot.jpg\",\"title\":\"Robô\",\"caption\":\"Feliz\",\"color\":\"#00F\"},{\"url\":\"http://s3-us-west-2.amazonaws.com/s.cdpn.io/4273/dada-voltaire-tinkerbot.jpg\",\"title\":\"Outro robo\",\"caption\":\"Triste pq n demos nome\",\"color\":\"green\"}],\"messages\":[\"Outro robo lindo e querido\",\"O Pritsch n acreditva que podia ter mais mensagens\"],\"city\":1399}";
                //"{\"images\":[{\"url\":\"data:image/png;base64,e3miTj7qEAAAAAElFTkSuQmCC\",\"title\":\"Natura\",\"caption\":\"Shampoo\",\"color\":\"rgb(255,0,0)\"},{\"url\":\"http://s3-us-west-2.amazonaws.com/s.cdpn.io/4273/happy-bot-tinkerbot.jpg\",\"title\":\"Robô\",\"caption\":\"Feliz\",\"color\":\"#00F\"},{\"url\":\"http://s3-us-west-2.amazonaws.com/s.cdpn.io/4273/dada-voltaire-tinkerbot.jpg\",\"title\":\"Outro robo\",\"caption\":\"Triste pq n demos nome\",\"color\":\"green\"}],\"messages\":[\"Outro robo lindo e querido\",\"O Pritsch n acreditva que podia ter mais mensagens\"],\"city\":1399}";
                "{\"images\":[";

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


    char header[1024] = "HTTP/1.0 200 OK\n"
            "Content-type: application/json\n"
            "Upgrade: websocket\n"
            "Connnection: upgrade\n"
            "Sec-WebSocket-Key: {see RFC6455 Section 11.3.1}\n"
            "Sec-WebSocket-Version: {see RFC6455 Section 11.3.5}\n"
            "Access-Control-Allow-Origin: *\n"
            "\n";

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

        getData();

        /*
         * write: echo the input string back to the client
         */
        n = 0;
        bzero(buf, BUFSIZE);
        buf[0] = "9";
        buf[1] = "8";
        //n = write(connection, reply, strlen(reply));
        n = send(connection, reply, strlen(reply), 0);
        if (n < 0) {
            error("ERROR writing to socket");
        }

        close(connection);
    }
}

void getData() {
    getSlidesCount();
}

void getSlidesCount() {
    DIR *dirp;
    struct dirent *entry;

    dirp = opendir("slides");                                     //abre o diretório slides
    while ((entry = readdir(dirp)) != NULL) {                       //itera sobre cada arquivo encontrado
        if (entry->d_type == DT_DIR) {                              //verifica se é diretório
            if ((strcmp(entry->d_name, ".") != 0) &&
                (strcmp(entry->d_name, "..") != 0)) {               //ignora os diretórios "." e ".."

                DIR *innerDIR;
                struct dirent *innerEntry;

                char dir[200] = "slides/";
                strcat(dir,
                       entry->d_name);                              //concatena o nome do diretório slides com a pasta de cada slide
                innerDIR = opendir(dir);                            //abre o diretório do slide

                char imgBuffer[1048576];                            //Max 1MB
                char titleBuffer[400];                              //Max 400 caracteres
                char captionBuffer[400];                            //Max 400 caracteres
                char colorBuffer[100];

                while ((innerEntry = readdir(innerDIR)) != NULL) {  //itera em cada diretório
                    if (innerEntry->d_type == DT_REG) {             //considera somente arquivos

                        char currentDir[200];
                        strcat(currentDir, dir);
                        strcat(currentDir, "/");
                        strcat(currentDir, innerEntry->d_name);

                        if (strstr(innerEntry->d_name, "img")) {    //se for uma imagem

                            /* le a imagem em base64 e salva num arquivo temporário */
                            char command[210] = "cat ";
                            strcat(command, currentDir);
                            strcat(command, " | base64 > temp.txt");
                            system(command);

                            /* Lê o arquivo temporário */
                            FILE *fpin;


                            fpin = fopen("temp.txt", "rt");
                            if (fpin == NULL) {
                                error("Não foi possível abrir o arquivo!");
                                break;
                            }

                            fread(imgBuffer, 1, INT32_MAX, fpin);
                            fclose(fpin);
                        }

                        else if (strstr(innerEntry->d_name, "text")) {
                            //TODO ler os texts
                        }

                        bzero(currentDir, 200);
                    }
                }
                char buf[1048576] = "{\\\"url\\\":\\\"data:image/png;base64,";
                strcat(buf, imgBuffer);
                strcat(buf, "\",\"title\":\"");
                //strcat(reply, buf);
                //TODO concatenar os texts

            }
        }
    }
    closedir(dirp);
}

//TODO Segmentar em métodos menores