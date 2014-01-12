HSC
====
Hybrid Spatial Compression (HSC) is a two-stage spatial compression process. At the first stage we take advantage of shortest path table of road network to reduce the edges we need to record for spatial path. At the second stage we first use a subset of the trajectories to train a Huffman-coded sub-trajectory data structure and then use it to encode (compress) the SP compressed trajectories. The result of HSC is a compressed binary code for each trajectory.

Please use x64 compiler to compile the program or else the memory allocation will break down.

### SP-Compression.cpp
Conduct shortest path (SP) compression. Please FIRST get the shortest path table and put this folder under the same folder of the executable code. SECOND put the road network data (3 files) under the same folder of the executable code.

The program takes two arguments to execute. Please follow this command line:

	SP-Compression Spatial_File_Path Result_File_Path

You can use the sample spatial path file (Spatial_1.txt and Spatial_2.txt) in sample dataset to test the program.

### FST-Compression.cpp
Conduct Frequent Sub-Trajectory (FST) compression. Please FIRST download Trie file, AC Automaton file and Huffman file from sample dataset. These files are exactly built from the Spatial_1.txt using Trie.cpp, AC-Automaton.cpp and Huffman.cpp.

The program takes five arguments to execute. Please follow this command line:

	FST-Compression Trie_File_Path AC_Automaton_File_Path Huffman_File_Path SP_Compressed_Spatial_File_Path Result_File_Path

You can first compress the sample spatial file by SP-Compress and then use the result as the fourth argument to test the program.
