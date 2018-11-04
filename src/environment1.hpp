#ifndef __ENVIRONMENT1_HPP__
#define __ENVIRONMENT1_HPP__
#include "environment.hpp"

class OtherEnv1: public Environment {
public:
	OtherEnv1(const std::string &type, options_t &options);
	virtual void performAction(action_t action);
	virtual bool isFinished();
protected:
	std::string env;
	options_t opt;
};

#endif // __ENVIRONMENT_HPP__
