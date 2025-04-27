#ifndef _assert_h
#define _assert_h

#include <stdio.h>
#include <stdlib.h>

#ifdef DEBUG
#define ASSERT_DOCHECKS
#endif

#ifdef ASSERT_DOCHECKS
#define assert(expr) ((expr) ? (void)0 : \
                      (fprintf(stderr, "Assertion Failed: \"%s\", file: \"%s\", line: %d.\n", \
                                #expr, __FILE__, __LINE__), \
                                assert_printstacktrace(), \
                                abort()))

void assert_printstacktrace(void);
#else
#define assert(expr)
#endif

#endif