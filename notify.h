#ifndef NOTIFY_H
#define NOTIFY_H
#include <iostream>
#include <string>
using namespace std;

// Inisialisasi struct notifikasi
struct Notif {
    string message;
    string timestamp;
    Notif* next;
};

struct NotifQueue {
    Notif* front;
    Notif* rear;
};

namespace Notify {
    void initQueue(NotifQueue* queue);
    void enqueue(NotifQueue* queue, const string& message, const string& timestamp);
    bool dequeue(NotifQueue* queue, string& message, string& timestamp);
    bool isEmpty(NotifQueue* queue);
    bool saveNotif(const string& username, NotifQueue* queue);
    bool loadNotif(const string& username, NotifQueue* queue);
    void likesNotif(const string& fromUser, const string& toUser, int postID, const string& postContent);
    void commentNotif(const string& fromUser, const string& toUser, int postID, const string& postContent);
    void showNotif(const string& username);
}




#endif