#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "threads/init.h"
#include "threads/malloc.h"
#include "threads/palloc.h"
#include "threads/thread.h"
#include "projects/pa/pa.h"

void run_patest(char **argv)
{   
    /// TODO: make your own test

    int numA = 25;
    int numB = 60;
    int numC = 16;
    int numD = 64;
    int numE = 18;

    printf("\n\n");
    // Request A(25)
    printf("Notice: request A(%d)\n", numA);
    size_t* a = palloc_get_multiple(PAL_USER, numA);
    palloc_get_status(PAL_USER);
    // Request B(60)
    printf("\n\n");
    printf("Notice: request B(%d)\n", numB);
    size_t* b = palloc_get_multiple(PAL_USER, numB);
    palloc_get_status(PAL_USER);
    // Request C(16)
    printf("\n\n");
    printf("Notice: request C(%d)\n", numC);
    size_t* c = palloc_get_multiple(PAL_USER, numC);
    palloc_get_status(PAL_USER);
    // Request D(64)
    printf("\n\n");
    printf("Notice: request D(%d)\n", numD);
    size_t* d = palloc_get_multiple(PAL_USER, numD);
    palloc_get_status(PAL_USER);
    // Release B
    printf("\n\nNotice: release B \n");
    palloc_free_multiple(b, numB);
    palloc_get_status(PAL_USER);
    // Release A
    printf("\n\nNotice: release A \n");
    palloc_free_multiple(a, numA);
    palloc_get_status(PAL_USER);
    // Request E(18)
    printf("\n\n");
    printf("Notice: request E(%d)\n", numE);
    size_t* e = palloc_get_multiple(PAL_USER, numE);
    palloc_get_status(PAL_USER);
    // Release C
    printf("\n\nNotice: release C \n");
    palloc_free_multiple(c, numC);
    palloc_get_status(PAL_USER);
    // Release E
    printf("\n\nNotice: release E \n");
    palloc_free_multiple(e, numE);
    palloc_get_status(PAL_USER);
    // Release D
    printf("\n\nNotice: release D \n");
    palloc_free_multiple(d, numD);
    palloc_get_status(PAL_USER);

    while (1) {
        timer_msleep(1000);
    }
}