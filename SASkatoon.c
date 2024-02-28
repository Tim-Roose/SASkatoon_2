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

#define ISO_8601_LEN_GMT   20
#define ISO_8601_LEN_MAX   25

typedef struct ISO_8601_date
{
    char YYYY[4];
    char dash1;
    char MM[2];
    char dash2;
    char DD[2];
    char tee;
    char hh[2];
    char colon1;
    char mm[2];
    char colon2;
    char ss[2];
    char TZD;
    char tz_hh[2];
    char colon3;
    char tz_mm[2];
} ISO_8601_DATE;

typedef struct date_time_parts
{
    char YYYY[4+1];
    char dash1;
    char MM[2+1];
    char dash2;
    char DD[2+1];
    char tee;
    char hh[2+1];
    char colon1;
    char mm[2+1];
    char colon2;
    char ss[2+1];
    char TZD;
    char tz_hh[2+1];
    char colon3;
    char tz_mm[2+1];
} DT_PARTS;

typedef struct node
{
    char date[ISO_8601_LEN_MAX + 1];
    struct node *prev;
    struct node *next;
} NODE;

NODE **pLL = NULL;


/* forward declarations */
int validateDateFormat(char *dateStr);


int main(void)
{
    FILE *fpIn    = NULL;
    FILE *fpOut   = NULL;
    char *infile  = "./infile";
    char *outfile = "./outfile";
    char inBuf[100];
    char *cp      = NULL;

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

    while (!feof(fpIn))
    {
        memset(inBuf, 0x0, sizeof(inBuf));
        fgets(inBuf, sizeof(inBuf), fpIn);
    
        if ((cp = strchr(inBuf, '\n')) != NULL)
        {
            *cp = 0x0;
        }
        else
        {
            ; // add  error checking
        }

        if (!validateDateFormat(inBuf))
        {    
            // if (!inLinkedList(inBuf))
            // {
            //    add to linked list

                  fprintf(fpOut, "%s\n", inBuf);
            // }
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

    return EXIT_SUCCESS;
}

/*
int validateDateFormat(char *dateStr)
{
    int i;
    int len = strlen(dateStr);
    char buf[ISO_8601_LEN_MAX + 1];

    if (len != ISO_8601_LEN_GMT && len != ISO_8601_LEN_MAX)
    {
        return EXIT_FAILURE;
    }
    
    strcpy(buf, dateStr);

    for (i=0; i<len; i++)
    {

        switch (i)
        {
                     //  1901-01-01T01:01:01+01:01

            case 0:   // YYYY
                 1:  
                 2:  
                 3:  
                 5:   // MM
                 6:
                 8:   // DD
                 9:
                11:   // hh
                12:
                14:   // mm
                15:
                17:   // ss
                18:
                20:
                21:
                23:
                24:
                      if (!isdigit(buf[i]))
                      {
                          return EXIT_FAILURE;
                      }
                      break;

                 4:
                 7:
                      if (buf[i] != '-')
                      {
                          return EXIT_FAILURE;
                      }
                      break;

                10:
                      if (buf[i] != 'T')
                      {
                          return EXIT_FAILURE;
                      }
                      break;

                13:
                16:
                22:
                      if (buf[i] != ':')
                      {
                          return EXIT_FAILURE;
                      }
                      break;

                19:
                      if (buf[i] != '+' && buf[i] != '-')
                      {
                          return EXIT_FAILURE;
                      }
                      break;
        }

    ...

    }

    return EXIT_SUCCESS;
}
*/

int validateDateFormat(char *dateStr)
{
    int len = strlen(dateStr);
    char buf[ISO_8601_LEN_MAX + 1];
    ISO_8601_DATE *inDate;
    DT_PARTS parts;

    if (len != ISO_8601_LEN_GMT && len != ISO_8601_LEN_MAX)
    {
        return EXIT_FAILURE;
    }

    memset(buf, 0x0, sizeof(buf));
    strcpy(buf, dateStr);

    inDate = (struct ISO_8601_date *)dateStr;

    memset(&parts, 0x0, sizeof(parts));
    strncpy(parts.YYYY, inDate->YYYY, sizeof(inDate->YYYY));
    parts.dash1 = inDate->dash1;
    strncpy(parts.MM,   inDate->MM,   sizeof(inDate->MM));
    parts.dash2 = inDate->dash2;
    strncpy(parts.DD,   inDate->DD,   sizeof(inDate->DD));
    parts.tee = inDate->tee;
    strncpy(parts.hh,   inDate->hh,   sizeof(inDate->hh)); 
    parts.colon1 = inDate->colon1; 
    strncpy(parts.mm,   inDate->mm,   sizeof(inDate->mm));
    parts.colon2 = inDate->colon2;
    strncpy(parts.ss,   inDate->ss,   sizeof(inDate->ss));  
    parts.TZD = inDate->TZD;

    if (len == ISO_8601_LEN_MAX)
    {
        strncpy(parts.tz_hh, inDate->tz_hh, sizeof(inDate->tz_hh));
        parts.colon3 = inDate->colon3;
        strncpy(parts.tz_mm, inDate->tz_mm, sizeof(inDate->tz_mm));
    }

    if (atoi(parts.MM) < 1 || atoi(parts.MM) > 12)
    {
        return EXIT_FAILURE;
    }

    if (atoi(parts.DD) < 1 || atoi(parts.DD) > 31)
    {
        return EXIT_FAILURE;
    }

    if (atoi(parts.hh) < 0 || atoi(parts.hh) > 23)
    {
        return EXIT_FAILURE;
    }

    if (atoi(parts.mm) < 0 || atoi(parts.mm) > 59)
    {
        return EXIT_FAILURE;
    }

    if (atoi(parts.ss) < 0 || atoi(parts.ss) > 59)
    {
        return EXIT_FAILURE;
    }

    if (len == ISO_8601_LEN_GMT)
    {
       if (parts.TZD != 'Z')
       {
           return EXIT_FAILURE;
       }
    }
    else
    {
        if (parts.TZD != '+' && parts.TZD != '-')
        {
            return EXIT_FAILURE;
        }

        if (atoi(parts.tz_hh) < 0 || atoi(parts.tz_hh) > 23)
        {
            return EXIT_FAILURE;
        }

        if (atoi(parts.tz_mm) < 0 || atoi(parts.tz_mm) > 59)
        {
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}



/********************************************************************************
TODO:

1. create linked list to store valid dates that have been written to the output file

2. To increase I/O efficiency, change the code to read multiple records at a time

3. To increase lookup/storage of linked list, implement binary search

********************************************************************************/

