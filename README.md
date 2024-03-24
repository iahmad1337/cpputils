# What is this
This is a collection of utilities that I sometimes use in my
projects.

# Plans
- use less features of modern standards when they are not available (via
ifdefs)
- more tools that I personally use often but which are absent in stl
- add a .clang_format and .clang_tidy with a makefile that formats and checks
the code here
- pick only appropriate headers to lower compilation time
- experiments
  - std::pmr - make an allocator that is initialized at startup and replace with
    it the default allocator. It could be useful to limit the memory used by
    application and also for tracking the memory usage (allocations can be
    logged).

# TODO
- `ReadFile`, `WriteFile`
- Scoped file (or stderr/stdout) logger
- Indent/Dedent
- Universal hash for any ordered collection?
- Type info wrapper with `__PRETTY_FUNCTION__` hack
- Enum serialization??? Might be possible with macro definition and constexpr
  functions
- Intrusive splay tree implementation from my bimap??
- The actual bimap??
- Set of functors like square, cube, power, hash. Along with it - a bundle of
  utilities that help with functional programming:
    - `utils::functional::Compose(f1, f2, ...)`
    - `utils::functional::Bind(PH, 42, PH, "hello")`
    - `utils::functional::???`
- `utils::Pipe(auto)` + `utils::Pipe operator|(utils::Pipe, auto)`
- Sparse bitset implementation plus set operations (intersection, union,
  difference) (???)
- Home dir: https://github.com/ospray/rkcommon/blob/master/rkcommon/os/FileName.cpp#L40
- move definitions to .cc
- There are not enough `std::forward`s and sometimes I don't even know if I need
  to use it.
- `platform.hh` - platform-dependent functionality
- Python script + library for embedding of binary resources: https://stackoverflow.com/questions/11813271/embed-resources-eg-shader-code-images-into-executable-library-with-cmake

## utils::math
- Matrices that are stored in blocks
- Tricks from hacker's delight

## Short-term plan
- Reflection for up to 10 fields: ToString specialization, structured
  bindings, hash, order. And...
- ToString specialization for vectors, hashmaps and other stl containers.
  Probably make it more generic?
- Self-made range-adapters that don't mutate the stored range and can
  have chained method calls!
  Reference methods: iota (with custom step), sort, unique, join, sum, minmax,
  count, groupby, frequencies, are_distinct, nth, sliding_window,  and other
  popular algorithms.
  Take inspiration from Java Stream API, python itertools, toolz, topk, etc.
- Write down thiw tip in my cheatsheets: https://stackoverflow.com/a/599416/17549962
  (`:%norm A \` and `:%norm I- `)
- More reflection for aggregates?
