#include "GitCommit.h"
#include "FileGraph.h"

#include <regex>
#include <sstream>
#include <fstream>
#include <string>
#include <iostream>
#include <queue>
#include <string.h>

using namespace std;

void get_commits();
string get_dir(string git_dir);
void read_commits();
void get_file_stat();
void file_list_init();
void read_tree(string& commit_ish, string& tree_ish, string path);
void gen_csv();
void bfs(FileHead* head);
void w_head(FileHead* head, int idx);
void w_node(FileNode* node);
void w_h2n(FileHead* head, FileNode* node, int head_idx);
void w_n2n(FileNode* cur, FileNode* ne);


const string start_flag = "***adfa*asdfo**";
const string end_flag = "**dasjf93hd9f()&&^";

GitCommit* first_commit;
int ish_len;
string git_dir;  // git仓库的文件路径



void test();


int main(int ac, char** av) {

    test();
    
    if (ac >= 2 ) {
        string s1 = av[1];
        string pwd;
        if (s1[0] == '/')
            pwd = s1;
        else
            pwd = get_dir(s1);
        
        git_dir = "GIT_DIR=" + pwd + ".git";
        // cout << git_dir << endl;

        if (ac >= 3 && av[2][0] == '-') {
            string s2 = av[2];
            if (s2.size() != 2) {
                cout << "参数错误" << endl;
                exit(1);
            }
            switch(s2[1]) {
                case 'g':
                    cout << "将仓库内commit信息写入文件..." << endl;
                    get_commits();
                    break;
                default:
                    cout << "参数错误" << endl;
                    exit(2);
                    break;
            }
        }

        
        
    } else {
        cout << "参数错误" << endl;
        exit(3);
    }

    cout << "获取commit信息..." << endl;
    read_commits();    // 获取commit的信息
    //print_commits();

    cout << "读取修改的文件..." << endl;
    get_file_stat();
    cout << "完成" << endl;
    
    // return 0;

    while(1) {
        int c;
        string s, a, b;
        int idx;
        cout << "输入一个数" << endl;
        scanf("%d", &c);
        switch(c) {
            case 1:
                cout << "文件版本图共有" << GitCommit::file_list.size() << endl;
                break;
            case 2:
                cout << "请输入commit hash" << endl;
                cin >> s;
                s = s.substr(0, ish_len);
                if(!GitCommit::commit_list.count(s)) {
                    cout << "commit hash 输入有误" << endl;
                    break;
                }
                cout << "此 commit 修改的文件为:\n";
                for (auto& m: GitCommit::commit_list[s]->modified_file) {
                    cout << m << endl;
                }
                break;
            case 3:
                cout << "输入文件名\n";
                cin >> a;
                cout << "输入文件blob\n";
                cin >> b;
                s = a + ' ' + b;
                if(!file_nodes.count(s)) {
                    cout << "文件名输入有误\n";
                    break;
                }
                while(1) {
                    cout << "此文件版本有 " << file_nodes[s]->prev_nodes.size() << " 个前置版本\n";
                    for(auto& x: file_nodes[s]->prev_nodes) {
                        cout << "blob hash: " << x->blob_ish << ' ' << "commit hash: " << x->commit_ish << endl;
                    }
                    cout << "此文件版本有 " << file_nodes[s]->next_nodes.size() << " 个后置版本\n";
                    for(auto& x: file_nodes[s]->next_nodes) {
                        cout << "blob hash: " << x->blob_ish << ' ' << "commit hash: " << x->commit_ish << endl;
                    }
                    cin >> b;
                    s = a + ' ' + b;

                }

            case 4:
                cout << "生成csv文件\n";
                gen_csv();
                cout << "生成完毕\n";
            
            
        }
    }

    return 0;


}

void get_commits() {
    string cmd = git_dir + " git log --pretty=format:\"" + start_flag + 
                 "%n %h %n %t %n %p %n %an %n %ae %n %ad %n %s %n %b %n " + end_flag + "\" --reverse" ; // *号后是提交的哈希， #号后是commit的信息
    char str[300];
   // cout << cmd << endl;
    strcpy(str, cmd.c_str());
    system(strcat(str, "> data/commits.txt"));  //git log 输出到commits.txt
}

string get_dir(string git_dir) {
    string cmd = "pwd";
    string cur_dir;
    FILE *fp = NULL;
    if((fp = popen(cmd.c_str(), "r")) != NULL){
        char read_str[1024] = "";
        fgets(read_str, sizeof(read_str), fp);
        cur_dir = read_str;
    }
    pclose(fp);
    cur_dir += '/' + git_dir;
    
    regex re("\n+");
    auto result = regex_replace(cur_dir, re, "");  // 去除回车
    // cout << result << endl;
    return result;
}

void read_commits() {
    ifstream ifile;
    
    ifile.open("data/commits.txt", ios::in);

    string line;
    int cnt = 0;  // 提交的个数
    int idx = 0;

    string commit_ish, tree_ish, author, email, date;
 

    while(getline(ifile, line)) {

        // if(cnt == 3) break;
        

        string out;
        istringstream in(line);
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
                GitCommit::commit_list[commit_ish]->add_tree(tree_ish);

                idx++;
                break;
            case 2:  // 获得它的父亲
                if (GitCommit::commit_list.size() == 1) {    //  第一个提交没有父亲
                    first_commit = GitCommit::commit_list[commit_ish];
                    idx ++;
                    break;
                }
                do {
                    GitCommit::commit_list[commit_ish]->add_parents(out);
                    GitCommit::commit_list[out]->add_child(commit_ish);
                } while(in >> out);
                
                idx ++;
                break;
            case 3:  // 作者
                author = "";
                do
                    author += out + ' ';
                while(in >> out);
                GitCommit::commit_list[commit_ish]->add_author(author);
                idx++;
                break;
            case 4:  // 邮箱
                email = "";
                do
                    email += out + ' ';
                while(in >> out);
                GitCommit::commit_list[commit_ish]->add_email(email);
                idx ++;

                break;
            case 5:  // 日期
                date = "";
                do
                    date += out + ' ';
                while(in >> out);
                GitCommit::commit_list[commit_ish]->add_date(date);
                idx ++;

                break;

            case 6:  // commit message
                break;


        }

    }
    cout << "commit数量: " << GitCommit::commit_list.size() << endl;
    // cout << "child: " << GitCommit::commit_list["440ecc9"]->children[0]->commit_ish << endl;
    // cout << "p:" << GitCommit::commit_list["dfb77fb"]->pcnt << endl;
}

void get_file_stat() {
    cout << "初始化..." << endl;
    file_list_init();  
    cout << "初始化完毕" << endl;
    // first_commit->print_file_list();

    
    queue<GitCommit*> q;
    q.push(first_commit);
    int cnt = 0;
    while(q.size()) {
        auto cur = q.front();
        


        q.pop();
        cur->diff_parents(git_dir);



       for(auto& child: cur->children){


            if(child->pcnt == 1){
                q.push(child);

                child->pcnt = child->parents.size();
            } else {
                child->pcnt = child->pcnt - 1;
            }
        }
        cnt ++;
        cout << "图构建中: " << cnt << " / " << GitCommit::commit_list.size() << "\r";
    }
    // first_commit->print_file_list();     // 打印file list
}

void file_list_init() {  // 初始化文件列表，从仓库的第一个commit开始
    if(GitCommit::commit_list.empty()) return ; 

    string first_commit_ish = first_commit->commit_ish;
    string first_tree_ish = first_commit->tree_ish;
    string path = "";
    ish_len = first_commit_ish.size();

    read_tree(first_commit_ish, first_tree_ish, path);
}


void read_tree(string& commit_ish, string& tree_ish, string path) {  // 深度优先读取树

    string cmd = git_dir + " git cat-file -p " + tree_ish;
    FILE *fp = NULL;
    if ((fp = popen(cmd.c_str(), "r")) != NULL) {
        char read_str[1024] = "";
        while(fgets(read_str, sizeof(read_str), fp)) {
            istringstream in(read_str);
            string out;            
            in >> out;
            if (out.empty()) continue;
            string filemode = out;
            in >> out;
            if (out == "blob") {
                in >> out;
                string blob_ish = out;
                blob_ish = blob_ish.substr(0, ish_len);
                in >> out;
                string file_name = path + out;
                auto t = new FileHead(file_name, blob_ish, commit_ish);
                GitCommit::file_list[file_name + ' ' + blob_ish] = t;
                GitCommit::same_name_file[file_name].push_back(t);


            } else if(out == "tree") {
                in >> out;
                string son_ish = out;
                in >> out;
                read_tree(commit_ish, son_ish, path + out + '/');
            }
        }
    }
    pclose(fp);
}



void gen_csv() {

    ofstream o1("data/csv/heads_header.csv");
    o1.clear();
    o1 << "headId:ID(Head-ID),:LABEL\n";
    o1.close();
    ofstream o2("data/csv/head_relation_header.csv");
    o2 << ":START_ID(Head-ID),:END_ID(Node-ID),:TYPE\n";
    o2.close();
    ofstream o3("data/csv/nodes_header.csv");
    o3 << "nodeId:ID(Node-ID),name,blobHash,commitHash,:LABEL\n";
    o3.close();
    ofstream o4("data/csv/node_relation_header.csv");
    o4 << ":START_ID(Node-ID),:END_ID(Node-ID),:TYPE\n";
    o4.close();


    // char s1[30] = "headId:ID(Head-ID),:LABEL";
    // system(strcat(s1, " > data/csv/heads_header.csv"));
    // char s2[50] = ":START_ID(Head-ID),:END_ID(Node-ID),:TYPE";
    // system(strcat(s2, " > data/csv/head_relation_header.csv"));
    // char s3[100] = "nodeId:ID(Node-ID),name,blobHash,commitHash,:LABEL";
    // system(strcat(s3, " > data/csv/nodes_header.csv"));
    // char s4[100] = ":START_ID(Node-ID),:END_ID(Node-ID),:TYPE";
    // system(strcat(s4, " > data/csv/node_relation_header.csv"));
    for (auto& fh: GitCommit::file_list) {
        bfs(fh.second);
    }
}

unordered_map<FileNode*, int> st;

void bfs(FileHead* head) {
    static int idx = 0;
    idx ++;
    auto t = head->head;
    w_head(head, idx);
    if (!st.count(t)) w_node(t);
    // else {
    //     cout << "bfs\n" << t << endl;
    //     t->print();
    //     cout << endl;
    //     // exit(2);
    // }
    w_h2n(head, head->head, idx);
    queue<FileNode*> q;
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
    string line = to_string(idx) + ",head\n";
    ofstream out;
    out.open("data/csv/heads.csv", ios::app);
    out << line;
    out.close();
}

void w_node(FileNode* node) {  // 写入nodes.csv
    static int idx = 0;
    idx ++;
    string line = to_string(idx) + ',' + node->file_name + ',' + node->blob_ish + ',' + node->commit_ish + ",node\n";
    ofstream out; 
    out.open("data/csv/nodes.csv", ios::app);
    out << line;
    out.close();
    st[node] = idx;
}

void w_h2n(FileHead* head, FileNode* node, int head_idx) {  // 写入head_relation.csv
    int node_idx = st[node];
    string line = to_string(head_idx) + ',' + to_string(node_idx) + ",head\n";
    ofstream out;
    out.open("data/csv/head_relation.csv", ios::app);
    out << line;
    out.close();
}

void w_n2n(FileNode* cur, FileNode* ne) {  // 写入node_relation.csv
    int cur_idx = st[cur];
    int ne_idx = st[ne];
    string line = to_string(cur_idx) + ',' + to_string(ne_idx) + ",is parent\n";
    ofstream out;
    out.open("data/csv/node_relation.csv", ios::app);
    out << line;
    out.close();
}



void test() {
    int n = 3;
    cout << "\n\ntest" << 

    n
    
    << "\n\n";
}


