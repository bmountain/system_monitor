#include "linux_parser.h"

#include <dirent.h>
#include <format>
#include <iomanip>
#include <sstream>
#include <string>
#include <unistd.h>
#include <vector>

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem()
{
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel()
{
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids()
{
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization()
{
  std::ifstream file(kProcDirectory + kMeminfoFilename);
  if (file.is_open()) {
    std::string line;
    string key, value;
    float memtotal, memfree;
    while (getline(file, line)) {
      std::istringstream ss{line};
      ss >> key >> value;
      if (key == "MemTotal:") {
        memtotal = stof(value);
      }
      if (key == "MemFree:") {
        memfree = stof(value);
        return (memtotal - memfree) / (memtotal);
      }
    }
  }
  return 0;
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime()
{
  std::ifstream fs{kProcDirectory + kUptimeFilename};
  std::string line;
  std::string uptime;
  if (fs.is_open()) {
    while (getline(fs, line)) {
      std::istringstream is{line};
      is >> uptime;
    }
    return stol(uptime);
  }
  return 0;
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies()
{
  std::ifstream fs{kProcDirectory + kStatFilename};
  if (!fs.is_open()) {
    return 0;
  }
  std::string line;
  while (getline(fs, line)) {
    std::istringstream ss{line};
    std::string key;
    ss >> key;
    if (key == "cpu") {
      long sum = 0;
      for (size_t i = 0; i != 10; ++i) {
        long jeffie;
        ss >> jeffie;
        sum += jeffie;
      }
      return sum;
    }
  }
  return 0;
}

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid)
{
  std::ifstream fs{kProcDirectory + "/" + std::to_string(pid) + kStatFilename};
  if (!fs.is_open()) {
    return 0;
  }
  std::string line;
  while (getline(fs, line)) {
    std::istringstream ss{line};
    std::string tmp;
    for (size_t i = 0; i != 14; ++i) {
      ss >> tmp;
    }
    long utime, stime;
    ss >> utime >> stime;
    return utime + stime;
  }
  return 0;
}

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies()
{
  std::ifstream fs{kProcDirectory + kStatFilename};
  if (!fs.is_open()) {
    return 0;
  }
  std::string line;
  while (getline(fs, line)) {
    std::istringstream ss{line};
    std::string key;
    ss >> key;
    if (key == "cpu") {
      long user, nice, system;
      ss >> user >> nice >> system;
      return user + system;
    }
  }
  return 0;
}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies()
{
  std::ifstream fs{kProcDirectory + kStatFilename};
  if (!fs.is_open()) {
    return 0;
  }
  std::string line;
  while (getline(fs, line)) {
    std::istringstream ss{line};
    std::string key;
    ss >> key;
    if (key == "cpu") {
      long user, nice, system, idle;
      ss >> user >> nice >> system >> idle;
      return idle;
    }
  }
  return 0;
}

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization()
{
  vector<string> cpu_data;
  std::ifstream fs{kProcDirectory + kStatFilename};
  if (!fs.is_open()) {
    return cpu_data;
  }
  string line;
  while (getline(fs, line)) {
    std::istringstream ss{line};
    string key;
    ss >> key;
    if (key == "cpu") {
      long user, nice, system, idle, iowait, irq, softirq, steal, guest,
          guest_nice;
      ss >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal
          >> guest >> guest_nice;
      long active_jeffie, idle_jeffie;
      active_jeffie
          = user + nice + system + irq + softirq + steal + guest + guest_nice;
      idle_jeffie = idle + iowait;
      float utilization = static_cast<float>(active_jeffie) / idle_jeffie;
      cpu_data.push_back(to_string(utilization) + "%");
      return cpu_data;
    }
  }
  return cpu_data;
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses()
{
  std::ifstream fs{kProcDirectory + kStatFilename};
  if (!fs.is_open()) {
    return 0;
  }
  string line;
  string key;
  long n_processes;
  while (getline(fs, line)) {
    std::istringstream ss{line};
    ss >> key;
    if (key != "processes") {
      continue;
    }
    ss >> n_processes;
    return n_processes;
  }
  return 0;
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses()
{
  std::ifstream fs{kProcDirectory + kStatFilename};
  if (!fs.is_open()) {
    return 0;
  }
  string line;
  string key;
  long n_processes;
  while (getline(fs, line)) {
    std::istringstream ss{line};
    ss >> key;
    if (key != "procs_running") {
      continue;
    }
    ss >> n_processes;
    return n_processes;
  }
  return 0;
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid)
{
  std::ifstream fs{kProcDirectory + "/" + to_string(pid) + kCmdlineFilename};
  if (!fs.is_open()) {
    return string{};
  }
  string line;
  while (getline(fs, line)) {
    string command;
    std::istringstream ss{line};
    ss >> command;
    return command;
  }
  return string{};
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid)
{
  std::ifstream fs{kProcDirectory + "/" + to_string(pid) + kStatusFilename};
  if (!fs.is_open()) {
    return string{};
  }
  string line;
  while (getline(fs, line)) {
    std::istringstream ss{line};
    string key, value;
    ss >> key >> value;
    if (key != "VmRSS:") {
      continue;
    }
    float size_mb = stof(value) / 1024;
    string mem_usage = std::format("{:.1f}", size_mb);
    return mem_usage;
  }
  return string{};
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid)
{
  std::ifstream fs{kProcDirectory + "/" + to_string(pid) + kStatFilename};
  if (!fs.is_open()) {
    return string{};
  }
  string line;
  while (getline(fs, line)) {
    std::istringstream ss{line};
    string key, value;
    ss >> key >> value;
    if (key != "Uid:") {
      continue;
    }
    return value;
  }
  return string{};
}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid)
{
  string target_uid = Uid(pid);
  std::ifstream fs{kPasswordPath};
  if (!fs.is_open()) {
    return string{};
  }
  string line;
  while (getline(fs, line, ':')) {
    std::istringstream ss{line};
    string user, pass, uid;
    ss >> user >> pass >> uid;
    if (uid == target_uid) {
      return user;
    }
  }
  return string{};
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid)
{
  std::ifstream fs{kProcDirectory + "/" + to_string(pid) + kStatFilename};
  if (!fs.is_open()) {
    return 0;
  }
  long utime, stime;
  std::string line;
  while (getline(fs, line)) {
    std::istringstream ss{line};
    std::string value;
    for (size_t i = 1; i != 14; ++i) {
      ss >> value;
    }
    ss >> utime >> stime;
    return utime + stime;
  }
  return 0;
}

float LinuxParser::CpuUtilization(int pid)
{
  std::ifstream fs{kProcDirectory + "/" + to_string(pid) + kStatFilename};
  if (!fs.is_open()) {
    return 0;
  }
  long starttime;
  std::string line;
  while (getline(fs, line)) {
    std::istringstream ss{line};
    std::string value;
    for (size_t i = 1; i != 22; ++i) {
      ss >> value;
    }
    ss >> starttime;
  }

  long hertz = sysconf(_SC_CLK_TCK);
  long starttime_sec = starttime / hertz;
  long proc_uptime_sec = UpTime(pid) / hertz;
  long system_uptime_sec = UpTime();
  if (system_uptime_sec == starttime_sec) {
    return 0;
  }
  return static_cast<float>(proc_uptime_sec)
         / (system_uptime_sec - starttime_sec);
}
