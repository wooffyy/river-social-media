#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <limits>
#include "account.h"
// Untuk membuat direktori baru
#ifdef _WIN32
    #include <direct.h>
    #define CREATE_DIRECTORY(dir) _mkdir(dir)
#else
    #include <sys/types.h>
    #include <sys/stat.h>
    #define CREATE_DIRECTORY(dir) mkdir(dir, 0777)
#endif

using namespace std;


namespace Account {
    // Inisialisasi vector untuk menyimpan daftar akun
    vector<User> userList;

    // Struct untuk node BST
    struct BSTNode {
        User user;
        BSTNode* left;
        BSTNode* right;
        
        BSTNode(const User& user) : user(user), left(nullptr), right(nullptr) {}
    };
    
    // Inisialisasi BST
    BSTNode* userTreeRoot = nullptr;
    
    // Memasukkan user ke BST
    void insertUserBST(const User& user) {
        BSTNode* newNode = new BSTNode(user);
        
        // Jika tree kosong, node baru menjadi root
        if (userTreeRoot == nullptr) {
            userTreeRoot = newNode;
            return;
        }
        
        BSTNode* current = userTreeRoot;
        BSTNode* parent = nullptr;
        
        // Memasukkan user ke subtree kanan atau kiri
        while (true) {
            parent = current;
            
            if (user.username < current->user.username) {
                current = current->left;
                if (current == nullptr) {
                    parent->left = newNode;
                    return;
                }
            } else {
                current = current->right;
                if (current == nullptr) {
                    parent->right = newNode;
                    return;
                }
            }
        }
    }
    
    // Mencari node user dengan BST
    BSTNode* searchUserBST(const string& username) {
        // Inisialisasi pointer current 
        BSTNode* current = userTreeRoot;
        
        // Menelusuri tree
        while (current != nullptr) {
            if (current->user.username == username) {
                return current; 
            } else if (username < current->user.username) {
                current = current->left;
            } else {
                current = current->right;
            }
        }
        
        return nullptr; // Jika tidak ditemukan
    }
    
    // Melakukan in-order traversal
    void inOrderTraversal(BSTNode* node, vector<User>& result) {
        if (node == nullptr) return;
        
        inOrderTraversal(node->left, result);
        result.push_back(node->user);
        inOrderTraversal(node->right, result);
    }
    
    // Membuat tree BST dari vector userList
    void buildBSTFromVector() {
        userTreeRoot = nullptr;
        
        // Memasukkan user ke BST
        for (const User& user : userList) {
            insertUserBST(user);
        }
    }
    
    // Mencari username menggunakan BST
    int findUserBST(const string& username) {
        BSTNode* result = searchUserBST(username);
        
        // Jika user telah ditemukan, cari indeks di userList
        if (result != nullptr) {
            for (size_t i = 0; i < userList.size(); i++) {
                if (userList[i].username == username) {
                    return i;
                }
            }
        }
        
        return -1; // Jika user tidak ditemukan
    }
    
    // Membersihkan alokasi memori BST
    void cleanupBST() {
        deleteBST(userTreeRoot);
        userTreeRoot = nullptr;
    }

    // Menghapus semua node BST
    void deleteBST(BSTNode* node) {
        if (node == nullptr) return;
        
        // Melakukan penghapusan secara post-order traversal
        deleteBST(node->left);
        deleteBST(node->right);
        delete node;
    }
    
    // Menghubungkan daftar user dari vector yang akan diubah ke bentuk BST 
    void loadUsersWithBST() {
        loadUsers(); 
        buildBSTFromVector(); 
    }
    
    // Membuat direktori untuk user baru
    bool create_user_dir(const string& username){
        CREATE_DIRECTORY("users");

        string userDIR = "users/" + username;
        int result = CREATE_DIRECTORY(userDIR.c_str());
        if(result != 0) {
            cout << "ERROR! \n";
            return false;
        } 

        // Membuat file post pribadi
        ofstream postFile(userDIR + "/posts.csv", ios::app);
        if (!postFile) {
            cout << "Gagal membuat file posts.csv untuk user " << username << "!\n";
            return false;
        }
        postFile.close();

        // Membuat file penyimpanan notifikasi pribadi
        ofstream notifFile(userDIR + "/notification.txt", ios::app);
        if (!notifFile) {
            cout << "Gagal membuat file notification.txt untuk user " << username << "!\n";
            return false;
        }
        notifFile.close();

        // Membuat file penyimpanan riwayat aktivitas pribadi
        ofstream actFile(userDIR + "/activity_stack.txt", ios::app);
        if (!actFile) {
            cout << "Gagal membuat file activity_stack.txt untuk user " << username << "!\n";
            return false;
        }
        actFile.close();
        cout << username << " berhasil dibuat!\n";
        return true;
    }
    
    // Memuat daftar user kemudian disimpan di vector userList
    void loadUsers() {
        userList.clear();
        ifstream file("user_data.txt");
        string line;
        if(!file) {
            cout << "no user\n";
            ofstream createFile("user_data.txt");
            createFile.close();
            return;
        }

        while (getline(file, line)) {
            stringstream ss(line);
            User u;
            ss >> u.id >> u.username >> u.password;
            userList.push_back(u);
        }
        file.close();

        sort(userList.begin(), userList.end(), compareUsers);
    }

    // Menyimpan daftar user (userList) ke dalam file user_data
    void saveUsers() {
        ofstream file("user_data.txt");
        if (!file) {
            cout << "Gagal membuka file user_data.txt untuk menulis!\n";
            return;
        }

        for (auto& u : userList) {
            file << u.id << " " << u.username << " " << u.password << "\n";
        }
        file.close();
    }

    // Melakukan binary search (tanpa tree) pada vector 
    int binarySearchUser(const string& username) {
        int left = 0, right = userList.size() - 1;
        while (left <= right) {
            int mid = (left + right) / 2;
            if (userList[mid].username == username) return mid;
            else if (userList[mid].username < username) left = mid + 1;
            else right = mid - 1;
        }
        return -1;
    }

    // Logika pengurutan user
    bool compareUsers(const User& a, const User& b) {
        return a.username < b.username;
    }

    // Fungsi untuk mengatur alur dan kerja fitur Sign Up
    void signUp() {
        // Alokasi memori
        User newUser;
        cout << "\n=== Registrasi Akun ===\n";
        cout << "Username: ";
        cin >> newUser.username;

        // Cek apakah username sudah digunakan
        if (binarySearchUser(newUser.username) != -1) {
            cout << "Username sudah digunakan.\n";
            return;
        }
    
        cout << "Password: ";
        cin >> newUser.password;
        
        // Membuat id untuk user baru
        if (userList.empty()) {
            newUser.id = 1;
        } else {
            int lastId = 0;
            for (const auto& user : userList){
                if (user.id > lastId) {
                    lastId = user.id;
                }
            }
            newUser.id = lastId + 1;
        }

        // Menambahkan user baru ke dalam vector
        userList.push_back(newUser);
        
        // Mengurutkan user berdasarkan urutan abjad (A-Z)
        sort(userList.begin(), userList.end(), compareUsers);
        // Memasukkan useer baru ke dalam tree
        insertUserBST(newUser);
        // Simpan user ke dalam file 
        saveUsers();

        if (create_user_dir(newUser.username)) {
            cout << "Akun berhasil dibuat! Halo @" << newUser.username << endl;
        } else {
            cout << "Gagal membuat akun. Silakan coba lagi.\n";
        }
    }

    // Fungsi untuk mengatur alur dan kerja fitur Login
    User* login() {
        string username, password;
        cout << "\n=== Login ===\n";
        cout << "Username: ";
        cin >> username;
        // Mencari username
        int idx = binarySearchUser(username);
        if (idx == -1) {
            cout << "Username tidak ditemukan.\n";
            return nullptr;
        }
        cout << "Password: ";
        cin >> password;
        if (userList[idx].password != password) {
            cout << "Password salah.\n";
            return nullptr;
        }
        cout << "Login berhasil! Selamat datang, " << userList[idx].username << "\n";
        return &userList[idx];
    }

    // Template function untuk hitung statistik akun
    template <typename T>
    T countStats(const string& username, const string& statType);

    // Template specialization dengan tipe data int
    template <>
    int countStats<int>(const string& username, const string& statType) {
        // Menghitung likes
        if (statType == "likes") {
            int totalLikes = 0;
            ifstream file("users/" + username + "/posts.csv");
            if (!file) {
                return 0;
            }
            
            string line;
            while (getline(file, line)) {
                stringstream ss(line);
                string id_str, user, content, likes_str;
                getline(ss, id_str, ',');
                getline(ss, user, ','); 
                getline(ss, content, ',');
                getline(ss, likes_str, ',');
                totalLikes += stoi(likes_str);
            }
            file.close();
            return totalLikes;
        }
        return 0;
    }
    
    // Template specialization dengan tipe data double
    template <>
    double countStats<double>(const string& username, const string& statType) {
        // Menghitung total post
        if (statType == "posts") {
            double totalPosts = 0;
            ifstream file("users/" + username + "/posts.csv");
            if (!file) {
                return 0;
            }

            string line;
            while (getline(file, line)) {
                totalPosts++;
            }
            file.close();
            return totalPosts;
        }
        return 0;
    }

    // Mencari akun user lain menggunakan BST
    void searchAccount(const string& username) {
        int idx = findUserBST(username);
        if (idx == -1) {
            cout << "Akun tidak ditemukan.\n";
            return;
        }
        cout << "Akun ditemukan: \n";
        cout << "Username: " << userList[idx].username << "\n";
        cout << endl;
        ifstream file("users/" + userList[idx].username + "/posts.csv");
        if (!file) {
            return;
        }
        cout << "====== Riwayat Post ======\n";
        string line;
        while (getline(file, line)) {
            stringstream ss(line);
            string id_str, username, content, likes_str;
            getline(ss, id_str, ',');
            getline(ss, username, ',');
            getline(ss, content, ',');
            getline(ss, likes_str, ',');
            cout << "[" << id_str << "]  " << content << " [Likes: " << likes_str << "]" << endl;
        }
        file.close();
    }

    // Fungsi untuk mengatur alur dan kerja fitur Profile Page
    void profilePage(const User& user) {
        int choice;
        do {
            cout << "\n=== Profil Anda ===\n";
            cout << "1. Riwayat Post" << endl;
            cout << "2. Ubah Password" << endl;
            cout << "3. Statistik Akun" << endl;
            cout << "0. Kembali" << endl;
            cout << ">>  ";
            if (!(cin >> choice)) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Invalid input. Please enter a number.\n";
                continue;
            }

            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            #ifdef _WIN32
                system("cls");
            #else
                system("clear");
            #endif


            if (choice == 1){
                // Menampilkan riwayat post
                ifstream file("users/" + user.username + "/posts.csv");
                if (!file) {
                    cout << "Gagal membuka file posts.csv untuk user " << user.username << "!\n";
                    return;
                }
                    cout << "====== Riwayat Post ======\n";
                    string line;
                    while (getline(file, line)) {
                        stringstream ss(line);
                        string id_str, username, content, likes_str;
                        getline(ss, id_str, ',');
                        getline(ss, username, ',');
                        getline(ss, content, ',');
                        getline(ss, likes_str, ',');
                        cout << "[" << id_str << "] Content: " << content << " [Likes: " << likes_str << "]" << endl;
                    }
                file.close();
            } else if (choice == 2) {
                // Mengubah password
                string newPassword;
                cout << "Masukkan password baru: ";
                cin >> newPassword;
                userList[binarySearchUser(user.username)].password = newPassword;
                saveUsers();
                cout << "Password berhasil diubah!\n";
            } else if (choice == 3) {
                // Melihat statistik akun
                double totalPosts = countStats<double>(user.username, "posts");
                int totalLikes = countStats<int>(user.username, "likes");
                cout << "=== Statistik Akun ===\n";
                cout << "Total Post: " << totalPosts << endl;
                cout << "Total Likes: " << totalLikes << endl;
            } else if (choice == 0) {
                cout << "Kembali ke menu utama.\n";
                return;
            } else {
                cout << "Pilihan tidak valid. Silakan coba lagi.\n";
            }
        } while(choice != 4);

    }
}
