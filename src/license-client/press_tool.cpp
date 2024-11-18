#include "verify.h"
#include <iostream>
#include <string>
#include <vector>
#include <atomic>
#include <execution>
#include <chrono>
#include <numeric>

void stress_test_verify(const std::string& ip, int port, int period, size_t num_requests) 
{
    std::vector<int> requests(num_requests); 
    std::iota(requests.begin(), requests.end(), 0); 

    std::atomic<size_t> success_count{0};
    std::atomic<size_t> failure_count{0};

    auto start_time = std::chrono::steady_clock::now();

    std::for_each(std::execution::par, requests.begin(), requests.end(),
                  [&](int request_id) {
                      std::cout << "Sending request ID: " << request_id << "\n";
                      if (lic::verify_press(ip, port, period)) {
                          success_count++;
                      } else {
                          failure_count++;
                      }
                  });

    auto end_time = std::chrono::steady_clock::now();

    std::cout << "Total requests: " << num_requests << "\n";
    std::cout << "Success: " << success_count << "\n";
    std::cout << "Failure: " << failure_count << "\n";
    std::cout << "Total time taken: "
              << std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count()
              << " ms\n";
}

int main(int argc, char** argv) 
{
    size_t num_requests = 1;

    if (argc > 1) {
        try {
            num_requests = std::stoul(argv[1]);
            if (num_requests == 0) {
                throw std::invalid_argument("Number of requests must be greater than 0");
            }
        } catch (const std::exception& e) {
            std::cerr << "Invalid input for number of requests: " << argv[1] << '\n';
            std::cerr << "Using default value: 10\n";
            num_requests = 10;
        }
    }

    const std::string server_ip = "127.0.0.1"; // 替换为目标服务器的 IP
    const int server_port = 8442;
    const int verify_period = 30;

    // 启动压力测试
    stress_test_verify(server_ip, server_port, verify_period, num_requests);

    return 0;
}
