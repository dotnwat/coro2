```bash
$ bazel build //:concat

$ echo -n aaaaa > f0
$ echo -n bbbbb > f1
$ echo -n ccccc > f2

$ bazel-bin/concat f0 f1 f2
concat 3 files
RESULT: [aaaaa...][bbbbb...][ccccc...]
```

### Notes

When a coroutine completes and delivers the result to its promise it is common
for the promise to react by scheduling a continutation that depends on this
result. To do this it needs to be able to access the runtime. Since we can't
directly create this promise, we need to rely on some form of dependency
injection.

In this code base we use a thread local variable which is very convenient, but
rather unsatisfying. Overall though, it results in code that is fairly clean
and easy to grok, compared to the alternatives.

I was able to eliminate the thread local variable entirely. Generally at the
point that I/O is going to occur we create an awaiter object which we can store
a reference to the runtime inside because the application creates this awaiter
directly. From there, with various contortions one can propogate back a pointer to the
runtime, but it feels awkward.

It seems one can also use await_transform operator which allows a coroutine to
communicate with its promise, and use that do something like `co_await
inject_runtime(runtime)` at the beginning of a coroutine (a noop that just sets
a variable in the promise). But apparently this technique doesn't work for
generators.

Maybe there are other options...
