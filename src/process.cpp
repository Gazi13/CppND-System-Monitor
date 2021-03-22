#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>
#include <unistd.h>
#include <iostream>

#include "process.h"
#include "linux_parser.h"

using namespace std;
using std::stol;
using std::string;
using std::to_string;
using std::vector;




// Constructor 
Process::Process(int pid) {
  Process::pid_ = pid;
}

// Return this process's ID
int Process::Pid() { return pid_; }

// Return this process's CPU utilization
// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
float Process::CpuUtilization() { 
  
  float totalTime = float(LinuxParser::ActiveJiffies(Pid()));
  float startTime = float(LinuxParser::UpTime(Pid()));
  float upTime = float(LinuxParser::UpTime());
  
  //float seconds = upTime - (startTime / sysconf(_SC_CLK_TCK)) ;
  
  //cout << " totalTime/hrz:"<< totalTime/sysconf(_SC_CLK_TCK)<<" / second:"<<seconds << "\n" << " upTime: " << upTime << "- startTime/hrz" << (startTime / sysconf(_SC_CLK_TCK)) << " id:" << Pid()<<" ";
  
  return (totalTime / sysconf(_SC_CLK_TCK) / (startTime));
}


// Return the command that generated this process
string Process::Command() { return LinuxParser::Command(pid_); }

// Return this process's memory utilization
string Process::Ram() { return LinuxParser::Ram(pid_); }

// Return the user (name) that generated this process
string Process::User() { return LinuxParser::User(pid_); }

// Return the age of this process (in seconds)
long int Process::UpTime() { return LinuxParser::UpTime(pid_); }

// Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const& a) const { 
  return  stol(LinuxParser::Ram(a.pid_)) < stol(LinuxParser::Ram(pid_));
}