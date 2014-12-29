/**
 * Copyright (c) 2014
 * All rights reserved. 
 *  
 * @file       RedisManager.h
 * @brief      （简要描述）
 *
 * @details    （详细描述关键点）
 *
 * @author     NightmareHelen
 * @version    产品版本信息1 yyyy-mm-dd 姓名 修改信息
 * @version    产品版本信息2 yyyy-mm-dd 姓名 修改信息
 */

#ifndef REDISMANAGER_H_
#define REDISMANAGER_H_

#include <string>
#include <vector>

#include "hiredis.h"

class RedisManager {
public:
	RedisManager();
	RedisManager(const char* ip, long port, long timeout);
	virtual ~RedisManager();

	/*
	 * return value is 0 when everything is OK,
	 * not 0 otherwise
	 */
	int init(const char* ip, unsigned int port,  long timeout);
	int getErrno();
	const char* getErrstr();

	int sGet(const std::string& key, std::vector<std::string>& values);

	/*
	 * if key not exist,just add a new pair
	 * otherwise, the old one is overwrite
	 */
	int sSet(const std::string& key, const std::string& values);

	int hashSet(const std::string& hash, const std::string& key, const std::string& values);
	
	/*
	 * just remove one or multiple elements from the set, multiple elements should be separated by blank space
	 */
	int sRemove(const std::string& key, const std::string& value);
	
	/*
	 *get all the key in redis
	 *@param keys return result of the keys in redis
	 */
	int getAllKeys(std::vector<std::string>& keys);
	
	int getKeys(const std::string& getkey, std::vector<std::string>& values);
	
	int hashGetALL(const std::string& hash, std::map<std::string, std::string>& values);
	
	int hashGet(const std::string& hash, const std::string& key, std::vector<std::string>& values);
	
	int hashMGet(const std::string& hash, std::vector<std::string>& fields, std::vector<std::string>& values);

	static void printQueryResult(std::vector<std::string>& values);
	
	static void printQueryResult_map(std::map<std::string, std::string>& values);
	
private:
	redisContext* rdsctx;
	std::string ip;
	unsigned int port;
	long timeout;
};

#endif
