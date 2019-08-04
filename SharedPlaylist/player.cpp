
#include "src/adt/repeat_mode.h"
#include "src/adt/player.h"


Player::Player() :
	m_queue(std::make_unique<SongQueue>()), m_repMode(RepeatMode::off), m_shuffled(false)
{
	m_queue->setShuffled(m_shuffled);
}


Player::~Player()
{
	
}


SongQueue const* Player::getQueue() const
{
	return m_queue.get();
}


const RepeatMode& Player::getRepeatMode() const
{
	return m_repMode;
}


void Player::setRepeatMode(const RepeatMode& repeatMode)
{
	m_repMode = repeatMode;
}


void Player::setShuffle(const bool& shuffle)
{
	m_shuffled = shuffle;
	m_queue->setShuffled(shuffle);
}