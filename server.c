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
    const char *filterimposter = (argc > 1)? argv [1]: "AmongUs>imposter!>";

    //variables
    int votes[10] = {0};
    int votecount = 0;    
    int players = 10;
    int rounds = 9;
    int opsomming = 1;
    srand(time(0));

    char opsommingS[10];
    char names[10][100];
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
        //names[i] = parse(3, buffer);
        strcpy(names[i],parse(3, buffer) );
        printf("Player %d : %s\n", i+1, names[i]);
    }

    //print list of players to all clients
    buffer[0] = '\0';
    memset(sentall,0,256);
    strcat(sentall,"These are the players that joined\n\n");
    for (int i = 0; i < 4; i++)
    {

        sprintf(opsommingS, "%d. ",i+1);
        strcat(sentall, opsommingS);
        strcat(sentall,names[i]);
        strcat(sentall, "\n");

    }
    strcat(sentnames, sentall);
    zmq_send(publisher, sentnames, strlen(sentnames),0);
    printf("\nAll players joined\n\n");

    sleep(1);
/*
    //subscribe to imposters and selecting 2 random imposters
    zmq_setsockopt(subscriber, ZMQ_SUBSCRIBE, filterimposter, 18);
    printf("choosing 2 random imposters...\n\n");
    int j = 0;
    int i = rand()%4;
        printf("%s", names[i]);
        do
        {
            j= rand()%10;
        }
        while(i == j);
        printf("%s\n", names[j]);

        printf("imposter is gekozen\n\n");
        zmq_send(publisher, "AmongUs>imposter!>You are imposter!", 35,0);
//    }
*/




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
        votes[i] = atoi(ParsedString);;
        //printf("Player %s voted for %d\n", names[i][14], votes[i]);

        votes[i-1] = votes [i-1]+1;

    }
    printf("debug print after votes came in + count\n\n");

    for (int i = 0;i<1;i++)
    {
        printf("%s heeft %d votes\n", names[i], votes[i]);
    }

    int temp = 0;
    int most_votes = votes[0];
    for (int i = 0; i < 4 ; i++)
    {
        if(most_votes < votes[i])
        {
            most_votes = votes[i];
            temp = i;
        }
    }
    printf("\n%s\n", votes[temp]);
//
//    for(int i = temp; i < rounds;i++)
//    {
//       strcpy(names[i], names[i+1]);
//    }





    zmq_close (subscriber);
    zmq_close (publisher);
    zmq_ctx_destroy (context);
    return 0;


}
