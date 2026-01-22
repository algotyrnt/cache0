/*
 * Cache0 - A lightweight C++ Key-Value Store
 * Copyright (c) 2026 Punjitha (algotyrnt)
 * Licensed under the MIT License.
 */

#include <iostream>
#include "../include/cache0.hpp"

Cache0::Cache0() {
    load();
}

Cache0::~Cache0() {
    save();
}

void Cache0::put(const std::string& key, const std::string& value) {
    store[key] = value;
}

std::optional<std::string> Cache0::get(const std::string& key) {
    if (store.find(key) != store.end()) {
        return store[key];
    }
    return std::nullopt;
}

bool Cache0::remove(const std::string& key) {
    return store.erase(key) > 0;
}

void Cache0::save() {
    std::ofstream file(filename);

    if (file.is_open()) {
        for (const auto& pair : store) {
            file << pair.first << "=" << pair.second << std::endl;
        }
        file.close();
        std::cout << "[cache0] Data saved to disk." << std::endl;
    } else {
        std::cerr << "[cache0] Error: Could not save data." << std::endl;
    }
}

void Cache0::load() {
    std::ifstream file(filename);
    std::string line;

    if (file.is_open()) {
        while (std::getline(file, line)) {
            size_t delimiterPos = line.find('=');

            if (delimiterPos != std::string::npos) {
                std::string key = line.substr(0, delimiterPos);
                std::string value = line.substr(delimiterPos + 1);
                store[key] = value;
            }
        }
        file.close();
        std::cout << "[cache0] Data loaded." << std::endl;
    }
}
