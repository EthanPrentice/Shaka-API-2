#ifndef QUEUE_H
#define QUEUE_H

#include <memory>
#include <vector>

#include "song.h"
#include "repeat_mode.h"


class SongQueue
{
public:
	SongQueue();
	~SongQueue();
	void addToQueue(std::unique_ptr<Song> song);
	void addToSubQueue(std::unique_ptr<Song> song);
	void removeFromQueue(const int& songIndex);
	void moveSong(const int& oldSongIndex, const int& newSongIndex);
	bool nextSong(RepeatMode repeatMode);
	bool prevSong();

	void clear();

	// GETTERS
	const Song* getSongAt(int songIndex) const;
	bool isEmpty() const;
	bool isShuffled() const;
	int size() const;

	// SETTERS
	void setCurrSong(const int& songIndex);
	void setShuffled(const bool& shuffled);

	// OVERLOADS
	operator std::vector<Song>() const;
	friend std::ostream& operator<<(std::ostream& out, const SongQueue& queue);


	class Iterator;
	class QueueNode;

	Iterator begin() const;
	Iterator end() const;
	Iterator rbegin() const;
	Iterator rend() const;

	class Iterator
	{
	public:
		// Iterator(const SongQueue& songQueue, const QueueNode* node);
		Iterator(const SongQueue& songQueue, const SongQueue::QueueNode* node) noexcept;

		Iterator& operator=(const QueueNode* node);

		Iterator& operator++();    // Prefix
		Iterator operator++(int);  // Postfix

		Iterator& operator--();    // Prefix
		Iterator operator--(int);  // Postfix

		bool operator!=(const Iterator& iter) const;
		Song const * operator*() const;

	private:
		const SongQueue& m_queue;
		const QueueNode* m_currNode;
	};


private:

	// No copying from SongQueue
	SongQueue(const SongQueue&) = delete;
	void operator=(const SongQueue&) = delete;

	class QueueNode
	{
	public:
		QueueNode();
		QueueNode(std::unique_ptr<Song> song);

		void setNext(QueueNode* node);
		void setPrev(QueueNode* node);

		void setNext(const bool& shuffled, QueueNode* node);
		void setPrev(const bool& shuffled, QueueNode* node);

		QueueNode* getNext(const bool& shuffled) const;
		QueueNode* getPrev(const bool& shuffled) const;

		std::unique_ptr<Song> data;


	private:
		QueueNode* next;
		QueueNode* prev;
		QueueNode* nextShuffled;
		QueueNode* prevShuffled;
	};


	void shuffle();


	/// Data will be nullptr so songs can be unshuffled and maintain previous order
	QueueNode* m_head;
	/// Data will be nullptr so songs can be unshuffled and maintain previous order
	QueueNode* m_tail;

	QueueNode* m_subQueueTail;
	QueueNode* m_currSong;

	int m_size;
	bool m_shuffled;
};


#endif