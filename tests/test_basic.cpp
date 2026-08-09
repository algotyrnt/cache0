#include <iostream>
#include <cassert>
#include <algotyrnt/cache0.hpp>

void test_basic_operations() {
    {
        algotyrnt::Cache0<std::string> db("test.db");
        db.put("key1", "val1");
        auto val = db.get("key1");
        assert(val.has_value() && *val == "val1");
    }
    std::cout << "[PASS] Basic Operations" << std::endl;
}

int main() {
    test_basic_operations();
    std::cout << "All tests passed!" << std::endl;
    return 0;
}
