//rank system hbarg
#pragma once
using namespace std;

#include <windows.h>
#include <iostream>

#define DEF_MAXRANKLEVEL 30                 // tope de rango
#define MAXRANKEXP (DEF_MAXRANKLEVEL + 1)   // la tabla necesita indices 0..30

// Tickets de rango: hasta que rango sube cada uno (el tercero usa DEF_MAXRANKLEVEL)
#define DEF_RANKTICKET_TOPE1 10
#define DEF_RANKTICKET_TOPE2 20

// En areuni y VipMap1 el rango deja de subir al llegar aqui.
// Hoy es igual al tope (30). Si el 20 original era deliberado, cambialo a 20.
#define DEF_RANKLIMIT_MAPASFARM DEF_MAXRANKLEVEL

class CRank
{
public:
	CRank();
	~CRank();

	void clear();
	void init();
	void update();
	void read();
	bool is_wanted_line(const string & line, string str);
	string get_line(string file, string value1);
	string getvalue(string val);
	int m_iMaxrankexp[MAXRANKEXP];
};

