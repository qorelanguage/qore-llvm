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

__attribute__((visibility("default"))) void deref(LValue qv) noexcept {
    LOG("deref qv = " << qv);
    if (qv->tag == Tag::Str) {
        qv->strValue->deref();
    }
    qv->tag = Tag::Nothing;
    qv->intValue = 0;
}

static void ref(LValue qv) {
    LOG("ref qv = " << qv);
    if (qv->tag == Tag::Str) {
        ++qv->strValue->refCount;
    }
}

__attribute__((visibility("default"))) void print_qv(Value qv) noexcept {
    switch (qv.tag) {
        case Tag::Nothing:
            printf("Qore Print: <Nothing>\n");
            break;
        case Tag::Int:
            printf("Qore Print: %li\n", qv.intValue);
//XXX using std::cout prevents LTO from inlining this function
//            std::cout << "Qore Print: " << qv.intValue << std::endl;
            break;
        case Tag::Str:
            printf("Qore Print: %s\n", qv.strValue->value.c_str());
//            std::cout << "Qore Print: " << qv.strValue << std::endl;
            break;
    }
}

__attribute__((visibility("default"))) void make_int(LValue qv, int64_t value) noexcept {
    qv->tag = Tag::Int;
    qv->intValue = value;
    LOG("make_int qv = " << qv);
}

__attribute__((visibility("default"))) void make_str(LValue qv, const char *value) noexcept {
    qv->tag = Tag::Str;
    qv->strValue = new QoreString(value);
    LOG("make_str qv = " << qv);
}

__attribute__((visibility("default"))) void make_nothing(LValue qv) noexcept {
    qv->tag = Tag::Nothing;
    LOG("make_nothing qv = " << qv);
}

static inline void append(std::string &dest, Value v) {
    if (v.tag == Tag::Nothing) {
        dest += "<Nothing>";
    } else if (v.tag == Tag::Int) {
        dest += std::to_string(v.intValue);
    } else if (v.tag == Tag::Str) {
        dest += v.strValue->value;
    } else {
        assert(false);
    }
}

__attribute__((visibility("default"))) void eval_add(LValue qv, Value l, Value r) noexcept {
    LOG("eval_add qv = " << qv << ", l = " << l << ", r = " << r);
    if (l.tag == Tag::Int && r.tag == Tag::Int) {
        make_int(qv, l.intValue + r.intValue);
        return;
    }
    std::string str;
    append(str, l);
    append(str, r);

    deref(qv);
    qv->tag = Tag::Str;
    qv->strValue = new QoreString(std::move(str));
}

static inline std::string trim(const std::string &s) {
   auto wsfront = std::find_if_not(s.begin(), s.end(), [](int c){return std::isspace(c);});
   auto wsback = std::find_if_not(s.rbegin(), s.rend(), [](int c){return std::isspace(c);}).base();
   return (wsback <= wsfront ? std::string() : std::string(wsfront, wsback));
}

__attribute__((visibility("default"))) void eval_trim(LValue qv) noexcept {
    LOG("eval_trim qv = " << qv);
    assert(qv->tag == Tag::Str);
    std::string str = trim(qv->strValue->value);
    if (qv->strValue->refCount == 1) {
        qv->strValue->value = std::move(str);
    } else {
        qv->strValue->deref();
        qv->strValue = new QoreString(std::move(str));
    }
}

void ref(LValue qv);

__attribute__((visibility("default"))) void eval_assign(LValue l, Value r) noexcept {
    LOG("eval_assign l = " << l << ", r = " << r);

    deref(l);
    *l = r;
    ref(l);
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
    }
    assert(false);
}
