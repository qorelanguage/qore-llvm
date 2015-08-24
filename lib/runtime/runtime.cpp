#include "qore/runtime/runtime.h"
#include <iostream>

void print_qv(struct QoreValue qv) {
    switch (qv.tag) {
        case Tag::Int:
            printf("Qore Print: %li\n", qv.intValue);
//XXX using std::cout prevents LTO from inlining this function
//            std::cout << "Qore Print: " << qv.intValue << std::endl;
            break;
        case Tag::Str:
            printf("Qore Print: %s\n", qv.strValue);
//            std::cout << "Qore Print: " << qv.strValue << std::endl;
            break;
    }
}

struct QoreValue make_int(int64_t value) {
    struct QoreValue qv;
    qv.tag = Tag::Int;
    qv.intValue = value;
    return qv;
}

struct QoreValue make_str(const char *value) {
    struct QoreValue qv;
    qv.tag = Tag::Str;
    qv.strValue = value;
    return qv;
}
