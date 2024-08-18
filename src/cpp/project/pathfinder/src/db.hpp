#ifndef HEADER_SCRIPTS_CXX_PAF_DB_
#define HEADER_SCRIPTS_CXX_PAF_DB_

#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include <cstdint>

namespace paf {
    typedef std::uint16_t db_flags_t;
    enum class db_flag : db_flags_t {
        none = 0,
        wordexp = 1 << 0,
    };

    struct db_item {
    public:
        std::string keycode;
        std::string path;
        db_flags_t flags;
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
        static void reset_db(db_type type);

        void dump(std::string_view sep, std::string_view end);
        std::optional<std::string> try_get_mark(const std::string& keycode);
        std::optional<db_item> try_get_mark_at(std::size_t index);
        void add_mark(const std::string& keycode, const std::string& path, db_flags_t flags);
        bool try_remove_mark(const std::string& keycode);
        bool try_remove_mark_at(std::size_t index);
        std::optional<std::size_t> index_of(const std::string& keycode);
        void cancel(void);
    };
} // namespace paf

#endif /* ifndef HEADER_SCRIPTS_CXX_PAF_DB_ */
