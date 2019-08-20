#pragma once
#include "Common.h"
#include "DataManager.h"
#define DB_NAME "doc.db"
#define TABLE_NAME "table_doc"

DataManager* DataManager::datamgr;
std::mutex DataManager::_mtx;

void SqlManager::Open(const string& path)
{
	int ret = sqlite3_open(path.c_str(), &_db);
	if (ret != SQLITE_OK)
	{
		ERROE_LOG("sqlite3_open\n");
	}
	else
	{
		TRACE_LOG("sqlite3_open success\n");
	}
}

void SqlManager::Close()
{
	int ret = sqlite3_close(_db);
	if (ret != SQLITE_OK)
	{
		ERROE_LOG("sqlite3_close\n");
	}
	else
	{
		TRACE_LOG("sqlite3_close success\n");
	}
}
//"insert into table_doc (path, doc_name,doc_name_pinyin,doc_name_initials) values ('C:\\Users\\Lenovo\\Desktop\\学习','IntelliJ IDEA Community Edition 2019.1.1 x64.lnk','IntelliJ IDEA Community Edition 2019."

void SqlManager::ExcutedSql(const string &sql)
{
	char *errmsg;
	int ret = sqlite3_exec(_db, sql.c_str(), nullptr, nullptr, &errmsg);
	if (ret != SQLITE_OK)
	{
		ERROE_LOG("sqlite3_exec(%s)\n", sql.c_str());
	}
	else
	{
		TRACE_LOG("sqlite3_exec success(%s)\n", sql.c_str());
	}
}

void SqlManager::GetTable(const string &sql, int& row, int& col, char**& pResult)
{
	char *errmsg;
	int ret = sqlite3_get_table(_db, sql.c_str(), &pResult, &row, &col, &errmsg);
	if (ret != SQLITE_OK)
	{
		ERROE_LOG("sqlite3_get_table(%s)\n", sql.c_str());
		sqlite3_free(errmsg);
	}
	else
	{
		TRACE_LOG("sqlite3_get_table success(%s)\n", sql.c_str());
	}
}

void DataManager::Init()
{
	_dbmgr.Open(DB_NAME);
	char sql[512];
	sprintf(sql, "create table if not exists %s (id integer primary key, path text, doc_name text,doc_name_pinyin text, doc_name_initials text)", TABLE_NAME);
	_dbmgr.ExcutedSql(sql);
}

void DataManager::GetData(const string& path, std::set<string>& Data)
{
	char sql[512];
	sprintf(sql, "select doc_name from %s where path = '%s'", TABLE_NAME, path.c_str());
	int row, col;
	char** pResult;
	AutoGetTable agtu(_dbmgr, sql, row, col, pResult);
	for (int i = 1; i < row; i++)
	{
		for (int j = 0; j < col; j++)
		{
			Data.insert(pResult[i*col + j]);
		}
	}
}

void DataManager::InsertData(const string& path, const string& Data)
{
	char sql[1024] = {0};
	string pinyin = ChineseConvertPinYinAllSpell(Data);
	string pinyin_init = ChineseConvertPinYinInitials(Data);
	sprintf(sql, "insert into %s (path, doc_name,doc_name_pinyin,doc_name_initials) values('%s','%s','%s','%s')", TABLE_NAME, path.c_str(), Data.c_str(), pinyin.c_str(), pinyin_init.c_str());
	_dbmgr.ExcutedSql(sql);
}

void DataManager::DeleteData(const string& path, const string& Data)
{
	char sql[512];
	sprintf(sql, "delete from %s where path = '%s' and doc_name = '%s'", TABLE_NAME, path.c_str(), Data.c_str());
	_dbmgr.ExcutedSql(sql);
	//没有删除干净，上面只删除目录中的子文档

	string path_ = path;
	path_ += "\\";
	path_ += Data;
	sprintf(sql, "delete from %s where path like '%s%%'", TABLE_NAME, path_.c_str());
	_dbmgr.ExcutedSql(sql);
}

void DataManager::Search(const string& key, vector<std::pair<string, string>>& doc_paths)
{
	char sql[512] = { '\0' };
	string pinyin = ChineseConvertPinYinAllSpell(key);
	string initials = ChineseConvertPinYinInitials(key);
	sprintf(sql, "select doc_name,path from %s where doc_name_pinyin like '%%%s%%' or doc_name_initials like '%%%s%%'", TABLE_NAME, pinyin.c_str(), initials.c_str());
	int row, col;
	char** ppret;
	AutoGetTable agtu(_dbmgr, sql, row, col, ppret);
	for (int i = 1; i <= row; i++)
	{
		doc_paths.push_back(std::make_pair(ppret[i*col + 0], ppret[i*col + 1]));
	}
}

void DataManager::SetHightLight(const string& str, const string& key, string& prefix, string& highlight, string& suffix)
{
	//1、直接是汉字部分直接高亮
	{
		size_t ht_start = str.find(key);
		if (ht_start != string::npos)
		{
			prefix = str.substr(0, ht_start);
			highlight = key;
			suffix = str.substr(ht_start + key.size(), string::npos);
			return;
		}
	}
	//2、直接是全拼的
	{
		string pinyin_str = ChineseConvertPinYinAllSpell(str);//nihaowolaile666
		string pinyin_key = ChineseConvertPinYinAllSpell(key);//wolaile
		size_t pinyin_start = pinyin_str.find(pinyin_key);
		if (pinyin_start != string::npos)
		{
			size_t ht_index = 0, ht_len = 0, ht_start = 0;//str中高亮的位置计数
			size_t pinyin_index = 0;//key在str中的位置计数
			size_t pinyin_end = pinyin_start + pinyin_key.size();
			while (pinyin_index < pinyin_end)
			{
				if (pinyin_index == pinyin_start)
				{
					ht_start = ht_index;
				}
				if (str[ht_index] >= 0 && str[ht_index] <= 127)
				{
					++ht_index;
					++pinyin_index;
				}
				else
				{
					char chinese[3] = { '\0' };
					chinese[0] = str[ht_index];
					chinese[1] = str[ht_index + 1];

					string _pinyin = ChineseConvertPinYinAllSpell(chinese);
					ht_index += 2;//高亮中文字符后+两个
					pinyin_index += _pinyin.size();//key全拼后+该汉字的拼音长度
				}
			}
			ht_len = ht_index - ht_start;

			prefix = str.substr(0, ht_start);
			highlight = str.substr(ht_start, ht_len);
			suffix = str.substr(ht_start + ht_len, string::npos);
		}
	}
	//key是汉字拼音首字母
	{
		string Initials_str = ChineseConvertPinYinInitials(str);
		string Initials_key = ChineseConvertPinYinInitials(key);
		size_t Initials_start = Initials_str.find(Initials_key);
		if (Initials_start != string::npos)
		{
			size_t ht_start = 0, ht_len = 0, ht_index = 0;
			size_t Initials_end = Initials_start + Initials_key.size();
			size_t Initials_index = 0;
			while (Initials_index < Initials_end)
			{
				if (Initials_index == Initials_start)
				{
					ht_start = ht_index;
				}
				if (str[ht_index] >= 0 && str[ht_index] <= 127)
				{
					++ht_index;
					++Initials_index;
				}
				else
				{
					ht_index += 2;
					++Initials_index;
				}
			}
			ht_len = ht_index - ht_start;

			prefix = str.substr(0, ht_start);
			highlight = str.substr(ht_start, ht_len);
			suffix = str.substr(ht_start + ht_len, string::npos);
		}
	}
}