#ifndef ACCOUNT_H
#define ACCOUNT_H
#include <iostream>
#include <string>
#include <vector>
#include <stack>
using namespace std;

// Inisialisasi struct User
struct User {
    int id;
    string username;
    string password;
    string bio;
    // Konstruktor default dan konstruktor dengan parameter
    User() 
        : id(0), username(""), password(""), bio("Empty") {}
    User(int _id, const string& _user, const string& _pass, const string& _bio = "Empty")
        : id(_id), username(_user), password(_pass), bio(_bio) {}
};

namespace Account {
    extern vector<User> userList;
    struct BSTNode;
    void insertUserBST(const User& user);
    BSTNode* searchUserBST(const string& username);
    void inOrderTraversal(BSTNode* node, vector<User>& result);
    void buildBSTFromVector();
    int findUserBST(const string& username);
    void deleteBST(BSTNode* node);
    void loadUsersWithBST();
    void cleanupBST();
    void loadUsers();
    bool compareUsers(const User& a, const User& b);
    void saveUsers();
    int binarySearchUser(const std::string& username);
    void signUp();
    User* login();
    bool create_user_dir(const string& username);
    void searchAccount(const string& username);
    void profilePage(const User& user);

    template <typename T>
    T countStats(const string& username, const string& statType);
    
    // LIKES
    template <>
    int countStats<int>(const string& username, const string& statType);
    
    // POSTS
    template <>
    double countStats<double>(const string& username, const string& statType);
}

#endif