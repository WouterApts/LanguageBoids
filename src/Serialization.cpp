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


bool serialization::SaveSimulationDataToFile(const SimulationData& data) {
    // Have user input save file location.
    auto file_name = GetFileNameThroughSaveDialog();
    if (file_name.empty()) {
        std::cerr << "Error: Empty file name! " << std::endl;
        return false;
    }
    std::ofstream file(file_name);
    if (!file.is_open()) {
        std::cerr << "Error: Unable to open file for writing: " << file_name << std::endl;
        return false;
    }

    // Write the simulation type and simulation specific config:
    file << "Simulation-Type: ";
    switch (data.type) {
        case CompSimulation:
            file << "CompetitionSimulation \n"
                 << "a_COEFFICIENT: " << data.config->a_COEFFICIENT << '\n'
                 << "INFLUENCE_RATE: " << data.config->CONVERSION_RATE << '\n';
            break;
        case EvoSimulation:
            file << "EvolutionSimulation \n"
                 << "LANGUAGE_SIZE: " << data.config->LANGUAGE_SIZE << '\n'
                 << "MUTATION_RATE: " << data.config->MUTATION_RATE << '\n'
                 << "MINIMUM_INTERACTION_RATE: " << data.config->MIN_INTERACTION_RATE << '\n'
                 << "MINIMUM_ADOPTION_RATE: " << data.config->MIN_ADOPTION_RATE << '\n'
                 << "BOID_LIFE_STEPS: " << data.config->BOID_LIFE_STEPS << '\n'
                 << "BETA: " << data.config->BETA << '\n'
                 << "KAPPA: " << data.config->KAPPA << '\n';
            break;
        case CompStudy:
            file << "CompetitionStudy \n"
                 << "a_COEFFICIENT: " << data.config->a_COEFFICIENT << '\n'
                 << "INFLUENCE_RATE: " << data.config->CONVERSION_RATE << '\n'
                 << "TOTAL_BOIDS: " << data.config->TOTAL_BOIDS << '\n'
                 << "RUNS_PER_FRACTION: " << data.config->RUNS_PER_FRACTION << '\n'
                 << "SECONDS_PER_RUN: " << data.config->TIME_STEPS_PER_RUN << '\n'
                 << "FRACTIONS: " << data.config->FRACTIONS << '\n';
            break;
    }

    // Write the rest of the Simulation Config:
    file << "COHERENCE_FACTOR: " << data.config->COHESION_FACTOR << '\n'
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
         << "POSITION_LOG_INTERVAL: " << data.config->POSITION_LOG_INTERVAL << '\n'
         << "MULTI_THREADING_ON: " << data.config->MULTI_THREADING << '\n';

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
    return true;
}


std::optional<SimulationData> serialization::LoadSimulationDataFromFile(const std::string &filename) {

    SimulationData data = SimulationData();

    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Unable to open file for reading: " << filename << std::endl;
        return std::nullopt;
    }

    std::string line;

      // Parse Simulation Type:
    if (std::getline(file, line)) {
        std::string str_type;
        std::stringstream ss(line);
        std::string prefix;
        ss >> prefix >> str_type;
        if (str_type == "CompetitionSimulation") {
            data.type = CompSimulation;
        } else if (str_type == "EvolutionSimulation") {
            data.type = EvoSimulation;
        } else if (str_type == "CompetitionStudy") {
            data.type = CompStudy;
        } else {
            std::cerr << "Error: Unknown simulation type. \nUnable to load from file: " << filename << std::endl;
            return std::nullopt;
        }
    }

    // Parse Configuration
    int n_max_parameters = 100;
    for (int i = 0; i < n_max_parameters; ++i) {
        std::getline(file, line);
        std::stringstream ss(line);
        std::string prefix;
        float value;
        ss >> prefix >> value;

        // Competition Simulation and Competition Study
        if (prefix == "a_COEFFICIENT:") {
            data.config->a_COEFFICIENT = value;
        } else if (prefix == "INFLUENCE_RATE:") {
            data.config->CONVERSION_RATE = value;
        // Competition Study
        } else if (prefix == "RUNS_PER_FRACTION:") {
            data.config->RUNS_PER_FRACTION = static_cast<int>(value);
        } else if (prefix == "FRACTIONS:") {
            data.config->FRACTIONS = static_cast<int>(value);
        } else if (prefix == "TOTAL_BOIDS:") {
            data.config->TOTAL_BOIDS = static_cast<int>(value);
        } else if (prefix == "SECONDS_PER_RUN:") {
            data.config->TIME_STEPS_PER_RUN = static_cast<int>(value);
        // Evolution Simulation
        } else if (prefix == "LANGUAGE_SIZE:") {
            data.config->LANGUAGE_SIZE = static_cast<int>(value);
        } else if (prefix == "MUTATION_RATE:") {
            data.config->MUTATION_RATE = value;
        } else if (prefix == "MINIMUM_INTERACTION_RATE:") {
            data.config->MIN_INTERACTION_RATE = value;
        } else if (prefix == "MINIMUM_ADOPTION_RATE:") {
            data.config->MIN_ADOPTION_RATE = value;
        } else if (prefix == "BOID_LIFE_STEPS:") {
            data.config->BOID_LIFE_STEPS = static_cast<int>(value);
        } else if (prefix == "BETA:") {
            data.config->BETA = value;
        } else if (prefix == "KAPPA:") {
            data.config->KAPPA = value;
        // Default Flocking Parameters
        } else if (prefix == "COHERENCE_FACTOR:") {
            data.config->COHESION_FACTOR = value;
        } else if (prefix == "ALIGNMENT_FACTOR:") {
            data.config->ALIGNMENT_FACTOR = value;
        } else if (prefix == "SEPARATION_FACTOR:") {
            data.config->SEPARATION_FACTOR = value;
        } else if (prefix == "AVOIDANCE_FACTOR:") {
            data.config->AVOIDANCE_FACTOR = value;
        } else if (prefix == "MAX_SPEED:") {
            data.config->MAX_SPEED = value;
        } else if (prefix == "MIN_SPEED:") {
            data.config->MIN_SPEED = value;
        } else if (prefix == "PERCEPTION_RADIUS:") {
            data.config->PERCEPTION_RADIUS = value;
        } else if (prefix == "INTERACTION_RADIUS:") {
            data.config->INTERACTION_RADIUS = value;
        } else if (prefix == "SEPARATION_RADIUS:") {
            data.config->SEPARATION_RADIUS = value;
        } else if (prefix == "BOID_COLLISION_RADIUS:") {
            data.config->BOID_COLLISION_RADIUS = value;
        } else if (prefix == "RESTITUTION_COEFFICIENT:") {
            data.config->RESTITUTION_COEFFICIENT = value;
        } else if (prefix == "LANGUAGE_LOG_INTERVAL:") {
            data.config->LANGUAGE_LOG_INTERVAL = static_cast<int>(value);
        } else if (prefix == "POSITION_LOG_INTERVAL:") {
            data.config->POSITION_LOG_INTERVAL = static_cast<int>(value);
        } else if (prefix == "MULTI_THREADING_ON:") {
            data.config->MULTI_THREADING = static_cast<int>(value);
        } else {
            break;
        }
    }

    // Parse World Size
    std::stringstream ss(line);
    std::string prefix;
    char delimiter;
    ss >> prefix >> data.world.width >> delimiter >> data.world.height;
    if (prefix != "Size:") {
        std::cerr << "Error: Invalid format in line:" << line << std::endl;
        return std::nullopt;
    }

    // Parse Obstacles, Terrain and Spawners
    while (std::getline(file, line)) {
        // Check the tag at the beginning of the line to determine the type of object
        if (line.compare(0, 12, "LineObstacle") == 0) {
            auto obstacle = LineObstacle::fromString(line);
            if (obstacle != nullptr) {
                data.world.obstacles.push_back(obstacle);
            }
        } else if (line.compare(0, 14, "CircleObstacle") == 0) {
            auto obstacle = CircleObstacle::FromString(line);
            if (obstacle != nullptr) {
                data.world.obstacles.push_back(obstacle);
            }
        } else if (line.compare(0, 7, "Terrain") == 0) {
            auto terrain = Terrain::FromString(line);
            if (terrain != nullptr) {
                data.world.terrains.push_back(terrain);
            }
        } else if (line.compare(0, 23, "CompBoidCircularSpawner") == 0) {
            auto spawner = CompBoidCircularSpawner::FromString(line);
            if (spawner != nullptr) {
                data.boid_spawners.push_back(spawner);
            }
        } else if (line.compare(0, 26, "CompBoidRectangularSpawner") == 0) {
            auto spawner = CompBoidRectangularSpawner::FromString(line);
            if (spawner != nullptr) {
                data.boid_spawners.push_back(spawner);
            }
        } else if (line.compare(0, 22, "EvoBoidCircularSpawner") == 0) {
            auto spawner = EvoBoidCircularSpawner::FromString(line);
            if (spawner != nullptr) {
                data.boid_spawners.push_back(spawner);
            }
        } else if (line.compare(0, 25, "EvoBoidRectangularSpawner") == 0) {
            auto spawner = EvoBoidCircularSpawner::FromString(line);
            if (spawner != nullptr) {
                data.boid_spawners.push_back(spawner);
            }
        } else {
            // Unknown type, handle accordingly (ignore or report error)
            std::cerr << "Error: Invalid format and/or unknown object type in line: " << line << std::endl;
            return std::nullopt;
        }
    }

    file.close();
    return data;
}

std::string serialization::GetFileNameThroughSaveDialog() {
#ifdef _WIN32
    char filename[MAX_PATH] = ""; // Initialize to empty string
    OPENFILENAME ofn = {}; // Zero-initialize for clarity
    ofn.lStructSize = sizeof(ofn);
    ofn.lpstrFilter = "Data Files (*.dat)\0*.dat\0All Files (*.*)\0*.*\0";
    ofn.lpstrFile = filename;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_EXPLORER | OFN_OVERWRITEPROMPT;

    if (GetSaveFileName(&ofn)) {
        std::string filepath(filename);

        // Ensure .dat extension if not already present (case-insensitive)
        if (filepath.substr(filepath.length() - 4)  != ".dat" &&
            filepath.substr(filepath.length() - 4)  != ".DAT") {
            filepath += ".dat";
            }

        return filepath;
    } else {
        // Handle error cases (see debugging suggestions below)
        // ...
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


