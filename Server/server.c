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
#define MAXLEN  1000000                                                   //10MB

char *getData();

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

        char *data = getData();

        /*
         * write: echo the input string back to the client
         */
        n = 0;
        bzero(buf, BUFSIZE);


        /* Manda o HEADER */
        n = send(connection, header, strlen(header), 0);
        if (n < 0) {
            error("ERROR writing to socket");
        }

        /* Manda o conteúdo */
        n = send(connection, data, strlen(data), 0);
        if (n < 0) {
            error("ERROR writing to socket");
        }

        /* Fecha a conexão */
        hostaddrp = NULL;
        hostp = NULL;
        data = NULL;
        close(connection);
    }
}

char *getData() {
    char data[MAXLEN] =
            "{\"images\":[";
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
                char textBuffer[600];
                char titleBuffer[200];
                char captionBuffer[200];
                char colorBuffer[200];

                /* while que itera os arquivos dentro da pasta slideN */
                while ((innerEntry = readdir(innerDIR)) != NULL) {
                    if (innerEntry->d_type == DT_REG) {             //considera somente tipo arquivos

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

                            fread(imgBuffer, 1, sizeof imgBuffer, fpin);

                            /* Retira um possível enter que possa ter no arquivo */
                            char *pos = strchr(imgBuffer, '\n');
                            pos[0] = '\0';

                            fclose(fpin);
                        }

                        else if (strstr(innerEntry->d_name, "text")) {
                            FILE *fpin;
                            fpin = fopen(currentDir, "rt");

                            fread(textBuffer, 1, sizeof textBuffer, fpin);

                            char *pos = strstr(textBuffer, "\n") + 1;

                            /* Pega o color */
                            char *color = strstr(pos + 1, "\n") + 1;


                            /* Pega o title */
                            int i = 0;
                            for (i; i < sizeof textBuffer; i++) {
                                if (textBuffer[i] != '\n' && textBuffer[i] != '\0') {
                                    titleBuffer[i] = textBuffer[i];
                                }
                                else {
                                    break;
                                }
                            }

                            /* Pega o caption */
                            for (i = 0; i < 200; i++) {
                                if (pos[i] != '\n' && pos[i] != '\0') {
                                    captionBuffer[i] = pos[i];
                                }
                                else {
                                    break;
                                }
                            }
                        }

                        bzero(currentDir, sizeof currentDir);
                    }
                }

                /* monta o slide em JSON */
                //"{\"images\":[{\"url\":\"../img/1.png\",\"title\":\"Natura\",\"caption\":\"Shampoo\",\"color\":\"rgb(255,0,0)\"},{\"url\":\"http://s3-us-west-2.amazonaws.com/s.cdpn.io/4273/happy-bot-tinkerbot.jpg\",\"title\":\"Robô\",\"caption\":\"Feliz\",\"color\":\"#00F\"},{\"url\":\"http://s3-us-west-2.amazonaws.com/s.cdpn.io/4273/dada-voltaire-tinkerbot.jpg\",\"title\":\"Outro robo\",\"caption\":\"Triste pq n demos nome\",\"color\":\"green\"}],\"messages\":[\"Outro robo lindo e querido\",\"O Pritsch n acreditva que podia ter mais mensagens\"],\"city\":1399}";
                char buf[2048576] = "{\"url\":\"data:image/png;base64,";
                strcat(buf, imgBuffer);
                strcat(buf, "\",\"title\":\"");
                strcat(buf, titleBuffer);
                strcat(buf, "\",\"caption\":\"");
                strcat(buf, captionBuffer);
                strcat(buf, "\",\"color\":\"");
                strcat(buf, colorBuffer);
                strcat(buf, "\"},");

                /* adiciona o slide ao array de slides */
                strcat(data, buf);


                //Limpa os arrays
                bzero(imgBuffer, sizeof imgBuffer);
                bzero(textBuffer, sizeof textBuffer);
                bzero(colorBuffer, sizeof colorBuffer);
                bzero(captionBuffer, sizeof captionBuffer);
                bzero(titleBuffer, sizeof titleBuffer);

            }
        }
    }
    closedir(dirp);

    FILE *fpin;
    fpin = fopen("slides/config.properties", "rt");

    char *msgBuffer[2000];
    fread(msgBuffer, 1, sizeof msgBuffer, fpin);

    /* Retira a vírgula do último objeto */
    char *pos = strchr(data, '\0') - 1;
    pos[0] = '\0';

    strcat(data, "],");
    strcat(data, msgBuffer);
    strcat(data, "}");

    return data;

}


//TODO Segmentar em métodos menores