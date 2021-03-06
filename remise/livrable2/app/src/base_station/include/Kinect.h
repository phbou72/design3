#ifndef __kinect_H_
#define __kinect_H_

#include <iostream>
#include <list>
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"

using namespace cv;
using namespace std;

class Kinect {

private:

    const static float OBSTACLE_RADIUS;
    static float const OBSTACLE_HEIGHT;
    const static float KINECTANGLE;
    const static float X_KINECT_POSITION;
    const static float Z_KINECT_POSITION;
    const static int X_OBSTACLE_LEFT_THRESHOLD;
    const static int X_OBSTACLE_RIGHT_THRESHOLD;
    const static int Y_OBSTACLE_TOP_THRESHOLD;
    const static int Y_OBSTACLE_BOTTOM_THRESHOLD;
    const static float OBSTACLE_DISTANCE_MIN_THRESHOLD;
    const static float OBSTACLE_DISTANCE_MAX_THRESHOLD;
    static float const ROBOT_RADIUS;
    static int const X_ROBOT_LEFT_THRESHOLD;
    static int const X_ROBOT_RIGHT_THRESHOLD;
    static int const Y_ROBOT_TOP_THRESHOLD;
    static int const Y_ROBOT_BOTTOM_THRESHOLD;
    static float const ROBOT_MAX_DISTANCE;
    static float const ROBOT_MIN_DISTANCE;
    static float const ROBOT_HEIGHT;

    Vec2f obstacle1;
    Vec2f obstacle2;
    Vec2f robot;

    int getAverageFromPointList(list<Point> obstacle);

    void findAllPossiblePositionForEachObstacle(Mat depthMatrix, list<Point> *obstacle1, list<Point> *obstacle2);

    vector<Point> findAllPossiblePositionForRobot(Mat depthMatrix, Vec2f obstacle1, Vec2f obstacle2);

    list<Vec2f> getSomeYDistanceAssociatedWithXForObstacle(int obstaclePositionX, Mat depthMatrix);

    list<Vec2f> getSomeYDistanceAssociatedWithXForRobot(int robotPositionX, Mat depthMatrix);

    static Vec2f getRotatedXZCoordFromKinectCoord(Vec3f depthXYZ);

    static Vec2f addObstacleRadiusToDistance(Vec2f distanceExtObstacle);

    static Vec2f translateXZCoordtoOrigin(Vec2f rotatedXZ);

    static Vec2f translateXZCoordtoKinect(Vec2f positionXZ);

    Vec2f getAverageDistanceForPointLine(list<Vec2f> allDistances);

    Vec2f getAveragePositionForObstacle(Mat depthMatrix, list<Point> obstacle);

    int getAverageFromPointListWithConditions(vector<Point> robotPositions, float minCondition, float maxCondition);

public:

    Vec2f getObstacle1();

    Vec2f getObstacle2();

    vector<Vec2f> findCenteredObstacle(Mat depthMatrix);

    static Vec2f getTrueCoordFromKinectCoord(Vec3f depthXYZ);

    Vec2f findRobot(Mat depthMatrix);

    Vec2f getRobot();

};

#endif //__kinect_H_
