/*
 * Log.c
 *
 * Obstarava kontrolni, chybove, ladici vypisy. Barevne odlisene od obycejnych vypisu.
 * Procedury zacinajici na Log pouzivaji pouze StdOut
 * Procedury zacinajici na Write pouzivaji StdErr
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include "Log.h"

#define PRINT 1

/**
 * Pise chybovou zpravu, cervene
 */
void LogError(char *msg)
{
	if (PRINT)
		printf("%s%s%s\n", COLOR_RED, msg, COLOR_NRM);
}

void WriteError(char *msg)
{
	if (PRINT)
		fprintf(stderr, "%s%s%s\n", COLOR_RED, msg, COLOR_NRM);
}

/**
 * Pise upozorneni, zlute
 */
void LogWarning(char *msg)
{
	if (PRINT)
		printf("%s%s%s\n", COLOR_YEL, msg, COLOR_NRM);
}

void WriteWarning(char *msg)
{
	if (PRINT)
		fprintf(stderr, "%s%s%s\n", COLOR_YEL, msg, COLOR_NRM);
}

/**
 * Pise debugovaci vypis, modre
 */
void LogDebug(char *msg)
{
	if (PRINT)
		printf("%s%s%s\n", COLOR_CYN, msg, COLOR_NRM);
}

void WriteLog(char *msg)
{
	if (PRINT)
		fprintf(stderr, "%s%s%s\n", COLOR_CYN, msg, COLOR_NRM);
}
