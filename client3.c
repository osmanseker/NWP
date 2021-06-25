#include <stdio.h>
#include <zmq.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <stdbool.h>

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
    const char *BerichtPlayer = (argc > 1)? argv [1]: "AmongUs>player3!>";


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
//put whole game in a do-while loop
while(1)
{
    //variables
    bool imposter = false;
    int players = 6;
    char sendvote[20];
    char tempvote[3];
    int votelim = 0;
    char name[15];
    char sendname[50];
    char buffer [256];
    char *ParsedString;

    //subscribing to player
    zmq_setsockopt (subscriber, ZMQ_SUBSCRIBE, "AmongUs>Player?>", 16);

    //asking player for name
    printf("Enter your name: ");
    scanf("%s", name);
    printf("waiting to join...\n");

    //parsing text to send
    strcpy(sendname, BerichtPlayer);
    strcat(sendname, name);

    //sending name to server
    rp = zmq_send(publisher, sendname, strlen(sendname), 0);

    //receive message of joined names
    memset(buffer,0,256);
    zmq_recv(subscriber, buffer, 256,0);
    ParsedString = parse(3, buffer);
    printf("%s\n", ParsedString);

    zmq_setsockopt(subscriber, ZMQ_UNSUBSCRIBE, "AmongUsPlayer?>", 16 );
    zmq_setsockopt (subscriber, ZMQ_SUBSCRIBE, "AmongUs>imposter3?>", 19);
    zmq_setsockopt (subscriber, ZMQ_SUBSCRIBE, "AmongUs>crewmember3?>", 21);
    memset(buffer,0,256);
    zmq_recv(subscriber, buffer, 256,0);
    ParsedString = parse(3, buffer);
    printf("%s\n", ParsedString);

    if((strcmp(ParsedString, "You are imposter\n")) == 0)
    {
        imposter = true;
        zmq_setsockopt(subscriber, ZMQ_SUBSCRIBE, "AmongUs>impvote?>", 17);
    }
    else
    {
        printf("Imposters are voting...\n\n");
        goto k;
    }


    zmq_setsockopt(subscriber, ZMQ_UNSUBSCRIBE, "AmongUs>imposter3?>", 19);
    zmq_setsockopt(subscriber, ZMQ_UNSUBSCRIBE,"AmongUs>crewmember3?>", 21);

    for (int i = 0 ;i<players;i++)
    {
        //if imposter, receive message to kill
        if(imposter == true)
        {
            memset(buffer,0,256);
            zmq_recv(subscriber, buffer, 256,0);
            ParsedString = parse(3, buffer);
            printf("%s\n", ParsedString);
        }

        if((strcmp(ParsedString, "Choose the number of player who you want to kill: ")) == 0)
        {
            scanf("%d", &votelim);
            sprintf(tempvote, "%d" ,votelim );
            if(votelim == 0)
            {
                zmq_setsockopt(subscriber, ZMQ_SUBSCRIBE, "AmongUs>skipvote?>", 18);
                printf("vote skipped\n");
                strcpy(sendvote, BerichtPlayer);
                strcat(sendvote, tempvote);
                zmq_send(publisher, sendvote, strlen(sendvote),0);
            }
            else
            {
                strcpy(sendvote, BerichtPlayer);
                strcat(sendvote, tempvote);
                zmq_send(publisher, sendvote, strlen(sendvote),0);
            }
        zmq_setsockopt(subscriber, ZMQ_UNSUBSCRIBE,"AmongUs>skipvote?>", 18);

        k: zmq_setsockopt(subscriber, ZMQ_SUBSCRIBE, "AmongUs>kick3?>", 15);
        memset(buffer,0,256);
        zmq_recv(subscriber, buffer, 256,0);
        ParsedString = parse(3, buffer);
        printf("%s\n", ParsedString);

        if((strcmp(ParsedString, "You have been kicked\n")) == 0)
        {
           goto e;
        }


    //receive message "choose to vote"
    zmq_setsockopt (subscriber, ZMQ_SUBSCRIBE, "AmongUs>vote?>", 14);
    memset(buffer,0,256);
    zmq_recv(subscriber, buffer, 256,0);
    ParsedString = parse(3, buffer);
    printf("%s\n", ParsedString);

    //code to send vote
    do
    {
        scanf("%d", &votelim);
        if(votelim == 0)
        {
            zmq_setsockopt(subscriber, ZMQ_SUBSCRIBE, "AmongUs>skipvote?>", 18);
            printf("vote skipped\n");
            strcpy(sendvote, BerichtPlayer);
            strcat(sendvote, tempvote);
            zmq_send(publisher, sendvote, strlen(sendvote),0);
            goto s;
        }

    }while(votelim > 10 || votelim < 1);

    sprintf(tempvote, "%d" ,votelim );
    strcpy(sendvote, BerichtPlayer);
    strcat(sendvote, tempvote);

    zmq_send(publisher, sendvote, strlen(sendvote),0);

    s:printf("waiting for votes to be counted...\n\n");

    //receive message if kicked or not
    zmq_setsockopt(subscriber, ZMQ_SUBSCRIBE, "AmongUs>kick3?>", 15);

    memset(buffer,0,256);
    zmq_recv(subscriber, buffer, 256,0);
    ParsedString = parse(3, buffer);
    printf("%s\n", ParsedString);

    e: if((strcmp(ParsedString, "You have been kicked\n")) == 0)
    {
        zmq_setsockopt(subscriber, ZMQ_UNSUBSCRIBE, "AmongUs>vote?>", 14 );
        zmq_setsockopt(subscriber, ZMQ_UNSUBSCRIBE, "AmongUs>kick3?>", 15 );
        zmq_setsockopt(subscriber, ZMQ_UNSUBSCRIBE, "AmongUs>impvote?>", 17 );
        goto w;
    }
    else
    {
        printf("Imposters are voting...\n\n");
    }

    //receive "new game bout to start" in again channel
    zmq_setsockopt(subscriber, ZMQ_SUBSCRIBE, "AmongUs>again?>", 15);
    memset(buffer,0,256);
    zmq_recv(subscriber, buffer, 256,0);
    ParsedString = parse(3, buffer);
    printf("%s\n", ParsedString);

    if((strcmp(ParsedString, "new game bout to start")) == 0)
    {
        goto w;
    }
    else
    {
       zmq_setsockopt(subscriber, ZMQ_UNSUBSCRIBE, "AmongUs>again?>", 15);
    }
}

    //receive win message
    w: printf("Wait untill the game is done...\n\n");
    zmq_setsockopt(subscriber, ZMQ_SUBSCRIBE, "AmongUs>winner?>", 16);
    memset(buffer,0,256);
    zmq_recv(subscriber, buffer, 256,0);
    ParsedString = parse(3, buffer);
    printf("%s\n", ParsedString);
    zmq_setsockopt(subscriber, ZMQ_UNSUBSCRIBE, "AmongUs>winner?>", 16);

    //receive "new game"
    zmq_setsockopt(subscriber, ZMQ_SUBSCRIBE, "AmongUs>again?>", 15);
    memset(buffer,0,256);
    zmq_recv(subscriber, buffer, 256,0);
    ParsedString = parse(3, buffer);
    printf("%s\n", ParsedString);

    zmq_setsockopt(subscriber, ZMQ_UNSUBSCRIBE, "AmongUs>Player?>", 16 );
    zmq_setsockopt(subscriber, ZMQ_UNSUBSCRIBE, "AmongUs>impvote?>", 17 );
    zmq_setsockopt(subscriber, ZMQ_UNSUBSCRIBE, "AmongUs>vote?>", 14);
    zmq_setsockopt(subscriber, ZMQ_UNSUBSCRIBE, "AmongUs>kick1?>", 15 );
    zmq_setsockopt(subscriber, ZMQ_UNSUBSCRIBE, "AmongUs>winner?>", 16 );
    zmq_setsockopt(subscriber, ZMQ_UNSUBSCRIBE, "AmongUs>again?>", 15 );
    sleep(4);
}

    zmq_close (publisher);
    zmq_close (subscriber);
    zmq_ctx_destroy (context);
    return 0;
}
}
