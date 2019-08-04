#ifndef PLAYER_H
#define PLAYER_H

#include <memory>

#include "repeat_mode.h"
#include "song_queue.h"


class Player
{
public:
	Player();
	~Player();

	// GETTERS
	SongQueue const* getQueue() const;
	const RepeatMode& getRepeatMode() const;
	const bool& getShuffle() const;

	// SETTERS
	void setRepeatMode(const RepeatMode& repeatMode);
	void setShuffle(const bool& shuffle);


private:
	std::unique_ptr<SongQueue> m_queue;

	RepeatMode m_repMode;
	bool m_shuffled;
};


#endif
