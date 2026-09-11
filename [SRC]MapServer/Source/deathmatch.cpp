#include "..\\Header\\deathmatch.h"
#include "..\\Header\\Game.h"
extern CMapServer * c_map;
#include "..\\Header\\Deathmatch.h"
#include "../../Shared/netmsg.h"
#include <iostream>
#include <algorithm>
#include <vector>

extern class CMapServer * c_map;
extern char G_cTxt[512];

//event config
static int maxkills = 100;
static int ekxkill = 3;
static const int kMaxEkPerKill = 1000;   // tope de /dmek
static int winreward = 5;

CDeathmatch::CDeathmatch()
{
	vec_fighters.clear();
}

CDeathmatch::~CDeathmatch()
{

}

void CDeathmatch::enable()
{
	PutLogList("Deathmatch ON");
	if (c_map->ActiveEvent) return;
	c_map->ActiveEvent = TRUE;
	DWORD dwTime = timeGetTime();
	c_map->dw_DmTime = dwTime - 100;
	vec_fighters.clear();
}

void CDeathmatch::disable()
{
	c_map->ActiveEvent = FALSE;
	PutLogList("Deathmatch OFF");
	vec_fighters.clear();
}


void CDeathmatch::join_event(int client)
{
	auto c = c_map;
	auto p = c->m_pClientList[client];
	if (!p) return;

	if (!g_ev.Is(EventID::Deathmatch)) return;
	if (p->IsInMap("dm")) return;

	if (!is_fighter(p->m_cCharName)) add_fighter(p->m_cCharName);

	notify_points(client, get_kills(p->m_cCharName), get_deaths(p->m_cCharName));
	c->RequestTeleportHandler(client, "2   ", "dm", -1, -1);
	p->m_bIsSafeAttackMode = FALSE;
	c->SendNotifyMsg(NULL, client, CLIENT_NOTIFY_SAFEATTACKMODE, NULL, NULL, NULL, NULL);
	sort_fighters();
	send_top10(client);
}

void CDeathmatch::set_ek_per_kill(int value)
{
	if (value < 0) value = 0;
	if (value > kMaxEkPerKill) value = kMaxEkPerKill;
	ekxkill = value;
}

int CDeathmatch::get_ek_per_kill()
{
	return ekxkill;
}

void CDeathmatch::getranking(int client)
{
	auto c = c_map;
	auto p = c->m_pClientList[client];
	if (!p) return;

	sort_fighters();
	send_top10(client);
}

void CDeathmatch::send_top10(int client)
{
	auto c = c_map;
	if (!g_ev.Is(EventID::Deathmatch)) return;
	auto p = c->m_pClientList[client];
	if (!p) return;

	char data[1024];
	auto * cp = data;

	Push(cp, (u32)NOT_DMTOP);
	Push(cp, (u16)0);

	short sendfighters = 0;
	if (vec_fighters.size() > 10) sendfighters = 10; else sendfighters = vec_fighters.size();

	Push(cp, sendfighters);

	for (int i = 0; i < sendfighters; i++)
	{
		Push(cp, vec_fighters[i].fighter, 12);
		Push(cp, vec_fighters[i].kills);
		Push(cp, vec_fighters[i].deaths);
	}

	c_map->m_pClientList[client]->m_pXSock->iSendMsg(data, cp - data);
}

void CDeathmatch::add_fighter(char * charname)
{
	auto c = c_map;
	if (!g_ev.Is(EventID::Deathmatch)) return;

	st_fighters u;

	ZeroMemory(u.fighter, sizeof(u.fighter));
	strcpy(u.fighter, charname);
	u.kills = 0;
	u.deaths = 0;

	vec_fighters.push_back(u);
}

bool CDeathmatch::is_fighter(char * charname)
{
	for (int i = 0; i < vec_fighters.size(); i++){
		if (strcmp(vec_fighters[i].fighter, charname) == 0) return true;
	}

	return false;
}

int CDeathmatch::get_kills(char * charname)
{
	for (int i = 0; i < vec_fighters.size(); i++){
		if (strcmp(vec_fighters[i].fighter, charname) == 0) return vec_fighters[i].kills;
	}

	return 0;
}

int CDeathmatch::get_deaths(char * charname)
{
	for (int i = 0; i < vec_fighters.size(); i++){
		if (strcmp(vec_fighters[i].fighter, charname) == 0) return vec_fighters[i].deaths;
	}

	return 0;
}

void CDeathmatch::remove_fighter(char * charname)
{
	for (int i = 0; i < (int)vec_fighters.size(); i++){
		if (strcmp(vec_fighters[i].fighter, charname) == 0)
		{
			vec_fighters.erase(vec_fighters.begin() + i);
			i--;   // sin esto se salta el elemento siguiente
		}
	}
}

void CDeathmatch::sort_fighters()
{
	auto sortFunc = [](st_fighters a, st_fighters b) -> bool {
		return a.kills > b.kills;
	};

	sort(vec_fighters.begin(), vec_fighters.end(), sortFunc);
}

void CDeathmatch::notify_points(int client, int kills, int deaths)
{
	auto c = c_map;
	auto p = c->m_pClientList[client];
	if (!p) return;

	char data[512];
	auto * cp = data;

	Push(cp, (u32)NOT_DMPOINTS);
	Push(cp, (u16)0);

	Push(cp, kills);
	Push(cp, deaths);

	c_map->m_pClientList[client]->m_pXSock->iSendMsg(data, cp - data);
}

void CDeathmatch::kill_enemy(int att, int tar)
{
	auto c = c_map;
	auto attacker = c->m_pClientList[att];
	auto target = c->m_pClientList[tar];
	if (!attacker || !target) return;

	// Solo se premia si el evento sigue activo y ambos estan inscritos.
	// Sin esto se seguian dando EK a quien se quedara en el mapa dm
	// despues de terminar el evento.
	bool bScored = g_ev.Is(EventID::Deathmatch)
		&& (tar != att)
		&& is_fighter(attacker->m_cCharName)
		&& is_fighter(target->m_cCharName);

	if (bScored)
	{
		attacker->m_iEnemyKillCount += ekxkill;
		c->SendCommand(attacker->client, "/eks", attacker->m_iEnemyKillCount);
		add_kills(attacker->m_cCharName, 1);
		add_deaths(target->m_cCharName, 1);
	}

	// Siempre, aunque no puntue: el llamador hace return justo despues
	// y el jugador se quedaria muerto en el mapa para siempre.
	request_revive(tar);
	if (bScored) notify(att, tar);
}

void CDeathmatch::request_revive(int client)
{
	auto c = c_map;
	auto p = c->m_pClientList[client];
	if (!p) return;

	p->m_bIsBeingResurrected = TRUE;
	p->requestrevive = true;
	c->SendCommand(client, "/revive");
}

void CDeathmatch::revive(int client)
{
	auto c = c_map;
	auto p = c->m_pClientList[client];
	if (!p) return;

	if (!p->m_bIsBeingResurrected) return;

	p->requestrevive = false;
	p->m_bIsKilled = FALSE;
	p->m_iHP = c->iGetMaxHP(client);
	p->m_iMP = (((c->m_pClientList[client]->m_iMag + c->m_pClientList[client]->m_iAngelicMag) * 2) + (c->m_pClientList[client]->m_iLevel / 2)) + (c->m_pClientList[client]->m_iInt + c->m_pClientList[client]->m_iAngelicInt) / 2;
	p->m_iSP = ((c->m_pClientList[client]->m_iStr + c->m_pClientList[client]->m_iAngelicStr) * 2) + (c->m_pClientList[client]->m_iLevel / 2);
	p->m_bIsBeingResurrected = FALSE;
	p->m_cMagicEffectStatus[DEF_MAGICTYPE_HOLDOBJECT] = 0;
	p->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] = 0;
	c->SendNotifyMsg(NULL, client, CLIENT_NOTIFY_HP, NULL, NULL, NULL, NULL);
	c->RequestTeleportHandler(client, "2   ", p->m_cMapName, -1, -1);
}

void CDeathmatch::not_revive(int client)
{
	auto c = c_map;
	auto p = c->m_pClientList[client];
	if (!p) return;

	if (!p->m_bIsBeingResurrected) return;

	p->requestrevive = false;
	p->m_bIsKilled = FALSE;
	p->m_iHP = c->iGetMaxHP(client);
	p->m_iMP = (((c->m_pClientList[client]->m_iMag + c->m_pClientList[client]->m_iAngelicMag) * 2) + (c->m_pClientList[client]->m_iLevel / 2)) + (c->m_pClientList[client]->m_iInt + c->m_pClientList[client]->m_iAngelicInt) / 2;
	p->m_iSP = ((c->m_pClientList[client]->m_iStr + c->m_pClientList[client]->m_iAngelicStr) * 2) + (c->m_pClientList[client]->m_iLevel / 2);
	p->m_bIsBeingResurrected = FALSE;
	p->m_cMagicEffectStatus[DEF_MAGICTYPE_HOLDOBJECT] = 0;
	p->m_cMagicEffectStatus[DEF_MAGICTYPE_ICE] = 0;
	c->SendNotifyMsg(NULL, client, CLIENT_NOTIFY_HP, NULL, NULL, NULL, NULL);
	// m_cSide (1=aresden, 2=elvine) es la ciudadania y no cambia al
	// entrar al mapa dm; m_cLocation si, por eso IsLocation fallaba.
	if (p->m_cSide == 2)
		c->RequestTeleportHandler(client, "2   ", "elvine", -1, -1);
	else
		c->RequestTeleportHandler(client, "2   ", "aresden", -1, -1);
}

void CDeathmatch::add_kills(char * charname, int iadd)
{
	for (int i = 0; i < vec_fighters.size(); i++){
		if (strcmp(vec_fighters[i].fighter, charname) == 0)
		{
			vec_fighters[i].kills += iadd;
			return;
		}
	}
}

void CDeathmatch::add_deaths(char * charname, int iadd)
{
	for (int i = 0; i < vec_fighters.size(); i++){
		if (strcmp(vec_fighters[i].fighter, charname) == 0)
		{
			vec_fighters[i].deaths += iadd;
			return;
		}
	}
}

void CDeathmatch::notify(int att, int tar)
{
	auto g = c_map;
	auto attacker = g->m_pClientList[att];
	auto target = g->m_pClientList[tar];
	if (!attacker || !target) return;

	attacker->Send(att, CLIENT_NOTIFY_ENEMYKILLREWARD, tar);
	auto attackerkills = get_kills(attacker->m_cCharName);
	notify_points(att, attackerkills, get_deaths(attacker->m_cCharName));
	notify_points(tar, get_kills(target->m_cCharName), get_deaths(target->m_cCharName));

	if (attackerkills >= maxkills)
	{
		WinnerReward(att);
		End(att);
	}
}

void CDeathmatch::End(int client)
{
	auto c = c_map;
	auto p = c->m_pClientList[client];
	if (!p) return;

	for (int i = 0; i < DEF_MAXCLIENTS; i++)
	{
		auto pi = c->m_pClientList[i];
		if (!pi) continue;
		if (pi == p) continue;
		wsprintf(G_cTxt, "%s is the Deathmatch Winner!", p->m_cCharName);
		c->ShowClientMsg(i, G_cTxt);
	}

	c->ShowClientMsg(client, "You won DeathMatch!");
	if (g_ev.Is(EventID::Deathmatch)) g_ev.Deactivate(EventID::Deathmatch);

}

void CDeathmatch::WinnerReward(int client)
{
	auto c = c_map;
	auto p = c->m_pClientList[client];
	if (!p) return;

	p->m_iCoins += winreward;
	c->SendCommand(client, "/coins", p->m_iCoins);
	wsprintf(G_cTxt, "You got %d coins", winreward);
	c->ShowClientMsg(client, G_cTxt);
}
