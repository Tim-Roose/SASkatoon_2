/***************************************************************/
/*                                                             */
/*  File:        SASkatoon_2.h                                 */
/*  Description: Header file for the interview prompt program  */
/*                                                             */
/***************************************************************/
 
#define ISO_8601_LEN_GMT   20
#define ISO_8601_LEN_MAX   25

#define ISO_8601_INVALID_LEN        1
#define ISO_8601_INVALID_STRUCTURE  2
#define ISO_8601_INVALID_YEAR       3
#define ISO_8601_INVALID_MONTH      4
#define ISO_8601_INVALID_DAY        5
#define ISO_8601_INVALID_HOUR       6
#define ISO_8601_INVALID_MIN        7
#define ISO_8601_INVALID_SEC        8
#define ISO_8601_INVALID_TZD        9
#define ISO_8601_INVALID_TZ_HOUR   10
#define ISO_8601_INVALID_TZ_MIN    11

#define ISO_8601_MAX_ERRORS        12

char *ISO_8601_ERROR_MSG[ISO_8601_MAX_ERRORS][sizeof(char *)] = \
                        { {"No error"},          \
                          {"Invalid length"},    \
                          {"Invalid structure"}, \
                          {"Invalid year"},      \
                          {"Invalid month"},     \
                          {"Invalid day"},       \
                          {"Invalid hour"},      \
                          {"Invalid min"},       \
                          {"Invalid sec"},       \
                          {"Invalid TZD"},       \
                          {"Invalid tz hour"},   \
                          {"Invalid tz min"}     \
                        };


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

