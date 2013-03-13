#ifndef PATHPLANNING2_H_
#define PATHPLANNING2_H_

#include <vector>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "Node.h"

const int TABLE_X = 231;
const int TABLE_Y = 114;
const int ROBOT_RADIUS = 13;
const int OBSTACLE_RADIUS = 7;
const int DRAWING_ZONE = 89;
const int BUFFER_SIZE = ROBOT_RADIUS + 3;

struct move {
    int angle;
    int distance;
};

class PathPlanning2 {
public:
    PathPlanning2();
    virtual ~PathPlanning2();

    std::vector<move> getPath(Position, Position);
    void setObstacles(Position, Position);
    void printTable();

private:
    Position obstacle1;
    Position obstacle2;
    bool obstaclesPositionsOK(Position, Position);

	std::vector<Node*> listOfNodes;
	Node* startNode;
	Node* destinationNode;
    void constructGraph();
	void createNodes();
	void addNode(Node*);
	void connectNodes();
	void clearConnections();
	std::vector<Position> getObstacleCorners();

	std::vector<move> findPathInGraph();
	move convertToMove(Position, Position);

    bool pointPassesThroughObstacle(Position);
    bool linePassesThroughObstacle(Position, Position);


	//PRINTING RELATED METHODS
	void updateMatrixTable();
	// In array,
	// 1 = normal area
	// 9 = wall or obstacle
	int table[TABLE_X + 1][TABLE_Y + 1];

	cv::Scalar white;
	cv::Scalar blue;
	cv::Scalar black;
	void showWindowWith(const char*, const cv::Mat &);
	void colorPixel(cv::Mat&, cv::Scalar, int, int);
};


#endif /* PATHPLANNING2_H_ */