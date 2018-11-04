#ifndef __ENVIRONMENT_HPP__
#define __ENVIRONMENT_HPP__

#include "main.hpp"
#include <cstdlib>
#include <memory.h>

class Environment {

public:

	// Constructor: set up the initial environment percept
	// TODO: implement in inherited class

	// receives the agent's action and calculates the new environment percept
	virtual void performAction(action_t action) = 0; // TODO: implement in inherited class

	// returns true if the environment cannot interact with the agent anymore
	virtual bool isFinished(void) const { return false; } // TODO: implement in inherited class (if necessary)

	void getPercept(symbol_list_t &symbol_list);

	percept_t getObservation(void) const { return m_observation; }

	percept_t getReward(void) const { return m_reward; }
	virtual ~Environment(){}

	virtual Environment *clone()const = 0;

protected: // visible to inherited classes
	action_t m_last_action;  // the last action performed by the agent
	percept_t m_observation; // the current observation
	percept_t m_reward;      // the current reward

};


// An experiment involving flipping a biased coin and having the agent predict
// whether it will come up heads or tails. The agent receives a reward of 1
// for a correct guess and a reward of 0 for an incorrect guess.
class CoinFlip : public Environment {
public:

	// set up the initial environment percept
	CoinFlip(options_t &options);

	// receives the agent's action and calculates the new environment percept
	virtual void performAction(action_t action);

	CoinFlip(const CoinFlip &prot){
		memcpy(this,&prot,sizeof(CoinFlip));
	}

	virtual Environment * clone() const{ 
		return new CoinFlip(*this);
	}

private:
	double p; // Probability of observing 1 (heads)
};

class PacMan: public Environment {
public:
	PacMan();
	virtual void performAction(action_t action);
	//virtual bool isFinished();
	PacMan(const PacMan &prot){
		memcpy(this,&prot,sizeof(PacMan));
	}

	virtual Environment * clone() const{ 
		return new PacMan(*this);
	}


protected:
	static const int R = 21;
	static const int C = 19;
	static const int G = 4;
	struct State{
		char m[R][C+1]={ //+1 for '\0'
			"###################",
			"#                 #",
			"# ## ### # ### ## #",
			"#o               o#",
			"# ## # ##### # ## #",
			"#    #   #   #    #",
			"#### ### # ### ####",
			"#### #       # ####",
			"#### # #   # # ####",
			"     # #   # #     ",
			"#### # ##### # ####",
			"#### #       # ####",
			"#### # ##### # ####",
			"#                 #",
			"# ## ### # ### ## #",
			"#o #           # o#",
			"## # # ##### # # ##",
			"#    #   #   #    #",
			"# ###### # ###### #",
			"#                 #",
			"###################"
		}; //map state '#'-wall ' '-road '.'-road-with-food
		//int g[G][3]={
		//	{8,9,0},{8,10,0},{9,9,0},{9,10,0}
		//}; //ghosts position and pursuing counter
		int g[G][2]={
			{8,9},{8,10},{9,9},{9,10}
		}; //ghosts positions
		int p[3]={ 13,9,0 }; //pacman position and pill counter
		bool noStanding(int i,int j)const;
		static const int di[4];
		static const int dj[4];
		bool moveChar(int *out,int i,int j,int dir)const;
		bool moveChar(int *out,const int ij[],int dir)const;
		int step(const State &prev,action_t action);
		static bool peq(const int a[],const int b[]);
		char chAt(const int a[]) const;
		char &chAt(const int a[]);
		static int manDist(const int a[], const int b[]);
		void show_dirty(int reward);
		State(){}
	}S;
	static const State S0;
	void zero();
};

/**
 * From the assignment specification:
 * The agent repeatedly plays Rock-Paper-Scissor against an opponent that has a slight, predictable
 * bias in its strategy. If the opponent has won a round by playing rock on the
 * previous cycle, it will always play rock at the next time step; otherwise it will pick an
 * action uniformly at random. The agents observation is the most recently chosen action
 * of the opponent. It receives a reward of 1 for a win, 0 for a draw and -1 for a loss.
 *
 * @Actions 0 (rock), 1 (paper), 2 (scissors)
 * @Observations 0 (rock), 1 (paper), 2 (scissors)
 * @Rewards 2 (win), 1 (draw), 0 (loss) -- seeing as the assignment framework does not support negative rewards.
 */
class RPS : public Environment {
public:
	RPS(options_t &options);

	virtual void performAction(action_t action);

	// Set the maximum values for these so that the isValid checks work.
	virtual action_t maxAction() const { return 2; }
	virtual percept_t maxObservation() const { return 2; }
	virtual percept_t maxReward() const { return 2; }
	// Minimum values are already 0.
	
	RPS(const RPS &prot){
		memcpy(this,&prot,sizeof(RPS));
	}

	virtual Environment * clone() const{ 
		return new RPS(*this);
	}

};

/**
 * From the assignment specification:
 * In this domain, the agent plays repeated games of TicTacToe against
 * an opponent who moves randomly. If the agent wins the game, it receives a reward
 * of 2. If there is a draw, the agent receives a reward of 1. A loss penalizes the agent
 * by -2. If the agent makes an illegal move, by moving on top of an already filled
 * square, then it receives a reward of -3. A legal move that does not end the game
 * earns no reward.
 *
 * @Actions 0 - 8, squares of the TicTacToe grid going top-left to bottom-right, across first.
 * @Observations Sequence of 9 pairs of bits. Each represents a square on the grid in the same
 *               order as actions. Each is a number from 0 - 2.
 *               0 is empty, 1 is the agent's marker, 2 is the environment's marker.
 * @Rewards 0 (Invalid move), 1 (Loss), 2 (Game continues), 3 (Draw), 4 (Win).
 *
 * After the game ends, the reward will be kept while the board (and observation) is reset.
 */
class TicTacToe : public Environment {
public:
	TicTacToe(options_t &options);

	virtual void performAction(action_t	action);

	TicTacToe(const TicTacToe &prot){
		memcpy(this,&prot,sizeof(TicTacToe));
	}

	virtual Environment * clone() const{ 
		return new TicTacToe(*this);
	}



private:
	void updateObservation();

	int checkWinStatus();

	void resetBoard();

	percept_t board[3][3]; // TicTacToe grid. All 0 (empty) to begin with.
};

/**
 * From the assignment specification:
 * The 1d-maze is a simple problem from [CKL94]. The agent begins at a
 * random, non-goal location within a 1×4 maze. There is a choice of two actions: left
 * or right. Each action transfers the agent to the adjacent cell if it exists, otherwise
 * it has no effect. If the agent reaches the third cell from the left, it receives a reward
 * of 1. Otherwise it receives a reward of 0. The distinguishing feature of this problem
 * is that the observations are uninformative; every observation is the same regardless
 * of the agents actual location.
 *
 * @Actions 0 - 1, 0 indicates a move left, 1 indicates a move right.
 * @Observations None. The agent will always observe 0, even when in the goal space.
 * @Rewards 0 (not on a goal space), 1 (on a goal space).
 *
 * @param options   1d-maze is configurable in is length and position of the goal space.
 *                  size      The length of the maze.
 *                  goal-cell The position of the goal space, between 1 and the size.
 */
class OneDMaze : public Environment {
public:
	OneDMaze(options_t &options);

	virtual void performAction(action_t	action);

	OneDMaze(const OneDMaze &prot){
		memcpy(this,&prot,sizeof(OneDMaze));
	}

	virtual Environment * clone() const{
		return new OneDMaze(*this);
	}

private:
	int max_location = 3; // Default size of 4  (4-1=3)
	int location; // Store the current location
	int goal = 2; // Default goal position of 2 (3rd from the left)
};

/**
 * From the assignment specification:
 * This well known problem is due to [McC96]. The agent is a mouse
 * inside a two dimensional maze seeking a piece of cheese. The agent has to choose
 * one of four actions: move up, down, left or right. If the agent bumps into a wall,
 * it receives a penalty of -10. If the agent finds the cheese, it receives a reward of
 * 10. Each movement into a free cell gives a penalty of -1. The problem is depicted
 * graphically in Figure 1. The number in each cell represents the decimal equivalent
 * of the four-bit binary observation the mouse receives in each cell. The problem
 * exhibits perceptual aliasing in that a single observation is potentially ambiguous.
 *
 * @Actions 0 - 3,  move in one of four directions: 0 = up, 1 = right, 2 = down, 3 = left.
 * @Observations 5, 7, 8, 9, 10, 12: According to the observation value specified for each cell in the maze.
 * @Rewards 0 (invalid move), 9 (valid move), 20 (valid move and agent finds the cheese).
 */
class CheeseMaze : public Environment {
public:
    CheeseMaze(options_t &options);

    virtual void performAction(action_t	action);

    CheeseMaze(const CheeseMaze &prot){
        memcpy(this,&prot,sizeof(CheeseMaze));
    }

    virtual Environment * clone() const{
        return new CheeseMaze(*this);
    }

private:
    void updateObservation();

    int rows = 5;
    int cols = 7;
    int loc_c; // Mouse's column coordinate
    int loc_r; // Mouse's row coordinate

    /**
     * '#' denotes a wall.
     * Anything else is an observation value, encoded in hexadecimal.
     */
    char maze[5][7+1] = { //
            "#######",    //
            "#9A8AC#",
            "#5#5#5#",
            "#7#7#7#",
            "#######"
    };
    const int cheese_r = 3;
    const int cheese_c = 3;
};


#endif // __ENVIRONMENT_HPP__
