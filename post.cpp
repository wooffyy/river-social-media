#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <sstream>
#include <string>
#include <ctime>
#include <limits>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include "post.h" 
#include "follow.h" 
#include "account.h"
#include "activity.h"
#include "notify.h"
#include "whirlpool.h"
using namespace std;


void showPostByTag(const string& currentUsername, const string& hashtag) {
    using namespace River;
    extern unordered_map<string, struct AVLNode*> hashtagIndex; // akses global
    
    auto it = hashtagIndex.find(hashtag);
    if (it == hashtagIndex.end() || it->second == nullptr) {
        cout << "Tidak ada post untuk #" << hashtag << endl;
        cout << "[Enter untuk kembali]"; cin.get();
        return;
    }
    vector<int> postIDs;
    inOrderTraversal(it->second, postIDs); 
    if (postIDs.empty()) {
        cout << "Tidak ada post untuk #" << hashtag << endl;
        cout << "[Enter untuk kembali]"; cin.get();
        return;
    }
    ifstream file("post_data.txt");
    if (!file) {
        cout << "Gagal membuka file post_data.txt!\n";
        return;
    }
    vector<Post*> feed;
    string line;
    bool isFirstLine = true;
    while (getline(file, line)) {
        if (isFirstLine) {
            isFirstLine = false;
            continue;
        }
        if (line.empty()) continue;
        stringstream ss(line);
        Post* post = new Post();
        string temp;
        getline(ss, temp, ',');
        int postID = stoi(temp);
        // Cek apakah postID ada di postIDs
        if (find(postIDs.begin(), postIDs.end(), postID) == postIDs.end()) {
            delete post;
            continue;
        }
        post->id = postID;
        getline(ss, post->username, ',');
        getline(ss, post->content, ',');
        getline(ss, temp, ',');
        post->likes = stoi(temp);
        post->left = nullptr;
        post->right = nullptr;
        feed.push_back(post);
    }
    file.close();
    // Buat doubly linked list
    Post* head = nullptr;
    Post* tail = nullptr;
    for (Post* p : feed) {
        if (head == nullptr) {
            head = tail = p;
        } else {
            tail->right = p;
            p->left = tail;
            tail = p;
        }
    }
    if (head != nullptr) {
        feedNavi(head, currentUsername);
    } else {
        cout << "Tidak ada post untuk #" << hashtag << endl;
        cout << "[Enter untuk kembali]"; cin.get();
    }
    // Cleanup
    for (Post* p : feed) {
        delete p;
    }
}

namespace River {

    // Cek apakah user sudah like post ini
    bool hasLiked(int postID, const string& username) {
        string likeFile = "liked_post_" + to_string(postID) + ".txt";
        ifstream file(likeFile);
        if (!file) return false;

        string line;
        while (getline(file, line)) {
            if (line == username) return true;
        }
        return false;
    }

    // Tambahkan user ke daftar like
    void addLikeUser(int postID, const string& username) {
        ofstream file("liked_post_" + to_string(postID) + ".txt", ios::app);
        file << username << "\n";
    }

    // Hapus user dari daftar like
    void removeLikeUser(int postID, const string& username) {
        string likeFile = "liked_post_" + to_string(postID) + ".txt";
        ifstream file(likeFile);
        if (!file) return;

        vector<string> users;
        string line;
        while (getline(file, line)) {
            if (line != username) {
                users.push_back(line);
            }
        }
        file.close();

        ofstream outFile(likeFile);
        for (const string& user : users) {
            outFile << user << "\n";
        }
    }

    Post* createPost(string username) {
        int globalID = 0;
        ifstream globalFile("post_data.txt");
        string line, lastLine;
        while (getline(globalFile, line)) {
            if (line.empty() || line.find("id,") != string::npos) continue;
            lastLine = line;
        }
        globalFile.close();

        if (!lastLine.empty()) {
            stringstream ss(lastLine);
            string idStr;
            getline(ss, idStr, ',');
            globalID = stoi(idStr);
        }

        Post* newPost = new Post();
        newPost->id = globalID + 1; 
        newPost->username = username;
        newPost->likes = 0;

        cout << "Masukkan isi post: ";
        getline(cin, newPost->content);

        string postFile = "users/" + username + "/posts.csv";
        ofstream app_content(postFile, ios::app);
        if (!app_content) {
            cout << "Gagal membuka file untuk menulis!\n";
            delete newPost;
            return nullptr;
        }
        app_content << newPost->id << "," << newPost->username << "," << newPost->content << "," << newPost->likes << "\n";
        app_content.close();

        ofstream feedPost("post_data.txt", ios::app);
        if (!feedPost) {
            cout << "Gagal membuka file untuk menulis!\n";
            return newPost;
        }

        ifstream feedFile("post_data.txt");
        bool isEmpty = feedFile.peek() == EOF;
        feedFile.close();

        if (isEmpty) {
            feedPost << "id,username,content,likes\n";  
        }

        feedPost << newPost->id << "," << newPost->username << "," << newPost->content << "," << newPost->likes << "\n";
        feedPost.close();

        cout << "Post berhasil dibuat!\n";
        buildHashtagIndexFromPostData();
        return newPost;
    }

    void showFeed(const string& currentUsername){
        ifstream file("post_data.txt");
        if (!file) {
            cout << "Gagal membuka file post_data.txt!\n";
            return;
        }

        vector<Post*> feed;
        string line;
        bool isFirstLine = true;
        while (getline(file, line)) {
            if (isFirstLine) {
                isFirstLine = false; 
                continue;
            }

            if (line.empty()) continue;

            stringstream ss(line);
            Post* post = new Post();
            string temp;

            getline(ss, temp, ',');
            try {
                post->id = stoi(temp);
            } catch (...) {
                delete post;
                continue;
            }
            getline(ss, post->username, ',');
            getline(ss, post->content, ',');
            getline(ss, temp, ',');
            try {
                post->likes = stoi(temp);
            } catch (const exception& e) {
                post->likes = 0; // fallback default jika parsing gagal
            }


            post->left = nullptr;
            post->right = nullptr;
            feed.push_back(post);
        }
        file.close();

        // Lambda sort descending by like count
        sort(feed.begin(), feed.end(), [](const Post* a, const Post* b) {
            return a->likes > b->likes;
        });

        Post* head = nullptr;
        Post* tail = nullptr;
        for (Post* p : feed) {
            if (head == nullptr) {
                head = tail = p;
            } else {
                tail->right = p;
                p->left = tail;
                tail = p;
            }
        }

        if (head != nullptr) {
            River::feedNavi(head, currentUsername);
        } 

        ofstream release("post_data.txt", ios::trunc);
        if (!release) {
            cout << "Gagal membuka file post_data.txt untuk menulis!\n";
            for (Post* p : feed) {
                delete p;
            }
            return;
        }

        release << "id,username,content,likes\n";
        for (Post* p : feed) {
            release << p->id << "," << p->username << "," << p->content << "," << p->likes << "\n";
            updateUserPost(p);
        }
        release.close();
    }

    void showFeedForUser(const string& currentUsername, const string& targetUsername) {
        ifstream file("post_data.txt");
        if (!file) {
            cout << "Gagal membuka file post_data.txt!\n";
            return;
        }

        vector<Post*> feed;
        string line;
        bool isFirstLine = true;
        while (getline(file, line)) {
            if (isFirstLine) {
                isFirstLine = false; 
                continue;
            }

            if (line.empty()) continue;

            stringstream ss(line);
            Post* post = new Post();
            string temp;

            getline(ss, temp, ',');
            try {
                post->id = stoi(temp);
            } catch (...) {
                delete post;
                continue;
            }
            getline(ss, post->username, ',');
            getline(ss, post->content, ',');
            getline(ss, temp, ',');
            post->likes = stoi(temp);

            post->left = nullptr;
            post->right = nullptr;

            // Ini dia: filter hanya post dari targetUsername
            if (post->username == targetUsername) {
                feed.push_back(post);
            } else {
                delete post;
            }
        }
        file.close();

        if (feed.empty()) {
            cout << "Tidak ada post dari @" << targetUsername << endl;
            cout << "[Enter untuk kembali]"; cin.get();
            return;
        }

        // Sort by like
        sort(feed.begin(), feed.end(), [](const Post* a, const Post* b) {
            return a->likes > b->likes;
        });

        // Linked list
        Post* head = nullptr;
        Post* tail = nullptr;
        for (Post* p : feed) {
            if (head == nullptr) {
                head = tail = p;
            } else {
                tail->right = p;
                p->left = tail;
                tail = p;
            }
        }

        if (head != nullptr) {
            River::feedNavi(head, currentUsername);
        }

        // Cleanup
        for (Post* p : feed) {
            delete p;
        }
    }

    void updateUserPost(Post* post) {
        string postFile = "users/" + post->username + "/posts.csv";
        ifstream file(postFile);
        if (!file) {
            cout << "Gagal membuka file posts.csv untuk user " << post->username << "!\n";
            return;
        }

        vector<string> posts;
        string line;
        while (getline(file, line)) {
            stringstream ss(line);
            string idStr;
            getline(ss, idStr, ',');
            if (!idStr.empty() && stoi(idStr) == post->id) {
                posts.push_back(to_string(post->id) + "," + post->username + "," + post->content + "," + to_string(post->likes));
            } else {
                posts.push_back(line);
            }
        }
        file.close();

        ofstream outFile(postFile);
        if (!outFile) {
            cout << "Gagal menulis ke file posts.csv untuk user " << post->username << "!\n";
            return;
        }

        for (const string& p : posts) {
            outFile << p << "\n";
        }
        outFile.close();
    }

    void showComments(int PostID){
        string filename = "comment_post_" + to_string(PostID) + ".csv";
        ifstream file(filename);
        if (!file) {
            cout << "Belum ada komentar untuk post ini!\n";
            return;
        }

        string line;
        bool commentFound = false;
        while (getline(file, line)) {
            commentFound = true;
            stringstream ss(line);
            string username, comment, date;
            getline(ss, username, ',');
            getline(ss, comment, ',');
            getline(ss, date, ',');
            cout << "@" << username << " (" << date << "): " << comment << endl;
        }
        file.close();

        if (!commentFound) {
            cout << "Belum ada komentar untuk post ini!\n";
        }
    }

    inline string getTime(){
        time_t now = time(0);
        tm *ltm = localtime(&now);
        stringstream ss;
        ss << 1900 + ltm->tm_year << "-" << 1 + ltm->tm_mon << "-" << ltm->tm_mday;
        return ss.str();
    }

    int countComments(int PostID){
        string filename = "comment_post_" + to_string(PostID) + ".csv";
        ifstream file(filename);
        if (!file) {
            return 0;
        }

        int count = 0;
        string line;
        while (getline(file, line)) {
            count++;
        }
        file.close();
        return count;
    }

    void feedNavi(Post* head, const string& currentUsername){
        Post* current = head;
        char input, lowerInput;

        while (true) {
            #ifdef _WIN32
                system("cls");
            #else
                system("clear");
            #endif

            if (current == nullptr) {
                cout << "Kamu telah mencapai akhir!\n";
                break;
            }

            // Tampilkan username + status following
            cout << "from @" << current->username;

            // Cek follow status
            bool following = isFollowing(currentUsername, current->username);
            if (currentUsername == current->username) {
                cout << "    [Your Post]\n";
            } else {
                cout << "    [" << (following ? "Following" : "Follow") << "]\n";
            }

            // Tampilkan post content
            cout << "Post : " << "\n\n" << current->content << endl;
            cout << "\n" << current->likes << " likes | " << countComments(current->id) << " yappers" << endl;
            cout << "----------------------------------------------------------" << endl;
            cout << "[L] to like    [C] to yap         [F] to follow/unfollow" << endl;
            cout << "[N] next post  [P] previous post  [X] to exit" << endl;
            cout << ">> ";
            cin >> input;
            lowerInput = tolower(input);
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            if (lowerInput == 'n') {
                current = current->right;
            } else if (lowerInput == 'p') {
                current = current->left;
            } else if (lowerInput == 'l') {
                if (hasLiked(current->id, currentUsername)) {
                    current->likes--;
                    removeLikeUser(current->id, currentUsername);
                    cout << "Post di-unlike!\n";
                } else {
                    current->likes++;
                    addLikeUser(current->id, currentUsername);
                    cout << "Post sudah di-like!\n";
                    Activity::recordLike(currentUsername, current->id, current->username);
                    Notify::likesNotif(currentUsername, current->username, current->id, current->content);
                }
            } else if (lowerInput == 'c') {
                string comment;
                showComments(current->id);
                cout << "----------------------------------------------------------" << endl;
                cout << "[T] untuk yap! , [X] untuk kembali : ";
                char comment_input;
                cin >> comment_input;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                char comment_inputLower = tolower(comment_input);
                if (comment_inputLower == 't') {
                    cout << "Tulis komentarmu:\n> ";
                    getline(cin, comment);

                    string filename = "comment_post_" + to_string(current->id) + ".csv";
                    ofstream file(filename, ios::app);
                    if (file.is_open()) {
                        file << currentUsername << "," << comment << "," << getTime() << "\n";
                        file.close();
                        cout << "Komentar berhasil dikirim!\n";
                        Activity::recordComment(currentUsername, current->id, current->username);
                        Notify::commentNotif(currentUsername, current->username, current->id, current->content);
                    } else {
                        cout << "Gagal menyimpan komentar.\n";
                    }
                } else if (comment_inputLower == 'x') {
                    continue;
                }
            } else if (lowerInput == 'f') {
                if (currentUsername == current->username) {
                    cout << "Tidak bisa follow diri sendiri.\n";
                } else if (isFollowing(currentUsername, current->username)) {
                    unfollowUser(currentUsername, current->username);
                } else {
                    followUser(currentUsername, current->username);
                }
            } else if (lowerInput == 'x') {
                cout << "Keluar dari feed\n";
                break;
            } else {
                cout << "Input tidak valid!\n";
            }
        }
    }
}
