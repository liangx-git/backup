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


typedef pair<int, int> Position;    //����(x, y)
typedef vector<pair<int, int>> Direction;

//������
class Checkerboard
{
	friend class Judge;
	friend class Player;
	friend class HumanPlayer;
	friend class ComputerPlayer;

public:
	//�������еĺڷ��Ͱ׷�
	enum Side
	{
		NONE = 0,
		BLACK = 1,
		WHITE = 2,

		//��֦�㷨��ʹ��
		PREPARE = 4,
		CHECK = 8
	};

	//���������ʹ��16λλ������ʾ������
	//ǰ��λ��ʾ�0x00�����壨0x01��������0x02�����壨0x04�������֣�0x08��������(0x10)�����λ��ʾ��������
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
		//��������
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
		status(height, vector<Side>(width, NONE)),  //��ʼ������״̬ΪNONE
		score_sheet          //��ʼ�����ֱ�
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
	//���̹��
	int width;
	int height;

	vector<vector<Side>> status; //������ÿ�����λ��״̬

								 //��ǰ�غ�������Ϣ
	Side cur_side;
	Position cur_pos;

	map<State, int> score_sheet; //��ӦState��������������ֱ�
	static const Direction directions;   //˳ʱ��˸������ϵ���������
};


//��һ���
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

	//����
	void PutAPiece();

protected:
	virtual Position GetNextPos() = 0;

protected:
	//ָ�򹫹������̶���
	shared_ptr<Checkerboard> ptr_to_cboard;

	//�û�������Ϸ��Ϣ
	int id;
	Checkerboard::Side side;
	int number_of_total_games;
	int number_of_win;
};


//�û����
class HumanPlayer : public Player
{
public:
	HumanPlayer(shared_ptr<Checkerboard> &checkerboard,
		int id = 0,
		Checkerboard::Side s = Checkerboard::BLACK, //���Ĭ��Ϊ�ڷ�
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


//�������
class ComputerPlayer : public Player
{
	enum Difficulty
	{
		NORMAL = 1,
		HARD = 2
	};

public:
	ComputerPlayer(shared_ptr<Checkerboard> checkerboard) :
		Player(checkerboard, -1, Checkerboard::WHITE),      //������ҵ�id���ض�Ϊ-1,Ĭ��Ϊ�׷�

		game_difficulty(NORMAL) {}
	virtual ~ComputerPlayer() {}

	void SetDifficulty(Difficulty diff);

protected:
	//override
	virtual Position GetNextPos();

private:
	//�����Ѷ�
	Position GetPosByHardWay();

	//AlphaBeta���Ѽ�֦
	int AlphaBeta(Position pos, int depth, int alp, int beta, Checkerboard::Side side, vector<vector<Checkerboard::Side>> status);

	bool HasNextChildPos(const vector<vector<Checkerboard::Side>> &status);

	Position GetNextChildPos(Checkerboard::Side s, const vector<vector<Checkerboard::Side>> &status);

	void SetPosCheck(vector<vector<Checkerboard::Side>> &status, const Position &pos);

	vector<vector<Checkerboard::Side>> UpdateStatus(const vector<vector<Checkerboard::Side>> &status, const Position &pos, Checkerboard::Side side);

	//���Ѷ�
	Position GetPosBySimpleWay();

	//�Ե�ǰ��������
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


//����
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
	//����ʵ�ָ�������

	int CalcPiecesInCurrDirection(const Position &pos, int dire);

	void MovePos(Position &pos, int dire);

private:

	//ӳ���û�ID�����ӷ���������ɫ��
	map<Checkerboard::Side, ID> player_side;

	//ָ�򹫹������̶���
	shared_ptr<Checkerboard> ptr_to_cboard;
};


//��������ݿ��л�ȡ�û���Ϣ���Լ��û���Ϣд�����ݿ�
class Recorder;


#endif // GOMOKU_H
