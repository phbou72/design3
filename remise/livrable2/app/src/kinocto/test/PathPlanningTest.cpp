#include <gtest/gtest.h>

#include "PathPlanning.h"

namespace {
  class PathPlanningTest : public ::testing::Test {
  protected:
    PathPlanning pathPlanning;
  };


  TEST_F(PathPlanningTest, print) {

	  position obs1;
	  obs1.x = 90;
	  obs1.y = 65;
	  position obs2;
	  obs2.x = 150;
	  obs2.y = 95;
	  position start;
	  start.x = 16;
	  start.y = 65;
	  position end;
	  end.x = 200;
	  end.y = 95;
	  pathPlanning.setObstacles(obs1,obs2);
	  pathPlanning.getPath(start, end);
	  pathPlanning.printTable();
  }
}
