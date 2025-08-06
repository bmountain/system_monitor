#ifndef PROCESSOR_H
#define PROCESSOR_H

#include "process.h"
#include <vector>

class Processor
{
public:
  Processor()
  : mProcesses{getProcesses()}
  {
  }

  float Utilization(); // TODO: See src/processor.cpp
  std::vector<Process> getProcesses();

  // TODO: Declare any necessary private members
private:
  std::vector<Process> mProcesses{};
};

#endif