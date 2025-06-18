#pragma once
#include <string>
#include <vector>
#include <unordered_map>
using namespace std;

struct AVLNode;

void insertTag(const string& hashtag, int postID);
void buildHashtagIndexFromPostData();
void showWhirlpoolMenu(const string& currentUsername);
void searchHashtagMenu(const string& currentUsername); 
void inOrderTraversal(AVLNode* node, vector<int>& postIDs);

extern unordered_map<string, int> hashtagCount;
extern unordered_map<string, struct AVLNode*> hashtagIndex;
