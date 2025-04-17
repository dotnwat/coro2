#include <cassert>
#include <expected>
#include <iostream>
#include <liburing.h>
#include <vector>

#include "runtime/io_uring.h"
#include "runtime/runtime.h"
#include "runtime/uv.h"

task<std::string> do_concat(std::vector<std::string> files) {
    std::string res;
    for (auto file : files) {
        auto fd = co_await runtime::open(file);
        auto data = co_await runtime::read(fd.value(), 0, 10);
        co_await runtime::close(fd.value());
        res = res + "[" + data.value() + "...]";
    }
    co_return res;
}

task<> concat(std::vector<std::string> files) {
    auto res = co_await do_concat(files);
    std::cout << "RESULT: " << res << "\n";
}

int main(int argc, char** argv) {
    std::vector<std::string> files;
    for (int i = 1; i < argc; ++i) {
        files.emplace_back(argv[i]);
    }
    std::cout << "concat " << files.size() << " files\n";

    // uv_backend backend;
    io_uring_backend backend;
    runtime sched(backend);

    task<void> t = concat(files);
    sched.start();

    return 0;
}
