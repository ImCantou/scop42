#ifndef __DELETIONQUEUE_CLASS__
#define __DELETIONQUEUE_CLASS__

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


#endif