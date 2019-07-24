#include "pch.h"

#include <memory>
#include <iostream>

#include "../SharedPlaylist/src/adt/queue.h"
#include "../SharedPlaylist/src/adt/song.h"


#define TEST_FLAG 0


int checkPointers(const Queue& queue) {
	std::vector<Song> songs = queue;
	
	auto song = queue.rbegin();
	int i;
	for (i = songs.size()-1; i >= 0 && song != queue.rend(); i--, song--) {
		std::cout << songs[i].number << " | " << (*song)->number << std::endl;
		EXPECT_EQ(songs[i].number, (*song)->number);
	}

	EXPECT_EQ(i, -1);

	return 0;
}

bool getPopulatedQueue(Queue& queue, int len) {
	queue.clear();
	std::unique_ptr<Song> song;
	for (int i = 0; i < len; i++) {
		song = std::make_unique<Song>(i);
		queue.addToQueue(std::move(song));
	}
	return true;
}

TEST(QueueTests, QueueIsEmpty) {
	Queue queue;
	
	EXPECT_EQ(true, queue.isEmpty());

	getPopulatedQueue(queue, 2);

	EXPECT_EQ(false, queue.isEmpty());

	queue.removeFromQueue(0);
	queue.removeFromQueue(0);

	EXPECT_EQ(true, queue.isEmpty());
}


// QueueRemoval depends on QueueIsEmpty
TEST(QueueTests, QueueRemoval) {
	Queue queue;

	getPopulatedQueue(queue, 3);
	checkPointers(queue);

	// Queue size should be 3
	EXPECT_EQ(3, queue.size());

	queue.removeFromQueue(1);
	queue.removeFromQueue(1);
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

TEST(QueueTests, QueueMoving1) {
	Queue queue;
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

TEST(QueueTests, QueueMoving2) {
	Queue queue;
	std::vector<Song> songs;
	getPopulatedQueue(queue, 5);

	// Move song to head
	queue.moveSong(2, 0);
	songs = queue;
	EXPECT_EQ(2, songs[0].number);
	EXPECT_EQ(0, songs[1].number);

	// Move song from tail
	queue.moveSong(4, 3);
	songs = queue;
	EXPECT_EQ(4, songs[3].number);
	EXPECT_EQ(3, songs[4].number);
}

TEST(QueueTests, QueueMoving3) {
	Queue queue;
	std::vector<Song> songs;
	getPopulatedQueue(queue, 5);

	// Move song from head
	queue.moveSong(0, 2);
	songs = queue;
	EXPECT_EQ(0, songs[2].number);
	EXPECT_EQ(1, songs[0].number);

	// Move song from tail
	queue.moveSong(4, 0);
	songs = queue;
	EXPECT_EQ(3, songs[4].number);
	EXPECT_EQ(4, songs[0].number);
}

TEST(QueueTests, QueueGetSongAt) {
	Queue queue;
	getPopulatedQueue(queue, 5);

	EXPECT_EQ(0, queue.getSongAt(0)->number);
	EXPECT_EQ(nullptr, queue.getSongAt(5));
}