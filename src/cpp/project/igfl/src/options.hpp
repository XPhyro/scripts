#ifndef HEADER_SCRIPTS_CXX_IGFL_OPTIONS_
#define HEADER_SCRIPTS_CXX_IGFL_OPTIONS_

#include <optional>
#include <string_view>

namespace igfl {
    class Options {
    public:
        static const constexpr float default_idle_fps = 24.0f;
        static const constexpr float default_active_fps = 144.0f;
        static const constexpr bool default_mouse_support = true;

    private:
        std::optional<std::string_view> init_dir = {};
        float idle_fps = default_idle_fps;
        float active_fps = default_active_fps;
        bool mouse_support = default_mouse_support;

    public:
        Options() = delete;
        Options(int& argc, char**& argv);

#define DECLARE_GET(FIELD) [[nodiscard]] decltype(FIELD) get_##FIELD() const noexcept
        DECLARE_GET(init_dir);
        DECLARE_GET(idle_fps);
        DECLARE_GET(active_fps);
        DECLARE_GET(mouse_support);
#undef DECLARE_GET
    };

} // namespace igfl

#endif /* ifndef HEADER_SCRIPTS_CXX_IGFL_OPTIONS_ */
