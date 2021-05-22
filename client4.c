#include <stdio.h>
#include <zmq.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

char *strsep(char **stringp, const char *delim) {
    char *rv = *stringp;
    if (rv) {
        *stringp += strcspn(*stringp, delim);
        if (**stringp)
            *(*stringp)++ = '\0';
        else
            *stringp = 0; }
    return rv;
}
char *parse(int keer, char *ParseString)
{
    char *String,*ParsedString;
    String = strdup(ParseString);
    for (int i = 0; i < keer; i++)
    {
        ParsedString = strsep(&String, ">");
    }
    return ParsedString;
}

int main( int argc, char * argv[] )
{
    //bericht
    const char *BerichtPlayer = (argc > 1)? argv [1]: "AmongUs>player4!>";

    //making chars
    char name[15];
    char sendname[50];
    char buffer [256];
    char *ParsedString;

    //connect
    printf("connecting to service...\n");
    void *context = zmq_ctx_new();
    void *publisher = zmq_socket(context, ZMQ_PUSH);
    void *subscriber = zmq_socket (context, ZMQ_SUB);

    int rp = zmq_connect(publisher, "tcp://benternet.pxl-ea-ict.be:24041");
    int rs = zmq_connect( subscriber, "tcp://benternet.pxl-ea-ict.be:24042" );

    sleep (1);

    //check if connect failed
    if (rp != 0 && rs != 0)
    {
        printf("ERROR: ZeroMQ error occurred during zmq_ctx_new(): %s\n", zmq_strerror(errno));
        return EXIT_FAILURE;
    }
    else
    {
        printf("connection success!\n");
    }

    //Welcoming message
    printf("\n\t\t\t\tWELCOME TO AMONG US IN C\n\n");
    printf("\nThe rules are very simpel: Don't get voted out as crewmate OR don't get killed by the imposters!\n");
    printf("\nThe goals is to vote out the imposters before they kill all the crewmates!\n");
    printf("\n**************************************************************************\n\n\n");

    //subscribing to player
    zmq_setsockopt (subscriber, ZMQ_SUBSCRIBE, "AmongUs>player?>", 16);

    //asking player for name
    printf("Enter your name: ");
    scanf("%s", name);

    //parsing text to send
    strcpy(sendname, BerichtPlayer);
    strcat(sendname, name);

    //sending name to server
    rp = zmq_send(publisher, sendname, strlen(sendname), 0);

    //ontvang resultaat
    memset(buffer,0,256);
    zmq_recv(subscriber, buffer, 256,0);
    ParsedString = parse(3, buffer);
    printf("%s", ParsedString);


    zmq_close(publisher);
    zmq_close (subscriber);
    zmq_ctx_destroy (context);
    return 0;
}
