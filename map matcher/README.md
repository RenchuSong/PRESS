Map matcher
=====

	This is the map matching program we used in PRESS.
	The approach is our submission to ACM SIGSPATIAL Cup 2012, and our approach ranked the second place within submissions all 
	around the world.
	
	We want to emphasize that our approach achieves the highest accuracy of all five winning submissions, although the time 
	consumption is not the lowest.
	
	We haven't decided to make it open-source. However, as PRESS needs map matching as its first step, we share this executable 
	file.

## Please reference the usage of our program to ACM SIGSPATIAL Cup as following:
	The map data that will be used for the GIS Cup was obtained from OpenStreetMap. More specifically, we provide the road network 
	graph (nodes and edges) of Washington State U.S.A. as represented in three separate files:
			
	WA_Nodes.txt
	This text file contains the nodes of the road network. The file defines 535,452 nodes, with each row representing a single 
	node. Each row contains three values, with each value being separated by a single space. The form of a node row is: 
	<NodeId> <lat> <long>
		
	<NodeId>: An integer value specifying the unique identification number of the node within the road network.
	<lat>: This value specifies the latitudinal location of the node within the road network in degrees.
	<long>: This value specifies the longitudinal location of the node within the road network in degrees.
	WA_Edges.txt
	This text file contains the edges of the road network. The file defines 1,283,540 edges, with each row representing a 
	single edge. Each row contains four values, with each value being separated by a single space. The form of an edge row is: 
	<EdgeId> <from> <to> <cost>

	<EdgeId>: An integer value specifying the unique identification number of the edge within the road network.
	<from>: This value represents id of the node that is at the head of the edge. If the edge is defined as (v,w), <from>is v. These 
	node id values correspond to the <NodeId>values in WA_Nodes.txt.
	<to>: This value represents id of the node that is at the tail of the edge. If the edge is defined as (v,w), <to>is w. These node 
	id values correspond to the <NodeId>values in WA_Nodes.txt.
	<cost>: This value defines the actual cost of a vehicle to traverse from one end of the edge to the other end. It is a cost 
	function based on length of the edge and the speed limit on the road segment the edge represents.
	Note that the road network graph is a directed graph. The edge that goes from node v to node w has a different <EdgeId>from the 
	edge that goes in the other direction (from node w to node v). Please make sure that your algorithm matches the GPS reading to the 
	edge that goes in the correct direction
	
	WA_EdgeGeometry.txt
	This text files contains the geometry data of each edge in the road network. The edge geometry makes a best attempt to define 
	the polyline of the actual road that the edge is representing. The file contains 1,283,540 entries, one for each edge in the network, 
	with each entry in a single row. Each row contains a minimum of eight values, with each value being separated by a caret (^). Each 
	entry defines n different points along the edge by specifying the point’s latitude and longitude values. There will be more than 
	eight values in a single entry if the entry contains longitude/latitude information about more than just the first and last 
	points of the edge. The form of an edge geometry row is: <EdgeId>^<Name>^<Type>^<Length>^<Lat_1>^<Lon_1>^...^<Lat_n>^<Lon_n>
		
	<EdgeId>: An integer value specifying the unique identification number of the edge within the road network. This value will 
	match a single edge defined in the WA_Edges.txt file.
	<Name>: This value describes the real-world name of the road segment that this specific edge represents. If no name is defined, the 
	attribute will contain an empty string.
	<Type>: This value describes the type of road that is represented by the edge. Some common values are:
	motorway
	motorway_link
	primary
	primary_link
	secondary
	secondary_link
	tertiary
	residential
	living_street
	service
	trunk
	trunk_link
	unclassified
	<Length>: This value is the length, in meters, of the edge.
	<Lat_1>: This value is the latitude of the first point of the edge. If the edge is defined as (v,w), <Lat_1>is the latitude value 
	of v.
	<Lon_1>: This value is the longitude of the first point of the edge. If the edge is defined as (v,w), <Lon_1>is the longitude value 
	of v.
	....<Lat_i><Lon_i>....: The latitude and longitude values for several points between the first and the last points of the edge. 
	These points are optional and the number of optional points varies according to the geometry of the represented edge.
	<Lat_n>: This value is the latitude of the last point of the edge. If the edge is defined as (v,w), <Lat_n>is the latitude value 
	of w.
	<Lon_n>: This value is the longitude of the last point of the edge. If the edge is defined as (v,w), <Lon_n>is the longitude value 
	of w.
		
		
## The execution process of our program is as this:
	The mapmatch.exe accepts three command line parameters. The usage of the mapmatch.exe program is as follows: mapmatch 
	[RoadNetworkInfo_Path] [Input_Path] [Output_Path]
	
	RoadNetworkInfo_Path: Specifies the directory that contains three text files (WA_Nodes.txt, WA_Edges.txt and WA_EdgeGeometry.txt). 
	These files contain the road network of Washington State in the U.S. The format of these files is detailed under the Road 
	Network Information section.
	
	Input_Path: Specifies the directory that contains n text files. Each file is a single test case and contains a series of recorded 
	GPS readings recorded in Washington State in the U.S. The files are named as: input_01.txt to input_n.txt, where input_i.txt 
	is the file that contains the ith test case. Examples of input test cases can be found in the Training Data Sets section.
	
	Output_Path: Specifies the directory where the program is expected to place the output files. For each input file “input_i.txt” 
	the mapmatch.exe program has to generate a corresponding output file named “output_i.txt”. Examples of the output files can 
	be also found in the Training Data Sets section.
	
	Note that the number of rows in the output file output_i.txt must be the same as the input file inut_i.txt. That is for every 
	single row in the input file, there exists a row in the output file that has the same <Time>values and indicates the 
	<EdgeId>that the GPS reading is believed to be on. Also, the <EdgeId>must be annotated with a confidence value. Note that all test 
	cases contain real world GPS data recorded using a GPS logger at a variable sampling rate. The sampling rate varies between one 
	second and 30 seconds. This variable sampling rate is intended to test the resiliency of the submitted map matching algorithm 
	under sparse GPS traces. The grading formula is best described by the following algorithm:

## *Note
	Our program is focused on contest, so we didn't take robust into consideration. Our program can only process no more than 10000 
	points within 1 trajectory. So please partition the trajectory into parts if it is longer. 
