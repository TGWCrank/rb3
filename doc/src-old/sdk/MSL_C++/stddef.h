#ifndef _STDDEF_H
#define _STDDEF_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef NULL
#define NULL 0
#endif

#ifndef nullptr
#define nullptr NULL
#endif

#define offsetof(ST, M) ((size_t) & (((ST *)0)->M))

typedef signed long intptr_t;
typedef unsigned long uintptr_t;
typedef intptr_t ptrdiff_t;

typedef unsigned long size_t;

#ifndef __cplusplus
typedef unsigned short wchar_t;
#endif

typedef void (*funcptr_t)(void);

#ifdef __cplusplus
}
#endif

#endif
