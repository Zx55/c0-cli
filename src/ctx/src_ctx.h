/*
 * Created by zx5 on 2019/11/27.
 * Copyright (c) zx5. All rights reserved.
 */

/* Warning: Don't run this code with Mingw64-GCC-8.1.0 on Windows. */

#ifndef C0_SRC_CTX_H
#define C0_SRC_CTX_H

#include <fstream>
#include <filesystem>

namespace cc0 {
    class SourceContext final {
    private:
        static std::filesystem::path _filename;
        static std::vector<std::string> _source;

    public:
        SourceContext() = delete;
        SourceContext(const SourceContext&) = delete;
        SourceContext(SourceContext&&) = delete;
        SourceContext& operator=(const SourceContext&) = delete;

        [[nodiscard]] static bool set_ctx(const std::string& filename);
        static void clear_ctx() {
            _filename.clear();
            _source.clear();
        }

        [[nodiscard]] inline static std::string get_absolute() {
            return std::filesystem::absolute(_filename).string();
        }
        [[nodiscard]] inline static auto& get_source() { return _source; }
        [[nodiscard]] inline static auto& get_line(int64_t row) { return _source.at(row); }
    };
}

#endif //C0_SRC_CTX_H
