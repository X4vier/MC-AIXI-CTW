#include "environment1.hpp"

#include <cassert>

#include "util.hpp"

OtherEnv1::OtherEnv1(const std::string &t, options_t &options):env(t),opt(options){
	//need to modify options here as in CoinFlip
	//options is both input and output
}
void OtherEnv1::performAction(action_t action){}
bool OtherEnv1::isFinished(){
	//int α=1;
	return false;
}
