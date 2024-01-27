#ifndef HEADER_SCRIPTS_CXX_IGFL_UI_
#define HEADER_SCRIPTS_CXX_IGFL_UI_

#include <cstddef>

#include "lib/imgui.h"

#include "types.hpp"

namespace igfl {
    class UI {
    public:
        application_target new_frame(ImGuiIO& io, float delta_time, std::size_t frame_count);
    };
}; // namespace igfl

#endif /* ifndef HEADER_SCRIPTS_CXX_IGFL_UI_ */
