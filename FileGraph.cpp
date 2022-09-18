#include "FileGraph.h"
#include <iostream>

using namespace std;



FileHead::FileHead(string file_name, string blob_ish, string commit_ish) {
    this->used_name.clear();
    this->used_name.push_back(file_name);
    // cout << this->used_name[0] << endl;
    this->size = 1;
    this->head = new FileNode(blob_ish, commit_ish);
    string name = file_name + ' ' + blob_ish;
}


FileNode::FileNode(string blob_ish, string commit_ish) {
    this->commit_ish = commit_ish;
    this->blob_ish = blob_ish;
    this->prev_nodes.clear();
    this->next_nodes.clear();
}











