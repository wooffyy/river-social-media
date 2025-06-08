#include "whirlpool.h"
#include "post.h" 
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <sstream>
#include <fstream>
#include <iostream>
#include <limits>

using namespace std;

struct AVLNode {
    int postID;
    AVLNode* left;
    AVLNode* right;
    int height;

    AVLNode(int id) : postID(id), left(nullptr), right(nullptr), height(1) {}
};

unordered_map<string, AVLNode*> hashtagIndex;
unordered_map<string, int> hashtagCount;

int height(AVLNode* N) {
    return N ? N->height : 0;
}

int getBalance(AVLNode* N) {
    return N ? height(N->left) - height(N->right) : 0;
}

AVLNode* rightRotate(AVLNode* y) {
    AVLNode* x = y->left;
    AVLNode* T2 = x->right;

    x->right = y;
    y->left = T2;

    y->height = max(height(y->left), height(y->right)) + 1;
    x->height = max(height(x->left), height(x->right)) + 1;

    return x;
}

AVLNode* leftRotate(AVLNode* x) {
    AVLNode* y = x->right;
    AVLNode* T2 = y->left;

    y->left = x;
    x->right = T2;

    x->height = max(height(x->left), height(x->right)) + 1;
    y->height = max(height(y->left), height(y->right)) + 1;

    return y;
}

AVLNode* insertAVL(AVLNode* node, int postID) {
    if (node == nullptr) return new AVLNode(postID);

    if (postID < node->postID)
        node->left = insertAVL(node->left, postID);
    else if (postID > node->postID)
        node->right = insertAVL(node->right, postID);
    else
        return node; // Duplicate postID not allowed

    node->height = 1 + max(height(node->left), height(node->right));

    int balance = getBalance(node);

    // Balancing
    if (balance > 1 && postID < node->left->postID)
        return rightRotate(node);

    if (balance < -1 && postID > node->right->postID)
        return leftRotate(node);

    if (balance > 1 && postID > node->left->postID) {
        node->left = leftRotate(node->left);
        return rightRotate(node);
    }

    if (balance < -1 && postID < node->right->postID) {
        node->right = rightRotate(node->right);
        return leftRotate(node);
    }

    return node;
}

void insertTag(const string& hashtag, int postID) {
    hashtagCount[hashtag]++;
    hashtagIndex[hashtag] = insertAVL(hashtagIndex[hashtag], postID);
}

void buildHashtagIndexFromPostData() {
    hashtagIndex.clear();
    hashtagCount.clear();

    ifstream file("post_data.txt");
    if (!file) return;

    string line;
    bool isFirstLine = true;
    while (getline(file, line)) {
        if (isFirstLine) {
            isFirstLine = false;
            continue;
        }
        if (line.empty()) continue;

        stringstream ss(line);
        string temp, username, content;
        int postID;

        getline(ss, temp, ',');
        postID = stoi(temp);
        getline(ss, username, ',');
        getline(ss, content, ',');

        // Parse hashtag dari content
        istringstream iss(content);
        string word;
        while (iss >> word) {
            if (word[0] == '#' && word.size() > 1) {
                string hashtag = word.substr(1);
                insertTag(hashtag, postID);
            }
        }
    }
    file.close();
}

int partition(vector<pair<string, int>>& vec, int low, int high) {
    int pivot = vec[high].second;
    int i = low - 1;

    for (int j = low; j <= high - 1; j++) {
        if (vec[j].second > pivot) { // descending
            i++;
            swap(vec[i], vec[j]);
        }
    }
    swap(vec[i + 1], vec[high]);
    return (i + 1);
}

void quickSortHashtag(vector<pair<string, int>>& vec, int low, int high) {
    if (low < high) {
        int pi = partition(vec, low, high);

        quickSortHashtag(vec, low, pi - 1);
        quickSortHashtag(vec, pi + 1, high);
    }
}

void inOrderTraversal(AVLNode* node, vector<int>& postIDs) {
    if (node == nullptr) return;
    inOrderTraversal(node->left, postIDs);
    postIDs.push_back(node->postID);
    inOrderTraversal(node->right, postIDs);
}

void showWhirlpoolMenu(const string& currentUsername) {
    while (true) {
        #ifdef _WIN32
            system("cls");
        #else
            system("clear");
        #endif

        cout << "(((((((WHIRLPOOL)))))))\n\n";

        // Build vector & sort Top 5
        vector<pair<string, int>> vec(hashtagCount.begin(), hashtagCount.end());
        if (!vec.empty()) {
            quickSortHashtag(vec, 0, vec.size() - 1);
        }

        int displayCount = min(5, (int)vec.size());
        for (int i = 0; i < displayCount; ++i) {
            cout << "[" << i + 1 << "] #" << vec[i].first << " - " << vec[i].second << " post\n";
        }

        cout << "---------------------------\n";
        cout << "[1-5] View Post\n";
        cout << "[F]   Find Specific Bait\n";
        cout << "[X]   Back\n";
        cout << ">> ";

        char choice;
        cin >> choice;
        choice = tolower(choice);
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (choice >= '1' && choice <= '5') {
            int index = choice - '1';
            if (index < displayCount) {
                string targetHashtag = vec[index].first;
                showPostByTag(currentUsername, targetHashtag);
            } else {
                cout << "Pilihan tidak valid.\n";
            }
        } else if (choice == 'f') {
            searchHashtagMenu(currentUsername);
        } else if (choice == 'x') {
            break;
        } else {
            cout << "Input tidak valid!\n";
        }
    }
}

void searchHashtagMenu(const string& currentUsername) {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif

    cout << "(((((((WHIRLPOOL)))))))\n";
    cout << "Enter a bait : ";
    string query;
    getline(cin, query);

    auto it = hashtagCount.find(query);
    if (it == hashtagCount.end()) {
        cout << "Tidak ada bait #" << query << endl;
        cout << "[Enter untuk kembali]"; cin.get();
        return;
    }
    cout << "Strike! Bait ditemukan : \n";
    cout << "\n#" << query << " - " << it->second << " post\n";
    cout << "----------------------------------\n";
    cout << "[V] View post          [X] Back\n";
    cout << ">> ";

    char choice;
    cin >> choice;
    choice = tolower(choice);
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    if (choice == 'v') {
        showPostByTag(currentUsername, query);
    }
}
