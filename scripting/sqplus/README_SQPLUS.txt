May-12-2008

Notes on some directories:

- sqplus          - Main SqPlus bindings directory
 
- testSqPlus2unit - Current test suite for SqPlus
                    
    This directory, with one test case per file, is 
    also a good example directory, where a many SqPlus 
    features are tested one by one.

    On making changes to SqPlus, verify that the unit 
    tests succeeds for:
      * Win32 (2K/XP/Vista) [preferably both Unicode and ANSI builds]
      * Linux/GCC [ANSI/UTF8 build the central one]


The file sqplus/changes.txt describes recent changes to SqPlus.

// ATS

-- 

10/08/05

To build and test the minimalSqPlus/ example:

   Open Squirrel.sln and build for Debug and Release.

   Open the minimalSqPlus/minimalSqPlus.sln and build (Debug or Release),
   then run.

Alternatively, after building testSqPlus2 in Squirrel.sln, set
minimalSqPlus as the Startup Project, then build and run (dependencies
are not set for minimalSqPlus to illustrate building with pre-compiled
libs).

See testSqPlus2/testSqPlus2.cpp for many examples (in the Squirrel.sln
solution).

testSqPlus/testSqPlus.cpp is included to show how the old version
using macros was used (it is no longer updated).

John
