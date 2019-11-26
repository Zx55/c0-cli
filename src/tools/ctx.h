/*
 * Created by zx5 on 2019/11/26.
 * Copyright (c) zx5. All rights reserved.
 */

#ifndef C0_CTX_H
#define C0_CTX_H

#include "alias.h"

#include <fstream>
#include <vector>
#include <string>
#include <filesystem>

namespace cc0 {
    class Context final {
    private:
        static std::filesystem::path _filename;
        static std::vector<std::string> _source;

    public:
        Context() = delete;
        Context(const Context&) = delete;
        Context(Context&&) = delete;
        Context& operator=(const Context&) = delete;

        [[nodiscard]] static bool set_ctx(const std::string& filename);
        inline static void clear_ctx() { _source.clear(); }

        [[nodiscard]] inline static std::vector<std::string>& get_source() { return _source; }
        [[nodiscard]] inline static std::string& get_line(int64_t row) { return _source.at(row); }
        [[nodiscard]] inline static std::string get_absolute() {
            return std::filesystem::absolute(_filename).string();
        }
    };
}

#endif //C0_CTX_H
