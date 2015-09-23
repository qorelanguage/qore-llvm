#ifndef INCLUDE_QORE_RUNTIME_RUNTIME_H_
#define INCLUDE_QORE_RUNTIME_RUNTIME_H_

#include <cstdint>
#include <ostream>

enum class Tag : uint8_t {
    Nothing = 0,
    Int = 1,
    Str = 2,
};

class QoreString;

/**
 * \private
 */
struct QoreValue {
    union {
        int64_t intValue;
        QoreString *strValue;
    };
    Tag tag;
};

extern "C" void print_qv(QoreValue qv) noexcept;
extern "C" QoreValue make_str(const char *value) noexcept;
extern "C" QoreValue eval_add(QoreValue l, QoreValue r) noexcept;
extern "C" void eval_trim(QoreValue qv) noexcept;
extern "C" void strongDeref(QoreValue qv) noexcept;
extern "C" void strongRef(QoreValue qv) noexcept;
extern "C" QoreValue load_unique(const QoreValue *qv) noexcept;
extern "C" bool eval_cond(QoreValue v) noexcept;

std::ostream &operator<<(std::ostream &os, const QoreValue &qv);
std::ostream &operator<<(std::ostream &os, const QoreValue *qv);
std::ostream &operator<<(std::ostream &os, const QoreString &qs);
std::ostream &operator<<(std::ostream &os, const QoreString *qs);

#endif /* INCLUDE_QORE_RUNTIME_RUNTIME_H_ */
