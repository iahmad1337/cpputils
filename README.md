# What is this
This is a collection of utilities that I sometimes use in my
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

# TODO
- Script that dumps all of the library into a single file (codegen, see
  translation methods lab for example)
    - The order of files can be hardcoded
    - All of `#pragma once` should be removed and all include files should move
      to the beginning. Also, remove duplicates
    - The resulting generated file must be included in git
    - There should be a test, that tells whether all files from `include` and
      `src` were included. It shouldn't ever be manually modified
- `ReadFile`, `WriteFile`
- `EXPECT` macro: `#define EXPECT(cond, message) if (!(cond)) { throw std::runtime_error(utils::Format(__FILE__ ":% %", __LINE__, message)); }`
- Pretty print for nested containers (based on `std::cbegin` and the like)
  with/without indentation
- Scoped file (or stderr/stdout) logger
- Indent/Dedent
- Universal hash for any ordered collection?
- Type info wrapper with `__PRETTY_FUNCTION__` hack
- Enum serialization??? Might be possible with macro definition and constexpr
  functions
- `std::vector<bool>` done right (i.e. `boost::dynamic_bitset`)
- Self-made range-adapters that mutate or don't mutate the stored range and can
  have chained method calls! This way we won't have to worry about any
  lifetimes.
  Reference methods: sort, unique, join, sum, minmax, count, groupby,
  frequencies, are_distinct, nth, sliding_window,  and other popular algorithms.
  Take inspiration from Java Stream API, python itertools, toolz, topk, etc.
- Intrusive splay tree implementation from my bimap??
- The actual bimap??
- Set of functors like square, cube, power, hash. Along with it - a bundle of
  utilities that help with functional programming:
    - `utils::functional::Compose(f1, f2, ...)`
    - `utils::functional::Bind(PH, 42, PH, "hello")`
    - `utils::functional::???`
- `utils::Pipe(auto)` + `utils::Pipe operator|(utils::Pipe, auto)`
- Determine, whether it's possible to make a macro like `NAMED_TUPLE(f1, f2, ...)`.
- Sparse bitset implementation plus set operations (intersection, union,
  difference)
