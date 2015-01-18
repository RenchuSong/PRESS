Sample Data
=====
According to the license of Singapore taxi trajectory dataset, we are NOT allowed to publish the original dataset. So we only publish the spatial and temporal representation of trajectories in road networks as the basic data. 

Due to space limitation of github, we share the data in [Baidu Cloud Netdisk](http://pan.baidu.com/s/1kT0zExp). 

We select the trajectories of the first day and the second day as an example. The trajectories of the first day serves as training dataset of the FST Compression stage in HSC. The data included is as following:

### Singapore road network
Please refer to the [ACM SIGSPATIAL Cup 2012 data format](https://github.com/RenchuSong/PRESS/tree/master/map%20matcher).

WA_EdgeGeometry.txt: The road geometry information.

WA_Edges.txt: The two ends of each road.

WA_Nodes.txt: The coordinate of each intersection.

### Spatial Component
The spatial component of each day is named as Spatial_X.txt. This is a binary file. The file format is:

[Trajectory Number: tNumber, an Integer] followed by tNumber groups of data where each group describes a trajectory.

Each group: [Point Number: pNumber, an Integer] followed by pNumber integers [Edge ID, the road that the trajectory follows].

### Temporal Component
The temporal component of each day is named as Temporal_X.txt. This is a binary file. The file format is:

[Trajectory Number: tNumber, an Integer] followed by tNumber groups of data where each group describes a trajectory.

Each group: [Point Number: pNumber, an Integer] followed by pNumber tuple integers [Distance: Integer, the distance from the start point of the trajectory; Time: Integer, the time that the object is at that position].
