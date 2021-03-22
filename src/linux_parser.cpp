#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// GENERIC FUNCTIONS
template <typename T>
T findValueByKey(std::string const &keyFilter, std::string const &filename) {
  std::string line, key;
  T value;

  std::ifstream stream(LinuxParser::kProcDirectory + filename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == keyFilter) {
          return value;
        }
      }
    }
  }
  return value;
};

template <typename T>
T getValueOfFile(std::string const &filename, int position=1) {
  
  std::string line;
  T value;

  std::ifstream stream(LinuxParser::kProcDirectory + filename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    int i = 1;
    while (linestream >> value) {
      if (i == position) {
        return value;
      }
      i++;
    }
    
  }
  return value;
};



// An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
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

// An example of how to read data from the filesystem
string LinuxParser::Kernel() {
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

// Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
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
// Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  //string memTotal = "MemTotal:";
  //string memFree = "MemFree:";
  float Total = findValueByKey<float>(LinuxParser::filterMemTotalString, kMeminfoFilename);// "/proc/memInfo"
  float Free = findValueByKey<float>(LinuxParser::filterMemFreeString, kMeminfoFilename);
  return (Total - Free) / Total;
}

long LinuxParser::UpTime() {
  
  string uptime = getValueOfFile<string>(kUptimeFilename);
  return stol(uptime);
}



// Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { 
  long total = 0;
  vector<string> cpuUtilization = LinuxParser::CpuUtilization();
  for(int i = kUser_; i <= kSteal_; i++) {  // 0...7
    total += stol(cpuUtilization[i]);
  }
  return total; 
}

// Read and return the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid) {

  long total = 0;
  // Example output of  /proc/[PID]/stat
  // We need index 13-utime, 14-stime, 15-cutime, 16-cstime
  for(int i=13; i<17;i++){
    total += std::stol(getValueOfFile<string>(to_string(pid) + kStatFilename,i));
  }
  return total;
}

// Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { 
  return LinuxParser::Jiffies() - LinuxParser::IdleJiffies();
}

// Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  vector<string> jiffies = CpuUtilization();
  return stol(jiffies[CPUStates::kIdle_]) + stol(jiffies[CPUStates::kIOwait_]);
}

// Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  
  string value;
  vector<string> values;
  // Example line & index start from "1" not "0"
  // cpu  17007 9693 7452 277318 1544 0  62  0  0   0
  for(int i=2; i<12;i++){
    value = getValueOfFile<string>(kStatFilename,i);
    values.push_back(value);  
  }
  return values;
}

// Read and return the total number of processes
int LinuxParser::TotalProcesses() {

  //string key = "processes";
  string value = findValueByKey<string>(LinuxParser::filterProcesses, kMeminfoFilename);
  return stoi(value);
}


// Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  //string key = "procs_running";
  string value = findValueByKey<string>(LinuxParser::filterRunningProcesses, kStatFilename);
  return stoi(value);
}


// Read and return the command associated with a process
string LinuxParser::Command(int pid) {
  return std::string(getValueOfFile<string>(std::to_string(pid) + kCmdlineFilename));
}


// Read and return the memory used by a process
string LinuxParser::Ram(int pid) {
  // VmData gives the exact physical memory being used as a part of Physical RAM.
  // VmSize gives the sum of all the virtual memory --  memory usage more than the Physical RAM size.
  //string key = "VmData:"; //"VmSize:";
  string value = findValueByKey<string>(LinuxParser::filterProcMem, to_string(pid) + kStatusFilename);
  return to_string(stol(value) / 1024);

}

// Read and return the memory used by a process
string LinuxParser::Uid(int pid) {
  
  //string key = "Uid:";
  string value = findValueByKey<string>(LinuxParser::filterUID, to_string(pid) + kStatusFilename);
  return value;

}


// Read and return the user associated with a process
string LinuxParser::User(int pid) { 
  
  string uid = LinuxParser::Uid(pid);
  string line, key, x, value;
  std::ifstream stream(kPasswordPath);
  if (stream.is_open()) {
   while (std::getline(stream, line)) {
     std::replace(line.begin(), line.end(), ':', ' ');
     std::istringstream linestream(line);
     linestream >> key >> x >> value;
     if (value == uid) {
       return key;
     }
   }
  }
  return "unknown";
}

// Read and return the uptime of a process
long LinuxParser::UpTime(int pid) { 
  
  int value_position = 22;
  string value = getValueOfFile<string>(to_string(pid) + kStatFilename, value_position);

  return LinuxParser::UpTime() - (stol(value) / sysconf(_SC_CLK_TCK));;
}
