#pragma once
#include "Common.h"
#include "DataManager.h"
#include "ScanManager.h"

void Search()
{
	DataManager::GetInstance()->Init();
	ScanManager::CreateIntaince();
	vector<std::pair<string, string>> document;
	string key;
	cout << "===========================================================" << endl;
	cout << "请输入要查找的文件名:";
	while (std::cin >> key)
	{
		document.clear();
		DataManager::GetInstance()->Search(key, document);
		printf("%-50s %-50s\n", "名称", "路径");
		for (const auto& e : document)
		{
			string prefix, highlight, suffix;
			DataManager::GetInstance()->SetHightLight(e.first, key, prefix, highlight, suffix);
			cout << prefix;
			ColourPrintf(highlight.c_str());
			cout << suffix;
			for (size_t i = e.first.size(); i < 50; ++i)
			{
				//补齐空格，进行对齐
				cout << " ";
			}
			printf("%-50s\n",e.second.c_str());
		}
		cout << "===========================================================" << endl;
		cout << "请输入要查找的文件名:";
	}
}

void TestSetHightLight()
{
	string prefix, highlight, suffix;
	DataManager::GetInstance()->SetHightLight("新建文本文档", "x", prefix, highlight, suffix);
	cout << prefix;
	ColourPrintf(highlight.c_str());
	cout << suffix;
}

int main()
{
	Search();
	return 0;
}