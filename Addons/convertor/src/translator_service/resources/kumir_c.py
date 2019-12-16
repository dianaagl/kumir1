D = """
#include "kumir.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <limits.h>

#ifdef __cplusplus
#define BOOL bool
#define TRUE true
#define FALSE false
#else
#define BOOL _Bool
#define TRUE 1
#define FALSE 0
#endif

void* dynamic_arrays[500];
int dynamic_arrays_count = 0;

char* kum_concatenate_ss(const char *s1, const char *s2) {
    int size;
    if (strlen(s1) + strlen(s2) > 255)
        size = strlen(s1) + strlen(s2);
    else
        size = 255;
    char *result = (char*) calloc(size, sizeof(char));
    strcpy(result, s1);
    strcat(result, s2);
    dynamic_arrays[dynamic_arrays_count] = result;
    dynamic_arrays_count ++;
    return result;
}

char* kum_concatenate_sc(const char *s1, const char c2) {
    int size;
    if (strlen(s1) + 1 > 255)
        size = strlen(s1) + 1;
    else
        size = 255;
    char *result = (char*) calloc(size, sizeof(char));
    strcpy(result, s1);
    result[strlen(s1)] = c2;
    result[strlen(s1) + 1] = '\0';
    dynamic_arrays[dynamic_arrays_count] = result;
    dynamic_arrays_count ++;
    return result;
}

char* kum_concatenate_cs(const char c1, const char *s2) {
    int size;
    if (1 + strlen(s2) > 255)
        size = 1 + strlen(s2);
    else
        size = 255;
    char *result = (char*) calloc(size, sizeof(char));
    strcpy(result, s2);
    result[strlen(s2)] = c1;
    result[strlen(s2) + 1] = '\0';
    dynamic_arrays[dynamic_arrays_count] = result;
    dynamic_arrays_count ++;
    return result;
}

char *kum_string_of_integer(const int i) {
    char *result = calloc(10, sizeof(char));
    sprintf(result, "%d", i);
    dynamic_arrays[dynamic_arrays_count] = result;
    dynamic_arrays_count ++;
    return result;
}

int kum_integer_of_string(const char *s, BOOL *ok)
{
    errno = 0;
    long int result = strtol(s, NULL, 10);
    if (errno!=0) {
        *ok = FALSE;
        return 0;
    }
    if (result>=INT_MAX || result<INT_MIN) {
        *ok = FALSE;
        return 0;
    }
    *ok = TRUE;
    return (int)result;
}

double kum_double_of_string(const char *s, BOOL *ok)
{
    errno = 0;
    long int result = strtod(s, NULL);
    if (errno!=0) {
        *ok = FALSE;
        return 0;
    }
    *ok = TRUE;
    return (int)result;
}

char* kum_string_of_char(const char c)
{
    char* result = (char*)calloc(2,sizeof(char));
    result[0] = c;
    result[1] = '\0';
    dynamic_arrays[dynamic_arrays_count] = result;
    dynamic_arrays_count++;
    return result;
}

char* kum_substring(const char *s, const int start, const int end)
{
    char* result = (char*)calloc(end-start+2, sizeof(char));
    strncpy(result, s+start, end-start+1);
    dynamic_arrays[dynamic_arrays_count] = result;
    dynamic_arrays_count++;
    return result;
}

void kum_cleanup()
{
    int i;
    for (i=0; i<dynamic_arrays_count; i++) {
        free(dynamic_arrays[i]);
    }
}
"""
