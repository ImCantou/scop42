#pragma once 

#include <functional>
#include <deque>

class	DeletionQueue
{
	private:
		std::deque<std::function<void()> >	_deleteFcts;

	public:
		void	add(std::function<void()>&& function);
		void	execute();
};
