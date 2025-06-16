/*
 * This program estimates the memory bandwidth of a system by continuosly reading from memory.
 * Developed by Juan Jose Olivera
*/

#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <string>
#include <cstdint>

#define MAX(a,b) ((a > b) ? a : b)
#define MIN(a,b) ((a < b) ? a : b)

class Timer
{
public:
    Timer() : Timer("no name") {}

    Timer(const std::string& name)
    {
        m_StartTimePoint = std::chrono::high_resolution_clock::now();
        m_IsRunning = true;
        m_Name = name;
    }

    ~Timer()
    {
        if (m_IsRunning)
            Stop();
    }

    std::chrono::nanoseconds Stop()
    {
        auto endTimePoint = std::chrono::high_resolution_clock::now();
        if (!m_IsRunning)
        {
            std::cout << "Timer is already stopped!" << std::endl;
            return std::chrono::nanoseconds(0);
        }
        m_IsRunning = false;

        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(
            endTimePoint - m_StartTimePoint
        );
        // std::cout << "(" << m_Name << ") total Duration: " << duration.count() << " nanoseconds" << std::endl;
        // std::cout << "(" << m_Name << ") total Duration: " << duration.count() / 1000000.0 << " milliseconds" << std::endl;
        return duration;
    }
private:
    std::chrono::time_point<std::chrono::high_resolution_clock> m_StartTimePoint;
    std::string m_Name;
    bool m_IsRunning = false;
};

int main() {
    // Make memory blob
    // std::ifstream myFile("764M-file.mp4", std::ios::binary | std::ios::ate);
    // std::streamsize size = myFile.tellg();
    // myFile.seekg(0, std::ios::beg);
    // std::vector<char> buffer(size);
    // if (!myFile.read(buffer.data(), size)) {
    //     std::cerr << "Error reading file!" << std::endl;
    //     return 1;
    // }
    // std::cout << "File read successfully and it takes " << buffer.size() << " bytes" << std::endl;
    
    int memSizeKb = 2;
    const int HOPS = 20;
    int sizeKbs[HOPS];
    double bwGBs[HOPS];
    int64_t latencyNs[HOPS];
    for (int hop = 0; hop < HOPS; hop++) {
        std::cout << "Using " << memSizeKb << "KBs" << std::endl;
        sizeKbs[hop] = memSizeKb;
        
        const ssize_t bufferSize = memSizeKb * 1024ULL; // 1024 MB1GB

        std::vector<uint64_t> buffer(bufferSize / sizeof(uint64_t));
        for (ssize_t i = 0; i < buffer.size(); i++) {
            buffer[i] = static_cast<uint64_t>(i);
        }
    
        // Calculate bandwidth by doing a simple XOR operation over all bytes, thus making all data to be transferred
        // (This of course is limited to the BW of a single thread/core)
        // volatile uint64_t  agg = 0;
        volatile uint64_t  agg0 = 0, agg1 = 0, agg2 = 0, agg3 = 0;
    
        double avgBwGBs = 0;
        int64_t minLatencyNs = INT64_MAX;
        for(int i = 0; i < 10; i++) {
            Timer timer("Vectorized Read");
            for (int j = 0; j < 1000000000/buffer.size(); j++)
            {
                for (ssize_t i = 0; i < buffer.size(); i+= 1) { // can be 1 or 64 for byte offset
                    // agg ^= buffer[i];
                    volatile double* _b = (volatile double *) buffer.data();
                    _b[i];
                }
            }
            auto duration_ns = timer.Stop();
            int64_t nanoseconds = duration_ns.count();
            double seconds;
            if (nanoseconds > 0) {
                seconds = nanoseconds / 1e9;
            } else {
                seconds = 0.5 / 1e9;
            }
            size_t bytesRead = bufferSize;
    
            auto bandwidth = 1000000000/buffer.size() * (bytesRead * 8.0) / seconds; // bits per 
            double bwGBs = bandwidth / 8e9;
            std::cout << "Bandwidth: " << bandwidth / 1e9 << " Gbps; " << nanoseconds << "ns" << std::endl;
            std::cout << "           " << bwGBs << " GB/s" << std::endl;
            avgBwGBs = MAX(bwGBs, avgBwGBs);
            minLatencyNs = MIN(minLatencyNs, nanoseconds);
        }
        bwGBs[hop] = avgBwGBs;
        // latencyNs[hop] = minLatencyNs;

        memSizeKb *= 2;
    }



    std::ofstream output("pbw.csv");
    output << "Cache Sizes\n";
    output << "Data Size (KBs),GB/s,Test\n";
    // output << "Data Size (KBs),Latency (ns),Test\n";
    for (int h = 0; h < HOPS; h++){
        output << sizeKbs[h] << "," << bwGBs[h] << ",1 Thread\n"; 
    }
    output.close();
    // // print agg as binary string
    // std::cout << "agg: " << agg0;
    // // for (size_t i = 0; i < sizeof(agg) * 8; ++i) {
    // //     std::cout << ((agg >> (sizeof(agg) * 8 - 1 - i)) & 1);
    // // }
    // std::cout << std::endl;
    return 0;
}
