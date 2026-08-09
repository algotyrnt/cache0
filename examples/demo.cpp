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
    std::cout << "Library initialized successfully." << std::endl;
    return 0;
}
