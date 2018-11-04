#include "predict.hpp"
#include <cassert>
#include <ctime>
#include <cmath>
#include "util.hpp"
#include <iostream>

long long CTNode::stat_new;
long long CTNode::stat_del;

CTNode::CTNode(void) :
	m_log_prob_est(0.0),
	m_log_prob_weighted(0.0)
{
	++stat_new;
	m_count[0] = 0;
	m_count[1] = 0;
	m_child[0] = NULL;
	m_child[1] = NULL;
}


CTNode::~CTNode(void) {
	++stat_del;
	if (m_child[0]) delete m_child[0];
	if (m_child[1]) delete m_child[1];
}


// number of descendants of a node in the context tree
size_t CTNode::size(void) const {

    size_t rval = 1;
    rval += child(false) ? child(false)->size() : 0;
    rval += child(true)  ? child(true)->size()  : 0;
    return rval;
}


// compute the logarithm of the KT-estimator update multiplier
// equations (23) and (24) in MC-AIXI-CTW paper
double CTNode::logKTMul(symbol_t sym) const {
	double KTMul = (m_count[sym] + 0.5) / (m_count[0] + m_count[1] + 1);
	return std::log (KTMul); // TODO: test
}

// Calculate the logarithm of the weighted block probability.
void CTNode::updateLogProbability(void) {
    if (child(0) && child(1)) {
		weight_t child_0_log_p = child(0) -> logProbWeighted();
		weight_t child_1_log_p = child(1) -> logProbWeighted();
		weight_t children_log_p = child_0_log_p + child_1_log_p;

		// Calculate the log weighted probability. Use the formulation which
		// has the least chance of overflow (see function doc for details).
		weight_t a = m_log_prob_est > children_log_p ? m_log_prob_est : children_log_p;
		weight_t b = m_log_prob_est < children_log_p ? m_log_prob_est : children_log_p;
		m_log_prob_weighted = log(0.5) + a + std::log(1.0 + std::exp(b - a));

		// weight_t m_prob_weighted = 0.5 * (exp(child_0_log_p + child_1_log_p) + exp(m_log_prob_est));
	}
	else
		m_log_prob_weighted = m_log_prob_est;
}

// Update the node after having observed a new symbol.
void CTNode::update(const symbol_t symbol){
	m_log_prob_est += CTNode::logKTMul(symbol);
	m_count[symbol] ++;

}

void CTNode::revert(const symbol_t symbol){
	m_count[symbol] --;
	m_log_prob_est -= CTNode::logKTMul(symbol);
}

// create a context tree of specified maximum depth
ContextTree::ContextTree(size_t depth) :
	m_root(new CTNode()),
	m_depth(depth)
{ return; }


ContextTree::~ContextTree(void) {
	if (m_root) delete m_root;
}


// clear the entire context tree
void ContextTree::clear(void) {
	m_history.clear();
	if (m_root) delete m_root;
	m_root = new CTNode();
}


void ContextTree::update(const symbol_t sym) {
	// TODO: work out whether doing nothing when history is short is the correct way to handle this
	
	m_history.push_back(sym);
	
	if (historySize() <= depth()) {
		return;
	}
	// int update_depth = (depth() >= historySize()) ? historySize() : depth();
	
	// std::cout << update_depth;
	std::vector<CTNode *> nodepath;
	nodepath.push_back(m_root);
	m_root -> update(sym);
	symbol_t prev_symbol;
	CTNode * currentnode = m_root;


	// going down the tree, updating the symbol counts
	while (nodepath.size() <= depth()) {
		if (!currentnode -> child(0)) {
			currentnode -> m_child[0] = new CTNode;
			currentnode -> m_child[1] = new CTNode;
		}
		prev_symbol = m_history[historySize() - nodepath.size() - 1];
		nodepath.push_back (currentnode -> m_child[prev_symbol]);
		currentnode = nodepath.back();
		currentnode -> update(sym);
	};

	// going back up the tree, updating the probability estimates
	while (!nodepath.empty()) {
		nodepath.back() -> updateLogProbability();
		nodepath.pop_back();
	}

	assert (historySize() + depth() >= m_root -> visits());
}


void ContextTree::update(const symbol_list_t &symbol_list) {
	for (size_t i = 0; i < symbol_list.size(); i++) {
		ContextTree::update(symbol_list[i]);
	}
}


// updates the history statistics, without touching the context tree
void ContextTree::updateHistory(const symbol_list_t &symbol_list) {

    for (size_t i=0; i < symbol_list.size(); i++) {
        m_history.push_back(symbol_list[i]);
    }
}


// removes the most recently observed symbol from the context tree
void ContextTree::revert(void) {

	//int update_depth = (depth() >= historySize()) ? (historySize() - 1) : depth();
	
	if (historySize() > depth()) {
		
		std::vector<CTNode *> nodepath;
		nodepath.push_back(m_root);
		symbol_t sym = m_history.back();
		m_root -> revert(sym);
		symbol_t prev_symbol;
		CTNode * currentnode = m_root;


		// going down the tree, updating the symbol counts
		while (nodepath.size() <= depth()) {
			// if (currentnode -> visits() == 0) {
			// 	delete currentnode -> m_child[0];
			// 	delete currentnode -> m_child[1];
			// }
			prev_symbol = m_history[historySize() - nodepath.size() - 1];
			if (currentnode -> m_child[prev_symbol]) {
				nodepath.push_back (currentnode -> m_child[prev_symbol]);
			}
			currentnode = nodepath.back();
			currentnode -> revert(sym);
		};

		// going back up the tree, updating the probability estimates
		while (!nodepath.empty()) {
			// Delete children if necessary
			nodepath.back() -> updateLogProbability();
			if (nodepath.back() && nodepath.back() -> visits() == 0 && nodepath.back() -> m_child[0] && nodepath.back() -> m_child[1] && nodepath.size() > 1) {
				delete nodepath.back() -> m_child[0];// -> ~CTNode();
				delete nodepath.back() -> m_child[1];// -> ~CTNode();
				nodepath.back() -> m_child[0] = NULL;
				nodepath.back() -> m_child[1] = NULL;
				
			}
				
			nodepath.pop_back();
		}

		assert (historySize() + depth() >= m_root -> visits());
		assert (m_root);

	}
	
	
	// remove most recent bit from history
	m_history.pop_back();

}


// shrinks the history down to a former size
// NOTE: this method keeps the old observations and discards the most recent bits
void ContextTree::revertHistory(size_t newsize) {

    assert(newsize <= m_history.size());
    while (m_history.size() > newsize) revert();
}



// generate a specified number of random symbols
// distributed according to the context tree statistics
void ContextTree::genRandomSymbols(symbol_list_t &symbols, size_t bits) {

	genRandomSymbolsAndUpdate(symbols, bits);

	// restore the context tree to it's original state
	for (size_t i=0; i < bits; i++) revert();
}


// generate a specified number of random symbols distributed according to
// the context tree statistics and update the context tree (and history) with the newly
// generated bits
void ContextTree::genRandomSymbolsAndUpdate(symbol_list_t &symbols, size_t bits) {

	int target_hist_size = historySize() + bits;
	while (historySize() < target_hist_size) {

		double r = rand01(); //See util.hpp. 
		double log_p_before_symbol = logBlockProbability();
		int s = 0;
		update(0);		// Assume the next bit is a 0, and correct this assumption later if necessary
		//double log_p_after_symbol = logBlockProbability();
		//double p_next_bit_0 = exp(log_p_after_symbol - log_p_before_symbol);
		double p_next_bit_0 = exp(logBlockProbability() - log_p_before_symbol);
		if (p_next_bit_0 < r) {
			revert();
			update(1);
			s = 1;
		}
		symbols.push_back(s);
	}
}


// the logarithm of the block probability of the whole sequence
double ContextTree::logBlockProbability(void) const {
    return m_root->logProbWeighted();
}


// get the n'th most recent history symbol, NULL if doesn't exist
const symbol_t *ContextTree::nthHistorySymbol(size_t n) const {
    return n < m_history.size() ? &m_history[n] : NULL;
}






double ContextTree::predict(symbol_t s){
	long long stat0 = CTNode::stat_new - CTNode::stat_del;
	history_t hs0 = m_history;
	weight_t p0 = logBlockProbability();
	update(s);
	weight_t p1 = logBlockProbability();
	revert();
	assert(logBlockProbability() == p0); //fail
	assert( hs0 == m_history); //pass
	assert( 0 == CTNode::stat_new - CTNode::stat_del - stat0 ); //fail 
	//prediction should not change number of outstanding tree nodes.
	//Not thread safe though
	if (p1 > -0.0000001) { // if we don't have enough information to make an informed guess (this is a log probability)
		return 0.5;
	}
	return std::exp(p1 - p0);
}
