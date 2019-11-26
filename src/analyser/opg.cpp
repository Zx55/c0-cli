/*
 * Created by zx5 on 2019/11/26.
 * Copyright (c) zx5. All rights reserved.
 */

#include "opg.h"

namespace cc0 {
    namespace opg {
        enum class Precedence: char {
            EQUAL,
            BELOW,
            GREAT,
            ERROR,
        };
    }
}