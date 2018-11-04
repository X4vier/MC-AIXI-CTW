#include "predict_pseudo.hpp"
#include <cassert>
#include <ctime>
#include <cmath>
#include "util.hpp"

// create a context tree of specified maximum depth
ContextTree::ContextTree(size_t depth) { return; }

ContextTree::~ContextTree(void) { return; }

// clear the entire context tree
void ContextTree::clear(void) { return; }

void ContextTree::update(const symbol_t sym) { return; }

void ContextTree::update(const symbol_list_t &symbol_list)
{
    if (symbol_list.size() == 1)
    {
        last_action = symbol_list[0];
    }
    return;
}

// updates the history statistics, without touching the context tree
void ContextTree::updateHistory(const symbol_list_t &symbol_list) { return; }

// removes the most recently observed symbol from the context tree
void ContextTree::revert(void) { return; }

// shrinks the history down to a former size
// NOTE: this method keeps the old observations and discards the most recent bits
void ContextTree::revertHistory(size_t newsize) { return; }

// generate a specified number of random symbols
// distributed according to the context tree statistics
void ContextTree::genRandomSymbols(symbol_list_t &symbols, size_t bits)
{
    genRandomSymbolsAndUpdate(symbols, bits);
}

// generate a specified number of random symbols distributed according to
// the context tree statistics and update the context tree (and history) with the newly
// generated bits
void ContextTree::genRandomSymbolsAndUpdate(symbol_list_t &symbols, size_t bits)
{

    //srand(time(0)); Please don't call srand here.
    //In general we should call srand once in the main,
    //maybe srand(time(0)) by default and provide an option
    //to manually specify the seed so that
    //we can reproduce the exact behaviour when testing.

    if (rand01() < 0.7)
    {
        symbols.push_back(1);
        if (last_action)
        {
            symbols.push_back(1);
        }
        else
        {
            symbols.push_back(0);
        }
    }
    else
    {
        symbols.push_back(0);
        if (last_action)
        {
            symbols.push_back(0);
        }
        else
        {
            symbols.push_back(1);
        }
    }
}

// the logarithm of the block probability of the whole sequence
double ContextTree::logBlockProbability(void) { return 0.0; }

// get the n'th most recent history symbol, NULL if doesn't exist
const symbol_t *ContextTree::nthHistorySymbol(size_t n) const { return NULL; }
