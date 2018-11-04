#ifndef __PREDICT_HPP__
#define __PREDICT_HPP__

#include <deque>

#include "main.hpp"

// stores symbol occurrence counts
typedef unsigned int count_t;

// holds context weights
typedef double weight_t;

// stores the agent's history in terms of primitive symbols
typedef std::deque<symbol_t> history_t;

class ContextTree {
    friend class TreeTest;
public:

	// create a context tree of specified maximum depth
	ContextTree(size_t depth);

	~ContextTree(void);

	// clear the entire context tree
	void clear(void);

    // updates the context tree with a new binary symbol
    void update(const symbol_t sym); // TODO: implement in predict.cpp
    // update the context tree with a list of symbols.
    void update(const symbol_list_t &symbol_list); // TODO: implement in predict.cpp
    // add a symbol to the history without updating the context tree.
    void updateHistory(const symbol_list_t &symbol_list);

    //Recalculate the log weighted probability for this node.
    void updateLogProbability(void);

    // removes the most recently observed symbol from the context tree
    void revert(void); // TODO: implement in predict.cpp

    // shrinks the history down to a former size
    void revertHistory(size_t newsize);

    // the estimated probability of observing a particular symbol or sequence
    double predict(symbol_t sym); // TODO: implement in predict.cpp
    double predict(symbol_list_t symbol_list); // TODO: implement in predict.cpp

    // generate a specified number of random symbols
    // distributed according to the context tree statistics
    void genRandomSymbols(symbol_list_t &symbols, size_t bits);

    // generate a specified number of random symbols distributed according to
    // the context tree statistics and update the context tree with the newly
    // generated bits
    void genRandomSymbolsAndUpdate(symbol_list_t &symbols, size_t bits); // TODO: implement in predict.cpp

    // the logarithm of the block probability of the whole sequence
	double logBlockProbability(void);

    // get the n'th history symbol, NULL if doesn't exist
    const symbol_t *nthHistorySymbol(size_t n) const;

    // the depth of the context tree
    size_t depth(void) const { return m_depth; }

    // the size of the stored history
    size_t historySize(void) const { return m_history.size(); }

    // number of nodes in the context tree
    size_t size(void) const { return 0; }

private:
    history_t m_history; // the agents history
    // CTNode *m_root;      // the root node of the context tree
    size_t m_depth;      // the maximum depth of the context tree
    action_t last_action;

};

#endif // __PREDICT_HPP__
