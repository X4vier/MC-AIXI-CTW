#include <iostream>
#include "predict.hpp"

using namespace std;

class TreeTest {
    public:
        void treeTest();
};


// symbol_list_t history = {0, 0, 0};
// symbol_list_t history = {1, 1, 1, 1, 1, 0, 0, 0};
void TreeTest::treeTest() {
    symbol_list_t history = {0, 0, 1, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0};
    // symbol_list_t history = {0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0};
    ContextTree mytree ( 3 );
    mytree.update(history);
    // mytree.update(1);
    // mytree.revert();
    // mytree.update(0);
    // mytree.update(0);
    // mytree.revert();
    // mytree.revert();

    mytree.update(1);

    cout << mytree.m_root -> logProbWeighted() << endl;
    cout << mytree.m_root -> logProbEstimated() << endl;

    for (int i = 0; i < mytree.m_history.size(); i ++) {
        cout << mytree.m_history[i];
    }
    cout << endl;
    symbol_list_t dummy;
    mytree.genRandomSymbolsAndUpdate(dummy, 15);
    mytree.revertHistory (14);
    mytree.update(1);
    mytree.revert();

    cout << mytree.m_root -> logProbWeighted() << endl;
    cout << mytree.m_root -> logProbEstimated() << endl;


    cout << endl;

    // CTNode * internalnode = mytree.m_root -> child(0);

    // cout << mytree.m_root -> m_count[0] << endl;
    // cout << mytree.m_root -> m_count[1] << endl;

    // cout << mytree.m_root -> child(0) -> m_count[0] << endl;
    // cout << mytree.m_root -> child(0) -> m_count[1] << endl;

    // cout << mytree.m_root -> child(1) -> m_count[0] << endl;
    // cout << mytree.m_root -> child(1) -> m_count[1] << endl;


    
    

    for (int i = 0; i < mytree.m_history.size(); i ++) {
        cout << mytree.m_history[i];
    }
    cout << endl;
    
}

int main() {

    TreeTest mytest;
    mytest.treeTest();



    return 0;
}
