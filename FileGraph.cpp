#include "FileGraph.h"

#include <iostream>

using namespace std;



FileHead::FileHead(string file_name, string blob_ish, string commit_ish) {
    this->used_name.clear();
    this->used_name.push_back(file_name);
    // cout << this->used_name[0] << endl;
    this->size = 1;
    this->head = new FileNode(blob_ish, commit_ish, file_name);
    string name = file_name + ' ' + blob_ish;
}


FileNode::FileNode(string commit_ish) {
    this->commit_ish = commit_ish;
    this->prev_nodes.clear();
    this->next_nodes.clear();
    this->type = "d";
}

FileNode::FileNode(string blob_ish, string commit_ish, string file_name) {
    this->commit_ish = commit_ish;
    this->blob_ish = blob_ish;
    this->prev_nodes.clear();
    this->next_nodes.clear();
    file_nodes[file_name + ' ' + blob_ish] = this;
}

FileNode::FileNode(string blob_ish, string commit_ish, string file_name, string type) {
    this->commit_ish = commit_ish;
    this->blob_ish = blob_ish;
    this->type = type;
    this->prev_nodes.clear();
    this->next_nodes.clear();
    file_nodes[file_name + ' ' + blob_ish] = this;
}


void FileNode::link(FileNode* prev_file) {
    prev_file->next_nodes.push_back(this);
    this->prev_nodes.push_back(prev_file);
    this->file_head = prev_file->file_head;
}

unordered_map<string, FileNode*> file_nodes;  // 文件名 + blob_ish 作为 key
// unordered_map<string, vector<FileNode*>> deteled_files;










