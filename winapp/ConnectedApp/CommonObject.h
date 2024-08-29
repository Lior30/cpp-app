#pragma once
#include <atomic>
#include <string>
#include <vector>
#include <mutex>

struct Punch {
    std::string type;
    std::string setup;
    std::string punchline;
    int id;
};

struct CommonObjects
{
    std::atomic_bool exit_flag = false;
    std::atomic_bool start_download = false;
    std::atomic_bool data_ready = false;
    std::string url;
    std::vector<Punch> punches;

    std::vector<Punch> favorites;

    std::mutex mtx;

};
