Katydid Use and API Documentation
=================================


Katydid is a C++-based analysis toolkit designed primarily for the Project 8 experiment, but easily applicable to other CRES (and non-CRES) experiments. At is heart is a more general analysis framework called `Nymph <https://github.com/project8/nymph>`_ (which was also originally written for the Project 8 experiment).

The goals behind the design of Katydid/Nymph are:
- Users should be able to reconfigure and rerun their analysis without needing to recompile Katydid.
- Code for performing a particular task should not need to be rewritten.

All of the analysis tasks are performed by Processors.  They operate on passive, extensible data objects.  Most processors will generate new data in response to input data; the new data type then gets added to the extensible data object.  There are, of course, exceptions to these guidelines, but most processors behave in this way.

Processors are connected to one another using a signal/slot mechanism.  When a Processor has finished its job, assuming it's creating a new data type, it announces the new data by emitting a signal.  Other Processors may be listening for that signal with a slot.  That slot then calls a function in the listening Processor that acts on the new data.

Further details on how this all works can be found on the :ref:`basic-concepts` page.

Contents:

.. toctree::
   :maxdepth: 2

   Basic Concepts <BasicConcepts>
   Installation <Installation>
   Quick Start <QuickStart>
   Output Formats <OutputFormats>
   User Interface <UserInterface>
   Processors <Processors>
   Executables <Executables>
   Developer's Guide <DevGuide>
   Developer's Tutorial <DevTutorial>
   
.. end of toc
.. (you must not remove or modify the above comment line, it is required by the API Doc generation)
`Full Doxygen API Reference <_static/index.html>`_



Getting Help
-----------------------

* Send your question by email to Noah Oblath: nsoblath-at-mit.edu
* For installation problems: :ref:`katydid-installation`
* Found a bug or have a suggestion: `submit an issue <https://github.com/project8/katydid/issues>`_
* For ROOT problems: `ROOT Website <https://root.cern.ch>`_


Copyright and Licensing
-----------------------
* The copyright for all original code in Katydid belongs to the the authors of Katydid and their institutions.
* Katydid is an open-source distribution. See the LICENSE file in the distribution for the full license text.

