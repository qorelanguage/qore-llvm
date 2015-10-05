#include "qore/common/Logging.h"
#include "qore/runtime/runtime.h"
#include <algorithm>
#include <iostream>
#include <string>
#include <cassert>

#define QORE_LOG_COMPONENT "RUNTIME"

__attribute__((visibility("default"))) void strongDeref(QoreValue qv) {
    if (qv.tag == Tag::Str) {
        qv.strValue->deref();
    }
}

__attribute__((visibility("default"))) void strongDerefNothrow(QoreValue qv) noexcept {
    try {
        strongDeref(qv);
    } catch (...) {
        //link to an existing exception
    }
}

__attribute__((visibility("default"))) void unlock(QoreValue *qv) noexcept {
}

__attribute__((visibility("default"))) void strongRef(QoreValue qv) noexcept {
    if (qv.tag == Tag::Str) {
        qv.strValue->ref();
    }
}

__attribute__((visibility("default"))) void print_qv(QoreValue qv) noexcept {
    switch (qv.tag) {
        case Tag::Nothing:
            printf("Qore Print: <Nothing>\n");
            break;
        case Tag::Int:
            printf("Qore Print: %li\n", qv.intValue);
            break;
        case Tag::Str:
            printf("Qore Print: %s\n", qv.strValue->getValue().c_str());
            break;
    }
}

__attribute__((visibility("default"))) qore::rt::QoreString *qrt_alloc_string(const char *value) {
    return new qore::rt::QoreString(value);
}

__attribute__((visibility("default"))) void qrt_deref_string(qore::rt::QoreString *&str) {
    str->deref();
}

//__attribute__((visibility("default"))) const char *qrt_get_string(const QoreValue &v) noexcept {
//    assert(v.tag == Tag::Str);
//    return v.strValue->value.c_str();
//}
//
//__attribute__((visibility("default"))) QoreValue make_str(const char *value) noexcept {
//    QoreValue qv;
//    qv.tag = Tag::Str;
//    qv.strValue = new QoreString(value);
//    return qv;
//}

static inline void append(std::string &dest, QoreValue v) {
    if (v.tag == Tag::Nothing) {
        dest += "<Nothing>";
    } else if (v.tag == Tag::Int) {
        dest += std::to_string(v.intValue);
    } else if (v.tag == Tag::Str) {
        dest += v.strValue->getValue();
    } else {
        assert(false);
    }
}

__attribute__((visibility("default"))) QoreValue eval_add(QoreValue l, QoreValue r) {
    if (l.tag == Tag::Int && r.tag == Tag::Int) {
        QoreValue qv;
        qv.tag = Tag::Int;
        qv.intValue = l.intValue + r.intValue;
        return qv;
    }
    std::string str;
    append(str, l);
    append(str, r);

    QoreValue qv;
    qv.tag = Tag::Str;
    qv.strValue = new qore::rt::QoreString(std::move(str));
    return qv;
}

__attribute__((visibility("default"))) void eval_trim(QoreValue qv) {
    if (qv.tag == Tag::Str) {
        qv.strValue->trim();
    }
}

__attribute__((visibility("default"))) QoreValue load_unique(const QoreValue *qv) {
    if (qv->tag != Tag::Str) {
        return *qv;
    }
    if (qv->strValue->getRefCount() == 1) {
        qv->strValue->ref();
        return *qv;
    }
    QoreValue c;
    c.tag = Tag::Str;
    c.strValue = new qore::rt::QoreString(qv->strValue->getValue());
    return c;
}

__attribute__((visibility("default"))) bool eval_cond(QoreValue v) noexcept {
    switch (v.tag) {
        case Tag::Nothing:
            return false;
        case Tag::Int:
            return v.intValue != 0;
        case Tag::Str:
            return v.strValue->getValue() != "";
    }
    assert(false);
}

std::ostream &operator<<(std::ostream &os, const QoreValue *qv) {
    return qv ? os << static_cast<const void*>(qv) << ":{" << *qv << "}" : os << "nullptr";
}

std::ostream &operator<<(std::ostream &os, const QoreValue &qv) {
    if (qv.tag == Tag::Nothing) {
        return os << "nothing()";
    } else if (qv.tag == Tag::Int) {
        return os << "intValue(" << qv.intValue << ")";
    } else if (qv.tag == Tag::Str) {
        return os << "strValue(" << qv.strValue << ")";
    }
    assert(false);
}

std::ostream &operator<<(std::ostream &os, const QoreValueHolder &qv) {
    return os << qv.get();
}
