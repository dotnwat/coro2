#include <expected>
#include <liburing.h>
#include <optional>
#include <system_error>

#include "runtime/backend.h"

class io_uring_backend : public backend {
public:
    size_t uring_count{0};
    std::optional<struct io_uring> ring;

    io_uring_backend() {
        ring.emplace();
        int ret = io_uring_queue_init(64, &ring.value(), 0);
        if (ret < 0) {
            ring.reset();
            throw std::system_error(
              -ret, std::system_category(), "iouring init failed");
        }
    }

    ~io_uring_backend() {
        if (ring.has_value()) {
            io_uring_queue_exit(&ring.value());
        }
    }

    void open(open_awaiter*) override;
    void read(read_awaiter*) override;
    void close(close_awaiter*) override;

    bool poll() override;
};
