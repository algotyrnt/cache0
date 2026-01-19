//
//  main.cpp
//  cache0
//
//  Created by Punjitha Bandara on 2026-01-19.
//

#include <iostream>
#include "../include/cache0.hpp"

int main() {
    Cache0 db;
    
    std::cout << "[Cache0] Initializing..." << std::endl;
    db.put("user_id", "1024");
    
    auto result = db.get("user_id");
    if (result) {
        std::cout << "[Cache0] Found: " << *result << std::endl;
    }
    
    return 0;
}
