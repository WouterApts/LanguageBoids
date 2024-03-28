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
#include "SimulationData.h"


void serialization::SaveSimulationDataToFile(const SimulationData& data) {
    // Have user input save file location.
    auto file_name = GetFileNameThroughSaveDialog();
    if (file_name.empty()) {
        std::cerr << "Error: Empty file name! " << std::endl;
        return;
    }
    std::ofstream file(file_name);
    if (!file.is_open()) {
        std::cerr << "Error: Unable to open file for writing: " << file_name << std::endl;
        return;
    }

    // Write the simulation type and simulation specific config:
    file << "Simulation-Type: ";
    switch (data.type) {
        case KeySimulation:
            file << "LanguageKey \n"
                 << "a_COEFFICIENT: " << data.config->a_COEFFICIENT << '\n'
                 << "INFLUENCE_RATE: " << data.config->INFLUENCE_RATE << '\n';
            break;
        case VectorSimulation:
            file << "LanguageVector \n"
                 << "LANGUAGE_SIZE: " << data.config->LANGUAGE_SIZE << '\n'
                 << "MUTATION_RATE: " << data.config->MUTATION_RATE << '\n';
            break;
        case DominanceSimulation:
            file << "DominanceStudy \n"
                 << "RUNS_PER_DISTRIBUTION: " << data.config->RUNS_PER_DISTRIBUTION << '\n'
                 << "DISTRIBUTIONS: " << data.config->DISTRIBUTIONS << '\n'
                 << "TOTAL_BOIDS: " << data.config->TOTAL_BOIDS << '\n';
            break;
    }

    // Write the rest of the Simulation Config:
    file << "COHERENCE_FACTOR: " << data.config->COHERENCE_FACTOR << '\n'
         << "ALIGNMENT_FACTOR: " << data.config->ALIGNMENT_FACTOR << '\n'
         << "SEPARATION_FACTOR: " << data.config->SEPARATION_FACTOR << '\n'
         << "AVOIDANCE_FACTOR: " << data.config->AVOIDANCE_FACTOR << '\n'
         << "MAX_SPEED: " << data.config->MAX_SPEED << '\n'
         << "MIN_SPEED: " << data.config->MIN_SPEED << '\n'
         << "PERCEPTION_RADIUS: " << data.config->PERCEPTION_RADIUS << '\n'
         << "INTERACTION_RADIUS: " << data.config->INTERACTION_RADIUS << '\n'
         << "SEPARATION_RADIUS: " << data.config->SEPARATION_RADIUS << '\n'
         << "BOID_COLLISION_RADIUS: " << data.config->BOID_COLLISION_RADIUS << '\n'
         << "RESTITUTION_COEFFICIENT: " << data.config->RESTITUTION_COEFFICIENT << '\n'
         << "LANGUAGE_LOG_INTERVAL: " << data.config->LANGUAGE_LOG_INTERVAL << '\n'
         << "POSITION_LOG_INTERVAL: " << data.config->POSITION_LOG_INTERVAL << '\n';

    // Write the world width and height
    file << "Size: " << data.world.width << " , " << data.world.height << "\n";

    // Write obstacle information
    for (const auto& obstacle : data.world.obstacles) {
        file << obstacle->ToString() << "\n";
    }

    // Write terrain information
    for (const auto& terrain : data.world.terrains) {
        file << terrain->ToString() << "\n";
    }

    // Write boid spawner information
    for (const auto& spawner : data.boid_spawners) {
        file << spawner->ToString() << "\n";
    }

    file.close();
    std::cout << "Simulation Data saved succesfully to: " << file_name << std::endl;
}

//TODO: FIX THIS
std::optional<World> serialization::LoadSimulationDataFromFile(const std::string& filename) {

    float width, height;
    std::vector<std::shared_ptr<Obstacle>> obstacles;
    std::vector<std::shared_ptr<Terrain>> terrains;
    std::vector<std::shared_ptr<KeyBoidSpawner>> competition_spawners;

    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Unable to open file for reading: " << filename << std::endl;
        return std::nullopt;
    }

    std::string line;

    // Parse Size
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

    // Parse Obstacles, Terrain and Spawners
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
            auto spawner = KeyBoidCircularSpawner::FromString(line);
            if (spawner != nullptr) {
                competition_spawners.push_back(spawner);
            }
        } else if (line.compare(0, 26, "CompBoidRectangularSpawner") == 0) {
            auto spawner = KeyBoidRectangularSpawner::FromString(line);
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

    // Construct simulation_data from parsed data;
    return std::nullopt;
}

std::string serialization::GetFileNameThroughSaveDialog() {
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
        std::string filepath(filename);
        // Ensure the file path has ".dat" extension
        if (filepath.substr(filepath.length() - 4) != ".dat") {
            filepath += ".dat";
        }
        return filepath;
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
        // Ensure the file path has ".dat" extension
        if (filepath.substr(filepath.length() - 4) != ".dat") {
            filepath += ".dat";
        }
        return filepath;
    } else {
        gtk_widget_destroy(dialog);
        return "";
    }
#elif __APPLE__
    NSSavePanel *panel = [NSSavePanel savePanel];
    [panel setAllowedFileTypes:@[@"dat"]];
    [panel setNameFieldStringValue:@"filename.dat"]; // Set default filename
    if ([panel runModal] == NSModalResponseOK) {
        std::string filepath([[panel URL] path]);
        // Ensure the file path has ".dat" extension
        if (filepath.substr(filepath.length() - 4) != ".dat") {
            filepath += ".dat";
        }
        return filepath;
    } else {
        return "";
    }
#endif
}

std::string serialization::GetFileNameThroughLoadDialog() {
#ifdef _WIN32
    OPENFILENAMEA ofn;
    CHAR fileName[MAX_PATH];
    ZeroMemory(&ofn, sizeof(ofn));

    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = nullptr;
    ofn.lpstrFile = fileName;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrFilter = "DAT Files (*.dat)\0*.dat\0All Files\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_EXPLORER;

    GetOpenFileNameA(&ofn);
    return fileName;

#elif __linux__
    GtkWidget *dialog;
    dialog = gtk_file_chooser_dialog_new("Open File",
                                         NULL,
                                         GTK_FILE_CHOOSER_ACTION_OPEN,
                                         "Cancel", GTK_RESPONSE_CANCEL,
                                         "Open", GTK_RESPONSE_ACCEPT,
                                         NULL);
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
    NSOpenPanel *panel = [NSOpenPanel openPanel];
    [panel setAllowedFileTypes:@[@"dat"]];
    [panel setCanChooseFiles:YES];
    [panel setCanChooseDirectories:NO];
    if ([panel runModal] == NSModalResponseOK) {
        return std::string([[panel URL] path]);
    } else {
        return "";
    }
#endif
}
