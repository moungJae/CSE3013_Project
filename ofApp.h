#pragma once

#include "ofMain.h"

class ofApp : public ofBaseApp {

public:
	ofSoundPlayer mySound;

	void setup();
	void update();
	void draw();

	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);
	void score_Update();
	void createMap();
	void deleteMap();
	void create_Player_Balloon();
	void create_Enemy_Balloon(int index);
	void create_Player_Location(int x, int y);
	void create_Enemy_Location(int x, int y,int index);
	void info_Init();
	void visited_Init();
	void memory_Allocation_Info();
	void memory_Free();
	bool find_Player(int startX, int startY, int endX, int endY);
	void enemy_Random_Set(int index);
	void enemy_Random_Play(int index);
	void enemy_Kill_Player(int startX,int startY,int endX,int endY,int index);
	void enemy_Put_Balloon(int index);
	int enemy_Estimated_Score(int x, int y, int index);
	void enemy_Find_Ground(int x, int y, int index);
	void enemy_Move_Ground(int x, int y, int index);
	bool enemy_Is_Alive(int x,int y, int index);
	bool enemy_In_Balloon_Range(int x, int y, int index);
	void enemy_Avoid_Balloon(int index);
	void enemy_Pop_Balloon(int index);

	void go_Up();
	void go_Down();
	void go_Right();
	void go_Left();
	void go_Space();
	void player_Pop_Balloon();
	void player_Life_Decrease();
	void enemy_Life_Decrease(int index);

	void ofApp::processOpenFileSelection(ofFileDialogResult openFileResult);

	struct Enemy {
		int x, y, score, direction;
		int balloon_Power, op, life;
		int balloonX, balloonY;
	};

	struct Player {
		int x, y, score, direction, life;
		int balloon_Power, balloon_Count;
		int current_Balloon_Count;
	};

	Enemy e[4];
	Player p;

	class Player_Balloon_Location {
	public:
		int x, y;
	};

	class Enemy_HighScore_Path {
	public:
		int x, y, score;
		Enemy_HighScore_Path(int x, int y, int score) {
			this->x = x;
			this->y = y;
			this->score = score;
		}
	};

	vector<Player_Balloon_Location> pb_V; 
	vector<vector<Enemy_HighScore_Path>> em_V[4]; 
	vector<Enemy_HighScore_Path> e_V[4]; 
	int e_Index[4];

	int enemy_Location[4][7][2] = { 
	{
		{0,0}, {0,1}, {0,2}, {1,2}, {-1,-1}, {-1, -1}, {-1, -1}
	},
	{
		{0,11}, {1,10}, {1,11}, {1,12}, {-1, -1}, {-1, -1}, {-1, -1}
	},
	{
		{14,1}, {13,0},	{13,1},	{12,1},	{11,0},	{11,1},	{10,1}
	},
	{
		{14,10}, {14,11}, {14,12}, {13,12}, {-1, -1}, {-1,-1}, {-1,-1}
	}
	};

	int load_flag, draw_flag;
	int Map_Width, Map_Height;
	int play_Time, game_Over;
	char **Map; 
	char **tempMap;
	int dx[4], dy[4];
	int play_Random[4];
	int **visited;
	map<int, int> flow_Block;
	map<int, int> enemy_Map;  
};
