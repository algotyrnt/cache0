#include <iostream>
#include <algotyrnt/cache0.hpp>

struct User {
    int id;
    std::string role;

    friend std::ostream& operator<<(std::ostream& os, const User& u) {
        return os << u.id << " " << u.role;
    }
    friend std::istream& operator>>(std::istream& is, User& u) {
        return is >> u.id >> u.role;
    }
};

int main() {
    using namespace algotyrnt;
    Cache0<User> userDB("users.db");

    User admin = {1, "SuperAdmin"};
    userDB.put("root", admin);
    userDB.flush();

    if (userDB.contains("root")) {
        auto fetched = userDB.get("root");
        if (fetched) {
            std::cout << "User root found: ID=" << fetched->id << ", Role=" << fetched->role << std::endl;
        }
    }

    std::cout << "Cache size: " << userDB.size() << " entry." << std::endl;

    userDB.compact();
    std::cout << "Library initialized and demonstrated successfully." << std::endl;
    return 0;
}
