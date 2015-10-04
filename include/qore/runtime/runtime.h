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

extern "C" QoreString *qrt_alloc_string(const char *value);
extern "C" void qrt_deref_string(QoreString *&str);
extern "C" const char *qrt_get_string(const QoreValue &v) noexcept;             //???

extern "C" void print_qv(QoreValue qv) noexcept;
extern "C" QoreValue make_str(const char *value) noexcept;
extern "C" QoreValue eval_add(QoreValue l, QoreValue r);
extern "C" void eval_trim(QoreValue qv);
extern "C" void strongDeref(QoreValue qv);
extern "C" void strongDerefNothrow(QoreValue qv) noexcept;
extern "C" void strongRef(QoreValue qv) noexcept;
extern "C" void unlock(QoreValue *qv) noexcept;
extern "C" QoreValue load_unique(const QoreValue *qv);
extern "C" bool eval_cond(QoreValue v) noexcept;

class QoreValueHolder {

public:
    QoreValueHolder() {
        qv.tag = Tag::Nothing;
    }

    QoreValueHolder(const std::string &str) {
        qv.tag = Tag::Str;
        qv.strValue = qrt_alloc_string(str.c_str());
    }

    QoreValueHolder(uint64_t v) {
        qv.tag = Tag::Int;
        qv.intValue = v;
    }

    QoreValueHolder(QoreValueHolder &&src) : qv(src.qv) {
        src.qv.tag = Tag::Nothing;
        src.qv.intValue = 0;
    }

    QoreValueHolder &operator=(QoreValueHolder &&src) {
        qv = src.qv;
        src.qv.tag = Tag::Nothing;
        src.qv.intValue = 0;
        return *this;
    }

    ~QoreValueHolder() {
        strongDeref(qv);
        qv.tag = Tag::Nothing;
    }

    const QoreValue &get() const {
        return qv;
    }

    void add(const QoreValueHolder &r) {
        qv = eval_add(qv, r.qv);
    }

private:
    QoreValueHolder(const QoreValueHolder &) = delete;
    QoreValueHolder &operator=(const QoreValueHolder &) = delete;

private:
    QoreValue qv;
};

std::ostream &operator<<(std::ostream &os, const QoreValue &qv);
std::ostream &operator<<(std::ostream &os, const QoreValue *qv);
std::ostream &operator<<(std::ostream &os, const QoreString &qs);
std::ostream &operator<<(std::ostream &os, const QoreString *qs);
std::ostream &operator<<(std::ostream &os, const QoreValueHolder &qv);

#endif /* INCLUDE_QORE_RUNTIME_RUNTIME_H_ */
