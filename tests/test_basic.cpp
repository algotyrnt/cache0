#include <iostream>
#include <cstdio>
#include <stdexcept>
#include <algotyrnt/cache0.hpp>

void test_basic_operations() {
    const std::string db_path = "test_basic.db";
    std::remove(db_path.c_str());

    {
        algotyrnt::Cache0<std::string> db(db_path);
        if (!db.empty() || db.size() != 0) {
            std::remove(db_path.c_str());
            throw std::runtime_error("Test failed: cache should be empty initially");
        }

        db.put("key1", "val1");
        db.put("key2", "val2");

        if (db.size() != 2 || !db.contains("key1")) {
            std::remove(db_path.c_str());
            throw std::runtime_error("Test failed: size or contains check failed");
        }

        db.remove("key2");
        db.flush();
    }

    {
        algotyrnt::Cache0<std::string> db2(db_path);
        auto val1 = db2.get("key1");
        if (!val1.has_value() || *val1 != "val1") {
            std::remove(db_path.c_str());
            throw std::runtime_error("Test failed: key1 was not restored with expected value 'val1'");
        }

        auto val2 = db2.get("key2");
        if (val2.has_value()) {
            std::remove(db_path.c_str());
            throw std::runtime_error("Test failed: key2 was not removed");
        }

        db2.compact();
    }

    std::remove(db_path.c_str());
    std::cout << "[PASS] Basic Operations & WAL Log Replay" << std::endl;
}

void test_move_and_clear() {
    const std::string db_path = "test_move.db";
    std::remove(db_path.c_str());

    {
        algotyrnt::Cache0<int> db(db_path);
        db.put("num1", 42);

        algotyrnt::Cache0<int> db_moved = std::move(db);
        auto val = db_moved.get("num1");
        if (!val.has_value() || *val != 42) {
            std::remove(db_path.c_str());
            throw std::runtime_error("Test failed: move constructor failed to transfer data");
        }

        db_moved.clear();
        if (!db_moved.empty() || db_moved.size() != 0) {
            std::remove(db_path.c_str());
            throw std::runtime_error("Test failed: clear failed to reset store");
        }
    }

    std::remove(db_path.c_str());
    std::cout << "[PASS] Move Semantics & Clear Operations" << std::endl;
}

int main() {
    test_basic_operations();
    test_move_and_clear();
    std::cout << "All tests passed successfully!" << std::endl;
    return 0;
}
