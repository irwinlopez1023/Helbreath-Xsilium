#include "Header\\Game.h"
#include <random>
extern char G_cTxt[512];

int ITEMSPREAD_FIEXD_COORD[25][2] = { { 0, 0 }, { 1, 0 }, { 1, 1 }, { 0, 1 }, { -1, 1 }, { -1, 0 }, { -1, -1 }, { 0, -1 }, { 1, -1 }, { 2, -1 }, { 2, 0 },
{ 2, 1 }, { 2, 2 }, { 1, 2 }, { 0, 2 }, { -1, 2 }, { -2, 2 }, { -2, 1 }, { -2, 0 }, { -2, -1 }, { -2, -2 }, { -1, -2 }, { 0, -2 }, { 1, -2 }, { 2, -2 } };

#define COIN_SMALL 99907
#define CONTRIB_SMALL 99901
#define MAJ_SMALL 99902
#define REP_SMALL 99903

#define COIN_MEDIUM 99908
#define CONTRIB_MEDIUM 99904
#define MAJ_MEDIUM 99905
#define REP_MEDIUM 99906


#define MAJ_BIG 99912
#define CONTRIB_BIG 99910
#define COIN_BIG 99909
#define REP_BIG 99911

#define CUSTOM_COIN 99913



// Tabla de multiplicadores segun el README. Cada peso es el porcentaje x 1.000.000
// y el conjunto suma exactamente kProbDenominator, asi que ninguna tirada se cae al final.
static const int kProbDenominator = 100000000;   // 100.000.000 = 100,000000 %

// rand() solo tiene 32.768 resultados posibles: no puede expresar 1 entre 16,7 millones,
// que es lo que pide el tramo mas raro. Generador propio solo para los drops.
static std::mt19937 g_dropRng((unsigned)time(NULL));

static int iRollDropTable()
{
	std::uniform_int_distribution<int> dist(1, kProbDenominator);
	return dist(g_dropRng);   // 1 .. 100.000.000, sin sesgo de modulo
}

static const int kNumMultipliers = 15;
static const int kMultiplierProbabilities[15] = {
	42165004, // x1    3% MR /   7% PA-MA   README 40,000000 % + residuo
	25000000, // x2    6% /  14%                  25,000000 %
	15000000, // x3    9% /  21%                  15,000000 %
	 9000000, // x4   12% /  28%                   9,000000 %
	 8745000, // x5   15% /  35%                   8,745000 %
	   83000, // x6   18% /  42%                   0,083000 %
	    3000, // x7   21% /  49%                   0,003000 %
	    2000, // x8   24% /  56%                   0,002000 %
	    1000, // x9   27% /  63%                   0,001000 %
	     500, // x10  30% /  70%                   0,000500 %
	     250, // x11  33% /  77%                   0,000250 %
	     150, // x12  36% /  84%                   0,000150 %
	      60, // x13  39% /  91%                   0,000060 %
	      30, // x14  42% /  98%                   0,000030 %
	       6  // x15  45% / 105%                   0,000006 %
};



int CMapServer::RollByProbabilityTable(const int* chances, int size) {
	int roll = iRollDropTable();      // 1 .. 100.000.000
	int acc = 0;
	for (int i = 0; i < size; ++i) {
		acc += chances[i];
		if (roll <= acc) return i + 1;   // la tabla ya es la probabilidad final
	}
	return 1;   // inalcanzable: los pesos suman el denominador exacto
}




static float g_fDropDifficulty = 1.0f;
inline void SetDropDifficulty(float f) { g_fDropDifficulty = f; }

//----------------------------------------------------------------------------- 
// Modificador de pesos global (solo para multiplicadores)
//----------------------------------------------------------------------------- 
// 1.0 = pesos originales; <1.0 reduce probabilidades de multiplicadores; >1.0 las aumenta

//Actualmente en desarrollo, por ahora no funciona. ¿Será que se habilita esta función? mmm
static float g_fWeightMultiplier = 1.0f;
inline void SetWeightMultiplier(float f) { g_fWeightMultiplier = f; }



void CMapServer::NpcDeadItemGenerator(int iNpcH, short sAttackerH, char cAttackerType)
{
#ifdef DEF_DEBUG
	try {
#endif
		class CItem * pItem;
		char  cColor, cItemName[21];
		BOOL  bIsGold;
		int   iGenLevel, iResult, iItemID, iItemprobability, iGoldProbMax, iSecondProb, iGoldProb2;
		DWORD dwType = 0, dwValue = 0;
		double dTmp1, dTmp2, dTmp3;
		BOOL SaveDrop = TRUE;

		if (m_pNpcList[iNpcH] == NULL) return;
		if ((cAttackerType != DEF_OWNERTYPE_PLAYER) || (m_pNpcList[iNpcH]->m_bIsSummoned == TRUE)) return;

		ZeroMemory(cItemName, sizeof(cItemName));
		bIsGold = FALSE;



		switch (m_pNpcList[iNpcH]->m_sType) {
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 21: // Guard
		case 34: // Dummy
		case 40: // CRUSADE
		case 64: // Crop
		case 87: // CT-elv y ares
		case 89: // AGC-elv y ares
		case 91: // Gates
		case 125: //NewGuard
		case 126: //NewGuard
		case 127: //NewGuard
			return;
		}

		// 6500 default; the lower the greater the Weapon/Armor/Wand Drop

		//Ver drop LaloRamos lalodrop
		iSecondProb = 2500; //5000// Fast 1000

		iItemprobability = 1500; //7500 // Fast 1500
		iGoldProbMax = 25;

		iGoldProb2 = 2500; //3500

		// 6500 default; the lower the greater the Weapon/Armor/Wand Drop
		if (iDice(1, 11000) >= iItemprobability) {
			// 35% Drop 60% of that is gold
			// 35% Chance of drop (35/100)
			SaveDrop = FALSE;

			if (iDice(1, 11000) <= iGoldProb2) {
				strcpy_secure(cItemName, "Gold");
				pItem = new class CItem;
				if (_bInitItemAttr(pItem, cItemName) == FALSE) {
					delete pItem;
					return;
				}
				pItem->m_dwCount = (DWORD)(iDice(1, (m_pNpcList[iNpcH]->m_iGoldDiceMax - m_pNpcList[iNpcH]->m_iGoldDiceMin)) + m_pNpcList[iNpcH]->m_iGoldDiceMin);
				if ((cAttackerType == DEF_OWNERTYPE_PLAYER) && (m_pClientList[sAttackerH]->m_iAddGold != NULL)) {
					dTmp1 = (double)m_pClientList[sAttackerH]->m_iAddGold;
					dTmp2 = (double)pItem->m_dwCount;
					dTmp3 = (dTmp1 / 100.0f)*dTmp2;
					pItem->m_dwCount += (int)dTmp3;
				}
			}
			else {
				// 9000 default; the lower the greater the Weapon/Armor/Wand Drop
				// 35% Drop 40% of that is an Item 
				if (iDice(1, 10000) <= iSecondProb) {
					// 40% Drop 90% of that is a standard drop
					// Standard Drop Calculation: (35/100) * (40/100) * (90/100) = 12.6%
					SaveDrop = FALSE;




					iResult = iDice(1, 13000);
					if ((iResult >= 1) && (iResult <= 3000))          dwValue = 1;
					else if ((iResult >= 3001) && (iResult <= 4000))  dwValue = 2;
					else if ((iResult >= 4001) && (iResult <= 5500))  dwValue = 3;
					else if ((iResult >= 5501) && (iResult <= 6700))  dwValue = 4;
					else if ((iResult >= 6701) && (iResult <= 8500))  dwValue = 5;
					else if ((iResult >= 8501) && (iResult <= 8999))  dwValue = 6;
					else if ((iResult >= 9000) && (iResult <= 10500))  dwValue = 7;
					else if ((iResult >= 10501) && (iResult <= 11200)) dwValue = 8;
					else if ((iResult >= 11201) && (iResult <= 13000)) dwValue = 9;

					switch (dwValue) {
					case 1: iItemID = 95; break; // Green Potion
					case 2: iItemID = 91; break; // Red Potion
					case 3: iItemID = 93; break; // Blue Potion
					case 4: iItemID = 390; break; // power green
					case 5: iItemID = 92; break; // Big Red Potion
					case 6: iItemID = 94; break; // Big Blue Potion
					case 7:
						switch (iDice(1, 2)) {
						case 1: iItemID = 390; break; // Power Green Potion
						case 2: iItemID = 390;  break; // Power Green Potion 
						}
						break;
					case 8:
						switch (iDice(1, 6)) {
						case 1: iItemID = 780; break; // Red Candy
						case 2: iItemID = 781; break; // Blue Candy
						case 3: iItemID = CONTRIB_SMALL; break; // ContribBall(+700)
							//case 1: iItemID = 3016; break; // ContribBall(+700)
							//case 2: iItemID = 3016; break; // ContribBall(+700)
						case 4:// 10%
							switch (iDice(1, 3)) {
							case 1: iItemID = 780; break; // Red Candy
							case 2: iItemID = 781; break; // Blue Candy
							case 3: iItemID = 782; break; // Green Candy
							}
							break;
							//	case 4: iItemID = 3016; break; // ContribBall(+700)
							//	case 4: iItemID = 650; break; // Zemstone of Sacrifice
						case 5: iItemID = CONTRIB_SMALL; break; // ContribBall(+700)
						case 6:// 10%
							switch (iDice(1, 3)) {
							case 1: iItemID = 656; break; // Xelima Stone
							case 2: iItemID = 657; break; // Merien Stone
							case 3: iItemID = 650; break; // Zemstone of Sacrifice
							}
							break;
						case 7: iItemID = 650; break; // Zemstone of Sacrifice
						}
						break;
					case 9:
						SYSTEMTIME SysTime;
						GetLocalTime(&SysTime);
						if (((short)SysTime.wMonth == 12) && (m_pNpcList[iNpcH]->m_sType == 61 || 55)) {
							switch (iDice(1, 4)) {
							case 1: iItemID = 780; break; // Red Candy
							case 2: iItemID = 781; break; // Blue Candy
							case 3: iItemID = 782; break; // Green Candy
							case 4: iItemID = CONTRIB_SMALL;  break; // ContribBall(+700)
								//	case 4:	iItemID = 650; break; // Zemstone of Sacrifice
								//case 4: iItemID = 3016;  break; // ContribBall(+700)
							}
						}
						break;
					}
					// If a non-existing item is created then delete the item
					pItem = new class CItem;
					if (_bInitItemAttr(pItem, iItemID) == FALSE) {
						delete pItem;
						return;
					}
				}
				else {
					// Valuable Drop Calculation: (35/100) * (40/100) * (10/100) = 1.4%
					// Define iGenLevel using Npc.cfg#
					switch (m_pNpcList[iNpcH]->m_sType) {

					case 10: // Slime
					case 16: // Giant-Ant
					case 22: // Amphis
					case 55: // Rabbit
					case 56: //	Cat
						iGenLevel = 1;
						break;

					case 11: // Skeleton
					case 14: // Orc, Orc-Mage
					case 17: // Scorpion
					case 18: // Zombie
						iGenLevel = 2;
						break;

					case 12: // Stone-Golem
					case 23: // Clay-Golem
						iGenLevel = 3;
						break;

					case 27: // Hellbound
					case 61: // Rudolph
					case 74: // Giant-Crayfish
						iGenLevel = 4;
						break;

					case 13: // Cyclops
					case 28: // Troll
					case 53: // Beholder
					case 60: // Cannibal-Plant
					case 62: // DireBoar
						iGenLevel = 5;
						break;

					case 29: // Orge
					case 33: // WereWolf
					case 48: // Stalker
					case 54: // Dark-Elf
					case 65: // Ice-Golem 
					case 70: // Barlog
					case 72: // Claw-Turtle
					case 78: // Minotaur
						iGenLevel = 6;
						break;

					case 30: // Liche
					case 63: // Frost
					case 79: // Nizzie
						iGenLevel = 7;
						break;

					case 31: // Demon
					case 32: // Unicorn
					case 49: // Hellclaw
					case 50: // Tigerworm
					case 52: // Gagoyle
					case 71: // Centaurus
						iGenLevel = 8;
						break;

					case 58: // MountainGiant
						iGenLevel = 9;
						break;

					case 59: // Ettin
					case 77: // MasterMage-Orc
					case 75: // Giant-Lizard
						iGenLevel = 10;
						break;

					case 93: // Ice-Worm
					case 94: // Fire-Worm
					case 100: // Snoob
					case 101: // Fire-Snoob
						iGenLevel = 11;
						break;

					case 95: // Blue-Demon
					case 96: // Red-Demon
						iGenLevel = 12;
						break;

					case 97: // Wood-Golem
					case 105: // Phamton
					case 99: // Butcher.
					case 122: //Goblin
					case 123:
					case 124:
						iGenLevel = 13;
						break;

					case 104: // Diablo
					case 102: // Satan
					case 103: // Giant-Spider
						iGenLevel = 14;
						break;

					case 98: // Ice-Lizard
					case 106: // Yeti
						iGenLevel = 15;
						break;

					case 107: // GrandEttin
						iGenLevel = 16;
						break;

					case 108: // HellDeath
					case 109: // LightWorm
					case 121: //GhostAba
						iGenLevel = 17;
						break;

					case 112: //Hellhound
					case 113: //Helliche
						iGenLevel = 18;
						break;

					case 128: //BlackWyvern
					case 114: //DeathWyvern
						iGenLevel = 19;
						break;

					case 115: //Hellbutcher
					case 116: //HellTrollde
						iGenLevel = 20;
						break;

					case 117: //MountDeath
						iGenLevel = 21;
						break;

					case 118: //Hellgoyle
					case 119: //HellDemon
						iGenLevel = 22;
						break;
					}

					if (iGenLevel == 0) return;

					// Weapon Drop: 
					// 1.4% chance Valuable Drop 60% that it is a Weapon
					if (iDice(1, 10000) <= 4000) {//6000
						if (iDice(1, 10000) <= 4000) {//5000 
							// 70% the Weapon is Melee
							switch (iGenLevel) {

							case 1: // Slime, Giant-Ant, Amphis, Rabbit, Cat
								switch (iDice(1, 3)) {
								case 1: iItemID = 1;  break; // Dagger
								case 2: iItemID = 8;  break; // ShortSword
								case 3: iItemID = 59; break; // LightAxe
								}
								break;

							case 2: // Skeleton, Orc, Orc-Mage, Scorpion, Zombie
								switch (iDice(1, 6)) {
								case 1: iItemID = 12;  break; // MainGauche
								case 2: iItemID = 15;  break; // Gradius
								case 3: iItemID = 65;  break; // SexonAxe
								case 4: iItemID = 62;  break; // Tomahoc
								case 5: iItemID = 23;  break; // Sabre
								case 6: iItemID = 31;  break; // Esterk
								}
								break;

							case 3: // Stone-Golem, Clay-Golem
								switch (iDice(1, 4)) {
								case 1: iItemID = 17;  break; // LongSword
								case 2: iItemID = 68;  break; // DoubleAxe
								case 3: iItemID = 23;  break; // Sabre
								case 4: iItemID = 31;  break; // Esterk
								}
								break;

							case 4: // Hellbound, Rudolph
								switch (iDice(1, 5)) {
								case 1: iItemID = 23;  break; // Sabre
								case 2: iItemID = 25;  break; // Scimitar
								case 3: iItemID = 28;  break; // Falchion
								case 4: iItemID = 31;  break; // Esterk
								case 5: iItemID = 34;  break; // Rapier
								}
								break;

							case 5: // Cyclops, Troll, Beholder, Cannibal-Plant, DireBoar
								switch (iDice(1, 3)) {
								case 1: iItemID = 31;  break; // Esterk
								case 2: iItemID = 34;  break; // Rapier
								case 3: iItemID = 71;  break; // WarAxe
								}
								break;

							case 11: //IceWorm, FireWorm, Snoob,FireSnoob
							case 13: //Wood Golem, Phamton, Butcher
							case 6: // Orge, WereWolf, Stalker, Dark-Elf, Ice-Golem //nuevos Lalo
								switch (iDice(1, 6)) {
								case 1: iItemID = 50;  break; // GreatSword
									//case 2: iItemID = 650; break; // Zemstone of Sacrifice
								case 2: iItemID = 46;  break; // Claymore
								case 3: iItemID = 31;  break; // Esterk
								case 4: iItemID = 34;  break; // Rapier
									//case 6: iItemID = 650; break; // Zemstone of Sacrifice
								case 5: iItemID = 54; break; // Flamberge
								case 6: iItemID = CONTRIB_SMALL;  break; // ContribBall(+700)
								}
								break;

							case 15:// Yeti IceLizard
							case 7: // Liche, Frost
								switch (iDice(1, 5)) {
								case 1: iItemID = 50;  break; // GreatSword
								case 2: iItemID = 54;  break; // Flameberge
								case 3: iItemID = 31;  break; // Esterk
								case 4: iItemID = 34;  break; // Rapier
								case 5: iItemID = 34;  break; // Rapier
								}
								break;

							case 12: //Red Demon, Blue Demon
							case 14: //Diablo, Satan, Giant-Spider
							case 8: // Demon, Unicorn, Hellclaw, Tigerworm, Gagoyle
							case 18: //heldhound y liche
								switch (iDice(1, 7)) {
									//case 1: iItemID = 3016;  break; // ContribBall(+700)
									//case 1: iItemID = 650; break; // Zemstone of Sacrifice
								case 1: iItemID = 54;  break; // Flameberge
								case 2: iItemID = 560; break; // BattleAxe
								case 3: iItemID = 615;  break; // GiantSword
								case 4: iItemID = 402;  break; // Cape
								case 5: iItemID = 615;  break; // GiantSword
								case 6: iItemID = 560;  break; // BattleAxe
								case 7: iItemID = CONTRIB_SMALL;  break; // ContribBall(+700)
								}
								break;

							case 9: // MountainGiant
								switch (iDice(1, 6)) {
								case 1: iItemID = 23;  break; // Sabre
								case 2: iItemID = 25;  break; // Scimitar
								case 3: iItemID = 760;  break; // Hammer
								case 4: iItemID = 31;  break; // Esterk
								case 5: iItemID = 28;  break; // Falchion
								case 6: iItemID = 760; break; // Hammer
								}
								break;

							case 16: //GrandEttin
								switch (iDice(1, 5)) {
								case 1: iItemID = 843;  break; // BarbarianHammer
								case 2: iItemID = 843;  break; // BarbarianHammer
								case 3: iItemID = 843;  break; // BarbarianHammer
								case 4: iItemID = MAJ_SMALL;  break; // BarbarianHammer
								case 5: iItemID = CONTRIB_SMALL;  break; // ContribBall(+700)
								}
								break;

							case 10: // Ettin
							case 21: //MountDeath
								switch (iDice(1, 5)) {
								case 1: iItemID = 761;  break; // BattleHammer
								case 2: iItemID = 761;  break; // BattleHammer
								case 3: iItemID = MAJ_SMALL;  break; // BattleHammer
									//case 4: iItemID = 650; break; // Zemstone of Sacrifice
									//case 4: iItemID = 3016; break; // ContribBall(+700)
								case 4: iItemID = 761; break; // BatlleHammer
								case 5: iItemID = CONTRIB_SMALL;  break; // ContribBall(+700)
								}
								break;
							}
						}
						else {
							// 30% the weapon is a Wand
							switch (iGenLevel) {
							case 2:
							case 3:
								iItemID = 258; // MagicWand(MS0)
								break;
							case 4:
							case 5:
							case 6:
								iItemID = 257; // MagicWand(MS10) 
								break;
							case 7:
							case 8:
								iItemID = 256; // MagicWand(MS20)
								break;
							case 9:
							case 10:
								break;
							}
						}
					}
					else {
						// 1.4% chance Valuable Drop 40% that drop is an Armor/Shield
						switch (iGenLevel) {
						case 1: // Slime, Giant-Ant, Amphis, Rabbit, Cat
							switch (iDice(1, 3)) {
							case 1: iItemID = 79;  break; // WoodShield
							case 2: iItemID = 80;  break; // LeatherShield
							case 3:
								switch (iDice(1, 2)) {
								case 1: iItemID = 450; break; // Shoes
								case 2: iItemID = 451; break; // Botas
								}
							}
							break;

						case 11:
						case 2: // Skeleton, Orc, Orc-Mage, Scorpion, Zombie
							switch (iDice(1, 3)) {
							case 1: iItemID = 80;  break; // LeatherShield
							case 2: iItemID = 81;  break; // TargeShield
							case 3:
								switch (iDice(1, 2)) {
								case 1: iItemID = 450; break; // Shoes
								case 2: iItemID = 451; break; // Botas
								}
							}
							break;

						case 3: // Stone-Golem, Clay-Golem
							iItemID = 81; // TargeShield
							break;

						case 4: // Hellbound, Rudolph
							switch (iDice(1, 10)) {
							case 1:
							case 2:
								iItemID = 83;
								break; // Hauberk(M)
							case 3:
							case 4:
								iItemID = 472;
								break; // Hauberk(W)
							case 5:
							case 6:
								iItemID = 461;
								break; // ChainHose(M)
							case 7:
							case 8:
								iItemID = 482;
								break; // ChainHose(W)
							case 9:
							case 10:
								iItemID = 454;
								break; // BlondeShield
							}
							break;

						case 5: // Cyclops, Troll, Beholder, Cannibal-Plant, DireBoar
							switch (iDice(1, 4)) {
							case 1:
								switch (iDice(1, 4)) {
								case 1:
								case 2: iItemID = 84;  break; // IronShield
								}
								break;
							case 2:
								switch (iDice(1, 4)) {
								case 1:
								case 2: iItemID = 455; break; // LeatherArmor(M)
								case 3:
								case 4: iItemID = 475; break; // LeatherArmor(W)
								}
								break;
							case 3:
								switch (iDice(1, 4)) {
								case 1:
								case 2: iItemID = 454; break; // Hauberk(M)
								case 3:
								case 4: iItemID = 472; break; // Hauberk(W)
								}
								break;
							case 4:
								switch (iDice(1, 4)) {
								case 1:
								case 2: iItemID = 461; break; // ChainHose(M)
								case 3:
								case 4: iItemID = 482; break; // ChainHose(W)
								}
								break;
							}
							break;

						case 13:
						case 20: //hellbutcher y troll
						case 18: //Hellhound yliche
						case 6: // Orge, WereWolf, Stalker, Dark-Elf, Ice-Golem
							switch (iDice(1, 6)) {
							case 1:
								switch (iDice(1, 4)) {
								case 1:
								case 2: iItemID = 476; break; // ChainMail(W)
								case 3:
								case 4: iItemID = 456; break; // ChainMail(M)
								}
								break;
							case 2:
								switch (iDice(1, 4)) {
								case 1:
								case 2: iItemID = 458; break; // PlateMail(M)
								case 3:
								case 4: iItemID = 478; break; // PlateMail(W)
								}
								break;
							case 3: iItemID = 85; break; // LagiShield
							case 4:
								switch (iDice(1, 8)) {
								case 1: iItemID = 750; break; // Horned-Helm(M)
								case 2: iItemID = 751; break; // Wings-Helm(M)
								case 3: iItemID = 754; break; // Horned-Helm(W)
								case 4: iItemID = 755; break; // Wings-Helm(W)
								case 5: iItemID = 752; break; // Wizard-Cap(M) 
								case 6: iItemID = 753; break; // Wizard-Hat(M)
								case 7: iItemID = 756; break; // Wizard-Cap(W) 
								case 8: iItemID = 757; break; // Wizard-Hat(W) 
								}
								break;
							case 5:
								switch (iDice(1, 4)) {
								case 1:
								case 2: iItemID = 454; break; // Hauberk(M)
								case 3:
								case 4: iItemID = 472; break; // Hauberk(W)
								}
								break;
							case 6:
								switch (iDice(1, 4)) {
								case 1:
								case 2: iItemID = 461; break; // ChainHose(M)
								case 3:
								case 4: iItemID = 482; break; // ChainHose(W)
								}
								break;
							}
							break;

						case 15:
						case 7: // Liche, Frost
							switch (iDice(1, 6)) {
							case 1:
								switch (iDice(1, 2)) {
								case 1: iItemID = 457; break; // ScaleMail(M)
								case 2: iItemID = 477; break; // ScaleMail(W)
								}
								break;
							case 2:
								switch (iDice(1, 2)) {
								case 1: iItemID = 458; break; // PlateMail(M)
								case 2: iItemID = 478; break; // PlateMail(W)
								}
								break;
							case 3: iItemID = 86; break; // KnightShield
							case 4: iItemID = 87; break; // TowerShield
							case 5:
								switch (iDice(1, 2)) {
								case 1: iItemID = 600; break; // Helm(M)
								case 2: iItemID = 602; break; // Helm(M)
								}
								break;
							case 6:
								switch (iDice(1, 2)) {
								case 1: iItemID = 601; break; // Full-Helm(M)
								case 2: iItemID = 603; break; // Full-Helm(M)
								}
								break;
							}
							break;

						case 17: //HellDeath
						case 19: //DeathWyvern
						case 21: //MountDeath
						case 22: //Hellgoyle ydemon
							switch (iDice(1, 2)) {
							case 1:
								switch (iDice(1, 2)) {
								case 1: iItemID = 402; break; //Cape
								case 2: iItemID = 87; break; //TowerShield
								}
								break;
							case 2:
								switch (iDice(1, 2)) {
								case 1: iItemID = 450; break; //Shoes
								case 2: iItemID = 451; break; //LongsBoots
								}
								break;
							}
							break;

						case 12:
						case 14:
						case 16:
						case 8: // Demon, Unicorn, Hellclaw, Tigerworm, Gagoyle
							iItemID = 402; // Cape
							break;

						case 9:
						case 10:
							break;
						}
					}

					pItem = new class CItem;
					if (iItemID == CONTRIB_SMALL)
					{
						int id = 3052;
						if (_bInitItemAttr(pItem, id) == FALSE) {
							delete pItem;
							return;
						}

						pItem->m_dwCount = RollDice(500, 750);
					}
					else if (iItemID == MAJ_SMALL)
					{
						int id = 3055;
						if (_bInitItemAttr(pItem, id) == FALSE) {
							delete pItem;
							return;
						}

						pItem->m_dwCount = RollDice(5, 20);
					}
					else if (iItemID == REP_SMALL)
					{
						int id = 3054;
						if (_bInitItemAttr(pItem, id) == FALSE) {
							delete pItem;
							return;
						}

						pItem->m_dwCount = RollDice(1, 3);
					}
					else
					{
						if (_bInitItemAttr(pItem, iItemID) == FALSE) {
							delete pItem;
							return;
						}
					}					
					
					//Calculo de % de items - LaloRamos - PrimerDropLalo - primer drop
					//(Stat principal de armas y su color)
					if (pItem->m_sItemEffectType == DEF_ITEMEFFECTTYPE_ATTACK) {
						//cambio orden y porcentaje 
						iResult = iDice(1, 80);
						if ((iResult >= 1) && (iResult <= 9)) {
							dwType = 6; //Light
							cColor = 2;
						}
						else if ((iResult >= 10) && (iResult <= 19)) {
							dwType = 8; //Strong
							cColor = 3;
						}
						else if ((iResult >= 20) && (iResult <= 29)) {
							dwType = 1; //Critical
							cColor = 5;
						}
						else if ((iResult >= 30) && (iResult <= 39)) {
							dwType = 5; //Agile
							cColor = 1;
						}
						else if ((iResult >= 40) && (iResult <= 49)) {
							dwType = 3; //Right
							cColor = 7;
						}
						else if ((iResult >= 50) && (iResult <= 59)) {
							dwType = 2; //Poison
							cColor = 4;
						}
						else if ((iResult >= 60) && (iResult <= 69)) {
							dwType = 7; //Sharp
							cColor = 6;
						}
						else if ((iResult >= 70) && (iResult <= 80)) {
							dwType = 9; //Ancient
							cColor = 8;
						}


						pItem->m_cItemColor = cColor;

						//Calculo de % de items - LaloRamos
						//(Poison Damage)
						iResult = iDice(1, 2600);
						if ((iResult >= 1) && (iResult < 1000))			dwValue = 1;
						else if ((iResult >= 1000) && (iResult < 1500))  dwValue = 2;
						else if ((iResult >= 1500) && (iResult < 1900))  dwValue = 3;
						else if ((iResult >= 1900) && (iResult < 2300))  dwValue = 4;
						else if ((iResult >= 2300) && (iResult < 2600))  dwValue = 5;
						//else if ((iResult >= 2600) && (iResult < 2700))  dwValue = 6;
						else dwValue = 3;

						switch (dwType) {
						case 1: if (dwValue <= 5) dwValue = 5; break;
						case 2: if (dwValue <= 4) dwValue = 4; break;
						case 6: if (dwValue <= 4) dwValue = 4; break;
						case 8: if (dwValue <= 2) dwValue = 2; break;
						}
						if ((iGenLevel <= 2) && (dwValue > 7)) dwValue = 7;

						pItem->m_dwAttribute = NULL;
						dwType = dwType << 20;
						dwValue = dwValue << 16;
						pItem->m_dwAttribute = pItem->m_dwAttribute | dwType | dwValue;

						if (iDice(1, 10000) >= 3000) {
							iResult = iDice(1, 1200);
							if ((iResult >= 1) && (iResult <= 299))          dwType = 2; //HP
							else if ((iResult >= 300) && (iResult <= 599))  dwType = 10; //CAD
							else if ((iResult >= 600) && (iResult <= 899))  dwType = 12; //GOLD 
							else if ((iResult >= 900) && (iResult <= 1200)) dwType = 11; //EXP

							//Calculo de % de items - LaloRamos
							//(Hitting Probability, Armas)

							int dwValue = RollByProbabilityTable(kMultiplierProbabilities, 15);

							switch (dwType) {
							case 2: /*if (dwValue <= 3) dwValue = 3;*/ break;
							case 10: /*if (dwValue > 7) dwValue = 7;*/ break;
							case 11:
								//dwValue = 2;
								//	if (dwValue <= 2) dwValue = 2; 
								//	if (dwValue >= 10) dwValue = 10;
								break;
							case 12:
								//dwValue = 5;
								//	if (dwValue <= 5) dwValue = 5;
								//	if (dwValue >= 10) dwValue = 10;
								break;
							}
							//if ((iGenLevel <= 2) && (dwValue > 7)) dwValue = 7;

							dwType = dwType << 12;
							dwValue = dwValue << 8;

							pItem->m_dwAttribute = pItem->m_dwAttribute | dwType | dwValue;
						}
					}

					else if (pItem->m_sItemEffectType == DEF_ITEMEFFECTTYPE_ATTACK_MANASAVE) {
						dwType = 10;
						cColor = 5;

						pItem->m_cItemColor = cColor;

						//Calculo de % de items - LaloRamos
						//(Magic Casting Probability , Wands)
						int dwValue = RollByProbabilityTable(kMultiplierProbabilities, 15);
						if ((iGenLevel <= 2) && (dwValue > 7)) dwValue = 7;
						pItem->m_dwAttribute = NULL;
						dwType = dwType << 20;
						dwValue = dwValue << 16;
						pItem->m_dwAttribute = pItem->m_dwAttribute | dwType | dwValue;

						if (iDice(1, 10000) >= 3000) {
							iResult = iDice(1, 1200);
							if ((iResult >= 1) && (iResult <= 299))          dwType = 2;
							else if ((iResult >= 300) && (iResult <= 599))  dwType = 10;
							else if ((iResult >= 600) && (iResult <= 899))  dwType = 12;
							else if ((iResult >= 900) && (iResult <= 1200)) dwType = 11;

							//Calculo de % de items - LaloRamos
							//(Hitting Probability , Wands)
							// Para alcanzar hasta dwValue = 15 (15 * 7 = 105%)

							int dwValue = RollByProbabilityTable(kMultiplierProbabilities, 15);
							if ((iGenLevel <= 2) && (dwValue > 7)) dwValue = 7;
							switch (dwType) {
							//case 2: if (dwValue <= 3) dwValue = 3; break;
							//case 10: if (dwValue > 7) dwValue = 7; break;
							case 11:
								dwValue = 2;
								//	if (dwValue <= 2) dwValue = 2;
								//	if (dwValue >= 10) dwValue = 10;
								break;
							case 12:
								dwValue = 5;
								//	if (dwValue <= 5) dwValue = 5;
								//	if (dwValue >= 10) dwValue = 10;
								break;
							}
							dwType = dwType << 12;
							dwValue = dwValue << 8;
							pItem->m_dwAttribute = pItem->m_dwAttribute | dwType | dwValue;
						}
					}
					else if (pItem->m_sItemEffectType == DEF_ITEMEFFECTTYPE_DEFENSE) {
						char  cTxt[200], cTemp1[120];


						iResult = iDice(1, 1200);
						if ((iResult >= 1) && (iResult <= 299))          dwType = 8;
						else if ((iResult >= 300) && (iResult <= 599))  dwType = 6;
						else if ((iResult >= 600) && (iResult <= 899))  dwType = 11; //dwType = 11;
						else if ((iResult >= 900) && (iResult <= 1200)) dwType = 12; //dwType = 12;

						int dwValue = RollByProbabilityTable(kMultiplierProbabilities, 15);

					



						//if ((iGenLevel <= 2) && (dwValue > 7)) dwValue = 7;
						if ((pItem->m_sIDnum == 402) && (dwValue == 8)) dwValue = 1;//402
						if ((pItem->m_sIDnum == 402) && (dwValue == 9)) dwValue = 1;//402 lalo fix cape

						pItem->m_dwAttribute = NULL;
						dwType = dwType << 20;
						dwValue = dwValue << 16;
						pItem->m_dwAttribute = pItem->m_dwAttribute | dwType | dwValue;

						if (iDice(1, 10000) >= 3000) {
							iResult = iDice(1, 2200);
							if ((iResult >= 1) && (iResult <= 299))           dwType = 3; //DR
							else if ((iResult >= 300) && (iResult <= 599))  dwType = 1; //PR
							else if ((iResult >= 600) && (iResult <= 899))  dwType = 5; //SP
							else if ((iResult >= 900) && (iResult <= 1199))  dwType = 4; //HP
							else if ((iResult >= 1200) && (iResult <= 1499))  dwType = 6; //MP
							else if ((iResult >= 1500) && (iResult <= 1799))  dwType = 7; //MR
							else if ((iResult >= 1800) && (iResult <= 1999))  dwType = 8; //PA
							else if ((iResult >= 2000) && (iResult <= 2200)) dwType = 9; //MA

							int dwValue = RollByProbabilityTable(kMultiplierProbabilities, 15);



							

							//New Code
							//Drop minimo multiplicador * 3
							//ejemplo 7*3 = 21 o 3*3 = 9

							//MA MR ETC minimo 21 o 9

							switch (dwType) {
							case 3: //DR
							case 7: //MR
							case 8: //PA
							case 9: //MA
								//if (dwValue <= 3) dwValue = 3; //quitando minimo
								break;
							case 1:
							case 4:
							case 5:
							case 6:
								//if (dwValue <= 3) dwValue = 3; //quitando minimo
								break;
							}
							//if ((iGenLevel <= 2) && (dwValue > 7)) dwValue = 7;

							/*

							if (dwValue >= 5){
								wsprintf(cTxt, "Atributo: %i", dwValue);
								CharacterLogList(cTxt);
							}

							*/


							dwType = dwType << 12;
							dwValue = dwValue << 8;

							

							pItem->m_dwAttribute = pItem->m_dwAttribute | dwType | dwValue;
						}
					}
					_AdjustRareItemValue(pItem);
				}
			}

			pItem->m_sTouchEffectType = DEF_ITET_ID;
			pItem->m_sTouchEffectValue1 = iDice(1, 100000);

			// Anti Dup System
			SYSTEMTIME SysTime;
			char cTemp[256];
			GetLocalTime(&SysTime);
			ZeroMemory(cTemp, sizeof(cTemp));
			pItem->m_sTouchEffectValue2 = iDice(1, 100000);
			wsprintf(cTemp, "%02d%02d", (short)SysTime.wMonth, (short)SysTime.wDay);
			pItem->m_sTouchEffectValue2 = atoi(cTemp);

			ZeroMemory(cTemp, sizeof(cTemp));
			wsprintf(cTemp, "%02d%02d%02d%d", (short)SysTime.wHour, (short)SysTime.wMinute, (short)SysTime.wSecond, (short)SysTime.wMilliseconds);
			pItem->m_sTouchEffectValue3 = atoi(cTemp);

			m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->bSetItem(m_pNpcList[iNpcH]->m_sX, m_pNpcList[iNpcH]->m_sY, pItem);
			pItem->m_dwTimeDrop = timeGetTime();

			SendEventToNearClient_TypeB(CLIENT_COMMON_EVENT, CLIENT_COMMON_ITEMDROP, m_pNpcList[iNpcH]->m_cMapIndex,
				m_pNpcList[iNpcH]->m_sX, m_pNpcList[iNpcH]->m_sY, pItem->m_sSprite, pItem->m_sSpriteFrame, pItem->m_cItemColor); //v1.4 color

			_bItemLog(DEF_ITEMLOG_NEWGENDROP, NULL, NULL, pItem, SaveDrop);
		}
#ifdef DEF_DEBUG
	}
	catch (...) {
		ErrorList("Crash Evitado en: NpcDeadItemGenerator");
	}
#endif
}


//HASTA ACA
void CMapServer::DeleteNpc(int iNpcH, BOOL bHeld, BOOL Drop)
{
#ifdef DEF_DEBUG
	try {
#endif
		int  i, iNamingValue, iItemID, iNumItem, iItemIDs[MAX_NPCITEMDROP];
		char cTmp[21], cItemName[21], cTemp[256];
		class CItem * pItem;
		DWORD dwCount, dwTime;
		POINT	ItemPositions[MAX_NPCITEMDROP];
		SYSTEMTIME SysTime;
		char cTxt[100];
		int k;
		short sAttackerH;

		int coinMin = 1;
		int coinMax = 2;
		int itemType = 0;

		int iItemCounts[10];
		memset(iItemCounts, 0, sizeof(iItemCounts));




		if (m_pNpcList[iNpcH] == NULL) return;

		iItemID = 0;
		iNumItem = 0;

		dwTime = timeGetTime();

		SendEventToNearClient_TypeA(iNpcH, DEF_OWNERTYPE_NPC, CLIENT_COMMON_COMMAND_LOG, DEF_MSGTYPE_REJECT, NULL, NULL, NULL);
		if (m_pNpcList[iNpcH]->m_sAreaSize == 0) {
			m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->ClearOwner(11, iNpcH, DEF_OWNERTYPE_NPC, m_pNpcList[iNpcH]->m_sX, m_pNpcList[iNpcH]->m_sY);
		}
		else {
			m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->ClearBigOwner(iNpcH, DEF_OWNERTYPE_NPC, m_pNpcList[iNpcH]->m_sX, m_pNpcList[iNpcH]->m_sY, m_pNpcList[iNpcH]->m_sAreaSize);
		}
		ZeroMemory(cTmp, sizeof(cTmp));
		strcpy_secure(cTmp, (char *)(m_pNpcList[iNpcH]->m_cName + 2));
		iNamingValue = atoi(cTmp);

		m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->SetNamingValueEmpty(iNamingValue);
		m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->m_iTotalActiveObject--;
		if (Drop == FALSE) m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->m_iTotalAliveObject--;

		// Spot-mob-generator���� 
		if (m_pNpcList[iNpcH]->m_iSpotMobIndex != NULL)
			m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->m_stSpotMobGenerator[m_pNpcList[iNpcH]->m_iSpotMobIndex].iCurMobs--;

		RemoveFromTarget(iNpcH, DEF_OWNERTYPE_NPC);

		switch (m_pNpcList[iNpcH]->m_sType) {
		case 36:
		case 37:
		case 38:
		case 39:
		case 42:
			m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->bRemoveCrusadeStructureInfo(m_pNpcList[iNpcH]->m_sX, m_pNpcList[iNpcH]->m_sY);

			for (i = 0; i < DEF_MAXGUILDS; i++) {
				if (m_pGuildTeleportLoc[i].m_iV1 == m_pNpcList[iNpcH]->m_iGuildGUID) {
					m_pGuildTeleportLoc[i].m_dwTime = dwTime;
					m_pGuildTeleportLoc[i].m_iV2--;
					if (m_pGuildTeleportLoc[i].m_iV2 < 0) m_pGuildTeleportLoc[i].m_iV2 = 0;
					break;
				}
			}
			break;

		case 43:
		case 44:
		case 45:
		case 46:
		case 47:
		case 51:
			for (i = 0; i < DEF_MAXGUILDS; i++) {
				if (m_pGuildTeleportLoc[i].m_iV1 == m_pNpcList[iNpcH]->m_iGuildGUID) {
					m_pGuildTeleportLoc[i].m_iNumSummonNpc--;
					if (m_pGuildTeleportLoc[i].m_iNumSummonNpc < 0) m_pGuildTeleportLoc[i].m_iNumSummonNpc = 0;
					break;
				}
			}
			break;

		case 64:	//v2.19 2002-12-16 ���� ��ų ����
			m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->bRemoveCropsTotalSum();
			break;
		}

		// DelayEvent�� ���� 
		bRemoveFromDelayEventList(iNpcH, DEF_OWNERTYPE_NPC, NULL);
		int iItemprobability = 65;

		if ((m_iMiddlelandMapIndex == m_pNpcList[iNpcH]->m_cMapIndex) && (m_iMiddlelandMapIndex != -1)) iItemprobability = 55;
		if (m_iWhouseMapIndex == m_pNpcList[iNpcH]->m_cMapIndex) iItemprobability = 15;
		if (m_pNpcList[iNpcH]->m_bIsSummoned == FALSE) {
			pItem = new class CItem;
			ZeroMemory(cItemName, sizeof(cItemName));

			//V10 Reapertura low drops
			switch (m_pNpcList[iNpcH]->m_sType) {
			case 1: // Mercenary
			case 2: // Mercenary
			case 3: // Mercenary
			case 4: // Mercenary
			case 5: // Mercenary
			case 6: // Mercenary
			case 36: // Sade Structure
			case 37: // Sade Structure
			case 38: // Sade Structure
			case 39: // Sade Structure
			case 40: // Sade Structure
			case 41: // Sade Structure
			case 42: // Sade Structure
			case 87: // Tower Denfense Structure
			case 89: // Tower Denfense Structure
			case 92: // Reliquia
			case 55: // Rabbit
			case 56: // Cat
				// No drop
				break;
			case 10: //Slime //New Rare Drop Helbreath Argentina v4
				switch (iDice(1, 4)) {
				case 1: if (iDice(1, 10) >= 5) iItemID = 220; break;	// "SlimeJelly"
				case 2: if (iDice(1, 1000) == 50) iItemID = 650; break;	// "ZemstoneofSacrifice"
				case 3: if (iDice(1, 1000) == 50) iItemID = 656; break;	// "StoneOfXelima"
				case 4: if (iDice(1, 1000) == 50) iItemID = 657; break;	// "StoneOfMerien"
				default: break;
				}
				break;
			case 11: //Skeleton
				switch (iDice(1, 6)) {
				case 1: if (iDice(1, 20) == 1) iItemID = 219; break;	// "SkeletonBones"
				case 2: if (iDice(1, 100) == 5) iItemID = 650; break;	// "RubyRing"
				case 3: if (iDice(1, 1000) == 50) iItemID = 650; break;	// "ZemstoneofSacrifice"
				case 4: if (iDice(1, 1000) == 50) iItemID = 656; break;	// "StoneOfXelima"
				case 5: if (iDice(1, 1000) == 50) iItemID = 657; break;	// "StoneOfMerien"
				case 6: if (iDice(1, 5000) == 500) iItemID = 650; break;	// "EmeraldRing"
				default: break;
				}
				break;
			case 12: //StoneGolem
				switch (iDice(1, 6)) {
				case 1: if (iDice(1, 30) == 1) iItemID = 221; break;		// "StoneGolemPiece"
				case 2: if (iDice(1, 1000) == 50) iItemID = 650; break;		// "ZemstoneofSacrifice"
				case 3: if (iDice(1, 1000) == 50) iItemID = 656; break;		// "StoneOfXelima"
				case 4: if (iDice(1, 1000) == 50) iItemID = 657; break;		// "StoneOfMerien"
				case 5: if (iDice(1, 2500) == 50) iItemID = 647; break;		// "NecklaceOfStoneGolem"
				case 6: if (iDice(1, 4500) == 100) iItemID = 858; break;		// "NecklaceOfMerien"
				default: break;
				}
				break;
			case 13://Cyclops
				switch (iDice(1, 12)) {
				case 1: if (iDice(1, 30) == 1) iItemID = 194; break;	// "CyclopsEye"
				case 2: if (iDice(1, 40) == 1) iItemID = 195; break;	// "CyclopsHandEdge"
				case 3: if (iDice(1, 30) == 1) iItemID = 196; break;	// "CyclopsHeart"
				case 4: if (iDice(1, 20) == 1) iItemID = 197; break;	// "CyclopsMeat"
				case 5: if (iDice(1, 40) == 1) iItemID = 198; break;	// "CyclopsLeather"
				case 6: if (iDice(1, 500) == 3) iItemID = REP_SMALL; break;	// "RepPotion(+1)"
				case 7: if (iDice(1, 1000) == 50) iItemID = 650; break;	// "ZemstoneofSacrifice"
				case 8: if (iDice(1, 1000) == 50) iItemID = 656; break;	// "StoneOfXelima"
				case 9: if (iDice(1, 1000) == 150) iItemID = 657; break;	// "StoneOfMerien"
				case 10: if (iDice(1, 500) == 15) iItemID = 308; break;	// "Neck(MS10)"
				case 11: if (iDice(1, 500) == 15) iItemID = 300; break;	// "Neck(RM10)"
				case 12: if (iDice(1, 500) == 15) iItemID = 311; break;	// "Neck(DF10)"
				default: break;
				}
				break;
			case 14: //orco
				switch (iDice(1, 14)) {
				case 1: if (iDice(1, 11) == 1) iItemID = 206; break;		// "OrcMeat"
				case 2: if (iDice(1, 20) == 1) iItemID = 207; break;		// "OrcLeather"
				case 3: if (iDice(1, 21) == 1) iItemID = 208; break;		// "OrcTeeth"
				case 4: if (iDice(1, 150) == 15) iItemID = 336; break;		// "SaphireRing"
				case 5: if (iDice(1, 150) == 25) iItemID = 333; break;		// "PlatiniumRing"
				case 6: if (iDice(1, 150) == 35) iItemID = 334; break;		// "LuckyGoldRing"
				case 7: if (iDice(1, 150) == 45) iItemID = 337; break;		// "RubyRing"
				case 8: if (iDice(1, 1000) == 15) iItemID = 335; break;		// "EmeraldRing"
				case 9: if (iDice(1, 100) == 45) iItemID = 634; break;		// "RingOfWizard"
				case 10: if (iDice(1, 200) == 25) iItemID = 635; break;		// "RingOfMage"
				case 11: if (iDice(1, 1000) == 35) iItemID = 650; break;		// "ZemstoneofSacrifice"
				case 12: if (iDice(1, 1000) == 45) iItemID = 656; break;		// "StoneOfXelima"
				case 13: if (iDice(1, 1000) == 25) iItemID = 657; break;		// "StoneOfMerien"
				case 14: if (iDice(1, 5000) == 500) iItemID = 638; break;		// "NecklaceOfFireProtection"
				default: break;
				}
				break;
			case 16: //Ant
				switch (iDice(1, 5)) {
				case 1: if (iDice(1, 9) == 1)  iItemID = 192; break;	// "AntLeg"
				case 2: if (iDice(1, 10) == 1) iItemID = 193; break;	// "AntFeeler"
				case 3: if (iDice(1, 1000) == 50) iItemID = 650; break;	// "ZemstoneofSacrifice"
				case 4: if (iDice(1, 1000) == 50) iItemID = 656; break;	// "StoneOfXelima"
				case 5: if (iDice(1, 1000) == 50) iItemID = 657; break;	// "StoneOfMerien"
				default: break;
				}
				break;
			case 17: //Scorpion
				switch (iDice(1, 11)) {
				case 1: if (iDice(1, 40) == 1) iItemID = 215; break;		// "ScorpionPincers"
				case 2: if (iDice(1, 20) == 1) iItemID = 216; break;		// "ScorpionMeat"
				case 3: if (iDice(1, 40) == 1) iItemID = 217; break;		// "ScorpionSting"
				case 4: if (iDice(1, 40) == 1) iItemID = 218; break;		// "ScorpionSkin"
				case 5: if (iDice(1, 500) == 3) iItemID = 3015; break;		// "RepPotion(+1)"
				case 6: if (iDice(1, 1000) == 50) iItemID = 650; break;		// "ZemstoneofSacrifice"
				case 7: if (iDice(1, 1000) == 50) iItemID = 656; break;		// "StoneOfXelima"
				case 8: if (iDice(1, 1000) == 50) iItemID = 657; break;		// "StoneOfMerien"
				case 9: if (iDice(1, 500) == 5) iItemID = 634; break;		// "RingOfWizard"
				case 10: if (iDice(1, 500) == 5) iItemID = 336; break;		// "SaphireRing"
				case 11: if (iDice(1, 500) == 145) iItemID = 333; break;	// "PlatiniumRing"
				default: break;
				}
				break;
			case 18: // Zombie
				switch (iDice(1, 6)) {
				case 1: if (iDice(1, 1000) == 30) iItemID = 650; break;			// "ZemstoneofSacrifice"
				case 2: if (iDice(1, 1000) == 30) iItemID = 656; break;			// "StoneOfXelima"
				case 3: if (iDice(1, 1000) == 30) iItemID = 657; break;			// "StoneOfMerien"
				case 4: if (iDice(1, 800) == 3) iItemID = 3015; break;			// "RepPotion(+3)"
				case 5: if (iDice(1, 16667) == 1) iItemID = 641; break;			// "NecklaceofMedusa" //probabilidad 1 cada 100mil zombies
				case 6: if (iDice(1, 16667) == 1) iItemID = 613; break;			// "SwordofMedusa"
				default: break;
				}
				break;
			case 22: //Snake
				switch (iDice(1, 8)) {
				case 1: if (iDice(1, 15) == 1) iItemID = 188; break;	// "SnakeMeat"
				case 2: if (iDice(1, 16) == 1) iItemID = 189; break;	// "SnakeSkin"
				case 3: if (iDice(1, 16) == 1) iItemID = 190; break;	// "SnakeTeeth"
				case 4: if (iDice(1, 17) == 1) iItemID = 191; break;	// "SnakeTounge"
				case 5: if (iDice(1, 1000) == 50) iItemID = 650; break;	// "ZemstoneofSacrifice"
				case 6: if (iDice(1, 1000) == 50) iItemID = 656; break;	// "StoneOfXelima"
				case 7: if (iDice(1, 1000) == 50) iItemID = 657; break;	// "StoneOfMerien"
				case 8: if (iDice(1, 5000) == 50) iItemID = 640; break;	// "NecklaceOfSufferent"
				default: break;
				}
				break;
			case 23: //ClayGolem
				switch (iDice(1, 4)) {
				case 1: if (iDice(1, 30) == 1) iItemID = 205; break;		// "LumpOfClay"
				case 2: if (iDice(1, 1000) == 50) iItemID = 650; break;		// "ZemstoneofSacrifice"
				case 3: if (iDice(1, 1000) == 50) iItemID = 656; break;		// "StoneOfXelima"
				case 4: if (iDice(1, 1000) == 50) iItemID = 657; break;		// "StoneOfMerien"
				case 5: if (iDice(1, 2500) == 350) iItemID = 858; break;		// "NecklaceOfMerien"							
				default: break;
				}
				break;
			case 27: //Hellbound
				switch (iDice(1, 10)) {
				case 1: if (iDice(1, 40) == 1) iItemID = 199; break;	// "HelhoundHeart"
				case 2: if (iDice(1, 30) == 1) iItemID = 200; break;	// "HelhoundLeather"
				case 3: if (iDice(1, 30) == 1) iItemID = 201; break;	// "HelhoundTail"
				case 4: if (iDice(1, 30) == 1) iItemID = 202; break;	// "HelhoundTeeth"
				case 5: if (iDice(1, 30) == 1) iItemID = 203; break;	// "HelhoundClaw"
				case 6: if (iDice(1, 50) == 1) iItemID = 204; break;	// "HelhoundTounge"
				case 7: if (iDice(1, 500) == 3) iItemID = REP_SMALL; break;	// "RepPotion(+1)"
				case 8: if (iDice(1, 1000) == 50) iItemID = 650; break;	// "ZemstoneofSacrifice"
				case 9: if (iDice(1, 1000) == 50) iItemID = 656; break;	// "StoneOfXelima"
				case 10: if (iDice(1, 1000) == 50) iItemID = 657; break;	// "StoneOfMerien"
				default: break;
				}
				break;
			case 28://Troll
				switch (iDice(1, 12)) {
				case 1: if (iDice(1, 35) == 1) iItemID = 222; break;	// "TrollHeart"
				case 2: if (iDice(1, 23) == 1) iItemID = 223; break;	// "TrollMeat"
				case 3: if (iDice(1, 25) == 1) iItemID = 224; break;	// "TrollLeather"
				case 4: if (iDice(1, 27) == 1) iItemID = 225; break;	// "TrollClaw"
				case 5: if (iDice(1, 500) == 3) iItemID = REP_SMALL; break;	// "RepPotion(+1)"
				case 6: if (iDice(1, 100) == 5) iItemID = 334; break;	// "LuckyGoldRing"
				case 7: if (iDice(1, 100) == 5) iItemID = 337; break;	// "RubyRing"
				case 8: if (iDice(1, 100) == 5) iItemID = 336; break;	// "SaphireRing"
				case 9: if (iDice(1, 100) == 5) iItemID = 333; break;	// "PlatinumRing"
				case 10: if (iDice(1, 1000) == 50) iItemID = 650; break;	// "ZemstoneofSacrifice"
				case 11: if (iDice(1, 1000) == 50) iItemID = 656; break;	// "StoneOfXelima"
				case 12: if (iDice(1, 1000) == 50) iItemID = 657; break;	// "StoneOfMerien"
				default: break;
				}
				break;
			case 29://Ogre
				switch (iDice(1, 13)) {
				case 1: if (iDice(1, 20) == 1) iItemID = 209; break;		// "OgreHair"
				case 2: if (iDice(1, 22) == 1) iItemID = 210; break;		// "OgreHeart"
				case 3: if (iDice(1, 25) == 1) iItemID = 211; break;		// "OgreMeat"
				case 4: if (iDice(1, 25) == 1) iItemID = 212; break;		// "OgreLeather"
				case 5: if (iDice(1, 25) == 1) iItemID = 213; break;		// "OgreTeeth"
				case 6: if (iDice(1, 25) == 1) iItemID = 214; break;		// "OgreClaw"
				case 7: if (iDice(1, 500) == 3) iItemID = REP_SMALL; break;		// "RepPotion(+3)"
				case 8: if (iDice(1, 100) == 15) iItemID = 300; break;		// "Neck(RM10)"
				case 9: if (iDice(1, 100) == 15) iItemID = 311; break;		// "Neck(DF10)"
				case 10: if (iDice(1, 500) == 20) iItemID = 632; break;		// "RingofOgrePower"
				case 11: if (iDice(1, 1000) == 60) iItemID = 650; break;		// "ZemstoneofSacrifice"
				case 12: if (iDice(1, 1000) == 60) iItemID = 656; break;		// "StoneOfXelima"
				case 13: if (iDice(1, 1000) == 60) iItemID = 657; break;		// "StoneOfMerien"
				default: break;
				}
				break;
			case 30://Liche
				switch (iDice(1, 12)) {
				case 1: if (iDice(1, 1000) == 60) iItemID = 656; break;		// "StoneOfXelima"
				case 2: if (iDice(1, 1000) == 60) iItemID = 657; break;		// "StoneOfMerien"
				case 3: if (iDice(1, 1000) == 60) iItemID = 650; break;		// "ZemstoneofSacrifice"
				case 4: if (iDice(1, 1500) == 50) iItemID = 259; break;		// "MagicWand(MSHIELD)"
				case 5: if (iDice(1, 500) == 3) iItemID = 636; break;		// "RingOfGrandMage"
				case 6:	if (iDice(1, 4000) == 3) iItemID = 734; break;		// "RingOfArcMage"
				case 7: if (iDice(1, 5000) == 82) iItemID = 648; break;		// "NecklaceOfLiche"
				case 8: if (iDice(1, 6000) == 250) iItemID = 291; break;		// "MagicWand(MS30-LLF)"
				case 9:	if (iDice(1, 8000) == 300) iItemID = 381; break;		// "MassFireStrikeManual"
				case 10: if (iDice(1, 10000) == 100) iItemID = 382; break;		// "BloodyShoWaveManual"
				case 11: if (iDice(1, 25000) == 1) iItemID = 380; break;	// IceStormManual 
				case 12: if (iDice(1, 25000) == 1) iItemID = 857; break;	// InibitionCastingManual
				default: break;
				}
				break;
			case 31://Demons
			
				switch (iDice(1, 10)) {
				case 1: if (iDice(1, 15) == 3) iItemID = 541; break;		// "DemonHeart"
				case 2:	if (iDice(1, 5) == 3) iItemID = 542; break;			// "DemonMeat"
				case 3:	if (iDice(1, 10) == 3) iItemID = 543; break;		// "DemonLeather"
				case 4:	if (iDice(1, 15) == 3) iItemID = 540; break;		// "DemonEye"
				case 5: if (iDice(1, 1000) == 50) iItemID = 656; break;		// "StoneOfXelima"
				case 6: if (iDice(1, 1000) == 50) iItemID = 657; break;		// "StoneOfMerien"
				case 7: if (iDice(1, 1000) == 50) iItemID = 650; break;		// "ZemstoneofSacrifice"
				case 8: if (iDice(1, 500) == 10) iItemID = 633; break;		// "RingofDemonpower"
				case 9: if (iDice(1, 222) == 1) iItemID = 382; break;		// "BloodyShoWaveManual"
				case 10: if (iDice(1, 10000) == 1) iItemID = 616; break; //Demon Slayer

				default: break;
				}
				break;
			case 32: //Unicorn
			
				switch (iDice(1, 11)) {
				case 1: if (iDice(1, 40) == 3) iItemID = 544; break;		// "UnicornHeart"
				case 2: if (iDice(1, 25) == 3) iItemID = 545; break;		// "UnicornHorn"	
				case 3: if (iDice(1, 10) == 3) iItemID = 546; break;		// "UnicornMeat"	
				case 4: if (iDice(1, 15) == 3) iItemID = 547; break;		// "UnicornLeather"					
				case 5: if (iDice(1, 50) == 1) iItemID = 656; break;		// "StoneOfXelima"
				case 6: if (iDice(1, 50) == 50) iItemID = 657; break;		// "StoneOfMerien"
				case 7: if (iDice(1, 50) == 50) iItemID = 650; break;		// "ZemstoneofSacrifice"
				case 8: if (iDice(1, 1000) == 20) iItemID = 851; break;		// "KlonessEsterk"
				case 9: if (iDice(1, 30000) == 1) iItemID = 864; break;		// "KlonessWand"
				case 10: if (iDice(1, 30000) == 1) iItemID = 849; break;	// "KlonessBlade"
				case 11: if (iDice(1, 30000) == 1) iItemID = 850; break;	// "KlonessAxe"
				default: break;
				}
				break;
			
			case 48: // Stalker
				switch (iDice(1, 5)) {
				case 1: if (iDice(1, 1000) == 10) iItemID = 656; break;		// "StoneOfXelima"
				case 2: if (iDice(1, 1000) == 20) iItemID = 657; break;		// "StoneOfMerien"
				case 3: if (iDice(1, 1000) == 50) iItemID = REP_SMALL; break;	// "RepPotion(+1)"
				case 4: if (iDice(1, 1000) == 30) iItemID = 650; break;		// "ZemstoneofSacrifice"
				case 5: if (iDice(1, 1500) == 30) iItemID = 292; break;		// "GoldenAxeLLF"
				default: break;
				}
				break;
			case 52: // Gagoyle:
				switch (iDice(1, 11)) {
				case 1: if (iDice(1, 1000) == 50) iItemID = 656; break;		// "StoneOfXelima"
				case 2: if (iDice(1, 1000) == 50) iItemID = 657; break;		// "StoneOfMerien"
				case 3: if (iDice(1, 1000) == 50) iItemID = 650; break;		// "ZemstoneofSacrifice"
				case 4: if (iDice(1, 500) == 10) iItemID = 630; break;		// "ringofxelima"
				case 5: if (iDice(1, 10000) == 1000) iItemID = 381; break;		// "FireStrikeManual"
				case 6: if (iDice(1, 10000) == 1000) iItemID = 382; break;		// "BloodyShockWaveManual"
				case 7: if (iDice(1, 5000) == 50) iItemID = 20; break;		// "Excalibur"
				case 8: if (iDice(1, 5000) == 100) iItemID = 610; break;		// "xelimablade"
				case 9: if (iDice(1, 90000) == 15800) iItemID = 860; break;		// "NecklaceOfXelima"
				case 10: if (iDice(1, 5000) == 400) iItemID = 612; break;		// "xelimarapier"
				case 11: if (iDice(1, 5000) == 150) iItemID = 611; break;		// "xelimaaxe"
				default: break;
				}
				break;
			case 54: // Dark-Elf
				switch (iDice(1, 4)) {
				case 1: if (iDice(1, 500) == 5) iItemID = 656; break;		// "StoneOfXelima"
				case 2: if (iDice(1, 500) == 5) iItemID = 657; break;		// "StoneOfMerien"
				case 3: if (iDice(1, 500) == 5) iItemID = 650; break;		// "ZemstoneofSacrifice"
				case 4: if (iDice(1, 60000) == 25810) iItemID = 618; break;	// "DarkelfBow"
				default: break;
				}
				break;
			case 58: // Mountain Giant
				switch (iDice(1, 5)) {
				case 1: if (iDice(1, 1000) == 10) iItemID = 656; break;		// "StoneOfXelima"
				case 2: if (iDice(1, 1000) == 20) iItemID = 657; break;		// "StoneOfMerien"
				case 3: if (iDice(1, 500) == 50) iItemID = REP_SMALL; break;	// "RepPotion(+1)"
				case 4: if (iDice(1, 1000) == 30) iItemID = 650; break;		// "ZemstoneofSacrifice"
				case 5: if (iDice(1, 500) == 3) iItemID = 337; break;		// "RubyRing"
				default: break;
				}
				break;
			case 59: // Ettin
				switch (iDice(1, 6)) {
				case 1: if (iDice(1, 1000) == 50) iItemID = 656; break;		// "StoneOfXelima"
				case 2: if (iDice(1, 1000) == 50) iItemID = 657; break;		// "StoneOfMerien"
				case 3: if (iDice(1, 1000) == 50) iItemID = 650; break;		// "ZemstoneofSacrifice"
				case 4: if (iDice(1, 500) == 30) iItemID = REP_SMALL; break;	// "RepPotion(+1)"
				case 5: if (iDice(1, 25000) == 1) iItemID = 762; break;		// GiantBattleHammer (1 cada 150k)
				case 6: if (iDice(1, 50000) == 1) iItemID = 4905; break;		// BlackHammer (1 cada 300k)
				default: break;
				}
				break;
			case 71: // Centaurus
				switch (iDice(1, 4)) {
				case 1: if (iDice(1, 1000) == 30) iItemID = 650; break;		// "ZemstoneOfSacrifice"	
				case 2: if (iDice(1, 1000) == 30) iItemID = 656; break;		// "StoneOfXelima"	
				case 3: if (iDice(1, 1000) == 30) iItemID = 657; break;		// "StoneOfMerien"
				case 4: if (iDice(1, 25000) == 1) iItemID = 848; break;		// LightningBlade
				default: break;
				}
				break;
			case 61: // Rudolph
				switch (iDice(1, 3)) {
				case 1: if (iDice(1, 1000) == 30) iItemID = 650; break;		// "ZemstoneOfSacrifice"
				case 2: if (iDice(1, 1000) == 30) iItemID = 656; break;		// "StoneOfXelima"
				case 3: if (iDice(1, 1000) == 30) iItemID = 657; break;		// "StoneOfMerien"
				default: break;
				}
				break;
			case 57: // Giant-Frog
			case 62: //Direboard
			case 70: // Barlog
			case 75: // Giant-Lizard
			case 77: // MasterMage-Orc
			case 122: //Goblin
			case 123: //GmGoblin
				switch (iDice(1, 3)) {
				case 1: if (iDice(1, 1000) == 30) iItemID = 650; break;		// "ZemstoneOfSacrifice"
				case 2: if (iDice(1, 1000) == 30) iItemID = 656; break;		// "StoneOfXelima"
				case 3: if (iDice(1, 1000) == 30) iItemID = 657; break;		// "StoneOfMerien"
			//	case 4: if (iDice(1, 5000) == 3520) iItemID = 3100; break;		// "Coins(+5)"
				default: break;
				}
				break;
				//Nuevos NPC 2Drop LaloRamos
			case 93:	//IceWorm
			case 94:	//FireWorm
			case 100:	//Snoob
			case 101:	//FireSnoob
				switch (iDice(1, 5)) {
				case 1: if (iDice(1, 1000) == 250) iItemID = 650; break; //"ZemstoneofSacrifice"
				case 2: if (iDice(1, 1000) == 250) iItemID = MAJ_SMALL; break; //"ExpStone" 
				case 3: if (iDice(1, 1000) == 250) iItemID = CONTRIB_SMALL; break; //"GoldStone"
				case 4: if (iDice(1, 1000) == 250) iItemID = REP_SMALL; break; //"PoisonStone"	
				case 5: if (iDice(1, 1000) == 250) iItemID = 942; break; //"RighStone"
				default: break;
				}
				break;
			case 97:	//Good-Golem
			case 98:	//Ice-Lizard
			case 99:	//Butcher
				switch (iDice(1, 6)) {
				case 1: if (iDice(1, 1000) == 350) iItemID = 941; break; //"PoisonStone"
				//case 2: if (iDice(1, 1000) == 350) iItemID = 942; break; //"RighStone"
				case 2: if (iDice(1, 1000) == 350) iItemID = 944; break; //"AgileStone"
				case 3: if (iDice(1, 1000) == 350) iItemID = 945; break; //"LightStone"
				case 4: if (iDice(1, 1000) == 350) iItemID = 946; break; //"SharpStone"
				//case 6: if (iDice(1, 1000) == 350) iItemID = 947; break; //"StrongStone"
				case 5: if (iDice(1, 1000) == 350) iItemID = 948; break; //"AncStone"
				case 6: if (iDice(1, 1000) == 350) iItemID = 951; break; //"CritStone"
				default: break;
				}
				break;
			case 103:	//GiantSpider
			case 104:	//Diablo
			case 105:	//Phamton
			case 106:	//Yeti
				/*switch (iDice(1, 8)) { //modificar
				case 1: if (iDice(1, 1000) == 350) iItemID = 949; break; //"MPCStone" 
				case 2: if (iDice(1, 1000) == 350) iItemID = 950; break; //"MCStone"
				case 3: if (iDice(1, 1000) == 350) iItemID = 952; break; //"PrStone"
				case 4: if (iDice(1, 1000) == 350) iItemID = 956; break; //"SpStone"
				case 5: if (iDice(1, 1000) == 350) iItemID = 955; break; //"HpStone"
				case 6: if (iDice(1, 1000) == 350) iItemID = 957; break; //"MpStone"
				case 7: if (iDice(1, 1000) == 350) iItemID = 961; break; //"CADStone"
				case 8: if (iDice(1, 1000) == 350) iItemID = 953; break; //"HitStone"
				default: break;
				}*/
				break;
			case 95:	//Blue Demon
			case 96:	//Red Demon
			case 102:	//Satan
			case 107:	//GrandEttin
				/*switch (iDice(1, 6)) { //modificar
				case 1: if (iDice(1, 1000) == 350) iItemID = 962; break; //"ExpStone" 
				case 2: if (iDice(1, 1000) == 350) iItemID = 963; break; //"GoldStone"
				case 3: if (iDice(1, 1000) == 350) iItemID = 954; break; //"DefRStone" 
				case 4: if (iDice(1, 1000) == 350) iItemID = 958; break; //"MrStone"
				case 5: if (iDice(1, 1000) == 350) iItemID = 959; break; //"PaStone"
				case 6: if (iDice(1, 1000) == 350) iItemID = 960; break; //"MaStone"
				default: break;
				}*/
				break;
			case 124: //Danny
				/*switch (iDice(1, 6)) { //modificar
				case 1: if (iDice(1, 1000) == 350) iItemID = 962; break; //"ExpStone" 
				case 2: if (iDice(1, 1000) == 350) iItemID = 963; break; //"GoldStone"
				case 3: if (iDice(1, 1000) == 350) iItemID = 954; break; //"DefRStone" 
				case 4: if (iDice(1, 1000) == 350) iItemID = 958; break; //"MrStone"
				case 5: if (iDice(1, 1000) == 350) iItemID = 959; break; //"PaStone"
				case 6: if (iDice(1, 1000) == 350) iItemID = 960; break; //"MaStone"
				default: break;
				}*/
				break;
			case 79:	// Nizie
			case 63:	// Frost
				switch (iDice(1, 4)) {
				case 1: if (iDice(1, 1000) == 50) iItemID = 656; break;		// "StoneOfXelima"
				case 2: if (iDice(1, 1000) == 50) iItemID = 657; break;		// "StoneOfMerien"
				case 3: if (iDice(1, 800) == 3) iItemID = REP_SMALL; break;		// "RepPotion(+7)"
				case 4: if (iDice(1, 500) == 3) iItemID = REP_SMALL; break;		// "RepPotion(+3)"
					//		case 5: if (iDice(1, 10000) == 100) iItemID = 862;	break;  // "BerserkWand(MS10)"
					//		case 6: if (iDice(1, 10000) == 100) iItemID = 861; break;	// "BerserkWand(MS30)"
					//		case 7: if (iDice(1, 10000) == 100) iItemID = 845; break;	// "StormBringer"
				default: break;
				}
				break;
			case 53: // Beholder
				switch (iDice(1, 3)) {
				case 1: if (iDice(1, 1000) == 20) iItemID = REP_SMALL; break;		// "RepPotion(+7)"	
				case 2: if (iDice(1, 1000) == 50) iItemID = REP_SMALL; break;		// "RepPotion(+3)"
				case 3: if (iDice(1, 100000) == 100) iItemID = 857; break;	// "I.M.CManual"
				default: break;
				}
				break;
			case 112://Newmobs
			case 113:
			case 115:
			case 116:
			case 117:
			case 118:
			case 119:
				switch (iDice(1, 9)) {
				case 1: if (iDice(1, 1000) == 30) iItemID = 942; break; //"RighStone"
				case 2: if (iDice(1, 1000) == 50) iItemID = 3733; break; //"ArmorDye(2) - Naranja"
				case 3: if (iDice(1, 1000) == 50) iItemID = 3734; break; //"ArmorDye(3) - Blanco"
				case 4: if (iDice(1, 1000) == 50) iItemID = 3737; break; //"ArmorDye(6) - Celeste"
				case 5: if (iDice(1, 1000) == 50) iItemID = 3738; break; //"ArmorDye(7) - Negro"
				case 6: if (iDice(1, 1000) == 50) iItemID = 3739; break; //"ArmorDye(8) - Amarillo"
				case 7: if (iDice(1, 1000) == 50) iItemID = 3740; break; //"ArmorDye(9) - Rosa"
				case 8: if (iDice(1, 1000) == 50) iItemID = 3741; break; //"ArmorDye(10) - Violeta"
				case 9: if (iDice(1, 1000) == 50) iItemID = 3742; break; //"ArmorDye(11) - Marron"
				default: break;
				}
				break;
			case 49: // Hellclaw
				if (Drop)
				{
					switch (iDice(1, 50)) {
						//case 1: iItemID = 620;	break;	// "MerienShield"
						//case 2: iItemID = 643; break;	// "KnecklaceOfIceEle"
					case 1: iItemID = 642; break;	// "KnecklaceOfIcePro"
					default:
						switch (iDice(1, 12))
						{
						case 1: iItemID = 259; break;	// "MagicWand(M.Shield)"
						case 2: iItemID = 311;	break;	// "MagicNecklace(DF+10)"
						case 3: iItemID = 308;	break;	// "MagicNecklace(MS10)"
						case 4: iItemID = 305;	break;	// "MagicNecklace(DM+1)"
						case 5: iItemID = 337;	break;	// "RubyRing"
						case 6: iItemID = 634; break;	// "RingofWizard"
						case 7: iItemID = 335; break;	// "EmeraldRing"
						case 8: iItemID = 3011;	break;	// "RepPotionRestore"
						case 9: iItemID = REP_SMALL; break;	// "rep+5"
						case 10: iItemID = 635; break;	// "RingOfMage" 
						case 11: iItemID = 636; break;	// "RingOfGrandMage" 
						case 12: iItemID = 642;	break;	// "KnecklaceOfIcePro"
						}
						break;
					}
				}
				break;
			case 50: // TigerWorm
				if (Drop)
				{
					switch (iDice(1, 50)) {
						//	case 1: iItemID = 637; break; // "NeckLightPro"
						//	case 2: iItemID = 643; break;// "knecklaceOfIceEle
					case 1: iItemID = 610; break; // "XelimaBlade"
					case 2: iItemID = 612; break; // "XelimaRapier"							
					case 3: iItemID = 611; break; // "XelimaAxe"
					case 4: iItemID = 642; break; // "KnecklaceOfIcePro"
					case 5: iItemID = 631; break; // "RingoftheAbaddon"
					case 6: iItemID = 638; break; // "NeckofFireProtec"
					case 7: iItemID = 291; break; // "MagicWand(MS30-LLF)"
						//case 11: iItemID = 857; break;// "I.M.CManual"
					default:
						switch (iDice(1, 12))
						{
						case 1: iItemID = 300;	break; // "MagicNecklace(RM10)"
						case 2: iItemID = 259;	break; // "MagicWand(M.Shield)"
						case 3: iItemID = 311;	break; // "MagicNecklace(DF+10)"
						case 4: iItemID = 308;	break; // "MagicNecklace(MS10)"
						case 5: iItemID = 305;	break; // "MagicNecklace(DM+1)"
						case 6: iItemID = 633; break; // "RingofDemonpower"
						case 7: iItemID = 630;	break; // "RingOfXelima"
						case 8: iItemID = 614;	break; // "SwordOfIceElemental"
						case 9: iItemID = 492; break; // "BloodRapier"
						case 10: iItemID = 20; break; // "Excaliber"
						case 11: iItemID = 491; break;// "BloodAxe"
						case 12: iItemID = 290; break;// "Flamberge+3LLF "
						}
						break;
					}
				}
				break;
			case 108: // HellDeath
				if (Drop)
				{
					switch (iDice(1, 10)) {
						//		case 1: iItemID = 849; break;		//	"KlonessBlade"
						//		case 2: iItemID = 762; break;		// "GiantBattleHammer"
						//		case 3: iItemID = 616; break;		//	"DemonSlayer"
						//		case 4: iItemID = 863; break;		//  "KlonessWand(MS.30)"
						//		case 5: iItemID = 848; break;		//	"LightingBlade"
						//		case 6: iItemID = 643; break;		//	"KnecklaceOfIceEle"
						//		case 7: iItemID = 611; break;		//  "XelimaAxe"
						//		case 8: iItemID = 4905; break;		//	"BlackHammer"
						//		case 9: iItemID = 861; break;		//	"BerserkWand(MS.30)
					case 1: iItemID = 4964; break;		//	"RingofPowerWar"
					case 2: iItemID = 4965; break;		//	"RingofPowerMage"
					case 3: iItemID = 4966; break;		//	"NeckPower(MS.30)"
					case 4: iItemID = 4968; break;		//	"NeckPowerXelima"
					case 5: iItemID = 4967; break;		//	"NeckPowerMerien"
					case 6: iItemID = CONTRIB_SMALL; break;		//	"Coins(+100)"
					default:
						switch (iDice(1, 6))
						{
						case 1: iItemID = 4964; break;		//	"RingofPowerWar"
						case 2: iItemID = 4965; break;		//	"RingofPowerMage"
						case 3: iItemID = 4966; break;		//	"NeckPower(MS.30)"
						case 4: iItemID = 4968; break;		//	"NeckPowerXelima"
						case 5: iItemID = 4967; break;		//	"NeckPowerMerien"
						case 6: iItemID = CONTRIB_SMALL; break;		//	"Coins(+100)"
						}
						break;
					}
				}
				break;
			case 109: // LightWorm
				if (Drop)
				{
					switch (iDice(1, 20)) {
						/*	case 1: iItemID = 4905; break;		//	"BlackHammer"
						case 2: iItemID = 861; break;		//	"BerserkWand(MS.30)
						case 3: iItemID = 850; break;		//	"KlonessAxe"
						case 4: iItemID = 849; break;		//	"KlonessBlade"
						case 5: iItemID = 850; break;		//	"KlonessAxe"
						case 6: iItemID = 848; break;		//	"LightingBlade"*/
						//		case 7: iItemID = 616; break;		//	"DemonSlayer"
						//		case 8: iItemID = 863; break;		//  "KlonessWand(MS.30)"
					case 1: iItemID = 4964; break;		//	RingofPowerWar
					case 2: iItemID = 4965; break;		//	RingofPowerMage
					case 3: iItemID = 611; break;		// "XelimaAxe"
					case 4: iItemID = 643; break;		// "knecklaceOfIceEle
					case 5: iItemID = CONTRIB_SMALL; break;		//	MerienShield
						//		case 14: iItemID = 762; break;		// "GiantBattleHammer"
						//		case 15: iItemID = 4905; break;		//	"BlackHammer"
						//		case 16: iItemID = 861; break;		//	"BerserkWand(MS.30)
					default:
						switch (iDice(1, 10))
						{
						case 1: iItemID = 4964; break;		//	RingofPowerWar
						case 2: iItemID = 4965; break;		//	RingofPowerMage
						case 3: iItemID = 4966; break;		//	NeckPower(MS.30)
						case 4: iItemID = 4967; break;		//	NeckPowerMerien
						case 5: iItemID = 4968; break;		//	NeckPowerXelima
						case 6: iItemID = CONTRIB_SMALL; break;		// "Coins(+500)"
						case 7: iItemID = 610; break;		// "XelimaBlade"
						case 8: iItemID = 611; break;		// "XelimaAxe"
						case 9: iItemID = 643; break;		// "knecklaceOfIceEle
						case 10: iItemID = CONTRIB_SMALL; break;		//	MerienShield
						}
						break;
					}
				}
				break;
			case 121: // GhostAba
				if (Drop)
				{
					
					switch (iDice(1, 1000)) {
					case 1: // 0.1% chance – Legendario
						switch (iDice(1, 200)) {
						case 1: iItemID = 849;  break; // KlonessBlade
						case 2: iItemID = 850; break; // KlonessAxe
						case 3: iItemID = 863;  break; // KlonessWand(MS.30)
						default:
							// Resto de legendarios dentro del mismo 0.1%
							switch (iDice(1, 2)) {
							case 1: iItemID = 866; break; // ResurWand(MS.10)
							case 2: iItemID = 857; break; // I.M.CManual
							}
							break;
						}
						break;

					default: {
						int roll = iDice(1, 1000);
						if (roll <= 10) { // 1.0% chance – Raro
							switch (iDice(1, 4)) {
							case 1: iItemID = 3083; break; // PowerBarbarian
							case 2: iItemID = 3084; break; // PowerRapier
							case 3: iItemID = 3102; break; // Coins(+20)
							case 4: iItemID = 3109; break; // RepPotion(+1000)
							}
						}
						else if (roll > 10 && roll < 300) { // 8.9% chance – Poco común
							switch (iDice(1, 3)) {
							case 1: iItemID = 4967; break; // NeckPowerMerien
							case 2: iItemID = 4968; break; // NeckPowerXelima
							case 3: iItemID = 4966; break; // NeckPower(MS.30)
							}
						}
						else { // ~90.1% chance – Común
							switch (iDice(1, 6)) {
							case 1: iItemID = 860;         break; // NecklaceOfXelima
							case 2: iItemID = 612;         break; // XelimaRapier
							case 3: iItemID = 611;         break; // XelimaAxe
							case 4: iItemID = 614;         break; // SwordofIceElemental
							case 5: iItemID = COIN_SMALL;  break; // Coins (1–3)
							case 6: iItemID = 858;         break; // NecklaceOfMerien
							}
						}
					}
						   break;

					}


				}
				break;

			case 66: // Wyvern 
				if (Drop == TRUE) {
					bGetMultipleItemNamesWhenDeleteNpc(m_pNpcList[iNpcH]->m_sType, iItemprobability,
						3, 9, m_pNpcList[iNpcH]->m_sX, m_pNpcList[iNpcH]->m_sY, DEF_ITEMSPREAD_FIXED, 4,
						iItemIDs, ItemPositions, &iNumItem);
				}
				break;
			case 73: // Fire-Wyvern
				if (Drop == TRUE) {
					bGetMultipleItemNamesWhenDeleteNpc(m_pNpcList[iNpcH]->m_sType, iItemprobability,
						3, 9, m_pNpcList[iNpcH]->m_sX, m_pNpcList[iNpcH]->m_sY, DEF_ITEMSPREAD_FIXED, 4,
						iItemIDs, ItemPositions, &iNumItem);
				}
				break;
			case 81: //Abaddon
				if (Drop == TRUE) {
					bGetMultipleItemNamesWhenDeleteNpc(m_pNpcList[iNpcH]->m_sType, iItemprobability,
						6, 12, m_pNpcList[iNpcH]->m_sX, m_pNpcList[iNpcH]->m_sY, DEF_ITEMSPREAD_FIXED, 65,
						iItemIDs, ItemPositions, &iNumItem);
				}
				break;
			case 128: // BlackWyvern
			case 114: // DeathWyvern 
				coinMin = 10;
				coinMax = 35;
				if (Drop == TRUE) {
					bGetMultipleItemNamesWhenDeleteNpc(m_pNpcList[iNpcH]->m_sType, iItemprobability,
						3, 9, m_pNpcList[iNpcH]->m_sX, m_pNpcList[iNpcH]->m_sY, DEF_ITEMSPREAD_FIXED, 4,
						iItemIDs, ItemPositions, &iNumItem);
				}
				break;
			case 120: // HellAbaddon 
				if (Drop == TRUE) {
					bGetMultipleItemNamesWhenDeleteNpc(m_pNpcList[iNpcH]->m_sType, iItemprobability,
						6, 12, m_pNpcList[iNpcH]->m_sX, m_pNpcList[iNpcH]->m_sY, DEF_ITEMSPREAD_FIXED, 65,
						iItemIDs, ItemPositions, &iNumItem);
				}
				break;


			case 33://WereWolf


				switch (iDice(1, 15)) {
				case 1: if (iDice(1, 30) == 3) iItemID = 551; break;		// "WerewolfTail"
				case 2: if (iDice(1, 25) == 3) iItemID = 548; break;		// "WerewolfHeart"
				case 3: if (iDice(1, 25) == 3) iItemID = 550; break;		// "WerewolfMeat"
				case 4: if (iDice(1, 35) == 3) iItemID = 553; break;		// "WerewolfLeather"
				case 5: if (iDice(1, 25) == 3) iItemID = 552; break;		// "WerewolfTeeth"
				case 6: if (iDice(1, 25) == 3) iItemID = 554; break;		// "WerewolfClaw"
				case 7: if (iDice(1, 35) == 3) iItemID = 549; break;		// "WerewolfNail"
				case 8: if (iDice(1, 1000) == 50) iItemID = 656; break;		// "StoneOfXelima"
				case 9: if (iDice(1, 1000) == 50) iItemID = 657; break;		// "StoneOfMerien"
				case 10: if (iDice(1, 1000) == 50) iItemID = 650; break;		// "ZemstoneofSacrifice"
				case 11: if (iDice(1, 25000) == 8539) iItemID = 621; break;		// "MerienPlateMailW"
				case 12: if (iDice(1, 25000) == 8539) iItemID = 622; break;		// "MerienPlateMailM"
				case 13: if (iDice(1, 25000) == 8539) iItemID = 3243; break;	// "MerienChainMailM"
				case 14: if (iDice(1, 25000) == 8539) iItemID = 3244; break;	// "MerienChainMailW"

				case 15:
					if (iDice(1, 200) == 1 && Drop == TRUE) { // 0.5% de probabilidad que salga un coin
						//Drop de Coins
						bGetMultipleItemNamesWhenDeleteNpc(m_pNpcList[iNpcH]->m_sType, iItemprobability, 6, 1, m_pNpcList[iNpcH]->m_sX, m_pNpcList[iNpcH]->m_sY, DEF_ITEMSPREAD_FIXED, 65, iItemIDs, ItemPositions, &iNumItem, iItemCounts);
					}
					break;

				default: break;
				}
				break;


			}

			dwCount = 1;

			if (iItemID == 0) {
				switch (m_pNpcList[iNpcH]->m_sType) {
				case 34:
					iItemID = 90;
					dwCount = (DWORD)(iDice(1, m_pNpcList[iNpcH]->m_iExp * 4) + m_pNpcList[iNpcH]->m_iExp);
					dwCount = dwCount - (dwCount / 3);
					break;
				}
			}
			if (iNumItem > 0) {
				GetLocalTime(&SysTime);
				wsprintf(cTemp, "%d%02d%", SysTime.wMonth, SysTime.wDay);
				for (int j = 0; j < iNumItem; j++) {
					if (pItem == NULL) pItem = new class CItem;


				
					//std::cout << "Item sin procesar: " << COIN_BIG << " Item Recibido " << iItemIDs[j] << std::endl;
					//pItem = new class CItem;

					
					itemType = iItemIDs[j];



					switch (iItemIDs[j])
					{
					
						case COIN_SMALL:
						case COIN_MEDIUM:
						case COIN_BIG:
						case CUSTOM_COIN:
							iItemIDs[j] = 3053;
						break;

						case CONTRIB_SMALL:
						case CONTRIB_MEDIUM:
						case CONTRIB_BIG:
							iItemIDs[j] = 3052;
							break;

						case REP_SMALL:
						case REP_MEDIUM:
						case REP_BIG:
							iItemIDs[j] = 3054;
							break;

						case MAJ_MEDIUM:
							iItemIDs[j] = 3055;
							break;


						default:
							//iItemIDs[j] = 90;
						break;
					}

					

					//std::cout << "Item procesado " << iItemIDs[j] << " ItemType: " << coinType << std::endl;
					

					if (_bInitItemAttr(pItem, iItemIDs[j]) != FALSE &&
						m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->bGetIsMoveAllowedTile((short)ItemPositions[j].x, (short)ItemPositions[j].y) != FALSE)
					{

					//	std::cout << "Item recibo despues de procesar: " << iItemIDs[j] << std::endl;





						switch (iItemIDs[j]) {
							case 3053: // Item 3053 ==  Coin
								switch (itemType)
								{
									case COIN_SMALL:
										pItem->m_dwCount = RollDice(1, 3);
									break;
									case COIN_MEDIUM:
										pItem->m_dwCount = RollDice(5, 10);
										break;
									case COIN_BIG:
										pItem->m_dwCount = RollDice(20, 40);
										break;
									default:
										pItem->m_dwCount = RollDice(coinMin, coinMax);
									break;
								}
								iItemIDs[j] = 3053;
							break;

							case 3052: // Item 3052 ==  Contrib
								switch (itemType)
								{
								case CONTRIB_SMALL:
									pItem->m_dwCount = RollDice(200, 1000);
									break;
								case CONTRIB_MEDIUM:
									pItem->m_dwCount = RollDice(5000, 50000);
									break;
								case CONTRIB_BIG:
									pItem->m_dwCount = RollDice(50000, 100000);
									break;
								default:
									pItem->m_dwCount = RollDice(coinMin, coinMax);
									break;
								}
								iItemIDs[j] = 3052;
								break;


							case 3055: // Item 3055 ==  Majestic
								switch (itemType)
								{
								case MAJ_SMALL:
									pItem->m_dwCount = RollDice(5, 100);
									break;
								case MAJ_MEDIUM:
									pItem->m_dwCount = RollDice(100, 500);
									break;
								case MAJ_BIG:
									pItem->m_dwCount = RollDice(1000, 5000);
									break;
								default:
									pItem->m_dwCount = RollDice(coinMin, coinMax);
									break;
								}
								iItemIDs[j] = 3052;
								break;
							default:
								
								if (iItemIDs[j] == 90) pItem->m_dwCount = iDice(10, 15000);
								else pItem->m_dwCount = dwCount;
							break;

						}


						pItem->m_sTouchEffectType = DEF_ITET_ID;
						pItem->m_sTouchEffectValue1 = iDice(1, 100000);

						// Anti Dup System
						SYSTEMTIME SysTime;
						char cTemp[256];
						GetLocalTime(&SysTime);
						ZeroMemory(cTemp, sizeof(cTemp));
						pItem->m_sTouchEffectValue2 = iDice(1, 100000);
						wsprintf(cTemp, "%02d%02d", (short)SysTime.wMonth, (short)SysTime.wDay);
						pItem->m_sTouchEffectValue2 = atoi(cTemp);

						ZeroMemory(cTemp, sizeof(cTemp));
						wsprintf(cTemp, "%02d%02d%02d%d", (short)SysTime.wHour, (short)SysTime.wMinute, (short)SysTime.wSecond, (short)SysTime.wMilliseconds);
						pItem->m_sTouchEffectValue3 = atoi(cTemp);

						m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->bSetItem((short)ItemPositions[j].x, (short)ItemPositions[j].y, pItem);
						pItem->m_dwTimeDrop = timeGetTime();
						SendEventToNearClient_TypeB(CLIENT_COMMON_EVENT, CLIENT_COMMON_ITEMDROP, m_pNpcList[iNpcH]->m_cMapIndex,
							(short)ItemPositions[j].x, (short)ItemPositions[j].y, pItem->m_sSprite, pItem->m_sSpriteFrame, pItem->m_cItemColor);
						_bItemLog(DEF_ITEMLOG_NEWGENDROP, NULL, m_pNpcList[iNpcH]->m_cNpcName, pItem);
						pItem = NULL;

					}
					else {
						delete pItem;
						pItem = NULL;

					}
				}
			}
			else {

				//pItem = new class CItem;
				if (iItemID == CONTRIB_SMALL)
				{
					iItemID = 3052;
				}
				else if (iItemID == MAJ_SMALL)
				{
					iItemID = 3055;					
				}
				else if (iItemID == REP_SMALL)
				{
					iItemID = 3054;					
				}
				
				if (_bInitItemAttr(pItem, iItemID) != FALSE) {

					if (iItemID == 3052) //contrib
					{
						pItem->m_dwCount = RollDice(500, 750);
					}
					else if (iItemID == 3055) //maj
					{
						pItem->m_dwCount = RollDice(5, 20);
					}
					else if (iItemID == 3054) //rep
					{
						pItem->m_dwCount = RollDice(1, 3);
					}
					else
					{
						pItem->m_dwCount = dwCount;
					}					

					pItem->m_sTouchEffectType = DEF_ITET_ID;
					pItem->m_sTouchEffectValue1 = iDice(1, 10000);

					// Anti Dup System
					SYSTEMTIME SysTime;
					char cTemp[256];
					GetLocalTime(&SysTime);
					ZeroMemory(cTemp, sizeof(cTemp));
					pItem->m_sTouchEffectValue2 = iDice(1, 100000);
					wsprintf(cTemp, "%02d%02d", (short)SysTime.wMonth, (short)SysTime.wDay);
					pItem->m_sTouchEffectValue2 = atoi(cTemp);

					ZeroMemory(cTemp, sizeof(cTemp));
					wsprintf(cTemp, "%02d%02d%02d%d", (short)SysTime.wHour, (short)SysTime.wMinute, (short)SysTime.wSecond, (short)SysTime.wMilliseconds);
					pItem->m_sTouchEffectValue3 = atoi(cTemp);

					m_pMapList[m_pNpcList[iNpcH]->m_cMapIndex]->bSetItem(m_pNpcList[iNpcH]->m_sX, m_pNpcList[iNpcH]->m_sY, pItem);
					pItem->m_dwTimeDrop = timeGetTime();

					SendEventToNearClient_TypeB(CLIENT_COMMON_EVENT, CLIENT_COMMON_ITEMDROP, m_pNpcList[iNpcH]->m_cMapIndex,
						m_pNpcList[iNpcH]->m_sX, m_pNpcList[iNpcH]->m_sY,
						pItem->m_sSprite, pItem->m_sSpriteFrame, pItem->m_cItemColor); // v1.4 color

					_bItemLog(DEF_ITEMLOG_NEWGENDROP, 0, m_pNpcList[iNpcH]->m_cNpcName, pItem);
				}
				else {
					delete pItem;
					pItem = NULL;
				}
			}
		}
#ifdef DEF_DEBUG
	}
	catch (...) {
		ErrorList("Crash Evitado en: DeleteNpc");
	}
#endif

	delete m_pNpcList[iNpcH];
	m_pNpcList[iNpcH] = NULL;
}




BOOL CMapServer::bGetMultipleItemNamesWhenDeleteNpc(short sNpcType, int iProbability, int iMin, int iMax, short sBaseX, short sBaseY,
	int iItemSpreadType, int iSpreadRange, int *iItemIDs, POINT *BasePos, int *iNumItem, int *iItemCounts)

{
#ifdef DEF_DEBUG
	try {
#endif


		int		iProb = 100;
		float	fProb, fProbA, fProbB, fProbC;
		int		iItemID;
		int		iNum = 0;
		bool bCustomDrop = false;

		for (int i = 0; i < iMax; i++) {
			if (i > iMin) iProb = iProbability;

			fProb = (float)((100 - iProb) / 10.0);
			if (fProb < 1.0) fProb = 1.0;

			fProbA = fProbB = fProbC = fProb;

			fProbA = fProbA * 8.0f;
			fProbB = fProbB * 4.0f;
			fProbC = fProbC;

			iItemID = 0;

			switch (sNpcType) {
			case 33: // WereWolf
				iItemID = COIN_SMALL;
			break;
			case 66: // Wyvern
				switch (iDice(1, 200)) {
				case 1: iItemID = 642; break;	// "KnecklaceOfIcePro"
				case 2: iItemID = 734; break;	// "RingofArcmage"
				case 3: iItemID = 847; break;	// "DarkExecutor"
					//		case 4: iItemID = 380; break;	// "IceStormManual""
				default:
					switch (iDice(1, 22))
					{
					case 1: iItemID = 300;	break;  // "MagicNecklace(RM10)"
					case 2: iItemID = 259;	break;  // "MagicWand(M.Shield)"	
					case 3: iItemID = 311;	break;  // "MagicNecklace(DF+10)"	
					case 4: iItemID = 308;	break;  // "MagicNecklace(MS10)"	
					case 5: iItemID = 305;	break;  // "MagicNecklace(DM+1)"		
					case 6: iItemID = 3012;	break;  // "RepPotion+1"		
					case 7: iItemID = 3013;	break;  // "RepPotion+3"
					case 8: iItemID = 634; break;	// "RingofWizard"	
					case 9: iItemID = 636; break;	// "RingofGrandMage"	
					case 10: iItemID = 337; break;	// "RubyRing"
					case 11: iItemID = 656; break;	// "StoneOfXelima"	
					case 12: iItemID = 657; break;	// "StoneOfMerien"	
					case 13: iItemID = 650; break;	// "ZemstoneOfSacrifice"
					case 14: iItemID = 273; break;	// "InviPotion"
					case 15: iItemID = 840; break;	// "PowerRedPotion"
					case 16: iItemID = 841; break;	// "PowerBluePotion"
					case 17: iItemID = 335; break;	// "EmeraldRing"
					case 18: iItemID = 492;	break;	// "BloodRapier"
					case 19: iItemID = 614;	break;	// "SwordofIceElemental"
					case 20: iItemID = 290;	break;	// "Flameberge+3(LLF)"
					case 21: iItemID = REP_MEDIUM; break;	// "CriticalBall(+1000)"
					case 22: iItemID = CONTRIB_MEDIUM; break;	// "ContribBall(+1000)"
					}
					break;
				}
				break;
			case 73: // Fire-Wyvern
				switch (iDice(1, 60)) {
				case 1: iItemID = 860; break;	// "NecklaceOfXelima"
				case 2: iItemID = 631; break;	// "RingoftheAbaddon"
				case 3: iItemID = 847; break;	// "DarkExecutor"
				default:
					switch (iDice(1, 22))
					{
					case 1: iItemID = 308;	break;		// "MagicNecklace(MS10)"	
					case 2: iItemID = 305;	break;		// "MagicNecklace(DM+1)"
					case 3: iItemID = 634; break;		// "RingofWizard"
					case 4: iItemID = 650; break;		// "RubyRing"
					case 5: iItemID = 636; break;		// "RingofGrandMage"
					case 6: iItemID = 656; break;		// "StoneOfXelima"	
					case 7: iItemID = 657; break;		// "StoneOfMerien"	
					case 8: iItemID = 650; break;		// "RingofDragonpower"
					case 9: iItemID = 650; break;		// "RingoftheXelima"
					case 10: iItemID = 3014; break;		// "RepPotion(+5)"
					case 11: iItemID = 492;	break;	// "BloodRapier"
					case 12: iItemID = 20;	break;	// "Excaliber"
					case 13: iItemID = 490;	break;	// "BloodSword"
					case 14: iItemID = 491;	break;	// "BloodAxe"	
					case 15: iItemID = 734; break;	// "RingofArcmage"
					case 16: iItemID = 638;	break;	// "KnecklaceOfFirePro"
					case 17: iItemID = 645;	break;	// "KnecklaceOfEfreet"
					case 18: iItemID = 382; break;	// "BloodyShockW.Manual"
					case 19: iItemID = 381; break;	// "MassFireStrikeManual"
					case 20: iItemID = 3025; break; // "ContribBall(+5000)"
					case 21: iItemID = 3024; break;		// "ContribBall(+1000)"
					case 22: iItemID = 3115; break;		// "CriticalBall(+1000)"
					}
					break;
				}
				break;
			case 114: // DeathWyvern
				switch (iDice(1, 20)) {
				case 1: iItemID = 308;	break;		// "MagicNecklace(MS10)"	
				case 2: iItemID = 305;	break;		// "MagicNecklace(DM+1)"
				case 3: iItemID = 634; break;		// "RingofWizard"
				case 4: iItemID = 337; break;		// "RubyRing"
				case 5: iItemID = 636; break;		// "RingofGrandMage"
				case 6: iItemID = 656; break;		// "StoneOfXelima"	
				case 7: iItemID = 657; break;		// "StoneOfMerien"	
				case 8: iItemID = 735; break;		// "RingofDragonpower"
				case 9: iItemID = 630; break;		// "RingoftheXelima"
				case 10: iItemID = CUSTOM_COIN; break;	// "Coins(+100)"
				default:
					switch (iDice(1, 5))
					{
					case 1: iItemID = 3087; break;  //"	RepPotion(+100)"
					case 2: iItemID = 3732; break;  //"ArmorDye(1)Verde"
					case 3: iItemID = 3735; break;  //"ArmorDye(4)Rojo"
					case 4: iItemID = 3736; break;  //"ArmorDye(5)Azul"
					case 5: iItemID = 3025; break;  //"ContribBall(+10000)"
					}
					break;
				}
				break;
			case 128: // BlackWyvern
				switch (iDice(1, 2000)) {
				case 1:
				case 2:
					switch (iDice(1, 200)) {
						case 1: iItemID = 850; break;		//	"KlonessAxe
						case 2: iItemID = 849; break;		//	"KlonessBlade"
						default:
							iItemID = CUSTOM_COIN;
						break;
					}
					break;

				case 3: iItemID = 621; break;		// "MerienPlateMailW"
				case 4: iItemID = 622; break;		// "MerienPlateMailM"
				case 5: iItemID = 3243; break;		// "MerienChainMailM"
				case 6: iItemID = 3244; break;		// "MerienChainMailW"	
				case 7: iItemID = 4964; break;	//	RingofPowerWar
				case 8: iItemID = 4965; break;	//	RingofPowerMage
				case 9: iItemID = 4966; break;	//	NeckPower(MS.30)
				case 10: iItemID = 4967; break;	//	NeckPowerMerien
				case 11: iItemID = 4968; break;	//	NeckPowerXelima
				default:
					switch (iDice(1, 10))
					{
					case 1: iItemID = 308;	break;		// "MagicNecklace(MS10)"	
					case 2: iItemID = 305;	break;		// "MagicNecklace(DM+1)"
					case 3: iItemID = 634; break;		// "RingofWizard"
					case 4: iItemID = 337; break;		// "RubyRing"
					case 5: iItemID = 636; break;		// "RingofGrandMage"
					case 6: iItemID = 656; break;		// "StoneOfXelima"	
					case 7: iItemID = 610; break;	// "XelimaBlade"
					case 8: iItemID = 611; break;	// "XelimaAxe"


					case 9: 
						switch (iDice(1,250))
						{
						case 1: iItemID = 620; break;		//	MerienShield
						case 2: iItemID = 616; break;		//	"DemonSlayer"
						case 3: iItemID = 643; break;		// "knecklaceOfIceEle 
						default:
							iItemID = COIN_MEDIUM;
							break;
						}
						break;
					case 10:
						switch (iDice(1, 2000)) {
						case 1: iItemID = 848; break;		//	"LightingBlade"
						case 2: iItemID = 863; break;		//  "KlonessWand(MS.30)"
						case 3: iItemID = 762; break;		// "GiantBattleHammer"
						default:
							iItemID = CUSTOM_COIN;
							break;
						}
						break;

					

					}
					break;
				}
				break;
			case 120: // HellAbaddon
				switch (iDice(1, 1000)) {
				case 1: // 0.1% chance - Legendario
					switch (iDice(1, 200)) {
					case 1: iItemID = 3116; break; // BlackZWand(MS.32)
					case 2: iItemID = 4913; break;   // BlackDevastator
					case 3: iItemID = 4969; break;  // StripIceHammer

					default:
						switch (iDice(1, 3))
						{
						case 1:
							iItemID = 762; // Giant Battle Hammer
							break;
						case 2:
							iItemID = 848; // LightingBlade
							break;

						case 3:
							iItemID = 861; // BerserkWand(MS.30)
						break;
						}

					break;
					
					}
					break;
				default:
				{
					int roll = iDice(1, 1000);
					if (roll <= 10) { // 12.9% chance - Raro
						switch (iDice(1, 5)) {
						case 1: iItemID = 4959; break; //AresElfBow
						case 2: iItemID = 4960; break; //ElvinElfBow
						case 3: iItemID = 620; break; //MerienShield
						case 4: iItemID = 865; break; //ResurWand(MS.30
						case 5: iItemID = 866; break; //ResurWand(MS.10)
						}
					}
					else if (roll > 10 && roll < 100) {

						iItemID = COIN_BIG;

					}
					else if (roll > 100 && roll < 150) {
						iItemID = 616; // DemonSlayer
					}
					else { // Común
						switch (iDice(1, 7)) {
						case 1: iItemID = 4968; break; //NeckPowerXelima
						case 2: iItemID = 4966; break; //NeckPower(MS.30)
						case 3: iItemID = 3109; break; //RepPotion(+1000)
						case 4: iItemID = 4964; break; //RingofPowerWar
						case 5: iItemID = 4965; break; //RingofPowerMage
						case 6: iItemID = COIN_MEDIUM; break; //COIN_MEDIUM (5 a 10)
						case 7: iItemID = COIN_MEDIUM; break; //COIN_MEDIUM (5 a 10)
						}
					}
				}
				break;

				}
				//FIXED
				if (iItemID == 0) {
					switch (iDice(1, 3)) {
					case 1: iItemID = 650; break; // ZemstoneOfSacrifice
					case 2: iItemID = 656; break; // StoneOfXelima
					case 3: iItemID = 657; break; // StoneOfMerien
					}
				}
				break;


				/*	Item = 3107	Coins(+1000) - Item = 3108	Coins(+5000) - Item = 3114	Coins(+10000)*/
			case 81: // Abaddon
				switch (iDice(1, 1000)) {
				case 1: // 0.1% chance - Legendario
					switch (iDice(1, 200)) {
					case 1: iItemID = 861; break; // BerserkWand(MS.30)
					case 2: iItemID = 846; break;   // Devas
					case 3: iItemID = 845; break;  // StormBringer

					default: 
						
						switch (iDice(1,2))
						{
						case 1:
							iItemID = 849;
							break;
						case 2:
							iItemID = 863;
							break;
						}
						
						break; 
					}
					break;
				default:
				{
						   int roll = iDice(1, 1000);
						   if (roll <= 10) { // 12.9% chance - Raro
							   switch (iDice(1, 5)) {
									case 1: iItemID = 849; break; //KlonessBlade
									case 2: iItemID = 850; break; //KlonessAxe
									case 3: iItemID = 863; break; //KlonessWand(MS.30)
									case 4: iItemID = 864; break; //KlonessWand(MS.10)
									case 5: iItemID = 851; break; //KlonessEsterk
							   }
						   }
						   else if (roll > 10 && roll < 100) {
							  
							   iItemID = COIN_MEDIUM; 
							   
						   }
						   else if (roll > 100 && roll < 105){
							   iItemID = 616; // DemonSlayer
						   }
						   else { // Común
							   switch (iDice(1, 8)) {
							   case 1: iItemID = COIN_SMALL; break; //Coins(+5000)
							   case 2: iItemID = COIN_SMALL; break; //Coins(+1000)
							   case 3: iItemID = 3087; break; //RepPotion(+100)
							   case 4: iItemID = COIN_SMALL; break; //Coins(+10000)
							   case 5: iItemID = 3081; break; //ContribBall(+100000)
							   case 6: iItemID = 3090; break; //TintaMagicaRoja
							   case 7: iItemID = 3089; break; //TintaMagica
							   case 8: iItemID = COIN_SMALL; break; //Coins(+20000)
							   }
						   }
				}
					break;

			}
				//FIXED
				if (iItemID == 0) {
					switch (iDice(1, 3)) {
					case 1: iItemID = 650; break; // ZemstoneOfSacrifice
					case 2: iItemID = 656; break; // StoneOfXelima
					case 3: iItemID = 657; break; // StoneOfMerien
					}
				}
				break;
			}


			if (bCustomDrop) {
				switch (iItemSpreadType) {
				case DEF_ITEMSPREAD_RANDOM:
					BasePos[iNum - 1].x = sBaseX + iSpreadRange - iDice(1, iSpreadRange * 2);
					BasePos[iNum - 1].y = sBaseY + iSpreadRange - iDice(1, iSpreadRange * 2);
					break;
				case DEF_ITEMSPREAD_FIXED:
					BasePos[iNum - 1].x = sBaseX + ITEMSPREAD_FIEXD_COORD[iNum - 1][0];
					BasePos[iNum - 1].y = sBaseY + ITEMSPREAD_FIEXD_COORD[iNum - 1][1];
					break;
				}

				*iNumItem = iNum;   
				return TRUE; 
			}


			// Gold 
			//if (iItemID == 0) iItemID = 90; // Gold
			if (iItemID == 0 && iNum == 0) {
				iItemID = 90;
			}

			//std::cout << "iItemID: " << iItemID << " iNum: " << iNum << std::endl;

			if (iItemID != 0 || iNum != 0) {

				if (iItemID != 0) iItemIDs[iNum] = iItemID;

				switch (iItemSpreadType) {
				case DEF_ITEMSPREAD_RANDOM:
					BasePos[iNum].x = sBaseX + iSpreadRange - iDice(1, iSpreadRange * 2);
					BasePos[iNum].y = sBaseY + iSpreadRange - iDice(1, iSpreadRange * 2);
					break;

				case DEF_ITEMSPREAD_FIXED:
					BasePos[iNum].x = sBaseX + ITEMSPREAD_FIEXD_COORD[iNum][0];
					BasePos[iNum].y = sBaseY + ITEMSPREAD_FIEXD_COORD[iNum][1];
					break;
				}
				iNum++;
			}
			else {
			}

		} // for
		*iNumItem = iNum;


#ifdef DEF_DEBUG
	}
	catch (...) {
		ErrorList("Crash Evitado en: bGetMultipleItemNamesWhenDeleteNpc");
	}
#endif


	return TRUE;
}
bool CMapServer::bCheckClientMoveFrequency(int client, BOOL running)
{
	DWORD dwTimeGap;
	DWORD dwTime = timeGetTime();

	CClient * player = m_pClientList[client];
	if (player == NULL) return FALSE;

	if (player->m_iAdminUserLevel > 0) return TRUE;

	/*if (player->m_dwMoveFreqTime == NULL)
		player->m_dwMoveFreqTime = dwTime;
	else {
		if (player->m_bIsMoveBlocked == TRUE) {
			player->m_dwMoveFreqTime = NULL;
			player->m_bIsMoveBlocked = FALSE;
			return TRUE;
		}

		if (player->m_bIsAttackModeChange == TRUE) {
			player->m_dwMoveFreqTime = NULL;
			player->m_bIsAttackModeChange = FALSE;
			return TRUE;
		}

		dwTimeGap = dwTime - player->m_dwMoveFreqTime;
		player->m_dwMoveFreqTime = dwTime;

		// Mostrar el tiempo de movimiento
		if (running)
		{
			player->m_runTime[player->m_runTurn] = dwTimeGap;

			uint32 sum = 0;
			for (int i = 0; i < 7; i++) {
				sum += player->m_runTime[i];
			}

			// Imprimir el tiempo de movimiento en caso de que el jugador esté corriendo
			wsprintf(G_cTxt, "Player %s ran with time gap: %d ms", player->m_cCharName, dwTimeGap);
			PutLogList(G_cTxt);

			if (sum < 200 * 7)
			{
				wsprintf(G_cTxt, "(-~-HACKING-~-) Speed hacker detected(%s) - run-avg(%i). BI banned", player->m_cCharName, sum / 7);
				PutLogList(G_cTxt);
			}
			else if (sum < 220 * 7)
			{
				wsprintf(G_cTxt, "(-~-HACKING-~-) Speed hack suspect(%s) - run-avg(%i)", player->m_cCharName, sum / 7);
				PutLogList(G_cTxt);
				++player->m_runTurn %= 7;
			}
			++player->m_runTurn %= 7;
		}
		else {
			player->m_moveTime[player->m_moveTurn] = dwTimeGap;

			uint32 sum = 0;
			for (int i = 0; i < 7; i++)
				sum += player->m_moveTime[i];

			// Imprimir el tiempo de movimiento en caso de que el jugador no esté corriendo
			wsprintf(G_cTxt, "Player %s moved with time gap: %d ms", player->m_cCharName, dwTimeGap);
			PutLogList(G_cTxt);

			if (sum < 300 * 7)
			{
				wsprintf(G_cTxt, "(-~-HACKING-~-) Speed hacker detected(%s) - move-avg(%i). BI banned", player->m_cCharName, sum / 7);
				PutLogList(G_cTxt);
			}
			else if (sum < 400 * 7)
			{
				wsprintf(G_cTxt, "(-~-HACKING-~-) Speed hack suspect(%s) - move-avg(%i)", player->m_cCharName, sum / 7);
				PutLogList(G_cTxt);
				++player->m_moveTurn %= 7;
			}
			++player->m_moveTurn %= 7;
		}
	}

	return TRUE;*/
	return FALSE;
}
