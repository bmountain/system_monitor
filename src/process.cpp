#include "process.h"

#include <unistd.h>

#include <cctype>
#include <sstream>
#include <string>
#include <vector>

using std::string;
using std::to_string;
using std::vector;

// TODO: Return this process's ID
int Process::Pid() const
{
  return mPid;
}

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() const
{
  return mCpuUtilization;
}

// TODO: Return the command that generated this process
string Process::Command() const
{
  return mCommand;
}

// TODO: Return this process's memory utilization
string Process::Ram() const
{
  return mRam;
}

// TODO: Return the user (name) that generated this process
string Process::User() const
{
  return mUser;
}

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() const
{
  return mUpTime;
}

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a) const
{
  return this->CpuUtilization() > a.CpuUtilization();
}