/*
 * Created by zx5 on 2019/11/22.
 * Copyright (c) zx5. All rights reserved.
 */

#include "core.h"

#include "argparse.hpp"

#include <iostream>

int main(int argc, char* argv[]) {
    argparse::ArgumentParser program("cc0");
    program.add_argument("input")
        .help("Specify the input file.");
    program.add_argument("-l", "--lex")
        .default_value(false)
        .implicit_value(true)
        .help("Perform lexical analyse.");
    program.add_argument("-t", "--tree")
        .default_value(false)
        .implicit_value(true)
        .help("Draw AST.");
    program.add_argument("-s")
        .default_value(false)
        .implicit_value(true)
        .help("Compile only.");
    program.add_argument("-c")
        .default_value(false)
        .implicit_value(true)
        .help("Compile and assemble.");
    program.add_argument("-o")
        .nargs(1)
        .default_value(std::string("out"))
        .help("Specify the output file. (default: ./out)");
    program.add_argument("-Wdisable")
        .default_value(false)
        .implicit_value(true)
        .help("Disable warning messages.");
    program.add_argument("-Werror")
        .default_value(false)
        .implicit_value(true)
        .help("Make all warnings into errors.");

    try {
        program.parse_args(argc, argv);
    } catch (const std::runtime_error &e) {
        std::cout << program;
        return 2;
    }

    auto in_fn = program.get<std::string>("input");
    auto out_fn = program.get<std::string>("-o");

    if (!cc0::SourceContext::set_ctx(in_fn, out_fn, program["-Wdisable"] != true,
            program["-Werror"] == true)) {
        std::cerr << "File error: file not exist." << std::endl;
        return 1;
    }

    int cnt = 0;
    (program["-s"] == true) ? ++cnt : cnt;
    (program["-c"] == true) ? ++cnt : cnt;
    (program["-l"] == true) ? ++cnt : cnt;

    if (cnt > 1) {
        std::cerr << "Argument error: bad argument" << std::endl;
        return 1;
    }

    if (program["-s"] == true) {
        std::cout << "ssss" << std::endl;
    } else if (program["-c"] == true) {
        std::cout << "cccc" << std::endl;
    } else if (program["-l"] == true)
        cc0::tokenize();
    else if (program["-t"] == true)
        cc0::analyse();
    else {
        std::cerr << "Argument error: bad argument" << std::endl;
        return 1;
    }

    return 0;
}