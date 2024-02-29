/***************************************************/
/*  Assumptions:                                   */
/*                                                 */
/*  1. Both the input and output files are to be   */
/*     text files, with one record per line        */
/*                                                 */
/*  2. No error file is needed                     */
/*                                                 */
/***************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "SASkatoon_2.h"

/* global variables */
NODE *pLL = NULL;


/* forward declarations */
static int validateDateFormat(char *dateStr);
static int inList(char *dateStr);
static void addToList(char *dateStr);
static void freeList(void);
static void trimTrailingWhitespace(char *dateStr);


/***************************************************/
/*  Function:    main                              */
/*                                                 */
/*  Description: Main entry point for the program  */
/*                                                 */
/***************************************************/
int main(void)
{
    int  i;
    int  retval         = 0;
    long numRecsInFile  = 0L;
    long numRecsWritten = 0L;
    long numDupes       = 0L;
    FILE *fpIn          = NULL;
    FILE *fpOut         = NULL;
    char *cp            = NULL;
    char *infile        = "./infile";
    char *outfile       = "./outfile";
    char inBuf[ISO_8601_LEN_MAX + 2];
    long error[ISO_8601_MAX_ERRORS];
    
    if ((fpIn = fopen(infile, "r")) == NULL)
    {
        fprintf(stderr, "Error: failed opening input file\n");
        perror(infile);
        return EXIT_FAILURE;
    }

    if ((fpOut = fopen(outfile, "w")) == NULL)
    {
        fprintf(stderr, "Error: failed opening output file\n");
        perror(outfile);
        return EXIT_FAILURE;
    }

    memset(error, 0, (ISO_8601_MAX_ERRORS * sizeof(long)));

    while (!feof(fpIn))
    {
        memset(inBuf, 0x0, sizeof(inBuf));
        fgets(inBuf, sizeof(inBuf), fpIn);
    
        numRecsInFile++;

        trimTrailingWhitespace(inBuf);

        if (inBuf[0] == '\0')
        {
            /* blank line, get next record */
            continue;
        }

        retval = validateDateFormat(inBuf);

        if (retval == 0)
        {    
            if (inList(inBuf))
            {
                numDupes++;
            }
            else
            {
                addToList(inBuf);
                fprintf(fpOut, "%s\n", inBuf);
                numRecsWritten++;
            }
        }
        else
        {
            error[retval]++;
        }
    }

    if (fpIn != NULL)
    {
        fclose(fpIn);
    }

    if (fpOut != NULL)
    {
        fclose(fpOut);
    }

    freeList();

    /* print processing stats */
    fprintf(stdout, "\n\n");
    fprintf(stdout, "Processing Stats\n");
    fprintf(stdout, "----------------\n");
    fprintf(stdout, "Dates written to file : %8ld\n\n", numRecsWritten);
    fprintf(stdout, "Duplicate dates       : %8ld\n\n", numDupes);

    for (i=1; i < ISO_8601_MAX_ERRORS; i++)
    {   
        printf("   %-17.17s  : %8ld\n", ISO_8601_ERROR_MSG[i][0], error[i]);
    }
    
    fprintf(stdout, "\n\n");

    return EXIT_SUCCESS;
}

/***************************************************/
/*  Function:    validateDateFormat                */
/*                                                 */
/*  Description: Validates a date string against   */
/*               the ISO 8601 specification        */
/*                                                 */
/***************************************************/
static int validateDateFormat(char *dateStr)
{
    int len = strlen(dateStr);
    char buf[ISO_8601_LEN_MAX + 1];
    ISO_8601_DATE *inDate;
    DT_PARTS parts;

    if (len != ISO_8601_LEN_GMT && len != ISO_8601_LEN_MAX)
    {
        return ISO_8601_INVALID_LEN;
    }

    memset(buf, 0x0, sizeof(buf));
    strcpy(buf, dateStr);

    inDate = (struct ISO_8601_date *)dateStr;

    memset(&parts, 0x0, sizeof(parts));
    strncpy(parts.YYYY, inDate->YYYY, sizeof(inDate->YYYY));
    parts.dash1 = inDate->dash1;
    strncpy(parts.MM, inDate->MM, sizeof(inDate->MM));
    parts.dash2 = inDate->dash2;
    strncpy(parts.DD, inDate->DD, sizeof(inDate->DD));
    parts.tee = inDate->tee;
    strncpy(parts.hh, inDate->hh, sizeof(inDate->hh)); 
    parts.colon1 = inDate->colon1; 
    strncpy(parts.mm, inDate->mm, sizeof(inDate->mm));
    parts.colon2 = inDate->colon2;
    strncpy(parts.ss, inDate->ss, sizeof(inDate->ss));  
    parts.TZD = inDate->TZD;

    if (len == ISO_8601_LEN_MAX)
    {
        strncpy(parts.tz_hh, inDate->tz_hh, sizeof(inDate->tz_hh));
        parts.colon3 = inDate->colon3;
        strncpy(parts.tz_mm, inDate->tz_mm, sizeof(inDate->tz_mm));
    }

/*  
    Not needed at this time, but included here 
    for the sake of completeness and in case the YYYY range
    needs to be limited in the future

    if (atoi(parts.YYYY) < 0 || atoi(parts.YYYY) > 9999)
    {
        return ISO_8601_INVALID_YEAR;
    }
*/

    if (parts.dash1 != '-')
    {
        return ISO_8601_INVALID_STRUCTURE;
    }

    if (atoi(parts.MM) < 1 || atoi(parts.MM) > 12)
    {
        return ISO_8601_INVALID_MONTH;
    }

    if (parts.dash2 != '-')
    {
        return ISO_8601_INVALID_STRUCTURE;
    }

    if (atoi(parts.DD) < 1 || atoi(parts.DD) > 31)
    {
        return ISO_8601_INVALID_DAY;
    }

    if (parts.tee != 'T')
    {
        return ISO_8601_INVALID_STRUCTURE;
    }

    if (atoi(parts.hh) < 0 || atoi(parts.hh) > 23)
    {
        return ISO_8601_INVALID_HOUR;
    }

    if (parts.colon1 != ':')
    {
        return ISO_8601_INVALID_STRUCTURE;
    }

    if (atoi(parts.mm) < 0 || atoi(parts.mm) > 59)
    {
        return ISO_8601_INVALID_MIN;
    }

    if (parts.colon2 != ':')
    {
        return ISO_8601_INVALID_STRUCTURE;
    }

    if (atoi(parts.ss) < 0 || atoi(parts.ss) > 59)
    {
        return ISO_8601_INVALID_SEC;
    }

    if (len == ISO_8601_LEN_GMT)
    {
        if (parts.TZD != 'Z')
        {
            return ISO_8601_INVALID_TZD;
        }

        if (parts.tz_hh[0] != '\0' || parts.tz_hh[1] != '\0')
        {
            return ISO_8601_INVALID_TZ_HOUR;
        }

        if (parts.colon3 != '\0')
        {
            return ISO_8601_INVALID_STRUCTURE;
        }
       
        if (parts.tz_mm[0] != '\0' || parts.tz_mm[1] != '\0')
        {
            return ISO_8601_INVALID_TZ_MIN;
        }
    }
    else
    {
        if (parts.TZD != '+' && parts.TZD != '-')
        {
            return ISO_8601_INVALID_TZD;
        }

        if (atoi(parts.tz_hh) < 0 || atoi(parts.tz_hh) > 23)
        {
            return ISO_8601_INVALID_TZ_HOUR;
        }

        if (parts.colon3 != ':')
        {
            return ISO_8601_INVALID_STRUCTURE;
        }

        if (atoi(parts.tz_mm) < 0 || atoi(parts.tz_mm) > 59)
        {
            return ISO_8601_INVALID_TZ_MIN;
        }
    }

    return EXIT_SUCCESS;
}

/***************************************************/
/*  Function:    addToList                         */
/*                                                 */
/*  Description: Adds a date to the linked list    */
/*               of unique dates                   */
/*                                                 */
/***************************************************/
void addToList(char *date)
{
    NODE *np  = NULL;
    NODE *cur = NULL;

    if ((np = (NODE *)calloc(1, sizeof(NODE))) == NULL)
    {
        fprintf(stderr, "Error: failed allocating memory\n");
        exit(EXIT_FAILURE);
    };

    strcpy(np->date, date);

    if (pLL == NULL)
    {
        pLL = np;
    }
    else
    {
        cur = pLL;
        while (cur->next != NULL)
        {
            cur = cur->next;
        }
        
        np->prev = cur;
        cur->next = np;
    }
}


/***************************************************/
/*  Function:    inList                            */
/*                                                 */
/*  Description: checks if supplied date exists    */
/*               in linked list                    */
/*                                                 */
/***************************************************/
static int inList(char *dateStr)
{
    NODE *cur = pLL;
    int retval = 0;

    if (cur == NULL)
    {
        return 0;
    }

    while (cur != NULL)
    {
        if (strcmp(dateStr, cur->date) == 0)
        {
            retval = 1;
            break;
        }
    
        cur = cur->next;
    }

    return retval;
}


/***************************************************/
/*  Function:    freeList                          */
/*                                                 */
/*  Description: frees the memory allocated        */
/*               for the linked list               */
/*                                                 */
/***************************************************/
static void freeList(void)
{
    NODE *cur = pLL;
    
    /* find last node in list */
    while (cur->next != NULL)
    {
        cur = cur->next;
    }

    while (cur->prev != NULL)
    {
        /* go to the previous node */
        cur = cur->prev;
        free(cur->next);
    }

    /* free the head of the list */
    free(cur);
}


/***************************************************/
/*  Function:    trimTrailingWhitespace            */
/*                                                 */
/*  Description: Trims trailing whitespace         */
/*               from the supplied string          */
/*                                                 */
/***************************************************/
static void trimTrailingWhitespace(char *dateStr)
{
    char *p = (dateStr + strlen(dateStr) - 1);
    while(p >= dateStr && isspace(*p))
    {
        *p = '\0';
         p--;
    }
}


/********************************************************************************
TODO:

1. To increase I/O efficiency, change the code to read multiple records at a time

3. To increase lookup/storage of linked list, implement binary search

********************************************************************************/

