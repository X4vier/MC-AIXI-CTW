#include <cmath>
#include "search.hpp"
#include "agent.hpp"
#include "util.hpp"
#include "environment.hpp"

long long SearchNode::stat_new=0;
long long SearchNode::stat_del=0;

SearchNode::SearchNode(bool is_chance_node)
{
	++stat_new;
	m_mean = 0;
	m_visits = 0;
	m_chance_node = is_chance_node;
	int m_num_action = 0;
}

SearchNode::~SearchNode(void)
{
	++stat_del;
	for(child_map_t::iterator iter = m_child.begin(); iter != m_child.end(); iter++)
	{
		delete iter->second;
	}
}

action_t SearchNode::selectAction(Agent &agent, unsigned int m) const
{
	// return NULL;
	action_t a_star;
	double max_V_plus = -inf;
	double log_Th = std::log(visits());
	double C2 = 2;

	for(action_t a = 0; a< agent.numActions(); a++)
	{
		SearchNode* child = findChild(a);

		double v_plus;

		if(child == NULL)
		{
			v_plus = inf;
		}
		else
		{
			int Tha = child->visits();
			// v_plus = child->expectation()/(m*(agent.maxReward() - agent.minReward() + 0.0) + std::sqrt(C2 * log_Th / Tha));
			v_plus = child->expectation() + std::sqrt(C2 * log_Th / Tha);
		}

		int num_ties = 1;

		if(v_plus > max_V_plus)
		{
			a_star = a;
			max_V_plus = v_plus;
			num_ties = 1;
		}
		else if(v_plus == max_V_plus)
		{
			a_star = (rand01() < (num_ties + 0.0) / (num_ties + 1)) ? a_star : a; 
			num_ties++;
		}
	}

	return a_star;
}

// simulate a path through a hypothetical future for the agent within it's
// internal model of the world, returning the accumulated reward.
reward_t SearchNode::playout(Agent &agent, unsigned int playout_len, const void *backdoor_to_real_env)
{
	// return 0; // TODO: implement
	reward_t reward = 0.0;

	if(backdoor_to_real_env){
		Environment *v = ((const Environment *)backdoor_to_real_env) -> clone();
		while(playout_len-- > 0) {
			action_t a = agent.genRandomAction();
			v->performAction(a);
			percept_t r = v->getReward();
			reward += r;
		}
		delete v;
	}else{
		while(playout_len-- > 0) {
			action_t a = agent.genRandomAction();
			agent.modelUpdate(a);

			percept_t percept, r;
			agent.genPerceptAndUpdate(percept, r);
			reward += r;
		}
	}

	return reward;
}

// reward_t SearchNode::playout(Agent &agent, unsigned int playout_len, action_t a)
// {
// 	// return 0; // TODO: implement
// 	reward_t reward = 0.0;

// 	agent.modelUpdate(a);

// 	percept_t percept, r;
// 	agent.genPerceptAndUpdate(percept, r);
// 	reward += r;

// 	playout_len--;

// 	while(playout_len-- > 0)
// 	{
// 		action_t a = agent.genRandomAction();
// 		agent.modelUpdate(a);

// 		percept_t percept, r;
// 		agent.genPerceptAndUpdate(percept, r);
// 		reward += r;
// 	}

// 	return reward;
// }


reward_t SearchNode::sample(Agent &agent, unsigned int m, const void *backdoor_to_real_env)
{
	reward_t reward = 0;

	if(m==0)
	{
		return reward;
	}
	else if(this->m_chance_node)
	{
		percept_t percept, r;
		agent.genPerceptAndUpdate(percept, r);
		if (!findChild(percept))
		{
			m_child[percept] = new SearchNode(false);
		}
		reward = r + m_child[percept]->sample(agent, m - 1, backdoor_to_real_env);
	}
	else if(!this->visits())
	{
		// if(this->act >= 0 && this->act < agent.numActions())
		// {
		// 	reward = playout(agent, m, this->act);
		// }
		// else
		// {
		// 	reward = playout(agent, m);
		// }
		reward = playout(agent, m, backdoor_to_real_env);
	}
	else
	{
		action_t a = selectAction(agent, m);
		agent.modelUpdate(a);

		if(findChild(a) == NULL)
		{
			m_child[a] = new SearchNode(true);
			// m_child[a]->act = a;
		}
		reward = m_child[a]->sample(agent, m, backdoor_to_real_env);
		//if a is child
	}

	double T = visits();
	this->m_mean = (reward + expectation() * T)/(T + 1);
	this->m_visits++;

	return reward;
}

SearchNode* SearchNode::findChild(const int aor) const
{
	child_map_t::const_iterator iter = m_child.find(aor);
	return iter == m_child.end() ? NULL : iter->second;
}


// determine the best action by searching ahead using MCTS
// extern action_t search(Agent &agent)
// {
// 	// return agent.genRandomAction(); // TODO: implement

// }
