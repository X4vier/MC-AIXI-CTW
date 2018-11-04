#ifndef __SEARCH_HPP__
#define __SEARCH_HPP__

#include "main.hpp"

class Agent;
class SearchNode;

// determine the best action by searching ahead
// extern action_t search(Agent &agent);

typedef unsigned long long visits_t;

// search options
static const visits_t MinVisitsBeforeExpansion = 1;
static const unsigned int MaxDistanceFromRoot = 100;
static size_t MaxSearchNodes;

typedef std::map<int, SearchNode*> child_map_t;

// contains information about a single "state"
class SearchNode
{

  public: 
	SearchNode(bool is_chance_node);
	~SearchNode();

	// determine the next action to play
	action_t selectAction(Agent &agent, unsigned int m) const; // TODO: implement

	// determine the expected reward from this node
	reward_t expectation(void) const { return m_mean; }

	// perform a sample run through this node and it's children,
	// returning the accumulated reward from this sample run
	reward_t sample(Agent &agent, unsigned int m, const void *backdoor_to_real_env); // TODO: implement

	// number of times the search node has been visited
	visits_t visits(void) const { return m_visits;}

    SearchNode* findChild(const int aor) const;

	reward_t playout(Agent &agent, unsigned int playout_len, const void *backdoor_to_real_env);
	//reward_t playout(Agent &agent, unsigned int playout_len, action_t a);

	// action_t act = -1;
	
	static long long stat_new,stat_del;

  private:
	bool m_chance_node; // true if this node is a chance node, false otherwise
	double m_mean;		// the expected reward of this node
	visits_t m_visits;  // number of times the search node has been visited
    child_map_t m_child;



	// TODO: decide how to reference child nodes
	//  e.g. a fixed-size array
};

#endif // __SEARCH_HPP__
