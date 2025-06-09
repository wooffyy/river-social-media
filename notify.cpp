#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <ctime>
#include <queue>
#include "notify.h"
#include "post.h"

using namespace std;

namespace Notify {

    // Simpan notifikasi ke file
    bool saveNotif(const string& username, const queue<Notif>& notifQueue) {
        string file_name = "users/" + username + "/notification.txt";
        ofstream file(file_name);
        if (!file) {
            cout << "Gagal membuka file notifikasi untuk menulis!\n";
            return false;
        }

        queue<Notif> temp = notifQueue;
        while (!temp.empty()) {
            Notif n = temp.front();
            temp.pop();
            file << n.message << "`" << n.timestamp << "`" << (n.is_checked ? "1" : "0") << "\n";
        }
        file.close();
        return true;
    }

    // Load notifikasi dari file
    bool loadNotif(const string& username, queue<Notif>& notifQueue) {
        notifQueue = queue<Notif>();
        string file_name = "users/" + username + "/notification.txt";
        ifstream file(file_name);
        if (!file) {
            // File tidak ada = tidak ada notifikasi
            return true;
        }

        string line;
        while (getline(file, line)) {
            if (line.empty()) continue;

            size_t pos1 = line.find('`');
            size_t pos2 = line.rfind('`');

            if (pos1 == string::npos || pos2 == string::npos || pos1 == pos2) continue;

            string msg = line.substr(0, pos1);
            string ts = line.substr(pos1 + 1, pos2 - pos1 - 1);
            string checkStr = line.substr(pos2 + 1);

            Notif n;
            n.message = msg;
            n.timestamp = ts;
            n.is_checked = (checkStr == "1");

            notifQueue.push(n);
        }

        file.close();
        return true;
    }

    // Tampilkan dan update status notifikasi
    void showNotif(const string& username) {
        queue<Notif> notifQueue;
        if (!loadNotif(username, notifQueue)) {
            cout << "Gagal memuat notifikasi.\n";
            return;
        }

        if (notifQueue.empty()) {
            cout << "No unread notifications.\n";
            return;
        }

        cout << "~~~ Notifications for @" << username << " ~~~\n";
        queue<Notif> updatedQueue;

        while (!notifQueue.empty()) {
            Notif n = notifQueue.front();
            notifQueue.pop();

            cout << (n.is_checked ? "  " : "* ") << n.message << " [" << n.timestamp << "]\n";
            n.is_checked = true; // tandai sudah dibaca
            updatedQueue.push(n);
        }

        cout << "============================\n";
        saveNotif(username, updatedQueue);
    }

    // Hitung jumlah notifikasi yang belum dibaca
    int countUnreadNotif(const string& username) {
        queue<Notif> notifQueue;
        if (!loadNotif(username, notifQueue)) return 0;

        int count = 0;
        queue<Notif> temp = notifQueue;
        while (!temp.empty()) {
            if (!temp.front().is_checked) count++;
            temp.pop();
        }
        return count;
    }

    // Tambah notif like
    void likesNotif(const string& fromUser, const string& toUser, int postID, const string& postContent) {
        if (fromUser == toUser) return;
        string shorten = postContent;
        if (shorten.length() > 20) {
            shorten = shorten.substr(0, 18) + "...";
        }

        Notif n;
        n.message = "@" + fromUser + " liked post: \"" + shorten + "\"";
        n.timestamp = River::getTime();
        n.is_checked = false;

        queue<Notif> notifQueue;
        loadNotif(toUser, notifQueue);
        notifQueue.push(n);
        saveNotif(toUser, notifQueue);
    }

    // Tambah notif komentar
    void commentNotif(const string& fromUser, const string& toUser, int postID, const string& postContent) {
        if (fromUser == toUser) return;
        string shorten = postContent;
        if (shorten.length() > 20) {
            shorten = shorten.substr(0, 18) + "...";
        }

        Notif n;
        n.message = "@" + fromUser + " commented on post: \"" + shorten + "\"";
        n.timestamp = River::getTime();
        n.is_checked = false;

        queue<Notif> notifQueue;
        loadNotif(toUser, notifQueue);
        notifQueue.push(n);
        saveNotif(toUser, notifQueue);
    }
    void followNotif(const string& fromUser, const string& toUser) {
        if (fromUser == toUser) return;

        Notif n;
        n.message = "@" + fromUser + " started following you.";
        n.timestamp = River::getTime();
        n.is_checked = false;

        queue<Notif> notifQueue;
        loadNotif(toUser, notifQueue);
        notifQueue.push(n);
        saveNotif(toUser, notifQueue);
    }
}
