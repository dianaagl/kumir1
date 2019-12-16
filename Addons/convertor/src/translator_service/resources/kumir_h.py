D = """
#ifndef __KUMIR_H
#define __KUMIR_H


#ifdef __cplusplus
extern "C" {
#endif
char* kum_concatenate_ss(const char *s1, const char *s2);
char* kum_concatenate_sc(const char *s1, const char c2);
char* kum_concatenate_cs(const char c1, const char *s2);
char* kum_string_of_integer(const int i);
#ifdef __cplusplus
int kum_integer_of_string(const char *s, bool *ok);
double kum_double_of_string(const char *s, bool *ok);
#else
int kum_integer_of_string(const char *s, _Bool *ok);
double kum_double_of_string(const char *s, _Bool *ok);
#endif
char* kum_string_of_char(const char c);
char* kum_substring(const char *s, const int start, const int end);

void kum_cleanup();

#ifdef __cplusplus
}
#endif

#endif

"""