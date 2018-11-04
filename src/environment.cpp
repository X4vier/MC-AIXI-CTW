#include "environment.hpp"

#include <cassert>

#include "util.hpp"

#include <cmath>
#include <cstdlib>
#include <stdexcept>

int maxi(int a, int b)
{
	return a>b ? a:b;
}

int min(int a, int b)
{
	return a<b ? a:b;
}


CoinFlip::CoinFlip(options_t &options) {
	// Determine the probability of the coin landing on heads
	p = 1.0;
	if (options.count("coin-flip-p") > 0) {
		strExtract(options["coin-flip-p"], p);
	}
	assert(0.0 <= p);
	assert(p <= 1.0);

	// Set up the initial observation
	m_observation = rand01() < p ? 1 : 0;
	m_reward = 0;
}

// Observes 1 (heads) with probability p and 0 (tails) with probability 1 - p.
// Observations are independent of the agent's actions. Gives a reward of 1 if
// the agent correctly predicts the next observation and 0 otherwise.
void CoinFlip::performAction(action_t action) {
	m_observation = rand01() < p ? 1 : 0;
	m_reward = action == m_observation ? 1 : 0;
}

const PacMan::State PacMan::S0; //TODO: move initializers here?

PacMan::PacMan(){
	zero();
}

void PacMan::zero(){
	S = S0;
	//"At the start of each episode, a food
	//pellet is placed down with probability 0.5 at every empty location on the grid.
	for (int i=0;i<R;++i){
		for (int j=0;j<C;++j){
			if(' '==S.m[i][j] && S.noStanding(i,j) && randRange(2)){
				S.m[i][j]='.';
			}
		}
	}
}

const int PacMan::State::di[4]={0,1,0,-1};
const int PacMan::State::dj[4]={1,0,-1,0};
bool PacMan::State::moveChar(int *out,const int ij[],int dir)const {
	return moveChar(out,ij[0],ij[1],dir);
}

bool PacMan::State::moveChar(int *out,int i0,int j0,int dir)const {
	assert(dir<4);
	int i = i0+di[dir];
	int j = j0+dj[dir];
	while(i<0){ i+=R; }
	i=i%R;
	while(j<0){ j+=C; }
	j=j%C;
	//if(m[i][j]!='#' && noStanding(i,j)){//Collision?
	if(m[i][j]!='#'){//Characters can be stacked
		out[0]=i;
		out[1]=j;
		return true;
	}else{
		return false;
	}
}

bool PacMan::State::noStanding(int i,int j)const{
	if (p[0]==i && p[1]==j) return false;
	for (int c=0;c<G;++c){
		if (g[c][0]==i && g[c][1]==j) return false;
	}
	return true;
}


bool PacMan::State::peq(const int a[], const int b[])
	{ return a[0]==b[0] && a[1]==b[1]; }

char PacMan::State::chAt(const int a[]) const
	{ return m[a[0]][a[1]]; }

char &PacMan::State::chAt(const int a[])
	{ return m[a[0]][a[1]]; }

int PacMan::State::manDist(const int a[], const int b[])
	{ return(abs(a[0]-b[0])+abs(a[1]-b[1])); }

int PacMan::State::step(const State &S,action_t act){
	assert(act<4); //must be moving;
	int r=0;
	/*if(action<4)*/{
		printf("move\n");
		S.moveChar(p,S.p,act); //don't pass member of S as output
		--r; //"The agent receives a penalty of 1 for each movement action,"
	}
	if(/*action<4 && */peq(S.p,p)) { //ran into a wall
		printf("wall\n");
		r -= 10; //a penalty of 10 for running into a wall,"
		assert(' '==S.chAt(p));
	}

	{
		if('.'==chAt(p)){
			printf("eat\n");
			r+=10; //" a reward of 10 for each food pellet eaten, "
			assert(act<4 && !peq(S.p,p));
		}else if('o'==chAt(p)){
			printf("dope\n");
			r+=10; //" The reward for eating power pill is same as eating a standard food pellet."
			p[2]=100; //" The effect of eating a power pill lasts for 100 time steps."
			//Assume that eating a second pill overrides the first pill
		}
		chAt(p) = ' ';
	}

	for(int i=0;i<4;++i){ //" Four ghosts roam the maze."
		//Do ghosts sense pacman at last step or this step?
		//if(manDist(S.p,S.g[i])<=5){
		if(manDist(p,S.g[i])<=5){
			//"until there is a Manhattan distance of 5 between
			//them and PacMan, whereupon they will aggressively
			//pursue PacMan for a short duration. "
			//According to Sultan ghosts only pursue PacMan for
			//the current frame. And ghosts cannot stop.
			//"If PacMan is under a power pill effect, the ghosts
			//run away from Pacman once their Manhattan distance
			//is less than or equal to 5 from Pacman."
			int s1 = -1;
			int d1 = -1;
			for(int d=0;d<4;++d){
				if(S.moveChar(g[i],S.g[i],d)){
					int s = manDist(S.p,g[i]);
					if(d1<0 || (p[2]==0 && s<s1) || (p[2]>0 && s>s1)){
						s1 = s;
						d1 = d;
					}
				}
			}
			assert(d1>=0);
			S.moveChar(g[i],S.g[i],d1);
		}else{
			//"They move initially at random,"
			int d0 = randRange(4);
			for(int d=0;d<4;++d){
				if(S.moveChar(g[i],S.g[i],(d0+d)%4)) break;
			}
		}
	}


	return r;
}

void PacMan::performAction(action_t action){
	State T=S; // "current" state
	m_reward = 128 + T.step(S,action);
	//precise offset should be 211
	//use 1000 for better interpretability

	//"however the PacMan agent is hampered by partial observability"
	m_observation = 0;
	int mask = 1;

	//"PacMan is unaware of the maze structure and only receives
	//a 4-bit observation describing the wall conﬁguration at its current location. "
	for(int d=0;d<4;++d){
		int x[2]={0,0};
		if(!T.moveChar(x,T.p[0],T.p[1],d)){
			m_observation |= mask;
		}
		mask <<= 1;
	}


	for(int d=0;d<4;++d){
		int x[2]={T.p[0],T.p[1]};
		while(T.moveChar(x,x[0],x[1],d)){
			//"It also does not know the exact location of the ghosts,
			//receiving only 4-bit observations indicating whether a ghost
			//is visible (via direct line of sight) in each of the four
			//cardinal directions."
			for(int i=0;i<4;++i){
				if(State::peq(x,T.g[i])){
					m_observation |= mask;
					break;
				}
			}
			if('.' == T.chAt(x)){
				// "and another 4-bit observation indicating
				// whether there is food in its direct line of sight."
				m_observation |= mask<<4;
			}
		}
		mask <<= 1;
	}
	mask <<= 4;


	//"In addition, the location of the food pellets is unknown except
	//for a 3-bit observation that indicates whether food can be smelt within a Manhattan
	//distance of 2, 3 or 4 from PacMans location,"
	{
		const int R = 4;
		int d[R*2+1][R*2+1];
		int realP[R*2+1][R*2+1][2];
		for(int i=-R;i<=R;++i){
			for(int j=-R;j<=R;++j){
				d[R+i][R+j] = 9999;
				realP[R+i][R+j][0] = -1;
				realP[R+i][R+j][1] = -1;
			}
		}
		d[R][R] = 0;
		realP[R][R][0] = T.p[0];
		realP[R][R][1] = T.p[1];
		bool more = true;
		while(more){
			more = false;
			for(int i=-R;i<=R;++i){
				for(int j=-R;j<=R;++j){
					int d0 = d[R+i][R+j];
					if(d0<9999){
						//int x0[2]={T.p[0]+i,T.p[1]+j};
						int x0[2]={0,0};
						for(int k=0;k<2;++k)
							x0[k] = realP[R+i][R+j][k];
						assert(T.chAt(x0)!='#');
						if('.' == T.chAt(x0)||'o' == T.chAt(x0)){
							//" The power pills smell same as the food pellets."
							if(d0<=2) m_observation |= mask;
							if(d0<=3) m_observation |= mask<<1;
							if(d0<=4) m_observation |= mask<<2;
						}
						for(int k=0;k<4;++k){
							int i1 = i+State::di[k];
							int j1 = j+State::dj[k];
							int x[2]={0,0};
							if(i1<=R && i1>=-R && j1<=R && j1>=-R && T.moveChar(x,x0,k)){
								int &d1 = d[R+i1][R+j1];
								if(d1 > d0+1){
									d1 = d0+1;
									realP[R+i1][R+j1][0] = x[0];
									realP[R+i1][R+j1][1] = x[1];
									more = true;
								}
							}
						}
					}
				}
			}
		}
		mask <<= 3;
	}


	//"A ﬁnal single bit indicates whether
	//PacMan is under the eﬀects of a power pill."
	if(T.p[2]>0) m_observation |= mask;
	mask <<= 1;


	assert(1<<16 == mask);



	//" a penalty of 50 if it is caught by a ghost,"
	//" and a reward of 100 for collecting all the food."
	//"The episode resets if the agent is caught or if it collects all the food."
	{
		bool caught = false;
		for(int i=0;i<4;++i){
			if(State::peq(T.p,T.g[i]) || (State::peq(T.p,S.g[i]) && State::peq(S.p,T.g[i]))){
				//If pacman just swapped position with a ghost it should be regarded as
				//being caught. Otherwise the game would seem against common sense.
				if(T.p[2]>0){ //Assume that PacMan can eat multiple ghosts in one frame
					m_reward += 30; // "Eating a ghost while on a power pill effect gives a reward of 30 "
					// "and resets the ghost’s location."
					T.g[i][0] = S0.g[i][0];
					T.g[i][1] = S0.g[i][1];
					printf("exorcised\n");
				}else if(!caught){ //Assume that PacMan can only be caught by one ghost in one frame. There are only 8 bits so negative reward can't exceed -128
					m_reward -= 50;
					caught = true;
					printf("caught\n");
				}
			}
		}
		if(T.p[2]>0) --T.p[2];
		bool cleared = true;
		for(int i=0;i<R;++i){
			for(int j=0;j<C;++j){
				if('.'==T.m[i][j]) cleared = false;
			}
		}
		if(cleared)
			m_reward += 100;
		if(caught || cleared)
			zero();
		else
			S=T;
	}

	{
		State D = S;
		D.show_dirty(m_reward - 128);
	}

	assert(m_reward>=0);
	assert(m_reward<256);

	//l-o-s
	//encoding
	//Collision tempo? (swap positions and collide on the new step) //they can occupy same pos`
	//power pill, eating ghosts, reward? new spec
	//short duration? path-finding? just prioritize one direction in current step;manhatten dist through walls
	//must move? ghost(try other directions)? pc? Must be moving.
}

void PacMan::State::show_dirty(int reward){
	static int c=0;
	chAt(p) = 'G';
	for(int i=0;i<4;++i){
		chAt(g[i]) = '&';
	}
	for(int i=0;i<R;++i){
		std::cout<<m[i]<<std::endl;
	}
	std::cout<<"========== "<<++c<<" "<<reward<<std::endl;
	std::cout<<std::endl;
}
//bool PacMan::isFinished(){ //int α=1; return false; }

RPS::RPS(options_t &options) { //don't keep the options object
	m_reward = 0; // no reward to begin with
	m_observation = 1; // anything but Rock so that the first move is random
}

void RPS::performAction(action_t action) {
	// Check if this 'opponent' played rock last time and won
	// Reward=0 means the agent lost and hence this opponent won
	if (m_observation == 0 && m_reward == 0) {
		m_observation = 0; // Play rock
	} else {
		m_observation = randRange(3); // Play something randomly
	}

	if (action == m_observation) {
		m_reward = 1; // Draw
	} else if (action == (m_observation + 1) % 3) { // Agent wins
		m_reward = 2;
	} else if ((action + 1) % 3 == m_observation) { // Opponent loses
		m_reward = 0;
	} else {
		throw std::runtime_error("Rock Paper Scissors implementation wrong");
	}
}

TicTacToe::TicTacToe(options_t &options) { // don't keep the options object
    m_reward = 0;
    updateObservation();
}

void TicTacToe::performAction(action_t action) {
    int row = action / 3;
    int col = action % 3; // Get the row and column numbers

    if (board[row][col] != 0) { // Check if this is an invalid move
        m_reward = 0;
        return updateObservation();
    }

    board[row][col] = 1; // Play the agent's move

    int winner = checkWinStatus();
    if (winner == 1) { // Agent wins
        m_reward = 4;
        return resetBoard();
    } else if (winner == 3) { // Tie
        m_reward = 3;
        return resetBoard();
    }

    assert(!(board[0][0]&&board[0][1]&&board[0][2]&&
		board[1][0]&&board[1][1]&&board[1][2]&&
		board[2][0]&&board[2][1]&&board[2][2]));

    // Pick a random square for the environment to play
    int e_r = 0;
    int e_c = 0;
    while (board[e_r][e_c] != 0) {
        e_r = randRange(3);
        e_c = randRange(3);
    }
    board[e_r][e_c] = 2; // Play the environment's move

    winner = checkWinStatus();
    if (winner == 2) { // Environment wins
        m_reward = 1;
        return resetBoard();
    } else if (winner == 3) { // Tie
        m_reward = 3;
        return resetBoard();
    }

    // The game continues to the next round
    m_reward = 2;
    updateObservation();
}

/**
 *  Calculate the observation data for the current state of the game.
 *  Produce a sequence of 9 pairs of bits. Each represents a square on the grid,
 *  presented in the order top-left to bottom-right, across first.
 *  Each pair is a number from 0 - 2, where 0 is empty, 1 is the agent's marker, 2 is the environment's marker.
 *  Set the observation value to be the integer representing the binary string of the above.
*/
void TicTacToe::updateObservation() {
    m_observation = 0; // Reset observation
    for (int i = 0; i < 3 ; i++) {
        for (int j = 0; j < 3; j++) {
            percept_t cell = board[i][j];
            m_observation = m_observation * 4 + cell;
        }
    }
}
/**
 * Determine whether any player has won the game.
 * When checking for three in a row, return the value of the cell on the board.
 * @return 0 if the game is incomplete, 1 if the agent wins, 2 if the environment wins, and 3 if it's a tie.
 */
int TicTacToe::checkWinStatus() {
    for (int i = 0; i < 3; i++) { // Check row strikes
        if (board[i][0] &&
            board[i][0] == board[i][1] &&
            board[i][1] == board[i][2])
            return board[i][0];
    }
    for (int i = 0; i < 3; i++) { // Check column strikes
        if (board[0][i] &&
            board[0][i] == board[1][i] &&
            board[1][i] == board[2][i])
            return board[0][i];
    }
    // Check diagonal strikes
    if (board[0][0] &&
        board[0][0] == board[1][1] &&
        board[1][1] == board[2][2])
        return board[0][0];
    if (board[0][2] &&
        board[0][2] == board[1][1] &&
        board[1][1] == board[2][0])
        return board[0][2];
    // Check if its a tie
    bool stillGoing = false;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (!(board[i][j])) stillGoing = true;
        }
    }
    if (!stillGoing) return 3; // Tie value
    // Nobody wins and the game continues
    return 0;
}

/**
 * Reset the board but not the reward value.
 */
void TicTacToe::resetBoard() {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            board[i][j] = 0;
        }
    }
    updateObservation();
}


OneDMaze::OneDMaze(options_t &options) {
	m_observation = 0; // Constant observation
    if (options.count("goal-cell") > 0) {  // Set the goal position if specified
         strExtract(options["goal-cell"], goal);
         goal = goal - 1;
    }
    if (options.count("size") > 0) {       // Set the maximum size if specified
        int size;
        strExtract(options["size"], size);
        if (goal > size - 1) throw std::runtime_error("Goal cell is outside 1d maze size");
        max_location = size - 1;
    }
	location = randRange(4); // Randomly select the starting point
	m_reward = (location == goal) ? 1 : 0; // Reward if the agent is at the goal already.
}

void OneDMaze::performAction(action_t action) {
    int new_location = location + (2 * action - 1);
    location = min(maxi(new_location, 0), max_location); // Keep within the 1x4 bounds
    m_reward = (location == goal) ? 1 : 0; // Reward if the agent is at the goal.
}

CheeseMaze::CheeseMaze(options_t &options) {
    m_reward = 0;
    do { // Pick a random starting point for the mouse
        loc_r = randRange(rows);
        loc_c = randRange(cols);
    } while (maze[loc_r][loc_c] == '#');
    updateObservation();
}

void CheeseMaze::performAction(action_t action) {
    // Actions: 0 = up, 1 = right, 2 = down, 3 = left
    // Find the new coordinate.
    int new_r = loc_r;
    int new_c = loc_c;
    if (action % 2 == 0) new_c += (int)action - 1;
    else                 new_r += (int)action - 2;

    // Check if the new spot is a wall.
    if (maze[new_r][new_c] == '#') {
        m_reward = 0;
        return;
    }

    // Move the mouse.
    loc_r = new_r;
    loc_c = new_c;

    // Check if the new spot is on the cheese.
    if (loc_r == cheese_r && loc_c == cheese_c) {
        m_reward = 20;
    }
    else m_reward = 9;

    updateObservation();
}

/**
    Find the observation value of the current cell from the maze.
    Decode it and update the observation field.
*/
void CheeseMaze::updateObservation() {
    std::stringstream ss;
    ss << std::hex << maze[loc_r][loc_c];
    ss >> m_observation;
}
