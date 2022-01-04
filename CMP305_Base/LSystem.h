#pragma once

#include <string>
#include "D3D.h"

class LSystem {
public:
	const std::string &getSystem() { return currentSystem; }
	void setAxiom(const std::string &str) { axiom = str; currentSystem = str; }
	const std::string &getAxiom() { return axiom; }

	void addRule(char from, const std::string &to);
	void run(int count);
	void iterate();
	void reset();

private:
	std::string axiom;
	std::string currentSystem;
	std::string rules[256];
};