#include "RobotDetector.h"
#include "KinectTransformator.h"
#define _USE_MATH_DEFINES
#include <math.h>

const float RobotDetector::ROBOT_RADIUS = 0.095f;
const float RobotDetector::CAMERA_OFFSET = 0.03f;
const int RobotDetector::X_ROBOT_LEFT_THRESHOLD = 180;
const int RobotDetector::X_ROBOT_RIGHT_THRESHOLD = 610;

RobotDetector::Orientation RobotDetector::getOrientation(){
    return _orientation;
}

Vec2f RobotDetector::getRobotPosition() {
    return _robotPosition;
}

float RobotDetector::getRobotAngle() {
    return _robotAngle;
}

RobotDetector::RobotDetector(){

}

RobotDetector::RobotDetector(Vec2f robotPosition)
{
    _robotPosition = robotPosition;
}

float RobotDetector::getAngleFrom2Distances(Vec2f distance1, Vec2f distance2){
    if(distance1[1] > 0 && distance2[1] > 0 && distance1[0] > 0 && distance2[0] > 0){
        float opp = distance2[1] - distance1[1];
        float adj = fabs(distance2[0] - distance1[0]);
        float angle = atan(opp/adj);
        return angle;
    }
    return 0;
}

void RobotDetector::get2MajorPointsDistance(Mat depthMatrix, vector<Point2f> validRobotPosition,
                                             Vec2f &trueLeftPosition, Vec2f &trueRightPosition){    
    Point2f leftPoint = validRobotPosition[0];
    Point2f rightPoint = validRobotPosition[validRobotPosition.size()-1];
    
    Vec3f leftPosition = depthMatrix.at<Vec3f>((int)leftPoint.y, (int)leftPoint.x);
    trueLeftPosition = KinectTransformator::getTrueCoordFromKinectCoord(leftPosition);
    if(trueLeftPosition[1] <= 0){
        leftPosition = depthMatrix.at<Vec3f>((int)leftPoint.y+5, (int)leftPoint.x+5);
        trueLeftPosition = KinectTransformator::getTrueCoordFromKinectCoord(leftPosition);
    }
    
    Vec3f rightPosition = depthMatrix.at<Vec3f>((int)rightPoint.y, (int)rightPoint.x);
    trueRightPosition = KinectTransformator::getTrueCoordFromKinectCoord(rightPosition);
    if(trueRightPosition[1] <= 0){
        rightPosition = depthMatrix.at<Vec3f>((int)rightPoint.y-5, (int)rightPoint.x-5);
        trueRightPosition = KinectTransformator::getTrueCoordFromKinectCoord(rightPosition);
    }
}

RobotDetector::Orientation RobotDetector::findOrientation(quadColor color, float angle){
    angle = angle / M_PI * 180;
    
    if(color == quadColor::BLUE){
        if(angle > 45){
            return Orientation::NORTH;
        }
        else if (angle < -45)
        {
            return Orientation::SOUTH;
        }
        else
        {
            return Orientation::EAST;
        }
    }
    else if (color == quadColor::BLACK){
        if(angle > 45){
            return Orientation::SOUTH;
        }
        else if (angle < -45)
        {
            return Orientation::NORTH;
        }
        else
        {
            return Orientation::WEST;
        }
    }
    else
    {
        return Orientation::NORTH;
    }
}

void RobotDetector::findRobotWithAngle(Mat depthMatrix, Mat rgbMatrix, Vec2f obstacle1, Vec2f obstacle2) {
    vector<Rect> validRectPosition;

    int generatedCount = generateQuads(rgbMatrix, validRectPosition);

    if(generatedCount >= 3){
        vector<Point2f> validRobotPosition;
        for (int i = 0; i < validRectPosition.size(); i++){
            validRobotPosition.push_back(Point2f(validRectPosition[i].x, validRectPosition[i].y));
        }
        
        Vec2f trueLeftPosition;
        Vec2f trueRightPosition;

        //The first run find approximate angle and the seconde one the good angle with the good position;
        get2MajorPointsDistance(depthMatrix, validRobotPosition, trueLeftPosition, trueRightPosition);
        float angleRad = getAngleFrom2Distances(trueLeftPosition, trueRightPosition);
        vector<Point2f> extremePoints = getExtremePointsOfRobot(depthMatrix, angleRad, validRobotPosition);
        get2MajorPointsDistance(depthMatrix, extremePoints, trueLeftPosition, trueRightPosition);
        angleRad = getAngleFrom2Distances(trueLeftPosition, trueRightPosition);

        quadColor quadColor = findQuadColor(rgbMatrix, validRectPosition);
        _orientation = findOrientation(quadColor, angleRad);

        Vec2f centerPosition = findRobotCenterPosition(trueRightPosition, trueLeftPosition, angleRad, _orientation);

        _robotAngle = angleRad;
        _robotPosition = centerPosition;
    }
}

vector<Point2f> RobotDetector::getExtremePointsOfRobot( Mat depthMatrix, float angleRad, 
                                                       vector<Point2f> validRobotPosition )
{ 
    vector<Point2f> pointsList;
    Vec2f squareLeftDistance;
    Vec2f squareRightDistance;

    get2MajorPointsDistance(depthMatrix, validRobotPosition, squareLeftDistance, squareRightDistance);

    Point2f leftPoint = validRobotPosition[0];
    Point2f rightPoint = validRobotPosition[validRobotPosition.size()-1];
    int consecutiveEndPoint = 0;
    Point leftRobotTrueCoords;
    Point rightRobotTrueCoords;
    
    for(int i = leftPoint.x; i > X_ROBOT_LEFT_THRESHOLD && consecutiveEndPoint < 3; i--){
        Vec3f leftPosition = depthMatrix.at<Vec3f>(leftPoint.y, i);
        Vec2f position = KinectTransformator::getTrueCoordFromKinectCoord(leftPosition);

        float expectedXDistance =  squareLeftDistance[0] - position[0];
        float expectedZDistance =  squareLeftDistance[1] + (tan(angleRad) * expectedXDistance);

        if(position[1] > expectedZDistance - 0.02 && position[1] < expectedZDistance + 0.02)
        {
            leftRobotTrueCoords = Point(i, leftPoint.y);
        }
        else
        {
            consecutiveEndPoint++;
        }
    }

    consecutiveEndPoint = 0;
    for(int i = rightPoint.x; i < X_ROBOT_RIGHT_THRESHOLD && consecutiveEndPoint < 3; i++){
        Vec3f rightPosition = depthMatrix.at<Vec3f>(rightPoint.y, i);
        Vec2f position = KinectTransformator::getTrueCoordFromKinectCoord(rightPosition);

        float expectedXDistance =  squareRightDistance[0] - position[0];
        float expectedZDistance =  squareRightDistance[1] + (tan(angleRad) * expectedXDistance);

        if(position[1] > expectedZDistance - 0.02 && position[1] < expectedZDistance + 0.02)
        {
            rightRobotTrueCoords = Point2f(i, rightPoint.y);
        }
        else
        {
            consecutiveEndPoint++;
        }
    }

    pointsList.push_back(leftRobotTrueCoords);
    pointsList.push_back(rightRobotTrueCoords);
    return pointsList;
}

Vec2f RobotDetector::findRobotCenterPosition( Vec2f trueRightPosition, Vec2f trueLeftPosition, 
                                             float angleRad, Orientation orientation) 
{
    orientation = Orientation::NORTH;
    float angle = angleRad / M_PI * 180;

    //Take account of the non-square robot because of the camera
    if((orientation == Orientation::NORTH || orientation == Orientation::EAST) && angle >= 0 ){
        trueRightPosition[1] += CAMERA_OFFSET * sin(angleRad);
        trueRightPosition[0] -= CAMERA_OFFSET * cos(angleRad);        
    }
    else if((orientation == Orientation::NORTH || orientation == Orientation::WEST) && angle <= 0) {
        trueLeftPosition[1] += CAMERA_OFFSET * sin(angleRad);
        trueLeftPosition[0] += CAMERA_OFFSET * cos(angleRad);
    }
    else if((orientation == Orientation::SOUTH || orientation == Orientation::EAST) && angle < 0){
        trueRightPosition[1] -= CAMERA_OFFSET * sin(angleRad);
        trueRightPosition[0] -= CAMERA_OFFSET * cos(angleRad);
    }
    else{
        trueLeftPosition[1] -= CAMERA_OFFSET * sin(angleRad);
        trueLeftPosition[0] += CAMERA_OFFSET * cos(angleRad);
    }

    float xDistance = ROBOT_RADIUS * sin(angleRad);
    float yDistance = ROBOT_RADIUS * cos(angleRad);

    Vec2f averagePosition((trueRightPosition[0] + trueLeftPosition[0])/2, 
        (trueRightPosition[1] + trueLeftPosition[1])/2);

    Vec2f distanceWithRadius(xDistance + averagePosition[0], yDistance + averagePosition[1]);
    
    return distanceWithRadius;
}