#include <iostream>
#include <cstdio>
#include <stdexcept>
#include <algotyrnt/cache0.hpp>

void test_basic_operations() {
    const std::string db_path = "test.db";
    std::remove(db_path.c_str());

    {
        algotyrnt::Cache0<std::string> db(db_path);
        db.put("key1", "val1");
        db.flush();
    }

    {
        algotyrnt::Cache0<std::string> db2(db_path);
        auto val = db2.get("key1");
        if (!val.has_value() || *val != "val1") {
            std::remove(db_path.c_str());
            throw std::runtime_error("Test failed: key1 was not restored with expected value 'val1'");
        }
    }

    std::remove(db_path.c_str());
    std::cout << "[PASS] Basic Operations" << std::endl;
}

int main() {
    test_basic_operations();
    std::cout << "All tests passed!" << std::endl;
    return 0;
}
