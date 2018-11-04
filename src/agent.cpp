#include "agent.hpp"
#include <cassert>
#include "predict.hpp"
#include "search.hpp"
#include "util.hpp"


// construct a learning agent from the command line arguments
Agent::Agent(options_t &options)
{
	std::string s;

	strExtract(options["agent-actions"], m_actions);
	strExtract(options["agent-horizon"], m_horizon);
	strExtract(options["observation-bits"], m_obs_bits);
	strExtract<unsigned int>(options["reward-bits"], m_rew_bits);

	// calculate the number of bits needed to represent the action
	for (unsigned int i = 1, c = 1; i < m_actions; i *= 2, c++)
	{
		m_actions_bits = c;
	}

	m_ct = new ContextTree(strExtract<unsigned int>(options["ct-depth"]));

	reset();
}

// destruct the agent and the corresponding context tree
Agent::~Agent(void)
{
	if (m_ct)
		delete m_ct;
}

action_t Agent::search(int num_search, const void *backdoor_to_real_env)
{
	ModelUndo ckpt = ModelUndo(*this);

	SearchNode* MCST = new SearchNode(false);

	for(int i = 0; i < num_search; i++)
	{
		MCST->sample(*this, m_horizon, backdoor_to_real_env);
		modelRevert(ckpt);
	}

	action_t a_star;
	reward_t V_a_star = -inf;

	int num_ties = 1;

	for(action_t a = 0; a < numActions(); a++)
	{
		if (!MCST->findChild(a))
		{
			continue;
		}

		reward_t V_a = MCST->findChild(a)->expectation();

		if(V_a > V_a_star)
		{
			a_star = a;
			V_a_star = V_a;
			num_ties = 1;
		}
		else if(V_a == V_a_star)
		{
			a_star = (rand01() < (num_ties + 0.0) / (num_ties + 1)) ? a_star : a; 
			num_ties ++;
		}

	}

	delete MCST;

	return a_star;

}

// current lifetime of the agent in cycles
lifetime_t Agent::lifetime(void) const
{
	return m_time_cycle;
}

// the total accumulated reward across an agents lifespan
reward_t Agent::reward(void) const
{
	return m_total_reward;
}

// the average reward received by the agent at each time step
reward_t Agent::averageReward(void) const
{
	return lifetime() > 1 ? reward() / reward_t(lifetime()-1) : 0.0;
}

// maximum reward in a single time instant
reward_t Agent::maxReward(void) const
{
	return reward_t((1 << m_rew_bits) - 1);
}

// minimum reward in a single time instant
reward_t Agent::minReward(void) const
{
	return 0.0;
}

// number of distinct actions
unsigned int Agent::numActions(void) const
{
	return m_actions;
}

// the length of the stored history for an agent
size_t Agent::historySize(void) const
{
	return m_ct->historySize();
}

// length of the search horizon used by the agent
size_t Agent::horizon(void) const
{
	return m_horizon;
}

// generate an action uniformly at random
action_t Agent::genRandomAction(void) const
{
	return randRange(m_actions);
}

// generate a percept distributed according
// to our history statistics
// percept_t Agent::genPercept(void) const
// {
// 	return NULL; // TODO: implement
// }

void Agent::genPercept(percept_t &percept, percept_t &r) const
{
	symbol_list_t syms;
	m_ct->genRandomSymbols(syms, m_obs_bits + m_rew_bits);
	r = decodeReward(syms);
	percept = decodePercept(syms);

}

// generate a percept distributed to our history statistics, and
// update our mixture environment model with it
// percept_t Agent::genPerceptAndUpdate(void)
// {
// 	  return NULL; // TODO: implement
// }

void Agent::genPerceptAndUpdate(percept_t &percept, percept_t &r)
{
	symbol_list_t syms;
	// syms.resize(m_obs_bits + m_rew_bits);
	m_ct->genRandomSymbolsAndUpdate(syms, m_obs_bits + m_rew_bits);
	r = decodeReward(syms);
	percept = decodePercept(syms);

	m_last_update_percept = true;
}

// Update the agent's internal model of the world after receiving a percept
void Agent::modelUpdate(percept_t observation, percept_t reward)
{
	// Update internal model
	symbol_list_t percept;
	encodePercept(percept, observation, reward);

	m_ct->update(percept);
	// m_ct->updateHistory(percept);

	// Update other properties
	assert(reward >= 0);
	assert(reward <= maxReward());
	m_total_reward += reward;
	m_last_update_percept = true;
}

// Update the agent's internal model of the world after performing an action
void Agent::modelUpdate(action_t action)
{

	assert(isActionOk(action));
	assert(m_last_update_percept == true);

	// Update internal model
	symbol_list_t action_syms;
	encodeAction(action_syms, action);
	m_ct->update(action_syms);
	// m_ct->updateHistory(action_syms);

	m_time_cycle++;
	m_last_update_percept = false;
}

// revert the agent's internal model of the world
// to that of a previous time cycle, false on failure
bool Agent::modelRevert(const ModelUndo &mu)
{

	size_t diff = historySize() - mu.historySize();

	while(diff>0)
	{
		m_ct->revert();
		diff--;
	}

	// m_ct->revertHistory(mu.historySize());

	m_time_cycle = mu.lifetime();
	m_total_reward = mu.reward();
	m_last_update_percept = mu.lastUpdate();
	return true;
}

void Agent::reset(void)
{
	m_ct->clear();

	m_time_cycle = 0;
	m_total_reward = 0.0;
}

// probability of selecting an action according to the
// agent's internal model of it's own behaviour
// double Agent::getPredictedActionProb(action_t action)
// {
// 	symbol_list_t action_syms;
// 	encodeAction(action_syms, action);
// 	return m_ct->predict(action_syms);
// 	// return NULL; // TODO: implement
// }

// get the agent's probability of receiving a particular percept
// double Agent::perceptProbability(percept_t observation, percept_t reward) const
// {
// 	symbol_list_t or_syms;
// 	encodePercept(or_syms, observation, reward);
// 	return m_ct->predict(or_syms);

// 	// return NULL; // TODO: implement
// }

// action sanity check
bool Agent::isActionOk(action_t action) const
{
	// printf("%d\n", action);
	return action < m_actions;
}

// reward sanity check
bool Agent::isRewardOk(reward_t reward) const
{
	return reward >= minReward() && reward <= maxReward();
}

// Encodes an action as a list of symbols
void Agent::encodeAction(symbol_list_t &symlist, action_t action) const
{
	symlist.clear();

	encode(symlist, action, m_actions_bits);
}

// Encodes a percept (observation, reward) as a list of symbols
void Agent::encodePercept(symbol_list_t &symlist, percept_t observation, percept_t reward) const
{
	symlist.clear();

	encode(symlist, observation, m_obs_bits);
	encode(symlist, reward, m_rew_bits);
}

// Decodes the observation from a list of symbols
action_t Agent::decodeAction(const symbol_list_t &symlist) const
{
	return decode(symlist, m_actions_bits);
}

// Decodes the reward from a list of symbols
percept_t Agent::decodeReward(const symbol_list_t &symlist) const
{
	return decode(symlist, m_rew_bits);
}

percept_t Agent::decodePercept(const symbol_list_t &symlist) const
{
	return decode(symlist, m_obs_bits);
}

// percept_t Agent::decodeObservation(const symbol_list_t &symlist) const
// {
// 	return decode(symlist, m_obs_bits);
// }

// used to revert an agent to a previous state
ModelUndo::ModelUndo(const Agent &agent)
{
	m_lifetime = agent.lifetime();
	m_reward = agent.reward();
	m_history_size = agent.historySize();
	m_last_update_percept = agent.getLastUpdate();
}

