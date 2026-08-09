/*
 * Cache0 - A lightweight C++ Key-Value Store
 * Copyright (c) 2026 Punjitha (algotyrnt)
 * Licensed under the MIT License.
 */

#ifndef ALGOTYRNT_CACHE0_HPP
#define ALGOTYRNT_CACHE0_HPP

#include <iostream>
#include <fstream>
#include <unordered_map>
#include <string>
#include <optional>
#include <sstream>
#include <type_traits> 
#include <stdexcept>   

namespace algotyrnt {

template <typename T>
class Cache0 {
private:
    std::unordered_map<std::string, T> store;
    std::string db_filename;

    void save() const {
        std::ofstream file(db_filename);
        if (!file.is_open()) {
            throw std::runtime_error("Cache0 Error: Could not open file for writing -> " + db_filename);
        }
        for (const auto& pair : store) {
            file << pair.first << "=" << pair.second << "\n";
        }
    }

    void load() {
        std::ifstream file(db_filename);
        if (!file.good()) return; 

        std::string line;
        while (std::getline(file, line)) {
            size_t delimPos = line.find('=');
            if (delimPos != std::string::npos) {
                std::string key = line.substr(0, delimPos);
                std::string valStr = line.substr(delimPos + 1);

                if constexpr (std::is_same_v<T, std::string>) {
                     store[key] = valStr; 
                } else {
                    std::stringstream ss(valStr);
                    T obj;
                    if (ss >> obj) {
                        store[key] = obj;
                    }
                }
            }
        }
    }

public:
    Cache0(const std::string& filename) : db_filename(filename) {
        load();
    }

    ~Cache0() {
        try {
            save();
        } catch (const std::exception& e) {
            std::cerr << e.what() << "\n";
        }
    }

    void put(const std::string& key, const T& value) {
        store[key] = value;
    }

    std::optional<T> get(const std::string& key) const {
        auto it = store.find(key);
        if (it != store.end()) {
            return it->second;
        }
        return std::nullopt;
    }

    bool remove(const std::string& key) {
        return store.erase(key) > 0;
    }
};

} // namespace algotyrnt

#endif
