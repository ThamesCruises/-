#include<iostream>
#include<iomanip>
#include<cstring>
#include <string>
#include <stdio.h>
#include <vector>
using namespace std;

//第二行：五个整数，依次是 dragon 、ninja、iceman、lion、wolf 的初始生命值。它们都大于0小于等于200
string allkind[5] = {"dragon", "ninja", "iceman", "lion", "wolf"}; // 输入顺序
string allweapon[3] = {"sword", "bomb", "arrow"};
string s[2] = {"red", "blue"}; // 哦我的天哪我竟然把它给忘了！
int seq_red[5] = {2, 3, 4, 1, 0}; // 红方司令部按照iceman、lion、wolf、ninja、dragon 的顺序制造武士。
int seq_blue[5] = {3, 0, 1, 2, 4}; // 蓝方司令部按照lion、dragon、ninja、iceman、wolf 的顺序制造武士。
string red_kind[5] = {"iceman", "lion", "wolf", "ninja", "dragon"};
string blue_kind[5] = {"lion", "dragon", "ninja", "iceman", "wolf"};
int allpower[5] = {0};
int alllife[5] = {0};
int M_elememts, N_city, R_arrow, K_loyal, T_extime;
bool end_ = 0;
int situation[3][22];

class Command;
class Warrior;
class City;
class battlefield;
class Weapon;

class Weapon
{
	friend Warrior;
	public:
	int wrnNo; // 编号
	int avltime; //还能使用几次
	bool avl; // 能否使用
	string wpnName;
	int wpnForce;
	Weapon() 
	{ 
		avl = false; wpnForce = 0; wrnNo = 3;
		wpnName = ""; 
	}
	Weapon(int num)
	{ 
		avl = true;
		wrnNo = num % 3; // 0, 1, 2
		wpnName = allweapon[wrnNo];
		if (wpnName == "arrow")
		{ wpnForce = R_arrow; avltime = 3; }
		else wpnForce = 0;
	}
};

class Warrior
{
	public:
	friend Command;
	friend Weapon;
	Weapon weapon[10]; // 武器数组
	Command *ptr;
	string warrName;
	int warrNo; // 编号
	int elements; // life
	int atkforce; // power
	int citypos; // which city it is in
	int loyalty;
	double morale;
	bool isarrow, isbomb, issword;
	int wpndata[3];
	Warrior(int num, int power, Command *p); // constructor
	Warrior() { wpndata[0] = wpndata[1] = wpndata[2] = 0; }
	void repoWeapon(int hour);
	virtual void Printmarch(int hour);
	virtual void PrintBorn(int hour) { return; }
	virtual void yell(int hour) { return; }
	inline void getWpnNum() // 统计武器数目
	{
		isarrow = isbomb = issword = 0;
		for (int c = 0; c < 3; c++) {
			if (weapon[c].avl == true) {
				if (c == 0) issword = 1;
				else if (c == 1) isbomb = 1;
				else if (c == 2) isarrow = 1;
			}
		}
	}
};

class Command
{
	friend Warrior;
	friend City;
	friend battlefield;
	public:
	Command() 
	{ }
	bool ok_produce;
	int lifenum; // 生命元
	int colour; // 阵营
	int totalnum; // 一共多少武士
	int warrpower[5]; // 每种武士攻击力
	int warrlife[5];
	int warrpos; // For Produce
	int Produce(int hour); // 00
	int reward(int hour); // 40
	void repoLife(int hour); // 报告生命元
	void initial(int clr, int lfn); // constructor
}RED, BLUE;

class City
{
	public:
	friend battlefield;
	// qsort(an, NUM, sizeof(unsigned int), MyCompare); 
	friend Warrior;
	friend Command;
	int cityNo;
	int taken;
	Warrior *red;
	Warrior *blue;
	Warrior *enemy;
	int acclife; // accessible lifenum
	int flag; // 0 == red, 1 == blue
	~City() {
		delete red; delete blue;
	}
	City() {
		red = NULL; blue = NULL; enemy = NULL;
		flag = -1;
		acclife = 0;
		taken = 0;
	}
	void bomb_use(int hour);
	void wolf_loot(Warrior *win_, Warrior *lose_, int hour);
	int fight(int hour);
}city[22];

Warrior::Warrior(int num, int power, Command *p) // constructor
{
	warrNo = num;
	wpndata[0] = wpndata[1] = wpndata[2] = 0; 
	int namePos = (num - 1) % 5;
	ptr = p;
	atkforce = power;
	loyalty = 0;
	morale = 0;
	elements = ptr->warrlife[namePos];
	if (ptr->colour == 0)
		citypos = 0;
	else
		citypos = N_city + 1;
}

void Warrior::Printmarch(int hour) 
{
	if (ptr->colour == 0) {
		citypos++;
		if (citypos != N_city + 1)
		{
			printf("%03d:10 ", hour);
			cout << "red " << warrName << " " << warrNo << " marched to city " << citypos <<
			" with " << elements << " elements and force " << atkforce << endl;
		}
		else // headquarter
		{
			printf("%03d:10 ", hour);
			cout << "red " << warrName << " " << warrNo << " reached blue headquarter with "
			<< elements << " elements and force " << atkforce << endl;
			city[N_city + 1].taken ++;
			if (city[N_city + 1].taken == 2) {
				printf("%03d:10 blue headquarter was taken\n", hour);
				end_ = 1;
			}
		}
	}
	else {
		citypos--;
		if (citypos != 0)
		{
			printf("%03d:10 ", hour);
			cout << "blue " << warrName << " " << warrNo << " marched to city " << citypos <<
			" with " << elements << " elements and force " << atkforce << endl;
		}
		else
		{
			printf("%03d:10 ", hour);
			cout << "blue " << warrName << " " << warrNo << " reached red headquarter with "
			<< elements << " elements and force " << atkforce << endl;
			city[0].taken ++;
			if (city[0].taken == 2) {
			printf("%03d:10 red headquarter was taken\n", hour);
			end_ = 1;
			}
		}
	}
}

// weapon[2] - arrow, [1] - bomb, [0] - sword
class Dragon:public Warrior
{
	public:
	Dragon(int num, int power, Command *p):Warrior(num, power, p)
	{
		warrName = "dragon";
		int t = warrNo % 3;
		weapon[t] = Weapon(t);
		if (weapon[t].wpnName == "sword")
		{
			weapon[t].wpnForce = atkforce * 2 / 10;
			if (weapon[t].wpnForce == 0)
				weapon[t].avl = false;
		}
		morale = (1.0 * p->lifenum) / (1.0 * alllife[0]);	
	}
	virtual void PrintBorn(int hour)
	{
		if (ptr->colour == 0) {
			printf("%03d:00 red dragon %d born\n", hour, warrNo);
			cout << "Its morale is " << setiosflags(ios::fixed) << setprecision(2) << morale << endl;
		}
		else if (ptr->colour == 1) {
			printf("%03d:00 blue dragon %d born\n", hour, warrNo);
			cout << "Its morale is " << setiosflags(ios::fixed) << setprecision(2) << morale << endl;
		}
	}
	virtual void yell(int hour) // 40
	{
		if (elements > 0 && morale - 0.8 > 0)
		{
			if (ptr->colour == 0) 
				printf("%03d:40 red dragon %d yelled in city %d\n", hour, warrNo, citypos);
			else
				printf("%03d:40 blue dragon %d yelled in city %d\n", hour, warrNo, citypos);
		}
	}
};

class Ninja:public Warrior
{
	public:
	bool ninjaSafe;
	Ninja(int num, int power, Command *p):Warrior(num, power, p)
	{
		warrName = "ninja";
		int t = warrNo % 3;
		int t2 = (warrNo + 1) % 3;
		weapon[t] = Weapon(t);
		if (weapon[t].wpnName == "sword") 
		{
			weapon[t].wpnForce = atkforce * 2 / 10;
			if (weapon[t].wpnForce == 0)
				weapon[t].avl = false;
		}
		weapon[t2] = Weapon(t2);
		if (weapon[t2].wpnName == "sword")
		{
			weapon[t2].wpnForce = atkforce * 2 / 10;
			if (weapon[t2].wpnForce == 0)
				weapon[t2].avl = false;
		}
	}
	virtual void PrintBorn(int hour)
	{
		if (ptr->colour == 0) 
			printf("%03d:00 red ninja %d born\n", hour, warrNo);
		else if (ptr->colour == 1) 
			printf("%03d:00 blue ninja %d born\n", hour, warrNo);
	}
};

class Iceman:public Warrior
{
	public:
	Iceman(int num, int power, Command *p):Warrior(num, power, p)
	{
		warrName = "iceman";
		int t = warrNo % 3;
		weapon[t] = Weapon(t);
		if (weapon[t].wpnName == "sword")
		{
			weapon[t].wpnForce = atkforce * 2 / 10;
			if (weapon[t].wpnForce == 0)
				weapon[t].avl = false;
		}
	}
	virtual void PrintBorn(int hour)
	{
		if (ptr->colour == 0)
			printf("%03d:00 red iceman %d born\n", hour, warrNo);
		else if (ptr->colour == 1)
			printf("%03d:00 blue iceman %d born\n", hour, warrNo);
	}
	virtual void Printmarch(int hour)
	{
		if (ptr->colour == 0) { // red
			citypos++;
			if (citypos % 2 == 0)
			{
				if (elements > 9)
					elements -= 9;
				else elements = 1;
				atkforce += 20;
			}
			if (citypos != N_city + 1) {
				printf("%03d:10 ", hour);
				cout << "red iceman " << warrNo << " marched to city " << citypos <<
				" with " << elements << " elements and force " << atkforce << endl;
			}
			else {
				printf("%03d:10 ", hour);
				cout << "red iceman " << warrNo << " reached blue headquarter with "
				<< elements << " elements and force " << atkforce << endl; 
				city[N_city + 1].taken++;
			}
		}
		else { // blue
			citypos--;
			if ((N_city + 1 - citypos) % 2 == 0)
			{
				if (elements > 9)
					elements -= 9;
				else elements = 1;
				atkforce += 20;
			}
			if (citypos !=0)
			{
				printf("%03d:10 ", hour);
				cout << "blue iceman " << warrNo << " marched to city " << citypos <<
				" with " << elements << " elements and force " << atkforce << endl;
			}
			else
			{
				printf("%03d:10 ", hour);
				cout << "blue iceman " << warrNo << " reached red headquarter with "
				<< elements << " elements and force " << atkforce << endl; 
				city[0].taken++;
			}
		}
	}
};

class Lion:public Warrior
{
	public:
	Lion(int num, int power, Command *p):Warrior(num, power, p)
	{
		warrName = "lion";
		ptr = p;
	}
	virtual void PrintBorn(int hour)
	{
		if (ptr->colour == 0)
			printf("%03d:00 red lion %d born\n", hour, warrNo);
		else if (ptr->colour == 1)
			printf("%03d:00 blue lion %d born\n", hour, warrNo);
		printf("Its loyalty is %d\n", loyalty);
	}
};

class Wolf:public Warrior
{
	public:
	Wolf(int num, int power, Command *p):Warrior(num, power, p)
	{
		warrName = "wolf";
		memset(weapon, 0, sizeof(weapon));
	}
	virtual void PrintBorn(int hour)
	{
		if (ptr->colour == 0)
			printf("%03d:00 red wolf %d born\n", hour, warrNo);
		else if (ptr->colour == 1)
			printf("%03d:00 blue wolf %d born\n", hour, warrNo);
	}
};

class battlefield
{
	public:
	friend Command;
	friend Warrior;
	friend City;
	void clear_field()
	{ 
		for (int k = 0; k <= 21; k++) {
			situation[0][k] = situation[1][k] = 0; // 赢了几次
			situation[2][k] = -1; // 当局结果
		}
	}
	battlefield()
	{
		for (int k = 0; k <= 21; k++) 
		{
			situation[0][k] = situation[1][k] = 0; // 赢了几次
			situation[2][k] = -1; // 当局结果
		}
	}
	void fleed(int hour);
	void march(int hour); // 进军
	void lifetaken(int hour); // 30
	void arrowtime(int hour); // 35
	void bombtime(int hour); // 38
	void battletime(int hour); // 40
	void loottime(int hour); 
	void yelltime(int hour); 
	void repo1(int hour); // 50
	void repo2(int hour); // 55
}field;

void Command::initial(int clr, int lfn)
{
	ok_produce = true;
	warrpos = 0;
	memset(warrlife, 0, sizeof(warrlife));
	memset(warrpower, 0, sizeof(warrpower));
	colour = clr;
	lifenum = lfn;
	totalnum = 0;
	if (this->colour == 0) {
		for (int i = 0; i < 5; i++) {
			warrlife[i] = alllife[seq_red[i]];
			warrpower[i] = allpower[seq_red[i]];
		}
	}
	else {
		for (int i = 0; i < 5; i++) {
			warrlife[i] = alllife[seq_blue[i]];
			warrpower[i] = allpower[seq_blue[i]];
		}
	}
}

// 00 born
int Command::Produce(int hour)
{
	if (warrlife[warrpos] > this->lifenum) // 生命元不够，等下一轮
		ok_produce = 0;
	else {
		totalnum++; // 武士数量++
		lifenum -= warrlife[warrpos];
		if (this->colour == 0) {
			string t = red_kind[warrpos];
			if (t == "dragon")
				city[0].red = new Dragon(totalnum, allpower[0], this);
			else if (t == "ninja")
				city[0].red = new Ninja(totalnum, allpower[1], this);
			else if (t == "iceman")
				city[0].red = new Iceman(totalnum, allpower[2], this);
			else if (t == "lion")
			{
				city[0].red = new Lion(totalnum, allpower[3], this);
				city[0].red->loyalty = lifenum;
			}
			else if (t == "wolf")
				city[0].red = new Wolf(totalnum, allpower[4], this);
			city[0].red->citypos = 0;
			city[0].red->PrintBorn(hour);
		}
		else {
			string t = blue_kind[warrpos];
			if (t == "dragon")
				city[N_city + 1].blue = new Dragon(totalnum, allpower[0], this);
			else if (t == "ninja")
				city[N_city + 1].blue = new Ninja(totalnum, allpower[1], this);
			else if (t == "iceman")
				city[N_city + 1].blue = new Iceman(totalnum, allpower[2], this);
			else if (t == "lion")
			{
				city[N_city + 1].blue = new Lion(totalnum, allpower[3], this);
				city[N_city + 1].blue->loyalty = lifenum;
			}
			else if (t == "wolf")
				city[N_city + 1].blue = new Wolf(totalnum, allpower[4], this);
			city[N_city + 1].blue->citypos = N_city + 1;
			city[N_city + 1].blue->PrintBorn(hour);
		}
		ok_produce = 1;
		warrpos = (warrpos + 1) % 5;
	}
	return ok_produce;
}
// 05 run
void battlefield::fleed(int hour)
{
	for (int pos = 0; pos <= N_city + 1; pos++)
	{
		if (city[pos].red != NULL && pos != N_city + 1)
		{
			if (city[pos].red->warrName == "lion" && city[pos].red->loyalty <= 0)
			{
				printf("%03d:05 ", hour);
				cout << "red lion " << city[pos].red->warrNo << " ran away" << endl;
				delete city[pos].red;
				city[pos].red = NULL;
			}
		}
		if (city[pos].blue != NULL && pos != 0)
		{
			if (city[pos].blue->warrName == "lion" && city[pos].blue->loyalty <= 0)
			{
				printf("%03d:05 ", hour);
				cout << "blue lion " << city[pos].blue->warrNo << " ran away" << endl;
				delete city[pos].blue;
				city[pos].blue = NULL;
			}
		}
	}
}
// 10 march
void battlefield::march(int hour)
{
	if (city[0].blue != NULL) {
		city[0].enemy = city[0].blue;
		city[0].blue = NULL;
	}
	if (city[N_city + 1].red != NULL) {
		city[N_city + 1].enemy = city[N_city + 1].red;
		city[N_city + 1].red = NULL;
	}
	for (int pos = 0; pos <= N_city; pos++) // blue
	{
		if (city[pos + 1].blue != NULL) {
			city[pos].blue = city[pos + 1].blue;
		}
		else city[pos].blue = NULL;
	}
	for (int pos = N_city + 1; pos > 0; pos--) // red
	{
		if (city[pos - 1].red != NULL) {
			city[pos].red = city[pos - 1].red;
		}
		else city[pos].red = NULL;
	}
	city[0].red = city[N_city + 1].blue = NULL;

	for (int pos = 0; pos <= N_city + 1; pos++)
	{
		if (city[pos].red != NULL)
			city[pos].red->Printmarch(hour);
		if (city[pos].blue != NULL)
			city[pos].blue->Printmarch(hour);
	}
	return;
}
// 20 城市产生生命元
void lifeup()
{
	for (int pos = 1; pos <= N_city; pos++) {
		city[pos].acclife += 10;
	}
	return;
}
// 30 单独的武士取走生命元
void battlefield::lifetaken(int hour)
{
	for (int pos = 1; pos <= N_city; pos++) {
		if (city[pos].red != NULL && city[pos].blue == NULL)
		{
			RED.lifenum += city[pos].acclife;
			printf("%03d:30 ", hour);
			cout << "red " << city[pos].red->warrName << " " << city[pos].red->warrNo 
			<< " earned " << city[pos].acclife << " elements for his headquarter" << endl;
			city[pos].acclife = 0;
		}
		else if (city[pos].blue != NULL && city[pos].red == NULL)
		{
			BLUE.lifenum += city[pos].acclife;
			printf("%03d:30 ", hour);
			cout << "blue " << city[pos].blue->warrName << " " << city[pos].blue->warrNo 
			<< " earned " << city[pos].acclife << " elements for his headquarter" << endl;
			city[pos].acclife = 0;
		}
	}
	return;
}

// 35 arrow time
void shoot(Warrior *atk, Warrior *beatk, int hour)
{
	if (atk == NULL || beatk == NULL) return;
	if (atk->weapon[2].avl == false) return; // no arrow
	beatk->elements -= R_arrow;
	if (beatk->elements <= 0) { // killed
		printf("%03d:35 ", hour);
		cout << s[atk->ptr->colour] << " " << atk->warrName << " " << atk->warrNo << " shot and killed " 
		<< s[beatk->ptr->colour] << " " << beatk->warrName << " " << beatk->warrNo << endl;
	}
	else {
		printf("%03d:35 ", hour);
		cout << s[atk->ptr->colour] << " " << atk->warrName << " " << atk->warrNo << " shot" << endl;
	}
	atk->weapon[2].avltime--;
	if (atk->weapon[2].avltime == 0) {
		atk->weapon[2].avl = false;
	}
}

void battlefield::arrowtime(int hour)
{
	for (int pos = 1; pos <= N_city; pos++)
	{
		if (city[pos].red != NULL && city[pos + 1].blue != NULL)
			shoot(city[pos].red, city[pos + 1].blue, hour);
		if (city[pos].blue != NULL && city[pos - 1].red != NULL)
			shoot(city[pos].blue, city[pos - 1].red, hour);
	}
	return;
}

// 38 bomb / weapon[0]-sword, [1]-bomb, [2]-arrow
void City::bomb_use(int hour)
{
	if ((this->flag == 0) || (this->flag == -1 && this->cityNo % 2 == 1)) // red attack
	{
		if (blue->elements <= 0) return;
		int blue_surv = 0;
		if (red->weapon[0].avl == true)
			blue_surv = blue->elements - red->atkforce - red->weapon[0].wpnForce;
		else blue_surv = blue->elements - red->atkforce;
		if (blue_surv > 0)  
		{
			if (blue->warrName == "ninja") return;
			if (red->weapon[1].avl == 0) return; // red has no bomb
			int red_suv = 0;
			if (blue->weapon[0].avl == true)
				red_suv = red->elements - blue->atkforce / 2 - blue->weapon[0].wpnForce;
			else red_suv = red->elements - blue->atkforce / 2;
			if (red_suv <= 0)
			{
				printf("%03d:38 ", hour);
				cout << "red " << red->warrName << " " << red->warrNo << " used a bomb and killed blue "
				<< blue->warrName << " " << blue->warrNo << endl;
				delete red;
				delete blue;
				red = blue = NULL;
				return;
			}
			else return; // red wouldn't be killed
		}
		else // blue would be killed and uses bomb
		{
			if (blue->weapon[1].avl == 0) return; // blue has no bomb
			printf("%03d:38 ", hour);
			cout << "blue " << blue->warrName << " " << blue->warrNo << " used a bomb and killed red "
			<< red->warrName << " " << red->warrNo << endl;
			delete red;
			delete blue;
			red = blue = NULL;
			return;
		}
	}
	else // blue attack
	{
		if (red->elements <= 0) return;
		int red_surv = 0;
		if (blue->weapon[0].avl == true)
			red_surv = red->elements - blue->weapon[0].wpnForce - blue->atkforce;
		else red_surv = red->elements - blue->atkforce;
		if (red_surv > 0)  
		{ 
			if (red->warrName == "ninja") return;
			if (blue->weapon[1].avl == 0) return; // blue has no bomb
			int blue_surv = 0;
			if (red->weapon[0].avl == true)
				blue_surv = blue->elements - red->atkforce / 2 - red->weapon[0].wpnForce;
			else blue_surv = blue->elements - red->atkforce / 2;
			if (blue_surv <= 0)
			{// blue would be killed
				printf("%03d:38 ", hour);
				cout << "blue " << blue->warrName << " " << blue->warrNo << " used a bomb and killed red "
				<< red->warrName << " " << red->warrNo << endl;
				delete red;
				delete blue;
				red = blue = NULL;
				return;
			}
			else return; // red wouldn't be killed
		}
		else // red would be killed and uses bomb
		{
			if (red->weapon[1].avl == 0) return;
			printf("%03d:38 ", hour);
			cout << "red " << red->warrName << " " << red->warrNo << " used a bomb and killed blue "
			<< blue->warrName << " " << blue->warrNo << endl;
			delete red;
			delete blue;
			red = blue = NULL;
			return;
		}
	}
}

void battlefield::bombtime(int hour)
{
	for (int pos = 1; pos <= N_city; pos++)
	{
		if (city[pos].red != NULL && city[pos].blue != NULL)
			if (city[pos].red->elements > 0 && city[pos].blue->elements > 0)
				city[pos].bomb_use(hour);
	}
}

// 40 battletime
void City::wolf_loot(Warrior *win_, Warrior *lose_, int hour)
{
	if (win_->warrName != "wolf")
		return;
	if (lose_->weapon[0].avl == 1 && win_->weapon[0].avl == 0) 
		win_->weapon[0] = lose_->weapon[0];
	if (lose_->weapon[1].avl == 1 && win_->weapon[1].avl == 0) 
		win_->weapon[1] = lose_->weapon[1];
	if (lose_->weapon[2].avl == 1 && win_->weapon[1].avl == 0) 
		win_->weapon[2] = lose_->weapon[2];
	lose_->weapon[0].avl = lose_->weapon[1].avl = lose_->weapon[2].avl = 0;
}

int City::fight(int hour) // 1 for blue, 0 for red, -1 for draw
{   // 无战斗城市里五分钟前被射死的武士
	if (blue == NULL && red != NULL && red->elements <= 0) {
		delete red; red = NULL;
	}
	if (red == NULL && blue != NULL && blue->elements <= 0) {
		delete blue; blue = NULL;
	}
	if (red == NULL || blue == NULL) return -1;

	if (red->elements <= 0 && blue->elements <= 0) { // 双方都在五分钟前被射死
		delete red; red = NULL;
		delete blue; blue = NULL;
		situation[2][cityNo] = -1; 
		return -1;
	}
	// 一方被射死
	if (red->elements <= 0 && blue->elements > 0) // red was shoot
	{
		if (blue->warrName == "dragon")
			blue->morale += 0.2;
		if (this->flag == 1 || (this->flag == -1 && this->cityNo % 2 == 0))
			blue->yell(hour);
		wolf_loot(blue, red, hour);
		printf("%03d:40 ", hour);
		cout << "blue " << blue->warrName << " " << blue->warrNo << " earned " << acclife 
		<< " elements for his headquarter" << endl;
		situation[0][this->cityNo] = 0; 
		situation[1][this->cityNo]++;
		if (situation[1][this->cityNo] == 2 && this->flag != 1)
		{
			printf("%03d:40 blue flag raised in city %d\n", hour, this->cityNo);
			this->flag = 1;
		}
		delete red;
		red = NULL;
		situation[2][cityNo] = 1;
		return 1;
	}
	else if (blue->elements <= 0 && red->elements > 0) // blue was shot
	{
		if (red->warrName == "dragon")
			red->morale += 0.2;
		if ((this->flag == 0) || (this->flag == -1 && this->cityNo % 2 == 1))
			red->yell(hour);
		wolf_loot(red, blue, hour);
		printf("%03d:40 ", hour);
		cout << "red " << red->warrName << " " << red->warrNo << " earned " << acclife 
		<< " elements for his headquarter" << endl;
		situation[0][this->cityNo]++; 
		situation[1][this->cityNo] = 0;
		if (situation[0][this->cityNo] == 2 && this->flag != 0)
		{
			printf("%03d:40 red flag raised in city %d\n", hour, this->cityNo);
			this->flag = 0;
		}
		delete blue;
		blue = NULL;
		situation[2][cityNo] = 0;
		return 0;
	}

	// both alive
	if ((this->flag == 0) || (this->flag == -1 && this->cityNo % 2 == 1)) // red attacked
	{
		printf("%03d:40 ", hour);
		cout << "red " << red->warrName << " " << red->warrNo << " attacked blue " << blue->warrName
		<< " " << blue->warrNo << " in city " << this->cityNo << " with " << red->elements 
		<< " elements and force " << red->atkforce << endl;
		int life_r = red->elements;
		int life_b = blue->elements;
		int harm = 0;
		if (red->weapon[0].avl == true) // sword available?
		{
			harm = red->weapon[0].wpnForce + red->atkforce;
			red->weapon[0].wpnForce = red->weapon[0].wpnForce * 4 / 5;
			if (red->weapon[0].wpnForce <= 0)
				red->weapon[0].avl = false; 
		}
		else harm = red->atkforce;
		if (blue->elements <= harm) // blue died
		{
			printf("%03d:40 ", hour);
			cout << "blue " << blue->warrName << " " << blue->warrNo << " was killed in city " << this->cityNo << endl;
			if (blue->warrName == "lion")
				red->elements += life_b;
			wolf_loot(red, blue, hour);
			if (red->warrName == "dragon") {
				red->morale += 0.2;
				red->yell(hour);
			}
			delete blue;
			blue = NULL;
			printf("%03d:40 ", hour);
			cout << "red " << red->warrName << " " << red->warrNo << " earned " << acclife 
			<< " elements for his headquarter" << endl;
			situation[0][this->cityNo]++; 
			situation[1][this->cityNo] = 0;
			if (this->flag != 0 && situation[0][this->cityNo] == 2)
			{
				printf("%03d:40 red flag raised in city %d\n", hour, this->cityNo);
				this->flag = 0;
			}
			situation[2][cityNo] = 0;
			return 0;
		}
		else // blue survived and fought back
		{
			blue->elements -= harm;
			if (blue->warrName != "ninja") 
			// ninjas do not fight back. Skip to the draw part
			{
				printf("%03d:40 ", hour);
				cout << "blue " << blue->warrName << " " << blue->warrNo << " fought back against red " 
				<< red->warrName << " " << red->warrNo << " in city " << cityNo << endl;
				if (blue->weapon[0].avl == true)
				{
					harm = blue->weapon[0].wpnForce + (blue->atkforce) / 2;
					blue->weapon[0].wpnForce = blue->weapon[0].wpnForce * 4 / 5;
					if (blue->weapon[0].wpnForce <= 0)
						blue->weapon[0].avl  = false;
				}
				else harm = blue->atkforce / 2;
				if (red->elements <= harm) // red died
				{
					printf("%03d:40 ", hour);
					cout << "red " << red->warrName << " " << red->warrNo << " was killed in city " << this->cityNo << endl;
					wolf_loot(blue, red, hour);
					if (red->warrName == "lion")
						blue->elements += life_r;
					if (blue->warrName == "dragon") {
						blue->morale += 0.2;
					}
					delete red;
					red = NULL;
					printf("%03d:40 ", hour);
					cout << "blue " << blue->warrName << " " << blue->warrNo << " earned " << acclife 
					<< " elements for his headquarter" << endl;
					situation[0][this->cityNo] = 0; 
					situation[1][this->cityNo]++;
					if (this->flag != 1 && situation[1][this->cityNo] == 2)
					{
						printf("%03d:40 blue flag raised in city %d\n", hour, this->cityNo);
						this->flag = 1;
					}
					situation[2][cityNo] = 1;
					return 1;
				}
				else
					red->elements = red->elements - harm;
			} // both blue and red survived, no matter blue is a ninja or no
			if (red->warrName == "dragon") {
				red->morale -= 0.2;
				red->yell(hour);
			}
			else if (red->warrName == "lion")
				red->loyalty -= K_loyal;
			if (blue->warrName == "dragon") {
				blue->morale -= 0,2;
			}
			else if (blue->warrName == "lion") 
				blue->loyalty -= K_loyal;
			situation[0][cityNo] = situation[1][cityNo] = 0; // 平局计数中断
			situation[2][cityNo] = -1;
			return -1;
		}
	}
	else // blue attacked
	{
		printf("%03d:40 ", hour);
		cout << "blue " << blue->warrName << " " << blue->warrNo << " attacked red " << red->warrName
		<< " " << red->warrNo << " in city " << this->cityNo << " with " << blue->elements 
		<< " elements and force " << blue->atkforce << endl;
		int life_r = red->elements;
		int life_b = blue->elements;
		int harm = 0;
		if (blue->weapon[0].avl == true) // sword available?
		{
			harm = blue->weapon[0].wpnForce + blue->atkforce;
			blue->weapon[0].wpnForce = blue->weapon[0].wpnForce * 4 / 5;
			if (blue->weapon[0].wpnForce <= 0)
				blue->weapon[0].avl = false; 
		}
		else harm = blue->atkforce;
		if (red->elements <= harm) // red died
		{
			printf("%03d:40 ", hour);
			cout << "red " << red->warrName << " " << red->warrNo << " was killed in city " << this->cityNo << endl;
			if (red->warrName == "lion")
				blue->elements += life_r;
			wolf_loot(blue, red, hour);
			if (blue->warrName == "dragon") {
				blue->morale += 0.2;
				blue->yell(hour);
			}
			delete red;
			red = NULL;
			printf("%03d:40 ", hour);
			cout << "blue " << blue->warrName << " " << blue->warrNo << " earned " << acclife 
			<< " elements for his headquarter" << endl;
			situation[0][cityNo] = 0;
			situation[1][cityNo]++;
			if (this->flag != 1 && situation[1][this->cityNo] == 2)
			{
				printf("%03d:40 blue flag raised in city %d\n", hour, this->cityNo);
				this->flag = 1;
			}
			situation[2][cityNo] = 1;
			return 1;
		}
		else // red survived and fought back
		{
			red->elements = red->elements - harm;
			if (red->warrName != "ninja") 
			{
				printf("%03d:40 ", hour);
				cout << "red " << red->warrName << " " << red->warrNo << " fought back against blue " 
				<< blue->warrName << " " << blue->warrNo << " in city " << cityNo << endl;
				if (red->weapon[0].avl == true)
				{
					harm = red->weapon[0].wpnForce + red->atkforce / 2;
					red->weapon[0].wpnForce = red->weapon[0].wpnForce * 4 / 5;
					if (red->weapon[0].wpnForce <= 0)
						red->weapon[0].avl  = false;
				}
				else harm = red->atkforce / 2;
				if (blue->elements <= harm) // blue died
				{
					printf("%03d:40 ", hour);
					cout << "blue " << blue->warrName << " " << blue->warrNo << " was killed in city " << this->cityNo << endl;
					wolf_loot(red, blue, hour);
					if (blue->warrName == "lion")
						red->elements += life_b;
					if (red->warrName == "dragon") {
						red->morale = red->morale + 0.2;
					}
					delete blue;
					blue = NULL;
					printf("%03d:40 ", hour);
					cout << "red " << red->warrName << " " << red->warrNo << " earned " << acclife 
					<< " elements for his headquarter" << endl;
					situation[0][cityNo]++;
					situation[1][cityNo] = 0;
					if (this->flag != 0 && situation[0][this->cityNo] == 2)
					{
						printf("%03d:40 red flag raised in city %d\n", hour, this->cityNo);
						this->flag = 0;
					}
					situation[2][cityNo] = 0;
					return 0;
				}
				else 
					blue->elements = blue->elements - harm;
			}
			// didnt fight back || fought back and atck survived
			if (red->warrName == "dragon") {
				red->morale -= 0.2;
			}
			else if (red->warrName == "lion")
				red->loyalty -= K_loyal;
			if (blue->warrName == "dragon") {
				blue->morale -= 0,2;
				blue->yell(hour);
			}
			else if (blue->warrName == "lion") 
				blue->loyalty -= K_loyal;
			situation[0][cityNo] = situation[1][cityNo]  = 0;
			situation[2][cityNo] = -1;
			return -1;
		}
	}
}
void battlefield::battletime(int hour)
{
	for (int pos = 0; pos <= 21; pos++)
		situation[2][pos] = -1;

	for (int pos = 1; pos <= N_city; pos++) // fight
		if (city[pos].red != NULL || city[pos].blue != NULL)
		{
			city[pos].fight(hour);
		}
	// reward
	for (int pos = 1; pos <= N_city; pos++) 
		if (situation[2][pos] == 0 && RED.lifenum >= 8 && city[pos].red != NULL) {
			RED.lifenum -= 8;
			city[pos].red->elements += 8;
		}
	for (int pos = N_city; pos >= 1; pos--)
		if (situation[2][pos] == 1 && BLUE.lifenum >= 8 && city[pos].blue != NULL) {
			BLUE.lifenum -= 8;
			city[pos].blue->elements += 8;
		}
	// get element
	for (int pos = 1; pos <= N_city; pos++)
	{
		if (situation[2][pos] == 0) {
			RED.lifenum += city[pos].acclife;
			city[pos].acclife = 0;
		}
		else if (situation[2][pos] == 1) {
			BLUE.lifenum += city[pos].acclife;
			city[pos].acclife = 0;
		}
	}
}
// 50 headquarters' report time
void Command::repoLife(int hour)
{
	if (this->colour == 0)
		printf("%03d:50 %d elements in red headquarter\n", hour, lifenum);
	else printf("%03d:50 %d elements in blue headquarter\n", hour, lifenum);
}
void battlefield::repo1(int hour)
{
	RED.repoLife(hour);
	BLUE.repoLife(hour);
}
// 55 warriors' report time
void Warrior::repoWeapon(int hour)
{
	this->getWpnNum();
	printf("%03d:55 ", hour);
	if (issword == 0 && isbomb == 0 && isarrow == 0)
		cout << s[this->ptr->colour] << " " << warrName << " " << warrNo << " has no weapon" << endl;
	else {
		int dot = 0;
		cout << s[this->ptr->colour] << " " << warrName << " " << warrNo << " has ";
		if (isarrow) {
			printf("arrow(%d)", weapon[2].avltime); dot = 1;
		}
		if (isbomb) {
			if (dot == 1) printf(",");
			printf("bomb"); dot = 1;
		}
		if (issword) {
			if (dot == 1) printf(",");
			printf("sword(%d)", weapon[0].wpnForce);
		}
		printf("\n");
	}
	return;
}
void battlefield::repo2(int hour)
{
	for (int pos = 0; pos <= N_city + 1; pos++)
	{
		if (city[pos].red != NULL)
			city[pos].red->repoWeapon(hour);
	}
	if (city[N_city + 1].enemy != NULL)
		city[N_city + 1].enemy->repoWeapon(hour);
	if (city[0].enemy != NULL)
		city[0].enemy->repoWeapon(hour);
	for (int pos = 0; pos <= N_city + 1; pos++)
	{
		if (city[pos].blue != NULL)
			city[pos].blue->repoWeapon(hour);
	}
}

void clear_city()
{
	for (int i = 0; i <= 21; i++)
	{
		city[i].red = NULL; city[i].blue = NULL;
		city[i].enemy = NULL;
		city[i].flag = -1;
		city[i].acclife = 0;
		city[i].taken = 0;
		city[i].cityNo = i;
	}
	return;
}

int main()
{
	int n;
	cin >> n;
	for (int circ = 1; circ <= n; circ++)
	{	// 清理现场
		end_ = 0;
		clear_city();
		field.clear_field();
		// 输入
		cout << "Case " << circ << ":" << endl;
		cin >> M_elememts >> N_city >> R_arrow >> K_loyal >> T_extime;
		for (int i = 0; i < 5; i++)
			cin >> alllife[i];
		for (int i = 0; i < 5; i++)
			cin >> allpower[i];
		RED.initial(0, M_elememts);
		BLUE.initial(1, M_elememts);
		//cout << "commands ready" << endl;
		int total_h = T_extime / 60;
		int minutes = T_extime % 60;
		
		// 开始
		for (int hour = 0; hour <= total_h; hour++)
		{
			// 00
			RED.Produce(hour);
			BLUE.Produce(hour);
			// 05
			if (hour == total_h && minutes < 5) break;
			field.fleed(hour);
			// 10
			if (hour == total_h && minutes < 10) break;
			field.march(hour);
			if (end_ == 1) break;
			// 20
			if (hour == total_h && minutes < 20) break;
			lifeup();
			// 30
			if (hour == total_h && minutes < 30) break;
			field.lifetaken(hour);
			// 35
			if (hour == total_h && minutes < 35) break;
			field.arrowtime(hour);
			// 38
			if (hour == total_h && minutes < 38) break;
			field.bombtime(hour);
			// 40
			if (hour == total_h && minutes < 40) break;
			field.battletime(hour);
			// 50
			if (hour == total_h && minutes < 50) break;
			field.repo1(hour);
			// 55
			if (hour == total_h && minutes < 55) break;
			field.repo2(hour);
		}
	}
	return 0;
}