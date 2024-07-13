#ifndef __KQUEUE_HPP__
# define __KQUEUE_HPP__

# include <cstring>
# include <sys/event.h>
# include <sys/time.h>

class	Sockets;

class KQueue {
public:
	KQueue();
	~KQueue();
	void	SET_QUEUE(int fd, short filter, bool mode);
	int	CHECK_QUEUE(struct kevent *events);
	int	get_current_events();

	int		kq;
	int		current_events;
private:
	struct	kevent	event;
};

# include "Sockets.hpp"

#endif
