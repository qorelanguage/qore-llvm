#include "qore/common/Logging.h"
#include "qore/runtime/runtime.h"
#include <algorithm>
#include <iostream>
#include <string>
#include <cassert>

#define QORE_LOG_COMPONENT "RUNTIME"

/**
 * \private
 */
class QoreString {
public:
    int refCount;
    std::string value;

    QoreString(const char *str) : refCount(1), value(str) {
        LOG("Creating QoreString " << this);
    }

    QoreString(std::string str) : refCount(1), value(std::move(str)) {
        LOG("Creating QoreString " << this);
    }

    ~QoreString() {
        LOG("Destroying QoreString " << this);
    }

    void deref() {
        if (!--refCount) {
            delete this;
        }
    }

private:
    QoreString(const QoreString &) = delete;
    QoreString(QoreString &&) = delete;
    QoreString &operator=(const QoreString &) = delete;
    QoreString &operator=(QoreString &&) = delete;
};


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
            printf("Qore Print: %s\n", qv->strValue->value.c_str());
//            std::cout << "Qore Print: " << qv.strValue << std::endl;
            break;
        case Tag::LValue:
            printf("Qore Print: LValue\n");
            break;
    }
}

__attribute__((visibility("default"))) void make_nothing(QoreValue *qv) {
    qv->tag = Tag::Nothing;
    qv->intValue = 0;
    LOG("make_nothing qv = " << qv);
}

__attribute__((visibility("default"))) void make_int(QoreValue *qv, int64_t value) {
    qv->tag = Tag::Int;
    qv->intValue = value;
    LOG("make_int qv = " << qv);
}

__attribute__((visibility("default"))) void make_str(QoreValue *qv, const char *value) {
    qv->tag = Tag::Str;
    qv->strValue = new QoreString(value);
    LOG("make_str qv = " << qv);
}

__attribute__((visibility("default"))) void make_lvalue(QoreValue *qv, QoreValue *value) {
    qv->tag = Tag::LValue;
    qv->qv = value;
    LOG("make_lvalue qv = " << qv);
}

static inline void append(std::string &dest, QoreValue *v) {
    if (v->tag == Tag::Nothing) {
        dest += "<Nothing>";
    } else if (v->tag == Tag::Int) {
        dest += std::to_string(v->intValue);
    } else if (v->tag == Tag::Str) {
        dest += v->strValue->value;
    } else {
        append(dest, v->qv);
    }
}

__attribute__((visibility("default"))) void eval_add(QoreValue *qv, QoreValue *l, QoreValue *r) {
    LOG("eval_add qv = " << qv << ", l = " << l << ", r = " << r);
    if (l->tag == Tag::Int && r->tag == Tag::Int) {
        make_int(qv, l->intValue + r->intValue);
        //deref(l);
        //deref(r);
        return;
    }
    std::string str;
    append(str, l);
    append(str, r);

    qv->tag = Tag::Str;
    qv->strValue = new QoreString(std::move(str));

    deref(l);
    deref(r);
}

static inline std::string trim(const std::string &s) {
   auto wsfront = std::find_if_not(s.begin(), s.end(), [](int c){return std::isspace(c);});
   auto wsback = std::find_if_not(s.rbegin(), s.rend(), [](int c){return std::isspace(c);}).base();
   return (wsback <= wsfront ? std::string() : std::string(wsfront, wsback));
}

__attribute__((visibility("default"))) void eval_trim(QoreValue *qv) {
    LOG("eval_trim qv = " << qv);
    assert(qv->tag == Tag::LValue && qv->qv->tag == Tag::Str);
    std::string str = trim(qv->qv->strValue->value);
    if (qv->qv->strValue->refCount == 1) {
        qv->qv->strValue->value = std::move(str);
    } else {
        qv->qv->strValue->deref();
        qv->qv->strValue = new QoreString(std::move(str));
    }
}

__attribute__((visibility("default"))) void eval_assign(QoreValue *l, QoreValue *r) {
    LOG("eval_assign l = " << l << ", r = " << r);

    deref(l->qv);

    if (r->tag == Tag::LValue) {
        *(l->qv) = *(r->qv);
    } else {
        *(l->qv) = *r;
    }
    ref(l->qv);
}

__attribute__((visibility("default"))) void deref(QoreValue *qv) {
    LOG("deref qv = " << qv);
    if (qv->tag == Tag::Str) {
        qv->strValue->deref();
    }
}

__attribute__((visibility("default"))) void ref(QoreValue *qv) {
    LOG("ref qv = " << qv);
    if (qv->tag == Tag::Str) {
        ++qv->strValue->refCount;
    }
}



std::ostream &operator<<(std::ostream &os, const QoreString *qs) {
    return os << static_cast<const void*>(qs) << ":{" << *qs << "}";
}

std::ostream &operator<<(std::ostream &os, const QoreString &qs) {
    return os << "[" << qs.refCount << "]'" << qs.value << "'";
}

std::ostream &operator<<(std::ostream &os, const QoreValue *qv) {
    return os << static_cast<const void*>(qv) << ":{" << *qv << "}";
}

std::ostream &operator<<(std::ostream &os, const QoreValue &qv) {
    if (qv.tag == Tag::Nothing) {
        return os << "nothing()";
    } else if (qv.tag == Tag::Int) {
        return os << "intValue(" << qv.intValue << ")";
    } else if (qv.tag == Tag::Str) {
        return os << "strValue(" << qv.strValue << ")";
    } else {
        return os << "lValue(" << qv.qv << ")";
    }
}

