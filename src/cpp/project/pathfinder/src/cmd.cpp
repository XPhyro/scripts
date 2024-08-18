#include "cmd.hpp"

#include <filesystem>
#include <string>
#include <utility>

#include <cstdlib>

#include <pwd.h>
#include <unistd.h>

#include <hedley.h>
#include <lyra/lyra.hpp>

#include <xph/die.hpp>
#include <xph/exec_info.hpp>
#include <xph/string.hpp>

#include "db.hpp"

namespace fs = std::filesystem;

paf::alias::alias(lyra::cli& cli)
{
    auto command = lyra::command("alias", [this](const lyra::group& group) { execute(group); })
                       .help("Print aliases for POSIX-compatible shells.")
                       .add_argument(lyra::help(m_show_help))
                       .optional();
    cli.add_argument(std::move(command));
}

paf::mark::mark(lyra::cli& cli)
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

paf::unmark::unmark(lyra::cli& cli)
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

paf::jump::jump(lyra::cli& cli)
{
    const auto arg_keycode =
        lyra::arg(m_keycode, "keycode")("keycode to match the directory").cardinality(0, 1);

    auto command = lyra::command("jump", [this](const lyra::group& group) { execute(group); })
                       .help("Jump to the directory for the given keycode.")
                       .add_argument(lyra::help(m_show_help))
                       .add_argument(std::move(arg_keycode))
                       .optional();

    cli.add_argument(std::move(command));
}

paf::open::open(lyra::cli& cli)
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

paf::print::print(lyra::cli& cli)
{
    const auto opt_db_type =
        lyra::opt(m_db_type, "database")["-d"]["--database"]("database to search")
            .choices("both", "directory", "file");
    const auto arg_keycode =
        lyra::arg(m_keycode, "keycode")("keycode to match the file").cardinality(1, 1);
    const auto opt_nul = lyra::opt(m_nul)["-0"]["-z"]["--null"]("use NUL as output delimiter");

    auto command = lyra::command("print", [this](const lyra::group& group) { execute(group); })
                       .help("Print the value for the given keycode.")
                       .add_argument(lyra::help(m_show_help))
                       .add_argument(std::move(opt_db_type))
                       .add_argument(std::move(arg_keycode))
                       .add_argument(std::move(opt_nul))
                       .optional();

    cli.add_argument(std::move(command));
}

paf::list::list(lyra::cli& cli)
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

PAF_CMD_NORETURN void paf::alias::execute(const lyra::group& group)
{
    if (m_show_help) {
        std::cout << group << '\n';
        std::exit(EXIT_SUCCESS);
    }

    std::cout << "m() { " << xph::exec_path << " mark \"$@\"; }\n";
    std::cout << R"#(g() { cd "$()#" << xph::exec_path << ' '
              << R"#(jump "$@")"; })#"
                 "\n";
    std::cout << "e() { " << xph::exec_path << " open \"$@\"; }\n";

    PAF_CMD_EXIT();
}

PAF_CMD_NORETURN void paf::mark::execute(const lyra::group& group)
{
    if (m_show_help) {
        std::cout << group << '\n';
        std::exit(EXIT_SUCCESS);
    }

    xph::die_if(!m_keycode.size(), "<keycode> cannot be empty");
    xph::die_if(m_file && !m_file->size(), "<file> cannot be empty");

    if (!m_file) {
        auto cwd = getcwd(nullptr, 0);
        m_file = cwd;
        free(cwd);
    }

    auto db_type = fs::is_directory(*m_file) ? db_type::directory : db_type::file;
    auto db = db::get_db(db_type);

    if (auto kc_idx = db.index_of(m_keycode); kc_idx) {
        if (!m_yes) {
            std::cout << "Keycode [" << m_keycode << "] already exists as ["
                      << *db.try_get_mark_at(*kc_idx)
                      << "]. "
                         "Overwrite? [y/N] ";

            std::string answer;
            std::getline(std::cin, answer);

            if (answer.size() > 3)
                goto err;

            xph::str::makelower(answer);
            if (answer != "y" && answer != "yes")
                goto err;
        }

        db.try_remove_mark_at(*kc_idx);
    }

    db.add_mark(m_keycode, *m_file);

err:
    PAF_CMD_EXIT();
}

PAF_CMD_NORETURN void paf::unmark::execute(const lyra::group& group)
{
    if (m_show_help) {
        std::cout << group << '\n';
        std::exit(EXIT_SUCCESS);
    }

    xph::die_if(!m_identifier.size(), "<identifier> cannot be empty");

    if (fs::exists(m_identifier)) {
        auto db_type = fs::is_directory(m_identifier) ? db_type::directory : db_type::file;
        auto db = db::get_db(db_type);
        db.try_remove_mark(m_identifier);
    } else {
        auto dir_db = db::get_db(db_type::directory);
        auto dir_idx = dir_db.index_of(m_identifier);

        auto file_db = db::get_db(db_type::file);
        auto file_idx = file_db.index_of(m_identifier);

        bool remove_dir = true;
        bool remove_file = true;
        if (dir_idx && file_idx) {
            std::cout << "Keycode [" << m_identifier
                      << "] exists in both directory & file databases as ["
                      << *dir_db.try_get_mark_at(*dir_idx) << "] & ["
                      << *file_db.try_get_mark_at(*file_idx)
                      << "]. "
                         "Unmark which one? [b/d/f] ";

            std::string answer;
            std::getline(std::cin, answer);

            if (answer.size() > 1)
                goto err;

            xph::str::makelower(answer);

            if (answer == "b")
                ;
            else if (answer == "d")
                remove_file = false;
            else if (answer == "f")
                remove_dir = false;
            else
                goto err;
        }

        if (dir_idx && remove_dir) {
            dir_db.try_remove_mark_at(*dir_idx);
            file_db.cancel();
        }

        if (file_idx && remove_file) {
            file_db.try_remove_mark_at(*file_idx);
            dir_db.cancel();
        }
    }

err:
    PAF_CMD_EXIT();
}

PAF_CMD_NORETURN void paf::jump::execute(const lyra::group& group)
{
    if (m_show_help) {
        std::cout << group << '\n';
        std::exit(EXIT_SUCCESS);
    }

    if (!m_keycode) {
        auto pw = getpwuid(getuid());
        std::cout << pw->pw_dir << '\n';
        PAF_CMD_EXIT();
        return;
    } else {
        xph::die_if(!m_keycode->size(), "<keycode> cannot be empty");
    }

    auto db = db::get_db(db_type::directory);
    auto dir = db.try_get_mark(*m_keycode);

    xph::die_if(!dir, "keycode [", *m_keycode, "] not found in database");
    std::cout << *dir << '\n';

    PAF_CMD_EXIT();
}

PAF_CMD_NORETURN void paf::open::execute(const lyra::group& group)
{
    if (m_show_help) {
        std::cout << group << '\n';
        std::exit(EXIT_SUCCESS);
    }

    const char* editor = getenv("EDITOR");
    if (!editor)
        editor = getenv("VISUAL");

    xph::die_if(!editor, "cannot determine editor, set EDITOR or VISUAL");

    auto db = db::get_db(db_type::file);
    auto file = db.try_get_mark(m_keycode);
    db.cancel();

    xph::die_if(!file, "keycode [", m_keycode, "] not found in database");

    ::execlp(editor, editor, "--", file->c_str(), nullptr);

    PAF_CMD_EXIT();
}

PAF_CMD_NORETURN void paf::print::execute(const lyra::group& group)
{
    if (m_show_help) {
        std::cout << group << '\n';
        std::exit(EXIT_SUCCESS);
    }

    if (m_db_type == "both" || m_db_type == "directory") {
        auto db = db::get_db(db_type::directory);
        auto dir = db.try_get_mark(m_keycode);
        db.cancel();
        if (dir) {
            std::cout << *dir;
            if (!m_db_type.ends_with('/'))
                std::cout << '/';
            std::cout << (m_nul ? '\0' : '\n');
        }
    }

    if (m_db_type == "both" || m_db_type == "file") {
        auto db = db::get_db(db_type::file);
        auto file = db.try_get_mark(m_keycode);
        db.cancel();
        if (file)
            std::cout << *file << (m_nul ? '\0' : '\n');
    }

    PAF_CMD_EXIT();
}

PAF_CMD_NORETURN void paf::list::execute(const lyra::group& group)
{
    if (m_show_help) {
        std::cout << group << '\n';
        std::exit(EXIT_SUCCESS);
    }

    if (m_db_type == "both" || m_db_type == "directory") {
        auto db = db::get_db(db_type::directory);
        db.dump(m_nul ? "\0" : " : ", m_nul ? "\0" : "\n");
        db.cancel();
    }

    if (m_db_type == "both" || m_db_type == "file") {
        auto db = db::get_db(db_type::file);
        db.dump(m_nul ? "\0" : " : ", m_nul ? "\0" : "\n");
        db.cancel();
    }

    PAF_CMD_EXIT();
}
