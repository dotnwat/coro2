#include "runtime.h"

static thread_local runtime* runtime_;
static thread_local backend* backend_;

void runtime::wire_up_support(backend* backend) {
    runtime_ = this;
    backend_ = backend;
}

runtime& runtime::get_runtime() { return *runtime_; }

backend& runtime::get_backend() { return *backend_; }
