#include <atomic>

class Performance_Monitoring{
    private:
        std::atomic<bool> stop_thread_flag;
        clock_t lastCPU, lastSysCPU, lastUserCPU;
        int numProcessors;
        unsigned long long lastTotalUser, lastTotalUserLow, lastTotalSys, lastTotalIdle;
        
        void monitor();

        // get memory usage
        int parse_line(char* line);
        int get_mem_data(const char* path, const char* line_name);

        void init();
        double get_process_cpu();
        double get_cpu();

    public:
        Performance_Monitoring(std::atomic<bool>& stop_thread_flag);
        void run();
};