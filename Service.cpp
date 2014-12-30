#include "RedisManager.h"

int main(){

	RedisManager* rm;
	rm = new RedisManager(redis_IP.c_str(), redis_Port, redis_Timeout);
	if(rm->getErrno()){
		userlog("ERROR: Fail to connect to REDIS database!");
		return -1;
	}else{
		userlog("Successfully connect to REDIS database!");
	}
	//Redis变量定义
	vector<string> vec_result;
	map<string, string> map_result;
	map<string, string>::iterator it;
	int ret; //执行redis后的返回值

	vec_result.clear();
	ret = rm->getKeys(cityID.c_str(), vec_result);
	if(rm->getErrno()){
		LOGERR(READ_REDIS_CITYID_ERR, "CityID %s: rm->getErrstr: %s",cityID.c_str(), rm->getErrstr());
	}
		
	ret = rm->hashGetALL(cityID.c_str(), map_result);
	if(rm->getErrno()){
		LOGERR(READ_REDIS_CITYID_ERR, "CityID %s: rm->getErrstr: %s",cityID.c_str(), rm->getErrstr());
	}
	it = map_result.find("city_name");
	if (it != map_result.end()){
		root_res["name_cn"]=it->second;
	}
	it = map_result.find("city_name_en");
	if (it != map_result.end()){
		root_res["name_en"]=it->second;
	}
}