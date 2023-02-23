#pragma once
#include"stdafx.h"
#include<fstream>
#include<list>
#include<string.h>

#define FILE_LOADED 0x0
#define UNABLE_TO_LOCATE_FILE 0x1
/*
Dont confuse it with blam cache
Its the cache file which i generate from MapHandler
-removed functions and codes that mutate the cache
*/
struct cache_BLOCK
{
	std::string name;
	int offset;//offset in the file,if loaded from disk
	int size;
	char* data;//actual data if generated at runtime
};
class cache_loader
{
	std::string cache_file_loc;
	//stream readers and writers
	std::ifstream cache_in;
	//std::ofstream cache_out;

	std::list<cache_BLOCK> cache_list;
	int _last_error;

public:
	//void add_BLOCK(std::string name,int size,char* data);

	//returns pointer to a BLOCK from the cache file based on the name supplied
	//no need to call delete,its a pointer maintained by cache_loader
	cache_BLOCK* get_BLOCK(std::string name);
	int get_last_error();

	cache_loader(std::string file_loc);//open a file for IO
	~cache_loader();
};

