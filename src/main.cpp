#include "main.hpp"
#include <cassert>
#include <fstream>
#include <iostream>
#include <string>
#include <cstdio>

#include "agent.hpp"
#include "environment.hpp"
#include "search.hpp"
#include "predict.hpp"
#include "util.hpp"

int max(int a, int b)
{
	return a>b ? a:b;
}

// Streams for logging
std::ofstream log_text;   // A verbose human-readable log
std::ofstream compactLog; // A compact comma-separated value log

// The main agent/environment interaction loop
void mainLoop(Agent &ai, Environment &env, options_t &options)
{

	// Determine exploration options
	bool explore = options.count("exploration") > 0;
	double explore_rate, explore_decay;
	if (explore)
	{
		strExtract(options["exploration"], explore_rate);
		strExtract(options["explore-decay"], explore_decay);
		assert(0.0 <= explore_rate && explore_rate <= 1.0);
		assert(0.0 <= explore_decay && explore_decay <= 1.0);
	}

	// Determine termination lifetime
	bool terminate_check = options.count("terminate-lifetime") > 0;
	lifetime_t terminate_lifetime;
	if (terminate_check)
	{
		strExtract(options["terminate-lifetime"], terminate_lifetime);
		assert(0 <= terminate_lifetime);
	}

	// int counter0 = 0;
	// int counter1 = 0;

	// Agent/environment interaction loop
	for (unsigned int cycle = 1; !env.isFinished(); cycle++)
	{

		// printf("%d\n", ai.historySize());

		// check for agent termination
		if (terminate_check && ai.lifetime() > terminate_lifetime)
		{
			log_text << "info: terminating lifetiment" << std::endl;
			break;
		}

		// Get a percept from the environment
		percept_t observation = env.getObservation();
		percept_t reward = env.getReward();

		// Update agent's environment model with the new percept
		ai.modelUpdate(observation, reward); // TODO: implement in agent.cpp

		// Determine best exploitive action, or explore
		action_t action;
		bool explored = false;
		if (explore && rand01() < explore_rate)
		{
			explored = true;
			action = ai.genRandomAction();
		}
		else if(options.count("parallel-worlds"))
		{
			action = ai.search(1000,&env);
		}
		else
		{
			action = ai.search(1000); // TODO: implement in search.cpp
		}

		// Send an action to the environment
		env.performAction(action); 
		assert(env.getReward()>=0);

		// Update agent's environment model with the chosen action
		// printf("%d", action);
		ai.modelUpdate(action);

		// Log this turn
		log_text << "cycle: " << cycle << std::endl;
		log_text << "observation: " << observation << std::endl;
		log_text << "reward: " << reward << std::endl;
		log_text << "action: " << action << std::endl;
		log_text << "explored: " << (explored ? "yes" : "no") << std::endl;
		log_text << "explore rate: " << explore_rate << std::endl;
		log_text << "total reward: " << ai.reward() << std::endl;
		log_text << "average reward: " << ai.averageReward() << std::endl;
		log_text << "mcst new del " << SearchNode::stat_new << " " << SearchNode::stat_del << std::endl;
		log_text << "ct new del " << CTNode::stat_new << " " << CTNode::stat_del << std::endl;

		// Log the data in a more compact form
		compactLog << cycle << ", " << observation << ", " << reward << ", "
				   << action << ", " << explored << ", " << explore_rate << ", "
				   << ai.reward() << ", " << ai.averageReward() << std::endl;

		// Print to standard output when cycle == 2^n
		// if ((cycle & (cycle - 1)) == 0)
		if((cycle & (cycle - 1)) == 0 || cycle % 512 == 0)
		{
			std::cout << "cycle: " << cycle << std::endl;
			std::cout << "average reward: " << ai.averageReward() << std::endl;
			if (explore)
			{
				std::cout << "explore rate: " << explore_rate << std::endl;
			}
			std::cout << "history size: " << ai.historySize() << std::endl;
		}

		// Update exploration rate
		if (explore)
			explore_rate *= explore_decay;
	}

	// Print summary to standard output
	std::cout << std::endl
			  << std::endl
			  << "SUMMARY" << std::endl;
	std::cout << "agent lifetime: " << ai.lifetime() << std::endl;
	std::cout << "average reward: " << ai.averageReward() << std::endl;
}

// Populate the 'options' map based on 'key=value' pairs from an input stream
void processOptions(std::ifstream &in, options_t &options)
{
	std::string line;
	size_t pos;

	for (int lineno = 1; in.good(); lineno++)
	{
		std::getline(in, line);

		// Ignore # comments
		if ((pos = line.find('#')) != std::string::npos)
		{
			line = line.substr(0, pos);
		}

		// Remove whitespace
		while ((pos = line.find(" ")) != std::string::npos)
			line.erase(line.begin() + pos);
		while ((pos = line.find("\t")) != std::string::npos)
			line.erase(line.begin() + pos);

		// Split into key/value pair at the first '='
		pos = line.find('=');
		std::string key = line.substr(0, pos);
		std::string value = line.substr(pos + 1);

		// Check that we have parsed a valid key/value pair. Warn on failure or
		// set the appropriate option on success.
		if (pos == std::string::npos)
		{
			std::cerr << "WARNING: processOptions skipping line " << lineno << " (no '=')" << std::endl;
		}
		else if (key.size() == 0)
		{
			std::cerr << "WARNING: processOptions skipping line " << lineno << " (no key)" << std::endl;
		}
		else if (value.size() == 0)
		{
			std::cerr << "WARNING: processOptions skipping line " << lineno << " (no value)" << std::endl;
		}
		else
		{
			options[key] = value; // Success!
			std::cout << "OPTION: '" << key << "' = '" << value << "'" << std::endl;
		}
	}
}

int main(int argc, char *argv[])
{
	//srand(0);

	if (argc < 2 || argc > 3)
	{
		std::cerr << "ERROR: Incorrect number of arguments" << std::endl;
		std::cerr << "The first argument should indicate the location of the configuration file and the second (optional) argument should indicate the file to log to." << std::endl;
		return -1;
	}

	// Set up logging
	std::string log_file = argc < 3 ? "log" : argv[2];
	log_text.open((log_file + ".log").c_str());
	compactLog.open((log_file + ".csv").c_str());

	// Print header to compactLog
	compactLog << "cycle, observation, reward, action, explored, explore_rate, total reward, average reward" << std::endl;

	// Load configuration options
	options_t options;

	// Default configuration values
	options["ct-depth"] = "4";
	options["agent-horizon"] = "2";
	options["exploration"] = "0";	 // do not explore
	options["explore-decay"] = "1.0"; // exploration rate does not decay

	// Read configuration options
	std::ifstream conf(argv[1]);
	if (!conf.is_open())
	{
		std::cerr << "ERROR: Could not open file '" << argv[1] << "' now exiting" << std::endl;
		return -1;
	}
	processOptions(conf, options);
	conf.close();

	// Set up the environment
	Environment *env;

	// TODO: instantiate the environment based on the "environment-name"
	// option. For any environment you do not implement you may delete the
	// corresponding if statement.
	// NOTE: you may modify the options map in order to set quantities such as
	// the reward-bits for each particular environment. See the coin-flip
	// experiment for an example.
	std::string environment_name = options["environment"];
	const int n_envs = 10;
	const std::string env_names[n_envs] = {//Just use these indices to identify methods!
										   "coin-flip",
										   "1d-maze",
										   "cheese-maze",
										   "tiger",
										   "extended-tiger",
										   "4x4-grid",
										   "tictactoe",
										   "biased-rock-paper-scissor",
										   "kuhn-poker",
										   "pacman"};
	const char *AOR_bits[n_envs][3] = {
		// Actions, Observation bits, Reward bits
		// Note that the actions number represents the actual number of actions,
		// not the number of bits needed to represent the number of actions.
		{"2", "1", "1"},
		{"2", "1", "1"},
		{"4", "4", "5"},
		{"2", "2", "7"},
		{"2", "3", "8"},
		{"2", "1", "1"},
		{"9", "18", "3"},
		{"3", "2", "2"},
		{"1", "4", "3"},
		{"4", "16", "8"}};

	Environment *envs[n_envs] = {
		new CoinFlip(options),
		new OneDMaze(options),
		new CheeseMaze(options),
		NULL,
		NULL,
		NULL,
		new TicTacToe(options),
		new RPS(options),
		NULL,
		new PacMan};

	for (int i = 0; i < n_envs; ++i)
	{
		if (env_names[i] == environment_name)
		{
			if (envs[i])
			{

				options["agent-actions"] = AOR_bits[i][0];
				options["observation-bits"] = AOR_bits[i][1];
				options["reward-bits"] = AOR_bits[i][2];
				env = envs[i];
				break;
			}
			else
			{
				std::cerr << "ERROR: unimplemented environment '"
						  << environment_name << "'" << std::endl;
				return (-1);
			}
		}
	}

	// Set up the agent
	Agent ai(options);

	// Run the main agent/environment interaction loop
	mainLoop(ai, *env, options);
	//delete env //It is main, Let it leak

	log_text.close();
	compactLog.close();

	for (int i = 0; i < n_envs; ++i)
	{
		if (envs[i])
		{
			delete envs[i];
		}
	}

	return 0;
}
