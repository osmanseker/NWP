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
    const char *filteragain = (argc > 1)? argv [1]: "AmongUs>again!>";

    //variables
    int votes[10] = {0};    
    int players = 10;
    int rounds = 9;
    srand(time(0));

    char sentimpmsg[10][20];
    char sentcrewmsg[10][25];
    char opsomming[10];
    char names[10][20];
    char sentnames[256] = "AmongUs>Player?>";
    char sentall[256];
    char buffer [256];
    char *ParsedString;

    //impostermessage
    strcpy(sentimpmsg[0], "You are imposter\n");
    strcpy(sentimpmsg[1], "You are imposter\n");
    strcpy(sentimpmsg[2], "You are imposter\n");
    strcpy(sentimpmsg[3], "You are imposter\n");

    //crewmember message
    strcpy(sentcrewmsg[0], "you are crewmember\n");
    strcpy(sentcrewmsg[1], "you are crewmember\n");
    strcpy(sentcrewmsg[2], "you are crewmember\n");
    strcpy(sentcrewmsg[3], "you are crewmember\n");

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
                printf("service ready!\n");
            }
//whole game in a while loop
while(1)
{

    sentnames[0] = '\0';
    strcpy(sentnames, "AmongUs>Player?>");
    sentall[0] = '\0';

    for (int i = 0;i <4;i++)
    {
           names[i][0] = '\0';
    }

    //Subscribing to players
    zmq_setsockopt(subscriber, ZMQ_SUBSCRIBE, filterjoin, 10);

    //stroring player names in names[] and displaying joined players
    printf("\nwaiting for players to enter name...\n\n");
    for( int i = 0; i < 4; i++)
    {
        memset(buffer,0,256);
        zmq_recv(subscriber, buffer, 256, 0);
        strcpy(names[i],parse(3, buffer) );
        printf("Player %d : %s\n", i+1, names[i]);
    }


    //print list of players to all clients
    strcpy(sentall,"These are the players that joined\n\n");
    for (int i = 0; i < 4; i++)
    {
        sprintf(opsomming, "%d. ",i+1);
        strcat(sentall, opsomming);
        strcat(sentall,names[i]);
        strcat(sentall, "\n");
    }

    strcat(sentnames, sentall);
    zmq_send(publisher, sentnames, strlen(sentnames),0);
    printf("\nAll players joined\n\n");

    sleep(1);

    //subscribe to imposters and selecting 2 random imposters
    zmq_setsockopt(subscriber, ZMQ_SUBSCRIBE, filterimposter, 18);
    printf("choosing 2 random imposters...\n\n");
    int j = 0;
    int i = rand()%4;
    printf("%s\n", names[i]);
    do
    {
        j= rand()%4;
    }
    while(i == j);
    printf("%s\n\n", names[j]);
    printf("imposters have been chosen\n\n");
/*
    //zmq_send(publisher, "AmongUs>imposter!>You are imposter!", 35,0);
    //sending roles to clients
    for (int i = 0; i< 4; i++)
    {
        if(i != r && 4 > 1)
        {
            zmq_send(publisher, sentimpmsg[i], strlen(sentimpmsg[i]),0);
        }
        else
        {
            zmq_send(publisher, sentimpmsg[i], strlen(sentimpmsg[i]),0);
        }
    }
*/

    //voting system for crewmembers
    printf("Crewmembers are voting...\n\n");
    zmq_setsockopt(subscriber, ZMQ_SUBSCRIBE, filtervote, 14);
    zmq_send(publisher, "AmongUs>vote?>Choose the number of player who you want to vote: ", 64,0);

    //receive incoming votes
    for( int i = 0; i < 4; i++)
    {
        memset(buffer,0,256);
        zmq_recv(subscriber, buffer, 256, 0);
        ParsedString = parse(3, buffer);
        printf("%s\n", buffer);
        votes[i] = atoi(ParsedString);
    }


    int temp = 0;
    int most_votes = 0;
    int votecount[10] = {0};
    for (int i = 0; i < 4 ; i++)
    {
        votecount[votes[i] -1] = votecount[votes[i] -1] +1;
    }

    //list of players with amounts of votes
    for (int i = 0; i < 4; i++)
    {
        printf("\n%s : %d", names[i], votecount[i]);
        if(most_votes < votecount[i])
        {
            most_votes = votecount[i];
            temp = i;
        }
    }
    printf("\n%s has the most votes with %d votes\n", names[temp], votecount[temp]);


    zmq_setsockopt(subscriber, ZMQ_SUBSCRIBE, filteragain, 15);
    zmq_send(publisher, "AmongUs>vote?>Do you want to play again? y or n?\n", 51,0);



    sleep(2);

}
    zmq_close (subscriber);
    zmq_close (publisher);
    zmq_ctx_destroy (context);
    return 0;


}
