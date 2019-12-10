/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/*
 * @authors: Alaettin Serhan Mete, Hasan Ozturk - alaettin.serhan.mete@cern.ch, haozturk@cern.ch
 */


#ifndef PERFMONCOMPS_PERFMONMTUTILS_H
#define PERFMONCOMPS_PERFMONMTUTILS_H

// STL includes
#include <ctime>
#include <chrono>
#include <fstream>

#include <sys/stat.h>  // to check whether /proc/* exists in the machine

typedef std::map< std::string, long > memory_map_t; // Component : Memory Measurement(kB)


/*
 * Inline function prototypes
*/
inline memory_map_t operator-( memory_map_t& map1,  memory_map_t& map2);
inline bool doesDirectoryExist(const std::string dir);

/*
 * Necessary tools
 */
namespace PMonMT {
  // Base methods for collecting data
  double get_thread_cpu_time();
  double get_process_cpu_time();
  double get_wall_time();
  memory_map_t get_mem_stats();
  

  // Step name and Component name pairs. Ex: Initialize - StoreGateSvc
  struct StepComp {
  
    std::string stepName;
    std::string compName;
  
    // Overload < operator, because we are using a custom key(StepComp)  for std::map
    bool operator<(const StepComp& sc) const {
      return std::make_pair( this->stepName, this->compName ) < std::make_pair( sc.stepName, sc.compName );
    }
  
  };  

  // Basic Measurement
  struct Measurement {

    typedef std::map< int, Measurement > event_meas_map_t; // Event number: Measurement

    // Variables to store measurements
    double cpu_time;
    double wall_time;
    memory_map_t mem_stats; // Vmem, Rss, Pss, Swap
    
    // This map stores the measurements captured in the event loop
    //std::map< int, Measurement > parallel_meas_map; // [Event count so far]: Measurement
    event_meas_map_t parallel_meas_map; // [Event count so far]: Measurement

    // Peak values for Vmem, Rss and Pss
    long vmemPeak = LONG_MIN;
    long rssPeak = LONG_MIN;
    long pssPeak = LONG_MIN;

    // Capture for serial steps
    void capture() {
      cpu_time = get_process_cpu_time();
      wall_time = get_wall_time();

      if(doesDirectoryExist("/proc")){
        mem_stats = get_mem_stats();
        if(mem_stats["vmem"] > vmemPeak)
          vmemPeak = mem_stats["vmem"];
        if(mem_stats["rss"] > rssPeak)
          rssPeak = mem_stats["rss"];
        if(mem_stats["pss"] > pssPeak)
          pssPeak = mem_stats["pss"];
      }
        
    }

    // Capture for serial steps
    void capture_MT ( int eventCount ) {
          
      cpu_time = get_process_cpu_time();
      wall_time = get_wall_time(); 

      Measurement meas;

      if(doesDirectoryExist("/proc")){
        mem_stats = get_mem_stats();
        meas.mem_stats = mem_stats;

        if(mem_stats["vmem"] > vmemPeak)
          vmemPeak = mem_stats["vmem"];
        if(mem_stats["rss"] > rssPeak)
          rssPeak = mem_stats["rss"];
        if(mem_stats["pss"] > pssPeak)
          pssPeak = mem_stats["pss"];
      }
      
      meas.cpu_time = cpu_time;
      meas.wall_time = wall_time;

      parallel_meas_map[eventCount] = meas;
    }

    Measurement() : cpu_time{0.}, wall_time{0.} { }
  };

  // Basic Data
  struct MeasurementData {

    typedef std::map< int, Measurement > event_meas_map_t; // Event number: Measurement

    // These variables are used to calculate and store the serial component level measurements
    double m_tmp_cpu, m_delta_cpu;
    double m_tmp_wall, m_delta_wall;
    memory_map_t m_memMon_tmp_map;
    memory_map_t m_memMon_delta_map;

    // This map is used to store the event level measurements
    event_meas_map_t m_parallel_delta_map;

    // Offset variables
    double m_offset_cpu;
    double m_offset_wall;
    memory_map_t m_offset_mem;
    
    void addPointStart(const Measurement& meas) {          

      m_tmp_cpu = meas.cpu_time;
      m_tmp_wall = meas.wall_time;
      
      if(doesDirectoryExist("/proc"))
        m_memMon_tmp_map = meas.mem_stats;
    }
    // make const
    void addPointStop(Measurement& meas)  {     

      m_delta_cpu = meas.cpu_time - m_tmp_cpu;
      m_delta_wall = meas.wall_time - m_tmp_wall;

      if(doesDirectoryExist("/proc"))
        m_memMon_delta_map = meas.mem_stats - m_memMon_tmp_map;  
    }

    // Record the event level measurements
    void record_MT(Measurement& meas, int eventCount ){

      // If it is the first event, set it as offset
      if(eventCount == 0){
        m_offset_cpu = meas.parallel_meas_map[eventCount].cpu_time;
        m_offset_wall = meas.parallel_meas_map[eventCount].wall_time;
        m_offset_mem = meas.parallel_meas_map[eventCount].mem_stats;
      }

      m_parallel_delta_map[eventCount].cpu_time = meas.parallel_meas_map[eventCount].cpu_time - m_offset_cpu;
      m_parallel_delta_map[eventCount].wall_time = meas.parallel_meas_map[eventCount].wall_time - m_offset_wall;

      if(doesDirectoryExist("/proc")){
        m_parallel_delta_map[eventCount].mem_stats["vmem"] = meas.parallel_meas_map[eventCount].mem_stats["vmem"] - m_offset_mem["vmem"];
        m_parallel_delta_map[eventCount].mem_stats["rss"] = meas.parallel_meas_map[eventCount].mem_stats["rss"] - m_offset_mem["rss"];
        m_parallel_delta_map[eventCount].mem_stats["pss"] = meas.parallel_meas_map[eventCount].mem_stats["pss"] - m_offset_mem["pss"];
        m_parallel_delta_map[eventCount].mem_stats["swap"] = meas.parallel_meas_map[eventCount].mem_stats["swap"] - m_offset_mem["swap"];
      }

    }

    event_meas_map_t getParallelDeltaMap() const{
      return m_parallel_delta_map;
    }

    double getDeltaCPU() const{
      return m_delta_cpu;
    }

    double getDeltaWall() const{
      return m_delta_wall;
    }

    long getMemMonDeltaMap(std::string mem_stat) {
      return m_memMon_delta_map[mem_stat];
    }

    
    MeasurementData() : m_tmp_cpu{0.}, m_delta_cpu{0.}, m_tmp_wall{0.}, m_delta_wall{0.} { }
  };
  
}


///////////////////////////////////////////////////////////////////
// Inline methods:
///////////////////////////////////////////////////////////////////

/*
 * Thread specific CPU time measurement in ms
 */
inline double PMonMT::get_thread_cpu_time() {
  // Get the thread clock id
  clockid_t thread_cid;
  pthread_getcpuclockid(pthread_self(),&thread_cid);

  // Get the thread specific CPU time
  struct timespec ctime;
  clock_gettime(thread_cid, &ctime);

  // Return the measurement in ms
  return static_cast<double>(ctime.tv_sec*1.e3 + ctime.tv_nsec*1.e-6);
}

/*
 * Process specific CPU time measurement in ms
 */
inline double PMonMT::get_process_cpu_time() {
  return static_cast<double>(std::clock()*(1.e3/CLOCKS_PER_SEC));
}

/*
 * Wall-time measurement since epoch in ms
 */
inline double PMonMT::get_wall_time() {
  return static_cast<double>(std::chrono::system_clock::now().time_since_epoch() /
                             std::chrono::milliseconds(1));
}

/*
 * Memory statistics for serial steps
 */

 // Read from process
inline memory_map_t PMonMT::get_mem_stats(){

  memory_map_t result;
  std::string fileName = "/proc/self/smaps";
  std::ifstream smaps_file(fileName); 
 
  std::string line;
  std::string key;
  std::string value;

  while(getline(smaps_file, line)){

    std::stringstream ss(line);
    ss >> key >> value;

    if(key == "Size:"){
      result["vmem"] += stol(value);
    }
    if(key == "Rss:"){
      result["rss"] += stol(value);
    }
    if(key == "Pss:"){
      result["pss"] += stol(value);
    }
    if(key == "Swap:"){
      result["swap"] += stol(value);
    }

  }
  return result; 
}

inline memory_map_t operator-( memory_map_t& map1,  memory_map_t& map2){
  memory_map_t result_map;
  for(auto it : map1){
    result_map[it.first] = map1[it.first] - map2[it.first];
  }
  return result_map;
}

inline bool doesDirectoryExist(const std::string dir){
  struct stat buffer;
  return (stat (dir.c_str(), &buffer) == 0);
}


#endif // PERFMONCOMPS_PERFMONMTUTILS_H
