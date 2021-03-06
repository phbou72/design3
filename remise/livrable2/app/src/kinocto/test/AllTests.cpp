#include <gtest/gtest.h>

#include "CaseTest.cpp"
#include "SudokubeTest.cpp"
#include "SudokubeSolverTest.cpp"
#include "NumberReaderIT.cpp"
#include "PathPlanningTest.cpp"

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

