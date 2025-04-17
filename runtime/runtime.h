#pragma once

#include <cassert>
#include <coroutine>
#include <expected>
#include <list>
#include <optional>

#include "backend.h"

class runtime {
private:
    std::list<std::coroutine_handle<>> ready;

public:
    runtime(backend& b) { wire_up_support(&b); }

    void start() {
        while (true) {
            while (!ready.empty()) {
                auto handle = ready.front();
                ready.pop_front();
                handle.resume();
            }
            if (!get_backend().poll() && ready.empty()) {
                break;
            }
        }
    }

    void schedule(std::coroutine_handle<> handle) {
        if (handle) {
            ready.push_back(handle);
        }
    }

    static backend::open_awaiter open(std::string path) {
        return backend::open_awaiter{&get_backend(), path};
    }

    static backend::read_awaiter read(int fd, int offset, int length) {
        return backend::read_awaiter(&get_backend(), fd, offset, length);
    }

    static backend::close_awaiter close(int fd) {
        return backend::close_awaiter(&get_backend(), fd);
    }

private:
    friend class backend;

    template<typename T>
    friend struct promise;

    static backend& get_backend();
    static runtime& get_runtime();

    void wire_up_support(backend*);
};

template<typename T>
struct task;

template<typename T>
struct promise;

template<typename T>
struct promise {
    std::coroutine_handle<> continuation;
    std::optional<std::expected<T, std::exception_ptr>>* result;

    task<T> get_return_object();
    std::suspend_never initial_suspend() const { return {}; }
    std::suspend_never final_suspend() noexcept { return {}; }
    void unhandled_exception() {
        *result = std::unexpected(std::current_exception());
        runtime::get_runtime().schedule(continuation);
    }
    void return_value(T&& value) {
        *result = std::move(value);
        runtime::get_runtime().schedule(continuation);
    }

    T get_result() {}
};

template<>
struct promise<void> {
    std::coroutine_handle<> continuation;
    std::optional<std::expected<void, std::exception_ptr>>* result;

    task<void> get_return_object();
    std::suspend_never initial_suspend() const { return {}; }
    std::suspend_never final_suspend() noexcept { return {}; }
    void unhandled_exception() {
        *result = std::unexpected(std::current_exception());
        runtime::get_runtime().schedule(continuation);
    }
    void return_void() { runtime::get_runtime().schedule(continuation); }
};

template<typename T = void>
struct [[nodiscard]] task {
    using promise_type = promise<T>;
    std::coroutine_handle<promise_type> handle;

    std::optional<std::expected<T, std::exception_ptr>> result;

    explicit task(std::coroutine_handle<promise_type> handle)
      : handle(handle) {
        handle.promise().result = &result;
    }

    task(task&&) noexcept = delete;
    task& operator=(task&&) noexcept = delete;

    bool await_ready() const noexcept { return handle.done(); }

    template<typename U>
    void await_suspend(std::coroutine_handle<U> c) noexcept {
        handle.promise().continuation = c;
    }

    T await_resume() {
        assert(result.has_value());
        if (result->has_value()) {
            return std::move(result->value());
        }
        std::rethrow_exception(result->error());
    }
};

template<typename T>
task<T> promise<T>::get_return_object() {
    return task<T>{std::coroutine_handle<promise<T>>::from_promise(*this)};
}

inline task<void> promise<void>::get_return_object() {
    return task<void>{
      std::coroutine_handle<promise<void>>::from_promise(*this)};
}
