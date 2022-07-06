#pragma once
#include <string>
#include <map>

using std::map;
using std::string;

// Forward delcare
class User; 

class USocial {
public:
    USocial() = default;
    ~USocial();

    USocial(const USocial& other);

    USocial& operator=(const USocial& other);

    User* registerUser(const string& name, bool isBusinessUser=false);
    User* getUserById(unsigned long userID);
    void removeUser(User* user);
private:
    map<unsigned long, User*> users_;

    void clearUsers();
    void copyUsers(const map<unsigned long, User*>& users);
};
