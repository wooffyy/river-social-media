#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <stack>
#include "activity.h"
#include "post.h"

using namespace std;

namespace Activity {
    // Simpan stack ke file
    bool saveStack(const string& username, const stack<Act>& activityStack) {
        string file_name = "users/" + username + "/activity_stack.txt";
        ofstream file(file_name);
        if (!file) {
            cout << "Gagal membuka file untuk menulis!\n";
            return false;
        }

        stack<Act> temp = activityStack; 
        vector<Act> reverseList;

        while (!temp.empty()) {
            reverseList.push_back(temp.top());
            temp.pop();
        }

        for (auto it = reverseList.rbegin(); it != reverseList.rend(); ++it) {
            file << it->type << "," << it->target_post << "," << it->target_user << "," << it->timestamp << "\n";
        }

        file.close();
        return true;
    }

    bool loadStack(const string& username, stack<Act>& activityStack) {
        string file_name = "users/" + username + "/activity_stack.txt";
        ifstream file(file_name);
        if (!file) {
            cout << "No activity yet\n";
            return true;
        }

        vector<Act> acts;
        string line;
        while (getline(file, line)) {
            if (line.empty()) continue;

            stringstream ss(line);
            string type_str, tp_str, tu_str, ts_str;
            getline(ss, type_str, ',');
            getline(ss, tp_str, ',');
            getline(ss, tu_str, ',');
            getline(ss, ts_str, ',');

            Act act;
            act.type = static_cast<Actype>(stoi(type_str));
            act.target_post = stoi(tp_str);
            act.target_user = tu_str;
            act.timestamp = ts_str;

            acts.push_back(act);
        }
        file.close();

        activityStack = stack<Act>();
        for (const auto& act : acts) {
            activityStack.push(act);
        }

        return true;
    }

    // Simpan aktivitas like
    void recordLike(const string& username, int postID, const string& owner) {
        stack<Act> actStack;
        loadStack(username, actStack);

        actStack.push({LIKE, postID, owner, River::getTime()});
        saveStack(username, actStack);
    }

    // Simpan aktivitas comment
    void recordComment(const string& username, int postID, const string& owner) {
        stack<Act> actStack;
        loadStack(username, actStack);

        actStack.push({COMMENT, postID, owner, River::getTime()});
        saveStack(username, actStack);
    }

    // Simpan aktivitas post
    void recordPost(const string& username, int postID, const string& owner) {
        stack<Act> actStack;
        loadStack(username, actStack);

        actStack.push({CREATE, postID, owner, River::getTime()});
        saveStack(username, actStack);
    }

    // Tampilkan riwayat aktivitas
    void showActivity(const string& username) {
        stack<Act> actStack;
        if (!loadStack(username, actStack)) {
            cout << "Gagal memuat aktivitas\n";
            return;
        }

        if (actStack.empty()) {
            cout << "No activity yet\n";
            return;
        }

        // Simpan dulu ke vector agar bisa ditampilkan dari atas ke bawah
        stack<Act> tempStack = actStack;
        vector<Act> reverseList;
        while (!tempStack.empty()) {
            reverseList.push_back(tempStack.top());
            tempStack.pop();
        }

        cout << "Riwayat Aktivitas - @" << username << ":\n";
        int count = 0;
        for (auto it = reverseList.rbegin(); it != reverseList.rend() && count < 10; ++it, ++count) {
            cout << "[" << it->timestamp << "] ";
            switch (it->type) {
                case LIKE:
                    cout << "@" << username << " liked a post from @" << it->target_user << endl;
                    break;
                case COMMENT:
                    cout << "@" << username << " commented a post from @" << it->target_user << endl;
                    break;
                case CREATE:
                    cout << "@" << username << " made a post" << endl;
                    break;
            }
        }

        cout << "==============================================\n";
        cout << "[X] Exit\n";
        char choice;
        do {
            cout << ">> ";
            cin >> choice;
            choice = tolower(choice);
            if (choice != 'x') {
                cout << "Invalid input\n";
            }
        } while (choice != 'x');
    }
}
