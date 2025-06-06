#include <iostream>
#include <limits>
#include <string>
#include "post.h"
#include "account.h"
#include "notify.h"
#include "activity.h"
#ifdef _WIN32
    #include <direct.h>
    #define CREATE_DIRECTORY(dir) _mkdir(dir)
#else
    #include <sys/types.h>
    #include <sys/stat.h>
    #define CREATE_DIRECTORY(dir) mkdir(dir, 0777)
#endif

using namespace std;

// Namespace untuk menu
namespace Menu {
    void showNotifications(const string& username) {
        Notify::showNotif(username);
    }
    
    void showActivities(const string& username) {
        Activity::showActivity(username);
    }
    
    void createPost(const string& username) {
        Post* newPost = River::createPost(username);
        
        if (newPost != nullptr) {
            
            Activity::recordPost(username, newPost->id, username);
            delete newPost;
        }
    }

    void profilePage(const string& username) { 
        int idx = Account::findUserBST(username);
        if (idx == -1) {
            cout << "Akun tidak ditemukan.\n";
            return;
        }
        Account::profilePage(Account::userList[idx]);
    }
    
    void showFeed(const string& username) {
        River::showFeed(username);
    }
    
    // Fungsi untuk main menu 
    void userMenu(const string& username) {
        int choice;
        bool logout = false;
        string query;
        
        do {
            cout << "\n=== RIVER - @" << username << " ===\n";
            cout << "1. Create Post\n";
            cout << "2. View Feed\n";
            cout << "3. Search User\n";
            cout << "4. Notifications\n";
            cout << "5. Activity History\n";
            cout << "6. Profile Page\n";
            cout << "0. Logout\n";
            cout << ">> ";
            
            if (!(cin >> choice)) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << ">> " << "\n";
                continue;
            }
            
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            
            if (choice < 0 || choice > 6) {
                cout << ">> " << "\n";
                continue;
            }
            
            system("cls");

            switch (choice) {
                case 1:
                    createPost(username);
                    break;
                    
                case 2:
                    showFeed(username);
                    break;
                
                case 3:
                    cout << "Enter username: ";
                    getline(cin, query);
                    Account::searchAccount(query);
                    break;
                    
                case 4:
                    showNotifications(username);
                    break;
                    
                case 5:
                    showActivities(username);
                    break;
                    

                case 6:
                    profilePage(username);
                    break;

                case 0:
                    cout << "Logging out...\n";
                    logout = true;
                    break;
                    
                default:
                    cout << "Invalid choice. Please try again.\n";
                    break;
            }
        } while (!logout);
    }
    
    // Fungsi untuk menampilkan menu login
    void loginMenu() {
        int choice;
        bool exit = false;
        
        do {
            cout << "\n=== WELCOME TO RIVER ===\n";
            cout << "1. Login\n";
            cout << "2. Sign Up\n";
            cout << "0. Exit\n";
            cout << ">> ";
            
            if (!(cin >> choice)) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Invalid input. Please enter a number.\n";
                continue;
            }
            
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            if (choice < 0 || choice > 2) {
                cout << "Invalid choice. Please try again.\n";
                continue;
            }

            system("cls");
            
            switch (choice) {
                case 1: {
                    User* currentUser = Account::login();
                    if (currentUser) {
                        userMenu(currentUser->username);
                    }
                    break;
                }
                
                case 2:
                    Account::signUp();
                    break;
                    
                case 0:
                    cout << "Thank you for using River. Goodbye!\n";
                    exit = true;
                    break;
                    
                default:
                    cout << "Invalid choice. Please try again.\n";
                    break;
            }
        } while (!exit);
    }
}

int main() {
    // Membuat direktori users jika belum ada
    CREATE_DIRECTORY("users");

    // Memuat user kemudian membuat BST
    Account::loadUsersWithBST();

    // Memanggil menu login
    Menu::loginMenu();

    // Membersihkan alokasi memori BST
    Account::cleanupBST();
    
    return 0;
}