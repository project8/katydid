Known Issues 
=========


Note that issues should be reported through the issue tracker not a docs page. We might get rid of this soon? TODO_DOCS


Known bugs/features/things-to-watch-out-for should be listed here. Please try to keep the list up-to-date as new things are found and existing problems are solved.

# Things to watch out for
* JSON syntax errors in your configuration file will cause Katydid to exit with an error message.  It should specify where the problem is in the json file.
* Signal/slot connection errors will cause Katydid to exit with an error message.  There are three common variations (missing signal, missing slot, and mismatched signatures), and the error message should tell you how to diagnose what is actually wrong.
* If you are using a concurrent queue in multithreaded mode (including any KTDataQueueProcessorTemplate class such as KTMultiSliceClustering), if the queue times out, that processor will stop processing, and its thread will exit.  This is appropriate behavior if processing of an egg file is complete.  If this happens too early, while processing is still going on, it can cause an enormous memory leak, as slices aren't deleted properly and new slices continue to be created.  Try increasing the timeout time to solve this issue.

# Problems while running

This topic has been moved to the Katydid [Issues](https://github.com/project8/katydid/issues) page

# Missing features

This topic has been moved to the Katydid [Issues](https://github.com/project8/katydid/issues) page