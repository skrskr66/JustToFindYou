#pragma once
#include "Common.h"
#include "DataManager.h"

class ScanManager
{
public:
	void Scan(const string& path);

	void StartScan()
	{
		while (1)
		{
			Scan("F:\\vs projects\\JustToFindYou");
			std::this_thread::sleep_for(std::chrono::seconds(5));
		}
	}

	static ScanManager* CreateIntaince()
	{
		static std::thread thd(&ScanManager::StartScan, &scanmgr);
		thd.detach();
		return &scanmgr;
	}

private:

	ScanManager() = default;
	static ScanManager scanmgr;
	ScanManager(const ScanManager&);
	//DataManager _datamgr;
};