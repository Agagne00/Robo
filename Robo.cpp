#include <iostream>
#include <string>
#include <ctime>
#include <cstdlib>
using namespace std;
string version = "1.02"; 
// 1.01: changing level curve from level*level+level to level*level*2.  no change to monster experience.
// 1.02: Player now starts with Poke as a move.  That should get them through the first Goblin fight.

class Move
{
public:
	std::string name;
	unsigned short memory;
	unsigned short damage;
	unsigned short range;
	short cost;
	double scale;
	Move() {
		name = "default";
		memory = 0;
		damage = 0;
		range = 0;
		cost = 0;
		scale = 0;
	}
	Move(std::string moveName, unsigned short mem, unsigned short dmg, unsigned short rng, short cst, double scl) {
		name = moveName;
		memory = mem;
		damage = dmg;
		range = rng;
		cost = cst;
		scale = scl;
	}
	short calculateDamage(unsigned short strength) {
		return short(rand() % (range + 1) + damage + (strength * scale));
	}
};


class Monster
{
public:
	std::string name;
	short moves[8];
	short maxHP, str, exp;
	Monster() {
		name = "default";
		moves[0] = 0;
		moves[1] = 0;
		moves[2] = 0;
		moves[3] = 0;
		moves[4] = 0;
		moves[5] = 0;
		moves[6] = 0;
		moves[7] = 0;
		maxHP = 0;
		str = 0;
		exp = 0;
	}
	Monster(std::string monName, short move1, short move2, short move3, short move4, short move5, short move6, short move7, short move8, short HP, short strength, short XP) {
		name = monName;
		moves[0] = move1;
		moves[1] = move2;
		moves[2] = move3;
		moves[3] = move4;
		moves[4] = move5;
		moves[5] = move6;
		moves[6] = move7;
		moves[7] = move8;
		maxHP = HP;
		str = strength;
		exp = XP;
	}
	short pickMove() {
		return moves[rand() % 8];
	}
};
unsigned short currXP, level, str, maxHP, maxMag, currMag; //memory = level?
short currHP;
short alive = 1;
short monKilled = 0;
const short LEARN_COUNT = 3;
const short MAX_MOVES = 8;
short yourMoveList[MAX_MOVES] = { 4,-1,-1,-1,-1,-1,-1,-1 };
Move moveList[31];
Monster monsterList[19];
unsigned short learnedList[31] = { 0,0,0,0,LEARN_COUNT,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }; // start with move 4, the move Poke.
short zone1List[8] = {1,1,1,1,2,2,3,4}; // put Monsters into appropriate Zones.
short zone2List[8] = {2,4,4,5,5,6,6,7}; // Zones are Goblin Cave, Cursed Swamp, Haunted Ruins, Master Dojo, and Black Keep.
short zone3List[8] = {8,8,9,9,9,11,11,10}; // Cave is simple early stuff, Swamp has more magic, Ruins has more blades, 
short zone4List[8] = {12,12,12,14,14,15,15,13}; // Dojo has more wrestling, Keep has a high-level mix.
short zone5List[8] = {16,16,16,17,17,17,18,0};
short nextFight;

short calcRandom(short list[]) { // general randomizer function, takes in a pointer to an 8-element list, returns a random element in the list.
	short r = rand() % 8;
	return list[r];
}

void checkLevel() {
	if (currXP >= (level*level*2)) { // considering changing level formula to 2*level*level, so later levels don't come so easily.  Gaining 2+ levels at a time should not be common.
		cout << "Gained a level!" << endl << "Robo has achieved level " << level+1 << "." << endl << "HP and Magic restored, and HP raised by 10." << endl << "Choose a stat to raise." << endl;
		cout << "1: HP    2: Strength    3: Magic" << endl;
		maxHP += 10;
		int option = 0;
		cin >> option;
		while (option != 1 && option != 2 && option != 3) {
			cout << "Invalid choice." << endl;
			cin >> option;
		}
		switch (option) {
		case 1: cout << "HP raised by 10." << endl;
			maxHP += 10;
			break;
		case 2: cout << "Strength raised by 2." << endl;
			str += 2;
			break;
		case 3: cout << "Magic raised by 4." << endl;
			maxMag += 4;
			break;
		}
		currHP = maxHP;
		currMag = maxMag;
		level += 1;
		checkLevel(); // in case of multiple levels gained in one fight
	}
}

void encounter(short monster) {
	//initialize monster stats
	short monHP = monsterList[monster].maxHP;
	int run = 0;
	cout << "You encounter " << monsterList[monster].name << "!" << endl;
	while (monHP > 0 && currHP > 0 && run == 0) {
		int option = 0;
		cout << "HP: " << currHP << "/" << maxHP << "    " << "Magic: " << currMag << "/" << maxMag << "    " << "Strength: " << str << endl << "Level: " << level << "    Exp: " << currXP << endl;
		cout << "Choose a move:" << endl;
		for (int i = 0; i <= MAX_MOVES-1; i++) {
			if (yourMoveList[i] != -1) cout << i+1 << ": " << moveList[yourMoveList[i]].name << "    Damage: " << moveList[yourMoveList[i]].damage << " to " << moveList[yourMoveList[i]].damage + moveList[yourMoveList[i]].range << " Memory: " << moveList[yourMoveList[i]].memory << " Magic Cost: " << moveList[yourMoveList[i]].cost << " Scaling: " << moveList[yourMoveList[i]].scale << endl;
		}
		cout << "9: Run away" << endl << "0: Wait" << endl;
		cin >> option;
		if (option >= 1 && option <= 8) {
			if (yourMoveList[option-1] == -1) cout << "Invalid selection, pick something else." << endl;
			else {
				if (moveList[yourMoveList[option-1]].cost > currMag) cout << "You don't have enough Magic!" << endl;
				else {
					cout << "Robo used " << moveList[yourMoveList[option-1]].name << "!" << endl;
					short damage = moveList[yourMoveList[option-1]].calculateDamage(str);
					cout << "    Deals " << damage << " damage." << endl;
					monHP -= damage;
					currMag -= moveList[yourMoveList[option-1]].cost; // cost is always less than current magic, from earlier check
					if (monHP <= 0) continue;
				}
			}
		}
		else if (option == 9) {
			if ((rand() % 16) <= (10 + level - monsterList[monster].str)) {
				cout << "Run successful!" << endl;
				run = 1;
				continue;
			}
			else { 
				cout << "Can't escape!" << endl; 
			}
		}
		else if (option != 0) {
			cout << "Invalid selection, pick something else." << endl;
			continue;
		}
		// a move has happened, so now it's the monster's turn.
		short nextMove = monsterList[monster].pickMove();
		short damage = moveList[nextMove].calculateDamage(monsterList[monster].str);
		cout << monsterList[monster].name << " used " << moveList[nextMove].name << "!" << endl << "    Deals " << damage << " damage." << endl;
		currHP -= damage;
		learnedList[nextMove] += 1;
		if (learnedList[nextMove] == LEARN_COUNT) cout << "Learned " << moveList[nextMove].name << "!" << endl;
	}
	if (monHP <= 0) {
		cout << "You won the fight!" << endl << "Gained " << monsterList[monster].exp << " experience." << endl;
		monKilled += 1;
		currXP += monsterList[monster].exp;
		checkLevel();
	}
}


int main() {
	srand(time(0));
	// initialize move list and monster list
	cout << "Initializing..." << endl;
	// name, memory, damage, range, cost, scale
	moveList[0] = Move("Punch", 1, 2, 1, 0, 0.2);
	moveList[1] = Move("Kick", 1, 1, 3, 0, 0.4);
	moveList[2] = Move("Flurry", 4, 6, 2, 0, 0.5);
	moveList[3] = Move("GodFist", 19, 32, 16, 0, 0.5);
	moveList[4] = Move("Poke", 0, 1, 0, 0, 0);
	moveList[5] = Move("Blast", 15, 20, 20, 0, 0);
	moveList[6] = Move("WildSwng", 2, 0, 12, 0, 0);
	moveList[7] = Move("HeatRise", 6, 8, 6, 0, 0);
	moveList[8] = Move("Uppercut", 5, 6, 4, 0, 0.5);
	moveList[9] = Move("OraOra", 9, 4, 12, 0, 0.8);
	moveList[10] = Move("Suplex", 10, 5, 0, 0, 2);
	moveList[11] = Move("Suprpowr", 17, 10, 5, 0, 3);
	moveList[12] = Move("AllMight", 24, 10, 5, 0, 4);
	moveList[13] = Move("Candle", 2, 3, 3, 2, 0);
	moveList[14] = Move("MagicMsl", 4, 10, 0, 3, 0);
	moveList[15] = Move("Fireball", 7, 15, 10, 8, 0);
	moveList[16] = Move("Lightning", 8, 0, 50, 10, 0);
	moveList[17] = Move("Blizzard", 12, 30, 20, 15, 0);
	moveList[18] = Move("Meteor", 18, 50, 50, 30, 0);
	moveList[19] = Move("Ragnarok", 25, 150, 0, 50, 0);
	moveList[20] = Move("Recharge", 2, 0, 0, -2, 0);
	moveList[21] = Move("ManaFont", 5, 0, 0, -6, 0);
	moveList[22] = Move("MagicTap", 12, 0, 0, -12, 0);
	moveList[23] = Move("SprtBall", 27, 10, 10, -20, 0);
	moveList[24] = Move("Blade", 3, 4, 4, 2, 0.2);
	moveList[25] = Move("RedBlade", 6, 8, 8, 4, 0.4);
	moveList[26] = Move("BluBlade", 9, 12, 12, 6, 0.6);
	moveList[27] = Move("YlwBlade", 12, 16, 16, 8, 0.8);
	moveList[28] = Move("GrnBlade", 16, 20, 20, 10, 1);
	moveList[29] = Move("BlkBlade", 20, 30, 30, 15, 1.5);
	moveList[30] = Move("HolyBlde", 30, 40, 40, 30, 2);
	// name, [ moves ], hp, str, xp
	monsterList[0] = Monster("Black Knight", 29, 29, 29, 29, 23, 19, 12, 30, 500, 50, 0);
	monsterList[1] = Monster("Goblin", 0, 0, 0, 0, 1, 1, 6, 4, 10, 2, 2);
	monsterList[2] = Monster("Goblin Wizard", 0, 0, 1, 13, 13, 13, 13, 13, 15, 1, 3);
	monsterList[3] = Monster("Goblin King", 14, 6, 24, 24, 24, 6, 6, 6, 35, 5, 10);
	monsterList[4] = Monster("Werewolf", 1, 1, 2, 2, 4, 4, 6, 6, 15, 4, 5);
	monsterList[5] = Monster("Witch", 14,14,14,13,20,20,15,16, 25, 0, 15);
	monsterList[6] = Monster("Swamp Thing", 4,8,6,7,7,8,8,20, 60, 4, 12);
	monsterList[7] = Monster("GreatWitch", 14,14,15,15,16,16,17,21, 50, 0, 30);
	monsterList[8] = Monster("Wraith", 20,20,7,7,25,25,9,9, 40, 5, 24);
	monsterList[9] = Monster("Haunted Armor", 25,25,26,26,26,27,17,10, 60, 8, 45);
	monsterList[10] = Monster("Undead Prince", 26,27,28,17,17,11,22,22, 100, 10, 80);
	monsterList[11] = Monster("Cursed Soul", 15,15,16,16,28,21,21,21, 50, 0, 55);
	monsterList[12] = Monster("Black Belt", 9,9,9,10,10,3,11,11, 120, 15, 90);
	monsterList[13] = Monster("Karate Master", 3,3,12,23,11,11,5,10, 200, 25, 150);
	monsterList[14] = Monster("Ki Warrior", 21,21,22,27,27,28,28,29, 100, 12, 120);
	monsterList[15] = Monster("Raging Oni", 10,10,8,27,12,9,9,3, 140, 20, 140);
	monsterList[16] = Monster("Sentinel", 27,27,28,28,18,18,5,5, 100, 10, 200);
	monsterList[17] = Monster("WarMech", 3,3,29,29,5,5,5,5, 180, 20, 280);
	monsterList[18] = Monster("Fallen Angel", 30,29,22,22,19,23,3,3, 250, 25, 350);
	
	maxHP = 40;
	currHP = 40;
	str = 0;
	maxMag = 0;
	currMag = 0;
	level = 1;
	cout << "Finished initializing." << endl;

	cout << "Welcome to Robo! ver." << version << endl << "You are Robo, a fighting robot that mimics techniques that you see." << endl << "You are tasked with bringing the dreaded Black Knight to justice." << endl << "He resides in the Black Keep, but he is very strong." << endl << "Go forth, become stronger, learn new moves, and succeed in your quest!" << endl;
	int mainMenu;
	cout << "HP: " << currHP << "/" << maxHP << "    " << "Magic: " << currMag << "/" << maxMag << "    " << "Strength: " << str << endl << "Level: " << level << "    Exp: " << currXP << endl;
	cout << "Choose an option." << endl << "1 for Goblin Warrens" << endl << "2 for Cursed Swamp" << endl << "3 for Haunted Ruins" << endl << "4 for Master Dojo" << endl << "5 for Black Keep" << endl << "6 to rearrange your moves." << endl << "0 to quit." << endl;
	cin >> mainMenu;
	while (mainMenu != 0) { //Main gameplay loop.  From this menu, you can choose to pick a zone to fight in (1-5) or reconfigure your move list (6).  0 to quit.
		switch (mainMenu) {
		case 1: nextFight = calcRandom(zone1List);
			break;
		case 2: nextFight = calcRandom(zone2List);
			break;
		case 3: nextFight = calcRandom(zone3List);
			break;
		case 4: nextFight = calcRandom(zone4List);
			break;
		case 5: nextFight = calcRandom(zone5List);
			break;
		case 6: {
			nextFight = -1; // this case should really be a separate function, but oh well!
			// fuck it, 8 moves max.
			int currMem = 0;
			int currMoves = 0;
			yourMoveList[0] = -1;
			yourMoveList[1] = -1;
			yourMoveList[2] = -1;
			yourMoveList[3] = -1;
			yourMoveList[4] = -1;
			yourMoveList[5] = -1;
			yourMoveList[6] = -1;
			yourMoveList[7] = -1;
			while (currMem <= level) {
				int moveMenu = 1;
				while (moveMenu != 0 && currMoves < 8) { // 1-31 to select moves, 0 to finish.
					cout << "Moves you know:" << endl;
					for (int i = 0; i <= 30; i++) { //list all moves you know, some basic stats on each, then you select a move to add to your list.
						if (learnedList[i] >= LEARN_COUNT) cout << i + 1 << ": " << moveList[i].name << "   Damage: " << moveList[i].damage << " to " << moveList[i].damage + moveList[i].range << "   Memory: " << moveList[i].memory << "   Magic Cost: " << moveList[i].cost << "   Scaling: " << moveList[i].scale << endl;
					}
					cout << "Current move list:" << endl;
					for (int i = 0; i <= MAX_MOVES-1; i++) {
						if (yourMoveList[i] != -1) cout << i+1 << ": " << moveList[yourMoveList[i]].name << endl;
					}
					cout << "Choose a number to add it to your move list.  (Please don't add the same move multiple times, it'll break something.  8 moves max.)  0 to finish." << endl << "Memory left: " << level - currMem << endl;
					cin >> moveMenu;
					if (moveMenu < 0 || moveMenu > 31) {
						cout << "Invalid input.  Enter something that works." << endl;
						continue;
					}
					if (moveMenu == 0) continue;
					if (learnedList[moveMenu - 1] < LEARN_COUNT) {
						cout << "ERROR: You don't know that move." << endl;
						continue;
					}
					else {
						yourMoveList[currMoves] = moveMenu - 1;
						currMem += moveList[moveMenu - 1].memory;
						currMoves += 1;
						cout << "Move added." << endl << endl;
					}
				}
				if (currMem > level) {
					cout << "ERROR: Memory capacity has been exceeded.  Total memory cannot exceed your level.  Your move list has been erased." << endl;
					yourMoveList[0] = -1;
					yourMoveList[1] = -1;
					yourMoveList[2] = -1;
					yourMoveList[3] = -1;
					yourMoveList[4] = -1;
					yourMoveList[5] = -1;
					yourMoveList[6] = -1;
					yourMoveList[7] = -1;
				}
				break;
			}
		}
		case 0: nextFight = -1;
			break;
		}
		if (nextFight != -1) encounter(nextFight); // initiate a fight with monsterList[nextFight], player inputs a move that gets performed, then check monster HP.  If monHP <= 0, you win the fight and gain xp.  Then check if nextFight is 0 (Black Knight) and if so, win the game.
		// Then a monster move is selected and performed (and learnedList incremented!), and if currHP hits zero you die. (exit fight and set mainMenu to 0?  break works too.)
		if (currHP <= 0) {
			cout << "You died..." << endl;
			system("PAUSE");
			alive = 0;
			break; //should break out of mainMenu
		}
		else if (nextFight == 0) {
			alive = 2;
			cout << "You have succeeded in your quest.  The Black Knight is finally defeated." << endl << "Robo, the ever-learning battle machine, has fulfilled its duty." << endl << "The metal protector knows that the world is safe, and powers down until a new monster threatens the world." << endl << "Silently, it yearns for a time of everlasting peace, when it can finally rest forever." << endl << endl << "THE END" << endl;
			break;
		}
	
	
	
	
		cout << "Choose an option." << endl << "1 for Goblin Warrens" << endl << "2 for Cursed Swamp" << endl << "3 for Haunted Ruins" << endl << "4 for Master Dojo" << endl << "5 for Black Keep" << endl << "6 to rearrange your moves." << endl << "0 to quit." << endl;
		cin >> mainMenu;
	}
	short learned = 0;
	for (int i = 0; i < MAX_MOVES; i++) if (learnedList[i] >= LEARN_COUNT) learned++;
	cout << "GAME OVER" << endl << "Level achieved: " << level << endl << "Experience gained: " << currXP << endl << "Monsters defeated: " << monKilled << endl << "Moves learned: " << learned << endl;
	// end of game stats. moves learned, monsters fought, level achieved, whether you killed Black Knight.
	if (alive == 0) cout << "Robo fell in battle against " << monsterList[nextFight].name << "." << endl;
	if (alive == 1) cout << "Robo abandoned his quest." << endl;
	if (alive == 2) cout << "Robo defeated the Black Knight and brought peace to the land!" << endl << "Now do it again with " << level - 1 << "levels!  :^)" << endl;


	system("PAUSE");
	return 0;
}