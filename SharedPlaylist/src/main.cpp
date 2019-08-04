#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include <string>
#include <iostream>
#include <ctime>

#include "adt/player.h"
#include "adt/song_queue.h"


bool getPopulatedQueue(SongQueue&, int);
void doStuff();


int main(std::string* argv) 
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	// Nothing here, just running unit tests for now.
	std::srand(std::time(0));
	doStuff();

	exit(0);
}

void doStuff()
{
	SongQueue queue;
	std::unique_ptr<Song> song;

	for (int i = 0; i < 6; ++i) {
		song = std::make_unique<Song>(i);
		queue.addToSubQueue(std::move(song));
	}
	for (int i = 6; i < 10; ++i) {
		song = std::make_unique<Song>(i);
		queue.addToQueue(std::move(song));
	}

	queue.setCurrSong(3);

	std::cout << "Before:     " << queue << std::endl;

	queue.setShuffled(true);
	std::cout << "Shuffle:    " << queue << std::endl;

	song = std::make_unique<Song>(-1);
	queue.addToSubQueue(std::move(song));
	std::cout << "Add to SubQueue:    " << queue << std::endl;

	song = std::make_unique<Song>(-2);
	queue.addToQueue(std::move(song));
	std::cout << "Add to Queue:    " << queue << std::endl;

	queue.setShuffled(false);
	std::cout << "Unshuffle:  " << queue << std::endl;

	queue.setShuffled(true);
	std::cout << "Reshuffle:  " << queue << std::endl;

	queue.setShuffled(false);
	std::cout << "Unshuffle:  " << queue << std::endl;
}


bool getPopulatedQueue(SongQueue& queue, int len) 
{
	queue.clear();
	std::unique_ptr<Song> song;
	for (int i = 0; i < len; i++) {
		song = std::make_unique<Song>(i);
		queue.addToQueue(std::move(song));
	}
	return true;
}