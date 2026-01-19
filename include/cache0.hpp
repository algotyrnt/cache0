//
//  cache0.hpp
//  cache0
//
//  Created by Punjitha Bandara on 2026-01-19.
//

#ifndef CACHE0_HPP
#define CACHE0_HPP

#include <string>
#include <unordered_map>
#include <optional>

class Cache0 {
private:
    std::unordered_map<std::string, std::string> store;

public:
    void put(const std::string& key, const std::string& value);
    std::optional<std::string> get(const std::string& key);
    bool remove(const std::string& key);
};

#endif
