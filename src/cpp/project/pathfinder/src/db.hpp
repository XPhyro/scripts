#ifndef HEADER_SCRIPTS_CXX_PAF_DB_
#define HEADER_SCRIPTS_CXX_PAF_DB_

#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace paf {
    struct db_item {
    public:
        std::string keycode;
        std::string path;
    };

    enum class db_type {
        both,
        directory,
        file,
    };

    class db {
    public:
        const db_type type;

    private:
        std::vector<db_item>& m_db_vec;
        bool cancelled = false;

    private:
        db(void) = delete;
        db(const db&) = delete;
        db(db&&) = delete;
        db& operator=(const db&) = delete;
        db& operator=(db&&) = delete;

        db(std::vector<db_item>& db_vec, db_type type);

        void save(void);

    public:
        ~db();

        static db get_db(db_type type);

        void dump(std::string_view sep, std::string_view end);
        std::optional<std::string> try_get_mark(const std::string& keycode);
        void add_mark(const std::string& keycode, const std::string& path);
        bool try_remove_mark(const std::string& keycode);
        bool try_remove_mark_at(std::size_t index);
        std::optional<std::size_t> index_of(const std::string& keycode);
        void cancel(void);
    };
} // namespace paf

#endif /* ifndef HEADER_SCRIPTS_CXX_PAF_DB_ */