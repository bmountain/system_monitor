#include "processor.h"
#include <numeric>

// TODO: Return the aggregate CPU utilization
float Processor::Utilization()
{
  float utilization
      = std::accumulate(mProcesses.begin(), mProcesses.end(), 0.0F,
                        [](float acc, Process process)
                        {
                          return acc + process.CpuUtilization();
                        });
  return utilization;
}

std::vector<Process> Processor::getProcesses()
{
  std::vector<int> pids = LinuxParser::Pids();
  std::vector<Process> processes;
  for (const int i : pids) {
    Process process{i};
    processes.push_back(process);
  }
  std::sort(processes.begin(), processes.end());
  return processes;
}
