#ifndef _MISSING_H_
#define	_MISSING_H_

extern size_t
strlcat(char *dst, const char *src, size_t siz);

extern size_t
strlcpy(char *dst, const char *src, size_t siz);

extern int
clock_gettime(int clock __attribute__ ((unused)), struct timespec *ts);

char *
fgetln(FILE *fp, size_t *len);

#endif /* !_MISSING_H_ */
