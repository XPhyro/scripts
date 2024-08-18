#include "db.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <optional>
#include <string_view>
#include <vector>

#include <cstdlib>

#include <wordexp.h>

#include <hedley.h>

#include <xph/db.h>
#include <xph/die.hpp>

namespace fs = std::filesystem;

using db_version_t = std::uint64_t;

namespace paf {
    static std::optional<std::vector<db_item>> db_dir_vec = std::nullopt,
                                               db_file_vec = std::nullopt;

    static std::string get_db_path(db_type type)
    {
        static const constexpr char* const config_prefix = "/scripts/pathfinder";
        static std::string prefix = ::confhome_s(config_prefix);

        switch (type) {
            case db_type::both:
                std::exit(EXIT_FAILURE);
            case db_type::directory:
                return prefix + "/dir_db.bin";
            case db_type::file:
                return prefix + "/file_db.bin";
            default:
                HEDLEY_UNREACHABLE();
        }
    }

    static void load_db_v1(std::vector<db_item>& db_vec, std::ifstream& ifs)
    {
        for (std::string k, v; std::getline(ifs, k, '\0') && std::getline(ifs, v, '\0');) {
            db_flags_t flags;
            ifs.read(reinterpret_cast<char*>(&flags), sizeof(flags));
            db_vec.emplace_back(k, v, flags);
        }
    }

    static void load_db_v0(std::vector<db_item>& db_vec, std::ifstream& ifs)
    {
        for (std::string k, v; std::getline(ifs, k, '\0') && std::getline(ifs, v, '\0');)
            db_vec.emplace_back(k, v);
    }

    static void load_db(std::vector<db_item>& db_vec, db_type type)
    {
        const auto path = get_db_path(type);
        std::ifstream ifs{ path };

        if (ifs.peek() == std::ifstream::traits_type::eof())
            return;

        db_version_t version;
        ifs.read(reinterpret_cast<char*>(&version), sizeof(version));

        if (version == 0)
            load_db_v0(db_vec, ifs);
        else if (version == 1)
            load_db_v1(db_vec, ifs);
        else
            xph::die("cannot load database with version [", version, "]");
    }

    static void save_db(const std::vector<db_item>& db_vec, db_type type)
    {
        static const constexpr db_version_t db_version = 1uz;

        const auto path = get_db_path(type);
        const auto tmp_path = path + ".tmp";

        {
            std::ofstream ofs{ tmp_path };

            ofs.write(reinterpret_cast<const char*>(&db_version), sizeof(db_version));

            for (const auto& item : db_vec) {
                ofs << item.keycode << '\0' << item.path << '\0';
                ofs.write(reinterpret_cast<const char*>(&item.flags), sizeof(item.flags));
            }
        }

        fs::rename(tmp_path, path);
    }

    db db::get_db(db_type type)
    {
        switch (type) {
            case db_type::both:
                std::exit(EXIT_FAILURE);
            case db_type::directory:
                if (!db_dir_vec) {
                    db_dir_vec = std::vector<db_item>{};
                    load_db(*db_dir_vec, type);
                }

                return db(*db_dir_vec, type);
            case db_type::file:
                if (!db_file_vec) {
                    db_file_vec = std::vector<db_item>{};
                    load_db(*db_file_vec, type);
                }

                return db(*db_file_vec, type);
            default:
                HEDLEY_UNREACHABLE();
        }
    }

    void db::reset_db(db_type type)
    {
        auto db_path = get_db_path(type);
        fs::remove(db_path);
    }

    db::db(std::vector<db_item>& db_vec, db_type type) : type(type), m_db_vec(db_vec) {}

    db::~db()
    {
        this->save();
    }

    void db::save(void)
    {
        if (!cancelled)
            save_db(m_db_vec, type);
    }

    void db::cancel(void)
    {
        cancelled = true;
    }

    void db::dump(std::string_view sep, std::string_view end)
    {
        for (const auto& item : m_db_vec)
            std::cout << item.keycode << sep << item.path << end;
    }

    std::optional<std::string> db::try_get_mark(const std::string& keycode)
    {
        for (auto item : m_db_vec) {
            if (item.keycode == keycode) {
                if (item.flags & static_cast<db_flags_t>(db_flag::wordexp)) {
                    wordexp_t result;
                    int r = ::wordexp(item.path.c_str(), &result, 0);
                    xph::die_if(r, "wordexp [", item.path, "] failed");
                    item.path = result.we_wordv[0];
                    ::wordfree(&result);
                }

                return item.path;
            }
        }

        return std::nullopt;
    }

    std::optional<db_item> db::try_get_mark_at(std::size_t index)
    {
        if (index >= m_db_vec.size())
            return std::nullopt;

        return m_db_vec[index];
    }

    void db::add_mark(const std::string& keycode, const std::string& path, db_flags_t flags)
    {
        m_db_vec.emplace_back(keycode, path, flags);
    }

    bool db::try_remove_mark(const std::string& keycode)
    {
        const auto pre_size = m_db_vec.size();
        std::erase_if(m_db_vec,
                      [&keycode](const db_item& item) { return item.keycode == keycode; });
        return m_db_vec.size() != pre_size;
    }

    bool db::try_remove_mark_at(std::size_t index)
    {
        if (index >= m_db_vec.size())
            return false;

        m_db_vec.erase(m_db_vec.begin() + index);
        return true;
    }

    std::optional<std::size_t> db::index_of(const std::string& keycode)
    {
        auto pos = std::find_if(m_db_vec.begin(), m_db_vec.end(), [&keycode](const db_item& item) {
            return item.keycode == keycode;
        });

        if (pos != m_db_vec.end())
            return std::distance(m_db_vec.begin(), pos);

        return std::nullopt;
    }
} // namespace paf
