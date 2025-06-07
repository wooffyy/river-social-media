#include "follow.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace std;

// Struktur data global
unordered_map<string, unordered_set<string>> followGraph;

// Load follow graph dari file
void loadFollowGraph() {
    followGraph.clear();
    ifstream file("follow_data.txt");
    if (!file) return;

    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;

        size_t colon = line.find(':');
        if (colon == string::npos) continue;

        string user = line.substr(0, colon);
        stringstream ss(line.substr(colon + 1));
        string follower;

        while (getline(ss, follower, ',')) {
            if (!follower.empty()) {
                followGraph[user].insert(follower);
            }
        }
    }
    file.close();
}

// Simpan follow graph ke file
void saveFollowGraph() {
    ofstream file("follow_data.txt");
    if (!file) {
        cout << "Gagal menyimpan follow data!\n";
        return;
    }

    for (const auto& pair : followGraph) {
        file << pair.first << ":";
        bool first = true;
        for (const auto& following : pair.second) {
            if (!first) file << ",";
            file << following;
            first = false;
        }
        file << "\n";
    }
    file.close();
}

// Tambah follow
void followUser(const string& from, const string& to) {
    if (from == to) {
        cout << "Tidak bisa follow diri sendiri.\n";
        return;
    }

    if (isFollowing(from, to)) {
        cout << "Kamu sudah follow @" << to << ".\n";
        return;
    }

    followGraph[from].insert(to);
    saveFollowGraph();
    cout << "Kamu sekarang mengikuti @" << to << "!\n";
}

// Unfollow user
void unfollowUser(const string& from, const string& to) {
    if (!isFollowing(from, to)) {
        cout << "Kamu tidak mengikuti @" << to << ".\n";
        return;
    }

    followGraph[from].erase(to);
    saveFollowGraph();
    cout << "Berhasil unfollow @" << to << ".\n";
}

// Cek apakah from mengikuti to
bool isFollowing(const string& from, const string& to) {
    auto it = followGraph.find(from);
    if (it == followGraph.end()) return false;
    return it->second.find(to) != it->second.end();
}

// Tampilkan daftar yang di-follow oleh user
void showFollowing(const string& user) {
    auto it = followGraph.find(user);
    if (it == followGraph.end() || it->second.empty()) {
        cout << "Kamu belum mengikuti siapa pun.\n";
        return;
    }

    cout << "Kamu mengikuti:\n";
    for (const string& name : it->second) {
        cout << "- @" << name << "\n";
    }
}

// Tampilkan siapa saja yang follow user ini
void showFollowers(const string& user) {
    vector<string> followers;
    for (const auto& pair : followGraph) {
        if (pair.second.find(user) != pair.second.end()) {
            followers.push_back(pair.first);
        }
    }

    if (followers.empty()) {
        cout << "Belum ada yang mengikuti kamu.\n";
        return;
    }

    cout << "Kamu diikuti oleh:\n";
    for (const string& name : followers) {
        cout << "- @" << name << "\n";
    }
}
