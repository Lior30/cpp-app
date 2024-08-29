# cpp_app

cpp_app is a C++ application that retrieves data from a public API in JSON format and displays it to the user using the ImGui library.
This program demonstrates how to integrate HTTP requests and JSON parsing in a C++ application and visualize the data.


 ## Features

- Fetches data from a public API.
- Parses JSON responses using the `nlohmann::json` library.
- Displays the data in a graphical user interface built with ImGui.
- Provides user-friendly controls for interacting with the data.


## Installation And Prerequisites

To run this application, you need to have the following installed:

- **Compiler**: Make sure you have a C++ compiler installed (GCC, Clang, or MSVC).
- **IDE**: The project is set up for Visual Studio, so it's recommended to use it. You can download Visual Studio [here](https://visualstudio.microsoft.com/).
- **Dependencies**:
  - [nlohmann/json](https://github.com/nlohmann/json): A modern C++ library for JSON parsing.
  - [ImGui](https://github.com/ocornut/imgui): A bloat-free graphical user interface library for C++.
  - [cpp-httplib](https://github.com/yhirose/cpp-httplib): A single-file C++ header-only HTTP/HTTPS server and client library.


### Cloning the Repository

To clone the repository to your local machine, run the following commands:

```bash
git clone https://github.com/Lior30/cpp_app.git
cd cpp_app

