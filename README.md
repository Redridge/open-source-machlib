# open-source-machlib

[![Build Status](https://travis-ci.org/Redridge/open-source-machlib.svg?branch=master)](https://travis-ci.org/Redridge/open-source-machlib) [![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](https://opensource.org/licenses/MIT)
# 

The goal of this project is to create an open source equivalent of machlib: a library for inspecting Apple binary format files, particularly the iOS kernelcache. It will provide features to list library calls, system calls, kernel extensions, segments and sections and others. It will be a library that could then be used as a back-end for other tools.


### Installing

Downloading the necessary dependecies.

```
make prepare
```
Building the sources, generating the library and a sample application.
The library and the sample application will be located inside the bin directory

```
make
```
Changing the LIBRARY PATH.

Linux 

```
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/path/to/bin
```
Macos

```
export DYLD_LIBRARY_PATH=$DYLD_LIBRARY_PATH:/path/to/bin
```




