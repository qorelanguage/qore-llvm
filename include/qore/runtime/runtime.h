#ifndef INCLUDE_QORE_RUNTIME_RUNTIME_H_
#define INCLUDE_QORE_RUNTIME_RUNTIME_H_

#include <cstdint>
#include <ostream>

enum class Tag : uint64_t {
    Nothing = 0,
    Int = 1,
    Str = 2,
    LValue = 3,
};

class QoreString;

struct QoreValue {
    Tag tag;
    union {
        int64_t intValue;
        QoreString *strValue;
        QoreValue *qv;
    };
};

extern "C" void print_qv(QoreValue *qv);
extern "C" void make_nothing(QoreValue *qv);
extern "C" void make_int(QoreValue *qv, int64_t value);
extern "C" void make_str(QoreValue *qv, const char *value);
extern "C" void make_lvalue(QoreValue *qv, QoreValue *value);
extern "C" void eval_add(QoreValue *qv, QoreValue *l, QoreValue *r);
extern "C" void eval_trim(QoreValue *qv);
extern "C" void eval_assign(QoreValue *l, QoreValue *r);
extern "C" void deref(QoreValue *qv);
extern "C" void ref(QoreValue *qv);

std::ostream &operator<<(std::ostream &os, const QoreValue &qv);
std::ostream &operator<<(std::ostream &os, const QoreValue *qv);
std::ostream &operator<<(std::ostream &os, const QoreString &qs);
std::ostream &operator<<(std::ostream &os, const QoreString *qs);

#endif /* INCLUDE_QORE_RUNTIME_RUNTIME_H_ */
