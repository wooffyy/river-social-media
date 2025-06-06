#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <ctime>
#include <limits>
#include "notify.h"
#include "post.h"

using namespace std;

namespace Notify {
    // Insialisasi queue
    void initQueue(NotifQueue* queue) {
        queue->front = nullptr;
        queue->rear = nullptr;
    }

    // Melakukan enqueue (Menambah queue dengan notifikasi baru)
    void enqueue(NotifQueue* queue, const string& message, const string& timestamp){
        // Alokasi memori notifikasi baru
        Notif* newNotif = new Notif();
        newNotif->message = message;     
        newNotif->timestamp = timestamp; 
        newNotif->next = nullptr;       
        
        if (isEmpty(queue)) {
            queue->front = queue->rear = newNotif;
        } else {
            queue->rear->next = newNotif;
            queue->rear = newNotif;
        }
    }

    // Melakukan dequeue
    bool dequeue(NotifQueue* queue, string& message, string& timestamp){
        if (isEmpty(queue)) {
            return false;
        }

        // Inisialisasi pointer temp
        Notif* temp = queue->front;
        message = temp->message;
        timestamp = temp->timestamp;

        queue->front = queue->front->next;
        if (queue->front == nullptr) {
            queue->rear = nullptr;
        }

        // Menghapus notifikasi di temp
        delete temp;
        return true;
    }

    // Cek apakah queue kosong
    bool isEmpty(NotifQueue* queue) {
        return queue->front == nullptr;
    }

    // Menyimpan notifikasi ke file notifikasi pribadi milik user
    bool saveNotif(const string& username, NotifQueue* queue) {
        string file_name = "users/" + username + "/notification.txt";
        ofstream file(file_name); 

        if (!file) {
            cout << "Gagal membuka file untuk menulis!\n";
            return false;
        }
        
        // Menulis queue ke file notifikasi
        Notif* current = queue->front;
        while (current != nullptr) {
            file << current->message << "`" << current->timestamp << "\n";
            current = current->next;
        }
        file.close();
        return true;
    }

    // Mengambil notifikasi dari file untuk disimpan ke queue kembali
    bool loadNotif(const string& username, NotifQueue* queue){
        string file_name = "users/" + username + "/notification.txt";
        ifstream file(file_name);

        if (!file) {
            cout << "Belum ada notifikasi\n";
            return true;
        }

        string msg, ts;
        while(dequeue(queue,msg,ts)){}

        string line;
        while (getline(file, line)) {
            if(line.empty()) continue;
            
            size_t pos = line.find('`');
            if (pos != string::npos) {
                string message = line.substr(0, pos);
                string timestamp = line.substr(pos + 1);
                enqueue(queue, message, timestamp);
            }
        }
        file.close();
        return true;
    }

    // Membuat notifikasi untuk aktivitas like
    void likesNotif(const string& fromUser, const string& toUser, int postID, const string& postContent) {
        // Memendekkan isi post untuk tampilan pesan di notifikasi
        if (fromUser == toUser) return;
            string shorten = postContent;
        if (shorten.length() > 20) {
            shorten = shorten.substr(0, 18) + "...";
        }

        string msg = "@" + fromUser + " memberi like di post: \"" + shorten + "\"";
        string timestamp = River::getTime();

        // Membuat queue notifikasi 
        NotifQueue queue;
        initQueue(&queue);
        loadNotif(toUser, &queue);
        enqueue(&queue, msg, timestamp);
        saveNotif(toUser, &queue);
    }

    // Membuat notifikasi untuk aktivitasi comment
    void commentNotif(const string& fromUser, const string& toUser, int postID, const string& postContent) {
        // Memendekkan isi post untuk tampilan pesan di notifikasi
        if (fromUser == toUser) return;
        string shorten = postContent;
        if (shorten.length() > 20) {
            shorten = shorten.substr(0, 18) + "...";
        }

        string msg = "@" + fromUser + " memberi komentar di post: \"" + shorten + "\"";
        string timestamp = River::getTime();

        // Membuat queue notifikasi
        NotifQueue queue;
        initQueue(&queue);
        loadNotif(toUser, &queue);
        enqueue(&queue, msg, timestamp);
        saveNotif(toUser, &queue);
    }

    // Fungsi untuk membuat tampilan notifikasi
    void showNotif(const string& username) {
        NotifQueue queue;
        initQueue(&queue);

        if(!loadNotif(username, &queue)) {
            cout << "Gagal memuat notifikasi.\n";
            return;
        }
        if(isEmpty(&queue)) {
            cout << "Tidak ada notifikasi baru.\n";
            return;
        }

        cout << "=== Notifikasi ===\n";
        Notif* current = queue.front;
        int count = 1;
        while (current != nullptr) {
            cout << current->message << " [" << current->timestamp << "]\n";
            current = current->next;
            count++;
        }
        cout << "===================\n";

        string notif_msg, notif_ts;
        while(dequeue(&queue, notif_msg, notif_ts)) {}
    }
}