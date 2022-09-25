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
    this->pcnt ++;
}
void GitCommit::add_child(string child) {
    this->children.push_back(this->commit_list[child]);
}


void GitCommit::link_prev_node(string prev_blob, string file) {
    auto t = new FileNode(this->commit_ish);
    t->link(file_nodes[file + ' ' + prev_blob]);
}

void GitCommit::link_prev_node(string prev_blob, string cur_blob, string file) {
    FileNode* t;
    string s = file + ' ' + cur_blob;
    if(file_nodes.count(s)) t = file_nodes[s];
    else t = new FileNode(cur_blob, this->commit_ish, file);

    t->link(file_nodes[file + ' ' + prev_blob]);

}

void GitCommit::link_prev_node(string prev_blob, string cur_blob, string prev_file, string cur_file, string type) {
    FileNode* t;
    string s = cur_file + ' ' + cur_blob;
    if(file_nodes.count(s)) t = file_nodes[s];
    else t = new FileNode(cur_blob, this->commit_ish, cur_file, type);

    t->link(file_nodes[prev_file + ' ' + prev_blob]);
}

void GitCommit::diff_parents(string& git_dir) {
    



    vector<string> res;
    string cmd;
    if (this->parents.size() == 1) {
        string prev_ish = this->parents[0]->commit_ish;
        cmd = git_dir + " git diff --raw " + prev_ish + ' '  + this->commit_ish;
        cmd += " -l0";
        FILE *fp = NULL;
        if ((fp = popen(cmd.c_str(), "r")) != NULL) {
            char read_str[1024] = "";
            while(fgets(read_str, sizeof(read_str), fp)) {
                res.push_back(read_str);
            }
        }
        pclose(fp);
    } else {
        for (auto& p: this->parents) {
            string prev_ish = p->commit_ish;
            cmd = git_dir + " git diff --raw " + prev_ish + ' '  + this->commit_ish;
            cmd += " -l0";
            FILE *fp = NULL;
            if ((fp = popen(cmd.c_str(), "r")) != NULL) {
                char read_str[1024] = "";
                while(fgets(read_str, sizeof(read_str), fp)) {
                    istringstream in(read_str);
                    string out;
                    int temp = 5;
                    while(temp --) {
                        in >> out;
                    }
                    if (out[0] == 'A') {
                       // cout << "多父亲存在新增" << endl;
                        continue;
                    }
                    if (out[0] == 'D') {

                        // cout << "多父亲存在删除：" << endl;
                        // cout << "当前节点：" << this->commit_ish << endl;
                        // for(auto& x: this->parents) {
                        //     cout << "父亲：" << x->commit_ish << endl;
                        // }
                        // cout << read_str << endl;
                        res.push_back(read_str);
                    }
                    if (out[0] == 'C') {
                        // cout << "多父亲存在复制\n";
                    }
                    if (out[0] == 'R') {
                        // cout << "多父亲存在重命名\n";
                        // cout << "当前节点：" << this->commit_ish << endl;
                        // for(auto& x: this->parents) {
                        //     cout << "父亲：" << x->commit_ish << endl;
                        // }
                        // cout << read_str << endl;
                        res.push_back(read_str);
                    }
                    if (out[0] == 'M') {
                        res.push_back(read_str);
                    }
                }
            }
            pclose(fp);
        }
    }

    string prev_blob, cur_blob, type, file, cur_file;
    for (auto& s: res) {



        istringstream in(s);
        string out;
        in >> out;
        in >> out;
        in >> out;
        prev_blob = out;
        in >> out;
        cur_blob = out;
        in >> out;
        type = out;
        in >> out;
        file = out;
        FileHead* t;

        switch(type[0]) {
            case 'A':
                
                t = new FileHead(file, cur_blob, this->commit_ish);
                this->modified_file.push_back(file + ' ' + cur_blob);
                GitCommit::file_list[file + ' ' + cur_blob] = t;
                GitCommit::same_name_file[file].push_back(t);
                break;
            case 'D':
                this->link_prev_node(prev_blob, file);
                this->modified_file.push_back(file + ' ' + cur_blob);
                break;
            case 'C':
                in >> out;
                cur_file = out;
                this->modified_file.push_back(file + ' ' + cur_blob);
                this->link_prev_node(prev_blob, cur_blob, file, cur_file, type);
                break;
            case 'R':
                in >> out;
                cur_file = out;
                this->modified_file.push_back(file + ' ' + cur_blob);
                this->link_prev_node(prev_blob, cur_blob, file, cur_file, type);
                break;
            case 'M':

                this->modified_file.push_back(file + ' ' + cur_blob);
                this->link_prev_node(prev_blob, cur_blob, file);
                break;
            default:
                
                cout << "其他情况" << endl;
                break;
        }
    }



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