#ifndef POLLER_H
#define POLLER_H

#include <map>
#include <vector>
#include <memory>

class Sapper;
class Poller;

typedef std::shared_ptr<Sapper> SapperPtr;
typedef std::vector<SapperPtr> SapperList;
typedef std::shared_ptr<Poller> PollerPtr;

class Poller
{
public:
	Poller() {}
	virtual void poll(int timeout, SapperList& activeSapper) = 0;
	virtual void updateSapper(SapperPtr){}
	virtual void removeSapper(SapperPtr){}
	virtual ~Poller(){}
};


#endif
