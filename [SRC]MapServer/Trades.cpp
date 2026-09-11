#include "Trades.h"
#include <tchar.h>
#include "Header//Game.h"

extern class CMapServer * c_map;
extern char G_cTxt[512];
extern void PutLogList(char * cMsg);

struct st_trade_ek
{
	st_trade_ek() {}
	short index;
	char itemname[21];
	int eks;
	short sprite;
	short spriteframe;
	short itemcolor;
};

vector<st_trade_ek> vec_trade_ek;

struct st_trade_contrib
{
	st_trade_contrib() {}
	short index;
	char itemname[21];
	int contrib;
	short sprite;
	short spriteframe;
	short itemcolor;
};

vector<st_trade_contrib> vec_trade_contrib;

struct st_trade_coin
{
	st_trade_coin() {}
	short index;
	char itemname[21];
	int coins;
	short sprite;
	short spriteframe;
	short itemcolor;
};

vector<st_trade_coin> vec_trade_coin;

struct st_trade_majs
{
	st_trade_majs() {}
	short index;
	char itemname[21];
	int majs;
	short sprite;
	short spriteframe;
	short itemcolor;
};

vector<st_trade_majs> vec_trade_majs;

Trades::Trades()
{
	get_trade_ek();
	get_trade_contrib();
	get_trade_coin();
	get_trade_majs();
}

Trades::~Trades()
{
}

static bool is_wanted_line(const string & line, string str)
{
	return (line.find(str) != string::npos);
}

static string get_line(string file, string value1)
{
	ifstream fin(file);

	string line;

	while (getline(fin, line))
	{
		if (is_wanted_line(line, value1))
			return line;
	}

	return "#";
}

static string getvalue(string val, short trade)
{
	string result;

	switch (trade)
	{
	case DEF_TRADEEK: result = get_line("Configs\\trades\\tradeek.cfg", val); break;
	case DEF_TRADECONTRIB: result = get_line("Configs\\trades\\tradecontrib.cfg", val); break;
	case DEF_TRADECOIN: result = get_line("Configs\\trades\\tradecoin.cfg", val); break;
	case DEF_TRADEMAJS: result = get_line("Configs\\trades\\trademajs.cfg", val); break;
	default:
		break;
	}

	if (string(result) == "#") return result;
	else result.erase(0, val.length());
	return result;
}

static void tokenize(string const &str, const char* delim,
	std::vector<string> &out)
{
	char *token = strtok(const_cast<char*>(str.c_str()), delim);
	while (token != nullptr)
	{
		out.push_back(string(token));
		token = strtok(nullptr, delim);
	}
}

void Trades::get_trade_ek()
{
	vec_trade_ek.clear();
	PutLogList("(!) Reading trade ek data...");
	for (int i = 0; i < MAXTRADEEKITEMS; i++)
	{
		wsprintf(G_cTxt, "trade-num-%d = ", i + 1);
		string token = getvalue(G_cTxt, DEF_TRADEEK);
		if (string(token) == "#") continue;

		const char* delim = " ";
		vector<string> out;
		tokenize(token, delim, out);
		string tempname;
		int tempval;
		short spr;
		short sprframe;
		short itemcolor;

		int count = 0;
		for (auto &token : out) {
			count++;
			switch (count)
			{
			case 1:
				tempname = token;
				break;

			case 2:
				tempval = atoi((char*)token.c_str());
				break;

			case 3:
				spr = atoi((char*)token.c_str());
				break;

			case 4:
				sprframe = atoi((char*)token.c_str());
				break;

			case 5:
				itemcolor = atoi((char*)token.c_str());
				break;

			default: break;
			}
		}

		st_trade_ek u;

		u.index = i + 1;
		ZeroMemory(u.itemname, sizeof(u.itemname));
		strcpy(u.itemname, (char*)tempname.c_str());
		u.eks = tempval;
		u.sprite = spr;
		u.spriteframe = sprframe;
		u.itemcolor = itemcolor;

		vec_trade_ek.push_back(u);
	}
}

void Trades::get_trade_contrib()
{
	vec_trade_contrib.clear();

	PutLogList("(!) Reading trade contrib data...");
	for (int i = 0; i < MAXTRADECONTRIBITEMS; i++)
	{
		wsprintf(G_cTxt, "trade-num-%d = ", i + 1);
		string token = getvalue(G_cTxt, DEF_TRADECONTRIB);
		if (string(token) == "#") continue;

		const char* delim = " ";
		vector<string> out;
		tokenize(token, delim, out);
		string tempname;
		int tempval;
		short spr;
		short sprframe;
		short itemcolor;

		int count = 0;
		for (auto &token : out) {
			count++;
			switch (count)
			{
			case 1:
				tempname = token;
				break;

			case 2:
				tempval = atoi((char*)token.c_str());
				break;
			case 3:
				spr = atoi((char*)token.c_str());
				break;

			case 4:
				sprframe = atoi((char*)token.c_str());
				break;

			case 5:
				itemcolor = atoi((char*)token.c_str());
				break;

			default: break;
			}
		}

		st_trade_contrib u;

		u.index = i + 1;
		ZeroMemory(u.itemname, sizeof(u.itemname));
		strcpy(u.itemname, (char*)tempname.c_str());
		u.contrib = tempval;
		u.sprite = spr;
		u.spriteframe = sprframe;
		u.itemcolor = itemcolor;

		vec_trade_contrib.push_back(u);
	}
}

void Trades::get_trade_coin()
{
	vec_trade_coin.clear();

	PutLogList("(!) Reading trade coin data...");
	for (int i = 0; i < MAXTRADECOINITEMS; i++)
	{
		wsprintf(G_cTxt, "trade-num-%d = ", i + 1);
		string token = getvalue(G_cTxt, DEF_TRADECOIN);
		if (string(token) == "#") continue;

		const char* delim = " ";
		vector<string> out;
		tokenize(token, delim, out);
		string tempname;
		int tempval;
		short spr;
		short sprframe;
		short itemcolor;

		int count = 0;
		for (auto &token : out) {
			count++;
			switch (count)
			{
			case 1:
				tempname = token;
				break;

			case 2:
				tempval = atoi((char*)token.c_str());
				break;
			case 3:
				spr = atoi((char*)token.c_str());
				break;

			case 4:
				sprframe = atoi((char*)token.c_str());
				break;

			case 5:
				itemcolor = atoi((char*)token.c_str());
				break;

			default: break;
			}
		}

		st_trade_coin u;

		u.index = i + 1;
		ZeroMemory(u.itemname, sizeof(u.itemname));
		strcpy(u.itemname, (char*)tempname.c_str());
		u.coins = tempval;
		u.sprite = spr;
		u.spriteframe = sprframe;
		u.itemcolor = itemcolor;

		vec_trade_coin.push_back(u);
	}
}



void Trades::get_trade_majs()
{
	vec_trade_majs.clear();

	PutLogList("(!) Reading trade majestics data...");
	for (int i = 0; i < MAXTRADECOINITEMS; i++)
	{
		wsprintf(G_cTxt, "trade-num-%d = ", i + 1);
		string token = getvalue(G_cTxt, DEF_TRADEMAJS);
		if (string(token) == "#") continue;

		const char* delim = " ";
		vector<string> out;
		tokenize(token, delim, out);
		string tempname;
		int tempval;
		short spr;
		short sprframe;
		short itemcolor;

		int count = 0;
		for (auto &token : out) {
			count++;
			switch (count)
			{
			case 1:
				tempname = token;
				break;

			case 2:
				tempval = atoi((char*)token.c_str());
				break;
			case 3:
				spr = atoi((char*)token.c_str());
				break;

			case 4:
				sprframe = atoi((char*)token.c_str());
				break;

			case 5:
				itemcolor = atoi((char*)token.c_str());
				break;

			default: break;
			}
		}

		st_trade_majs u;

		u.index = i + 1;
		ZeroMemory(u.itemname, sizeof(u.itemname));
		strcpy(u.itemname, (char*)tempname.c_str());
		u.majs = tempval;
		u.sprite = spr;
		u.spriteframe = sprframe;
		u.itemcolor = itemcolor;

		vec_trade_majs.push_back(u);
	}
}

void Trades::send_trade_data(int client, short trade)
{
	char data[10000];
	auto * cp = data;
	u32 msgid = -1;
	u16 nulled = 0;
	int iSize = 0;

	switch (trade)
	{
	case DEF_TRADEEK:
		msgid = NOT_TRADE_EK;
		iSize = vec_trade_ek.size();
		break;

	case DEF_TRADECONTRIB:
		msgid = NOT_TRADE_CONTRIB;
		iSize = vec_trade_contrib.size();
		break;

	case DEF_TRADECOIN:
		msgid = NOT_TRADE_COIN;
		iSize = vec_trade_coin.size();
		break;

	case DEF_TRADEMAJS:
		msgid = NOT_TRADE_MAJS;
		iSize = vec_trade_majs.size();
		break;

	default:
		break;
	}

	Push(cp, msgid);
	Push(cp, nulled);
	Push(cp, iSize);

	for (int i = 0; i < iSize; i++)
	{
		int val = -1;
		char itemname[21];
		ZeroMemory(itemname, sizeof(itemname));
		short spr = -1;
		short sprframe = -1;
		short itemcolor;

		switch (trade)
		{
		case DEF_TRADEEK:
			strcpy(itemname, vec_trade_ek[i].itemname);
			val = vec_trade_ek[i].eks;
			spr = vec_trade_ek[i].sprite;
			sprframe = vec_trade_ek[i].spriteframe;
			itemcolor = vec_trade_ek[i].itemcolor;
			break;

		case DEF_TRADECONTRIB:
			strcpy(itemname, vec_trade_contrib[i].itemname);
			val = vec_trade_contrib[i].contrib;
			spr = vec_trade_contrib[i].sprite;
			sprframe = vec_trade_contrib[i].spriteframe;
			itemcolor = vec_trade_contrib[i].itemcolor;
			break;

		case DEF_TRADECOIN:
			strcpy(itemname, vec_trade_coin[i].itemname);
			val = vec_trade_coin[i].coins;
			spr = vec_trade_coin[i].sprite;
			sprframe = vec_trade_coin[i].spriteframe;
			itemcolor = vec_trade_coin[i].itemcolor;
			break;

		case DEF_TRADEMAJS:
			strcpy(itemname, vec_trade_majs[i].itemname);
			val = vec_trade_majs[i].majs;
			spr = vec_trade_majs[i].sprite;
			sprframe = vec_trade_majs[i].spriteframe;
			itemcolor = vec_trade_majs[i].itemcolor;
			break;

		default:
			break;
		}

		Push(cp, itemname, 21);
		Push(cp, val);
		Push(cp, spr);
		Push(cp, sprframe);
		Push(cp, itemcolor);
	}

	c_map->m_pClientList[client]->m_pXSock->iSendMsg(data, cp - data);
}


static void deleteText(std::string& str, const std::string& toRemove) {
	size_t pos = str.find(toRemove);
	if (pos != std::string::npos) {
		str.erase(pos, toRemove.length());
	}
}

static bool startsWith(const std::string& str, const std::string& prefix) {
	return str.rfind(prefix, 0) == 0; // Verifica si `prefix` es el inicio de `str`
}


void Trades::req_trade(int client, char * pData)
{
	int index = -1;
	int quant = 1;
	short mode = -1;

	auto p = c_map->m_pClientList[client];
	if (!p) return;

	Pop(pData, mode);
	Pop(pData, index);
	Pop(pData, quant);

	switch (mode)
	{
	case DEF_TRADEEK:
	{
		if (index < 0 || (size_t)index >= vec_trade_ek.size()) return;
		if (quant < 1 || quant > 20) return;

		if (c_map->_iGetItemSpaceLeft(client) < quant)
		{
			c_map->SendItemNotifyMsg(client, CLIENT_NOTIFY_CANNOTCARRYMOREITEM, NULL, NULL);
			return;
		}

		auto icost = vec_trade_ek[index].eks * quant;

		if (p->m_iEnemyKillCount < icost || p->m_iEnemyKillCount < 0)
		{
			c_map->ShowClientMsg(client, "Not Enough Eks");
			return;
		}

		p->m_iEnemyKillCount -= icost;          // cobra ANTES de entregar
		int iGiven = 0;

		class CItem * pItem;
		int iItemID, iEraseReq;

		for (int i = 0; i < quant; i++)
		{
			pItem = new class CItem;
			if (!c_map->_bInitItemAttr(pItem, vec_trade_ek[index].itemname))
			{
				delete pItem;
				break;
			}

			if (!c_map->_bAddClientItemList(client, pItem, &iEraseReq))
			{
				delete pItem;
				break;
			}

			c_map->SendItemNotifyMsg(client, CLIENT_NOTIFY_ITEMOBTAINED, pItem, NULL);
			iGiven++;
		}

		if (iGiven < quant)          // devuelve lo que no pudo entregar
		{
			p->m_iEnemyKillCount += vec_trade_ek[index].eks * (quant - iGiven);
			c_map->ShowClientMsg(client, "No puedes cargar mas peso. Solo se cobro lo entregado.");
		}
		c_map->SendCommand(client, "/eks", p->m_iEnemyKillCount);

		break;
	}

	case DEF_TRADECONTRIB:
	{
		if (index < 0 || (size_t)index >= vec_trade_contrib.size()) return;
		if (quant < 1 || quant > 20) return;

		if (c_map->_iGetItemSpaceLeft(client) < quant)
		{
			c_map->SendItemNotifyMsg(client, CLIENT_NOTIFY_CANNOTCARRYMOREITEM, NULL, NULL);
			return;
		}

		auto icost = vec_trade_contrib[index].contrib * quant;

		if (p->m_iContribution < icost || p->m_iContribution < 0)
		{
			c_map->ShowClientMsg(client, "Not Enough Contribution");
			return;
		}

		p->m_iContribution -= icost;          // cobra ANTES de entregar
		int iGiven = 0;

		class CItem * pItem;
		int iItemID, iEraseReq;

		for (int i = 0; i < quant; i++)
		{
			pItem = new class CItem;
			if (!c_map->_bInitItemAttr(pItem, vec_trade_contrib[index].itemname))
			{
				delete pItem;
				break;
			}

			if (!c_map->_bAddClientItemList(client, pItem, &iEraseReq))
			{
				delete pItem;
				break;
			}

			c_map->SendItemNotifyMsg(client, CLIENT_NOTIFY_ITEMOBTAINED, pItem, NULL);
			iGiven++;
		}

		if (iGiven < quant)          // devuelve lo que no pudo entregar
		{
			p->m_iContribution += vec_trade_contrib[index].contrib * (quant - iGiven);
			c_map->ShowClientMsg(client, "No puedes cargar mas peso. Solo se cobro lo entregado.");
		}
		c_map->SendCommand(client, "/contrib", p->m_iContribution);
		break;
	}

	case DEF_TRADECOIN:
	{
		if (index < 0 || (size_t)index >= vec_trade_coin.size()) return;
		if (quant < 1 || quant > 20) return;

		string reward = vec_trade_coin[index].itemname;
		string contrib_text = "Contrib+";
		string maj_text = "Majestic+";
		string rep_text = "Rep+";
		
		if (startsWith(reward, contrib_text))
		{
			deleteText(reward, contrib_text);
			int contrib = atoi(reward.c_str());
			if (quant > 1) return;

			auto icost = vec_trade_coin[index].coins;

			if (p->m_iCoins < icost || p->m_iCoins < 0)
			{
				c_map->ShowClientMsg(client, "Not Enough Coins");
				return;
			}

			p->m_iCoins -= icost;
			c_map->SendCommand(client, "/coins", p->m_iCoins);

			if (contrib > 0) {
				wsprintf(G_cTxt, "You get %d contribution from trade coin.", contrib);
				c_map->ShowClientMsg(client, G_cTxt);
				p->m_iContribution += contrib;
				c_map->SendCommand(client, "/contrib", p->m_iContribution);
			}
		}
		else if (startsWith(reward, maj_text))
		{
			deleteText(reward, maj_text);
			int majs = atoi(reward.c_str());
			if (quant > 1) return;
			auto icost = vec_trade_coin[index].coins;

			if (p->m_iCoins < icost || p->m_iCoins < 0)
			{
				c_map->ShowClientMsg(client, "Not Enough Coins");
				return;
			}

			p->m_iCoins -= icost;
			c_map->SendCommand(client, "/coins", p->m_iCoins);

			if (majs > 0) {
				wsprintf(G_cTxt, "You get %d majestics from trade coin.", majs);
				c_map->ShowClientMsg(client, G_cTxt);
				p->m_iGizonItemUpgradeLeft += majs;
				c_map->SendCommand(client, "/majs", p->m_iGizonItemUpgradeLeft);
			}
		}
		else if (startsWith(reward, rep_text))
		{
			deleteText(reward, rep_text);
			int rep = atoi(reward.c_str());
			if (quant > 1) return;
			auto icost = vec_trade_coin[index].coins;

			if (p->m_iCoins < icost || p->m_iCoins < 0)
			{
				c_map->ShowClientMsg(client, "Not Enough Coins");
				return;
			}

			p->m_iCoins -= icost;
			c_map->SendCommand(client, "/coins", p->m_iCoins);

			if (rep > 0) {
				wsprintf(G_cTxt, "You get %d reputation from trade coin.", rep);
				c_map->ShowClientMsg(client, G_cTxt);
				p->m_iRating += rep;
				c_map->SendCommand(client, "/rep", p->m_iRating);
			}
		}
		else
		{
			if (c_map->_iGetItemSpaceLeft(client) < quant)
			{
				c_map->SendItemNotifyMsg(client, CLIENT_NOTIFY_CANNOTCARRYMOREITEM, NULL, NULL);
				return;
			}

			auto icost = vec_trade_coin[index].coins * quant;

			if (p->m_iCoins < icost || p->m_iCoins < 0)
			{
				c_map->ShowClientMsg(client, "Not Enough Coins");
				return;
			}

			p->m_iCoins -= icost;          // cobra ANTES de entregar
			int iGiven = 0;

			class CItem * pItem;
			int iItemID, iEraseReq;

			for (int i = 0; i < quant; i++)
			{
				pItem = new class CItem;
				if (!c_map->_bInitItemAttr(pItem, vec_trade_coin[index].itemname))
				{
					delete pItem;
					break;
				}

				if (!c_map->_bAddClientItemList(client, pItem, &iEraseReq))
				{
					delete pItem;
					break;
				}

				c_map->SendItemNotifyMsg(client, CLIENT_NOTIFY_ITEMOBTAINED, pItem, NULL);
				iGiven++;
			}

			if (iGiven < quant)          // devuelve lo que no pudo entregar
			{
				p->m_iCoins += vec_trade_coin[index].coins * (quant - iGiven);
				c_map->ShowClientMsg(client, "No puedes cargar mas peso. Solo se cobro lo entregado.");
			}
			c_map->SendCommand(client, "/coins", p->m_iCoins);
		}
				
		break;
	}

	case DEF_TRADEMAJS:
	{
		if (index < 0 || (size_t)index >= vec_trade_majs.size()) return;
		if (quant < 1 || quant > 20) return;

		if (c_map->_iGetItemSpaceLeft(client) < quant)
		{
			c_map->SendItemNotifyMsg(client, CLIENT_NOTIFY_CANNOTCARRYMOREITEM, NULL, NULL);
			return;
		}

		auto icost = vec_trade_majs[index].majs * quant;

		if (p->m_iGizonItemUpgradeLeft < icost || p->m_iGizonItemUpgradeLeft < 0)
		{
			c_map->ShowClientMsg(client, "Not Enough Majestics");
			return;
		}

		p->m_iGizonItemUpgradeLeft -= icost;          // cobra ANTES de entregar
		int iGiven = 0;

		class CItem * pItem;
		int iItemID, iEraseReq;

		for (int i = 0; i < quant; i++)
		{
			pItem = new class CItem;
			if (!c_map->_bInitItemAttr(pItem, vec_trade_majs[index].itemname))
			{
				delete pItem;
				break;
			}

			if (!c_map->_bAddClientItemList(client, pItem, &iEraseReq))
			{
				delete pItem;
				break;
			}

			if (string(pItem->m_cName) == "AngelicPandent(STR)" ||
				string(pItem->m_cName) == "AngelicPandent(DEX)" ||
				string(pItem->m_cName) == "AngelicPandent(INT)" ||
				string(pItem->m_cName) == "AngelicPandent(MAG)")
			{
				pItem->m_sTouchEffectType = DEF_ITET_UNIQUE_OWNER;
				pItem->m_sTouchEffectValue1 = p->m_sCharIDnum1;
				pItem->m_sTouchEffectValue2 = p->m_sCharIDnum2;
				pItem->m_sTouchEffectValue3 = p->m_sCharIDnum3;
			}
			
			c_map->SendItemNotifyMsg(client, CLIENT_NOTIFY_ITEMOBTAINED, pItem, NULL);
			iGiven++;
		}

		if (iGiven < quant)          // devuelve lo que no pudo entregar
		{
			p->m_iGizonItemUpgradeLeft += vec_trade_majs[index].majs * (quant - iGiven);
			c_map->ShowClientMsg(client, "No puedes cargar mas peso. Solo se cobro lo entregado.");
		}
		c_map->SendCommand(client, "/majs", p->m_iGizonItemUpgradeLeft);
		break;
	}

	default: return; break;
	}
}

void Trades::req_tradeball(int client, char * data)
{
	auto p = c_map->m_pClientList[client];
	if (!p) return;

	short ballmode;
	int quant;

	Pop(data, ballmode);
	Pop(data, quant);

	switch (ballmode)
	{
		case BALL_CONTRIB:
		{
			getContributionBall(client, quant);
			break;
		}

		case BALL_COIN:
		{
			getCoinBall(client, quant);
			break;
		}
	
		case BALL_REP:
		{
			getReputationBall(client, quant);
			break;
		}

		case BALL_MAJS:
		{
			getMajesticBall(client, quant);
			break;
		}

	default: return; break;
	}
}

void Trades::getContributionBall(int client, int quant)
{
	auto p = c_map->m_pClientList[client];
	if (!p) return;

	if (p->m_iContribution <= 0)
	{
		c_map->ShowClientMsg(client, "No tienes suficiente contribución.");
		return;
	}

	if (quant < 1)
	{
		c_map->ShowClientMsg(client, "La cantidad debe ser al menos 1.");
		return;
	}

	int totalCost = quant; 
	if (p->m_iContribution < totalCost)
	{
		c_map->ShowClientMsg(client, "No tienes suficiente contribución para esta cantidad.");
		return;
	}

	if (c_map->_iGetItemSpaceLeft(client) < 1)
	{
		c_map->SendItemNotifyMsg(client, CLIENT_NOTIFY_CANNOTCARRYMOREITEM, NULL, NULL);
		return;
	}

	p->m_iContribution -= totalCost;
	c_map->SendCommand(client, "/contrib", p->m_iContribution);

	char itemname[21];
	wsprintf(itemname, "ContribBall");

	class CItem* pItem;
	int iEraseReq;

	pItem = new class CItem;
	if (!c_map->_bInitItemAttr(pItem, itemname))
	{
		p->m_iContribution += totalCost;      // devuelve lo cobrado
		c_map->SendCommand(client, "/contrib", p->m_iContribution);
		delete pItem;
		return;
	}

	pItem->m_dwCount = quant; 

	if (!c_map->_bAddClientItemList(client, pItem, &iEraseReq))
	{
		p->m_iContribution += totalCost;      // devuelve lo cobrado
		c_map->SendCommand(client, "/contrib", p->m_iContribution);
		delete pItem;
		return;
	}

	c_map->SendItemNotifyMsg(client, CLIENT_NOTIFY_ITEMOBTAINED, pItem, NULL); 	
}

void Trades::getCoinBall(int client, int quant)
{
	auto p = c_map->m_pClientList[client];
	if (!p) return;

	if (p->m_iCoins <= 0)
	{
		c_map->ShowClientMsg(client, "No tienes suficientes coins.");
		return;
	}

	if (quant < 1)
	{
		c_map->ShowClientMsg(client, "La cantidad debe ser al menos 1.");
		return;
	}

	int totalCost = quant;
	if (p->m_iCoins < totalCost)
	{
		c_map->ShowClientMsg(client, "No tienes suficientes coins para esta cantidad.");
		return;
	}

	if (c_map->_iGetItemSpaceLeft(client) < 1)
	{
		c_map->SendItemNotifyMsg(client, CLIENT_NOTIFY_CANNOTCARRYMOREITEM, NULL, NULL);
		return;
	}

	p->m_iCoins -= totalCost;
	c_map->SendCommand(client, "/coins", p->m_iCoins);

	char itemname[21];
	wsprintf(itemname, "CoinBall");

	class CItem* pItem;
	int iEraseReq;

	pItem = new class CItem;
	if (!c_map->_bInitItemAttr(pItem, itemname))
	{
		p->m_iCoins += totalCost;      // devuelve lo cobrado
		c_map->SendCommand(client, "/coins", p->m_iCoins);
		delete pItem;
		return;
	}

	pItem->m_dwCount = quant;

	if (!c_map->_bAddClientItemList(client, pItem, &iEraseReq))
	{
		p->m_iCoins += totalCost;      // devuelve lo cobrado
		c_map->SendCommand(client, "/coins", p->m_iCoins);
		delete pItem;
		return;
	}

	c_map->SendItemNotifyMsg(client, CLIENT_NOTIFY_ITEMOBTAINED, pItem, NULL);
}


void Trades::getReputationBall(int client, int quant)
{
	auto p = c_map->m_pClientList[client];
	if (!p) return;

	if (p->m_iRating <= 0)
	{
		c_map->ShowClientMsg(client, "No tienes suficiente reputacion.");
		return;
	}

	if (quant < 1)
	{
		c_map->ShowClientMsg(client, "La cantidad debe ser al menos 1.");
		return;
	}

	int totalCost = quant;
	if (p->m_iRating < totalCost)
	{
		c_map->ShowClientMsg(client, "No tienes suficiente reputacion para esta cantidad.");
		return;
	}

	if (c_map->_iGetItemSpaceLeft(client) < 1)
	{
		c_map->SendItemNotifyMsg(client, CLIENT_NOTIFY_CANNOTCARRYMOREITEM, NULL, NULL);
		return;
	}

	p->m_iRating -= totalCost;
	c_map->SendCommand(client, "/rep", p->m_iRating);

	char itemname[21];
	wsprintf(itemname, "RepBall");

	class CItem* pItem;
	int iEraseReq;

	pItem = new class CItem;
	if (!c_map->_bInitItemAttr(pItem, itemname))
	{
		p->m_iRating += totalCost;      // devuelve lo cobrado
		c_map->SendCommand(client, "/rep", p->m_iRating);
		delete pItem;
		return;
	}

	pItem->m_dwCount = quant;

	if (!c_map->_bAddClientItemList(client, pItem, &iEraseReq))
	{
		p->m_iRating += totalCost;      // devuelve lo cobrado
		c_map->SendCommand(client, "/rep", p->m_iRating);
		delete pItem;
		return;
	}

	c_map->SendItemNotifyMsg(client, CLIENT_NOTIFY_ITEMOBTAINED, pItem, NULL);
}

void Trades::getMajesticBall(int client, int quant)
{
	auto p = c_map->m_pClientList[client];
	if (!p) return;

	if (p->m_iGizonItemUpgradeLeft <= 0)
	{
		c_map->ShowClientMsg(client, "No tienes suficientes majestics.");
		return;
	}

	if (quant < 1)
	{
		c_map->ShowClientMsg(client, "La cantidad debe ser al menos 1.");
		return;
	}

	int totalCost = quant;
	if (p->m_iGizonItemUpgradeLeft < totalCost)
	{
		c_map->ShowClientMsg(client, "No tienes suficientes majestics para esta cantidad.");
		return;
	}

	if (c_map->_iGetItemSpaceLeft(client) < 1)
	{
		c_map->SendItemNotifyMsg(client, CLIENT_NOTIFY_CANNOTCARRYMOREITEM, NULL, NULL);
		return;
	}

	p->m_iGizonItemUpgradeLeft -= totalCost;
	c_map->SendCommand(client, "/majs", p->m_iGizonItemUpgradeLeft);

	char itemname[21];
	wsprintf(itemname, "MajBall");

	class CItem* pItem;
	int iEraseReq;

	pItem = new class CItem;
	if (!c_map->_bInitItemAttr(pItem, itemname))
	{
		p->m_iGizonItemUpgradeLeft += totalCost;      // devuelve lo cobrado
		c_map->SendCommand(client, "/majs", p->m_iGizonItemUpgradeLeft);
		delete pItem;
		return;
	}

	pItem->m_dwCount = quant;

	if (!c_map->_bAddClientItemList(client, pItem, &iEraseReq))
	{
		p->m_iGizonItemUpgradeLeft += totalCost;      // devuelve lo cobrado
		c_map->SendCommand(client, "/majs", p->m_iGizonItemUpgradeLeft);
		delete pItem;
		return;
	}

	c_map->SendItemNotifyMsg(client, CLIENT_NOTIFY_ITEMOBTAINED, pItem, NULL);
}
