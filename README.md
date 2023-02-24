# What is this
This is a collection of utilities that I oftenly use in my
projects. Often I just want to get the shit done and I don't care about the
performance, so the code here may not be optimal (but serves its purpose
well enough). I might consider speeding up some of the code in the future,
but for now this is sufficient.
# Plans
- use less features of modern standards when they are not available (via
ifdefs)
- more tools that I personally use often but which are absent in stl
- add a .clang_format and .clang_tidy with a makefile that formats and checks
the code here
- pick only appropriate headers to lower compilation time
- stb-like util that replaces all occurances of stb header with it's contents
- experiments
  - std::pmr - make an allocator that is initialized at startup and replace with
    it the default allocator. It could be useful to limit the memory used by
    application and also for tracking the memory usage (allocations can be
    logged).
  - binary serialization (a good cppcon talk on this topic:
    https://youtu.be/G7-GQhCw8eE)

# Good libraries that I definetely don't want to reimplement
- https://www.boost.org/doc/libs/master/doc/html/stacktrace.html
- https://github.com/p-ranav/pprint
- https://github.com/oneapi-src/oneTBB
- https://github.com/chriskohlhoff/asio
- https://github.com/nlohmann/json
- https://github.com/fmtlib/fmt
- https://github.com/ericniebler/range-v3
- https://github.com/gabime/spdlog
- https://github.com/p-ranav/argparse
- regex: https://t.me/experimentalchill/151
- re2
