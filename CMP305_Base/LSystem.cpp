#include "LSystem.h"

void LSystem::addRule(char from, const std::string &to) {
	rules[from] = to;
}

void LSystem::run(int count) {
	reset();
	for (int i = 0; i < count; ++i) {
		iterate();
	}
}

void LSystem::iterate() {	
	std::string next;
	next.reserve(currentSystem.size() * 2);

	for (char c : currentSystem) {
		next += rules[c];
	}

	currentSystem = std::move(next);
}

void LSystem::reset() {
	currentSystem = axiom;
}
