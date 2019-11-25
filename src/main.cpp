/*
 * Created by zx5 on 2019/11/22.
 * Copyright (c) zx5. All rights reserved.
 */

#include "argparse.hpp"

#include <iostream>
#include <fstream>

int main(int argc, char* argv[]) {
    argparse::ArgumentParser program("cc0");
    program.add_argument("input")
        .help("Specify the file to be compiled.");
    program.add_argument("-s")
        .default_value(false)
        .implicit_value(true)
        .help("Translate the input c0 source code to assembly text.");
    program.add_argument("-c")
        .default_value(false)
        .implicit_value(true)
        .help("Transform the input c0 source code to binary object.");
    program.add_argument("-o")
        .nargs(1)
        .default_value(std::string("out"))
        .help("Specify the output file");

    try {
        program.parse_args(argc, argv);
    } catch (const std::runtime_error &e) {
        std::cout << program;
        return 2;
    }

    auto in_fn = program.get<std::string>("input");
    auto in_file = std::ifstream(in_fn, std::ios::in);
    if (!in_file) {
        std::cerr << "File error: file not exists" << std::endl;
        return 1;
    }
    std::cin.rdbuf(in_file.rdbuf());

    auto out_fn = program.get<std::string>("-o");
    auto out_file = std::ofstream(out_fn);

    if (program["-s"] == true && program["-c"] == true) {
        std::cerr << "Argument error: bad argument" << std::endl;
        return 1;
    }

    if (program["-s"] == true) {
        std::cout << "ssss" << std::endl;
    } else if (program["-c"] == true) {
        std::cout << "cccc" << std::endl;
    } else {
        std::cerr << "Argument error: bad argument" << std::endl;
        return 1;
    }

    return 0;
}