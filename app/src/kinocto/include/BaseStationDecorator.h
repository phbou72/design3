#ifndef BASESTATIONDECORATION_H_
#define BASESTATIONDECORATION_H_

#include "ros/ros.h"
#include "Position.h"

#include "basestation/FindRobotPosition.h"
#include "basestation/FindObstaclesPosition.h"
#include "basestation/ShowSolvedSudocube.h"
#include "basestation/LoopEnded.h"
#include "basestation/TraceRealTrajectory.h"
#include "basestation/UpdateRobotPosition.h"
#include "basestation/ShowConfirmStartRobot.h"

class BaseStationDecorator{
public:
    BaseStationDecorator(ros::NodeHandle & nodeHandle);
    virtual ~BaseStationDecorator();

    //Request
    Position requestRobotPosition();
    std::vector<Position> requestObstaclesPosition();

    //Messsages
    void sendSolvedSudocube(std::string sudocube, int redCaseValue);
    void sendLoopEndedMessage();
    void sendTrajectory(std::vector<Position> positions);
    void sendUpdateRobotPositionMessage(Position position);
    void sendConfirmRobotStarted();
private:
    ros::NodeHandle nodeHandle;

    ros::ServiceClient findRobotPositionClient;
    ros::ServiceClient findObstaclesPositionClient;
    ros::ServiceClient showSolvedSudocubeClient;
    ros::ServiceClient loopEndedClient;
    ros::ServiceClient traceRealTrajectoryClient;
    ros::ServiceClient updateRobotPositionClient;
    ros::ServiceClient showConfirmStartRobotClient;
};

#endif /* BASESTATIONDECORATION_H_ */
