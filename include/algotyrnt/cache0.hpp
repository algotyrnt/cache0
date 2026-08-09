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
#include <utility>

namespace algotyrnt {

template <typename T>
class Cache0 {
private:
    std::unordered_map<std::string, T> store;
    std::string db_filename;
    mutable std::ofstream wal_stream;

    static std::string serialize_value(const T& val) {
        if constexpr (std::is_same_v<T, std::string>) {
            return val;
        } else {
            std::stringstream ss;
            ss << val;
            return ss.str();
        }
    }

    static T deserialize_value(const std::string& valStr) {
        if constexpr (std::is_same_v<T, std::string>) {
            return valStr;
        } else {
            std::stringstream valSS(valStr);
            T obj;
            if (valSS >> obj) {
                return obj;
            }
            throw std::runtime_error("Cache0 Error: Deserialization failed for record payload");
        }
    }

    void open_wal_append() const {
        if (db_filename.empty()) return;
        if (wal_stream.is_open()) {
            wal_stream.close();
        }
        wal_stream.open(db_filename, std::ios::binary | std::ios::app);
        if (!wal_stream.is_open()) {
            throw std::runtime_error("Cache0 Error: Could not open WAL log file -> " + db_filename);
        }
    }

    void append_set_record(const std::string& key, const std::string& valStr) const {
        if (db_filename.empty()) return;
        if (!wal_stream.is_open()) {
            open_wal_append();
        }
        wal_stream << "1 " << key.size() << " " << valStr.size() << "\n";
        wal_stream << key << valStr << "\n";
    }

    void append_del_record(const std::string& key) const {
        if (db_filename.empty()) return;
        if (!wal_stream.is_open()) {
            open_wal_append();
        }
        wal_stream << "2 " << key.size() << "\n";
        wal_stream << key << "\n";
    }

    void load() {
        if (db_filename.empty()) return;

        std::ifstream check_stream(db_filename, std::ios::binary);
        if (!check_stream.is_open()) {
            std::ofstream init_file(db_filename, std::ios::binary);
            if (!init_file.is_open()) {
                throw std::runtime_error("Cache0 Error: Could not create database file -> " + db_filename);
            }
            init_file << "#CACHE0_WAL_V1\n";
            init_file.close();
            open_wal_append();
            return;
        }

        std::string header;
        if (!std::getline(check_stream, header)) {
            check_stream.close();
            open_wal_append();
            return;
        }

        if (header != "#CACHE0_WAL_V1") {
            check_stream.close();
            throw std::runtime_error("Cache0 Error: Invalid database header in -> " + db_filename);
        }

        std::string line;
        while (std::getline(check_stream, line)) {
            if (line.empty()) continue;
            std::stringstream ss(line);
            int opCode = 0;
            if (!(ss >> opCode)) continue;

            if (opCode == 1) {
                size_t keyLen = 0, valLen = 0;
                if (!(ss >> keyLen >> valLen)) {
                    throw std::runtime_error("Cache0 Error: Malformed SET record header in -> " + db_filename);
                }
                std::string key(keyLen, '\0');
                if (keyLen > 0 && !check_stream.read(&key[0], keyLen)) {
                    throw std::runtime_error("Cache0 Error: Incomplete key in WAL -> " + db_filename);
                }
                std::string valStr(valLen, '\0');
                if (valLen > 0 && !check_stream.read(&valStr[0], valLen)) {
                    throw std::runtime_error("Cache0 Error: Incomplete value in WAL -> " + db_filename);
                }
                char nl;
                check_stream.get(nl);

                try {
                    store[key] = deserialize_value(valStr);
                } catch (const std::exception& e) {
                    throw std::runtime_error("Cache0 Error: " + std::string(e.what()) + " in -> " + db_filename);
                }
            } else if (opCode == 2) {
                size_t keyLen = 0;
                if (!(ss >> keyLen)) {
                    throw std::runtime_error("Cache0 Error: Malformed DEL record header in -> " + db_filename);
                }
                std::string key(keyLen, '\0');
                if (keyLen > 0 && !check_stream.read(&key[0], keyLen)) {
                    throw std::runtime_error("Cache0 Error: Incomplete key in WAL -> " + db_filename);
                }
                char nl;
                check_stream.get(nl);
                store.erase(key);
            }
        }

        check_stream.close();
        open_wal_append();
    }

public:
    explicit Cache0(const std::string& filename) : db_filename(filename) {
        load();
    }

    Cache0(const Cache0&) = delete;
    Cache0& operator=(const Cache0&) = delete;

    Cache0(Cache0&& other) noexcept
        : store(std::move(other.store)),
          db_filename(std::move(other.db_filename)),
          wal_stream(std::move(other.wal_stream)) {}

    Cache0& operator=(Cache0&& other) noexcept {
        if (this != &other) {
            if (wal_stream.is_open()) {
                wal_stream.close();
            }
            store = std::move(other.store);
            db_filename = std::move(other.db_filename);
            wal_stream = std::move(other.wal_stream);
        }
        return *this;
    }

    ~Cache0() {
        try {
            if (!db_filename.empty() && wal_stream.is_open()) {
                compact();
            }
        } catch (const std::exception& e) {
            std::cerr << e.what() << "\n";
        }
    }

    void flush() const {
        if (wal_stream.is_open()) {
            wal_stream.flush();
            if (wal_stream.fail()) {
                throw std::runtime_error("Cache0 Error: Failed to flush WAL stream -> " + db_filename);
            }
        }
    }

    void compact() const {
        if (db_filename.empty()) return;
        if (wal_stream.is_open()) {
            wal_stream.close();
        }

        std::string tmp_filename = db_filename + ".compact.tmp";
        std::ofstream tmp_file(tmp_filename, std::ios::binary);
        if (!tmp_file.is_open()) {
            throw std::runtime_error("Cache0 Error: Could not open temporary compaction file -> " + tmp_filename);
        }

        tmp_file << "#CACHE0_WAL_V1\n";
        for (const auto& pair : store) {
            std::string valStr = serialize_value(pair.second);
            tmp_file << "1 " << pair.first.size() << " " << valStr.size() << "\n";
            tmp_file << pair.first << valStr << "\n";
        }

        tmp_file.flush();
        if (tmp_file.fail()) {
            tmp_file.close();
            std::remove(tmp_filename.c_str());
            throw std::runtime_error("Cache0 Error: Failed to flush compaction file -> " + tmp_filename);
        }
        tmp_file.close();

        if (std::rename(tmp_filename.c_str(), db_filename.c_str()) != 0) {
            std::remove(tmp_filename.c_str());
            throw std::runtime_error("Cache0 Error: Failed to replace database file during compaction -> " + db_filename);
        }

        open_wal_append();
    }

    void put(const std::string& key, const T& value) {
        store[key] = value;
        append_set_record(key, serialize_value(value));
    }

    std::optional<T> get(const std::string& key) const {
        auto it = store.find(key);
        if (it != store.end()) {
            return it->second;
        }
        return std::nullopt;
    }

    bool remove(const std::string& key) {
        auto it = store.find(key);
        if (it != store.end()) {
            store.erase(it);
            append_del_record(key);
            return true;
        }
        return false;
    }

    bool contains(const std::string& key) const {
        return store.find(key) != store.end();
    }

    size_t size() const noexcept {
        return store.size();
    }

    bool empty() const noexcept {
        return store.empty();
    }

    void clear() {
        store.clear();
        compact();
    }
};

} // namespace algotyrnt

#endif
