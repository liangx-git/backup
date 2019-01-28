#include "gomoku-test.h"
#include<iostream>
int main(int argc, char *argv[])
{
	shared_ptr<Checkerboard> ptr_cb = std::make_shared<Checkerboard>();
	ptr_cb->PrintCBoard();
	shared_ptr<Player> player_1 = std::make_shared<HumanPlayer>(ptr_cb, 1, Checkerboard::BLACK);  //���1
																								  //shared_ptr<Player> player_2 = std::make_shared<HumanPlayer>(ptr_cb, 2, Checkerboard::WHITE);  //���2
	shared_ptr<Player> computer_player = std::make_shared<ComputerPlayer>(ptr_cb);
	Judge judge(ptr_cb);    //����
	while (true)
	{
		//�û����1
		player_1->PutAPiece();
		if (judge.IsWin())
			judge.AnnounceWinerAndFinishGame();

		//�������
		computer_player->PutAPiece();
		if (judge.IsWin())
			judge.AnnounceWinerAndFinishGame();

		//�û����2
		//        player_2 -> PutAPiece();
		//        if(judge.IsWin())
		//            judge.AnnounceWinerAndFinishGame();
	}

	return 0;
}
