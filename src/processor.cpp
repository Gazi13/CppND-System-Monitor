#include <thread>
#include "processor.h"
#include "linux_parser.h"

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() { 
  totalJiffiesStart = LinuxParser::Jiffies();
  activeJiffiesStart = LinuxParser::ActiveJiffies();
  
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  
  totalJiffiesEnd = LinuxParser::Jiffies();
  activeJiffiesEnd = LinuxParser::ActiveJiffies();
  
  long totalDelta = totalJiffiesEnd - totalJiffiesStart;
  long activeDelta = activeJiffiesEnd - activeJiffiesStart;
  
  if (totalDelta == 0) {
    return 0.0;
  }
  
  return float(activeDelta) / float(totalDelta); 
}