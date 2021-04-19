#include <stdio.h>
#include <time.h>

int main()
{
    int arrsize = 10;
    char names[10][15];
    int votes[10] = {0};
    int most_votes = votes[0];
    int opsomming = 1;
    int j = 0;
    char c;
    srand(time(0));
/*
    //Welcoming message
    printf("\n\t\t\t\tWELCOME TO AMONG US IN C\n\n");
    printf("\nThe rules are very simpel: Don't get voted out as crewmate OR don't get killed by the imposters!\n");
    printf("\nThe goals is to vote the imposters out before they kill all the crewmates!\n");
    printf("\n**************************************************************************\n\n\n");
*/
    //storing 10 player names in array
    printf("Enter player names: \n\n");
    for (int i = 0; i < 10;i++)
    {
        printf("%d.", opsomming);
        scanf("%s" , names[i]);
        opsomming++;
    }

    do
    {
        //Selecting 2 imposters
        printf("\n10 player names are saved!\nNow, 2 imposters are going to be picked! DUM DUM DUUUUUUM\n\n");

        int i = rand()%10;
        printf("%s\n", names[i]);
        do
        {
            j= rand()%10;
        }
        while(i == j);
        printf("%s\n", names[j]);

        //Voing system for imposters
        int kill;
        do
        {
            printf("imposters, choose the number of the person you want to kill: \n");
            scanf("%d" , &kill);

        }while(kill > 10 || kill < 1);

        for (i = kill; i < 10; i++)
        votes[i] = votes[i+1];

        if(kill < 0 || kill > 10 )
        {
                printf("Invalid vote");
        }
        else
        {
          //Traverse an array
          for(i = kill-1; i <10; i++)
          {
              votes[i] = votes[i+1];
          }
          arrsize--;

          printf("Remaining players\n");
          for(i = 0; i < 10; i++)
          {
                printf("%d\n", votes[i]);
          }
        }

        printf("The imposters killed %d", kill);

        //Voting system for crewmembers
        printf("\n\nCrewmembers...time to vote...\n");
         for (int i = 0;i < 10;i++)
         {
            int persoon = 0;

            do
            {
                printf("\n%s choose the number of the player who you want to vote out: ", names[i]);
                scanf("%d" , &persoon);

            }while(persoon > 10 || persoon < 1);


            votes[persoon -1] = votes[persoon -1] +1;

         }

         for (int i = 0; i < 10;i++)
         {
             printf("\n%s: %d\n", names[i], votes[i]);
         }

         printf("and the person that is going to be kicked is: \n");


         int temp = 0;
         for (int i = 0; i < 10 ; i++)
         {
             if(most_votes < votes[i])
             {
                 most_votes = votes[i];
                 temp = i;
             }
         }
         printf("\n%s\n", names[temp]);
         printf("do you want to play again? y or n?\n");
         scanf("%s", &c);
         if(c == 'y')
         {
            printf("\n\t\t*************** A new round has started! ***************\n");
         }
         else
         {
                printf("\nThank you for playing! See you next time!\n");
         }
    }while (c == 'y');


    return 0;
}

