#pragma once

#include <coroutine>
#include <expected>
#include <string>

class backend {
public:
    class awaiter {
    public:
        virtual void complete(int) {}
    };

    class open_awaiter : public awaiter {
    public:
        open_awaiter(backend*, std::string);
        bool await_ready() const;
        bool await_suspend(std::coroutine_handle<>);
        void complete(int) override;
        std::expected<int, int> await_resume();

        std::coroutine_handle<> handle;
        backend* b;
        std::string path;
        int res;
    };

    class read_awaiter : public awaiter {
    public:
        read_awaiter(backend*, int fd, int offset, int size);
        bool await_ready() const;
        bool await_suspend(std::coroutine_handle<>);
        void complete(int) override;
        std::expected<std::string, int> await_resume();

        std::coroutine_handle<> handle;
        backend* b;
        int fd;
        int offset;
        int size;
        std::string data;
        int res;
    };

    class close_awaiter : public awaiter {
    public:
        close_awaiter(backend*, int);
        bool await_ready() const;
        bool await_suspend(std::coroutine_handle<>);
        void complete(int) override;
        std::expected<void, int> await_resume();

        std::coroutine_handle<> handle;
        backend* b;
        int fd;
        int res;
    };

    virtual ~backend() = default;

    virtual void open(open_awaiter*) = 0;
    virtual void read(read_awaiter*) = 0;
    virtual void close(close_awaiter*) = 0;

    virtual bool poll() = 0;
};
