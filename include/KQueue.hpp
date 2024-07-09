#ifndef __KQUEUE_HPP__
# define __KQUEUE_HPP__

# include <sys/event.h>
# include <sys/time.h>

class KQueue {
public:
	KQueue();
	~KQueue();
	void	SET_QUEUE(int fd, short filter, u_short flags);
	void	CHECK_QUEUE(struct kevent *events, int size);

private:
	int		_kq;
};


#endif