/*
 * Cache0 - A lightweight C++ Key-Value Store
 * Copyright (c) 2026 Punjitha (algotyrnt)
 * Licensed under the MIT License.
 */

#ifndef CACHE0_HPP
#define CACHE0_HPP

#include <string>
#include <unordered_map>
#include <optional>
#include <fstream>

class Cache0 {
private:
    std::unordered_map<std::string, std::string> store;
    const std::string filename = "cache0.db";

    void load();
    void save();
public:
    Cache0();  // Constructor (Loads data)
    ~Cache0(); // Destructor (Saves data)

    void put(const std::string& key, const std::string& value);
    std::optional<std::string> get(const std::string& key);
    bool remove(const std::string& key);
};

#endif
