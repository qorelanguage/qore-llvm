#include "qore/runtime/runtime.h"
#include <iostream>

void print_qv(struct QoreValue qv) {
    switch (qv.tag) {
        case Tag::Int:
            std::cout << "Qore Print: " << qv.intValue << std::endl;
            break;
        case Tag::Str:
            std::cout << "Qore Print: " << qv.strValue << std::endl;
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
