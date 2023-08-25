#include <boost/log/core.hpp>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <chrono>
#include <thread>
#include <sstream>
#include <iostream>
#include <sys/times.h>
#include <sys/vtimes.h>

#include <Performance.h>
#include <Logging.h>

Performance_Monitoring::Performance_Monitoring(std::atomic<bool>& stop_thread_flag)
    : stop_thread_flag(&stop_thread_flag){
        init();
    }

void Performance_Monitoring::monitor(){
    Logging log;

    while(stop_thread_flag){

        // process CPU; total CPU; process memory; ram avail; total ram; avalable swap; total swap
        std::stringstream ss;
        ss << get_process_cpu() << ";" << get_cpu() << ";" <<
        get_mem_data("/proc/self/status", "VmRSS:") << ";" << 
        get_mem_data("/proc/meminfo", "MemAvailable:") <<  ";" << get_mem_data("/proc/meminfo", "MemTotal:") <<  ";" << 
        get_mem_data("/proc/meminfo", "SwapFree:") << ";" << get_mem_data("/proc/meminfo", "SwapTotal:");
        
        log.log_trace(ss.str(), "PERFORMANCE_LOGGING");

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}

int Performance_Monitoring::parse_line(char* line){
    // This assumes that a digit will be found and the line ends in " Kb".
    int i = strlen(line);
    const char* p = line;
    while(*p <'0' || *p > '9'){
        p++;
    }
    line[i-3] = '\0';
    i = atoi(p);
    return i;
}

// returns data in KB
int Performance_Monitoring::get_mem_data(const char* path, const char* line_name){
    FILE* file = fopen(path, "r");
    int result = -1;
    char line[128];

    while(fgets(line, 128, file) != NULL){
        if(strncmp(line, line_name, 6) == 0){
            result = parse_line(line);
            break;
        }
    }
    fclose(file);
    return result;
}

void Performance_Monitoring::init(){
    FILE* file_process_cpu;
    struct tms timeSample;
    char line[128];

    lastCPU = times(&timeSample);
    lastSysCPU = timeSample.tms_stime;
    lastUserCPU = timeSample.tms_utime;

    file_process_cpu = fopen("/proc/cpuinfo", "r");
    numProcessors = 0;
    while(fgets(line, 128, file_process_cpu) != NULL){
        if (strncmp(line, "processor", 9) == 0) numProcessors++;
    }
    fclose(file_process_cpu);

    FILE* file_cpu = fopen("/proc/stat", "r");
    fscanf(file_cpu, "cpu %llu %llu %llu %llu", &lastTotalUser, &lastTotalUserLow,
        &lastTotalSys, &lastTotalIdle);
    fclose(file_cpu);
}

double Performance_Monitoring::get_process_cpu(){
    struct tms timeSample;
    clock_t now;
    double percent;

    now = times(&timeSample);
    if (now <= lastCPU || timeSample.tms_stime < lastSysCPU ||
        timeSample.tms_utime < lastUserCPU){
        //Overflow detection. Just skip this value.
        percent = -1.0;
    }
    else{
        percent = (timeSample.tms_stime - lastSysCPU) +
            (timeSample.tms_utime - lastUserCPU);
        percent /= (now - lastCPU);
        percent /= numProcessors;
        percent *= 100;
    }
    lastCPU = now;
    lastSysCPU = timeSample.tms_stime;
    lastUserCPU = timeSample.tms_utime;

    return percent;
}

double Performance_Monitoring::get_cpu(){
    double percent;
    FILE* file;
    unsigned long long totalUser, totalUserLow, totalSys, totalIdle, total;

    file = fopen("/proc/stat", "r");
    fscanf(file, "cpu %llu %llu %llu %llu", &totalUser, &totalUserLow,
        &totalSys, &totalIdle);
    fclose(file);

    if (totalUser < lastTotalUser || totalUserLow < lastTotalUserLow ||
        totalSys < lastTotalSys || totalIdle < lastTotalIdle){
        //Overflow detection. Just skip this value.
        percent = -1.0;
    }
    else{
        total = (totalUser - lastTotalUser) + (totalUserLow - lastTotalUserLow) +
            (totalSys - lastTotalSys);
        percent = total;
        total += (totalIdle - lastTotalIdle);
        percent /= total;
        percent *= 100;
    }

    lastTotalUser = totalUser;
    lastTotalUserLow = totalUserLow;
    lastTotalSys = totalSys;
    lastTotalIdle = totalIdle;

    return percent;
}

void Performance_Monitoring::run(){
    monitor();
}