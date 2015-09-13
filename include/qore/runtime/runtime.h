#ifndef INCLUDE_QORE_RUNTIME_RUNTIME_H_
#define INCLUDE_QORE_RUNTIME_RUNTIME_H_

#include <cstdint>
#include <ostream>

enum class Tag : uint64_t {
    Nothing = 0,
    Int = 1,
    Str = 2,
};

class QoreString;

/**
 * \private
 */
struct QoreValue {
    Tag tag;
    union {
        int64_t intValue;
        QoreString *strValue;
    };

    QoreValue() noexcept : tag(Tag::Nothing), intValue(0) {
    }

    ~QoreValue() noexcept;
};

//TODO shouldn't QoreValue be opaque?
//TODO references are C++, keep API strictly C?
typedef QoreValue *LValue;
typedef const QoreValue &Value;

extern "C" void print_qv(Value qv) noexcept;
extern "C" void make_int(LValue qv, int64_t value) noexcept;
extern "C" void make_str(LValue qv, const char *value) noexcept;
extern "C" void eval_add(LValue qv, Value l, Value r) noexcept;
extern "C" void eval_trim(LValue qv) noexcept;
extern "C" void eval_assign(LValue l, Value r) noexcept;

std::ostream &operator<<(std::ostream &os, const QoreValue &qv);
std::ostream &operator<<(std::ostream &os, const QoreValue *qv);
std::ostream &operator<<(std::ostream &os, const QoreString &qs);
std::ostream &operator<<(std::ostream &os, const QoreString *qs);

#endif /* INCLUDE_QORE_RUNTIME_RUNTIME_H_ */
