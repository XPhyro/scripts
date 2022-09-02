#ifndef HEADER_SCRIPTS_CXX_MACROS
#define HEADER_SCRIPTS_CXX_MACROS

#define COMMA ,

#define DEFINE_DEFAULT_ENUM(NAME, VALS) \
    struct _##NAME##_definition {       \
        enum type { VALS };             \
    };                                  \
    typedef macros::safe_enum<_##NAME##_definition> NAME;

#define DEFINE_TYPED_ENUM(TYPE, NAME, VALS) \
    struct _##NAME##_definition {           \
        enum type { VALS };                 \
    };                                      \
    typedef macros::safe_enum<_##NAME##_definition, TYPE> NAME;

#define DEFINE_ENUM(NAME, VALS)   \
    struct _##NAME##_definition { \
        enum type { VALS };       \
    };                            \
    typedef macros::safe_enum<_##NAME##_definition, int> NAME;

namespace macros
{
template <typename def, typename inner = typename def::type>
class safe_enum : public def {
    typedef inner type;
    inner val;

public:
    safe_enum() {}
    safe_enum(type v)
        : val(v)
    {
    }

    type value() const
    {
        return val;
    }

    friend bool operator==(const safe_enum& lhs, const safe_enum& rhs)
    {
        return lhs.val == rhs.val;
    }
    friend bool operator!=(const safe_enum& lhs, const safe_enum& rhs)
    {
        return lhs.val != rhs.val;
    }
    friend bool operator<(const safe_enum& lhs, const safe_enum& rhs)
    {
        return lhs.val < rhs.val;
    }
    friend bool operator<=(const safe_enum& lhs, const safe_enum& rhs)
    {
        return lhs.val <= rhs.val;
    }
    friend bool operator>(const safe_enum& lhs, const safe_enum& rhs)
    {
        return lhs.val > rhs.val;
    }
    friend bool operator>=(const safe_enum& lhs, const safe_enum& rhs)
    {
        return lhs.val >= rhs.val;
    }
};
} // namespace macros

#endif /* ifndef HEADER_SCRIPTS_CXX_MACROS */
