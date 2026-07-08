#include <stdio.h>
#include "License_Checker.h"

#define TOC_BUFFER_SIZE 30
#define APP_NAME_BUFFER_SIZE 10

int main(void)
{
    INT8 appname[APP_NAME_BUFFER_SIZE] = "CORE";
    INT8 toc[TOC_BUFFER_SIZE] = "Fri 2026-02-6 11:00:40 UTC";

    INT32 status = checkLicense(appname, toc);
    printf("Helper License Check Status: %ld\n", (long)status);

    return (status == LICENSE_VALID) ? 0 : 1;
}