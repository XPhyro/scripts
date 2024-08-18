#ifndef HEADER_SCRIPTS_CXX_PAF_COMMAND_
#define HEADER_SCRIPTS_CXX_PAF_COMMAND_

#include <lyra/lyra.hpp>

#define PAF_COMMAND_NORETURN
#define PAF_COMMAND_EXIT() \
    do {                   \
    } while (false)

namespace paf {
    class Alias {
    private:
        bool m_show_help = false;

    public:
        Alias() = delete;
        Alias(lyra::cli& cli);
        PAF_COMMAND_NORETURN void execute(const lyra::group& g);
    };

    class Mark {
    private:
        bool m_show_help = false;
        bool m_yes = false;
        std::string m_keycode = "";
        std::optional<std::string> m_file = {};

    public:
        Mark() = delete;
        Mark(lyra::cli& cli);
        PAF_COMMAND_NORETURN void execute(const lyra::group& g);
    };

    class Unmark {
    private:
        bool m_show_help = false;
        std::string m_identifier = "";

    public:
        Unmark() = delete;
        Unmark(lyra::cli& cli);
        PAF_COMMAND_NORETURN void execute(const lyra::group& g);
    };

    class ToggleMark {
    private:
        bool m_show_help = false;
        bool m_yes = false;
        std::string m_identifier = "";
        std::optional<std::string> m_file = {};

    public:
        ToggleMark() = delete;
        ToggleMark(lyra::cli& cli);
        PAF_COMMAND_NORETURN void execute(const lyra::group& g);
    };

    class Jump {
    private:
        bool m_show_help = false;
        std::string m_keycode = "";

    public:
        Jump() = delete;
        Jump(lyra::cli& cli);
        PAF_COMMAND_NORETURN void execute(const lyra::group& g);
    };

    class Open {
    private:
        bool m_show_help = false;
        std::string m_keycode = "";

    public:
        Open() = delete;
        Open(lyra::cli& cli);
        PAF_COMMAND_NORETURN void execute(const lyra::group& g);
    };

    class Print {
    private:
        bool m_show_help = false;
        std::string m_db_type = "both";
        std::string m_keycode = "";

    public:
        Print() = delete;
        Print(lyra::cli& cli);
        PAF_COMMAND_NORETURN void execute(const lyra::group& g);
    };

    class List {
    private:
        bool m_show_help = false;
        std::string m_db_type = "both";
        bool m_nul = false;

    public:
        List() = delete;
        List(lyra::cli& cli);
        PAF_COMMAND_NORETURN void execute(const lyra::group& g);
    };

    class Edit {
    private:
        bool m_show_help = false;

    public:
        Edit() = delete;
        Edit(lyra::cli& cli);
        PAF_COMMAND_NORETURN void execute(const lyra::group& g);
    };
} // namespace paf

#endif /* ifndef HEADER_SCRIPTS_CXX_PAF_COMMAND_ */
