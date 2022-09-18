#include "GitCommit.h"
#include "FileGraph.h"

#include <regex>
#include <sstream>
#include <fstream>
#include <string>
#include <string.h>

using namespace std;

void get_commits();
string get_dir(string git_dir);
void read_commits();
void get_file_stat();
void file_list_init();
void read_tree(string& commit_ish, string& tree_ish, string path);

const string start_flag = "***adfa*asdfo**";
const string end_flag = "**dasjf93hd9f()&&^";

GitCommit* first_commit;
int ish_len;
string git_dir;  // git仓库的文件路径



int main(int ac, char** av) {
    
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

        string out;
        istringstream in(line);
        in >> out;



        if (!line.compare(start_flag)) {
            idx = 0;
            continue;
        }

        if (!line.compare(end_flag)) {
            continue;
        }
        switch(idx) {
            case 0:   // 一个新的提交
                commit_ish = out;
                GitCommit::commit_list[commit_ish] = new GitCommit(commit_ish);
                first_commit = GitCommit::commit_list[commit_ish];
                idx++;
                break;
            case 1:   // tree对象
                tree_ish = out;
                GitCommit::commit_list[commit_ish]->add_tree(tree_ish);

                idx++;
                break;
            case 2:  // 获得它的父亲
                if (GitCommit::commit_list.size() == 1) {    //  第一个提交没有父亲

                    idx ++;
                    break;
                }
                do {
                    GitCommit::commit_list[commit_ish]->add_parents(out);
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
    cout << "commit数量: " << GitCommit::commit_list.size() << endl;;
}

void get_file_stat() {
    cout << "初始化..." << endl;
    file_list_init();  
    first_commit->print_file_list();
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













