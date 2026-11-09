#ifndef TYPES_HPP
#define TYPES_HPP

#include <cstdint>

/* Status will be used in fn. return type */
enum class Status
{
    success,
    failure
};

enum class OperationType
{
    encode,
    decode,
    unsupported
};

#endif
