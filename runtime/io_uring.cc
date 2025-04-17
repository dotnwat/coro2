#include "io_uring.h"

#include <cassert>

void io_uring_backend::open(open_awaiter* awaiter) {
    auto* sqe = io_uring_get_sqe(&ring.value());
    io_uring_prep_openat(sqe, AT_FDCWD, awaiter->path.data(), O_RDONLY, 0);
    io_uring_sqe_set_data(sqe, awaiter);
    io_uring_submit(&ring.value());
    uring_count++;
}

void io_uring_backend::read(read_awaiter* awaiter) {
    awaiter->data.resize(awaiter->size);
    auto* sqe = io_uring_get_sqe(&ring.value());
    io_uring_prep_read(
      sqe,
      awaiter->fd,
      awaiter->data.data(),
      awaiter->data.size(),
      awaiter->offset);
    io_uring_sqe_set_data(sqe, awaiter);
    io_uring_submit(&ring.value());
    uring_count++;
}

void io_uring_backend::close(close_awaiter* awaiter) {
    auto* sqe = io_uring_get_sqe(&ring.value());
    io_uring_prep_close(sqe, awaiter->fd);
    io_uring_sqe_set_data(sqe, awaiter);
    io_uring_submit(&ring.value());
    uring_count++;
}

bool io_uring_backend::poll() {
    unsigned head;
    unsigned count{0};
    struct io_uring_cqe* cqe;
    io_uring_for_each_cqe(&ring.value(), head, cqe) {
        ++count;
        auto* awaiter = reinterpret_cast<class awaiter*>(
          io_uring_cqe_get_data(cqe));
        assert(awaiter);
        awaiter->complete(cqe->res);
    }
    io_uring_cq_advance(&ring.value(), count);
    uring_count -= count;
    return count != 0;
}
