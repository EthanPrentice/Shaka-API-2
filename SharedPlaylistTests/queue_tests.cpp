#include "pch.h"

#include <memory>
#include <iostream>
#include <iostream>

#include "../SharedPlaylist/src/adt/song_queue.h"
#include "../SharedPlaylist/src/adt/song.h"


// Checks whether or not iterating forward is the same as backwards.
void checkPointers(const SongQueue& queue) 
{
	std::vector<Song> songs = queue;
	auto song = queue.rbegin();
	int i;

	for (i = songs.size() - 1; i >= 0 && song != queue.rend(); i--, song--) {
		EXPECT_EQ(songs[i].number, (*song)->number);
	}
	EXPECT_EQ(i, -1);
}

void getPopulatedQueue(SongQueue& queue, int len)
{
	queue.clear();
	std::unique_ptr<Song> song;
	for (int i = 0; i < len; i++) {
		song = std::make_unique<Song>(i);
		queue.addToQueue(std::move(song));
	}
}

bool queueEqualsVector(SongQueue& queue, std::vector<int> vector)
{
	if (queue.size() != vector.size()) {
		return false;
	}

	auto song = queue.begin();
	for (int i = 0; song != queue.end(); ++song, ++i) {
		if ((*song)->number != vector[i]) {
			return false;
		}
	}

	return true;
}


// Simple test for queue.isEmpty()
TEST(QueueTests, QueueIsEmpty) 
{
	SongQueue queue;
	
	EXPECT_EQ(true, queue.isEmpty());

	getPopulatedQueue(queue, 2);

	EXPECT_EQ(false, queue.isEmpty());

	queue.clear();

	EXPECT_EQ(true, queue.isEmpty());
}


// Depends of QueueIsEmpty
// 
TEST(QueueTests, QueueRemoval) 
{
	SongQueue queue;

	getPopulatedQueue(queue, 3);
	checkPointers(queue);

	// Queue size should be 3
	EXPECT_EQ(3, queue.size());

	// Remove 2 values from queue -> Queue size should be 2
	std::vector<Song> songs = queue;
	queue.removeFromQueue(1);
	songs = queue;
	queue.removeFromQueue(1);
	songs = queue;
	EXPECT_EQ(1, queue.size());

	checkPointers(queue);

	// Index 30 is out of range -> expect error
	bool wasError = false;
	try {
		queue.removeFromQueue(30);
	}
	catch (const std::invalid_argument e) {
		std::cerr << e.what();
		wasError = true;
	}
	EXPECT_EQ(true, wasError);
	EXPECT_EQ(1, queue.size());

	queue.removeFromQueue(0);

	// All values removed from queue -> expect queue to be empty
	EXPECT_EQ(true, queue.isEmpty());
}

// Depends of QueueIsEmpty
// 
TEST(QueueTests, QueueRemovalShuffled)
{
	SongQueue queue;

	getPopulatedQueue(queue, 3);
	checkPointers(queue);

	// Queue size should be 3
	EXPECT_EQ(3, queue.size());

	// Remove 2 values from queue -> Queue size should be 2
	std::vector<Song> songs = queue;
	queue.removeFromQueue(1);
	songs = queue;
	queue.removeFromQueue(1);
	songs = queue;
	EXPECT_EQ(1, queue.size());

	checkPointers(queue);

	// Index 30 is out of range -> expect error
	bool wasError = false;
	try {
		queue.removeFromQueue(30);
	}
	catch (const std::invalid_argument e) {
		std::cerr << e.what();
		wasError = true;
	}
	EXPECT_EQ(true, wasError);
	EXPECT_EQ(1, queue.size());

	queue.removeFromQueue(0);

	// All values removed from queue -> expect queue to be empty
	EXPECT_EQ(true, queue.isEmpty());
}



// Tests moving in the middle of the list
TEST(QueueTests, QueueMoving1)
{
	SongQueue queue;
	std::vector<Song> songs;
	getPopulatedQueue(queue, 5);

	// Move song forward
	queue.moveSong(1, 3);
	songs = queue;
	EXPECT_EQ(2, songs[1].number);
	EXPECT_EQ(1, songs[3].number);

	// Move song backward
	queue.moveSong(3, 1);
	songs = queue;
	EXPECT_EQ(1, songs[1].number);
	EXPECT_EQ(3, songs[3].number);
}

// Tests moving songs to the first / last elements
TEST(QueueTests, QueueMoving2) 
{
	SongQueue queue;
	bool isValid;
	getPopulatedQueue(queue, 5);

	// Move song to head
	queue.moveSong(2, 0);
	isValid = queueEqualsVector(queue, {2, 0, 1, 3, 4});
	EXPECT_EQ(true, isValid);

	// Move song to tail
	queue.moveSong(2, 4);
	isValid = queueEqualsVector(queue, {2, 0, 3, 4, 1});
	EXPECT_EQ(true, isValid);

}

// Tests moving songs from the first / last elements
TEST(QueueTests, QueueMoving3) 
{
	SongQueue queue;
	bool isValid;
	getPopulatedQueue(queue, 5);

	// Move song from head
	queue.moveSong(0, 2);
	isValid = queueEqualsVector(queue, {1, 2, 0, 3, 4});
	EXPECT_EQ(true, isValid);

	// Move song from tail to head
	queue.moveSong(4, 0);
	isValid = queueEqualsVector(queue, {4, 1, 2, 0, 3});
	EXPECT_EQ(true, isValid);
}

TEST(QueueTests, QueueGetSongAt) 
{
	SongQueue queue;
	getPopulatedQueue(queue, 5);

	EXPECT_EQ(0, queue.getSongAt(0)->number);
	EXPECT_EQ(nullptr, queue.getSongAt(5));
}



TEST(QueueTests, ShuffleTest) 
{
	SongQueue queue;
	getPopulatedQueue(queue, 10);

	queue.setShuffled(true);

}