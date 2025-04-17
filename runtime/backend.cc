#include "backend.h"

#include "runtime.h"

backend::open_awaiter::open_awaiter(backend* b, std::string path)
  : b(b)
  , path(path) {}

bool backend::open_awaiter::await_ready() const { return false; }

bool backend::open_awaiter::await_suspend(std::coroutine_handle<> h) {
    handle = h;
    b->open(this);
    return true;
}

void backend::open_awaiter::complete(int result) {
    res = result;
    runtime::get_runtime().schedule(handle);
}

std::expected<int, int> backend::open_awaiter::await_resume() {
    if (res < 0) {
        return std::unexpected(res);
    }
    return res;
}

backend::read_awaiter::read_awaiter(backend* b, int fd, int offset, int length)
  : b(b)
  , fd(fd)
  , offset(offset)
  , size(length) {}

bool backend::read_awaiter::await_ready() const { return false; }

bool backend::read_awaiter::await_suspend(std::coroutine_handle<> h) {
    handle = h;
    b->read(this);
    return true;
}

void backend::read_awaiter::complete(int result) {
    res = result;
    runtime::get_runtime().schedule(handle);
}

std::expected<std::string, int> backend::read_awaiter::await_resume() {
    if (res < 0) {
        return std::unexpected(res);
    }
    data.resize(res);
    return data;
}

backend::close_awaiter::close_awaiter(backend* b, int fd)
  : b(b)
  , fd(fd) {}

bool backend::close_awaiter::await_ready() const { return false; }

bool backend::close_awaiter::await_suspend(std::coroutine_handle<> h) {
    handle = h;
    b->close(this);
    return true;
}

void backend::close_awaiter::complete(int result) {
    res = result;
    runtime::get_runtime().schedule(handle);
}

std::expected<void, int> backend::close_awaiter::await_resume() {
    if (res < 0) {
        return std::unexpected(res);
    }
    return {};
}
