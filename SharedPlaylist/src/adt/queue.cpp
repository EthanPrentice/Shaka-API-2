#include <memory>
#include "queue.h"
#include <cassert>
#include <stdexcept>


Queue::Queue() : 
	m_head(nullptr), m_tail(nullptr), m_spotifyQueueTail(nullptr), m_currSong(nullptr), m_size(0) { }


Queue::~Queue()
{
	QueueNode* tmp = m_head;
	while (m_head != NULL) {
		tmp = m_head->next;
		delete m_head;
		m_head = tmp;
	}
}


/**
 *  @brief      Adds a song to the end of the SpotifyQueue (same way Spotify handles it)
 */
void Queue::addToQueue(std::unique_ptr<Song> song)
{
	if (isEmpty()) {
		m_head = new QueueNode(std::move(song));
		m_tail = m_head;
		m_spotifyQueueTail = m_head;
	}
	else {
		QueueNode* newNode = new QueueNode(std::move(song));
		QueueNode* tmp = m_spotifyQueueTail->next;
		
		// update nodes pointing to newNode
		m_spotifyQueueTail->next = newNode;
		if (tmp != nullptr) {
			tmp->prev = newNode;
		}

		// update newNode pointers
		newNode->next = tmp;
		newNode->prev = m_spotifyQueueTail;

		if (m_tail == m_spotifyQueueTail) {
			m_tail = m_tail->next;
		}
		m_spotifyQueueTail = m_spotifyQueueTail->next;
	}

	m_size += 1;
}


/**
 *  @brief      Removes a song at an index
 *  @param[in]  songIndex index of song to delete
 *
 */
void Queue::removeFromQueue(const int& songIndex)
{

	if (songIndex < 0 || songIndex >= m_size) {
		throw std::invalid_argument("songIndex out of bounds");
	}

	int i = 0;
	QueueNode* currNode = m_head;
	while (i++ != songIndex) {
		currNode = currNode->next;
		if (currNode == NULL) {
			throw std::invalid_argument("Internal logic error, NULL node found before expected.");
		}
	}

	// Queue has one item, will be empty after removal
	if (currNode == m_head && currNode == m_tail) {
		m_currSong = NULL;
		m_spotifyQueueTail = NULL;
		m_head = NULL;
		m_tail = NULL;
		delete currNode;

		m_size -= 1;
		return;
	}

	if (currNode != m_tail && currNode != m_head) {
		currNode->prev->next = currNode->next;
		currNode->next->prev = currNode->prev;
	}

	if (currNode == m_head) {
		m_head = m_head->next;
		m_head->prev = NULL;
	}
	if (currNode == m_tail) {
		m_tail = m_tail->prev;
		m_tail->next = NULL;
	}

	if (currNode == m_spotifyQueueTail) {
		m_spotifyQueueTail = currNode->prev;
	}
	if (currNode == m_currSong) {
		if (m_currSong->prev == NULL) {
			m_currSong = NULL;
		}
		else {
			m_currSong = m_currSong->prev;
		}
	}
	delete currNode;

	m_size -= 1;
}


/**
 *  @brief      Adds a song to the end of the SpotifyQueue (same way Spotify handles it)
 *  @param[in]  songIndex     The index of where the song currently is
 *  @param[in]  newSongIndex  The index to move the song to
 *
 */
void Queue::moveSong(const int& songIndex, const int& newSongIndex)
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

	QueueNode* songNode = NULL;
	QueueNode* newSongNode = NULL;

	QueueNode* currNode = m_head;
	while (i <= maxIndex) {
		if (currNode == nullptr) {
			throw std::invalid_argument("Internal logic error, NULL node found before expected.");
		}
		if (i == songIndex) {
			songNode = currNode;
		}
		else if (i == newSongIndex) {
			newSongNode = currNode;
		}

		currNode = currNode->next;
		i += 1;
	}

	assert(songNode != NULL);
	assert(newSongNode != NULL);

	if (songNode == m_currSong) {
		throw std::invalid_argument("Cannot move the currently playing song.");
	}

	if (songNode == m_spotifyQueueTail) {
		m_spotifyQueueTail = songNode->prev;
	}


	if (songNode != m_head) {
		songNode->prev->next = songNode->next;
	}
	if (songNode != m_tail) {
		songNode->next->prev = songNode->prev;
	}

	// move forward
	if (songIndex < newSongIndex) {
		if (songNode == m_head) {
			m_head = songNode->next;
		}

		songNode->next = newSongNode->next;
		songNode->prev = newSongNode;

		if (newSongNode == m_tail) {
			m_tail = songNode;
		} 
		else {
			newSongNode->next->prev = songNode;
		}
		newSongNode->next = songNode;
	}
	// move backward
	else {
		if (songNode == m_tail) {
			m_tail = songNode->prev;
		}
		songNode->next = newSongNode;
		songNode->prev = newSongNode->prev;

		if (newSongNode == m_head) {
			m_head = songNode;
		}
		else {
			newSongNode->prev->next = songNode;
		}
		newSongNode->prev = songNode;
	}

}


/**
 *  @brief   Moves currSong to next song if available
 *  @param   repeatMode Enum for what RepeatMode is set to within the Player
 *
 *  @return  True if moved back successfully. Only returns false when currSong is the first in queue
 */
bool Queue::nextSong(const Player::RepeatMode repeatMode)
{
	if (m_currSong->next != nullptr) {
		m_currSong = m_currSong->next;
		return true;
	}
	return false;
}


/**
 *  @brief   Moves currSong to previous if available
 *  @return  True if moved back successfully. Only returns false when currSong is the first in queue
 */
bool Queue::prevSong()
{
	if (m_currSong->prev != nullptr) {
		m_currSong = m_currSong->prev;
		return true;
	}
	return false;
}

/**
 *  @brief   Removes all items from the queue
 */
void Queue::clear() 
{
	while (!isEmpty()) {
		removeFromQueue(0);
	}
}

/**
 *  @brief   Should only be used when only one item needs to be accessed.
 *  @return  Returns pointer to Song at songIndex, or nullptr if songIndex is out of range
 */
Song* Queue::getSongAt(int songIndex) const
{
	if (songIndex < 0 || songIndex >= m_size) {
		return nullptr;
	}

	auto song = begin();
	for (int i = 0; song != end(); ++i, ++song) {
		if (i == songIndex) {
			return (*song).get();
		}
	}

	return nullptr;
}


/**
 *  @brief   Checks if queue contains no songs
 *  @return  True if queue contains no songs, false otherwise
 */
bool Queue::isEmpty() const 
{
	return m_head == NULL;
}


int Queue::size() const
{
	return m_size;
}


void Queue::setCurrSong(const int& songIndex)
{
	if (songIndex < 0 || songIndex >= m_size) {
		throw std::invalid_argument("songIndex is out of bounds");
	}

	QueueNode* currNode = m_head;
	for (int i = 0; i < songIndex; i++) {
		currNode = currNode->next;
	}
	m_currSong = currNode;
}


/**
 *  @brief   Overrides cast to vector.
             Should only really be used for testing or if queue won't be added/removed for awhile and multiple
			 operations need to be done, that way we get constant access
 *  @return  True if queue contains no songs, false otherwise
 */
Queue::operator std::vector<Song>() const
{
	std::vector<Song> songs;
	for (auto song = begin(); song != end(); ++song) {
		songs.push_back(**song);
	}

	return songs;
}


// Iterator stuff
Queue::Iterator Queue::begin() const
{
	return Queue::Iterator(m_head);
}

Queue::Iterator Queue::end() const
{
	return Queue::Iterator(nullptr);
}

Queue::Iterator Queue::rbegin() const
{
	return Queue::Iterator(m_tail);
}

Queue::Iterator Queue::rend() const
{
	return Queue::Iterator(nullptr);
}

Queue::Iterator& Queue::Iterator::operator=(Queue::QueueNode* node)
{
	this->m_currNode = node;
	return *this;
}

Queue::Iterator& Queue::Iterator::operator++()
{
	if (m_currNode) {
		m_currNode = m_currNode->next;
	}
	return *this;
}

Queue::Iterator Queue::Iterator::operator++(int)
{
	Iterator iter = *this;
	++* this;
	return iter;
}

Queue::Iterator& Queue::Iterator::operator--()
{
	if (m_currNode)
		m_currNode = m_currNode->prev;
	return *this;
}

Queue::Iterator Queue::Iterator::operator--(int)
{
	Iterator iter = *this;
	--* this;
	return iter;
}

bool Queue::Iterator::operator!=(const Iterator& iter)
{
	return m_currNode != iter.m_currNode;
}

const std::unique_ptr<Song>& Queue::Iterator::operator*()
{
	return m_currNode->data;
}