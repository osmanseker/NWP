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
    const char *filterimpvote = (argc > 1)? argv [1]: "AmongUs>impvote!>";
    const char *filtervote = (argc > 1)? argv [1]: "AmongUs>vote!>";
    const char *filterskipvote = (argc > 1)? argv [1]: "AmongUs>skipvote!>";
    const char *filterimposter = (argc > 1)? argv [1]: "AmongUs>imposter!>";
    const char *filtercrew = (argc > 1)? argv [1]: "AmongUs>crewmember!>";
    const char *filterkick = (argc > 1)? argv [1]: "AmongUs>kick!>";
    const char *filterwinner= (argc > 1)? argv [1]: "AmongUs>winner!>";
    const char *filteragain = (argc > 1)? argv [1]: "AmongUs>again!>";

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
    //variables
    int votes[10] = {0};
    int players = 6;
    int rounds = 6;
    srand((unsigned)time(0));
    int aantalimp = 2;

    char sentimpmsg[10][256];
    char sentcrewmsg[10][256];
    char sentkick[10][40];
    char sentsurvive[10][45];
    char opsomming[10];
    char names[10][20];
    char sentnames[256] = "AmongUs>Player?>";
    char sentall[256];
    char buffer [256];
    char *ParsedString;

    //role message
    strcpy(sentimpmsg[0], "AmongUs>imposter1?>You are imposter\n");
    strcpy(sentimpmsg[1], "AmongUs>imposter2?>You are imposter\n");
    strcpy(sentimpmsg[2], "AmongUs>imposter3?>You are imposter\n");
    strcpy(sentimpmsg[3], "AmongUs>imposter4?>You are imposter\n");
    strcpy(sentimpmsg[4], "AmongUs>imposter5?>You are imposter\n");
    strcpy(sentimpmsg[5], "AmongUs>imposter6?>You are imposter\n");

    strcpy(sentcrewmsg[0], "AmongUs>crewmember1?>you are crewmember");
    strcpy(sentcrewmsg[1], "AmongUs>crewmember2?>you are crewmember");
    strcpy(sentcrewmsg[2], "AmongUs>crewmember3?>you are crewmember");
    strcpy(sentcrewmsg[3], "AmongUs>crewmember4?>you are crewmember");
    strcpy(sentcrewmsg[4], "AmongUs>crewmember5?>you are crewmember");
    strcpy(sentcrewmsg[5], "AmongUs>crewmember6?>you are crewmember");

    //kick message
    strcpy(sentkick[0], "AmongUs>kick1?>You have been kicked\n");
    strcpy(sentkick[1], "AmongUs>kick2?>You have been kicked\n");
    strcpy(sentkick[2], "AmongUs>kick3?>You have been kicked\n");
    strcpy(sentkick[3], "AmongUs>kick4?>You have been kicked\n");
    strcpy(sentkick[4], "AmongUs>kick5?>You have been kicked\n");
    strcpy(sentkick[5], "AmongUs>kick6?>You have been kicked\n");

    //survive message
    strcpy(sentsurvive[0], "AmongUs>kick1?>You go to the next round");
    strcpy(sentsurvive[1], "AmongUs>kick2?>You go to the next round");
    strcpy(sentsurvive[2], "AmongUs>kick3?>You go to the next round");
    strcpy(sentsurvive[3], "AmongUs>kick4?>You go to the next round");
    strcpy(sentsurvive[4], "AmongUs>kick5?>You go to the next round");
    strcpy(sentsurvive[5], "AmongUs>kick6?>You go to the next round");

    //reset list of player names
    sentnames[0] = '\0';
    strcpy(sentnames, "AmongUs>Player?>");
    sentall[0] = '\0';
    for (int i = 0;i <players;i++)
    {
           names[i][0] = '\0';
    }

    //Subscribing to players
    zmq_setsockopt(subscriber, ZMQ_SUBSCRIBE, filterjoin, 10);

    //stroring player names in names[] and displaying joined players
    printf("\nwaiting for players to enter name...\n\n");
    for( int i = 0; i < players; i++)
    {
        memset(buffer,0,256);
        zmq_recv(subscriber, buffer, 256, 0);
        strcpy(names[i],parse(3, buffer) );
        printf("Player %d : %s\n", i+1, names[i]);
    }


    //print list of players to all clients
    strcpy(sentall,"These are the players that joined\n\n");
    for (int i = 0; i < players; i++)
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
    zmq_setsockopt(subscriber, ZMQ_SUBSCRIBE, filterimposter, strlen(filterimposter));
    zmq_setsockopt(subscriber, ZMQ_SUBSCRIBE, filtercrew, strlen(filtercrew));
    printf("choosing 2 random imposters...\n");
    int j = 0;
    int r = rand()%players;
    do
    {
        j= rand()%players;
    }
    while(r == j);
    printf("%s and %s are the imposters\n", names[r], names[j]);

    //sending roles to clients
    for (int i = 0; i< rounds; i++)
    {
        if( names[i] == names[j] || names[i] == names[r])
        {
            zmq_send(publisher, sentimpmsg[i], strlen(sentimpmsg[i]),0);
        }
        else
        {
            zmq_send(publisher, sentcrewmsg[i], strlen(sentcrewmsg[i]),0);
        }
    }

//vnf hier moet de forloop met rounds komen voor de game
for (int i = 0; i < rounds; i++)
{
//-----------------------------------------IMPOSTERS-----------------------------------------------------------------------
    //voting system imposters
    sleep(1);
    zmq_setsockopt(subscriber, ZMQ_SUBSCRIBE, filterimpvote, strlen(filterimpvote));
    zmq_setsockopt(subscriber, ZMQ_SUBSCRIBE, filterskipvote, strlen(filterskipvote));
    zmq_send(publisher, "AmongUs>impvote?>Choose the number of player who you want to kill: ", 67,0);
    printf("Imposters are voting...\n\n");
    sleep(1);

    //receive incoming imposters kill
    for (int i = 0; i < aantalimp; i++)
    {
            memset(buffer,0,256);
            zmq_recv(subscriber, buffer, 256, 0);
            ParsedString = parse(3, buffer);
            printf("%s\n", buffer);
            votes[i] = atoi(ParsedString);
    }

    //save votes
    int itemp = 0;
    int imost_votes = 0;
    int iskip = 0;
    int ivotecount[10] = {0};
    for (int i = 0; i < aantalimp ; i++)
    {
        if(votes[i] == 0 )
        {
            iskip++;
        }
        else
        {
            ivotecount[votes[i]-1] = ivotecount[votes[i]-1] +1;
        }

    }

    //list of players with amounts of votes
    for (int i = 0; i < rounds; i++)
    {
        printf("\n%s : %d", names[i], ivotecount[i]);
        if(imost_votes < ivotecount[i])
        {
           imost_votes = ivotecount[i];
           itemp = i;
        }
    }
    printf("\nSkipped votes = %d\n", iskip);

    if(iskip > ivotecount[itemp])
    {
        printf("\nNo one got killed\n");
        //itemp = 0;
    }
    else if (imost_votes%2)
    {
        printf("its a draw, no one got kicked\n");
    }
    else
    {
        printf("\n%s got killed\n", names[itemp]);
    }


    //loop to sent message to kicked or survived player
    printf("%d\n", itemp);
    zmq_setsockopt(subscriber, ZMQ_SUBSCRIBE, filterkick, strlen(filterkick));
    for (int i = 0; i <rounds ;i++)
    {
        if(iskip > ivotecount[itemp])
        {
            zmq_send(publisher, sentsurvive[i], strlen(sentsurvive[i]), 0);
        }
        else if (imost_votes%2==0)
        {
            zmq_send(publisher, sentsurvive[i], strlen(sentsurvive[i]), 0);
        }
        else if((names[i] == names[itemp] && rounds > 1))
        {
            zmq_send(publisher, sentkick[i], strlen(sentkick[i]), 0);
        }
        else
        {
            zmq_send(publisher, sentsurvive[i], strlen(sentsurvive[i]), 0);
        }
    }


    //statement to change imposter postition in list and subtract if needed
    if(itemp == 0)
    {
        printf("%d imp overgebleven\n", aantalimp);
    }
    if (itemp <= r)
    {
        if(itemp < r)
        {
           r--;
        }
        else if (itemp == r)
        {
            aantalimp--;
            printf("%d imposters left\n", aantalimp);
        }
    }
    else
    {
        printf("%d imposters left na die if statement --> dus ik zit in de else\n", aantalimp);
    }

    if (itemp <= j)
    {
        if(itemp < j)
        {
           j--;
        }
        else if (itemp == j)
        {
            aantalimp--;
            printf("%d imposters left\n", aantalimp);
        }
    }
    else
    {
        printf("%d imposters left na die if statement --> dus ik zit in de else\n", aantalimp);
    }

    //loop to clean up list
    for (int i = itemp; i<rounds; i++)
    {
        if(iskip > ivotecount[itemp])
        {
            printf("list unchanged\n");
        }
        else if (imost_votes%2)
        {
                printf("list unchanged22\n");
        }
        else
        {
            strcpy(sentkick[i], sentkick[i+1]);
            strcpy(sentsurvive[i], sentsurvive[i+1]);
            strcpy(names[i], names[i+1]);
        }
    }

    rounds--;

    printf("resterende rounds zijn %d\n\n", rounds);

    printf("remaining player list\n");
    if(iskip > ivotecount[itemp])
    {
        rounds++;
    }
    int som = 1;
    for (int i = 0; i < rounds; i++)
    {
        printf("%d. ", som);
        printf("%s\n", names[i]);
        som++;
    }
    itemp = 0;

//-----------------------------------------CREWMEMBERS-----------------------------------------------------------------------
    //voting system for crewmembers
    printf("\nCrewmembers are voting...\n\n");
    zmq_setsockopt(subscriber, ZMQ_SUBSCRIBE, filtervote, strlen(filtervote));
    sleep(1);
    zmq_send(publisher, "AmongUs>vote?>Choose the number of player who you want to vote: ", 64,0);

    //receive incoming votes
    for( int i = 0; i < rounds; i++)
    {
        memset(buffer,0,256);
        zmq_recv(subscriber, buffer, 256, 0);
        ParsedString = parse(3, buffer);
        printf("%s\n", buffer);
        votes[i] = atoi(ParsedString);
    }

    //save votes
    int temp = 0;
    int most_votes = 0;
    int skip = 0;
    int votecount[10] = {0};
    for (int i = 0; i < rounds ; i++)
    {
        if (votes[i] == 0)
        {
            skip++;
        }
        else
        {
            votecount[votes[i]-1] = votecount[votes[i]-1] +1;
        }
    }

    //list of players with amounts of votes
    for (int i = 0; i < rounds; i++)
    {
        printf("\n%s : %d", names[i], votecount[i]);
        if(most_votes < votecount[i])
        {
            most_votes = votecount[i];
            temp = i;
        }
    }

    printf("\nSkipped votes = %d\n", iskip);

    if(skip > votecount[temp])
    {
        printf("\nNo one got killed\n");

    }
    else if (most_votes%2 ==0)
    {
        printf("its a draw, no one got kicked\n");
    }
    else
    {
        printf("\n%s has the most votes of %d and is going to be kicked\n", names[temp], votecount[temp]);
    }


    printf("imp = %d\n\n", aantalimp);
    //loop to sent message to kicked or survived player
    zmq_setsockopt(subscriber, ZMQ_SUBSCRIBE, filterkick, strlen(filterkick));

    for (int i = 0; i <rounds ;i++)
    {
        if(skip>votecount[itemp])
        {
            zmq_send(publisher, sentsurvive[i], strlen(sentsurvive[i]), 0);
        }
        else if (most_votes%2 == 0)
        {
            zmq_send(publisher, sentsurvive[i], strlen(sentsurvive[i]), 0);
        }
        else if(names[i] == names[temp])
        {
            zmq_send(publisher, sentkick[i], strlen(sentkick[i]), 0);
        }
        else
        {
            zmq_send(publisher, sentsurvive[i], strlen(sentsurvive[i]), 0);
        }
    }

    //statement to change imposter postition in list and subtract if needed
    if(itemp == 0)
    {
        printf("%d imp overgebleven\n", aantalimp);
        goto l;
    }
    if (temp <= r)
    {
        if(temp < r)
        {
           r--;
        }
        else if (temp == r)
        {
            aantalimp--;
            printf("%d imposters left\n", aantalimp);
        }
    }
    else
    {
        printf("%d imposters left na die if statement --> dus ik zit in de else\n", aantalimp);
    }

    if (temp <= j)
    {
        if(temp < j)
        {
           j--;
        }
        else if (temp == j)
        {
            aantalimp--;
            printf("%d imposters left\n", aantalimp);
        }
    }
    else
    {
        printf("%d imposters left na die if statement --> dus ik zit in de else\n", aantalimp);
    }

    //loop to clean up list
    l: for (int i = temp; i<rounds; i++)
    {
        if(skip>votecount[temp])
        {
            printf("list unchanged\n");
        }
        else
        {
            strcpy(sentkick[i], sentkick[i+1]);
            strcpy(sentsurvive[i], sentsurvive[i+1]);
            strcpy(names[i], names[i+1]);
        }

    }

    rounds--;

    printf("resterende rounds zijn %d\n", rounds);

    printf("remaining player list\n");
    if(skip>votecount[itemp])
    {
        rounds++;
    }

    int som2 = 1;
    for (int i = 0; i < rounds; i++)
    {
        printf("%d. ", som2);
        printf("%s\n", names[i]);
        som2++;
    }
    temp = 0;

    //win condition
    sleep(1);
    printf("\nChecking wincondition...\n");
    zmq_setsockopt(subscriber, ZMQ_SUBSCRIBE, filterwinner, strlen(filterwinner));
    printf("imp = %d\n", aantalimp);
    printf("rounds = %d\n", rounds);
    if(aantalimp == 0)
    {
        //this text will be send to those who are still alive and need to receive the win text as well
        printf("new game bout to start send\n\n");
        zmq_send(publisher, "AmongUs>again?>new game bout to start", 37,0);
        sleep(1);
        zmq_send(publisher, "AmongUs>winner?>Congratulations!! Imposters are DEAD and Crewmembers won!", 74 ,0);
        goto a;
    }
    else if (aantalimp == 2 && rounds == 4)
    {
        //this text will be send to those who are still alive and need to receive the win text as well
        printf("new game bout to start send\n\n");
        zmq_send(publisher, "AmongUs>again?>new game bout to start", 37,0);
        sleep(1);
        zmq_send(publisher, "AmongUs>winner?>TUM TUM TUUUUMM... Imposters won!", 50 ,0);
        goto a;
    }
    else if (aantalimp == 1 && rounds == 2)
    {
        //this text will be send to those who are still alive and need to receive the win text as well
        printf("new game bout to start send\n\n");
        zmq_send(publisher, "AmongUs>again?>new game bout to start", 37,0);
        sleep(1);
        zmq_send(publisher, "AmongUs>winner?>TUM TUM TUUUUMM... Imposters won!", 50 ,0);
        goto a;
    }
    else
    {
        zmq_send(publisher, "AmongUs>again?>no win established", 34,0);
        printf("no win established\n");
    }


}
    a: sleep(1);
    zmq_setsockopt(subscriber, ZMQ_SUBSCRIBE, filteragain, 15);
    zmq_send(publisher, "AmongUs>again?>\n\t*************** A new game has started! ***************\n", 74,0);

}
    zmq_close (subscriber);
    zmq_close (publisher);
    zmq_ctx_destroy (context);
    return 0;


}

