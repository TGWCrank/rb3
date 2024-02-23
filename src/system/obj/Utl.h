#ifndef OBJ_UTL_H
#define OBJ_UTL_H
#include "obj/Object.h"
#include "utl/Symbol.h"

void InitObject(Hmx::Object*);
const char* PathName(const Hmx::Object*);
const char* SafeName(Hmx::Object*);

bool RecurseSuperClassesSearch(Symbol, Symbol);
bool IsASubclass(Symbol, Symbol);

// mergefilter classes go here

#endif
