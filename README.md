# ILDPerformance
[![Build Status](https://travis-ci.org/iLCSoft/ILDPerformance.svg?branch=master)](https://travis-ci.org/iLCSoft/ILDPerformance)
[![Coverity Scan Build Status](https://scan.coverity.com/projects/12351/badge.svg)](https://scan.coverity.com/projects/ilcsoft-ildperformance)

Package to evalute the Performance of the ILD detector simulation 

ILDPerformance is distributed under the [GPLv3 License](http://www.gnu.org/licenses/gpl-3.0.en.html)

[![License](https://www.gnu.org/graphics/gplv3-127x51.png)](https://www.gnu.org/licenses/gpl-3.0.en.html)

## Usage

ILDPerformance is part of a standard iLCSoft release, so you can use it directly after initializing iLCsoft, e.g.:

```sh
source /cvmfs/ilc.desy.de/sw/x86_64_gcc49_sl6/v01-19-05/init_ilcsoft.sh
```
In order to run most of the performance scripts it is however required to have your own *writable copy* of ILDPerformance.
For this you can either copy the one from `$ILCSOFT/ILDPerformance` or directly download the HEAD version from Github:

```sh
# download HEAD version from Github
git clone https://github.com/iLCSoft/ILDPerformance.git

cd ILDPerformance
```

If you want to build your own version of ILDPerformance you can use the usual iLCSoft build sequence (only needed if you want to
make changes to the C++ code or you know that you need a newer version than the one installed in iLCSoft):


```sh
# init iLCSoft, e.g:
source /cvmfs/ilc.desy.de/sw/x86_64_gcc49_sl6/v01-19-05/init_ilcsoft.sh

mkdir build ; cd build

# configure cmake
cmake -C $ILCSOFT/ILCSoft.cmake ..

# build the library
make -j 4 install

# make Marlin use this version of ILDPerformance
export MARLIN_DLL=$PWD/../lib/libILDPerformance.so
```

See the following section for documentation on how to run the various performance plots.

### Documentation of Sub-Packages

- [Particle Identification (PID)](./PID/README.md)
- [Tracking](./tracking/README)
- [Pi0](pi0/README)
- [validateSim](validateSim/doc/README)


## License and Copyright
Copyright (C), ILDPerformance Authors

ILDPerformance is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License long with this program.  If not, see <http://www.gnu.org/licenses/>.
