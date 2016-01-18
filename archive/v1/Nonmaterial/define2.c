#define DATA_INPUT_FILE "./input_01.txt"				//采样数据文件
#define N_MAX_NODE 1000000
#define EARTHRAD 6372797												//地球半径（平均）
#define PI180 0.0174532925199432957694

/**
 * 2-dimensional array stores the coordinary nodes
 */
double *coordNode;

/**
 * The beginning index of the line segment of each road segment
 */
int *edgeStart;

/**
 * Coordinate sequence of each road segment of the road network
 */
double *coordNet;

/**
 * The Up, Bottom, Left and Right Bolder of the coordinate system
 */
double maxLat = -1e50, minLat = 1e50, maxLong = -1e50, minLong = 1e50;

/**
 * the Number of Nodes, Edges, Line Segment
 */
int nodeNumber = 0;
int edgeNumber = 0;
int lineNumber = 0;



/**
 * Adjacency list
 */
int *pre, *other, *thisSide, *last, *cost, *typeList;

///**
// * list of Length
// */
double *len;




int originNodeNumber=1000000;						//原始节点个数
int originEdgeNumber=2000000;						//原始边个数
int originLineNumber=20000000;						//原始线段个数

/**
 * the Number of RoadType
 */
int roadTypeNumber = 15;

/**
 * Sampling Rate
 */
int rate[1000];

/**
 * Time Stamp Sequence of one test file
 */
int timeSequence[200][10000];

/**
 * the number of Time Stamps in one test file
 */
int nowTime[1000];

/**
 * Sampling point coordinate sequence
 * (the nth sampling point's Lat is [2 * n], Lng is [2 * n + 1])
 */
double nodeSequence[200][20000];


int crossSize=0;									//索引的连边数量
int crossSize2=0;									//索引的连边数量

/**
 * Answer List
 */
int ans[200][10000];

//Things below are thread related
int nodeInsertedToIndex[1000000];					//每个点是否被插入了邻接表
int indexCreateFlag=0;
int doneFlag=0;
int inputLoaded[200];
//int dataNumber=0;
int gridFlag=0;
char inPathSet[1000][1000];
char outPathSet[1000][1000];

/**
 * DijkstraNode Array Index Array
 */
int DijkstraIndex[N_MAX_NODE];
int DijkstraIndex2[N_MAX_NODE];
