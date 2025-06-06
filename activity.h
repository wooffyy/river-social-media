#ifndef ACTIVITY_H
#define ACTIVITY_H

#include <string>
#include <stack>

using namespace std;

// Enum tipe aktivitas
enum Actype {
    LIKE,
    COMMENT,
    CREATE
};

// Struct aktivitas
struct Act {
    Actype type;
    int target_post;
    string target_user;
    string timestamp;
};

namespace Activity {
    // Simpan dan muat stack aktivitas
    bool saveStack(const string& username, const stack<Act>& activityStack);
    bool loadStack(const string& username, stack<Act>& activityStack);

    // Fungsi untuk mencatat aktivitas
    void recordLike(const string& username, int postID, const string& owner);
    void recordComment(const string& username, int postID, const string& owner);
    void recordPost(const string& username, int postID, const string& owner);

    // Tampilkan aktivitas
    void showActivity(const string& username);
}

#endif
