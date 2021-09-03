[![Build Status][build-status-shield]][build-status-url]
[![Coverage Status][coverage-status-shield]][coverage-status-url]
[![Contributors][contributors-shield]][contributors-url]
[![Issues][issues-shield]][issues-url]
[![LGPL License][license-shield]][license-url]

<br />
<p align="center">
  <a href="https://github.com/formicidae-tracker/hermes">
    <img src="resources/icons/fort-hermes.svg" alt="Logo" width="180" height="180">
  </a>
  <h1 align="center"><code> fort-hermes</code></h1>
  <p align="center">
	Tracking data exchange format and utilities for the FORmicidae Tracker.
    <br />
    <a href="https://fort-hermes.readthedocs.io/en/stable/"><strong><code>fort-hermes</code> docs »</strong></a>
    <br />
    <br />
    <a href="https://github.com/formicidae-tracker/documentation/wiki">FORT Project Wiki</a>
    ·
    <a href="https://github.com/formicidae-tracker/hermes/issues">Report Bug</a>
    ·
    <a href="https://github.com/formicidae-tracker/hermes/issues">Request Feature</a>
  </p>
</p>


## About

`fort-hermes` is a small library, implemented in C/C++, go and python
for the exchange format of tracking data for the FORmicidae Tracker
project. It provides utilities to access file sequence of saved
tracking data or to connect to live tracking instance to access
broadcasted data.

## Getting started

### C/C++

`libfort-hermes` and `libfort-hermes-cpp` packages are distributed
within the larger conda `libfort-myrmidon` package through the
[formicidae-tracker](https://anaconda.org/formicidae-tracker) channel.

``` bash
conda install -c formicidae-tracker libfort-myrmidon
```

### Python

The `py_fort_hermes` package is available through PyPI
[formicidae-tracker-hermes](https://pypi.org/project/formicidae-tracker-hermes/)
project.

### Golang

``` bash
go get github.com/formicidae-tracker/hermes
```

### Documentation

[Documentation](https://fort-hermes.readthedocs.io) is hosted through readthedocs.

## Contributing

Please read [CONTRIBUTING.md](CONTRIBUTING.md) for details on our code
of conduct, and the process for submitting pull requests.

## Versioning

We use [SemVer](http://semver.org/) for versioning. For the versions
available, see the
[releases](https://github.com/formicidae-tracker/hermes/releases).

## Authors

The file [AUTHORS](AUTHORS) lists all copyright holders (physical or moral person) for this repository.

See also the list of [contributors](https://github.com/formicidae-tracker/hermes/contributors) who participated in this project.

## License

This project is licensed under the GNU Lesser General Public License v3.0 or later - see the [COPYING.LESSER](COPYING.LESSER) file for details


## Documentation
C API Documentation can be found at https://formicidae-tracker.github.io/hermes/index.html


[build-status-shield]: https://github.com/formicidae-tracker/hermes/actions/workflows/build.yml/badge.svg
[build-status-url]: https://github.com/formicidae-tracker/hermes/actions/workflows/build.yml
[coverage-status-shield]: https://codecov.io/gh/formicidae-tracker/hermes/branch/master/graph/badge.svg
[coverage-status-url]: https://codecov.io/gh/formicidae-tracker/hermes
[contributors-shield]: https://img.shields.io/github/contributors/formicidae-tracker/hermes.svg
[contributors-url]: https://github.com/formicidae-tracker/hermes/graphs/contributors
[issues-shield]: https://img.shields.io/github/issues/formicidae-tracker/hermes.svg
[issues-url]: https://github.com/formicidae-tracker/hermes/issues
[license-shield]: https://img.shields.io/github/license/formicidae-tracker/hermes.svg
[license-url]: https://github.com/formicidae-tracker/hermes/blob/master/COPYING.LESSER

[![codecov](?token=UTIV500Z66)]()
