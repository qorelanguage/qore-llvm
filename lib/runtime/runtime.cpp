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
        LOG("Creating " << this);
    }

    QoreString(std::string str) : refCount(1), value(std::move(str)) {
        LOG("Creating " << this);
    }

    ~QoreString() {
        LOG("Destroying " << this);
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
        ++qv.strValue->refCount;
    }
}

__attribute__((visibility("default"))) void print_qv(QoreValue qv) noexcept {
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

__attribute__((visibility("default"))) QoreString *qrt_alloc_string(const char *value) {
    return new QoreString(value);
}

__attribute__((visibility("default"))) void qrt_deref_string(QoreString *&str) {
    if (!--str->refCount) {
        delete str;
        str = nullptr;
    }
}

__attribute__((visibility("default"))) const char *qrt_get_string(const QoreValue &v) noexcept {
    assert(v.tag == Tag::Str);
    return v.strValue->value.c_str();
}

__attribute__((visibility("default"))) QoreValue make_str(const char *value) noexcept {
    QoreValue qv;
    qv.tag = Tag::Str;
    qv.strValue = new QoreString(value);
    return qv;
}

static inline void append(std::string &dest, QoreValue v) {
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
    qv.strValue = new QoreString(std::move(str));
    return qv;
}

static inline std::string trim(const std::string &s) {
   auto wsfront = std::find_if_not(s.begin(), s.end(), [](int c){return std::isspace(c);});
   auto wsback = std::find_if_not(s.rbegin(), s.rend(), [](int c){return std::isspace(c);}).base();
   return (wsback <= wsfront ? std::string() : std::string(wsfront, wsback));
}

__attribute__((visibility("default"))) void eval_trim(QoreValue qv) {
    if (qv.tag == Tag::Str) {
        if (qv.strValue->value.find('a') != std::string::npos) {
            throw 123;
        }
        std::string str = trim(qv.strValue->value);
        qv.strValue->value = std::move(str);
    }
}

__attribute__((visibility("default"))) QoreValue load_unique(const QoreValue *qv) {
    if (qv->tag != Tag::Str) {
        return *qv;
    }
    if (qv->strValue->refCount == 1) {
        ++qv->strValue->refCount;
        return *qv;
    }
    return make_str(qv->strValue->value.c_str());
}

__attribute__((visibility("default"))) bool eval_cond(QoreValue v) noexcept {
    switch (v.tag) {
        case Tag::Nothing:
            return false;
        case Tag::Int:
            return v.intValue != 0;
        case Tag::Str:
            return v.strValue->value != "";
    }
    assert(false);
}

std::ostream &operator<<(std::ostream &os, const QoreString *qs) {
    return os << *qs;
}

std::ostream &operator<<(std::ostream &os, const QoreString &qs) {
    return os << "QoreString[" << qs.refCount << "]{" << static_cast<const void*>(&qs) << "}:\"" << qs.value << "\"";
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
