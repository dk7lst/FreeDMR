# FreeDMR
Tools for an open Digital Mobile Radio (DMR) repeater network.

# Current State
Current Project State: Under construction, not yet functional.

You'll need some basic understanding on how software development works in order to use this software as it is in a very early state.

# Dependencies
I try to avoid too fancy dependencies in order to keep the software simple to build, use and maintain. But you'll need at least:
- C++ compiler (g++)
- pthread library
- make
- subversion or git make updating a lot easier

# Building the software
Go to "src"-directory and type "make".

Makefiles are very rudimentary at the moment and may fail for incremental building. Consider a "make clean" if in doubt before making.

There are no branches with stable versions at the moment. Everything is under heavy development and may change quickly. If compiling fails try an older version.

# Running the software
When compiling succeeded, go to "src/hardware/dv4mini"-directory and run the "dv4mini-server" binary. It will print some help when run with unknown command line arguments like "-help".

Depending on your linux distribution and settings you may have to add the unix user to the "dialout" (or similar) group or change the the device permissions in order to run the software as an user other than root.

WARNING: Some features of the software may require a ham radio license valid in your country!

# Web Links
- Project-Page: https://github.com/dk7lst/FreeDMR
- Wiki: https://github.com/dk7lst/FreeDMR/wiki

# Author
Main Development: Lars Struß, DK7LST (http://www.dk7lst.de/)

# License
Open Source licensed under GPL v3, see "LICENSE"-file.

The software is intended for educational purpose in the context of amateur radio.
Live the ham spirit and share your knowledge for a better world!
