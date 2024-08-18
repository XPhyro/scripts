#include "command.hpp"

#include <string>
#include <utility>

#include <cstdlib>

#include <lyra/lyra.hpp>

paf::Alias::Alias(lyra::cli& cli)
{
    auto command = lyra::command("alias", [this](const lyra::group& group) { execute(group); })
                       .help("Print aliases `eval`able by a argIX-compatible shell.")
                       .add_argument(lyra::help(m_show_help))
                       .optional();
    cli.add_argument(std::move(command));
}

PAF_COMMAND_NORETURN void paf::Alias::execute(const lyra::group& group)
{
    if (m_show_help) {
        std::cout << group << '\n';
        std::exit(EXIT_SUCCESS);
    }

    std::cout << R"#(m() { eval "$(pathfinder toggle-mark "$@")"; })#"
                 "\n"
                 R"#(g() { eval "$(pathfinder jump "$@")"; })#"
                 "\n"
                 R"#(e() { eval "$(pathfinder open "$@")"; })#"
              << std::endl;

    PAF_COMMAND_EXIT();
}

paf::Mark::Mark(lyra::cli& cli)
{
    const auto opt_yes =
        lyra::opt(m_yes)["-y"]["--yes"]("don't require manual confirmation for overwriting marks");
    const auto arg_keycode =
        lyra::arg(m_keycode, "keycode")("keycode to assign the file to").cardinality(1, 1);
    const auto arg_file = lyra::arg(m_file, "file")("file to mark").cardinality(0, 1);

    auto command = lyra::command("mark", [this](const lyra::group& group) { execute(group); })
                       .help("Mark the given directory or file.")
                       .add_argument(lyra::help(m_show_help))
                       .add_argument(std::move(opt_yes))
                       .add_argument(std::move(arg_keycode))
                       .add_argument(std::move(arg_file))
                       .optional();

    cli.add_argument(std::move(command));
}

PAF_COMMAND_NORETURN void paf::Mark::execute(const lyra::group& group)
{
    if (m_show_help) {
        std::cout << group << '\n';
        std::exit(EXIT_SUCCESS);
    }

    PAF_COMMAND_EXIT();
}

paf::Unmark::Unmark(lyra::cli& cli)
{
    const auto arg_identifier =
        lyra::arg(m_identifier, "identifier")("keycode, directory or file to unmark")
            .cardinality(1, 1);

    auto command = lyra::command("unmark", [this](const lyra::group& group) { execute(group); })
                       .help("Unmark the given keycode, directory or file.")
                       .add_argument(lyra::help(m_show_help))
                       .add_argument(std::move(arg_identifier))
                       .optional();

    cli.add_argument(std::move(command));
}

PAF_COMMAND_NORETURN void paf::Unmark::execute(const lyra::group& group)
{
    if (m_show_help) {
        std::cout << group << '\n';
        std::exit(EXIT_SUCCESS);
    }

    PAF_COMMAND_EXIT();
}

paf::ToggleMark::ToggleMark(lyra::cli& cli)
{
    const auto opt_yes =
        lyra::opt(m_yes)["-y"]["--yes"]("don't require manual confirmation for overwriting marks");
    const auto arg_identifier =
        lyra::arg(m_identifier, "identifier")("keycode, directory or file to mark or unmark")
            .cardinality(1, 1);
    const auto arg_file =
        lyra::arg(m_file, "file")("directory or file to mark; if not given, $PWD is used")
            .cardinality(0, 1);

    auto command =
        lyra::command("toggle-mark", [this](const lyra::group& group) { execute(group); })
            .help("Mark or unmark the given keycode, directory or file.")
            .add_argument(lyra::help(m_show_help))
            .add_argument(std::move(opt_yes))
            .add_argument(std::move(arg_identifier))
            .add_argument(std::move(arg_file))
            .optional();

    cli.add_argument(std::move(command));
}

PAF_COMMAND_NORETURN void paf::ToggleMark::execute(const lyra::group& group)
{
    if (m_show_help) {
        std::cout << group << '\n';
        std::exit(EXIT_SUCCESS);
    }

    PAF_COMMAND_EXIT();
}

paf::Jump::Jump(lyra::cli& cli)
{
    const auto arg_keycode =
        lyra::arg(m_keycode, "keycode")("keycode to match the directory").cardinality(1, 1);

    auto command = lyra::command("jump", [this](const lyra::group& group) { execute(group); })
                       .help("Jump to the directory for the given keycode.")
                       .add_argument(lyra::help(m_show_help))
                       .add_argument(std::move(arg_keycode))
                       .optional();

    cli.add_argument(std::move(command));
}

PAF_COMMAND_NORETURN void paf::Jump::execute(const lyra::group& group)
{
    if (m_show_help) {
        std::cout << group << '\n';
        std::exit(EXIT_SUCCESS);
    }

    PAF_COMMAND_EXIT();
}

paf::Open::Open(lyra::cli& cli)
{
    const auto arg_keycode =
        lyra::arg(m_keycode, "keycode")("keycode to match the file").cardinality(1, 1);

    auto command = lyra::command("open", [this](const lyra::group& group) { execute(group); })
                       .help("Open the file for the given keycode.")
                       .add_argument(lyra::help(m_show_help))
                       .add_argument(std::move(arg_keycode))
                       .optional();

    cli.add_argument(std::move(command));
}

PAF_COMMAND_NORETURN void paf::Open::execute(const lyra::group& group)
{
    if (m_show_help) {
        std::cout << group << '\n';
        std::exit(EXIT_SUCCESS);
    }

    PAF_COMMAND_EXIT();
}

paf::Print::Print(lyra::cli& cli)
{
    const auto opt_db_type =
        lyra::opt(m_db_type, "database")["-d"]["--database"]("database to search")
            .choices("both", "directory", "file");
    const auto arg_keycode =
        lyra::arg(m_keycode, "keycode")("keycode to match the file").cardinality(1, 1);

    auto command = lyra::command("print", [this](const lyra::group& group) { execute(group); })
                       .help("Print the value for the given keycode.")
                       .add_argument(lyra::help(m_show_help))
                       .add_argument(std::move(opt_db_type))
                       .add_argument(std::move(arg_keycode))
                       .optional();

    cli.add_argument(std::move(command));
}

PAF_COMMAND_NORETURN void paf::Print::execute(const lyra::group& group)
{
    if (m_show_help) {
        std::cout << group << '\n';
        std::exit(EXIT_SUCCESS);
    }

    PAF_COMMAND_EXIT();
}

paf::List::List(lyra::cli& cli)
{
    const auto opt_db_type =
        lyra::opt(m_db_type, "database")["-d"]["--database"]("database to list")
            .choices("both", "directory", "file");
    const auto opt_nul = lyra::opt(m_nul)["-0"]["-z"]["--null"]("use NUL as output delimiter");

    auto command = lyra::command("list", [this](const lyra::group& group) { execute(group); })
                       .help("List all marks.")
                       .add_argument(lyra::help(m_show_help))
                       .add_argument(std::move(opt_db_type))
                       .add_argument(std::move(opt_nul))
                       .optional();

    cli.add_argument(std::move(command));
}

PAF_COMMAND_NORETURN void paf::List::execute(const lyra::group& group)
{
    if (m_show_help) {
        std::cout << group << '\n';
        std::exit(EXIT_SUCCESS);
    }

    PAF_COMMAND_EXIT();
}
