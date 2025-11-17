#include "../../../includes/config/block/EventBlock.hpp"

EventBlock::EventBlock(void)
{
}

EventBlock::~EventBlock(void)
{
}

EventBlock::EventBlock(const EventBlock &ref)
{
	*this = ref;
}

EventBlock &EventBlock::operator=(const EventBlock &ref)
{
	if (this != &ref)
	{
		this->workerConnections = ref.workerConnections;
	}
	return *this;
}

void EventBlock::setWorkerConnections(int num)
{
	this->workerConnections = num;
}

int EventBlock::getWorkerConnections() const
{
	return this->workerConnections;
}
