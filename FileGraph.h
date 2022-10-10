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
    string type;
    string file_name;

    vector<FileNode*> prev_nodes;
    // int p_cnt;
    vector<FileNode*> next_nodes;
    FileHead* file_head;

    FileNode(string, string);  // 新增"删除"节点
    FileNode(string, string, string);
    FileNode(string, string, string, string);

    void link(FileNode*);

    void print();
};

extern unordered_map<string, FileNode*> file_nodes;  // 文件名 + blob_ish 作为 key
// extern unordered_map<string, FileNode*> deleted_files; // 被删除的文件







#endif