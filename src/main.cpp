//
//  main.cpp
//  cache0
//
//  Created by Punjitha Bandara on 2026-01-19.
//

#include <iostream>
#include "../include/cache0.hpp"

int main() {
    std::cout << "[cache0] Initializing..." << std::endl;
    
    Cache0 db;
    
    db.put("user_id", "1024");
    
    auto result = db.get("user_id");
    if (result) {
        std::cout << "[cache0] Found: " << *result << std::endl;
    }
    
    return 0;
}
