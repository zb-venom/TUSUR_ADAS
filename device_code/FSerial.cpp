#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "FSerial.h"

void FakeSerial::println(const char *str)
{
    printf(str);
    printf("\n");
}

void FakeSerial::print(const char *str)
{
    printf(str);
}

void FakeSerial::println(const char c)
{
    printf("%c", c);
    printf("\n");
}

void FakeSerial::print(const char c)
{
    printf("%c", c);
}


FakeSerial Serial;