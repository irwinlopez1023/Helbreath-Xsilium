#include <vector>
#include <windows.h>
using std::vector;
#pragma once

class CDeathmatch
{
public:
	CDeathmatch();
	~CDeathmatch();
	void enable();
	void disable();
	void notify_points(int client, int kills, int deaths);
	void kill_enemy(int att, int tar);
	void request_revive(int client);
	void revive(int client);
	void not_revive(int client);
	void add_kills(char * charname, int iadd);
	void add_deaths(char * charname, int iadd);
	void notify(int att, int tar);
	void End(int client);
	void WinnerReward(int client);
	void join_event(int client);
	void getranking(int client);
	void set_ek_per_kill(int value);
	int  get_ek_per_kill();
	void send_top10(int client);
	void add_fighter(char * charname);
	void remove_fighter(char * charname);
	void sort_fighters();
	bool is_fighter(char * charname);
	int get_kills(char * charname);
	int get_deaths(char * charname);
	
	struct st_fighters
	{
		st_fighters() : kills(0), deaths(0) { ZeroMemory(fighter, sizeof(fighter)); }
		char fighter[12];
		int kills;
		int deaths;
	};

	vector<st_fighters> vec_fighters;

	//heaton dm timer
	DWORD m_dwDeathmatchTimer;
};

