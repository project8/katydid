Modules
==============

Katydid is divided into several modules, each of which builds a shared-object library.  Each module has a general purpose, and the classes that go into each module pertain primarily to that purpose.

Dependencies between the modules are certainly allowed, though care should be taken to ensure that those dependencies go only in one direction (i.e. be careful to avoid circular dependencies!).

The library filenames are libKatydid[ModuleName].so (or .dylib).

Modules in Katydid each have source files, and you can find conventions of different types of modules in the README of the "Source" directory. 