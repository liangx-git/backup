#pragma once
#ifndef GOMOKU_H
#define GOMOKU_H

#define HEIGHT 15
#define WIDTH  15

#include "gomoku-test.h"

#include<iostream>
#include<vector>
#include<string>
#include<utility>
#include<memory>
#include<map>
#include<functional>


using std::cin;
using std::cout;
using std::endl;
using std::vector;
using std::string;
using std::pair;
using std::shared_ptr;
using std::map;


typedef pair<int, int> Position;    //坐标(x, y)
typedef vector<pair<int, int>> Direction;

//棋盘类
class Checkerboard
{
	friend class Judge;
	friend class Player;
	friend class HumanPlayer;
	friend class ComputerPlayer;

public:
	//五子棋中的黑方和白方
	enum Side
	{
		NONE = 0,
		BLACK = 1,
		WHITE = 2,

		//剪枝算法中使用
		PREPARE = 4,
		CHECK = 8
	};

	//棋面情况，使用16位位向量表示棋局情况
	//前八位表示活（0x00）、冲（0x01）、死（0x02）、冲（0x04）、禁手（0x08）、长连(0x10)，后八位表示连子数，
	enum State
	{
		HUO_1 = 0x0001,
		HUO_2 = 0x0002,
		CHONG_3 = 0X0103,
		HUO_3 = 0x0003,
		CHONG_4 = 0x0104,
		JIN_3_3 = 0x0803,
		HUO_4 = 0x0004,
		JIN_4_4 = 0x0804,
		HUO_5 = 0x0005,
		CHONG_5 = 0X0105,
		SI_5 = 0X0205,
		CHANG_n = 0x00ff,
		//辅助类型
		HUO = 0x0000,
		SI = 0x0200,
		CHONG = 0x0400,
		JIN = 0x0800,
		WIN = 0xffff
	};

public:
	Checkerboard(int w = WIDTH, int h = HEIGHT) :
		width(w),
		height(h),
		status(height, vector<Side>(width, NONE)),  //初始化棋盘状态为NONE
		score_sheet          //初始化评分表
		({ { HUO_1, 5 },
			{ HUO_2, 50 },
			{ CHONG_3, 50 },
			{ HUO_3, 100 },
			{ CHONG_4, 100 },
			{ JIN_3_3, 1000 },
			{ HUO_4, 1500 },
			{ JIN_4_4, 1500 },
			{ HUO_5, 50000 },
			{ CHONG_5, 50000 },
			{ SI_5, 50000 },
			{ CHANG_n, 50000 },
			{ WIN, 50000 } }) {}

	~Checkerboard() {}

	void PrintCBoard();
	void UpdateCBoard(const Position &pos);
	void PrintMessage(const string &msg);

private:
	bool PosIsAvilable(const Position &pos);
	bool PosOutOfRange(const Position &pos);

private:
	//棋盘规格
	int width;
	int height;

	vector<vector<Side>> status; //棋盘中每个落点位置状态

								 //当前回合落子信息
	Side cur_side;
	Position cur_pos;

	map<State, int> score_sheet; //对应State中所有情况的评分表
	static const Direction directions;   //顺时针八个方向上的增量数组
};


//玩家基类
class Player
{
public:
	Player(shared_ptr<Checkerboard> checkerboard,
		int id = 0,
		Checkerboard::Side s = Checkerboard::NONE,
		int ng = 0,
		int nw = 0) :
		ptr_to_cboard(checkerboard),
		id(id),
		side(s),
		number_of_total_games(ng),
		number_of_win(nw)
	{}

	virtual ~Player() {}

	//落子
	void PutAPiece();

protected:
	virtual Position GetNextPos() = 0;

protected:
	//指向公共的棋盘对象
	shared_ptr<Checkerboard> ptr_to_cboard;

	//用户基本游戏信息
	int id;
	Checkerboard::Side side;
	int number_of_total_games;
	int number_of_win;
};


//用户玩家
class HumanPlayer : public Player
{
public:
	HumanPlayer(shared_ptr<Checkerboard> &checkerboard,
		int id = 0,
		Checkerboard::Side s = Checkerboard::BLACK, //玩家默认为黑方
		int nc = 0,
		int nw = 0) :
		Player(checkerboard, id, s, nc, nw) {}

	virtual ~HumanPlayer() {}

protected:
	//@override
	virtual Position GetNextPos();

private:
	Position GetNextPosFromIO();

};


//电脑玩家
class ComputerPlayer : public Player
{
	enum Difficulty
	{
		NORMAL = 1,
		HARD = 2
	};

public:
	ComputerPlayer(shared_ptr<Checkerboard> checkerboard) :
		Player(checkerboard, -1, Checkerboard::WHITE),      //电脑玩家的id号特定为-1,默认为白方

		game_difficulty(NORMAL) {}
	virtual ~ComputerPlayer() {}

	void SetDifficulty(Difficulty diff);

protected:
	//override
	virtual Position GetNextPos();

private:
	//困难难度
	Position GetPosByHardWay();

	//AlphaBeta深搜剪枝
	int AlphaBeta(Position pos, int depth, int alp, int beta, Checkerboard::Side side, vector<vector<Checkerboard::Side>> status);

	bool HasNextChildPos(const vector<vector<Checkerboard::Side>> &status);

	Position GetNextChildPos(Checkerboard::Side s, const vector<vector<Checkerboard::Side>> &status);

	void SetPosCheck(vector<vector<Checkerboard::Side>> &status, const Position &pos);

	vector<vector<Checkerboard::Side>> UpdateStatus(const vector<vector<Checkerboard::Side>> &status, const Position &pos, Checkerboard::Side side);

	//简单难度
	Position GetPosBySimpleWay();

	//对当前局面评分
	int Evaluate(const Position &pos, vector<vector<Checkerboard::Side>> status);

	int EvaluateOneSide(const Position &pos, Checkerboard::Side side, vector<vector<Checkerboard::Side>> status);

	void GetAllDireStateInfo(const Position &pos, Checkerboard::Side side, vector<vector<Checkerboard::Side>> status, map<Checkerboard::State, int> &state_info);

	Checkerboard::Side ReverseSide(Checkerboard::Side s);

	int CalcPiecesInCurrDirection(const Position &pos, Checkerboard::Side s, int dire, vector<vector<Checkerboard::Side>> status);

	bool EndPosIsEmpty(const Position &pos, int dire, int len, vector<vector<Checkerboard::Side>> status);

	void MovePos(Position &pos, int dire);

	//AlphaBeat

private:
	Difficulty game_difficulty;
};


//裁判
typedef int ID;
class Judge
{
	typedef Checkerboard::Side Side;


public:
	Judge(shared_ptr<Checkerboard> checkerboard) :
		ptr_to_cboard(checkerboard) {}

	~Judge() {}

	void SetPlayersInfo(pair<Checkerboard::Side, ID> &player_info);

	bool IsWin();

	void AnnounceWinerAndFinishGame();

private:
	//功能实现辅助函数

	int CalcPiecesInCurrDirection(const Position &pos, int dire);

	void MovePos(Position &pos, int dire);

private:

	//映射用户ID到棋子方（棋子颜色）
	map<Checkerboard::Side, ID> player_side;

	//指向公共的棋盘对象
	shared_ptr<Checkerboard> ptr_to_cboard;
};


//负责从数据库中获取用户信息，以及用户信息写入数据库
class Recorder;


#endif // GOMOKU_H
