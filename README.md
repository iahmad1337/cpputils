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
- micro test framework (I already experimented on this in proof converter;
also I had an idea on creating a proxy-class for comparisons and other stuff
like that, that automatically implements all needed operations)
- IO utils (this may need a separate header as it imports heavy iostream)
- logging (to file, to stream; most likely a global context that gets set up on
  start of the program; maybe I should also enable/disable functionality based
  on preprocessor definition)
- more algorithms + adapters
- more string utils (join, split, strip, trim)
- formatters for pretty text (e.g. aligned columns and layout engine (nested
  structures like 1d/2d trees))
- ligthweight jinja-like templates?
- tabular data utils?
- fuzzing and property-based testing?
- argparse?
- meta utils (lambda overloading util for std::variant::visit,
AllSame<T...>, Distinct<T...>, Ith<N, T...>)
- add a .clang_format and .clang_tidy with a makefile that formats and checks
the code here
- pick only appropriate headers to lower compilation time
- stb-like util that replaces all occurances of stb header with it's contents
- experiments
  - popular codecs
  - stacktrace with pretty format: https://habr.com/ru/post/302170/
    The stacktrace in the article is a bit intrusive, but if we move the macro
    from declaration to the first line of defintion and enable it on some
    preprocessor definition - It might even work!

