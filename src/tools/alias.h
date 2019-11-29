/*
 * Created by zx5 on 2019/11/26.
 * Copyright (c) zx5. All rights reserved.
 */

#ifndef C0_ALIAS_H
#define C0_ALIAS_H

#include <vector>
#include <memory>
#include <cstdint>
#include <utility>

namespace cc0 {
    using int32_t = std::int32_t;
    using uint32_t = std::uint32_t;
    using int64_t = std::int64_t;
    using uint64_t = std::uint64_t;

    using pos_t = std::pair<int64_t, int64_t>;
    using range_t = std::pair<pos_t, pos_t>;

    namespace ast {
        template <typename T> using _ptr = std::unique_ptr<T>;
        template <typename T> using _ptrs = std::vector<_ptr<T>>;
    }
}

#endif //C0_ALIAS_H
