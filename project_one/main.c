#include <stdio.h>
#include <ctype.h>
#include "LuckyNumber.h"
#include "record.h"

// wins, guessBelow, guessAbove

void main() 
{
    int userGuess = -1;
    char userPlayAgain = 'N';
    struct record guessRecord = { 0, 0, 0 };

    do 
    {
        do 
        {
            printf("Enter a guess between 1 and 5: ");
            scanf("%d", &userGuess);
        } while (userGuess < 1 || userGuess > 5);
        int checkNumber = checkLuckyNumber(userGuess);
        switch (checkNumber) 
        {
            case 0:
                printf("You guessed correctly!\n");
                guessRecord.wins++;
                break;
            case -1:
                printf("You guessed below the number!\n");
                guessRecord.guessBelow++;
                break;
            case 1:
                printf("You guessed above the number!\n");
                guessRecord.guessAbove++;
                break; 
        }
        printf("\nGuessed above the lucky number: %5d\n", guessRecord.guessAbove);
        printf("Guessed correctly: %18d\n", guessRecord.wins);
        printf("Guessed below the lucky number: %5d\n", guessRecord.guessBelow);
        
        do
        {
            printf("\nDo you want to play again? (Y/N): ");
            scanf(" %c", &userPlayAgain);
        } while (toupper(userPlayAgain) != 'Y' && toupper(userPlayAgain) != 'N');
    } while (toupper(userPlayAgain) == 'Y');

    printf("\n\nThank you for playing!\n");
}

