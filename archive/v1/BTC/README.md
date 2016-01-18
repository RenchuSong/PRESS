BTC
====
Error Bounded Temporal Compression (BTC) is a temporal compression process. We use Before Opening Window (BOPW) to reduce the d-t tuples we need to keep and conduct angular-range improvement to reduce the time complexity.

### BTC.c
Conduct temporal compression. Please FIRST put the road network data (3 files) under the same folder of the executable code.

The program takes six arguments to execute. Please follow this command line:

	BTC ./ Edge_Length_Table_Path Temporal_Sequence_File_Path Result_File_Path TSND:int NSTD:int

You can use the sample spatial path file (Temporal_1.txt and Temporal_2.txt) in sample dataset to test the program.