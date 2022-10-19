#ifndef _GITCOMMIT_H_
#define _GITCOMMIT_H_

#include "FileGraph.h"

#include <vector>
#include <unordered_map>
#include <iostream>
#include <string>
#include <sstream>



class GitCommit {
public:
    GitCommit();
    GitCommit(std::string);
    GitCommit(std::string, std::string, std::string, std::string, std::string);
    ~GitCommit() {};

    void add_tree(std::string);
    void add_author(std::string);
    void add_email(std::string);
    void add_date(std::string);
    void add_parents(std::string);
    void add_child(std::string);

    std::string commit_ish;  // 提交的hash id
    std::string tree_ish;     // 提交的tree对象
    //std::string type;       // 提交的类型
    std::string author;     // 提交的作者
    std::string email;      // 作者的邮箱
    std::string date;       // 提交的日期
    std::string commit_message;
    std::vector<std::string> tags;         // tag
    int pcnt;
    std::vector<GitCommit*> parents;      // 父提交

    std::vector<GitCommit*> children;     // 儿子
    std::vector<std::string> modified_file;

    static std::unordered_map<std::string, GitCommit*> commit_list;
    
    static std::unordered_map<std::string, FileHead*> file_list;   // 文件名 + blob_ish 作为 key
    static std::unordered_map<std::string, std::vector<FileHead*>> same_name_file;   // 文件名 作为 key
    

    void diff_parents(std::string&, int);
    void link_prev_node(std::string, std::string);
    void link_prev_node(std::string, std::string, std::string);
    void link_prev_node(std::string, std::string, std::string, std::string, std::string);
    void print();
    void print_file_list();

};



#endif