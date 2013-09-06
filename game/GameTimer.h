#pragma once
#ifndef _GAMETIMER_H_
#define _GAMETIMER_H_

namespace LAE {

class GameTimer {
public:
	static GameTimer* i();

	unsigned long getTime() const;
	void setDiscrete(bool discrete);
	bool isDiscrete() const;
	void updateTime();
	void updateTime( unsigned long diff );
	unsigned long getSystemTime() const;

private:
	GameTimer();
	GameTimer(const GameTimer&);
	GameTimer& operator=(const GameTimer&);
	~GameTimer();

	bool isDiscrete_;
	unsigned long currentTime_;
};

}

#endif // _GAMETIMER_H_
