//
//  cache0.cpp
//  cache0
//
//  Created by Punjitha Bandara on 2026-01-19.
//

#include "../include/cache0.hpp"

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
