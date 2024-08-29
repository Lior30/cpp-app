#include "DrawThread.h"
#include "GuiMain.h"
#include "../../shared/ImGuiSrc/imgui.h"
#include <list>
#include <vector>
#include <algorithm>
#include <random>
#include <fstream>
#include <filesystem>
#include <string>
#include <iostream>
#include <mutex>

void removePunchFromFavorites(std::vector<Punch>& favorites, int recId, int punchId);        // A declaration of a function, that we will use later.
void savePunchToPC(const Punch& punch, std::string directoryPath);
void clearFolder(const std::string& folderPath);
void loadFavoritesFromDirectory(std::vector<Punch>& favorites);
void initializeFavorites(CommonObjects* common);

// Function to shuffle a vector
template <typename T>
void shuffleVector(std::vector<T>& vec) {           // Receiving the vector. A function to shuffle the vector of the punches. 
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(vec.begin(), vec.end(), g);
}

void DrawAppWindow(void* common_ptr)
{
    auto common = (CommonObjects*)common_ptr;

    std::lock_guard<std::mutex> lock(common->mtx);                      // Locking the mutex.

    initializeFavorites(common);

    ImVec2 screenSize = ImGui::GetIO().DisplaySize;                     // Retrieving screen size.
    // Calculating sizes and positions.
    ImVec2 favoritesSize(screenSize.x, screenSize.y * 0.25f);           // 25% height for Favorites window.
    ImVec2 jokesListSize(screenSize.x, screenSize.y * 0.75f);           // 75% height for Jokes List window.

    ImVec2 favoritesPos(0, 0); // Position of Favorites window
    ImVec2 jokesListPos(0, favoritesSize.y); // Position of Jokes List window

    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.12f, 0.12f, 0.12f, 1.0f));        // Pushing window style color (background and border).
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(1.0f, 1.0f, 0.0f, 1.0f));

    // Changing tab colors to white.
    ImGui::PushStyleColor(ImGuiCol_Tab, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));                // Tab background.
    ImGui::PushStyleColor(ImGuiCol_TabHovered, ImVec4(0.8f, 0.8f, 0.8f, 1.0f));         // Hovered tab background.
    ImGui::PushStyleColor(ImGuiCol_TabActive, ImVec4(0.6f, 0.6f, 0.6f, 1.0f));          // Active tab background.
    ImGui::PushStyleColor(ImGuiCol_TabUnfocused, ImVec4(1.0f, 1.0f, 1.0f, 0.7f));       // Unfocused tab background.
    ImGui::PushStyleColor(ImGuiCol_TabUnfocusedActive, ImVec4(0.8f, 0.8f, 0.8f, 1.0f)); // Unfocused active tab background.

    ImGui::PopStyleColor(5); // Reverting all five style colors.

    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
    ImGui::GetStyle().Colors[ImGuiCol_TitleBg] = ImVec4(0.68f, 0.85f, 0.9f, 1.0f);
    ImGui::SetNextWindowSize(jokesListSize);
    ImGui::SetNextWindowPos(jokesListPos);
    ImGui::Begin("Jokes List", nullptr, ImGuiWindowFlags_NoCollapse);
    ImGui::Text("");
    ImGui::PopStyleColor();


    static char buff[200] = "";                                            // A buffer for input text.
    static char search_keyword[100] = "";
    static std::string selected_type = "";                                 // Storing the type that the user selected.

    ImGui::SetNextItemWidth(150.0f);                                       // Set the width of the input text field.

    // Changing background colors of the input field.
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.98f, 0.98f, 0.66f, 1.0f));         // Light yellow color.

    // Setting text color for the input field text and placeholder.
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));              // Text color (black).
    ImGui::PushStyleColor(ImGuiCol_TextDisabled, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));

    // Input text with hint.
    if (ImGui::InputTextWithHint("##type_input", "Select Type", buff, sizeof(buff), ImGuiInputTextFlags_EnterReturnsTrue)) {
        selected_type = buff;                                              // Update the selected type when Enter is pressed.
    }

    ImGui::PopStyleColor(3);                                               // Revert all three style color changes

    ImGui::SameLine();
    // Starting to style the buttons line.
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 1.0f, 0.0f, 1.0f));          // Button color
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.8f, 0.8f, 0.0f, 1.0f));   // Hovered color
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.97f, 0.96f, 0.70f, 1.0f)); // Active color
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));            // Black text color
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0f);                          // Rounded corners
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5, 5));                   // Padding inside buttons

    if (ImGui::Button("Set", ImVec2(60, 25))) {
        selected_type = buff;                                              // Updating the chart to show only elements of the selected type.
    }

    ImGui::SameLine();
    if (ImGui::Button("Reset", ImVec2(60, 25))) {
        selected_type = "";
        memset(buff, 0, sizeof(buff));
        ImGui::SetNextItemWidth(150.0f);                                   // Ensuring the width is set again (if needed) for the input text.
        ImGui::InputText("Select Type", buff, sizeof(buff));
    }

    ImGui::SameLine();
    if (ImGui::Button("Shuffle", ImVec2(60, 25))) {
        shuffleVector(common->punches);
    }

    ImGui::PopStyleVar(2);                                                 // Revert frame rounding and padding
    ImGui::PopStyleColor(4);                                               // Revert button colors

    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));   // White background.
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.8f, 0.8f, 0.8f, 1.0f));    // Light gray border.
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));      // Black text color.

    
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));      // White text color.
    ImGui::Text("Search:");
    ImGui::PopStyleColor();

    ImGui::InputText("##search", search_keyword, sizeof(search_keyword));      // Search input field.

    ImGui::PopStyleColor(3);                                                   // Reverting color changes.

    ImGui::SameLine();


    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.0f, 0.0f, 1.0f));       // Red button color.
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));// Hovered color.
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.6f, 0.0f, 0.0f, 1.0f)); // Active color.

    if (ImGui::Button("Clear Punches Folder", ImVec2(200, 25))) {
        clearFolder("punchesFile");
    }

    ImGui::Text("");                                                              // An empty line.

    ImGui::PopStyleColor(3);                                                      // restoring original button colors.

    // Converting search keyword and selected type to lowercase for case-insensitive search.
    std::string keyword(search_keyword);
    std::transform(keyword.begin(), keyword.end(), keyword.begin(), ::tolower);
    std::string type_lower(selected_type);
    std::transform(type_lower.begin(), type_lower.end(), type_lower.begin(), ::tolower);


    static bool show_data = false;      // When new data is detected, it sets a flag to show the data and then resets the readiness indicator.
    if (common->data_ready) {
        show_data = true;
        common->data_ready = false;
    }

    if (show_data)
    {
        // Begin a new table with 5 columns
        if (ImGui::BeginTable("punches", 5, ImGuiTableFlags_RowBg))                      
        {
            ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed, 100.0f);   // Fixed width for 'type'.
            ImGui::TableSetupColumn("Setup", ImGuiTableColumnFlags_WidthStretch);        // Stretchable width for 'setup'...
            ImGui::TableSetupColumn("Punchline", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("Id", ImGuiTableColumnFlags_WidthFixed, 50.0f);
            ImGui::TableSetupColumn("Action", ImGuiTableColumnFlags_WidthFixed, 100.0f);

            ImGui::TableHeadersRow();          // Add headers row.

            for (auto& rec : common->punches)  // Iterating over the punches vector, and display only those that match the selected type.
            {

                std::string type_check = rec.type;
                std::string setup_check = rec.setup;
                std::string punchline_check = rec.punchline;
                std::transform(type_check.begin(), type_check.end(), type_check.begin(), ::tolower);
                std::transform(setup_check.begin(), setup_check.end(), setup_check.begin(), ::tolower);
                std::transform(punchline_check.begin(), punchline_check.end(), punchline_check.begin(), ::tolower);

                // Checking if the keyword and type match any of the joke fields
                bool keyword_match = keyword.empty() ||
                    type_check.find(keyword) != std::string::npos ||
                    setup_check.find(keyword) != std::string::npos ||
                    punchline_check.find(keyword) != std::string::npos;

                bool type_match = type_lower.empty() || type_check.find(type_lower) != std::string::npos;

                if ((keyword_match && type_match))  // Show all thr punches if no type is selected or if it matches.
                {
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    if (rec.type == "programming")
                        ImGui::TextColored(ImVec4(0.678f, 0.847f, 0.902f, 1.0f), "%s", rec.type.c_str()); // Blue.
                    else if (rec.type == "general")
                        ImGui::TextColored(ImVec4(0.565f, 0.929f, 0.565f, 1.0f), "%s", rec.type.c_str()); // Green.
                    else if (rec.type == "knock-knock")
                        ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.8f, 1.0f), "%s", rec.type.c_str());       // Red.
                    else // In case of 'dad' type - the color is defult (white).
                        ImGui::TextWrapped("%s", rec.type.c_str());
                    ImGui::TableSetColumnIndex(1);
                    ImGui::TextWrapped("%s", rec.setup.c_str());
                    ImGui::TableSetColumnIndex(2);
                    ImGui::TextWrapped("%s", rec.punchline.c_str());
                    ImGui::TableSetColumnIndex(3);
                    ImGui::Text("%d", rec.id);
                    ImGui::TableSetColumnIndex(4);

                    // Button and Popup
                    std::string popup_id = "OptionsPopup_" + std::to_string(rec.id);                // Creating a unique popup ID for each row.

                    // Set the style color for the button
                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5f, 0.85f, 1.0f, 1.0f));        // Light blue button background.
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.6f, 0.8f, 1.0f));  // Lighter blue hover background.
                    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.5f, 0.85f, 1.0f, 1.0f));  // Even lighter blue when button is active.
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));           // Black text color.

                    if (ImGui::Button(("Options##" + popup_id).c_str(), ImVec2(80, 25)))            // Unique button ID.
                    {
                        ImGui::OpenPopup(popup_id.c_str());                                         // Openning the popup with the unique ID.
                    }

                    ImGui::PopStyleColor(4);                                                        // Restorring original style colors.

                    // Begin Popup
                    if (ImGui::BeginPopup(popup_id.c_str()))                                        // Ensurring the popup ID matches.
                    {

                        if (ImGui::MenuItem("Save On PC"))
                        {
                            savePunchToPC(rec, "punchesFile/");

                        }

                        // Checking if the punch is already in favorites.
                        bool alreadyInFavorites = std::any_of(common->favorites.begin(), common->favorites.end(),
                            [&](const Punch& p) { return p.id == rec.id; });

                        // Display "Save To Favorites" only if it's not already in the favorites list
                        if (!alreadyInFavorites)
                        {
                            if (ImGui::MenuItem("Save To Favorites"))
                            {
                                common->favorites.push_back(rec); // Add the current punch to favorites directly

                                savePunchToPC(rec, "favorites/");
                            }
                        }
                        else
                        {
                            ImGui::MenuItem("Already in Favorites", "", false, false);  // Show as disabled or with a different message.
                        }

                        // Only show "Remove From Favorites" if the joke is in the favorites list
                        if (alreadyInFavorites)
                        {
                            if (ImGui::MenuItem("Remove From Favorites"))
                            {
                                removePunchFromFavorites(common->favorites, rec.id, rec.id);
                            }
                        }

                        ImGui::EndPopup();
                    }
                   
                }
            }

            ImGui::EndTable();
        }
    }
    ImGui::End();

    // Restore the previous window style color
    ImGui::PopStyleColor(2);

    // --------------------- SHOW FAVORITE WINDOW -----------------------------
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(1.0f, 1.0f, 0.0f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));

    ImGui::SetNextWindowSize(favoritesSize);
    ImGui::SetNextWindowPos(favoritesPos);
    ImGui::Begin("Favorites", nullptr, ImGuiWindowFlags_NoCollapse);   // Passing a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked).

    ImGui::PopStyleColor(2);  // Revert style color change.

    if (ImGui::BeginTable("punches", 5, ImGuiTableFlags_RowBg))
    {
        ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed, 100.0f);
        ImGui::TableSetupColumn("Setup", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Punchline", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Id", ImGuiTableColumnFlags_WidthFixed, 50.0f);
        ImGui::TableSetupColumn("Action", ImGuiTableColumnFlags_WidthFixed, 100.0f);

        ImGui::TableHeadersRow();  // Add headers row.

        for (auto& rec : common->favorites)  // Iterate over the favorites vector, and display the punches that in it.
        {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", rec.type.c_str());
            ImGui::TableSetColumnIndex(1);
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", rec.setup.c_str());
            ImGui::TableSetColumnIndex(2);
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", rec.punchline.c_str());
            ImGui::TableSetColumnIndex(3);
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%d", rec.id);
            ImGui::TableSetColumnIndex(4);

            // Button and Popup
            std::string popup_id = "OptionsPopup_" + std::to_string(rec.id);      // Creating a unique popup ID for each row.


            // Set the style color for the button
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5f, 0.85f, 1.0f, 1.0f));        // Light blue button background.
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.6f, 0.8f, 1.0f));  // Lighter blue hover background.
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.5f, 0.85f, 1.0f, 1.0f));  // Even lighter blue when button is active.
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));           // Black text color.

            if (ImGui::Button(("Options##" + popup_id).c_str(), ImVec2(80, 25)))  // Unique button ID.
            {
                ImGui::OpenPopup(popup_id.c_str());  // Open the popup with the unique ID.
            }

            ImGui::PopStyleColor(4);                 // Restore original style colors

            // Begin Popup
            if (ImGui::BeginPopup(popup_id.c_str())) // Ensure the popup ID matches
            {
                if (ImGui::MenuItem("Remove From Favorites"))
                {
                    removePunchFromFavorites(common->favorites, rec.id, rec.id);
                }

                ImGui::EndPopup();
            }
        }


        ImGui::EndTable();

    }
    ImGui::End();
}

void DrawThread::operator()(CommonObjects& common)
{
    GuiMain(DrawAppWindow, &common);
    common.exit_flag = true;
}

void removePunchFromFavorites(std::vector<Punch>& favorites, int recId, int punchId) {  // Function to remove a selected punch from favorites.
    favorites.erase(std::remove_if(favorites.begin(), favorites.end(),                  // Removing the element from the vector.
        [punchId](const Punch& p) { return p.id == punchId; }),
        favorites.end());

    std::string filePath = "favorites/punch_" + std::to_string(punchId) + ".txt";             // Deleting the corresponding file from the directory.
    if (std::filesystem::exists(filePath)) {
        std::filesystem::remove(filePath);                                              // Deleting the file.
    }
}

void savePunchToPC(const Punch& punch, std::string directoryPath) {        // Function to save punch to PC.
    
    if (!std::filesystem::exists(directoryPath)) {                  // Check if the directory exists, if not create it
        std::filesystem::create_directory(directoryPath);
    }
    std::string filePath = directoryPath + "/punch_" + std::to_string(punch.id) + ".txt";

    std::ofstream outFile(filePath);
    if (outFile.is_open()) {
        outFile << "Type: " << punch.type << "\n";
        outFile << "Setup: " << punch.setup << "\n";
        outFile << "Punchline: " << punch.punchline << "\n";
        outFile << "ID: " << punch.id << "\n";
        outFile.close();
        //ImGui::Text("Saved to %s", filePath.c_str());
    }
    //else {
    //    ImGui::Text("Failed to save to file.");
    //}
}

void clearFolder(const std::string& folderPath) {   // Function to clear a folder from its files.       
    namespace fs = std::filesystem;

    if (!std::filesystem::exists(folderPath)) {
        return;
    }

    for (const auto& entry : fs::directory_iterator(folderPath)) {
        fs::remove(entry.path());
    }
}


void loadFavoritesFromDirectory(std::vector<Punch>& favorites) {    // Function to load punches from files in the directory.
    
    std::string directoryPath = "favorites/";
    if (!std::filesystem::exists(directoryPath)) {                  // Check if the directory exists. If not - create it.
        std::filesystem::create_directory(directoryPath);
    }
    for (const auto& entry : std::filesystem::directory_iterator(directoryPath)) {
        Punch punch;
        std::ifstream inFile(entry.path());

        if (inFile.is_open()) { // Iterating over each file. Copying from it the values of the struct: punch.
            std::string line;
            while (std::getline(inFile, line)) {
                if (line.find("Type: ") == 0) {
                    punch.type = line.substr(6);
                }
                else if (line.find("Setup: ") == 0) {
                    punch.setup = line.substr(7);
                }
                else if (line.find("Punchline: ") == 0) {
                    punch.punchline = line.substr(11);
                }
                else if (line.find("ID: ") == 0) {
                    punch.id = std::stoi(line.substr(4));
                }
            }
            favorites.push_back(punch);
            inFile.close();
        }
    }
}

void initializeFavorites(CommonObjects* common) {       // Function to be called in the start of the program.
    common->favorites.clear();                          // Clearing the favorites vector, because in the beginning of the program it already reloads the favorites from the directory again.


    loadFavoritesFromDirectory(common->favorites);
}
