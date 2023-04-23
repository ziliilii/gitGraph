#include "FileGraph.h"

#include <iostream>




FileHead::FileHead(std::string file_name, std::string blob_ish, std::string commit_ish) {
    this->used_name.clear();
    this->used_name.push_back(file_name);
    // cout << this->used_name[0] << endl;
    this->size = 1;
    this->head = new FileNode(blob_ish, commit_ish, file_name);
    std::string name = file_name + ' ' + blob_ish;
}


FileNode::FileNode(std::string commit_ish, std::string file_name) {  // 新增"删除"节点
    this->commit_ish = commit_ish;
    this->blob_ish = "000000";
    this->file_name = file_name;
    this->prev_nodes.clear();
    this->next_nodes.clear();
    this->type = "d";
}

FileNode::FileNode(std::string blob_ish, std::string commit_ish, std::string file_name) {  // 新增修改节点
    this->commit_ish = commit_ish;
    this->blob_ish = blob_ish;
    this->file_name = file_name;
    this->prev_nodes.clear();
    this->next_nodes.clear();
    file_nodes[file_name + ' ' + blob_ish] = this;
}

FileNode::FileNode(std::string blob_ish, std::string commit_ish, std::string file_name, std::string type) {  // 新增复制 重命名的节点
    this->commit_ish = commit_ish;
    this->blob_ish = blob_ish;
    this->file_name = file_name;
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

std::unordered_map<std::string, FileNode*> file_nodes;  // 文件名 + blob_ish 作为 key
// unordered_map<string, vector<FileNode*>> deteled_files;





void FileNode::print() {
    std::cout << "打印file node基本信息\n";
    std::cout << "blob: " << this->blob_ish << std::endl;
    std::cout << "commit: " << this->commit_ish << std::endl;;
    std::cout << "file head: " << this->file_head << std::endl;
    std::cout << "file head's name: " << this->file_head->used_name[0];
    std::cout << "file name: " << this->file_name << std::endl;
    std::cout << "后续节点个数: " << this->next_nodes.size() << std::endl;
    for (auto& node: this->next_nodes) {
        std::cout << "后续节点文件名: " << node->file_name << "  后续节点blob: " << node->blob_ish << std::endl;
    }
    std::cout << "前置节点个数: " << this->prev_nodes.size() << std::endl;
    for (auto& node: this->prev_nodes) {
        std::cout << "前置节点文件名: " << node->file_name << "  前置节点blob: " << node->blob_ish << std::endl;
    }
    std::cout << "打印完毕\n";
}











