#include "GitCommit.h"
#include "FileGraph.h"
#include "FindRiscv.hpp"

#include <regex>
#include <sstream>
#include <fstream>
#include <string>
#include <iostream>
#include <queue>
#include <string.h>

#include <time.h>


void get_commits();
std::string get_dir(std::string git_dir);
void read_commits();
void get_file_stat();
void file_list_init();
void read_tree(std::string& commit_ish, std::string& tree_ish, std::string path);
void gen_csv();
void bfs(FileHead* head);
void w_head(FileHead* head, int idx);
void w_node(FileNode* node);
void w_h2n(FileHead* head, FileNode* node, int head_idx);
void w_n2n(FileNode* cur, FileNode* ne);



const std::string start_flag = "***adfa*asdfo**";
const std::string end_flag = "**dasjf93hd9f()&&^";

std::vector<GitCommit*> first_commits;
int ish_len;
std::string git_dir;  // git仓库的文件路径



void test();
std::string get_time();


int main(int ac, char** av) {

    test();

    std::cout << get_time() << std::endl;
    
    if (ac >= 2 ) {
        std::string s1 = av[1];
        std::string pwd;
        if (s1[0] == '/')
            pwd = s1;
        else
            pwd = get_dir(s1);
        
        git_dir = "GIT_DIR=" + pwd + ".git";
        // std::cout << git_dir << endl;

        if (ac >= 3 && av[2][0] == '-') {
            std::string s2 = av[2];
            if (s2.size() != 2) {
                std::cout << "参数错误" << std::endl;
                exit(1);
            }
            switch(s2[1]) {
                case 'g':
                    std::cout << "将仓库内commit信息写入文件..." << std::endl;
                    get_commits();
                    std::cout << "写入完毕\n";
                    break;
                default:
                    std::cout << "参数错误" << std::endl;
                    exit(2);
                    break;
            }
        }

        
        
    } else {
        std::cout << "参数错误" << std::endl;
        exit(3);
    }

    std::cout << "获取commit信息..." << std::endl;
    read_commits();    // 获取commit的信息
    std::cout << "获取完毕\n";  

    find_riscv_commits(first_commits);

    //print_commits();

    // std::cout << "读取修改的文件..." << std::endl;
    // get_file_stat();
    // std::cout << "完成" << std::endl;

    // std::cout << get_time() << std::endl;
    
    // return 0;

    // while(1) {
    //     int c;
    //     std::string s, a, b;
    //     int idx;
    //     std::cout << "输入一个数" << std::endl;
    //     scanf("%d", &c);
    //     switch(c) {
    //         case 1:
    //             std::cout << "文件版本图共有" << GitCommit::file_list.size() << std::endl;
    //             break;
    //         case 2:
    //             std::cout << "请输入commit hash" << std::endl;
    //             std::cin >> s;
    //             s = s.substr(0, ish_len);
    //             if (!GitCommit::commit_list.count(s)) {
    //                 std::cout << "commit hash 输入有误" << std::endl;
    //                 break;
    //             }
    //             std::cout << "此 commit 修改的文件为:\n";
    //             for (auto& m: GitCommit::commit_list[s]->modified_file) {
    //                 std::cout << m << std::endl;
    //             }
    //             break;
    //         case 3:
    //             std::cout << "输入文件名\n";
    //             std::cin >> a;
    //             std::cout << "输入文件blob\n";
    //             std::cin >> b;
    //             s = a + ' ' + b;
    //             if (!file_nodes.count(s)) {
    //                 std::cout << "文件名输入有误\n";
    //                 break;
    //             }
    //             while(1) {
    //                 std::cout << "此文件版本有 " << file_nodes[s]->prev_nodes.size() << " 个前置版本\n";
    //                 for (auto& x: file_nodes[s]->prev_nodes) {
    //                     std::cout << "blob hash: " << x->blob_ish << ' ' << "commit hash: " << x->commit_ish << std::endl;
    //                 }
    //                 std::cout << "此文件版本有 " << file_nodes[s]->next_nodes.size() << " 个后置版本\n";
    //                 for (auto& x: file_nodes[s]->next_nodes) {
    //                     std::cout << "blob hash: " << x->blob_ish << ' ' << "commit hash: " << x->commit_ish << std::endl;
    //                 }
    //                 std::cin >> b;
    //                 s = a + ' ' + b;

    //             }

    //         case 4:
    //             std::cout << "生成csv文件\n";
    //             gen_csv();
    //             std::cout << "生成完毕\n";
            
            
    //     }
    // }



    return 0;


}

void get_commits() {
    std::string cmd = git_dir + " git log --pretty=format:\"" + start_flag + 
                 "%n %h %n %t %n %p %n %an %n %ae %n %ad %n %s %n %b %n " + end_flag + "\" --date-order --reverse" ; // *号后是提交的哈希， #号后是commit的信息
    char str[300];
    //git log --pretty=format:"%n %h %n %t %n %p %n %an %n %ae %n %ad %n %s %b %n"
   // cout << cmd << endl;
    strcpy(str, cmd.c_str());
    system(strcat(str, "> data/commits.txt"));  //git log 输出到commits.txt
}

std::string get_dir(std::string git_dir) {
    std::string cmd = "pwd";
    std::string cur_dir;
    FILE *fp = NULL;
    if( (fp = popen(cmd.c_str(), "r")) != NULL) {
        char read_str[1024] = "";
        fgets(read_str, sizeof(read_str), fp);
        cur_dir = read_str;
    }
    pclose(fp);
    cur_dir += '/' + git_dir;
    
    std::regex re("\n+");
    auto result = std::regex_replace(cur_dir, re, "");  // 去除回车
    // cout << result << endl;
    if (result[result.size() - 1] != '/') result += '/';
    return result;
}

void read_commits() {
    std::ifstream ifile;
    
    ifile.open("data/commits.txt", std::ios::in);
    std::cout << "open data/commits.txt\n";
    std::regex riscv_reg("risc-?v");

    std::string line;
    int cnt = 0;  // 提交的个数
    int idx = 0;

    std::string commit_ish, tree_ish, author, email, date;
 

    while(getline(ifile, line)) {

        // if(cnt == 3) break;
        // std::cout << line << std::endl;

        std::string out;
        std::istringstream in(line);
        in >> out;



        if (!line.compare(start_flag)) {
            idx = 0;
            cnt++;
            continue;
        }

        if (!line.compare(end_flag)) {
            continue;
        }
        switch(idx) {
            case 0:   // 一个新的提交
                commit_ish = out;
                GitCommit::commit_list[commit_ish] = new GitCommit(commit_ish);
                
                idx++;
                break;
            case 1:   // tree对象
                tree_ish = out;
                if (GitCommit::commit_list.count(commit_ish)) GitCommit::commit_list[commit_ish]->add_tree(tree_ish);
                else std::cout << "\ncase 1\n\n";

                idx++;
                break;
            case 2:  // 获得它的父亲

                if (out.empty()) {
                    std::cout << "fisrt commit: " << commit_ish << " parent: " << out << std::endl;
                    if (GitCommit::commit_list.count(commit_ish)) first_commits.push_back(GitCommit::commit_list[commit_ish]);
                    else std::cout << "\ncase 2-1\n\n";
                    idx ++;
                    break;
                }

                do {
                    
                    if (GitCommit::commit_list.count(commit_ish)) GitCommit::commit_list[commit_ish]->add_parents(out);
                    else std::cout << "\ncase 2-2\n\n";
                    if (GitCommit::commit_list.count(commit_ish)) GitCommit::commit_list[out]->add_child(commit_ish);
                    else std::cout << "\ncase 2-3\n\n";
                } while(in >> out);
                
                idx ++;
                break;
            case 3:  // 作者
                author = "";
                do
                    author += out + ' ';
                while(in >> out);
                if (GitCommit::commit_list.count(commit_ish)) GitCommit::commit_list[commit_ish]->add_author(author);
                else std::cout << "\ncase 3\n\n";
                idx++;
                break;
            case 4:  // 邮箱
                email = "";
                do
                    email += out + ' ';
                while(in >> out);
                if (GitCommit::commit_list.count(commit_ish)) GitCommit::commit_list[commit_ish]->add_email(email);
                else std::cout << "\ncase 4\n\n";
                idx ++;

                break;
            case 5:  // 日期
                date = "";
                do
                    date += out + ' ';
                while(in >> out);
                if (GitCommit::commit_list.count(commit_ish)) GitCommit::commit_list[commit_ish]->add_date(date);
                else std::cout << "\ncase 5\n\n";  
                idx ++;

                break;
 
            case 6:  // commit message
                if (std::regex_search(line, riscv_reg)) {
                    GitCommit::commit_list[commit_ish]->add_rv();
                }
                break;


        }

    }
    std::cout << "commit数量: " << GitCommit::commit_list.size() << std::endl;
    // std::cout << "child: " << GitCommit::commit_list["440ecc9"]->children[0]->commit_ish << std::endl;
    // std::cout << "p:" << GitCommit::commit_list["dfb77fb"]->pcnt << std::endl;
}


std::unordered_map<GitCommit*, int> stt;
void get_file_stat() {
    std::cout << "初始化..." << std::endl;
    file_list_init();  
    std::cout << "初始化完毕" << std::endl;
    // first_commit->print_file_list();

    
    std::queue<GitCommit*> q;
    for (auto& commit: first_commits) q.push(commit);
    
    int cnt = 0;
    while(q.size()) {
        auto cur = q.front();
        
        // std::cout << cur->commit_ish << endl << std::endl;

        q.pop();
        stt[cur] ++;
        if (stt[cur] > 1) {
            cur->print();
            exit(1);
            
        }
        cur->diff_parents(git_dir, ish_len);



       for(auto& child: cur->children){


            if(child->pcnt == 1){
                q.push(child);

                child->pcnt = child->parents.size();
            } else {
                child->pcnt = child->pcnt - 1;
            }
        }
        cnt ++;
        std::cout << "图构建中: " << cnt << " / " << GitCommit::commit_list.size() << "\r";
        
    }
    // first_commit->print_file_list();     // 打印file list
}

void file_list_init() {  // 初始化文件列表，从仓库的第一个commit开始
    if(GitCommit::commit_list.empty()) return ; 

    for (auto& commit: first_commits) {
        std::string commit_ish = commit->commit_ish;
        std::string tree_ish = commit->tree_ish;
        std::string path = "";
        ish_len = commit_ish.size();

        read_tree(commit_ish, tree_ish, path);
    }

    
}


void read_tree(std::string& commit_ish, std::string& tree_ish, std::string path) {  // 深度优先读取树

    std::string cmd = git_dir + " git cat-file -p " + tree_ish;
    FILE *fp = NULL;
    if ((fp = popen(cmd.c_str(), "r")) != NULL) {
        char read_str[1024] = "";
        while(fgets(read_str, sizeof(read_str), fp)) {
            std::istringstream in(read_str);
            std::string out;            
            in >> out;
            if (out.empty()) continue;
            std::string filemode = out;
            in >> out;
            if (out == "blob") {
                in >> out;
                std::string blob_ish = out;
                blob_ish = blob_ish.substr(0, ish_len);
                in >> out;
                std::string file_name = path + out;
                auto t = new FileHead(file_name, blob_ish, commit_ish);
                GitCommit::file_list[file_name + ' ' + blob_ish] = t;
                GitCommit::same_name_file[file_name].push_back(t);


            } else if(out == "tree") {
                in >> out;
                std::string son_ish = out;
                in >> out;
                read_tree(commit_ish, son_ish, path + out + '/');
            }
        }
    }
    pclose(fp);
}



void gen_csv() {

    system("rm date/csv/*");


    std::ofstream o1("data/csv/heads_header.csv");
    o1.clear();
    o1 << "headId:ID(Head-ID),:LABEL\n";
    o1.close();
    std::ofstream o2("data/csv/head_relation_header.csv");
    o2 << ":START_ID(Head-ID),:END_ID(Node-ID),:TYPE\n";
    o2.close();
    std::ofstream o3("data/csv/nodes_header.csv");
    o3 << "nodeId:ID(Node-ID),name,blobHash,commitHash,:LABEL\n";
    o3.close();
    std::ofstream o4("data/csv/node_relation_header.csv");
    o4 << ":START_ID(Node-ID),:END_ID(Node-ID),:TYPE\n";
    o4.close();


    for (auto& fh: GitCommit::file_list) {
        bfs(fh.second);
    }
}

std::unordered_map<FileNode*, int> st;

void bfs(FileHead* head) {
    static int idx = 0;
    idx ++;
    auto t = head->head;
    w_head(head, idx);
    if (!st.count(t)) w_node(t);
    // else {
    //     std::cout << "bfs\n" << t << std::endl;
    //     t->print();
    //     std::cout << std::endl;
    //     // exit(2);
    // }
    w_h2n(head, head->head, idx);
    std::queue<FileNode*> q;
    q.push(t);
    while(q.size()) {
        auto cur_node = q.front();
        q.pop();
        for (auto& next_node: cur_node->next_nodes) {
            if(!st.count(next_node)) {
                q.push(next_node);
                w_node(next_node);
            } 
            w_n2n(cur_node, next_node);
        }
    }
}

void w_head(FileHead* head, int idx) {  //写入heads.csv
    std::string line = std::to_string(idx) + ",head\n";
    std::ofstream out;
    out.open("data/csv/heads.csv", std::ios::app);
    out << line;
    out.close();
}

void w_node(FileNode* node) {  // 写入nodes.csv
    static int idx = 0;
    idx ++;
    std::string line = std::to_string(idx) + ',' + node->file_name + ',' + node->blob_ish + ',' + node->commit_ish + ",node\n";
    std::ofstream out; 
    out.open("data/csv/nodes.csv", std::ios::app);
    out << line;
    out.close();
    st[node] = idx;
}

void w_h2n(FileHead* head, FileNode* node, int head_idx) {  // 写入head_relation.csv
    int node_idx = st[node];
    std::string line = std::to_string(head_idx) + ',' + std::to_string(node_idx) + ",head\n";
    std::ofstream out;
    out.open("data/csv/head_relation.csv", std::ios::app);
    out << line;
    out.close();
}

void w_n2n(FileNode* cur, FileNode* ne) {  // 写入node_relation.csv
    int cur_idx = st[cur];
    int ne_idx = st[ne];
    std::string line = std::to_string(cur_idx) + ',' + std::to_string(ne_idx) + ",is parent\n";
    std::ofstream out;
    out.open("data/csv/node_relation.csv", std::ios::app);
    out << line;
    out.close();
}



void test() {
    int n = 4;
    std::cout << "\n\ntest" << 

    n
    
    << "\n\n";
}


std::string get_time()
{
    time_t timep;
    time (&timep); //获取time_t类型的当前时间
    char tmp[64];
    strftime(tmp, sizeof(tmp), "%Y-%m-%d %H:%M:%S",localtime(&timep) );//对日期和时间进行格式化
    return tmp;
}



