#ifndef NOTIFY_H
#define NOTIFY_H

#include <string>
#include <queue>

using namespace std;

// Struktur notifikasi modern
struct Notif {
    string message;
    string timestamp;
    bool is_checked; // false = belum dibaca, true = sudah dibaca
};

namespace Notify {
    // Penyimpanan dan pemuatan notifikasi
    bool saveNotif(const string& username, const queue<Notif>& notifQueue);
    bool loadNotif(const string& username, queue<Notif>& notifQueue);

    // Fungsi notifikasi untuk aktivitas
    void likesNotif(const string& fromUser, const string& toUser, int postID, const string& postContent);
    void commentNotif(const string& fromUser, const string& toUser, int postID, const string& postContent);

    // Tampilan dan statistik
    void showNotif(const string& username);
    int countUnreadNotif(const string& username); // baru
    void followNotif(const string& fromUser, const string& toUser);

}

#endif
