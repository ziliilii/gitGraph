#ifndef _FILEGRAPH_H_
#define _FILEGRAPH_H_

#include <vector>
#include <queue>
#include <string>
#include <unordered_map>
#include <unordered_set>



class FileNode;

class FileHead {
public:
    int size;
    std::vector<std::string> used_name;
    FileNode* head;
    

    FileHead(std::string, std::string, std::string);
    static void init(std::string, std::string, FileHead*);
};


class FileNode {
public:
    std::string commit_ish;
    std::string blob_ish;
    std::string type;
    std::string file_name;

    std::vector<FileNode*> prev_nodes;
    // int p_cnt;
    std::vector<FileNode*> next_nodes;
    FileHead* file_head;

    FileNode(std::string, std::string);  // 新增"删除"节点
    FileNode(std::string, std::string, std::string);
    FileNode(std::string, std::string, std::string, std::string);

    void link(FileNode*);

    void print();
};

extern std::unordered_map<std::string, FileNode*> file_nodes;  // 文件名 + blob_ish 作为 key
// extern unordered_map<string, FileNode*> deleted_files; // 被删除的文件







#endif