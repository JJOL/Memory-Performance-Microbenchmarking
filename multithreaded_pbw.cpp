/*
 * This program estimates the memory bandwidth of a system using multiple threads.
 * Developed by Daniel Lemire
 * URL: https://github.com/lemire/Code-used-on-Daniel-Lemire-s-blog/blob/master/2024/01/13/bandwidth.cpp
 * Companion blog post: https://lemire.me/blog/2024/01/13/estimating-your-memory-bandwidth/
*/

#include <chrono>
#include <thread>
#include <vector>
#include <cstdint>
#include <iostream>
#include <fstream>

volatile size_t g_sum = 0;

template <bool prefetch>
__attribute__ ((noinline))
uint64_t sum(const uint8_t *data, size_t start, size_t len, size_t skip = 1) {
    uint64_t sum = 0;
    for (size_t i = start; i < len; i+= skip) {
        sum += data[i];
        if(prefetch) { __builtin_prefetch(&data[i + 4096], 0, 3); }
    }
    g_sum += sum;
    return sum;
}
template <bool prefetch>
double estimate_bandwidth(size_t threads_count, const uint8_t* data, size_t data_volume) {
    std::vector<std::thread> threads;
    threads.reserve(threads_count);
    size_t segment_length = data_volume / threads_count;
    size_t cache_line = 64;
    std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
    if(threads_count == 1) {
        sum<prefetch>(data, 0, segment_length, cache_line);
    } else {
        for (size_t i = 0; i < threads_count; i++) {
            threads.emplace_back(sum<prefetch>, data, segment_length*i, segment_length*(i+1), cache_line);
        }
        for (std::thread &t : threads) {
            t.join();
        }
    }
    std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
    std::chrono::duration<double, std::nano>  elapsed = end - start;
    return data_volume / elapsed.count();
}


int main(int argc, char *argv[]) {
    // open file, write in first line Multithreaded Peak Bandwidth
    // in second line write Cores,MB/s,Prefetch?
    // save file

    auto numaName = argv[1];

    std::ofstream output("mpbw.csv");
    output << "Multithread Peak Bandwidth\n";
    output << "Cores,MB/s,Prefetch?\n";


    // Create a large data volume in main memory
    const auto GB = 16; // 2GB
    size_t data_volume = GB*1024*1024*1024ULL;
    std::vector<uint8_t> data(data_volume);
    for (size_t i = 0; i < data_volume; i++) {
        data[i] = 1;
    }

    // each scenario is run N times to get the best out of N
    // we will run it with increasing number of threads from 1 up to the number of available threads
    size_t N = 3;
    const size_t MAX_THREADS = 56; //std::thread::hardware_concurrency();
    for(size_t threads = 1; threads <= MAX_THREADS; threads++) {
        // First we turn off prefetching
	if (!(threads % 2 == 0 || threads == 1)) continue;
        double bw = estimate_bandwidth<false>(threads, data.data(), data_volume);
        // best out of N
        for(size_t i = 0; i < N; i++) {
            double cbw = estimate_bandwidth<false>(threads, data.data(), data_volume);
            if(cbw > bw) {
                bw = cbw;
            }
        }
        // number of threads + bandwidth in GB/s
        printf("%lu %.1f ", threads, bw);
        output << threads << "," << bw << "," << numaName << " No Prefetch" << "\n";

        // Now we turn on prefetching
        bw = estimate_bandwidth<true>(threads, data.data(), data_volume);
        // // best out of N
        for(size_t i = 0; i < N; i++) {
            double cbw = estimate_bandwidth<true>(threads, data.data(), data_volume);
            if(cbw > bw) {
                bw = cbw;
            }
        }
        printf("%.1f\n", bw);
        output << threads << "," << bw << "," << numaName << " Prefetch" << "\n";

    }
    output.close();

    return 0;
}
