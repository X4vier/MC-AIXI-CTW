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
    // symbol_list_t history = {0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0};
    for(int depth=1;depth<10;++depth){
       cout << "depth="<<depth<<endl;
       ContextTree mytree ( depth );
          symbol_list_t history = {0,1,0,1};
       mytree.update(history);

       for (int i = 0; i < mytree.m_history.size(); i ++) cout << mytree.m_history[i];
       cout << endl;
       cout << mytree.predict(0) << endl;
       cout << mytree.predict(1) << endl;
       cout << mytree.predict(0) << endl;
       cout << mytree.predict(1) << endl;

       for (int i = 0; i < mytree.m_history.size(); i ++) cout << mytree.m_history[i];
       cout << endl;
       mytree.update(history);
       cout << mytree.predict(0) << endl;
       cout << mytree.predict(1) << endl;
       cout << mytree.predict(0) << endl;
       cout << mytree.predict(1) << endl;
    }

    for(int depth=1;depth<10;++depth){
       cout << "depth="<<depth<<endl;
       ContextTree mytree ( depth ); 
       symbol_list_t history = {1,0,1,0};

       mytree.update(history);
       for (int i = 0; i < mytree.m_history.size(); i ++) cout << mytree.m_history[i];
       cout << endl;
       cout << mytree.predict(0) << endl;
       cout << mytree.predict(1) << endl;
       cout << mytree.predict(0) << endl;
       cout << mytree.predict(1) << endl;

       mytree.update(history);
       for (int i = 0; i < mytree.m_history.size(); i ++) cout << mytree.m_history[i];
       cout << endl;
       cout << mytree.predict(0) << endl;
       cout << mytree.predict(1) << endl;
       cout << mytree.predict(0) << endl;
       cout << mytree.predict(1) << endl;
    }

    for(int depth=1;depth<10;++depth){
       cout << "depth="<<depth<<endl;
       ContextTree mytree ( depth ); 
       symbol_list_t history = {1,0,0,0,1,0,0,0};

       mytree.update(history);
       for (int i = 0; i < mytree.m_history.size(); i ++) cout << mytree.m_history[i];
       cout << endl;
       cout << mytree.predict(0) << endl;
       cout << mytree.predict(1) << endl;
       cout << mytree.predict(0) << endl;
       cout << mytree.predict(1) << endl;

       mytree.update(history);
       for (int i = 0; i < mytree.m_history.size(); i ++) cout << mytree.m_history[i];
       cout << endl;
       cout << mytree.predict(0) << endl;
       cout << mytree.predict(1) << endl;
       cout << mytree.predict(0) << endl;
       cout << mytree.predict(1) << endl;
    }



    // CTNode * internalnode = mytree.m_root -> child(0);

    //cout << mytree.m_root -> m_count[0] << endl;
    //cout << mytree.m_root -> m_count[1] << endl;

    //cout << mytree.m_root -> child(0) -> m_count[0] << endl;
    //cout << mytree.m_root -> child(0) -> m_count[1] << endl;

    //cout << mytree.m_root -> child(1) -> m_count[0] << endl;
    //cout << mytree.m_root -> child(1) -> m_count[1] << endl;

    //cout << mytree.m_root -> logProbWeighted() << endl;
    //cout << mytree.m_root -> logProbEstimated() << endl;
    
    //symbol_list_t dummy;
    //mytree.genRandomSymbolsAndUpdate(dummy, 2);

    //for (int i = 0; i < mytree.m_history.size(); i ++) {
    //    cout << mytree.m_history[i];
    //}
    //cout << endl;
    
}

int main() {

    TreeTest mytest;
    mytest.treeTest();



    return 0;
}
