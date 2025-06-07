#ifndef POST_H
#define POST_H
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
using namespace std;

// Inisilisasi struct Post
struct Post {
    int id;
    string username;
    string content;
    int likes;
    Post *left, *right;
};

namespace River {
    Post* createPost(string username);
    bool sortbylike(const Post* a, const Post* b);
    void showFeed(const string& currentUsername);
    void showFeedForUser(const string& currentUsername, const string& targetUsername);
    void updateUserPost(Post* post);
    void showComments(int postID);
    string getTime();
    int countComments(int postID);
    void feedNavi(Post* head, const string& currentUsername);
}

#endif