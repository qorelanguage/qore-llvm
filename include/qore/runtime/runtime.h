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

extern "C" void print_qv(struct QoreValue qv);
extern "C" struct QoreValue make_int(int64_t value);
extern "C" struct QoreValue make_str(const char *value);

#endif /* INCLUDE_QORE_RUNTIME_RUNTIME_H_ */
