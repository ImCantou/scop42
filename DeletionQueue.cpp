#include "DeletionQueue.hpp"

void	DeletionQueue::add(std::function<void()>&& function) {
	_deleteFcts.push_back(function);
}

void	DeletionQueue::execute() {
	for (std::deque<std::function<void ()>>::reverse_iterator it = _deleteFcts.rbegin(); it != _deleteFcts.rend(); ++it) {
		(*it)();
	}

	_deleteFcts.clear();
}