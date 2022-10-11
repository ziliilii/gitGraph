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


void GitCommit::link_prev_node(string prev_blob, string file) {  // 新增一个代表删除的文件节点
    auto t = new FileNode(this->commit_ish, file);   // 新增"删除"节点
    t->link(file_nodes[file + ' ' + prev_blob]);
}

void GitCommit::link_prev_node(string prev_blob, string cur_blob, string file) {  // 新增修改的节点
    if (this->commit_ish == "50eca3eb89d7") cout << "in link_prev_node cur_blob: " << endl << cur_blob << endl;

    FileNode* t;
    string s = file + ' ' + cur_blob;
    if (this->commit_ish == "50eca3eb89d7") cout << "in link_prev_node s: " << endl << s << endl;
    if(file_nodes.count(s)) t = file_nodes[s];
    else t = new FileNode(cur_blob, this->commit_ish, file);  // 新增修改节点
    if (this->commit_ish == "50eca3eb89d7") cout << "in link_prev_node cur_blob 2: " << endl << cur_blob << endl;

    if (!file_nodes.count(file + ' ' + prev_blob)) {
        cout << file << ' ' + prev_blob << "\n" << "100\n";
        exit(1);
    }
    else {
        t->link(file_nodes[file + ' ' + prev_blob]);
    }

}

// 新增复制 重命名的节点
void GitCommit::link_prev_node(string prev_blob, string cur_blob, string prev_file, string cur_file, string type) {
    FileNode* t;
    string s = cur_file + ' ' + cur_blob;
    if(file_nodes.count(s)) t = file_nodes[s];
    else t = new FileNode(cur_blob, this->commit_ish, cur_file, type);   // 新增复制 重命名的节点

    t->link(file_nodes[prev_file + ' ' + prev_blob]);
}


// 当前节点与父亲git diff
void GitCommit::diff_parents(string& git_dir, int ish_len) {
    vector<string> res, merge_A;
    string cmd;


    if (this->parents.size() == 1) {  // 当前节点只有一个父亲
        string prev_ish = this->parents[0]->commit_ish;
        cmd = git_dir + " git diff-tree -r " + prev_ish + ' '  + this->commit_ish;
        cmd += " -l0";
        FILE *fp = NULL;
        if ((fp = popen(cmd.c_str(), "r")) != NULL) {
            char read_str[4096] = "";
            while(fgets(read_str, sizeof(read_str), fp)) {
                res.push_back(read_str);
                if (this->commit_ish == "50eca3eb89d7") cout<< res[res.size() - 1] << "\n";
            }
        }
        pclose(fp);
    } else {  // 当前节点是merge节点，有多个父亲
        for (auto& p: this->parents) {
            string prev_ish = p->commit_ish;
            // cmd = git_dir + " git diff --raw " + prev_ish + ' '  + this->commit_ish;
            cmd = git_dir + " git diff-tree -r " + prev_ish + ' '  + this->commit_ish;
            cmd += " -l0";
            FILE *fp = NULL;
            if ((fp = popen(cmd.c_str(), "r")) != NULL) {
                char read_str[4096] = "";
                while(fgets(read_str, sizeof(read_str), fp)) {
                    istringstream in(read_str);
                    string out;
                    int temp = 5;
                    while(temp --) {
                        in >> out;
                    }
                    if (out[0] == 'A') {
                       // cout << "多父亲存在新增" << endl;
                        merge_A.push_back(read_str);
                        continue;   // merge新增
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
    for (auto& s: res) {   // 遍历git diff出来的结果

        istringstream in(s);
        string out;
        in >> out;
        in >> out;
        in >> out;
        prev_blob = out.substr(0, ish_len);
        in >> out;
        cur_blob = out.substr(0, ish_len);
        in >> out;
        type = out;
        in >> out;
        file = out;
        FileHead* t;

        switch(type[0]) {
            case 'A':  // 新增               
                t = new FileHead(file, cur_blob, this->commit_ish);  // 新建一个文件头
                this->modified_file.push_back(file + ' ' + cur_blob);  // 在当前commit节点内新增这个文件版本节点
                GitCommit::file_list[file + ' ' + cur_blob] = t;
                GitCommit::same_name_file[file].push_back(t);
                break;
            case 'D':   // 删除
                this->link_prev_node(prev_blob, file);  // 新增一个代表删除的文件节点
                this->modified_file.push_back(file + ' ' + cur_blob);
                break;
            case 'C':  // 复制
                in >> out;
                cur_file = out;
                this->modified_file.push_back(file + ' ' + cur_blob);
                this->link_prev_node(prev_blob, cur_blob, file, cur_file, type);  // 新增复制的节点
                break;
            case 'R':  // 重命名
                in >> out;
                cur_file = out;
                this->modified_file.push_back(file + ' ' + cur_blob);
                this->link_prev_node(prev_blob, cur_blob, file, cur_file, type);  //  新增重命名的节点
                break;
            case 'M': // 修改

                this->modified_file.push_back(file + ' ' + cur_blob);
                this->link_prev_node(prev_blob, cur_blob, file);  // 新增修改的节点
                break;
            default:
                
                cout << "其他情况" << endl;
                break;
        }
    }

    for (auto& s: merge_A) {
        istringstream in(s);
        string out;
        in >> out;
        in >> out;
        in >> out;
        prev_blob = out.substr(0, ish_len);
        in >> out;
        cur_blob = out.substr(0, ish_len);
        in >> out;
        type = out;
        in >> out;
        file = out;
        FileHead* t;
        string cur_file_node = file + ' ' + cur_blob;
        if (!file_nodes.count(cur_file_node)) {
            t = new FileHead(file, cur_blob, this->commit_ish);  // 新建一个文件头
            this->modified_file.push_back(file + ' ' + cur_blob);  // 在当前commit节点内新增这个文件版本节点
            GitCommit::file_list[file + ' ' + cur_blob] = t;
            GitCommit::same_name_file[file].push_back(t);
        }
    }



}




void GitCommit::print_file_list() {
    int n = 0;
    for(auto& f: GitCommit::file_list) {
        cout << f.first << endl;
        cout << f.second->used_name[0] << endl;
        
        cout << "head's node commit hash: " << f.second->head->commit_ish << endl;
        cout << "--------------\n"; 
        n++;
    }
    cout << n << endl;
}