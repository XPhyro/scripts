#ifndef HEADER_SCRIPTS_CXX_IO_
#define HEADER_SCRIPTS_CXX_IO_

#include <iomanip>
#include <iostream>
#include <ostream>
#include <ranges>
#include <span>
#include <string>
#include <vector>

#include <boost/range/combine.hpp>

namespace xph {
    namespace table {
        using cell_offset_t = std::size_t;
        using row_offsets_t = std::vector<std::size_t>;
        using table_offsets_t = std::vector<row_offsets_t>;

        enum alignment_t { left, center, right };

        template <typename Tcontainer>
        inline table_offsets_t calculate_offsets(const Tcontainer& table,
                                                 const std::span<alignment_t> alignments)
        {
            static_assert(
                std::is_same<typename Tcontainer::value_type::value_type, std::string>::value);

            table_offsets_t table_sizes(table.size());
            std::vector<std::size_t> max_column_sizes;

            for (const auto& row : table) {
                row_offsets_t row_offsets(row.size());
                max_column_sizes.reserve(row.size());

                for (auto&& i : std::views::iota(0uz, row.size())) {
                    const auto& cell = row[i];
                    row_offsets.push_back(cell.size());

                    if (i >= max_column_sizes.size()) {
                        max_column_sizes.push_back(cell.size());
                    } else if (cell.size() > max_column_sizes[i]) {
                        max_column_sizes[i] = cell.size();
                    }
                }

                table_sizes.push_back(row_offsets);
            }

            for (auto&& i : std::views::iota(1uz, max_column_sizes.size())) {
                const auto& prev_offset = max_column_sizes[i - 1];
                const auto& curr_offset = max_column_sizes[i];
                max_column_sizes[i] = prev_offset + curr_offset;
            }

            std::vector<std::size_t> column_sizes(max_column_sizes.size());
            for (auto& row_sizes : table_sizes) {
                column_sizes.clear();
                column_sizes.reserve(row_sizes.size());

                std::size_t carry_offset = 0;

                for (auto&& i : std::views::iota(1uz, row_sizes.size())) {
                    column_sizes.push_back(row_sizes[i] - row_sizes[i - 1]);

                    std::size_t offset = carry_offset;
                    switch (alignments[i - 1]) {
                        case alignment_t::left: {
                            offset += 0;
                            carry_offset = max_column_sizes[i - 1] - column_sizes[i - 1];
                        } break;
                        case alignment_t::center: {
                            auto offset_offset =
                                (max_column_sizes[i - 1] - column_sizes[i - 1]) / 2;
                            offset += offset_offset;
                            carry_offset = max_column_sizes[i - 1] - offset_offset;
                        } break;
                        case alignment_t::right: {
                            offset += max_column_sizes[i - 1] - column_sizes[i - 1];
                            carry_offset = 0;
                        } break;
                    }

                    row_sizes[i - 1] = offset;
                }

                std::size_t offset = carry_offset;
                switch (alignments.back()) {
                    case alignment_t::left: {
                        offset += 0;
                    } break;
                    case alignment_t::center: {
                        offset += (max_column_sizes.back() - column_sizes.back()) / 2;
                    } break;
                    case alignment_t::right: {
                        offset += max_column_sizes.back() - column_sizes.back();
                    } break;
                }

                row_sizes.back() = offset;
            }

            return table_sizes;
        }

        template table_offsets_t calculate_offsets<std::vector<std::vector<std::string>>>(
            const std::vector<std::vector<std::string>>& table,
            const std::span<alignment_t> alignments);
        template table_offsets_t calculate_offsets<std::span<std::span<std::string>>>(
            const std::span<std::span<std::string>>& table,
            const std::span<alignment_t> alignments);

        template <typename Tcontainer>
        inline void print_pretty(std::ostream& out,
                                 const Tcontainer& table,
                                 const std::span<alignment_t> alignments)
        {
            table_offsets_t offsets;

            if constexpr (std::is_same<typename Tcontainer::value_type::value_type,
                                       std::string>::value) {
                offsets = calculate_offsets(table, alignments);
            } else {
                std::vector<std::vector<std::string>> string_table(table.size());

                for (const auto& row : table) {
                    string_table.emplace_back(row.size());
                    auto& string_row = string_table.back();
                    std::transform(
                        row.begin(), row.end(), std::back_inserter(string_row), std::to_string);
                }

                offsets = calculate_offsets(string_table, alignments);
            }

            for (const auto& [row, row_offsets] : boost::combine(table, offsets)) {
                for (const auto& [cell, cell_offset] : boost::combine(row, row_offsets)) {
                    out << std::setfill(' ') << std::setw(cell_offset) << cell;
                }
                out << '\n';
            }
        }

        template <typename Tcontainer>
        inline void print_pretty(const Tcontainer& table, const std::span<alignment_t> alignments)
        {
            print_pretty(std::cout, table, alignments);
        }
    } // namespace table
} // namespace xph

#endif /* ifndef HEADER_SCRIPTS_CXX_IO_ */
