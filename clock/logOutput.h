#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEBUG 1

#ifdef DEBUG
#define ERROR(format, ...)	printf("file:[%s],line:[%d]: "format" ", __FILE__, __LINE__, ##__VA_ARGS__)
#else
#define ERROR(...)
#endif

#ifdef DEBUG
#define WARNING(format, ...)	printf("file:[%s],line:[%d]: "format" ", __FILE__, __LINE__, ##__VA_ARGS__)
#else
#define WARNING(...)
#endif

#ifdef DEBUG
#define INFO(format, ...)	printf("file:[%s],line:[%d]: "format" ", __FILE__, __LINE__, ##__VA_ARGS__)
#else
#define INFO(...)
#endif
