The Big Idea
===============

Katydid is built on the Nymph analysis framework.  Since it doesn't have its own documentation yet, I'll include the general principles here.

The goals behind the design of Katydid/Nymph are:
- Users should be able to reconfigure and rerun their analysis without needing to recompile Katydid.
- Code for performing a particular task should not need to be rewritten.

All of the analysis tasks are performed by Processors.  They operate on passive, extensible data objects.  Most processors will generate new data in response to input data; the new data type then gets added to the extensible data object.  There are, of course, exceptions to these guidelines, but most processors behave in this way.

Processors are connected to one another using a signal/slot mechanism.  When a Processor has finished its job, assuming it's creating a new data type, it announces the new data by emitting a signal.  Other Processors may be listening for that signal with a slot.  That slot then calls a function in the listening Processor that acts on the new data.

