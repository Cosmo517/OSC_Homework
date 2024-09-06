#include <stdio.h>
#include "LuckyNumber.h"

int checkLuckyNumber(int guess)
{
    srand(time(NULL));
    int randomNumber = rand() % 5 + 1;
    if (guess == randomNumber)
        return 0;
    else if (guess < randomNumber)
        return -1;
    else
        return 1;
}
