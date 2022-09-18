#ifndef _FILEGRAPH_H_
#define _FILEGRAPH_H_

#include <vector>
#include <string>
#include <unordered_map>

using namespace std;

class FileNode;

class FileHead {
public:
    int size;
    vector<string> used_name;
    FileNode* head;

    FileHead(string, string, string);
    static void init(string, string, FileHead*);
};


class FileNode {
public:
    string commit_ish;
    string blob_ish;

    vector<FileNode*> prev_nodes;
    vector<FileNode*> next_nodes;
    FileHead* file_head;

    FileNode(string, string);
};







#endif