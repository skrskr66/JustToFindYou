#pragma once
#include "ScanManager.h"
#include "Common.h"
#include "DataManager.h"

ScanManager ScanManager::scanmgr;

void ScanManager::Scan(const string& path)
{
	vector<string> localdir;
	vector<string> localfile;
	DirectorList(path, localdir, localfile);
	
	std::set<string> localset;
	localset.insert(localdir.begin(), localdir.end());
	localset.insert(localfile.begin(), localfile.end());

	std::set<string> dbset;
	DataManager::GetInstance()->GetData(path, dbset);

	auto localmod = localset.begin();
	auto dbmod = dbset.begin();

	while (localmod != localset.end() && dbmod != dbset.end())
	{
		if (*localmod < *dbmod) // 本地有，数据没有->新增数据
		{
			//增加数据
			DataManager::GetInstance()->InsertData(path, *localmod);
			++localmod;
		}
		else if (*localmod > *dbmod)// 本地没有，数据有->删除数据
		{
			//删除数据
			DataManager::GetInstance()->DeleteData(path, *dbmod);
			++dbmod;
		}
		else
		{
			++localmod;
			++dbmod;
		}
	}
	while (localmod != localset.end())
	{
		//增加数据
		DataManager::GetInstance()->InsertData(path, *localmod);
		++localmod;
	}
	while (dbmod != dbset.end())
	{
		//删除数据
		DataManager::GetInstance()->DeleteData(path, *dbmod);
		++dbmod;
	}

	//递归扫描子目录
	for (const auto subdirs : localdir)
	{
		string subpath = path;
		subpath += '\\';
		subpath += subdirs;
		Scan(subpath);
	}
}