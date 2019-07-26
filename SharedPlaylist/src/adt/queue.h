#ifndef QUEUE_H
#define QUEUE_H

#include <memory>
#include "song.h"
#include "player.h"
#include <vector>


class Queue
{

private:
	class QueueNode
	{
	public:
		QueueNode() : next(NULL), prev(NULL) { };
		QueueNode(std::unique_ptr<Song> song) : data(std::move(song)), next(NULL), prev(NULL) {};

		~QueueNode()
		{
			data.reset();
		}

		std::unique_ptr<Song> data;
		QueueNode* next;
		QueueNode* prev;
	};

	QueueNode* m_head;
	QueueNode* m_tail;
	QueueNode* m_spotifyQueueTail;
	QueueNode* m_currSong;

	int m_size;


public:
	Queue();
	~Queue();
	void addToQueue(std::unique_ptr<Song> song);
	void removeFromQueue(const int& songIndex);
	void moveSong(const int& oldSongIndex, const int& newSongIndex);
	bool nextSong(Player::RepeatMode repeatMode);
	bool prevSong();

	void clear();

	// GETTERS
	Song* getSongAt(int songIndex) const;
	bool isEmpty() const;
	int size() const;

	// SETTERS
	void setCurrSong(const int& songIndex);

	// OVERLOADS
	operator std::vector<Song>() const;


	class Iterator;
	Iterator begin() const;
	Iterator end() const;
	Iterator rbegin() const;
	Iterator rend() const;

	class Iterator
	{
	public:
		Iterator(const QueueNode* node) noexcept : m_currNode(node) { }

		Iterator& operator=(QueueNode* node);

		Iterator& operator++();    // Prefix
		Iterator operator++(int);  // Postfix

		Iterator& operator--();    // Prefix
		Iterator operator--(int);  // Postfix

		bool operator!=(const Iterator& iter);
		const std::unique_ptr<Song>& operator*();

	private:
		const QueueNode* m_currNode;
	};

};


#endif
