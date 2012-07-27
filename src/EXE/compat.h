#ifndef __COMPAT_H_
#define __COMPAT_H_

#ifdef __linux__

#include <errno.h>

#define strcat_s(d,n,s) strncat(d,s,n)

#define strcpy_s(d,n,s) strncpy(d,s,n)

#define _strcmpi(a,b) strcasecmp(a,b)
#define stricmp(a,b) strcasecmp(a,b)
#define strnicmp(a,b,n) strncasecmp(a,b,n)

#define _itoa_s(i,d,l) snprintf(d,l,"%d",i)

int fopen_s(FILE** pFile, const char *filename, const char *mode) {
    FILE* file = fopen(filename, mode);
    *pFile = file;
    return errno;
}

#include <ctype.h>

char* strlwr( char* s )
  {
  char* p = s;
  while ((*p = tolower( *p ))) p++;
  return s;
  }

#define fprintf_s fprintf

#endif

#endif
