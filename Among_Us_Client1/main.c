#include <stdio.h>
#include <zmq.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

char * strsep(char **stringp, const char *delim)
{
    char *s  = NULL;
    char *tok = NULL;
    const char *spanp;
    int c, sc;

    if (stringp == NULL) return (NULL);   /* empty stringp */
    s = stringp;
    for (tok = s;;)
     {
        c = *s++;
        spanp = delim;
        do
          {
            if((sc = *spanp++) == c)
              {
                if (c == 0) s = NULL;
                else
                    s[-1] = 0;
                *stringp = s;
                return (tok);
            }
        } while (sc);
    }

    /* should not get here */
   fprintf(stderr, "strsep error\n");
   exit(-1);
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
    void *context = zmq_ctx_new();
    void *publisher = zmq_socket(context, ZMQ_PUSH);
    void *subscriber = zmq_socket (context, ZMQ_SUB);

    //connect
    int rp = zmq_connect(publisher, "tcp://benternet.pxl-ea-ict.be:24041");
    int rs = zmq_connect( subscriber, "tcp://benternet.pxl-ea-ict.be:24042" );
    printf("connecting to server...\n");
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
//--------------------------------------------------------------------------------------------------------------
    //bericht
    const char *berichtJoin = (argc > 1)? argv [1]: "AmongUs>join?>";

    //filter instellen
    const char *filterJoin = (argc > 1)? argv [1]: "AmongUs>join?>";

    //making chars
    char buf [256];
    char *ParsedString;
    char name[10];
    char textJoin[60];

    buf[0] ='\0';

    //asking player for name
    printf("Enter your name: ");
    scanf("%s", name);

    //parsing text to send
    strcpy(textJoin, berichtJoin);
    strcat(textJoin, name);

    rs = zmq_setsockopt (subscriber, ZMQ_SUBSCRIBE, "AmongUs>player1?>", 17);

    //sending join with name to server
    rp = zmq_send(publisher, textJoin, strlen(textJoin), 0);
    assert (rp == strlen(textJoin));



    zmq_close( publisher );
    zmq_close( subscriber );
    zmq_ctx_shutdown( context ); //optional for cleaning lady order (get ready you l*zy f*ck)
    zmq_ctx_term( context ); //cleaning lady goes to work

}
