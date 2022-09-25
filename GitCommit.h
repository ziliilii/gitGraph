#ifndef _GITCOMMIT_H_
#define _GITCOMMIT_H_

#include "FileGraph.h"

#include <vector>
#include <unordered_map>
#include <iostream>
#include <string>
#include <sstream>


using namespace std;

class GitCommit {
public:
    GitCommit();
    GitCommit(string);
    GitCommit(string, string, string, string, string);
    ~GitCommit() {};

    void add_tree(string);
    void add_author(string);
    void add_email(string);
    void add_date(string);
    void add_parents(string);
    void add_child(string);

    string commit_ish;  // 提交的hash id
    string tree_ish;     // 提交的tree对象
    //string type;       // 提交的类型
    string author;     // 提交的作者
    string email;      // 作者的邮箱
    string date;       // 提交的日期
    string commit_message;
    vector<string> tags;         // tag
    int pcnt;
    vector<GitCommit*> parents;      // 父提交

    vector<GitCommit*> children;     // 儿子
    vector<string> modified_file;

    static unordered_map<string, GitCommit*> commit_list;
    
    static unordered_map<string, FileHead*> file_list;   // 文件名 + blob_ish 作为 key
    static unordered_map<string, vector<FileHead*>> same_name_file;   // 文件名 作为 key
    

    void diff_parents(string&);
    void link_prev_node(string, string);
    void link_prev_node(string, string, string);
    void link_prev_node(string, string, string, string, string);

    void print_file_list();

};



#endif