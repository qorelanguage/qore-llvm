#include "qore/runtime/runtime.h"
#include <algorithm>
#include <iostream>
#include <cstring>

__attribute__((visibility("default"))) void print_qv(QoreValue *qv) {
    switch (qv->tag) {
        case Tag::Nothing:
            printf("Qore Print: <Nothing>\n");
            break;
        case Tag::Int:
            printf("Qore Print: %li\n", qv->intValue);
//XXX using std::cout prevents LTO from inlining this function
//            std::cout << "Qore Print: " << qv.intValue << std::endl;
            break;
        case Tag::Str:
            printf("Qore Print: %s\n", qv->strValue);
//            std::cout << "Qore Print: " << qv.strValue << std::endl;
            break;
    }
}

__attribute__((visibility("default"))) void make_nothing(QoreValue *qv) {
    qv->tag = Tag::Nothing;
}

__attribute__((visibility("default"))) void make_int(QoreValue *qv, int64_t value) {
    qv->tag = Tag::Int;
    qv->intValue = value;
}

__attribute__((visibility("default"))) void make_str(QoreValue *qv, const char *value) {
    qv->tag = Tag::Str;
    qv->strValue = strdup(value);    //TODO strdup
}

static inline void append(std::string &dest, QoreValue *v) {
    if (v->tag == Tag::Nothing) {
        dest += "<Nothing>";
    } else if (v->tag == Tag::Int) {
        dest += std::to_string(v->intValue);
    } else {
        dest += v->strValue;
    }
}

__attribute__((visibility("default"))) void eval_add(QoreValue *qv, QoreValue *l, QoreValue *r) {
    if (l->tag == Tag::Int && r->tag == Tag::Int) {
        make_int(qv, l->intValue + r->intValue);
        return;
    }
    std::string str;
    append(str, l);
    append(str, r);
    make_str(qv, str.c_str());
}

static inline std::string trim(const std::string &s)
{
   auto wsfront = std::find_if_not(s.begin(), s.end(), [](int c){return std::isspace(c);});
   auto wsback = std::find_if_not(s.rbegin(), s.rend(), [](int c){return std::isspace(c);}).base();
   return (wsback <= wsfront ? std::string() : std::string(wsfront, wsback));
}

__attribute__((visibility("default"))) void eval_trim(QoreValue *qv) {
    if (qv->tag == Tag::Str) {
        std::string str = trim(qv->strValue);
        qv->strValue = strdup(str.c_str());
    }
}

__attribute__((visibility("default"))) void eval_assign(QoreValue *l, QoreValue *r) {
    *l = *r;
}
