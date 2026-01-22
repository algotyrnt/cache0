/*
 * Cache0 - A lightweight C++ Key-Value Store
 * Copyright (c) 2026 Punjitha (algotyrnt)
 * Licensed under the MIT License.
 */

#include <iostream>
#include <cassert>
#include "../include/cache0.hpp"

// Helper function to print text for success
void pass(const std::string& name) {
    std::cout << "[PASS] " << name << std::endl;
}

void test_basic_operations() {
    {
        Cache0 db;

        // 1. Test PUT and GET
        db.put("test_key", "123");
        auto val = db.get("test_key");
        assert(val.has_value() && "Key should exist");
        assert(*val == "123" && "Value should be 123");

        // 2. Test UPDATE (Overwrite)
        db.put("test_key", "456");
        val = db.get("test_key");
        assert(*val == "456" && "Value should be updated to 456");

        // 3. Test REMOVE
        db.remove("test_key");
        val = db.get("test_key");
        assert(!val.has_value() && "Key should be deleted");
    }

    pass("Basic Operations");
}

void test_persistence() {
    {
        Cache0 db;
        db.put("persistent_key", "alive");
    }

    {
        Cache0 db2;
        auto val = db2.get("persistent_key");
        assert(val.has_value() && "Data should survive restart");
        assert(*val == "alive" && "Value should be preserved");
    }

    pass("Persistence (Save/Load)");
}

int main() {
    std::cout << "Running cache0 Tests..." << std::endl;

    test_basic_operations();
    test_persistence();

    std::cout << "All tests passed successfully!" << std::endl;
    return 0;
}
