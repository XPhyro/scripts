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
        std::string m_keycode = "";

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
        std::string m_db_type = "both";
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
        std::string m_db_type = "both";
        bool m_nul = false;

    public:
        list(void) = delete;
        list(lyra::cli& cli);
        PAF_CMD_NORETURN void execute(const lyra::group& g);
    };

    class Edit {
    private:
        bool m_show_help = false;

    public:
        Edit(void) = delete;
        Edit(lyra::cli& cli);
        PAF_CMD_NORETURN void execute(const lyra::group& g);
    };
} // namespace paf

#endif /* ifndef HEADER_SCRIPTS_CXX_PAF_CMD_ */
