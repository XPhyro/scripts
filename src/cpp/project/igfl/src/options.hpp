#ifndef HEADER_SCRIPTS_CXX_IGFL_OPTIONS_
#define HEADER_SCRIPTS_CXX_IGFL_OPTIONS_

#include <optional>
#include <string>

namespace igfl {
    class Options {
#define DECLARE_FIELD(TYPE, NAME, DEFAULT)                \
public:                                                   \
    static const constexpr TYPE default_##NAME = DEFAULT; \
                                                          \
private:                                                  \
    TYPE NAME = default_##NAME;                           \
                                                          \
public:                                                   \
    [[nodiscard]] TYPE get_##NAME() const noexcept

        DECLARE_FIELD(std::optional<std::string>, init_dir, {});
        DECLARE_FIELD(float, idle_fps, 24.0f);
        DECLARE_FIELD(float, active_fps, 144.0f);
        DECLARE_FIELD(std::optional<std::string>, ini_file, {});
        DECLARE_FIELD(std::optional<std::string>, log_file, {});
        DECLARE_FIELD(bool, disable_mouse_support, false);

    public:
        Options() = delete;
        Options(int argc, char** argv);
    };

} // namespace igfl

#endif /* ifndef HEADER_SCRIPTS_CXX_IGFL_OPTIONS_ */
