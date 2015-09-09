#ifndef INCLUDE_QORE_RUNTIME_RUNTIME_H_
#define INCLUDE_QORE_RUNTIME_RUNTIME_H_

#include <cstdint>

enum class Tag : uint64_t {
    Int = 1,
    Str = 2,
};

struct QoreValue {
    Tag tag;
    union {
        int64_t intValue;
        const char *strValue;
    };
};

extern "C" void print_qv(QoreValue *qv);
extern "C" void make_int(QoreValue *qv, int64_t value);
extern "C" void make_str(QoreValue *qv, const char *value);
extern "C" void eval_add(QoreValue *qv, QoreValue *l, QoreValue *r);
extern "C" void eval_trim(QoreValue *qv);

#endif /* INCLUDE_QORE_RUNTIME_RUNTIME_H_ */
