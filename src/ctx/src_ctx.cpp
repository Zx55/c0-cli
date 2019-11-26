/*
 * Created by zx5 on 2019/11/27.
 * Copyright (c) zx5. All rights reserved.
 */

#include "src_ctx.h"

namespace cc0 {
    std::filesystem::path SourceContext::_filename;
    std::vector<std::string> SourceContext::_source;

    [[nodiscard]] bool SourceContext::set_ctx(const std::string& filename) {
        _source.clear();

        auto path = std::filesystem::path(filename);
        if (std::filesystem::exists(path)) {
            _filename = std::move(path);
            auto in_file = std::ifstream(_filename);

            std::string line;
            while (std::getline(in_file, line))
                _source.push_back(line + '\n');
            in_file.close();

            return true;
        }

        return false;
    }
}