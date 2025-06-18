#ifndef FOLLOW_H
#define FOLLOW_H

#include <string>
#include <unordered_map>
#include <unordered_set>

extern std::unordered_map<std::string, std::unordered_set<std::string>> followGraph;

void loadFollowGraph(); 
void saveFollowGraph(); 

void followUser(const std::string& from, const std::string& to);     
void unfollowUser(const std::string& from, const std::string& to);   
bool isFollowing(const std::string& from, const std::string& to);    
void showFollowing(const std::string& user); 
void showFollowers(const std::string& user); 

#endif 