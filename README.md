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
This is an ongoing effort. Currently only core algorithms of PRESS is finished. core/src/main.cpp includes sample code how to call the components of PRESS.

To compile the binary, run:
```
bash install.sh
```
It will download a sample trajectory and WA state roadnet from my google drive.
You can change the file paths in main.cpp to run it.
TODO: parameterize the file path

To get the initial experiment code, switch to main branch and take a look at v1 folder.
