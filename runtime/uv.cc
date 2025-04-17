#include "uv.h"

#include "backend.h"

struct read_context {
    backend::awaiter* awaiter;
    uv_buf_t iov;
    uv_fs_t req;
};

namespace {
void on_open(uv_fs_t* req) {
    auto* awaiter = reinterpret_cast<backend::awaiter*>(req->data);
    awaiter->complete(req->result);
    uv_fs_req_cleanup(req);
    delete req;
}

void on_read(uv_fs_t* req) {
    auto* ctx = reinterpret_cast<read_context*>(req->data);
    ctx->awaiter->complete(req->result);
    uv_fs_req_cleanup(req);
    delete ctx;
}

void on_close(uv_fs_t* req) {
    auto* awaiter = reinterpret_cast<backend::awaiter*>(req->data);
    awaiter->complete(req->result);
    uv_fs_req_cleanup(req);
    delete req;
}
} // namespace

void uv_backend::open(open_awaiter* awaiter) {
    auto* req = new uv_fs_t;
    req->data = awaiter;
    uv_fs_open(loop, req, awaiter->path.data(), O_RDONLY, 0, on_open);
}

void uv_backend::read(read_awaiter* awaiter) {
    awaiter->data.resize(awaiter->size);
    auto* ctx = new read_context;
    ctx->awaiter = awaiter;
    ctx->iov = uv_buf_init(awaiter->data.data(), awaiter->data.size());
    ctx->req.data = ctx;
    uv_fs_read(
      loop, &ctx->req, awaiter->fd, &ctx->iov, 1, awaiter->offset, on_read);
}

void uv_backend::close(close_awaiter* awaiter) {
    auto* req = new uv_fs_t;
    req->data = awaiter;
    uv_fs_close(loop, req, awaiter->fd, on_close);
}

bool uv_backend::poll() {
    if (!uv_loop_alive(loop)) {
        return false;
    }
    uv_run(loop, UV_RUN_ONCE);
    return true;
}
