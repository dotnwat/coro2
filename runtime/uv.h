#include <expected>
#include <uv.h>

#include "backend.h"

class uv_backend : public backend {
public:
    uv_loop_t* loop;

    uv_backend() { loop = uv_default_loop(); }
    ~uv_backend() { uv_loop_close(loop); }

    void open(open_awaiter*) override;
    void read(read_awaiter*) override;
    void close(close_awaiter*) override;

    bool poll() override;
};
