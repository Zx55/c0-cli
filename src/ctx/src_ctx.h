/*
 * Created by zx5 on 2019/11/27.
 * Copyright (c) zx5. All rights reserved.
 */

/* Warning: Don't run this code with Mingw64-GCC-8.1.0 on Windows. */

#ifndef C0_SRC_CTX_H
#define C0_SRC_CTX_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>

namespace cc0 {
    class SourceContext final {
        friend class RuntimeContext;

    private:
        static std::filesystem::path _in;
        static std::filesystem::path _out;
        static std::vector<std::string> _source;

        // enable all warning messages
        static bool _f_wall;
        // make warning into fatal error
        static bool _f_werror;
        // enable sstream as input, usually for test
        static bool _f_ss;

    public:
        SourceContext() = delete;
        SourceContext(const SourceContext&) = delete;
        SourceContext(SourceContext&&) = delete;
        SourceContext& operator=(const SourceContext&) = delete;

        [[nodiscard]] static bool set_ctx(const std::string& in_fn, const std::string& out_fn,
                bool f_wall = true, bool f_werror = false);
        /* api for sstream */
        static void set_ctx(const std::istringstream&);
        static void clear_ctx();

        [[nodiscard]] inline static std::string get_in_absolute() {
            return _f_ss ? "string stream" : std::filesystem::absolute(_in).string();
        }
        [[nodiscard]] inline static std::string get_out() {
            return std::filesystem::absolute(_out).string();
        }
        [[nodiscard]] inline static auto& get_source() { return _source; }
        [[nodiscard]] inline static auto& get_line(int64_t row) {
            static std::string empty_line = "        ";
            return _source.empty() ? empty_line : _source.at(row);
        }
    };
}

#endif //C0_SRC_CTX_H
