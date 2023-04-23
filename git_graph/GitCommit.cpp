#include "GitCommit.h"

std::unordered_map<std::string, GitCommit*> GitCommit::commit_list;
std::unordered_map<std::string, FileHead*> GitCommit::file_list;
std::unordered_map<std::string, std::vector<FileHead*>> GitCommit::same_name_file;

GitCommit::GitCommit() {
    this->pcnt = 0;
    this->is_rv = false;
    this->parents.clear();
    this->children.clear();
}

GitCommit::GitCommit(std::string commit_ish) {
    this->commit_ish = commit_ish;
    this->pcnt = 0;
    this->is_rv = false;
    this->parents.clear();
    this->children.clear();
}

GitCommit::GitCommit(std::string commit_ish, std::string tree_ish, std::string author, std::string email, std::string date) {
    this->commit_ish = commit_ish;
    this->tree_ish = tree_ish;
    this->author = author;
    this->email = email;
    this->date = date;
    this->pcnt = 0;
    this->is_rv = false;
    this->parents.clear();
    this->children.clear();
}

void GitCommit::add_tree(std::string tree_ish) {
    this->tree_ish = tree_ish;
}

void GitCommit::add_author(std::string author) {
    this->author = author;
}
void GitCommit::add_email(std::string email) {
    this->email = email;
}
void GitCommit::add_date(std::string date) {
    this->date = date;
}
void GitCommit::add_parents(std::string parent) {
    this->parents.push_back(this->commit_list[parent]);
    this->pcnt ++;
}
void GitCommit::add_child(std::string child) {
    this->children.push_back(this->commit_list[child]);
}

void GitCommit::add_rv() {
    this->is_rv = true;
}


void GitCommit::link_prev_node(std::string prev_blob, std::string file) {  // 新增一个代表删除的文件节点
    auto t = new FileNode(this->commit_ish, file);   // 新增"删除"节点
    t->link(file_nodes[file + ' ' + prev_blob]);
}

void GitCommit::link_prev_node(std::string prev_blob, std::string cur_blob, std::string file) {  // 新增修改的节点

    FileNode* t;
    std::string s = file + ' ' + cur_blob;

    if(file_nodes.count(s)) t = file_nodes[s];
    else t = new FileNode(cur_blob, this->commit_ish, file);  // 新增修改节点


    if (!file_nodes.count(file + ' ' + prev_blob)) {
        std::cout << file << ' ' + prev_blob << "\n" << "100\n";
        exit(1);
    }
    else {
        t->link(file_nodes[file + ' ' + prev_blob]);
    }

}

// 新增复制 重命名的节点
void GitCommit::link_prev_node(std::string prev_blob, std::string cur_blob, std::string prev_file, std::string cur_file, std::string type) {
    FileNode* t;
    std::string s = cur_file + ' ' + cur_blob;
    if(file_nodes.count(s)) t = file_nodes[s];
    else t = new FileNode(cur_blob, this->commit_ish, cur_file, type);   // 新增复制 重命名的节点

    t->link(file_nodes[prev_file + ' ' + prev_blob]);
}


// 当前节点与父亲git diff
void GitCommit::diff_parents(std::string& git_dir, int ish_len) {
    std::vector<std::string> res, merge_A;
    std::string cmd;

    if (this->parents.size() == 0) return;

    if (this->parents.size() == 1) {  // 当前节点只有一个父亲
        std::string prev_ish = this->parents[0]->commit_ish;
        cmd = git_dir + " git diff-tree -r " + prev_ish + ' '  + this->commit_ish;
        cmd += " -l0";
        FILE *fp = NULL;
        if ((fp = popen(cmd.c_str(), "r")) != NULL) {
            char read_str[1024] = "";
            while(fgets(read_str, sizeof(read_str), fp)) {
                res.push_back(read_str);
            }
        }
        pclose(fp);
    } else {  // 当前节点是merge节点，有多个父亲
        for (auto& p: this->parents) {
            std::string prev_ish = p->commit_ish;
            cmd = git_dir + " git diff --raw " + prev_ish + ' '  + this->commit_ish;
            // cmd = git_dir + " git diff-tree -r " + prev_ish + ' '  + this->commit_ish;
            cmd += " -l0";
            FILE *fp = NULL;
            if ((fp = popen(cmd.c_str(), "r")) != NULL) {
                char read_str[1024] = "";
                while(fgets(read_str, sizeof(read_str), fp)) {
                    std::istringstream in(read_str);
                    std::string out;
                    int temp = 5;
                    while(temp --) {
                        in >> out;
                    }
                    if (out[0] == 'A') {
                       // std::cout << "多父亲存在新增" << endl;
                        merge_A.push_back(read_str);
                        continue;   // merge新增
                    }
                    if (out[0] == 'D') {

                        // std::cout << "多父亲存在删除：" << endl;
                        // std::cout << "当前节点：" << this->commit_ish << endl;
                        // std::for(auto& x: this->parents) {
                        //     std::cout << "父亲：" << x->commit_ish << endl;
                        // }
                        // std::cout << read_str << endl;
                        res.push_back(read_str);
                    }
                    if (out[0] == 'C') {
                        std::cout << "多父亲存在复制\n";
                    }
                    if (out[0] == 'R') {
                        // std::cout << "多父亲存在重命名\n";
                        // std::cout << "当前节点：" << this->commit_ish << endl;
                        // for(auto& x: this->parents) {
                        //     std::cout << "父亲：" << x->commit_ish << endl;
                        // }
                        // std::cout << read_str << endl;
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


    std::string prev_blob, cur_blob, type, file, cur_file;
    for (auto& s: res) {   // 遍历git diff出来的结果

        std::istringstream in(s);
        std::string out;
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
                
                std::cout << "其他情况" << std::endl;
                break;
        }
    }

    for (auto& s: merge_A) {
        std::istringstream in(s);
        std::string out;
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
        std::string cur_file_node = file + ' ' + cur_blob;
        if (!file_nodes.count(cur_file_node)) {
            t = new FileHead(file, cur_blob, this->commit_ish);  // 新建一个文件头
            this->modified_file.push_back(file + ' ' + cur_blob);  // 在当前commit节点内新增这个文件版本节点
            GitCommit::file_list[file + ' ' + cur_blob] = t;
            GitCommit::same_name_file[file].push_back(t);
        }
    }



}

void GitCommit::print() {
    std::cout << "当前提交" << this->commit_ish << std::endl;
    std::cout << "父亲数：" << this->parents.size() << std::endl;
    for (auto p: this->parents) {
        std::cout << "父亲hash: " << p->commit_ish << std::endl;
    }
    std::cout << "儿子数：" << this->children.size() << std::endl;
    for (auto c: this->children) {
        std::cout << "儿子hash: " << c->commit_ish << std::endl;
    }
    std::cout << "----------打印完毕----------\n";
}


void GitCommit::print_file_list() {
    int n = 0;
    for(auto& f: GitCommit::file_list) {
        std::cout << f.first << std::endl;
        std::cout << f.second->used_name[0] << std::endl;
        
        std::cout << "head's node commit hash: " << f.second->head->commit_ish << std::endl;
        std::cout << "--------------\n"; 
        n++;
    }
    std::cout << n << std::endl;
}