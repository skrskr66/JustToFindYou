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
	cout << "������Ҫ���ҵ��ļ���:";
	while (std::cin >> key)
	{
		document.clear();
		DataManager::GetInstance()->Search(key, document);
		printf("%-50s %-50s\n", "����", "·��");
		for (const auto& e : document)
		{
			string prefix, highlight, suffix;
			DataManager::GetInstance()->SetHightLight(e.first, key, prefix, highlight, suffix);
			cout << prefix;
			ColourPrintf(highlight.c_str());
			cout << suffix;
			for (size_t i = e.first.size(); i < 50; ++i)
			{
				//����ո񣬽��ж���
				cout << " ";
			}
			printf("%-50s\n",e.second.c_str());
		}
		cout << "===========================================================" << endl;
		cout << "������Ҫ���ҵ��ļ���:";
	}
}

void TestSetHightLight()
{
	string prefix, highlight, suffix;
	DataManager::GetInstance()->SetHightLight("�½��ı��ĵ�", "x", prefix, highlight, suffix);
	cout << prefix;
	ColourPrintf(highlight.c_str());
	cout << suffix;
}

int main()
{
	Search();
	return 0;
}