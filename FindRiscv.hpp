#ifndef _FINDRISCV_H_
#define _FINDRISCV_H_

#include "GitCommit.h"
#include <fstream>


void find_riscv_commits(std::vector<GitCommit*> first_commits) {
    std::unordered_set<GitCommit*> st;
    std::ofstream ofile("data/rv_commits.txt");
    std::queue<GitCommit*> q;
    for (auto commit: first_commits) {
        st.emplace(commit);
        q.push(commit);
    }
    int rv_cnt = 0, cnt = 0;
    while(q.size()) {
        auto cur = q.front();
        q.pop();
        if (cur->is_rv) {
            ofile << "commit hash: " << cur->commit_ish << '\n';
            ofile << "commit tree: " << cur->tree_ish << '\n';
            ofile << "commit author: " << cur->author << '\n';
            if (cur->parents.size()) {
                for (auto p: cur->parents) {
                    ofile << "parent: " << p->commit_ish << '\n';
                }
            }
            if (cur->children.size()) {
                for (auto c: cur->children) {
                    ofile << "child: " << c->commit_ish << '\n';
                }
            }
            std::cout << "rv commit: " << rv_cnt << "   \r";
            rv_cnt ++;
        }
        for (auto p: cur->parents) {
            
        }
        for (auto c: cur->children) {
            // if (st.count(c)) {
            //     std::cout << c->commit_ish << '\n';
            //     continue;
            // }
            // q.push(c);
            // st.emplace(c);
            if (c->pcnt == 1) {
                q.push(c);
                c->pcnt = c->parents.size();
            } else if (c->pcnt > 1) {
                c->pcnt --;
            }
        }
        cnt ++;
        // std::cout << cnt << " / " << GitCommit::commit_list.size() << '\r';
    }
    std::cout << "rv commit: " << rv_cnt << "   \n";
}





#endif