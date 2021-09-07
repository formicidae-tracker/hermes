.. raw:: html

    <p align="center"><img src="https://github.com/formicidae-tracker/hermes/raw/master/resources/icons/fort-hermes.svg" width="200px" alt="fort-hermes logo"></p>

``fort-hermes`` — Tracking Data exchange format for the FORMicidae Tracker
**************************************************************************

|Build Status| |Coverage Status| |Stable Documentation Status| |PyPI package| |License|

`FORT Project Wiki <https://github.com/formicidae-tracker/documentation/wiki>`_
• `Report a Bug <https://github.com/fortmicidae-tracker/hermes/issues/new>`_
• `Request a Feature <https://github.com/formicidae-tracker/hermes/issues/new>`_

**fort-hermes** is a small library, implemented in C/C++, go and
python for the exchange format of tracking data for the FORmicidae
Tracker project. This is mostly based on protocol buffer. It provides
utilities to access file sequence of saved tracking data or to connect
to live tracking instance to access broadcasted data.

Installation
------------

C/C++
=====

**libfort-hermes** and **libfort-hermes-cpp** packages are distributed
within the larger conda **libfort-myrmidon** package through the
`formicidae-tracker <https://anaconda.org/formicidae-tracker>`_ channel.

.. code-block:: bash

   conda install -c formicidae-tracker libfort-myrmidon

Python
======

The **py_fort_hermes** package is available through PyPI
`formicidae-tracker-hermes <https://pypi.org/project/formicidae-tracker-hermes/>`_
project.

Golang
======

.. code-block:: bash

   go get github.com/formicidae-tracker/hermes


Versioning
==========

**fort-hermes** use `SemVer <http://semver.org/>`_ for versioning. For the versions
available, see the `releases <https://github.com/formicidae-tracker/hermes/releases>`_

Authors
-------

The file `AUTHORS
<https://github.com/formicidae-tracker/hermes/blob/masert/AUTHORS>`_
lists all copyright holders (physical or moral person) for this
repository.

See also the list of `contributors
<https://github.com/formicidae-tracker/hermes/contributors>`_ who
participated in this project.

License
-------

This project is licensed under the GNU Lesser General Public License
v3.0 or later - see the `LICENSE
<https://github.com/formicidae-tracker/hermes/blob/master/LICENSE>`_
file for details

..


.. |Build Status| image:: https://github.com/formicidae-tracker/hermes/actions/workflows/build.yml/badge.svg
   :target: https://github.com/formicidae-tracker/hermes/actions/workflows/build.yml
.. |coverage Status| image:: https://codecov.io/gh/formicidae-tracker/hermes/branch/master/graph/badge.svg
   :target: https://codecov.io/gh/formicidae-tracker/hermes
.. |Stable Documentation Status| image:: https://github.com/formicidae-tracker/hermes/actions/workflows/docs.yml/badge.svg
   :target: https://formicidae-tracker.github.io/hermes/latest
.. |PyPI package|  image:: https://img.shields.io/pypi/v/formicidae-tracker-hermes.svg
   :target: https://pypi.org/project/formicidae-tracker-hermes/
.. |License| image:: https://img.shields.io/github/license/formicidae-tracker/hermes.svg
   :target: https://github.com/formicidae-tracker/hermes/blob/master/LICENSE
