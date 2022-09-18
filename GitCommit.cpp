#include "GitCommit.h"

unordered_map<string, GitCommit*> GitCommit::commit_list;
unordered_map<string, FileHead*> GitCommit::file_list;
unordered_map<string, vector<FileHead*>> GitCommit::same_name_file;

GitCommit::GitCommit() {
    this->pcnt = 0;
    this->parents.clear();
    this->children.clear();
}

GitCommit::GitCommit(string commit_ish) {
    this->commit_ish = commit_ish;
    this->pcnt = 0;
    this->parents.clear();
    this->children.clear();
}

GitCommit::GitCommit(string commit_ish, string tree_ish, string author, string email, string date) {
    this->commit_ish = commit_ish;
    this->tree_ish = tree_ish;
    this->author = author;
    this->email = email;
    this->date = date;
    this->pcnt = 0;
    this->parents.clear();
    this->children.clear();
}

void GitCommit::add_tree(string tree_ish) {
    this->tree_ish = tree_ish;
}

void GitCommit::add_author(string author) {
    this->author = author;
}
void GitCommit::add_email(string email) {
    this->email = email;
}
void GitCommit::add_date(string date) {
    this->date = date;
}
void GitCommit::add_parents(string parent) {
    this->parents.push_back(this->commit_list[parent]);
}
void GitCommit::add_child(string child) {
    this->children.push_back(this->commit_list[child]);
}



void GitCommit::print_file_list() {
    int n = 0;
    for(auto& f: GitCommit::file_list) {
        cout << f.first << ' ';
        cout << f.second->used_name[0] << endl;
        n++;
    }
    cout << n << endl;
}