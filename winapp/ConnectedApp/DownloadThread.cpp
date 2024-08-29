#include "DownloadThread.h"
#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"
#include "nlohmann/json.hpp"
#include <iostream>
#include <mutex>

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Punch, type, setup, punchline, id)


void DownloadThread::operator()(CommonObjects& common)
{
    const std::string host = "official-joke-api.appspot.com"; // Host without protocol
    const std::string path = "/jokes/random/400"; // Path for the endpoint

    httplib::SSLClient cli(host.c_str()); // Use SSLClient for HTTPS

    auto res = cli.Get(path.c_str()); // Perform GET request to the path

    if (res && res->status == 200) {
        try {
            auto json_result = nlohmann::json::parse(res->body); // Parse JSON response

            // Lock the mutex
            std::lock_guard<std::mutex> lock(common.mtx);


            if (json_result.is_array() && !json_result.empty()) {
                common.punches.clear(); // Clear any existing data in common.punches

                // Parse each item in the JSON array into the Punch structure and add to common.punches
                for (const auto& item : json_result) {
                    Punch punch = item.get<Punch>();
                    common.punches.push_back(punch);
                }

                // Set data_ready to true if we have successfully added punches to the list.
                if (!common.punches.empty()) {
                    common.data_ready = true;
                }
            }
        }
        catch (const std::exception& e) {
            std::cerr << "Error parsing JSON: " << e.what() << std::endl;
        }
    }
    else {
        std::cerr << "HTTP request failed with status: " << (res ? res->status : 0) << std::endl;
    }
}


void DownloadThread::SetUrl(std::string_view new_url) {
    _download_url = new_url.data();
}
