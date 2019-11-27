/*
 * Created by zx5 on 2019/11/27.
 * Copyright (c) zx5. All rights reserved.
 */

#include "src_ctx.h"

namespace cc0 {
    std::filesystem::path SourceContext::_in;
    std::filesystem::path SourceContext::_out;
    std::vector<std::string> SourceContext::_source;
    bool SourceContext::_f_wall = true;

    [[nodiscard]] bool SourceContext::set_ctx(const std::string& in_fn, const std::string& out_fn,
            bool f_wall) {
        _source.clear();

        auto path = std::filesystem::path(in_fn);
        if (std::filesystem::exists(path)) {
            // handle input
            _in = std::move(path);
            auto in_file = std::ifstream(_in);

            std::string line;
            while (std::getline(in_file, line))
                _source.push_back(line + '\n');
            in_file.close();

            //handle output
            _out = std::filesystem::path(out_fn);

            // set flags
            _f_wall = f_wall;

            return true;
        }

        return false;
    }

    void SourceContext::clear_ctx() {
        _in.clear();
        _out.clear();
        _source.clear();

        _f_wall = true;
    }
}