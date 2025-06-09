#include <iostream>
#include <limits>
#include <string>
#include "post.h"
#include "account.h"
#include "notify.h"
#include "follow.h"
#include "activity.h"
#include "whirlpool.h"

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

    void searchPageMenu(const string& currentUsername, const string& targetUsername) {
        int idx = Account::findUserBST(targetUsername);
        if (idx == -1) {
            cout << "User not found.\n";
            return;
        }

        while (true) {
            #ifdef _WIN32
                system("cls");
            #else
                system("clear");
            #endif

            cout << targetUsername << "\n";
            cout << "1. View post\n";
            cout << "2. " << (isFollowing(currentUsername, targetUsername) ? "Unfollow" : "Follow") << "\n";
            cout << "0. Back\n";
            cout << ">> ";

            int choice;
            if (!(cin >> choice)) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                continue;
            }
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            if (choice == 1) {
                River::showFeedForUser(currentUsername, targetUsername);
            } else if (choice == 2) {
                if (currentUsername == targetUsername) {
                    cout << "You can't follow yourself :)\n";
                } else if (isFollowing(currentUsername, targetUsername)) {
                    unfollowUser(currentUsername, targetUsername);
                } else {
                    followUser(currentUsername, targetUsername);
                }
            } else if (choice == 0) {
                break;
            } else {
                cout << "Invalid choice.\n";
            }
        }
    }

    void profilePageMenu(const string& currentUsername) {
        while (true) {
        #ifdef _WIN32
            system("cls");
        #else
            system("clear");
        #endif

        // Hitung followers & following & post count
        int followers = 0, following = 0;
            for (const auto& pair : followGraph) {
                if (pair.second.find(currentUsername) != pair.second.end()) {
                    followers++;
                }
                if (pair.first == currentUsername) {
                    following = pair.second.size();
                }
            }

            double postCount = Account::countStats<double>(currentUsername, "posts");

            // Tampilkan header
            cout << currentUsername << "         " << followers << " Follower       " << following << " Following         " << postCount << " Posts\n";
            cout << endl;
            cout << "~ " << Account::userList[Account::binarySearchUser(currentUsername)].bio << "\n";
            cout << "--------------------------------------------------------\n";

            // Menu
            cout << "1. View Your Post \n";
            cout << "2. Edit Your Profile \n";
            cout << "3. Friend Suggestion\n";
            cout << "0. Back\n";
            cout << ">> ";

            int choice;
            if (!(cin >> choice)) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                continue;
            }
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            if (choice == 1) {
                // View post milik sendiri
                River::showFeedForUser(currentUsername, currentUsername);
            } else if (choice == 2) {
                // Edit Profile
                int editChoice;
                cout << "Edit Profile:\n";
                cout << "1. Edit Password\n";
                cout << "2. Edit Bio\n";
                cout << "0. Back\n";
                cout << ">> ";

                if (!(cin >> editChoice)) {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    continue;
                }
                cin.ignore(numeric_limits<streamsize>::max(), '\n');

                if (editChoice == 1) {
                    cout << "Insert new password (8-20 characters): ";
                    string newPassword = Account::inputPasswordHidden();
                    if (newPassword.length() < 8 || newPassword.length() > 20) {
                        cout << "Password must be between 8 and 20 characters.\n";
                        continue;
                    }
                    // Update user_data.txt
                    int idx = Account::binarySearchUser(currentUsername);
                    Account::userList[idx].password = newPassword;    
                    Account::saveUsers();

                    cout << "Fresh from the oven!\n";   
                    
                } else if (editChoice == 2) {
                    cout << "Bio Anda: " << Account::userList[Account::binarySearchUser(currentUsername)].bio << "\n";
                    cout << "Insert new bio (max 100 characters): \n";
                    string newBio;
                    getline(cin, newBio);
                    if (newBio.empty() || newBio == "Empty") {
                        newBio = "Empty";
                        cout << "Reset bio to'Empty'.\n";
                    } else if (newBio.length() > 100) {
                        cout << "Bio must be less than 100 characters.\n";
                        continue;
                    }
                    // Update user_data.txt
                    int idx = Account::binarySearchUser(currentUsername);
                    Account::userList[idx].bio = newBio;    
                    Account::saveUsers();
                    
                    cout << "Fresh from the oven!\n";

                } else if (editChoice == 0) {
                    continue;
                }
            } else if (choice == 3) {
                // Friend Suggestion (BFS)
                cout << "Friend Suggestion:\n";
                unordered_set<string> visited;
                queue<string> q;
                q.push(currentUsername);
                visited.insert(currentUsername);

                vector<string> suggestions;
                while (!q.empty() && suggestions.size() < 5) {
                    string user = q.front(); q.pop();
                    auto it = followGraph.find(user);
                    if (it != followGraph.end()) {
                        for (const string& neighbor : it->second) {
                            if (visited.find(neighbor) == visited.end()) {
                                visited.insert(neighbor);
                                q.push(neighbor);
                                if (!isFollowing(currentUsername, neighbor)) {
                                    suggestions.push_back(neighbor);
                                    if (suggestions.size() >= 5) break;
                                }
                            }
                        }
                    }
                }

                if (suggestions.empty()) {
                    cout << "No suggestions.\n";
                } else {
                    cout << "Friend Suggestions:\n";
                    for (const string& s : suggestions) {
                        cout << "- @" << s << "\n";
                    }
                }

                cout << "[Enter to continue]"; cin.get();
            } else if (choice == 0) {
                break;
            } else {
                cout << "Invalid choice.\n";
            }
        }   
    }

    
    void showFeed(const string& username) {
        River::showFeed(username);
    }
    
    // Fungsi untuk main menu 
    void userMenu(const string& username) {
        int unread = Notify::countUnreadNotif(username);
        int choice;
        bool logout = false;
        string query;

        do {
            cout << "\n~~~~~~~~~~~~~~~~~~~~";
            cout << "\nRIVER - @" << username;
            cout << "\n~~~~~~~~~~~~~~~~~~~~\n";
            cout << "1. Create Post\n";
            cout << "2. View Feed\n";
            cout << "3. Search User\n";
            cout << "4. Notifications (" << unread << ")\n";
            cout << "5. Activity History\n";
            cout << "6. Profile Page\n";
            cout << "7. Whirlpool\n"; 
            cout << "0. Logout\n";
            cout << ">> ";

            if (!(cin >> choice)) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                continue;
            }

            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            if (choice < 0 || choice > 7) {
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
                    searchPageMenu(username, query); 
                    break;

                case 4:
                    showNotifications(username);
                    break;

                case 5:
                    showActivities(username);
                    break;

                case 6:
                    profilePageMenu(username);
                    break;

                case 7:
                    showWhirlpoolMenu(username); 
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

    void loginMenu() {
        int choice;
        bool exit = false;
        
        do {
            cout << "\n~~~~~~~~~~~~~~~~~~~~~";
            cout << "\n~~~     RIVER     ~~~";
            cout << "\n~~~~~~~~~~~~~~~~~~~~~\n";
            cout << "Welcome to the RIVER!\n";
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

    buildHashtagIndexFromPostData();

    // Memanggil menu login
    Menu::loginMenu();

    // Membersihkan alokasi memori BST
    Account::cleanupBST();
    
    return 0;
}