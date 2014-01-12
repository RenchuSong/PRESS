HSC
====
Hybrid Spatial Compression (HSC) is a two-stage spatial compression process. At the first stage we take advantage of shortest path table of road network to reduce the edges we need to record for spatial path. At the second stage we first use a subset of the trajectories to train a Huffman-coded sub-trajectory data structure and then use it to encode (compress) the SP compressed trajectories. The result of HSC is a compressed binary code for each trajectory.

Please use x64 compiler to compile the program or else the memory allocation will break down.

### SP-Compression.cpp
Conduct shortest path (SP) compression. Please FIRST get the shortest path table from [Baidu Cloud Netdisk](http://pan.baidu.com/s/1dDGaQJZ) and put this folder under the same folder of the executable code. SECOND put the road network data (3 files) under the same folder of the executable code.

The program takes two arguments to execute. Please follow this command line:

	SP-Compression Spatial_File_Path Result_File_Path

You can use the sample spatial path file (Spatial_1.txt and Spatial_2.txt) in sample dataset to test the program.

