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

    std::chrono::microseconds Stop()
    {
        auto endTimePoint = std::chrono::high_resolution_clock::now();
        if (!m_IsRunning)
        {
            std::cout << "Timer is already stopped!" << std::endl;
            return std::chrono::microseconds(0);
        }
        m_IsRunning = false;

        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
            endTimePoint - m_StartTimePoint
        );
        std::cout << "(" << m_Name << ") total Duration: " << duration.count() << " microseconds" << std::endl;
        std::cout << "(" << m_Name << ") total Duration: " << duration.count() / 1000.0 << " milliseconds" << std::endl;
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
    
    const size_t bufferSize = 764 * 1024 * 1024; // 764 MB
    std::vector<uint64_t> buffer(bufferSize / sizeof(uint64_t));
    for (size_t i = 0; i < buffer.size(); ++i) {
        buffer[i] = static_cast<uint64_t>(i);
    }

    // Calculate bandwidth by doing a simple XOR operation over all bytes, thus making all data to be transferred
    // (This of course is limited to the BW of a single thread/core)
    // volatile uint64_t  agg = 0;
    volatile uint64_t agg0 = 0, agg1 = 0, agg2 = 0, agg3 = 0;
    size_t i;
    {
        Timer timer("Vectorized Read");
        for (i = 0; i < buffer.size(); i+= 1) { // can be 1 or 64 for byte offset
            // agg ^= buffer[i];
            agg0 ^= buffer[i];
            // agg1 ^= buffer[i + 1];
            // agg2 ^= buffer[i + 2];
            // agg3 ^= buffer[i + 3];
        }
        // agg0 = agg0 ^ agg1 ^ agg2 ^ agg3;
        auto duration_us = timer.Stop();
        double seconds = duration_us.count() / 1e6;
        size_t bytesRead = bufferSize;

        auto bandwidth = (bytesRead * 8.0) / seconds; // bits per second
        std::cout << "Bandwidth: " << bandwidth / 1e9 << " Gbps" << std::endl;
        std::cout << "           " << bandwidth / 8e9 << " GB/s" << std::endl;

    }
    // print agg as binary string
    std::cout << "agg: " << agg0;
    // for (size_t i = 0; i < sizeof(agg) * 8; ++i) {
    //     std::cout << ((agg >> (sizeof(agg) * 8 - 1 - i)) & 1);
    // }
    std::cout << std::endl;
    return 0;
}