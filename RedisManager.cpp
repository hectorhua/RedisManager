/**
 * Copyright (c) 2014,travelsky
 * All rights reserved. 
 * Travelsky CONFIDENTIAL
 *  
 * @file       RedisManager.cpp
 * @brief      （简要描述）
 *
 * @details    （详细描述关键点）
 *
 * @author     NightmareHelen
 * @version    产品版本信息1 yyyy-mm-dd 姓名 修改信息
 * @version    产品版本信息2 yyyy-mm-dd 姓名 修改信息
 */

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <map>

#include "RedisManager.h"

RedisManager::RedisManager():rdsctx(NULL), ip(""), port(0), timeout(0) {

}

RedisManager::RedisManager(const char* ip, long port,  long timeout){
	rdsctx = NULL;

	init(ip, port, timeout);
}

RedisManager::~RedisManager() {
	if(rdsctx){
		redisFree(rdsctx);
		rdsctx = NULL;
	}
}

int RedisManager::init(const char* ip, unsigned int port,  long timeout){
	this->ip = ip;
	this->port = port;
	this->timeout = timeout;
	struct timeval tm = {timeout, 0}; // 1 seconds

	if(rdsctx){
		redisFree(rdsctx);
		rdsctx = NULL;
	}

	rdsctx = redisConnectWithTimeout(this->ip.c_str(), port, tm);
	return rdsctx->err;

}

int RedisManager::getErrno(){
	return rdsctx->err;
}

const char* RedisManager::getErrstr(){
	return rdsctx->errstr;
}

int RedisManager::sGet(const std::string& key, std::vector<std::string>& values){
	redisReply* reply=NULL;
	reply = (redisReply*)redisCommand(rdsctx, "smembers %s", key.c_str());

	/*
	 * imply the connection is lost ,
	 * so reconnect until the connection is OK.
	 */
	if(!reply){
		while(init(ip.c_str(), port, timeout)){
		
		}
	}

	/*
	 * if result set contains data,just copy them to the vector:values
	 */
	if((reply!=NULL) && (reply->type == REDIS_REPLY_ARRAY) && (reply->elements>0)){
		for(size_t i=0; i<reply->elements; i++){
			values.push_back(reply->element[i]->str);
		}
	}
	if(reply != NULL){
		freeReplyObject(reply);
	}

	return rdsctx->err;
}

int RedisManager::sSet(const std::string& key, const std::string& value){
	redisReply* reply=NULL;
	reply = (redisReply*)redisCommand(rdsctx, "sadd %s %s", key.c_str(), value.c_str());

	if(reply != NULL){
		freeReplyObject(reply);
	}

	return reply->integer;
}

int RedisManager::hashSet(const std::string& hash, const std::string& key, const std::string& value){
	redisReply* reply=NULL;
	reply = (redisReply*)redisCommand(rdsctx, "HSET %s %s %s", hash.c_str(), key.c_str(), value.c_str());

	if(reply != NULL){
		freeReplyObject(reply);
	}

	return reply->integer;
}

int RedisManager::hashGetALL(const std::string& hash, std::map<std::string, std::string>& values){
	redisReply* reply=NULL;
	reply = (redisReply*)redisCommand(rdsctx, "HGETALL %s", hash.c_str());

	if(!reply){
		while(init(ip.c_str(), port, timeout)){
		
		}
	}

	/*
	 * if result set contains data,just copy them to the vector:values
	 */
	if((reply!=NULL) && (reply->type == REDIS_REPLY_ARRAY) && (reply->elements>0)){

		for(size_t i=0; i<reply->elements-1; i=i+2){
			values.insert(std::make_pair(reply->element[i]->str,reply->element[i+1]->str));
		}
	}
	if(reply != NULL){
		freeReplyObject(reply);
	}

	return rdsctx->err;
}

int RedisManager::hashGet(const std::string& hash, const std::string& key, std::vector<std::string>& values){
	redisReply* reply=NULL;
	reply = (redisReply*)redisCommand(rdsctx, "HGET %s %s", hash.c_str(), key.c_str());

	if(!reply){
		while(init(ip.c_str(), port, timeout)){
		
		}
	}

	/*
	 * if result set contains data,just copy them to the vector:values
	 */
	if((reply!=NULL) && (reply->type == REDIS_REPLY_STRING)){
		values.push_back(reply->str);
	}
	if(reply != NULL){
		freeReplyObject(reply);
	}

	return rdsctx->err;
}

int RedisManager::hashMGet(const std::string& hash, std::vector<std::string>& fields, std::vector<std::string>& values){
	
	redisReply* reply=NULL;
	reply = (redisReply*)redisCommand(rdsctx, "HMGET %s %s %s %s", hash.c_str(), fields[0].c_str(), fields[1].c_str(), fields[2].c_str());
	
	if(!reply){
		while(init(ip.c_str(), port, timeout)){
		
		}
	}

	/*
	 * if result set contains data,just copy them to the vector:values
	 */
	if((reply!=NULL) && (reply->type == REDIS_REPLY_ARRAY) && (reply->elements>0)){
		
		for(size_t i=0; i<reply->elements; i++){
			values.push_back(reply->element[i]->str);
		}
	}
	if(reply != NULL){
		freeReplyObject(reply);
	}

	return rdsctx->err;
}

int RedisManager::getKeys(const std::string& getkey, std::vector<std::string>& values){
	redisReply* reply=NULL;
	reply = (redisReply*)redisCommand(rdsctx, "KEYS %s", getkey.c_str());

	if(!reply){
		while(init(ip.c_str(), port, timeout)){
		
		}
	}

	/*
	 * if result set contains data,just copy them to the vector:values
	 */
	if((reply!=NULL) && (reply->type == REDIS_REPLY_ARRAY) && (reply->elements>0)){
		
		for(size_t i=0; i<reply->elements; i++){
			values.push_back(reply->element[i]->str);
		}
	}
	if(reply != NULL){
		freeReplyObject(reply);
	}

	return rdsctx->err;
}

int RedisManager::getAllKeys(std::vector<std::string>& keys){
	redisReply* reply=NULL;
	
	bool isFirstTime = true;
	char buffer[256];
	sprintf(buffer, "0");
	
	/*
	*使用scan 指令得到所有的key值
	* SCAN 命令的回复是一个包含两个元素的数组，
	*第一个数组元素是用于进行下一次迭代的新游标， 
	*而第二个数组元素则是一个数组， 
	*这个数组中包含了所有被迭代的元素。
	*第一次迭代使用 0 作为游标， 表示开始一次新的迭代,
	*当迭代结束时，返回游标返回值为0
	*/
	while(strcmp(buffer, "0") != 0 || isFirstTime){

		reply = (redisReply*)redisCommand(rdsctx, "scan %s", buffer);
		/*
		 * imply the connection is lost ,
		 * so reconnect until the connection is OK.
		 */
		if(!reply){
			while(init(ip.c_str(), port, timeout)){
			
			}
		}
		/*
		 * if result set contains data,just copy them to the vector:values
		 */
		if((reply!=NULL) && (reply->type == REDIS_REPLY_ARRAY) && (reply->elements>0)){
			strcpy(buffer, reply->element[0]->str);
			for(int i=0; i<reply->element[1]->elements; i++){
				keys.push_back(reply->element[1]->element[i]->str);
			}
		}

		if(reply != NULL){
			freeReplyObject(reply);
		}
		
		if(isFirstTime){
			isFirstTime = false;
		}
	}

	return rdsctx->err;
}

int RedisManager::sRemove(const std::string& key, const std::string& value){
	redisReply* reply=NULL;
	reply = (redisReply*)redisCommand(rdsctx, "srem %s %s", key.c_str(), value.c_str());

	if(reply != NULL){
		freeReplyObject(reply);
	}

	return reply->integer;
}

//打印vector
void RedisManager::printQueryResult(std::vector<std::string>& values){
	printf("%s\n","Print Query Result:");
	for(size_t i=0; i<values.size(); i++){
		printf("%s\n",values[i].c_str());
	}
}

//打印map
void RedisManager::printQueryResult_map(std::map<std::string, std::string>& values){
	printf("%s\n","Print Query Result_map:");
	std::map<std::string, std::string>::iterator it;
	for(it=values.begin(); it!=values.end(); ++it){
		printf("%s %s\n",it->first.c_str(), it->second.c_str());
	}
	
}