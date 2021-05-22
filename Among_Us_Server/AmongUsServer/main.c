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
    printf("service starting...\n");
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

    //Bericht
    const char *berichtJoin = (argc > 1)? argv [1]: "AmongUs>join!>";

    //filter instellen
    const char *filterplayer1 = (argc > 1)? argv [1]: "AmongUs>player1!>";
    const char *filterplayer2 = (argc > 1)? argv [1]: "AmongUs>player2!>";
    const char *filterplayer3 = (argc > 1)? argv [1]: "AmongUs>player3!>";
    const char *filterplayer4 = (argc > 1)? argv [1]: "AmongUs>player4!>";

    const char *filterJoin = (argc > 1)? argv [1]: "AmongUs>join!>";


    zmq_setsockopt(subscriber, ZMQ_SUBSCRIBE, filterplayer1, 14);
    zmq_setsockopt(subscriber, ZMQ_SUBSCRIBE, filterplayer2, 14);
    zmq_setsockopt(subscriber, ZMQ_SUBSCRIBE, filterplayer3, 14);
    zmq_setsockopt(subscriber, ZMQ_SUBSCRIBE, filterplayer4, 14);

    //Making chars
    char buf [256];
    char *names[3][15];
    int rnd = rand()%10;
    srand(time(0));

    zmq_setsockopt (subscriber, ZMQ_SUBSCRIBE, "AmongUs>",8);

    for (int i = 0; i<4 ;i++)
    {
        memset(buf,0,256);
        zmq_recv(subscriber, buf , 256,0);
        names[i][15] = parse(3,buf);
        printf("%s has joined\n", names[i][15]);
    }


}

