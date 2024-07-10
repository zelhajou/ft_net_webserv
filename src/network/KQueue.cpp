#include "KQueue.hpp"

KQueue::KQueue()
	: current_events(0) {
		this->kq = kqueue();
		if (this->kq < 0)	throw	1;
	}

int	KQueue::get_current_events() { return this->get_current_events; }

void	KQueue::SET_QUEUE(int fd, short filter, u_short flags) {
	std::memset(&this->event, 0, sizeof(this->event));
	FD_SET(&this->event, fd, filter, flags|EV_ENABLE, 0, 0, 0);
	kevent(this->kq, &this->event, 1, NULL, 0, NULL);
	if (flags == EV_DELETE)	this->current_events -= 1;
	else			this->current_events += 1;
}

int	KQueue::CHECK_QUEUE(struct kevent *events) {
	if (this->current_events <= 0)	return 0;
	return	kevent(this->kq, NULL, 0, events, this->current_events, NULL);
}
