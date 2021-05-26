#include <stdio.h>
#include <zmq.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <time.h>

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
    //filter instellen
    const char *filterjoin = (argc > 1)? argv [1]: "AmongUs>player!>";
    const char *filtervote = (argc > 1)? argv [1]: "AmongUs>vote!>";

    //variables
    int votes[10] = {0};
    char *names[10][15];
    int players = 10;
    int rounds = 9;
    int opsomming = 1;
    char opsommingS[10];
    int j = 0;
    srand(time(0));


    char sentnames[16] = "AmongUs>Player?>";
    char sentall[256];
    char buffer [256];
    char *ParsedString;

            //connect
            printf("service starting...\n");
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
                printf("service ready!\n\n");
            }

    //Subscribing to players
    zmq_setsockopt(subscriber, ZMQ_SUBSCRIBE, filterjoin, 10);

    //stroring player names in names[] and displaying joined players
    printf("waiting for players to enter name...\n\n");
    for( int i = 0; i < 4; i++)
    {
        memset(buffer,0,256);
        zmq_recv(subscriber, buffer, 256, 0);
        names[i][14] = parse(3, buffer);
        printf("Player %d : %s\n", i+1, names[i][14]);
    }

    //print list of players to all clients
    strcat(sentall,"These are the players that joined\n\n");
    for (int i = 0; i < 4; i++)
    {

        sprintf(opsommingS, "%d. ",i+1);
        strcat(sentall, opsommingS);
        //strcat(sentall, " ");
        strcat(sentall,names[i][14]);
        strcat(sentall, "\n");

    }
    strcat(sentnames, sentall);
    zmq_send(publisher, sentnames, strlen(sentnames),0);
    printf("\nAll players joined\n\n");

    sleep(1);

    //voting system for crewmembers
    printf("Crewmembers are voting...\n\n");
    zmq_setsockopt(subscriber, ZMQ_SUBSCRIBE, filtervote, 14);
    for (int i = 0; i< 4; i++)
    {
        zmq_send(publisher, "AmongUs>vote?>Choose the number of player who you want to vote: ", 64,0);
    }

    for( int i = 0; i < 4; i++)
    {
        memset(buffer,0,256);
        zmq_recv(subscriber, buffer, 256, 0);
        ParsedString = parse(3, buffer);
        printf("%s\n", buffer);
        //votes[i] = atoi(ParsedString);;
        //printf("Player %s voted for %d\n", names[i][14], votes[i]);
    }
    printf("debug print after votes came in");




//    }
    zmq_close (subscriber);
    zmq_close (publisher);
    zmq_ctx_destroy (context);
    return 0;

}
