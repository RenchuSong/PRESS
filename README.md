PRESS
=====

[PRESS: A Novel Framework of Trajectory Compression in Road Networks](http://www.vldb.org/pvldb/vol7/p661-song.pdf)

## Authors
* [Renchu Song](http://www.songrenchu.com)
* [Prof. Weiwei Sun](http://homepage.fudan.edu.cn/wwsun/)
* [Prof. Baihua Zheng](http://www.mysmu.edu/faculty/bhzheng/)
* [Dr. Yu Zheng](http://research.microsoft.com/en-us/people/yuzheng/)

## V3
V3 version is the re-written version of the experiments for better code quality and ease of use.
To get the initial experiment code, switch to master branch and take a look at v1 folder.

This is an ongoing effort. Currently only PRESS core service is finished.

The code is verified to run on MacOS and Ubuntu. PRESS core compiles with g++ that supports C++ 11.

Read core/src/press_core_test_client.cpp to learn sample code of calling the APIs of PRESS service in C++.

Read core/src/ad_hoc.cpp to learn sample code of directly calling the C++ functions in PRESS without going through the service APIs. This might be good for quick ad-hoc experiments.

#### Install PRESS

To install PRESS, run:
```
./install.sh
```
It compiles service binaries, materializes config file based on the local path you cloned the code, and downloads sample dataset(s).

#### Start/Stop PRESS

To start PRESS, run:
```
./padmin.sh start
```
To stop PRESS, run:
```
./padmin.sh stop
```

#### Run sample code

To run sample code of calling the APIs of PRESS Core service, run:
```
./run_press_test_client.sh
```
Please make sure your computer has at least 8G memory to run the sample dataset(s).

TODO:
Write a UI portal to facilitate doing experiments.
