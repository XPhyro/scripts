#ifndef HEADER_SCRIPTS_CXX_PAF_OPTIONS_
#define HEADER_SCRIPTS_CXX_PAF_OPTIONS_

#include <optional>
#include <string>

#define DECLARE_FIELD(TYPE, NAME, DEFAULT)                   \
public:                                                      \
    static const constexpr TYPE km_default_##NAME = DEFAULT; \
                                                             \
private:                                                     \
    TYPE m_##NAME = km_default_##NAME;

namespace paf {
    class Options {
        DECLARE_FIELD(std::optional<std::string>, init_dir, {});

    public:
        Options() = delete;
        Options(int argc, char** argv);
    };
} // namespace paf

#endif /* ifndef HEADER_SCRIPTS_CXX_PAF_OPTIONS_ */
