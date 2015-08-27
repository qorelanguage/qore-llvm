#include "qore/runtime/runtime.h"
#include <iostream>
#include <cstring>

void print_qv(QoreValue qv) {
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

QoreValue make_int(int64_t value) {
    QoreValue qv;
    qv.tag = Tag::Int;
    qv.intValue = value;
    return qv;
}

QoreValue make_str(const char *value) {
    QoreValue qv;
    qv.tag = Tag::Str;
    qv.strValue = strdup(value);    //TODO strdup
    return qv;
}

static inline void append(std::string &dest, QoreValue v) {
    if (v.tag == Tag::Int) {
        dest += std::to_string(v.intValue);
    } else {
        dest += v.strValue;
    }
}

QoreValue eval_add(QoreValue l, QoreValue r) {
    if (l.tag == Tag::Int && r.tag == Tag::Int) {
        return make_int(l.intValue + r.intValue);
    }
    std::string str;
    append(str, l);
    append(str, r);
    return make_str(str.c_str());
}
