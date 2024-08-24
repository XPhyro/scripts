#ifndef HEADER_SCRIPTS_CXX_PAF_CMD_
#define HEADER_SCRIPTS_CXX_PAF_CMD_

#include <lyra/lyra.hpp>

#define PAF_CMD_NORETURN
#define PAF_CMD_EXIT() \
    do {               \
    } while (false)

namespace paf {
    class alias {
    private:
        bool m_show_help = false;

    public:
        alias(void) = delete;
        alias(lyra::cli& cli);
        PAF_CMD_NORETURN void execute(const lyra::group& g);
    };

    class mark {
    private:
        bool m_show_help = false;
        bool m_relative = false;
        bool m_use_dir = false;
        bool m_use_file = false;
        bool m_wordexp = false;
        bool m_yes = false;
        std::string m_keycode = "";
        std::optional<std::string> m_file = {};

    public:
        mark(void) = delete;
        mark(lyra::cli& cli);
        PAF_CMD_NORETURN void execute(const lyra::group& g);
    };

    class unmark {
    private:
        bool m_show_help = false;
        std::string m_identifier = "";

    public:
        unmark(void) = delete;
        unmark(lyra::cli& cli);
        PAF_CMD_NORETURN void execute(const lyra::group& g);
    };

    class jump {
    private:
        bool m_show_help = false;
        std::optional<std::string> m_keycode = {};

    public:
        jump(void) = delete;
        jump(lyra::cli& cli);
        PAF_CMD_NORETURN void execute(const lyra::group& g);
    };

    class open {
    private:
        bool m_show_help = false;
        std::string m_keycode = "";

    public:
        open(void) = delete;
        open(lyra::cli& cli);
        PAF_CMD_NORETURN void execute(const lyra::group& g);
    };

    class print {
    private:
        bool m_show_help = false;
        bool m_use_dir = false;
        bool m_use_file = false;
        std::string m_keycode = "";
        bool m_nul = false;

    public:
        print(void) = delete;
        print(lyra::cli& cli);
        PAF_CMD_NORETURN void execute(const lyra::group& g);
    };

    class list {
    private:
        bool m_show_help = false;
        bool m_use_dir = false;
        bool m_use_file = false;
        bool m_nul = false;

    public:
        list(void) = delete;
        list(lyra::cli& cli);
        PAF_CMD_NORETURN void execute(const lyra::group& g);
    };
} // namespace paf

#endif /* ifndef HEADER_SCRIPTS_CXX_PAF_CMD_ */
