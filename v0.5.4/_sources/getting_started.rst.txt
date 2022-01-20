.. getting_started

Getting Started
===============

C++
^^^

Installation from conda
-----------------------

It is recommanded to install the `conda <https://conda.io>`_ package
`libfort-myrmidon
<https://anaconda.org/formicidae-tracker/libfort-myrmidon>`_ from a
fresh miniconda install:

.. code-block:: bash

   conda create -n my-env
   conda activate my-env
   conda install -c formicidae-tracker libfort-myrmidon

This will install the **fort-hermes** library which
**libfort-myrmidon** depends upon. Alternatively one can install the
library from sources.

Installation from sources
-------------------------

The library requires a recent **cmake**, **libasio-dev**,
**libprotobuf-dev** and **protobuf-compiler** packages on Debian based distribution.

.. code-block:: bash

   git clone https://github.com/formicidae-tracker/hermes
   cd hermes
   mkdir -p build
   cd build
   cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo ../
   make
   sudo make install

Linking to a C++ project
------------------------

CMake based project
~~~~~~~~~~~~~~~~~~~

The installation provides a **FortHermesCppConfig.cmake** file simply use
in your project:

.. code-block:: cmake

   find_package(FortHermesCpp REQUIRED)
   include_directories(${FORT_HERMES_CPP_INCLUDE_DIRS})

   target_link_libraries(my-target ${FORT_HERMES_CPP_LIBRARIES})

**pkg-config** project
~~~~~~~~~~~~~~~~~~~~~~

The package provides a **fort-hermes.pc** files to provides cflags and
linker options.

Go
^^^

The package `github.com/formicidae-tracker/hermes/src/go/hermes
<https://pkg.go.dev/github.com/formicidae-tracker/hermes@v0.5.0/src/go/hermes>`_
provides compiled protobuf messages and utilities.

Python
^^^^^^

The python package is available through `PyPI's project
formicidae-tracker-hermes
<https://pypi.org/project/formicidae-tracker-hermes/>`_
