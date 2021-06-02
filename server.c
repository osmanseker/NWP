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
    int voteplayer0 = 0;
    int voteplayer1 = 0;
    int voteplayer2 = 0;
    int voteplayer3 = 0;
    int players = 10;
    int rounds = 9;
    int r;
    srand(time(0));

    char sentimpmsg[10][20];
    char sentcrewmsg[10][25];
    char voterec[3];
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
        strcpy(names[i],parse(3, buffer) );
        printf("Player %d : %s\n", i+1, names[i]);
    }



    //print list of players to all clients
    printf("print voor memset\n\n");
    memset(buffer,0,256);

    //strcpy(sentnames, "AmongUs>Player?>");
    memset(sentall,0,256);
    buffer[0] = '\0';
    strcpy(sentall,"These are the players that joined\n\n");
    //printf("%s\n", sentnames);

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

        switch (votes[i])
        {
            case 0:
                voteplayer0++;
                break;
            case 1:
                voteplayer1++;
                break;
            case 2:
                voteplayer2++;
                break;
            case 3:
                voteplayer3++;
                break;
        }

    }

    printf("debug print after votes came in + count\n\n");
    printf("%d %d %d %d \n", votes[0],votes[1],votes[2],votes[3]);

    printf("%s heeft %d votes\n", names[i], votes[i]);


    int temp = 0;
    int most_votes = votes[0];
    for (int i = 0; i < 4 ; i++)
    {
        if(most_votes < votes[i])
        {
            most_votes = votes[i];
            temp = i;
            votes[i] = atoi(ParsedString);
            printf("kijken wat er gebeurd met de votes");
            printf("%d\n", votes[i]);

        }
    }

    for (int i = 0;i<4;i++)
    {
     printf("%d\n", votes[i]);
    }
    printf("printje van alle votes na comparison\n");

    printf("%s heeft %d votes\n", names[i], votes[i]);

//    printf("\n%s\n", votes[temp]);
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
