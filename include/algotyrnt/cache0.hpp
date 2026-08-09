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
#include <cstdio>

namespace algotyrnt {

template <typename T>
class Cache0 {
private:
    std::unordered_map<std::string, T> store;
    std::string db_filename;

    void save_to_file(const std::string& target_filename) const {
        std::string tmp_filename = target_filename + ".tmp";
        std::ofstream file(tmp_filename, std::ios::binary);
        if (!file.is_open()) {
            throw std::runtime_error("Cache0 Error: Could not open file for writing -> " + tmp_filename);
        }

        file << "#CACHE0_V1\n";
        for (const auto& pair : store) {
            std::string valStr;
            if constexpr (std::is_same_v<T, std::string>) {
                valStr = pair.second;
            } else {
                std::stringstream ss;
                ss << pair.second;
                valStr = ss.str();
            }

            file << pair.first.size() << " " << valStr.size() << "\n";
            file << pair.first << valStr << "\n";
        }

        file.flush();
        if (file.fail()) {
            file.close();
            std::remove(tmp_filename.c_str());
            throw std::runtime_error("Cache0 Error: Failed to flush data to file -> " + tmp_filename);
        }
        file.close();

        if (std::rename(tmp_filename.c_str(), target_filename.c_str()) != 0) {
            std::remove(tmp_filename.c_str());
            throw std::runtime_error("Cache0 Error: Failed to replace database file -> " + target_filename);
        }
    }

    void load() {
        FILE* check_file = std::fopen(db_filename.c_str(), "rb");
        if (!check_file) {
            // Missing file is treated as empty cache
            return;
        }
        std::fclose(check_file);

        std::ifstream test(db_filename, std::ios::binary);
        if (!test.is_open()) {
            throw std::runtime_error("Cache0 Error: Existing database file cannot be opened -> " + db_filename);
        }

        std::string header;
        if (!std::getline(test, header)) {
            // Empty file is valid empty cache
            return;
        }

        if (header == "#CACHE0_V1") {
            std::string lenLine;
            while (std::getline(test, lenLine)) {
                if (lenLine.empty()) continue;
                std::stringstream ss(lenLine);
                size_t keyLen = 0, valLen = 0;
                if (!(ss >> keyLen >> valLen)) {
                    throw std::runtime_error("Cache0 Error: Malformed record header in -> " + db_filename);
                }

                std::string key(keyLen, '\0');
                if (keyLen > 0 && !test.read(&key[0], keyLen)) {
                    throw std::runtime_error("Cache0 Error: Incomplete key payload in -> " + db_filename);
                }

                std::string valStr(valLen, '\0');
                if (valLen > 0 && !test.read(&valStr[0], valLen)) {
                    throw std::runtime_error("Cache0 Error: Incomplete value payload in -> " + db_filename);
                }

                char newline;
                if (test.get(newline) && newline != '\n') {
                    throw std::runtime_error("Cache0 Error: Missing line delimiter in -> " + db_filename);
                }

                if constexpr (std::is_same_v<T, std::string>) {
                    store[key] = valStr;
                } else {
                    std::stringstream valSS(valStr);
                    T obj;
                    if (valSS >> obj) {
                        store[key] = obj;
                    } else {
                        throw std::runtime_error("Cache0 Error: Deserialization failed for record in -> " + db_filename);
                    }
                }
            }
        } else {
            // Compatibility path for legacy key=val files
            test.clear();
            test.seekg(0, std::ios::beg);
            std::string line;
            while (std::getline(test, line)) {
                if (line.empty()) continue;
                size_t delimPos = line.find('=');
                if (delimPos == std::string::npos) {
                    throw std::runtime_error("Cache0 Error: Malformed legacy record in -> " + db_filename);
                }
                std::string key = line.substr(0, delimPos);
                std::string valStr = line.substr(delimPos + 1);

                if constexpr (std::is_same_v<T, std::string>) {
                    store[key] = valStr;
                } else {
                    std::stringstream valSS(valStr);
                    T obj;
                    if (valSS >> obj) {
                        store[key] = obj;
                    } else {
                        throw std::runtime_error("Cache0 Error: Deserialization failed for legacy record in -> " + db_filename);
                    }
                }
            }
        }
    }

public:
    Cache0(const std::string& filename) : db_filename(filename) {
        load();
    }

    Cache0(const Cache0&) = delete;
    Cache0& operator=(const Cache0&) = delete;
    Cache0(Cache0&&) = delete;
    Cache0& operator=(Cache0&&) = delete;

    ~Cache0() {
        try {
            flush();
        } catch (const std::exception& e) {
            std::cerr << e.what() << "\n";
        }
    }

    void flush() const {
        save_to_file(db_filename);
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
