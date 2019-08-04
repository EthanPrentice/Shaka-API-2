#include <memory>
#include <cassert>
#include <stdexcept>
#include <iostream>

#include "song_queue.h"
#include "repeat_mode.h"
#include <algorithm>


SongQueue::SongQueue() : 
	m_head(new QueueNode()), m_tail(m_head), m_subQueueTail(nullptr), m_currSong(nullptr), 
	m_size(0), m_shuffled(false) { }


SongQueue::~SongQueue()
{
	clear();
	delete m_head;
}


/*!
 *  @brief       Adds a song to the end of the Queue (same way Spotify handles it)
 *  @param[in]   song   The song to be added to the queue
 */
void SongQueue::addToQueue(std::unique_ptr<Song> song)
{
	if (isEmpty()) {
		m_tail = new QueueNode();

		QueueNode* newNode = new QueueNode(std::move(song));
		newNode->setPrev(m_head);
		newNode->setNext(m_tail);
		m_head->setNext(newNode);

		m_tail->setPrev(m_head->getNext(isShuffled()));
		m_subQueueTail = m_head->getNext(isShuffled());
	}
	else {
		QueueNode* newNode = new QueueNode(std::move(song));
		QueueNode* tmp;

		// Add to end of queue of both shuffled and unshuffled.

		bool shuffled = false;
		for (int i = 0; i < 2; ++i) {
			tmp = m_tail->getPrev(shuffled);
			tmp->setNext(shuffled, newNode);
			newNode->setPrev(shuffled, tmp);
			newNode->setNext(shuffled, m_tail);
			m_tail->setPrev(shuffled, newNode);
			
			shuffled = !shuffled;
		}
	}

	m_size += 1;
}


void SongQueue::addToSubQueue(std::unique_ptr<Song> song) 
{
	if (isEmpty()) {
		m_tail = new QueueNode();

		QueueNode* newNode = new QueueNode(std::move(song));
		newNode->setPrev(m_head);
		newNode->setNext(m_tail);
		m_head->setNext(newNode);

		m_tail->setPrev(m_head->getNext(isShuffled()));
		m_subQueueTail = m_head->getNext(isShuffled());
	}
	else {
		QueueNode* newNode = new QueueNode(std::move(song));
		QueueNode* tmp = m_subQueueTail->getNext(isShuffled());

		// update nodes pointing to newNode
		m_subQueueTail->setNext(newNode);
		if (tmp != nullptr) {
			tmp->setPrev(newNode);
		}

		// update newNode pointers
		newNode->setNext(tmp);
		newNode->setPrev(m_subQueueTail);
		m_subQueueTail = m_subQueueTail->getNext(isShuffled());
	}

	m_size += 1;
}


/*!
 *  @brief       Removes a song at an index
 *  @param[in]   songIndex  Index of song to delete
 *
 */
void SongQueue::removeFromQueue(const int& songIndex)
{
	if (songIndex < 0 || songIndex >= m_size) {
		throw std::invalid_argument("songIndex out of bounds");
	}

	if (m_size == 1) {
		delete m_head->getNext(isShuffled());
		delete m_tail;

		m_currSong = nullptr;
		m_subQueueTail = nullptr;

		m_tail = m_head;

		m_size = 0;
		return;
	}

	int i = 0;
	QueueNode* currNode = m_head->getNext(isShuffled());
	while (i++ != songIndex) {
		currNode = currNode->getNext(isShuffled());
		if (currNode == nullptr) {
			throw std::invalid_argument("Internal logic error, null node found before expected.");
		}
	}

	bool toggle = false;

	for (int i = 0; i < 2; ++i, toggle = !toggle) {

		currNode->getPrev(isShuffled())->setNext(isShuffled(), currNode->getNext(isShuffled()));
		currNode->getNext(isShuffled())->setPrev(isShuffled(), currNode->getPrev(isShuffled()));

		if (currNode == m_subQueueTail) {
			m_subQueueTail = currNode->getPrev(isShuffled());
		}
		if (currNode == m_currSong) {
			if (m_currSong->getPrev(isShuffled()) == m_head) {
				m_currSong = nullptr;
			}
			else {
				m_currSong = m_currSong->getPrev(isShuffled());
			}
		}

	}

	delete currNode;

	m_size -= 1;
}


/*!
 *  @brief      Adds a song to the end of the Queue (same way Spotify handles it)
 *
 *  @param[in]  songIndex     The index of where the song currently is
 *  @param[in]  newSongIndex  The index to move the song to
 *
 */
void SongQueue::moveSong(const int& songIndex, const int& newSongIndex)
{
	
	if (songIndex == newSongIndex) {
		return;
	}
	if (songIndex < 0 || songIndex >= m_size) {
		throw std::invalid_argument("oldSongIndex out of bounds");
	}
	if (newSongIndex < 0 || newSongIndex >= m_size) {
		throw std::invalid_argument("newSongIndex out of bounds");
	}

	int i = 0;
	int maxIndex = songIndex < newSongIndex ? newSongIndex : songIndex;

	QueueNode* songNode = nullptr;
	QueueNode* newSongNode = nullptr;

	QueueNode* currNode = m_head->getNext(isShuffled());
	while (i <= maxIndex) {
		if (currNode == nullptr) {
			throw std::invalid_argument("Internal logic error, null node found before expected.");
		}
		if (i == songIndex) {
			songNode = currNode;
		}
		else if (i == newSongIndex) {
			newSongNode = currNode;
		}

		currNode = currNode->getNext(isShuffled());
		i += 1;
	}

	assert(songNode != nullptr);
	assert(newSongNode != nullptr);

	if (songNode == m_currSong) {
		throw std::invalid_argument("Cannot move the currently playing song.");
	}

	if (songNode == m_subQueueTail) {
		m_subQueueTail = songNode->getPrev(isShuffled());
	}

	songNode->getPrev(isShuffled())->setNext(isShuffled(), songNode->getNext(isShuffled()));
	songNode->getNext(isShuffled())->setPrev(isShuffled(), songNode->getPrev(isShuffled()));

	// move forward
	if (songIndex < newSongIndex) {
		songNode->setNext(isShuffled(), newSongNode->getNext(isShuffled()));
		songNode->setPrev(isShuffled(), newSongNode);

		newSongNode->getNext(isShuffled())->setPrev(isShuffled(), songNode);
		newSongNode->setNext(isShuffled(), songNode);
	}
	// move backward
	else {
		songNode->setNext(isShuffled(), newSongNode);
		songNode->setPrev(isShuffled(), newSongNode->getPrev(isShuffled()));

		newSongNode->getPrev(isShuffled())->setNext(isShuffled(),songNode);
		newSongNode->setPrev(isShuffled(), songNode);
	}

}


/*!
 *  @brief   Moves currSong to next song if available
 *
 *  @param[in]   repeatMode  Enum for what RepeatMode is set to within the Player
 *
 *  @return  True if moved back successfully. Only returns false when currSong is the first in queue
 */
bool SongQueue::nextSong(const RepeatMode repeatMode)
{
	if (m_currSong == nullptr) {
		return false;
	}
	if (m_currSong->getNext(isShuffled()) != m_tail) {
		m_currSong = m_currSong->getNext(isShuffled());
		return true;
	}
	return false;
}


/*!
 *  @brief   Moves currSong to previous if available
 *  @return  True if moved back successfully. Only returns false when currSong is the first in queue
 */
bool SongQueue::prevSong()
{
	if (m_currSong == nullptr) {
		return false;
	}
	if (m_currSong->getPrev(isShuffled()) != nullptr) {
		m_currSong = m_currSong->getPrev(isShuffled());
		return true;
	}
	return false;
}


/*!
 *  @brief   Removes all items from the queue
 */
void SongQueue::clear() 
{
	while (!isEmpty()) {
		removeFromQueue(0);
	}
}

/*!
 *  @brief   Should only be used when only one item needs to be accessed.
 *  @return  Returns pointer to Song at songIndex, or nullptr if songIndex is out of range
 */
Song const * SongQueue::getSongAt(int songIndex) const
{
	if (songIndex < 0 || songIndex >= m_size) {
		return nullptr;
	}

	auto song = begin();
	for (int i = 0; song != end(); ++i, ++song) {
		if (i == songIndex) {
			return (*song);
		}
	}

	return nullptr;
}


/*!
 *  @brief   Checks if queue contains no songs
 *  @return  True if queue contains no songs, false otherwise
 */
bool SongQueue::isEmpty() const 
{
	return m_head == m_tail;
}

bool SongQueue::isShuffled() const
{
	return m_shuffled;
}

int SongQueue::size() const
{
	return m_size;
}


void SongQueue::setCurrSong(const int& songIndex)
{
	if (songIndex < 0 || songIndex >= m_size) {
		throw std::invalid_argument("songIndex is out of bounds");
	}

	QueueNode* currNode = m_head->getNext(isShuffled());
	for (int i = 0; i < songIndex; i++) {
		currNode = currNode->getNext(isShuffled());
	}
	m_currSong = currNode;
}

void SongQueue::setShuffled(const bool& shuffled)
{
	m_shuffled = shuffled;
	if (m_shuffled) {
		shuffle();
	}
}

void SongQueue::shuffle()
{
	if (isEmpty()) {
		return;
	}

	int indicator = 0;
	std::vector<QueueNode*> beforeCurrSong;
	std::vector<QueueNode*> subQueue;
	std::vector<QueueNode*> afterSubQueue;

	// Populate the above vectors.
	// They will be shuffled seperately (subQueue will maintain order though)
	for (QueueNode* node = m_head->getNext(false); node != m_tail; node = node->getNext(false)) {
		if (node == m_currSong) {
			indicator = 1;
			if (m_subQueueTail == nullptr || node == m_subQueueTail) {
				indicator = 2;
			}
			// don't add currSong to any vectors
			continue;
		}

		switch(indicator) {
		case 0:
			beforeCurrSong.push_back(node);
			break;
		case 1:
			subQueue.push_back(node);
			break;
		case 2:
			afterSubQueue.push_back(node);
			break;
		}

		if (node == m_subQueueTail) {
			indicator = 2;
		}
	}

	std::random_shuffle(beforeCurrSong.begin(), beforeCurrSong.end());
	std::random_shuffle(afterSubQueue.begin(), afterSubQueue.end());

	// copy subQueue normal order to shuffled order
	// this way subQueue maintains order
	if (!subQueue.empty()) {
		m_currSong->setNext(true, subQueue[0]);
		subQueue[0]->setPrev(true, m_currSong);

		for (auto s : subQueue) {
			s->setNext(true, s->getNext(false));
			s->setPrev(true, s->getNext(false));
		}
	}

	if (!beforeCurrSong.empty()) {
		m_head->setNext(true, beforeCurrSong[0]);
		beforeCurrSong[0]->setPrev(true, m_head);

		if (m_currSong != nullptr) {
			m_currSong->setPrev(true, beforeCurrSong[beforeCurrSong.size() - 1]);
			beforeCurrSong[beforeCurrSong.size() - 1]->setNext(true, m_currSong);
		}
		else if (!afterSubQueue.empty()) {
			afterSubQueue[0]->setPrev(true, beforeCurrSong[beforeCurrSong.size() - 1]);
			afterSubQueue[0]->getPrev(true)->setNext(true, afterSubQueue[0]);
			beforeCurrSong[beforeCurrSong.size() - 1]->setNext(true, afterSubQueue[0]);
		}
		else {
			m_tail->setPrev(true, beforeCurrSong[beforeCurrSong.size() - 1]);
			beforeCurrSong[beforeCurrSong.size() - 1]->setNext(true, m_tail);
		}

		for (int i = 0; i < beforeCurrSong.size(); ++i) {
			if (i != 0) {
				beforeCurrSong[i]->setPrev(true, beforeCurrSong[i - 1]);
			}
			if (i != beforeCurrSong.size() - 1) {
				beforeCurrSong[i]->setNext(true, beforeCurrSong[i + 1]);
			}
		}
	}

	if (!afterSubQueue.empty()) {
		m_subQueueTail->setNext(true, afterSubQueue[0]);
		afterSubQueue[0]->setPrev(true, m_subQueueTail);

		m_tail->setPrev(true, afterSubQueue[afterSubQueue.size() - 1]);
		afterSubQueue[afterSubQueue.size() - 1]->setNext(true, m_tail);

		for (int i = 0; i < afterSubQueue.size(); ++i) {
			if (i != 0) {
				afterSubQueue[i]->setPrev(true, afterSubQueue[i - 1]);
			}
			if (i != afterSubQueue.size() - 1) {
				afterSubQueue[i]->setNext(true, afterSubQueue[i + 1]);
			}
		}
	}
}


SongQueue::operator std::vector<Song>() const
{
	std::vector<Song> v;

	for (auto song : *this) {
		v.push_back(*song);
	}

	return v;
}


/*!
 *  @brief   Overrides insertion operator to ostream, outputs data as "Queue: [...]"
 *  @return  Reference to ostream
 */
std::ostream& operator<<(std::ostream& out, const SongQueue& queue)
{
	out << "Queue: [";

	auto song = queue.begin();
	out << (*song)->number;
	++song;

	for (; song != queue.end(); ++song) {
		out << ", " << (*song)->number;
	}
	out << "]";

	if (queue.m_currSong != nullptr) {
		out << " currSong: " << queue.m_currSong->data->number;
	}
	else {
		out << " currSong: nullptr";
	}
	
	if (queue.m_subQueueTail != nullptr) {
		out << " subQueueTail: " << queue.m_subQueueTail->data->number;
	}
	else {
		out << " subQueueTail: nullptr";
	}

	return out;
}


// Iterator Implementation
SongQueue::Iterator::Iterator(const SongQueue& songQueue, const SongQueue::QueueNode* node) noexcept :
	m_queue(songQueue), m_currNode(node) { };

SongQueue::Iterator SongQueue::begin() const
{
	if (isEmpty()) {
		return SongQueue::Iterator(*this, m_head);
	}
	
	return SongQueue::Iterator(*this, m_head->getNext(isShuffled()));
}

SongQueue::Iterator SongQueue::end() const
{
	return SongQueue::Iterator(*this, m_tail);
}

SongQueue::Iterator SongQueue::rbegin() const
{
	if (isEmpty()) {
		return SongQueue::Iterator(*this, m_tail);
	}
	return SongQueue::Iterator(*this, m_tail->getPrev(isShuffled()));
}

SongQueue::Iterator SongQueue::rend() const
{
	return SongQueue::Iterator(*this, nullptr);
}



SongQueue::Iterator& SongQueue::Iterator::operator=(const SongQueue::QueueNode* node)
{
	m_currNode = node;
	return *this;
}

SongQueue::Iterator& SongQueue::Iterator::operator++()
{
	if (m_currNode) {
		m_currNode = m_currNode->getNext(m_queue.isShuffled());
	}
	return *this;
}

SongQueue::Iterator SongQueue::Iterator::operator++(int)
{
	Iterator iter = *this;
	++* this;
	return iter;
}

SongQueue::Iterator& SongQueue::Iterator::operator--()
{
	if (m_currNode)
		m_currNode = m_currNode->getPrev(m_queue.isShuffled());
	return *this;
}

SongQueue::Iterator SongQueue::Iterator::operator--(int)
{
	Iterator iter = *this;
	--* this;
	return iter;
}

bool SongQueue::Iterator::operator!=(const Iterator& iter) const
{
	return m_currNode != iter.m_currNode;
}


Song const * SongQueue::Iterator::operator*() const
{
	return (m_currNode->data).get();
}


// QueueNode Implementation
SongQueue::QueueNode::QueueNode() :
	next(nullptr), prev(nullptr), nextShuffled(nullptr), prevShuffled(nullptr) { };


SongQueue::QueueNode::QueueNode(std::unique_ptr<Song> song) :
	data(std::move(song)), next(nullptr), prev(nullptr), nextShuffled(nullptr), prevShuffled(nullptr) {};


void SongQueue::QueueNode::setNext(QueueNode* node)
{
	next = node;
	nextShuffled = node;
}


void SongQueue::QueueNode::setPrev(QueueNode* node)
{
	prev = node;
	prevShuffled = node;
}


void SongQueue::QueueNode::setNext(const bool& shuffled, QueueNode* node)
{
	if (shuffled) {
		nextShuffled = node;
	}
	else {
		next = node;
	}
}


void SongQueue::QueueNode::setPrev(const bool& shuffled, QueueNode* node)
{
	if (shuffled) {
		prevShuffled = node;
	}
	else {
		prev = node;
	}
}

SongQueue::QueueNode* SongQueue::QueueNode::getNext(const bool& shuffled) const
{
	return shuffled ? nextShuffled : next;
}


SongQueue::QueueNode* SongQueue::QueueNode::getPrev(const bool& shuffled) const
{
	return shuffled ? prevShuffled : prev;
}