#include "gomoku-test.h"
#include<algorithm>

#define INFINITY 50000

/******************************Checkerboard****************************/

void Checkerboard::PrintCBoard()
{
	cout << endl;
	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			if (status[y][x] == Checkerboard::NONE)
				cout << '*';
			else if (status[y][x] == Checkerboard::BLACK)
				cout << '1';
			else if (status[y][x] == Checkerboard::WHITE)
				cout << '2';
			cout << ' ';

		}
		if (y == 0)
			cout << 'x';
		cout << endl;
	}
	cout << 'y' << endl;
	//TODO ͨ��qt�������
}

void Checkerboard::UpdateCBoard(const Position &pos)
{
	if (PosIsAvilable(pos))
	{
		//TODO��������
		cur_pos = pos;
		status[pos.second][pos.first] = cur_side;
		PrintCBoard();
	}
}

bool Checkerboard::PosOutOfRange(const Position &pos)
{
	return !(pos.first >= 0 && pos.first < width          //ˮƽ����δԽ��
		&& pos.second >= 0 && pos.second < height);    //��ֱ����δԽ��
}

bool Checkerboard::PosIsAvilable(const Position &pos)
{
	return (!PosOutOfRange(pos)
		&& status[pos.second][pos.first] == NONE);     //��λ����û������
}

//��̬������ʼ��
const Direction Checkerboard::directions{ { 0, -1 },{ 1, -1 },{ 1, 0 },{ 1, 1 },{ 0, 1 },{ -1, 1 },{ -1, 0 },{ -1, -1 } };



/******************************Player****************************/

void Player::PutAPiece()
{
	ptr_to_cboard->cur_side = side;

	//���ò���ģʽ(NVI��ͨ������ӿڵ���protect�麯��)
	Position next_pos = GetNextPos();
	ptr_to_cboard->UpdateCBoard(next_pos);
}



/**********HumanPlayer********/

Position HumanPlayer::GetNextPos()
{
	return GetNextPosFromIO();
}

Position HumanPlayer::GetNextPosFromIO()
{
	cout << id << "! ���������꣺" << endl;
	int x, y;
	cin >> x >> y;
	return Position(x, y);
}



/******************************ComputerPlayer****************************/


Position ComputerPlayer::GetNextPos()
{
	//Ĭ��ʹ�ü�AI
	return GetPosBySimpleWay();
	//    return GetPosByHardWay();
	//    if (game_difficulty == NORMAL)
	//        return GetPosBySimpleWay();
	//    else
	//        return GetPosByHardWay();
}

void ComputerPlayer::SetDifficulty(ComputerPlayer::Difficulty diff)
{
	game_difficulty = diff;
}

Position ComputerPlayer::GetPosByHardWay()
{
	int max_score = 0;
	Position best_pos;
	int width = ptr_to_cboard->width;
	int height = ptr_to_cboard->height;
	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			Position new_pos = std::make_pair(x, y);
			if (ptr_to_cboard->PosIsAvilable(new_pos))
			{
				vector<vector<Checkerboard::Side>> temp_status(ptr_to_cboard->status);
				temp_status[y][x] = Checkerboard::PREPARE;
				int new_score = AlphaBeta(new_pos, 1, -INFINITY, INFINITY, ReverseSide(side), temp_status);
				if (new_score > max_score)
				{
					max_score = new_score;
					best_pos = new_pos;
				}
			}
		}
	}
	return best_pos;
}

int ComputerPlayer::AlphaBeta(Position pos, int depth, int alp, int beta, Checkerboard::Side side, vector<vector<Checkerboard::Side>> status)
{
	if (depth == 0 || !HasNextChildPos(status))
		return Evaluate(pos, status);

	if (side == ComputerPlayer::side)   //����ֵ��
	{
		while (HasNextChildPos(status))
		{
			Position child_pos = GetNextChildPos(side, status);
			alp = std::max(alp, AlphaBeta(child_pos, depth - 1, alp, beta, ReverseSide(side), UpdateStatus(status, child_pos, side)));
			SetPosCheck(status, child_pos); //���ø��ӽڵ�λ��Ϊ��̽��
			if (beta <= alp)    //��֦
				break;
		}
		return alp;
	}
	else
	{
		while (HasNextChildPos(status))  //��Сֵ��
		{
			Position child_pos = GetNextChildPos(side, status);
			beta = std::min(beta, AlphaBeta(child_pos, depth - 1, alp, beta, ReverseSide(side), UpdateStatus(status, child_pos, side)));
			SetPosCheck(status, child_pos); //���ø��ӽڵ�λ��Ϊ��̽��
			if (beta <= alp)    //��֦
				break;
		}
		return beta;
	}
}

bool ComputerPlayer::HasNextChildPos(const vector<vector<Checkerboard::Side>> &status)
{
	int width = ptr_to_cboard->width;
	int height = ptr_to_cboard->height;
	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			if (status[y][x] == Checkerboard::NONE
				&& status[y][x] != Checkerboard::CHECK)
				return true;
		}
	}
	return false;
}

Position ComputerPlayer::GetNextChildPos(Checkerboard::Side s, const vector<vector<Checkerboard::Side>> &status)
{
	int width = ptr_to_cboard->width;
	int height = ptr_to_cboard->height;
	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			if (status[y][x] == Checkerboard::NONE
				&& status[y][x] != Checkerboard::CHECK)
				return std::make_pair(x, y);
		}
	}
}

void ComputerPlayer::SetPosCheck(vector<vector<Checkerboard::Side> > &status, const Position &pos)
{
	status[pos.second][pos.first] = Checkerboard::CHECK; //���Ϊ����̽��
}

vector<vector<Checkerboard::Side> > ComputerPlayer::UpdateStatus(const vector<vector<Checkerboard::Side> > &status, const Position &pos, Checkerboard::Side side)
{
	vector<vector<Checkerboard::Side> > temp_status(status);
	temp_status[pos.second][pos.first] = side;
	return temp_status;
}

Position ComputerPlayer::GetPosBySimpleWay()
{
	//����ȫ�̿������ӵĵ㣬�ҵ������ͶԷ�������ߵĵ�
	int ai_max_score = 0;
	int man_max_score = 0;
	int temp_score = 0;
	Position max_pos, max_oppo_pos;
	for (int y = 0; y < ptr_to_cboard->height; ++y)
	{
		for (int x = 0; x < ptr_to_cboard->width; ++x)
		{
			if (ptr_to_cboard->status[y][x] == Checkerboard::NONE)
			{
				Position pos = std::make_pair(x, y);
				temp_score = EvaluateOneSide(pos, side, ptr_to_cboard->status);
				if (temp_score > ai_max_score)      //�����������������
				{
					ai_max_score = temp_score;
					max_pos = pos;
					if (ai_max_score > Checkerboard::WIN)
						return max_pos;
				}
				temp_score = EvaluateOneSide(pos, ReverseSide(side), ptr_to_cboard->status);
				if (temp_score > man_max_score)     //�����û������������
				{
					man_max_score = temp_score;
					max_oppo_pos = pos;
				}
			}
		}
	}

	return (ai_max_score > man_max_score) ? max_pos : max_oppo_pos;
}

int ComputerPlayer::Evaluate(const Position &pos, vector<vector<Checkerboard::Side> > status)
{
	int com_side_score = EvaluateOneSide(pos, side, status);
	int human_side_score = EvaluateOneSide(pos, ReverseSide(side), status);
	return com_side_score > human_side_score ? com_side_score : human_side_score;
}

int ComputerPlayer::EvaluateOneSide(const Position &pos, Checkerboard::Side s, vector<vector<Checkerboard::Side>> status)
{
	int score = 0;
	map<Checkerboard::State, int> state_info;
	GetAllDireStateInfo(pos, s, status, state_info);

	//��һ��ѭ���ҵ����ڵĳ�����3_3���ֺ�4_4����
	for (auto &state : state_info)
	{
		if ((state.first & 0x00ff) > 5)   //���ҳ���
		{
			score += ptr_to_cboard->score_sheet[Checkerboard::CHANG_n];
			state.second = 0;
		}
		else if (state.first == Checkerboard::HUO_3 //����3_3����
			&& state.second == 2)
		{
			score += ptr_to_cboard->score_sheet[Checkerboard::JIN_3_3];
			state.second = 0;
		}

		else if (state.first == Checkerboard::HUO_4     //����4_4����
			&& state.second == 2)
		{
			score += ptr_to_cboard->score_sheet[Checkerboard::JIN_4_4];
			state.second = 0;
		}
	}

	//����ʣ��÷����
	for (auto state : state_info)
	{
		if (state.second != 0
			&& (ptr_to_cboard->score_sheet).find(state.first) != (ptr_to_cboard->score_sheet).end())    //��score_sheet�����ҵ���state����
			score += ptr_to_cboard->score_sheet[state.first];
	}
	return score;
}

void ComputerPlayer::GetAllDireStateInfo(const Position &pos, Checkerboard::Side s, vector<vector<Checkerboard::Side>> status, map<Checkerboard::State, int> &state_info)
{
	//˳ʱ�����posλ�õİ˸�С�����ϱ�����¼�������������Լ�ĩβλ�����
	vector<pair<int, bool>> temp;
	for (int d = 0; d < 8; ++d)
	{
		int len = CalcPiecesInCurrDirection(pos, s, d, status);
		bool end_is_empty = false;
		if (EndPosIsEmpty(pos, d, len, status))
			end_is_empty = true;
		temp.push_back(std::make_pair(len, end_is_empty));
	}

	//����temp��Ϣ������pos����ĸ������ϵ�State��Ϣ
	unsigned short state = 0;
	for (int d = 0; d < 4; ++d)
	{
		//�������
		if (temp[d].second == true
			&& temp[d + 4].second == true)
			state |= Checkerboard::HUO;     //��
		else if (temp[d].second == false
			&& temp[d + 4].second == false)
			state |= Checkerboard::SI;      //��
		else
			state |= Checkerboard::CHONG;   //��

											//��������ϵĳ���
		state = state << 8;     //�Ͱ�λ��ʾ����
		state = temp[d].first + temp[d + 4].first + 1;

		++state_info[Checkerboard::State(state)];
	}
}

int ComputerPlayer::CalcPiecesInCurrDirection(const Position &pos, Checkerboard::Side s, int dire, vector<vector<Checkerboard::Side>> status)
{
	int piece_counts = 0;
	Position cur_pos = pos;
	MovePos(cur_pos, dire);
	while (!ptr_to_cboard->PosOutOfRange(cur_pos)     //ȷ���������������
		&& s == status[cur_pos.second][cur_pos.first])
	{
		++piece_counts;
		MovePos(cur_pos, dire);
	}
	return piece_counts;
}

Checkerboard::Side ComputerPlayer::ReverseSide(Checkerboard::Side s)
{
	if (s = Checkerboard::WHITE)
		return Checkerboard::BLACK;
	else
		return Checkerboard::WHITE;
}

bool ComputerPlayer::EndPosIsEmpty(const Position &pos, int dire, int len, vector<vector<Checkerboard::Side>> status)
{
	Position end_pos = pos;
	for (int i = 0; i <= len; ++i)
	{
		MovePos(end_pos, dire);
		if (ptr_to_cboard->PosOutOfRange(end_pos))     //λ�ó���
			return false;
	}
	return status[end_pos.second][end_pos.first] == Checkerboard::NONE;
}

void ComputerPlayer::MovePos(Position &pos, int dire)
{
	pos.first += Checkerboard::directions[dire].first;
	pos.second += Checkerboard::directions[dire].second;
}



/******************************Judge****************************/
bool Judge::IsWin()
{
	Position cur_pos = ptr_to_cboard->cur_pos;
	//�洢�ĸ��������������ӵĸ���,���ڱ����ĸ�������ж��Ƿ�����������
	vector<int> pieces_of_all_direction(4, 0);

	//˳ʱ������˸������ϱ�����¼����������
	for (int d = 0; d < 8; ++d)
	{
		pieces_of_all_direction[d % 4] += CalcPiecesInCurrDirection(cur_pos, d);
	}

	//�����ĸ��������Ƿ���������������������ǰλ�ã��ĸ������㣩
	for (auto nums : pieces_of_all_direction)
	{
		if (nums >= 4)
			return true;
	}
	return false;
}

int Judge::CalcPiecesInCurrDirection(const Position &pos, int dire)
{
	Side player_side = ptr_to_cboard->cur_side;
	int piece_counts = 0;
	Position cur_pos = pos;
	MovePos(cur_pos, dire);
	while (!ptr_to_cboard->PosOutOfRange(cur_pos)     //ȷ���������������
		&& player_side == ptr_to_cboard->status[cur_pos.second][cur_pos.first])
	{
		++piece_counts;
		MovePos(cur_pos, dire);
	}
	return piece_counts;
}

void Judge::MovePos(Position &pos, int dire)
{
	pos.first += Checkerboard::directions[dire].first;
	pos.second += Checkerboard::directions[dire].second;
}

void Judge::SetPlayersInfo(pair<Checkerboard::Side, ID> & player_info)
{
	player_side[player_info.first] = player_info.second;
}

void Judge::AnnounceWinerAndFinishGame()
{
	//TODO
	cout << "player" << ptr_to_cboard->cur_side << "is winer!" << endl;
	exit(0);
}





