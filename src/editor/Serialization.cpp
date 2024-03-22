//
// Created by wouter on 21-3-2024.
//
#include <iostream>

#ifdef _WIN32
#include <windows.h>
#elif __linux__
#include <gtk/gtk.h>
#elif __APPLE__
#include <Cocoa/Cocoa.h>
#endif

#include "Serialization.h"
#include "BoidSpawners.h"


void serialization::SaveWorldToFile(const World& world) {
    auto file_name = GetFileNameThroughFileDialog();
    if (file_name.empty()) {
        std::cerr << "Error: Empty file name! " << std::endl;
        return;
    }
    std::ofstream file(file_name);
    if (!file.is_open()) {
        std::cerr << "Error: Unable to open file for writing: " << file_name << std::endl;
        return;
    }

    // Write the width and height
    file << "Size: " << world.width << " , " << world.height << "\n";

    // Write obstacle information
    for (const auto& obstacle : world.obstacles) {
        file << obstacle->ToString() << "\n";
    }

    // Write terrain information
    for (const auto& terrain : world.terrains) {
        file << terrain->ToString() << "\n";
    }

    // Write competition boid spawner information
    for (const auto& spawner : world.competition_boid_spawners) {
        file << spawner->ToString() << "\n";
    }

    file.close();
    std::cout << "World saved succesfully to: " << file_name << std::endl;
}

std::optional<World> serialization::LoadWorldFromFile(const std::string& filename) {

    float width, height;
    std::vector<std::shared_ptr<Obstacle>> obstacles;
    std::vector<std::shared_ptr<Terrain>> terrains;
    std::vector<std::shared_ptr<CompBoidSpawner>> competition_spawners;

    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Unable to open file for reading: " << filename << std::endl;
        return std::nullopt;
    }

    std::string line;

    // Read width
    if (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string prefix;
        char delimiter;
        ss >> prefix >> width >> delimiter >> height;
        if (prefix != "Size:") {
            std::cerr << "Error: Invalid format." << std::endl;
            return std::nullopt;
        }
    } else {
        std::cerr << "Error: Missing Line" << std::endl;
        return std::nullopt;
    }

    while (std::getline(file, line)) {
        // Check the tag at the beginning of the line to determine the type of object
        if (line.compare(0, 12, "LineObstacle") == 0) {
            auto obstacle = LineObstacle::fromString(line);
            if (obstacle != nullptr) {
                obstacles.push_back(obstacle);
            }
        } else if (line.compare(0, 14, "CircleObstacle") == 0) {
            auto obstacle = CircleObstacle::FromString(line);
            if (obstacle != nullptr) {
                obstacles.push_back(obstacle);
            }
        } else if (line.compare(0, 7, "Terrain") == 0) {
            auto terrain = Terrain::FromString(line);
            if (terrain != nullptr) {
                terrains.push_back(terrain);
            }
        } else if (line.compare(0, 23, "CompBoidCircularSpawner") == 0) {
            auto spawner = CompBoidCircularSpawner::FromString(line);
            if (spawner != nullptr) {
                competition_spawners.push_back(spawner);
            }
        } else {
            // Unknown type, handle accordingly (ignore or report error)
            std::cerr << "Error: Unknown object type in line: " << line << std::endl;
            return std::nullopt;
        }
    }

    file.close();
    World world;
    world.obstacles = obstacles;
    world.terrains = terrains;
    world.competition_boid_spawners = competition_spawners;
    world.height = height;
    world.width = width;

    return world;
}

std::string serialization::GetFileNameThroughFileDialog() {
#ifdef _WIN32
    char filename[MAX_PATH];
    OPENFILENAME ofn;
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.lpstrFilter = "Data Files (*.dat)\0*.dat\0All Files (*.*)\0*.*\0";
    ofn.lpstrFile = filename;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_EXPLORER | OFN_OVERWRITEPROMPT;
    if (GetSaveFileName(&ofn) == TRUE) {
        return filename;
    } else {
        return "";
    }
#elif __linux__
    GtkWidget *dialog;
    dialog = gtk_file_chooser_dialog_new("Save File",
                                         NULL,
                                         GTK_FILE_CHOOSER_ACTION_SAVE,
                                         "Cancel", GTK_RESPONSE_CANCEL,
                                         "Save", GTK_RESPONSE_ACCEPT,
                                         NULL);
    gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(dialog), TRUE);
    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        char *filename;
        filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        std::string filepath(filename);
        g_free(filename);
        gtk_widget_destroy(dialog);
        return filepath;
    } else {
        gtk_widget_destroy(dialog);
        return "";
    }
#elif __APPLE__
    NSSavePanel *panel = [NSSavePanel savePanel];
    [panel setAllowedFileTypes:@[@"dat"]];
    if ([panel runModal] == NSModalResponseOK) {
        return std::string([[panel URL] path]);
    } else {
        return "";
    }
#endif
}
