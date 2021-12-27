#include "ofApp.h"

// 20192138 조명재 
// 컴퓨터공학설계및실험1 기말 프로젝트

const int OF_KEY_SPACE = 32; // ASCII Code Value of Space Key

//--------------------------------------------------------------
void ofApp::setup(){ // Set the game
	ofSetFrameRate(15);
	ofBackground(255);
	ofSetLineWidth(4);

	load_flag = 0, draw_flag = 0;
	play_Time = 0, game_Over = 0;
	Map_Width = 0, Map_Height = 0;

	// Determine the Location with a dx and dy index
	// 0 => Right, 1 => Left, 2 => Down, 3 => Up
	dx[0] = 1, dx[1] = -1, dx[2] = 0, dx[3] = 0; 
	dy[0] = 0, dy[1] = 0, dy[2] = 1, dy[3] = -1;

	srand(time(NULL));
}

void ofApp::visited_Init() {
	for (int i = 0; i < Map_Width; i++) { // Initialize the visited to false.
		for (int j = 0; j < Map_Height; j++) {
			visited[i][j] = false;
		}
	}
}

void ofApp::player_Life_Decrease() { // If the player touches the balloon or encounters the enemy, Player life is decrease.
	if(play_Time) p.life--;
	if (p.life == 0) { // If player life is 0, Game over.
		game_Over = 1;
		cout << "Game Over!!! => Player's life : " << p.life <<" Player's final score : " << p.score << endl;
		mySound.load("Crazy Arcade Lose.mp3");
		mySound.play();
		play_Time = -1;
	}
	else {
		if(p.life) cout << "Player life's : " << p.life << endl;
	}
}

void ofApp::enemy_Life_Decrease(int index) { // If the Enemy touches the balloon, Enemy life is decrease.
	if(play_Time) e[index].life--;
	if (e[index].life == 0) { // If Enemy life is 0, Enemy disappears on the map.
		if (e[index].balloonX >= 0 && e[index].balloonY >= 0) {
			Map[e[index].balloonX][e[index].balloonY] = 'G';
			e[index].balloonX = -1, e[index].balloonY = -1;
		}
		e[index].x = -1, e[index].y = -1; 
		e[index].op = 0;
		p.score += 100;
		cout << "Enemy" << index+1 << " is slayed! => Enemy" << index+1 << "'s life : " << e[index].life << endl;
	}
	else {
		cout << "Enemy" << index+1 << "'s life : " << e[index].life << endl;
	}
	
	int cnt = 0;
	for (int i = 0; i < 4; i++) {
		if (e[i].life == 0) cnt++;
	}
	if (cnt == 4) { // If all enemys have been slayed, Player Win. 
		cout << "Player Win! => Player's life : " << p.life << " Player's final score : " << p.score << endl; 
		play_Time = -1;
		mySound.load("Crazy Arcade Win.mp3");
		mySound.play();
	}
}

bool ofApp::find_Player(int startX, int startY, int endX, int endY) {
	// Check that the Enemy can find the Player by using Breadth First Search. 
	visited_Init(); // Initialize 2D Array visited to false.
	queue<pair<int, int>> q; 
	q.push({ startX,startY });
	visited[startX][startY] = true;
	while (!q.empty()) { // Through process of Queue's push and pop, Enemy searchs the location of player.
		int curX = q.front().first;
		int curY = q.front().second;
		q.pop();
		if (curX == endX && curY == endY) return true; // If Enemy can search the location of player, return true.
		for (int i = 0; i < 4; i++) { // save the next path to the queue
			int nextX = curX + dx[i];
			int nextY = curY + dy[i];
			if (nextX >= 0 && nextX < Map_Width && nextY >= 0 && nextY < Map_Height) {
				if (!visited[nextX][nextY] && Map[nextX][nextY]=='G') {
					q.push({ nextX,nextY });
					visited[nextX][nextY] = true;
				}
			}
		}
	}
	return false; // If Enemy can't search the location of player, return false.
}

void ofApp::enemy_Kill_Player(int startX,int startY,int endX,int endY, int index) {
	// Enemy searches the location of player by using BFS.
	e[index].op = 0;
	visited_Init(); // Initialize 2D Array visited to false.
	queue<pair<pair<int, int>, vector<pair<int,int>>>> q;
	vector<pair<int,int>> path_V; // Save the paths that pass in vector of path_V.
	path_V.push_back({ startX, startY });
	q.push({ { startX, startY }, path_V });
	visited[startX][startY] = true;

	// Find the case that the Enemy move the ground.
	while (!q.empty()) {
		int curX = q.front().first.first;
		int curY = q.front().first.second;
		vector<pair<int,int>> path = q.front().second;
		q.pop();
		if (curX == endX && curY == endY) {
			// Move closer to the player.
			int nextX = path[1].first;
			int nextY = path[1].second;
			if (nextX - e[index].x == 1) { // Go to Right 
				e[index].direction = 0;
			}
			else if (e[index].x - nextX == 1) { // Go to Left 
				e[index].direction = 1;
			}
			else if (nextY - e[index].y == 1) { // Go to Down 
				e[index].direction = 2;
			}
			else if (e[index].y - nextY == 1) { // Go to Up 
				e[index].direction = 3;
			}
			e[index].x = nextX, e[index].y = nextY;
			if (e[index].x == p.x && e[index].y == p.y) {
				player_Life_Decrease();
			}
			return;
		}
		for (int i = 0; i < 4; i++) { // save the next path to the queue
			int nextX = curX + dx[i];
			int nextY = curY + dy[i];
			if (nextX >= 0 && nextX < Map_Width && nextY >= 0 && nextY < Map_Height) {
				if (Map[nextX][nextY] == 'G' && !visited[nextX][nextY]) {
					path.push_back({ nextX,nextY });
					q.push({ {nextX,nextY},path });
					path.pop_back();
					visited[nextX][nextY] = true;
				}
			}
		}
	}
}

bool ofApp::enemy_In_Balloon_Range(int x,int y, int index) { // Check that enemy is within the range of the balloon.
	if (x == e[index].balloonX && y == e[index].balloonY) return true; 

	// Right case
	int nextX = e[index].balloonX + e[index].balloon_Power;
	if (x > e[index].balloonX && x <= nextX && y == e[index].balloonY) return true;
	// Left case
	nextX = e[index].balloonX - e[index].balloon_Power;
	if (x < e[index].balloonX && x >= nextX && y == e[index].balloonY) return true;
	
	// Down case
	int nextY = e[index].balloonY + e[index].balloon_Power;
	if (y > e[index].balloonY && y <= nextY && x == e[index].balloonX) return true;
	// Up case
	nextY = e[index].balloonY - e[index].balloon_Power;
	if (y < e[index].balloonY && y >= nextY && x == e[index].balloonX) return true;

	// All case are not satisfied? => Get out the balloon Range.
	return false;
}

void ofApp::enemy_Move_Ground(int x,int y, int index) { // Find if enemy can move to the ground.
	queue<pair<pair<int, int>,vector<pair<int,int>>>> q;
	vector<pair<int,int>> path_V; // Save the paths that pass in vector of path_V.
	q.push({ { x, y }, path_V });
	visited[x][y] = true;

	// Find a case that enemy can move to the ground.
	while (!q.empty()) {
		int curX = q.front().first.first;
		int curY = q.front().first.second;
		vector<pair<int,int>> path = q.front().second;
		q.pop();
		if (!enemy_In_Balloon_Range(curX, curY, index)) { // enemy find the ground? => change the enemy location.
			int nextX = path[0].first;
			int nextY = path[0].second;
			if (nextX - e[index].x == 1) { // Go to Right. 
				e[index].direction = 0;
			}
			else if (e[index].x - nextX == 1) { // Go to Left. 
				e[index].direction = 1;
			}
			else if (nextY - e[index].y == 1) { // Go to Down. 
				e[index].direction = 2;
			}
			else if (e[index].y - nextY == 1) { // Go to Up. 
				e[index].direction = 3;
			}
			e[index].x = nextX, e[index].y = nextY;
			return;
		}
		for (int i = 0; i < 4; i++) {
			int nextX = curX + dx[i];
			int nextY = curY + dy[i];
			if (nextX >= 0 && nextX < Map_Width && nextY >= 0 && nextY < Map_Height) {
				if (Map[nextX][nextY] == 'G' && !visited[nextX][nextY]) {
					visited[nextX][nextY] = true;
					path.push_back({ nextX,nextY });
					q.push({ {nextX,nextY}, path });
					path.pop_back();
				}
			}
		}
	}

}

void ofApp::enemy_Pop_Balloon(int index) {
	visited_Init(); // Initialize 2D Array visited to false.
	
	// Set that the enemy pops the balloon. 
	int player_Life_Op = 0; // Check that the player's life is decreasing.
	int enemy_Life_Op[4] = { 0,0,0,0 }; // Check that the enemy's life is decreasing.
	int currentX = e[index].balloonX, currentY = e[index].balloonY, power = e[index].balloon_Power;
	int e_loc = currentY * Map_Width + currentX; // convert two-dimensional coordinates to index.
	enemy_Map[e_loc] = 0; // Remove the enemy's balloon
	e[index].op = 0, e[index].balloonX = -1, e[index].balloonY = -1; // Get away the balloon.
	Map[currentX][currentY] = 'G'; 
	
	for (int i = 0; i < 4; i++) { // Pop the balloon in left, right, side to side.
		// Let the balloon be as effective as the enemy's power. 
		int nextX = currentX + dx[i] * power;
		int nextY = currentY + dy[i] * power;
		// Is there a block that needs to be removed by approaching one space in left, right, side to side?
		if (i == 0) { // Find the block in right.
			for (int x = currentX; x <= nextX; x++) {
				if (x >= Map_Width || visited[x][currentY]) break; // Don't invade the spot where the balloon pop.
				if (x == p.x && currentY == p.y) { // If the current player's location is within the range of the balloon range, decrease the player life.
					if (!player_Life_Op) {
						player_Life_Decrease();
						player_Life_Op = 1;
					}
				}
				for (int k = 0; k < 4; k++) { // If the current enemy's location is within the range of the balloon range, decrease the enemy life.
					if (k == index) continue;
					if (x == e[k].x && currentY == e[k].y) {
						if (!enemy_Life_Op[k]) {
							enemy_Life_Decrease(k);
							enemy_Life_Op[k] = 1;
						}
					}
				}
				if (Map[x][currentY] == 'B' || Map[x][currentY] == 'X' || Map[x][currentY] == 'F') {
					// Block, Box, Flow_Box removed by balloon => Each different score.
					if (Map[x][currentY] == 'B') {
						e[index].score += 10;
						Map[x][currentY] = 'G'; // Block removed by balloon => change the ground 
						visited[x][currentY] = true; // The location is pop, so visit processing is essential. 
												    // (To keep the other balloon from crossing its position)
					}
					else if (Map[x][currentY] == 'X') {
						e[index].score += 15;
						Map[x][currentY] = 'G';  // Box removed by balloon => change the ground 
						visited[x][currentY] = true; // The location is pop, so visit processing is essential.
												    // (To keep the other balloon from crossing its position)
					}
					else if (Map[x][currentY] == 'F') {
						int loc_index = currentY * Map_Width + x; // convert the location to index.
						if (++flow_Block[loc_index] == 3) { // If balloon pop by weight(3)?
							e[index].score += 20;
							Map[x][currentY] = 'G'; // Flow_Box removed by balloon => change the ground 
							visited[x][currentY] = true; // The location is pop, so visit processing is essential.
												        // (To keep the other balloon from crossing its position)
						}
						else {
							visited[x][currentY] = true; // The location is pop, so visit processing is essential.
												        // (To keep the other balloon from crossing its position)
						}
					}
					break;
				}
				else if (Map[x][currentY] == 'H' || Map[x][currentY] == 'T' || Map[x][currentY] == 'W')
				{
					break; // Case of House, Tree, Water Balloon touched =>  Finish the balloon effect in direction.
				}
			}
		}
		else if (i == 1) { // If you want information, Check i==0.
			for (int x = currentX; x >= nextX; x--) {
				if (x < 0 || visited[x][currentY]) break;
				if (x == p.x && currentY == p.y) {
					if (!player_Life_Op) {
						player_Life_Decrease();
						player_Life_Op = 1;
					}
				}
				for (int k = 0; k < 4; k++) {
					if (k == index) continue;
					if (x == e[k].x && currentY == e[k].y) {
						if (!enemy_Life_Op[k]) {
							enemy_Life_Decrease(k);
							enemy_Life_Op[k] = 1;
						}
					}
				}
				if (Map[x][currentY] == 'B' || Map[x][currentY] == 'X' || Map[x][currentY] == 'F') {
					if (Map[x][currentY] == 'B') {
						e[index].score += 10;
						Map[x][currentY] = 'G';
						visited[x][currentY] = true;
					}
					else if (Map[x][currentY] == 'X') {
						e[index].score += 15;
						Map[x][currentY] = 'G';
						visited[x][currentY] = true;
					}
					else if (Map[x][currentY] == 'F') {
						int loc_index = currentY * Map_Width + x;
						if (++flow_Block[loc_index] == 3) {
							e[index].score += 20;
							Map[x][currentY] = 'G';
							visited[x][currentY] = true;
						}
						else {
							visited[x][currentY] = true;
						}
					}
					break;
				}
				else if (Map[x][currentY] == 'H' || Map[x][currentY] == 'T' || Map[x][currentY] == 'W')
				{
					break;
				}
			}
		}
		else if (i == 2) { // If you want information, Check i==0.
			for (int y = currentY; y <= nextY; y++) {
				if (y >= Map_Height || visited[currentX][y]) break;
				if (currentX == p.x && y == p.y) {
					if (!player_Life_Op) {
						player_Life_Decrease();
						player_Life_Op = 1;
					}
				}
				for (int k = 0; k < 4; k++) {
					if (k == index) continue;
					if (currentX == e[k].x && y == e[k].y) {
						if (!enemy_Life_Op[k]) {
							enemy_Life_Decrease(k);
							enemy_Life_Op[k] = 1;
						}
					}
				}
				if (Map[currentX][y] == 'B' || Map[currentX][y] == 'X' || Map[currentX][y] == 'F') {
					if (Map[currentX][y] == 'B') {
						e[index].score += 10;
						Map[currentX][y] = 'G';
						visited[currentX][y] = true;
					}
					else if (Map[currentX][y] == 'X') {
						e[index].score += 15;
						Map[currentX][y] = 'G';
						visited[currentX][y] = true;
					}
					else if (Map[currentX][y] == 'F') {
						int loc_index = y * Map_Width + currentX;
						if (++flow_Block[loc_index] == 3) {
							e[index].score += 20;
							Map[currentX][y] = 'G';
							visited[currentX][y] = true;
						}
						else {
							visited[currentX][y] = true;
						}
					}
					break;
				}
				else if (Map[currentX][y] == 'H' || Map[currentX][y] == 'T' || Map[currentX][y] == 'W')
				{
					break;
				}
			}
		}
		else if (i == 3) { // If you want information, Check i==0.
			for (int y = currentY; y >= nextY; y--) {
				if (y < 0 || visited[currentX][y]) break;
				if (currentX == p.x && y == p.y) {
					if (!player_Life_Op) {
						player_Life_Decrease();
						player_Life_Op = 1;
					}
				}
				for (int k = 0; k < 4; k++) {
					if (k == index) continue;
					if (currentX == e[k].x && y == e[k].y) {
						if (!enemy_Life_Op[k]) {
							enemy_Life_Decrease(k);
							enemy_Life_Op[k] = 1;
						}
					}
				}
				if (Map[currentX][y] == 'B' || Map[currentX][y] == 'X' || Map[currentX][y] == 'F') {
					if (Map[currentX][y] == 'B') {
						e[index].score += 10;
						Map[currentX][y] = 'G';
						visited[currentX][y] = true;
					}
					else if (Map[currentX][y] == 'X') {
						e[index].score += 15;
						Map[currentX][y] = 'G';
						visited[currentX][y] = true;
					}
					else if (Map[currentX][y] == 'F') {
						int loc_index = y * Map_Width + currentX;
						if (++flow_Block[loc_index] == 3) {
							e[index].score += 20;
							Map[currentX][y] = 'G';
							visited[currentX][y] = true;
						}
						else {
							visited[currentX][y] = true;
						}
					}
					break;
				}
				else if (Map[currentX][y] == 'H' || Map[currentX][y] == 'T' || Map[currentX][y] == 'W')
				{
					break;
				}
			}
		}
	}
}

void ofApp::enemy_Avoid_Balloon(int index) {
	if (enemy_In_Balloon_Range(e[index].x, e[index].y, index)) { // Current location is in a balloon range
		visited_Init(); // Initialize 2D Array visited to false.
		// find the closet distance to the ground and move there
		enemy_Move_Ground(e[index].x, e[index].y, index);
	}
	else { // Current location is not in a balloon range
		enemy_Pop_Balloon(index);
	}
}

int ofApp::enemy_Estimated_Score(int x,int y, int index) {
	int currentX = x, currentY = y;
	int power = e[index].balloon_Power;
	int score = 0;

	// Estimate how many scores will be in the (x, y) coordinates of the map.
	for (int i = 0; i < 4; i++) {  
		int nextX = currentX + dx[i] * power;
		int nextY = currentY + dy[i] * power;
		// Is there a block that needs to be removed by approaching one space from top to bottom ?
		if (i == 0) { // Find the block in right.
			for (int x = currentX; x <= nextX; x++) {
				if (x >= Map_Width) break; 
				if (Map[x][currentY] == 'B' || Map[x][currentY] == 'X' || Map[x][currentY] == 'F') {
					// Block, Box, Flow_Box removed by balloon => Each different score.
					if (Map[x][currentY] == 'B') {
						score += 10;
					}
					else if (Map[x][currentY] == 'X') {
						score += 15;
					}
					else if (Map[x][currentY] == 'F') {
						score += 20;
					}
					break;
				}
				else if (Map[x][currentY] == 'H' || Map[x][currentY] == 'T' || Map[x][currentY] == 'W')
				{
					break; // Case of House, Tree, Water Balloon touched =>  Finish the balloon effect in direction.
				}
			}
		}
		else if (i == 1) { // If you want information, Check i==0.
			for (int x = currentX; x >= nextX; x--) {
				if (x < 0) break;
				if (Map[x][currentY] == 'B' || Map[x][currentY] == 'X' || Map[x][currentY] == 'F') {
					if (Map[x][currentY] == 'B') {
						score += 10;
					}
					else if (Map[x][currentY] == 'X') {
						score += 15;
					}
					else if (Map[x][currentY] == 'F') {
						score += 20;
					}
					break;
				}
				else if (Map[x][currentY] == 'H' || Map[x][currentY] == 'T' || Map[x][currentY] == 'W')
				{
					break;
				}
			}
		}
		else if (i == 2) { // If you want information, Check i==0.
			for (int y = currentY; y <= nextY; y++) {
				if (y >= Map_Height) break;
				if (Map[currentX][y] == 'B' || Map[currentX][y] == 'X' || Map[currentX][y] == 'F') {
					if (Map[currentX][y] == 'B') {
						score += 10;
					}
					else if (Map[currentX][y] == 'X') {
						score += 15;
					}
					else if (Map[currentX][y] == 'F') {
						score += 20;
					}
					break;
				}
				else if (Map[currentX][y] == 'H' || Map[currentX][y] == 'T' || Map[currentX][y] == 'W')
				{
					break;
				}
			}
		}
		else if (i == 3) { // If you want information, Check i==0.
			for (int y = currentY; y >= nextY; y--) {
				if (y < 0) break;
				if (Map[currentX][y] == 'B' || Map[currentX][y] == 'X' || Map[currentX][y] == 'F') {
					if (Map[currentX][y] == 'B') {
						score += 10;
					}
					else if (Map[currentX][y] == 'X') {
						score += 15;
					}
					else if (Map[currentX][y] == 'F') {
						score += 20;
					}
					break;
				}
				else if (Map[currentX][y] == 'H' || Map[currentX][y] == 'T' || Map[currentX][y] == 'W')
				{
					break;
				}
			}
		}
	}

	return score;
}

bool ofApp::enemy_Is_Alive(int x, int y, int index) {
	visited_Init(); // Initialize 2D Array visited to false.
	queue<pair<int, int>> q;
	q.push({ x, y });
	visited[x][y] = true;

	// We find that Enemy is moving to the Ground.
	while (!q.empty()) {
		int curX = q.front().first;
		int curY = q.front().second;
		q.pop();
		if (!enemy_In_Balloon_Range(curX, curY, index)) { // Find the Ground
			return true;
		}
		for (int i = 0; i < 4; i++) {
			int nextX = curX + dx[i];
			int nextY = curY + dy[i];
			if (nextX >= 0 && nextX < Map_Width && nextY >= 0 && nextY < Map_Height) {
				if (Map[nextX][nextY] == 'G' && !visited[nextX][nextY]) {
					visited[nextX][nextY] = true;
					q.push({ nextX,nextY });
				}
			}
		}
	}
	return false;
}   

void ofApp::enemy_Find_Ground(int x,int y, int index) {
	// When you use the BFS to go to a point from the current point and place a water balloon,
	// make sure to store the path to the highest score.
	// The saved paths are assigned to em_V.
	visited_Init(); // Initialize 2D Array visited to false.
	queue<pair<pair<int, int>, vector<Enemy_HighScore_Path>>> q;
	vector<Enemy_HighScore_Path> path_V; // Save the paths that pass in vector of path_V.
	path_V.push_back({ x,y,enemy_Estimated_Score(x,y,index) });
	em_V[index].push_back(path_V);
	q.push({ { x, y }, path_V });
	visited[x][y] = true;

	// Find that Enemy can move to the ground.
	while (!q.empty()) {
		int curX = q.front().first.first;
		int curY = q.front().first.second;
		vector<Enemy_HighScore_Path> path = q.front().second;
		q.pop();
		for (int i = 0; i < 4; i++) {
			int nextX = curX + dx[i];
			int nextY = curY + dy[i];
			if (nextX >= 0 && nextX < Map_Width && nextY >= 0 && nextY < Map_Height) {
				if (Map[nextX][nextY]=='G' && !visited[nextX][nextY]) {
					path.push_back({ nextX,nextY,enemy_Estimated_Score(nextX,nextY,index) });
					em_V[index].push_back(path);
					q.push({ {nextX,nextY},path });
					path.pop_back();
					visited[nextX][nextY] = true;
				}
			}
		}
	}

	// Find index for the maximum score at a point when you move from the current point to a point.
	int max_Score=0, max_index=0, e_loc=0, e_before_loc=-1;
	for (int i = 0; i < em_V[index].size(); i++) {
		int score = em_V[index][i][em_V[index][i].size() - 1].score;
		if (score == 0) continue;
		if (score >= max_Score) {
			int move_X = em_V[index][i][em_V[index][i].size() - 1].x;
			int move_Y = em_V[index][i][em_V[index][i].size() - 1].y;
			e_loc = move_Y * Map_Width + move_X;
			e[index].balloonX = move_X, e[index].balloonY = move_Y;
			if (enemy_Is_Alive(move_X,move_Y, index) && !enemy_Map[e_loc]) { // Extract scores from moved points.
				max_Score = score, max_index = i;
				enemy_Map[e_before_loc] = 0;
				enemy_Map[e_loc] = 1;
				e_before_loc = e_loc;
			}
			e[index].balloonX = -1, e[index].balloonY = -1;
		}
	}

	// It contains all the information in index corresponding to the maximum score in the em_V vector in e_V.
	for (int i = 0; i < em_V[index][max_index].size(); i++) {
		e_V[index].push_back({ em_V[index][max_index][i].x,em_V[index][max_index][i].y,em_V[index][max_index][i].score });
	}

	em_V[index].clear(); // Since the role of the em_V vector has ended, free.
	e[index].op = 1;
	enemy_Put_Balloon(index); // Directly to path.
}

void ofApp::enemy_Put_Balloon(int index) {
	if (e_V[index].size() == 1) { // What if the only path is the current Enemy location?
		Map[e[index].x][e[index].y] = 'W';
		e[index].balloonX = e[index].x, e[index].balloonY = e[index].y;
		e[index].op = 2; // Change Balloon to avoid.
	}
	else {
		if (e_Index[index] == e_V[index].size()) { // You have moved to the last point where you can get the maximum score.
			Map[e[index].x][e[index].y] = 'W';
			e[index].balloonX = e[index].x, e[index].balloonY = e[index].y;
			e[index].op = 2; // Change Balloon to avoid.
		}
		else {
			int nextX = e_V[index][e_Index[index]].x;
			int nextY = e_V[index][e_Index[index]].y;
			if (nextX - e[index].x == 1) { // Go to Right.
				e[index].direction = 0;
			}
			else if (e[index].x - nextX == 1) { // Go to Left. 
				e[index].direction = 1;
			}
			else if (nextY - e[index].y == 1) { // Go to Down.
				e[index].direction = 2;
			}
			else if (e[index].y - nextY == 1) { // Go to Up.
				e[index].direction = 3;
			}
			e[index].x = nextX, e[index].y = nextY;
			e_Index[index]++;
		}
	}
}

void ofApp::enemy_Random_Set(int index) {
	e_Index[index] = 1;
	e_V[index].clear();
	em_V[index].clear();
}

void ofApp::enemy_Random_Play(int index) {
	if (e[index].op == 0) { // Process that Enemy finds the ground of high score 	
		enemy_Random_Set(index);
		enemy_Find_Ground(e[index].x, e[index].y, index);
	}
	else if (e[index].op == 1) { // Process that Enemy puts the Water Balloon
		enemy_Put_Balloon(index);
	}
	else if (e[index].op == 2) { // Process that Enemy avoids the Water Balloon
		enemy_Avoid_Balloon(index);
	}
}

void ofApp::score_Update(){ // Show that player and enemy balloon power and count.
	if (p.score) { // Show the player information.
		if (p.score >= 450) {
			if (p.balloon_Count != 10) {
				p.balloon_Count = 10;
				p.balloon_Power = 10;
				cout << "Finish Upgrade! Max Player's Balloon Power : " << p.balloon_Power
					<< " Max Player's Balloon Count : " << p.balloon_Count << endl;
			}
		}
		else {
			int op = p.balloon_Power;
			switch (p.score / 50) {
			case 8:
				p.balloon_Count = p.balloon_Power = 9;
				break;
			case 7:
				p.balloon_Count = p.balloon_Power = 8;
				break;
			case 6:
				p.balloon_Count = p.balloon_Power = 7;
				break;
			case 5:
				p.balloon_Count = p.balloon_Power = 6;
				break;
			case 4:
				p.balloon_Count = p.balloon_Power = 5;
				break;
			case 3:
				p.balloon_Count = p.balloon_Power = 4;
				break;
			case 2:
				p.balloon_Count = p.balloon_Power = 3;
				break;
			case 1:
				p.balloon_Count = p.balloon_Power = 2;
				break;
			}
			if (op != p.balloon_Power) {
				cout << "Upgrade! Player's Balloon Power : " << p.balloon_Power
					<< " Player's Balloon Count : " << p.balloon_Count << endl;
			}
		}
	}
	for (int i = 0; i < 4; i++) { // show the enemys information.
		if (e[i].score) {
			if (e[i].score >= 250) {
				if (e[i].balloon_Power != 6) {
					e[i].balloon_Power = 6;
					cout << "Finish Upgrade! Max Enemy" << i+1 <<"'s Balloon Power : " << e[i].balloon_Power << endl;
				}
			}
			else {
				int op = e[i].balloon_Power;
				switch (e[i].score / 50) {
				case 4:
					e[i].balloon_Power = 5;
					break;
				case 3:
					e[i].balloon_Power = 4;
					break;
				case 2:
					e[i].balloon_Power = 3;
					break;
				case 1:
					e[i].balloon_Power = 2;
					break;
				}
				if (op != e[i].balloon_Power) {
					cout << "Upgrade! Enemy" << i+1 <<"'s Balloon Power : " << e[i].balloon_Power << endl;
				}
			}
		}
	}
}

//--------------------------------------------------------------
void ofApp::update(){
	if (draw_flag && game_Over==0) {
		score_Update();
		if (play_Time > 0) { // If player pressed direction key or space Key, increase the play_Time
			if (play_Time == 1) cout << "Game Start!" << endl;
			play_Time = play_Time++ == 100000 ? 2 : play_Time; // 1 ~ 100000
			// Implement the enemy to move.
			for (int i = 0; i < 4; i++) {
				if (!(e[i].x == p.x && e[i].y == p.y) && e[i].life) { // If Enemy and Player did not meet? 
					int rand_Time = play_Random[i] / 2;
					if (play_Time % rand_Time == 1 && find_Player(e[i].x, e[i].y, p.x, p.y)) { // If Enemy found the Player?
						if (e[i].op == 2) { // If Enemy finds Player while placing the water balloon?
							enemy_Avoid_Balloon(i);
						}
						else {
							enemy_Kill_Player(e[i].x, e[i].y, p.x, p.y, i);
							enemy_Random_Set(i);
						}
					}
					else if (play_Time % play_Random[i] == 1) { // If Enemy does not find the Player?
						enemy_Random_Play(i);
					}
				}
			}
		}
	}
}

void ofApp::createMap() { // Compose the written Map 
	for (int h = 0; h < Map_Height; h++) {
		for (int w = 0; w < Map_Width; w++) {
			// (x1,y1) to (x2,y2) positioned from top left to bottom right
			int x1 = 40 + 65 * w;
			int y1 = 40 + 60 * h;
			int x2 = 40 + 65 * (w + 1);
			int y2 = 40 + 60 * (h + 1);
			if (Map[w][h] == 'G') { // Ground, w is six to eight, Gray, and green.
				if (w == 6 || w == 8) {
					if (h == 2 || h == 10) {
						ofSetColor(255); // White draws a rectangle.
						ofDrawRectangle(x1, y1, x2 - x1, y2 - y1);
						ofSetColor(189, 189, 189); // Gray |] [] [| Draw.
						ofDrawRectangle(x1, y1 + 5, 10, y2 - y1 - 10);
						ofDrawRectangle(x1 + 25, y1 + 5, 15, y2 - y1 - 10);
						ofDrawRectangle(x1 + 55, y1 + 5, 10, y2 - y1 - 10);
					}
					else {
						ofSetColor(189, 189, 189); // Draw a gray rectangle.
						ofDrawRectangle(x1, y1, x2 - x1, y2 - y1);
					}
				}
				else if (w == 7) {
					if (h == 2 || h == 10) {
						ofSetColor(255); // White draws a rectangle.
						ofDrawRectangle(x1, y1, x2 - x1, y2 - y1);
						ofSetColor(189, 189, 189); // Gray |] [] [| Draw.
						ofDrawRectangle(x1, y1 + 5, 10, y2 - y1 - 10);
						ofDrawRectangle(x1 + 25, y1 + 5, 15, y2 - y1 - 10);
						ofDrawRectangle(x1 + 55, y1 + 5, 10, y2 - y1 - 10);
					}
					else {
						ofSetColor(189, 189, 189); // Gray draws a rectangle.
						ofDrawRectangle(x1, y1, x2 - x1, y2 - y1);
						ofSetColor(255); // White [] Draw.
						ofDrawRectangle(x1 + 30, y1 + 5, 5, y2 - y1 - 10);
					}
				}
				else {
					ofSetColor(125, 254, 116); // Green draws a rectangle.
					ofDrawRectangle(x1, y1, x2 - x1, y2 - y1);
					ofSetColor(206, 251, 201); // Draw four shallow green circles.
					ofDrawCircle(x1 + 18, y1 + 16, 11);
					ofDrawCircle(x1 + 18, y1 + 44, 11);
					ofDrawCircle(x1 + 47, y1 + 16, 11);
					ofDrawCircle(x1 + 47, y1 + 44, 11);
				}
			}
			else if (Map[w][h] == 'B') // Block, (w+h) Red if even, Orange if odd.
			{
				if ((w + h) % 2 == 0) {
					ofSetColor(244, 95, 95); // Red rectangle.
					ofDrawRectangle(x1, y1, x2 - x1, y2 - y1);
					ofSetColor(255, 167, 167); // shallow red circle 4.
					ofDrawCircle(x1 + 18, y1 + 14, 10);
					ofDrawCircle(x1 + 18, y1 + 38, 10);
					ofDrawCircle(x1 + 47, y1 + 14, 10);
					ofDrawCircle(x1 + 47, y1 + 38, 10);
					ofSetColor(152, 0, 0); // dark red bottom.
					ofDrawRectangle(x1, y1 + 52, x2 - x1, y2 - (y1 + 52));
				}
				else {
					ofSetColor(242, 150, 97); // Orange Rectnagle.
					ofDrawRectangle(x1, y1, x2 - x1, y2 - y1);
					ofSetColor(255, 193, 158); // 4 shallow orange circles.
					ofDrawCircle(x1 + 18, y1 + 14, 10);
					ofDrawCircle(x1 + 18, y1 + 38, 10);
					ofDrawCircle(x1 + 47, y1 + 14, 10);
					ofDrawCircle(x1 + 47, y1 + 38, 10);
					ofSetColor(153, 56, 0); // dark orange bottom.
					ofDrawRectangle(x1, y1 + 52, x2 - x1, y2 - (y1 + 52));
				}
			}
			else if (Map[w][h] == 'H') // House, 
			{
				if (h < 6) { // Up
					if (w < 6) { // Left
						ofSetColor(204, 114, 61); // Orange Rectangle
						ofDrawRectangle(x1, y1, x2 - x1, y2 - y1);
						ofSetColor(0); // Black in the center
						ofDrawRectangle(x1 + 10, y1 + 20, 45, 40);
						ofSetColor(255); // Door Handle
						ofDrawCircle(x1 + 45, y1 + 40, 3.5);
						ofSetColor(102, 37, 0); // Roof
						ofDrawRectangle(x1, y1, 65, 15);
					}
					else { // Right
						ofSetColor(196, 183, 59); // Dark yelloow Rectangle
						ofDrawRectangle(x1, y1, x2 - x1, y2 - y1);
						ofSetColor(0); // Black in the center
						ofDrawRectangle(x1 + 10, y1 + 20, 45, 40);
						ofSetColor(255); // Door Handle
						ofDrawCircle(x1 + 45, y1 + 40, 3.5);
						ofSetColor(102, 92, 0); // Roof
						ofDrawRectangle(x1, y1, 65, 15);
					}
				}
				else { // Down
					if (w < 6) { // Left
						ofSetColor(70, 65, 217); // Blue Rectangle
						ofDrawRectangle(x1, y1, x2 - x1, y2 - y1);
						ofSetColor(0); // Black in the center
						ofDrawRectangle(x1 + 10, y1 + 20, 45, 40);
						ofSetColor(255); // Door Handle
						ofDrawCircle(x1 + 45, y1 + 40, 3.5);
						ofSetColor(3, 0, 102); // Roof
						ofDrawRectangle(x1, y1, 65, 15);
					}
					else { // Right
						ofSetColor(204, 114, 61); // Orange Rectangle
						ofDrawRectangle(x1, y1, x2 - x1, y2 - y1);
						ofSetColor(0); // Black in the center
						ofDrawRectangle(x1 + 10, y1 + 20, 45, 40);
						ofSetColor(255); // Door handle
						ofDrawCircle(x1 + 45, y1 + 40, 3.5);
						ofSetColor(102, 37, 0); // Roof
						ofDrawRectangle(x1, y1, 65, 15);
					}
				}
			}
			else if (Map[w][h] == 'X') // Box 
			{
				ofSetColor(250, 237, 125);
				ofDrawRectangle(x1, y1, x2 - x1, y2 - y1);
				ofSetColor(153, 138, 0);
				ofDrawRectangle(x1 + 10, y1 + 5, 45, 30);
				ofSetColor(250, 237, 125);
				ofSetLineWidth(5);
				ofDrawLine(x1 + 10, y1 + 5, x1 + 55, y1 + 35);
				ofDrawLine(x1 + 10, y1 + 35, x1 + 55, y1 + 10);
				ofSetColor(229, 216, 92); // both sides of the bottom
				ofDrawRectangle(x1, y1 + 40, x2 - x1, 20);
				ofSetColor(153, 138, 0); // bottom
				ofDrawRectangle(x1 + 10, y1 + 40, 45, 20);
			}
			else if (Map[w][h] == 'T') // Tree
			{
				ofSetColor(34, 116, 28); // shadow
				ofDrawEllipse(x1 + 35, y1 + 35, 55, 40);
				ofSetColor(153, 56, 0); // Bottom column
				ofDrawRectangle(x1 + 30, y1 + 30, 5, 10);
				ofSetColor(47, 157, 39); // Triangle
				ofDrawTriangle(x1, y1 + 30, x1 + 65, y1 + 30, x1 + 33.5, y1 - 20);
			}
			else if (Map[w][h] == 'F') // Flow Block => Composed as a three-step block
			{
				int index = h * Map_Width + w;
				if (flow_Block[index] == 0) { // Initialization
					ofSetColor(102, 92, 0); // Rectangle
					ofDrawRectangle(x1, y1, x2 - x1, y2 - y1);
					ofSetColor(196, 183, 59); // Rectangle
					ofDrawRectangle(x1 + 10, y1 + 10, x2 - x1 - 20, y2 - y1 - 20);
					ofSetColor(255, 255, 72); // Rectangle
					ofDrawRectangle(x1 + 20, y1 + 20, x2 - x1 - 40, y2 - y1 - 40);
				}
				else if (flow_Block[index] == 1) { // step 1
					ofSetColor(102, 92, 0); // Rectangle
					ofDrawRectangle(x1, y1, x2 - x1, y2 - y1);
					ofSetColor(196, 183, 59); // Rectangle
					ofDrawRectangle(x1 + 10, y1 + 10, x2 - x1 - 20, y2 - y1 - 20);
				}
				else if (flow_Block[index] == 2) { // step 2
					ofSetColor(102, 92, 0); // Rectangle
					ofDrawRectangle(x1, y1, x2 - x1, y2 - y1);
				}
				// step 3 is finalized. (convert Flow Block to Ground!) 
			}
		}
	}
}

void ofApp::create_Player_Balloon() { // Show the Player balloon 
	visited_Init(); // Initialize 2D Array visited to false.
	for (int i = 0; i < pb_V.size(); i++) { // Check the current player's Balloon Location + expression.
		int curX = pb_V[i].x, curY = pb_V[i].y;
		visited[curX][curY] = true;

		// Expression of the balloon
		ofSetColor(126, 255, 255);
		ofDrawRectangle(40 + 65 * curX, 40 + 60 * curY, 65, 60);
		ofSetColor(0, 216, 255);
		ofDrawCircle(40 + 65 * curX + 32.5, 40 + 60 * curY + 30, 30);
	}
	for (int i = 0; i < pb_V.size(); i++) { // The range that can be seen in Balloon's Map using Power in the current Player.
		int curX = pb_V[i].x, curY = pb_V[i].y;
		for (int j = 0; j < 4; j++) { 
			int nextX = curX + dx[j] * p.balloon_Power;
			int nextY = curY + dy[j] * p.balloon_Power;
			if (j == 0) // If you find something to pop with balloon in the right direction, you'll be able to process visits to the area and break.
			{
				for (int x = curX + 1; x <= nextX; x++) {
					if (x >= Map_Width || visited[x][curY]) break;
					if (Map[x][curY] == 'H' || Map[x][curY] == 'T') break;
					if (Map[x][curY] == 'B' || Map[x][curY] == 'X' || Map[x][curY] == 'F') // When we encounter a block, box, flow box, No express.
					{
						break;
					}

					// Expression of balloon range
					ofSetColor(126, 255, 255); 
					ofDrawRectangle(40 + 65 * x, 40 + 60 * curY, 65, 60);
					for (int k = 0; k < 4; k++) {
						if (e[k].x == x && e[k].y == curY) {
							int x2 = 40 + 65 * e[k].x, y2 = 40 + 60 * e[k].y;
							if (e[k].life) create_Enemy_Location(x2, y2, k); // Show the Enemy Location
						}
					}
				}
			}
			else if (j == 1) { // If you find something to pop with balloon in the left direction, you'll be able to process visits to the area and break.
				for (int x = curX - 1; x >= nextX; x--) {
					if (x < 0 || visited[x][curY]) break;
					if (Map[x][curY] == 'H' || Map[x][curY] == 'T') break;
					if (Map[x][curY] == 'B' || Map[x][curY] == 'X' || Map[x][curY] == 'F') // When we encounter a block, box, flow box, No express.
					{
						visited[x][curY] = true;
						break;
					}

					// Expression of balloon range
					ofSetColor(126, 255, 255);
					ofDrawRectangle(40 + 65 * x, 40 + 60 * curY, 65, 60);
					for (int k = 0; k < 4; k++) {
						if (e[k].x == x && e[k].y == curY) {
							int x2 = 40 + 65 * e[k].x, y2 = 40 + 60 * e[k].y;
							if (e[k].life) create_Enemy_Location(x2, y2, k); // Show the Enemy Location
						}
					}
				}
			}
			else if (j == 2) { // If you find something to pop with balloon in the down direction, you'll be able to process visits to the area and break.
				for (int y = curY + 1; y <= nextY; y++) {
					if (y >= Map_Height || visited[curX][y]) break;
					if (Map[curX][y] == 'H' || Map[curX][y] == 'T') break;
					if (Map[curX][y] == 'B' || Map[curX][y] == 'X' || Map[curX][y] == 'F') // When we encounter a block, box, flow box, No express.
					{
						visited[curX][y] = true;
						break;
					}

					// Expression of balloon range
					ofSetColor(126, 255, 255);
					ofDrawRectangle(40 + 65 * curX, 40 + 60 * y, 65, 60);
					for (int k = 0; k < 4; k++) {
						if (e[k].x == curX && e[k].y == y) {
							int x2 = 40 + 65 * e[k].x, y2 = 40 + 60 * e[k].y;
							if (e[k].life) create_Enemy_Location(x2, y2, k); // Show the Enemy Location
						}
					}
				}
			}
			else if (j == 3) { // If you find something to pop with balloon in the up direction, you'll be able to process visits to the area and break.
				for (int y = curY - 1; y >= nextY; y--) {
					if (y < 0 || visited[curX][y]) break;
					if (Map[curX][y] == 'H' || Map[curX][y] == 'T') break;
					if (Map[curX][y] == 'B' || Map[curX][y] == 'X' || Map[curX][y] == 'F') // When we encounter a block, box, flow box, No express.
					{
						visited[curX][y] = true;
						break;
					}

					// Expression of balloon range
					ofSetColor(126, 255, 255);
					ofDrawRectangle(40 + 65 * curX, 40 + 60 * y, 65, 60);
					for (int k = 0; k < 4; k++) {
						if (e[k].x == curX && e[k].y == y) {
							int x2 = 40 + 65 * e[k].x, y2 = 40 + 60 * e[k].y;
							if (e[k].life) create_Enemy_Location(x2, y2, k); // Show the Enemy Location
						}
					}
				}
			}
		}
	}
}

void ofApp::create_Enemy_Balloon(int index) {
	// Check the current Balloon Location of Enemy + expression.
	int curX = e[index].balloonX, curY = e[index].balloonY;
	visited[curX][curY] = true;
	ofSetColor(255, 36, 36);
	ofDrawRectangle(40 + 65 * curX, 40 + 60 * curY, 65, 60);
	ofSetColor(152, 0, 0);
	ofDrawCircle(40 + 65 * curX + 32.5, 40 + 60 * curY + 30, 30);

	// The range that can be seen in Balloon's Map using Enemy's Power now.
	for (int i = 0; i < 4; i++) {
		int nextX = curX + dx[i] * e[index].balloon_Power;
		int nextY = curY + dy[i] * e[index].balloon_Power;
		if (i == 0) // If you find something to pop with balloon in the right direction, you'll be able to process visits to the area and break.
		{
			for (int x = curX + 1; x <= nextX; x++) {
				if (x >= Map_Width || visited[x][curY]) break;
				if (Map[x][curY] == 'H' || Map[x][curY] == 'T') break;
				if (Map[x][curY] == 'B' || Map[x][curY] == 'X' || Map[x][curY] == 'F') // When we encounter a block, box, flow box, No express.
				{
					visited[x][curY] = true;
					break;
				}

				// Expression of balloon range
				ofSetColor(255, 36, 36);
				ofDrawRectangle(40 + 65 * x, 40 + 60 * curY, 65, 60);
			}
		}
		else if (i == 1) { // If you find something to pop with balloon in the left direction, you'll be able to process visits to the area and break.
			for (int x = curX - 1; x >= nextX; x--) {
				if (x < 0 || visited[x][curY]) break;
				if (Map[x][curY] == 'H' || Map[x][curY] == 'T') break;
				if (Map[x][curY] == 'B' || Map[x][curY] == 'X' || Map[x][curY] == 'F') // When we encounter a block, box, flow box, No express.
				{
					visited[x][curY] = true;
					break;
				}

				// Expression of balloon range
				ofSetColor(255, 36, 36);
				ofDrawRectangle(40 + 65 * x, 40 + 60 * curY, 65, 60);
			}
		}
		else if (i == 2) { // If you find something to pop with balloon in the down direction, you'll be able to process visits to the area and break.
			for (int y = curY + 1; y <= nextY; y++) {
				if (y >= Map_Height || visited[curX][y]) break;
				if (Map[curX][y] == 'H' || Map[curX][y] == 'T') break;
				if (Map[curX][y] == 'B' || Map[curX][y] == 'X' || Map[curX][y] == 'F') // When we encounter a block, box, flow box, No express.
				{
					visited[curX][y] = true;
					break;
				}

				// Expression of balloon range
				ofSetColor(255, 36, 36);
				ofDrawRectangle(40 + 65 * curX, 40 + 60 * y, 65, 60);
			}
		}
		else if (i == 3) { // If you find something to pop with balloon in the up direction, you'll be able to process visits to the area and break.
			for (int y = curY - 1; y >= nextY; y--) {
				if (y < 0 || visited[curX][y]) break;
				if (Map[curX][y] == 'H' || Map[curX][y] == 'T') break;
				if (Map[curX][y] == 'B' || Map[curX][y] == 'X' || Map[curX][y] == 'F') // When we encounter a block, box, flow box, No express.
				{
					visited[curX][y] = true;
					break;
				}

				// Expression of balloon range
				ofSetColor(255, 36, 36);
				ofDrawRectangle(40 + 65 * curX, 40 + 60 * y, 65, 60);
			}
		}
	}
}

void ofApp::create_Player_Location(int x, int y) { // Show the Player Location
	if (p.direction == 2) { // Down direction.
		ofSetColor(255, 178, 245);
		ofDrawEllipse(x + 32.5, y + 30, 50, 45);
		ofSetColor(0);
		ofDrawCircle(x + 25, y + 20, 3);
		ofDrawCircle(x + 40, y + 20, 3);
		ofSetLineWidth(1);
		ofDrawLine(x + 25, y + 30, x + 40, y + 30);
		ofSetLineWidth(2);
		ofDrawLine(x + 7.5, y + 30, x + 2, y + 45);
		ofDrawLine(x + 57.5, y + 30, x + 63, y + 45);
		ofDrawLine(x + 25, y + 50, x + 25, y + 60);
		ofDrawLine(x + 40, y + 50, x + 40, y + 60);
	}
	else if (p.direction == 3) { // Up direction
		ofSetColor(255, 178, 245);
		ofDrawEllipse(x + 32.5, y + 30, 50, 45);
		ofSetColor(0);
		ofSetLineWidth(2);
		ofDrawLine(x + 7.5, y + 30, x + 2, y + 45);
		ofDrawLine(x + 57.5, y + 30, x + 63, y + 45);
		ofDrawLine(x + 25, y + 50, x + 25, y + 60);
		ofDrawLine(x + 40, y + 50, x + 40, y + 60);
	}
	else { // Left and right direction
		ofSetColor(255, 178, 245);
		ofDrawEllipse(x + 32.5, y + 30, 30, 45);
		ofSetColor(0);
		if (p.direction == 1) { // Left direction.
			ofDrawCircle(x + 30, y + 20, 3);
			ofSetLineWidth(1);
			ofDrawLine(x + 25, y + 25, x + 30, y + 27.5);
			ofSetLineWidth(2);
			ofDrawLine(x + 32.5, y + 30, x + 35, y + 45);
		}
		else { // Right direction
			ofDrawCircle(x + 35, y + 20, 3);
			ofSetLineWidth(1);
			ofDrawLine(x + 40, y + 25, x + 35, y + 27.5);
			ofSetLineWidth(2);
			ofDrawLine(x + 32.5, y + 30, x + 30, y + 45);
		}
		ofDrawLine(x + 32.5, y + 52.5, x + 32.5, y + 60);
	}
}

void ofApp::create_Enemy_Location(int x, int y, int index) { // Show the Enemy Location
	if (index == 0) { // First Enemy color
		ofSetColor(90, 255, 255);
	}
	else if (index == 1) { // Second Enemy color
		ofSetColor(0, 84, 255);
	}
	else if (index == 2) { // Third Enemy color
 		ofSetColor(102, 0, 51);
	}
	else if (index == 3) { // Fourth Enemy color
		ofSetColor(167, 72, 255);
	}
	if (e[index].direction == 2) { // Down Direction
		ofDrawEllipse(x + 32.5, y + 30, 50, 45);
		ofSetColor(255, 0, 0);
		ofDrawCircle(x + 25, y + 20, 3);
		ofDrawCircle(x + 40, y + 20, 3);
		ofSetColor(0);
		ofSetLineWidth(1);
		ofDrawLine(x + 25, y + 30, x + 40, y + 30);
		ofSetLineWidth(2);
		ofDrawLine(x + 7.5, y + 30, x + 2, y + 45);
		ofDrawLine(x + 57.5, y + 30, x + 63, y + 45);
		ofDrawLine(x + 25, y + 50, x + 25, y + 60);
		ofDrawLine(x + 40, y + 50, x + 40, y + 60);
	}
	else if (e[index].direction == 3) { // Up Direction 
		ofDrawEllipse(x + 32.5, y + 30, 50, 45);
		ofSetColor(0);
		ofSetLineWidth(2);
		ofDrawLine(x + 7.5, y + 30, x + 2, y + 45);
		ofDrawLine(x + 57.5, y + 30, x + 63, y + 45);
		ofDrawLine(x + 25, y + 50, x + 25, y + 60);
		ofDrawLine(x + 40, y + 50, x + 40, y + 60);
	}
	else { // Left and Right Direction
		ofDrawEllipse(x + 32.5, y + 30, 30, 45);
		if (e[index].direction == 1) { // Left Direction
			ofSetColor(255, 0, 0);
			ofDrawCircle(x + 30, y + 20, 3);
			ofSetColor(0);
			ofSetLineWidth(1);
			ofDrawLine(x + 25, y + 25, x + 30, y + 27.5);
			ofSetLineWidth(2);
			ofDrawLine(x + 32.5, y + 30, x + 35, y + 45);
		}
		else { // Right Direction
			ofSetColor(255, 0, 0);
			ofDrawCircle(x + 35, y + 20, 3);
			ofSetColor(0);
			ofSetLineWidth(1);
			ofDrawLine(x + 40, y + 25, x + 35, y + 27.5);
			ofSetLineWidth(2);
			ofDrawLine(x + 32.5, y + 30, x + 30, y + 45);
		}
		ofDrawLine(x + 32.5, y + 52.5, x + 32.5, y + 60);
	}
}

void ofApp::deleteMap() {
	ofSetColor(103, 0, 0);
	ofDrawRectangle(0, 0, 1055, 40);
	ofDrawRectangle(0, 820, 1055, 40);
	ofDrawRectangle(0, 0, 40, 860);
	ofDrawRectangle(1015, 0, 40, 860);
}

//--------------------------------------------------------------
void ofApp::draw(){
	if (draw_flag) { 
		ofBackground(125, 254, 116);
		createMap(); // Compose the Map

		visited_Init(); // Initialize 2D Array visited to false.
		for (int i = 0; i < 4; i++) {
			// Balloon Information of Enemy
			if (e[i].op == 2) {
				create_Enemy_Balloon(i);
			}
		}
		for (int i = 0; i < 4; i++){
			// Location (x,y) of Enemy
			int x2 = 40 + 65 * e[i].x, y2 = 40 + 60 * e[i].y;
			if (e[i].life) create_Enemy_Location(x2, y2, i); // Show the Enemy Location
		}

		// Balloon Information of Player
		if (pb_V.size()) {
			create_Player_Balloon(); // Show the Player Balloon
		}

		// Location (x,y) of Player
		int x1 = 40 + 65 * p.x, y1 = 40 + 60 * p.y;
		if (p.life) create_Player_Location(x1, y1); // Show the Player Location

	}
	else {
		ofBackground(255);
	}

	if (game_Over==0) {
		deleteMap(); // Hide the Map
	}
	else { // Player is slayed
		ofSetColor(3, 0, 102);
		ofDrawRectangle(0, 0, 1055, 860);
	}
}

void ofApp::go_Up() {
	// Set a Up Direction
	int nextX = p.x;
	int nextY = p.y - 1;
	if (nextX >= 0 && nextX < Map_Width && nextY >= 0 && nextY < Map_Height) { // In the Map Range
		for (int i = 0; i < 4; i++) {
			if (nextX == e[i].x && nextY == e[i].y) { // Meet the Enemy at next location => Game over!!!
				player_Life_Decrease();
			}
		}
		if (Map[nextX][nextY] == 'G') // Next location is Ground => Move next
		{
			p.x = nextX, p.y = nextY;
		}
		else if (Map[nextX][nextY] == 'X') { // Next location is X-Box 
			int nextnextY = nextY - 1;
			if (nextnextY >= 0 && Map[nextX][nextnextY] == 'G') 
			{  //  Next location is Ground => convert X-Box and Ground of each other 
				Map[nextX][nextnextY] = 'X';
				Map[nextX][nextY] = 'G';
				p.x = nextX, p.y = nextY;
			}
		}
		else if (Map[nextX][nextY] == 'W') { // If We find the water balloon?
			if (nextY - 1 >= 0 && Map[nextX][nextY - 1] == 'G') { // If the next space in the water balloon is Ground?
				Map[nextX][nextY] = 'G';
				for (int i = nextY - 1; i >= 0; i--) { // Go to a non-ground point
					if (Map[nextX][i] != 'G') {
						int finX = nextX;
						int finY = i + 1;
						Map[finX][finY] = 'W'; // Set to have water balloon at that point
						for (int j = 0; j < pb_V.size(); j++) {
							// Player Balloon information needs to be updated to a new location.
							if (pb_V[j].x == nextX && pb_V[j].y == nextY) {
								pb_V[j].x = finX;
								pb_V[j].y = finY;
								break;
							}
						}
						break;
					}
					else if (Map[nextX][i] == 'G' && i == 0) { // If the water balloon is in the top ground of the screen
						int finX = nextX;
						int finY = i;
						Map[finX][finY] = 'W'; // Set to have water balloon at that point
						for (int j = 0; j < pb_V.size(); j++) {
							// Player Balloon information needs to be updated to a new location.
							if (pb_V[j].x == nextX && pb_V[j].y == nextY) {
								pb_V[j].x = finX;
								pb_V[j].y = finY;
								break;
							}
						}
					}
				}
			}
		}
	}
	p.direction = 3;
}

void ofApp::go_Down() { // If you want information, Check go_Up(). (The flow of the function is similar)
	int nextX = p.x;
	int nextY = p.y + 1;
	if (nextX >= 0 && nextX < Map_Width && nextY >= 0 && nextY < Map_Height) {
		for (int i = 0; i < 4; i++) {
			if (nextX == e[i].x && nextY == e[i].y) {
				player_Life_Decrease();
			}
		}
		if (Map[nextX][nextY] == 'G')
		{
			p.x = nextX, p.y = nextY;
		}
		else if (Map[nextX][nextY] == 'X') {
			int nextnextY = nextY + 1;
			if (nextnextY < Map_Height && Map[nextX][nextnextY] == 'G')
			{
				Map[nextX][nextnextY] = 'X';
				Map[nextX][nextY] = 'G';
				p.x = nextX, p.y = nextY;
			}
		}
		else if (Map[nextX][nextY] == 'W') { 
			if (nextY + 1 < Map_Height && Map[nextX][nextY + 1] == 'G') { 
				Map[nextX][nextY] = 'G'; 
				for (int i = nextY + 1; i < Map_Height; i++) { 
					if (Map[nextX][i] != 'G') {
						int finX = nextX;
						int finY = i - 1;
						Map[finX][finY] = 'W'; 
						for (int j = 0; j < pb_V.size(); j++) {
							if (pb_V[j].x == nextX && pb_V[j].y == nextY) {
								pb_V[j].x = finX;
								pb_V[j].y = finY;
								break;
							}
						}
						break;
					}
					else if (Map[nextX][i] == 'G' && i == Map_Height-1) { 
						int finX = nextX;
						int finY = i;
						Map[finX][finY] = 'W'; 
						for (int j = 0; j < pb_V.size(); j++) {
							if (pb_V[j].x == nextX && pb_V[j].y == nextY) {
								pb_V[j].x = finX;
								pb_V[j].y = finY;
								break;
							}
						}
					}
				}
			}
		}
	}
	p.direction = 2;
}

void ofApp::go_Right() { // If you want information, Check go_Up(). (The flow of the function is similar)
	int nextX = p.x + 1;
	int nextY = p.y;
	if (nextX >= 0 && nextX < Map_Width && nextY >= 0 && nextY < Map_Height) {
		for (int i = 0; i < 4; i++) {
			if (nextX == e[i].x && nextY == e[i].y) {
				player_Life_Decrease();
			}
		}
		if (Map[nextX][nextY] == 'G')
		{
			p.x = nextX, p.y = nextY;
		}
		else if (Map[nextX][nextY] == 'X') {
			int nextnextX = nextX + 1;
			if (nextnextX < Map_Height && Map[nextnextX][nextY] == 'G')
			{
				Map[nextnextX][nextY] = 'X';
				Map[nextX][nextY] = 'G';
				p.x = nextX, p.y = nextY;
			}
		}
		else if (Map[nextX][nextY] == 'W') {
			if (nextX + 1 < Map_Width && Map[nextX + 1][nextY] == 'G') {
				Map[nextX][nextY] = 'G';
				for (int i = nextX + 1; i < Map_Width; i++) {
					if (Map[i][nextY] != 'G') {
						int finX = i - 1;
						int finY = nextY;
						Map[finX][finY] = 'W';
						for (int j = 0; j < pb_V.size(); j++) {
							if (pb_V[j].x == nextX && pb_V[j].y == nextY) {
								pb_V[j].x = finX;
								pb_V[j].y = finY;
								break;
							}
						}
						break;
					}
					else if (Map[i][nextY] == 'G' && i == Map_Width-1) {
						int finX = i;
						int finY = nextY;
						Map[finX][finY] = 'W';
						for (int j = 0; j < pb_V.size(); j++) {
							if (pb_V[j].x == nextX && pb_V[j].y == nextY) {
								pb_V[j].x = finX;
								pb_V[j].y = finY;
								break;
							}
						}
					}
				}
			}
		}
	}
	p.direction = 0;
}

void ofApp::go_Left() { // If you want information, Check go_Up(). (The flow of the function is similar)
	int nextX = p.x - 1;
	int nextY = p.y;
	if (nextX >= 0 && nextX < Map_Width && nextY >= 0 && nextY < Map_Height) {
		for (int i = 0; i < 4; i++) {
			if (nextX == e[i].x && nextY == e[i].y) {
				player_Life_Decrease();
			}
		}
		if (Map[nextX][nextY] == 'G')
		{
			p.x = nextX, p.y = nextY;
		}
		else if (Map[nextX][nextY] == 'X') {
			int nextnextX = nextX - 1;
			if (nextnextX >= 0 && Map[nextnextX][nextY] == 'G')
			{
				Map[nextnextX][nextY] = 'X';
				Map[nextX][nextY] = 'G';
				p.x = nextX, p.y = nextY;
			}
		}
		else if (Map[nextX][nextY] == 'W') {
			if (nextX - 1 >= 0 && Map[nextX-1][nextY] == 'G') { 
				Map[nextX][nextY] = 'G'; 
				for (int i = nextX - 1; i >= 0; i--) { 
					if (Map[i][nextY] != 'G') {
						int finX = i + 1; 
						int finY = nextY;
						Map[finX][finY] = 'W'; 
						for (int j = 0; j < pb_V.size(); j++) {
							if (pb_V[j].x == nextX && pb_V[j].y == nextY) {
								pb_V[j].x = finX;
								pb_V[j].y = finY;
								break;
							}
						}
						break;
					}
					else if (Map[i][nextY] == 'G' && i == 0) { 
						int finX = i; 
						int finY = nextY;
						Map[finX][finY] = 'W'; 
						for (int j = 0; j < pb_V.size(); j++) {
							if (pb_V[j].x == nextX && pb_V[j].y == nextY) {
								pb_V[j].x = finX;
								pb_V[j].y = finY;
								break;
							}
						}
					}
				}
			}
		}
	}
	p.direction = 1;
}

void ofApp::go_Space() {
	if (p.current_Balloon_Count < p.balloon_Count) {
		p.current_Balloon_Count++;
		Map[p.x][p.y] = 'W';
		pb_V.push_back({ p.x, p.y });
	}
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	if (key == OF_KEY_UP && game_Over == 0)
	{
		if (play_Time == 0) play_Time = 1;
		if(play_Time > 0) go_Up();
	}
	if (key == OF_KEY_DOWN && game_Over == 0)
	{
		if (play_Time == 0) play_Time = 1;
		if (play_Time > 0) go_Down();
	}
	if (key == OF_KEY_RIGHT && game_Over == 0) 
	{
		if (play_Time == 0) play_Time = 1;
		if (play_Time > 0) go_Right();
	}
	if (key == OF_KEY_LEFT && game_Over == 0) 
	{
		if (play_Time == 0) play_Time = 1;
		if (play_Time > 0) go_Left();
	}
	if (key == OF_KEY_SPACE && game_Over == 0)
	{
		if (play_Time == 0) play_Time = 1;
		if (play_Time > 0) go_Space();
	}
}

void ofApp::info_Init() {
	visited_Init(); // Initialize 2D Array visited to false.
	game_Over = 0, play_Time = -1;
	pb_V.clear();
	int index = 0;
	while (index != 4) { // Randomly set enemy time
		int rand_Time = rand() % 4;
		play_Random[index] = rand_Time;
		for (int j = 0; j < index; j++) {
			if (play_Random[j] == rand_Time) {
				index--;
				break;
			}
		}
		index++;
	}
	for (int i = 0; i < 4; i++) { // Initialization of enemy information.
		em_V[i].clear();
		e_V[i].clear();
		e_Index[i] = 1;
		int rand_Time = 2*(play_Random[i])+ 10;
		play_Random[i] = rand_Time;
	}
	for (int w = 0; w < Map_Width; w++) { // Initialize flow block to zero weight
		for (int h = 0; h < Map_Height; h++) {
			int index = w + h * Map_Width;
			if (tempMap[w][h] == 'F') {
				flow_Block[index] = 0;
			}
			enemy_Map[index] = 0;
		}
	}
	
	while (1) { // Initializing information in Enemy and Player.
		int x1, y1, x2[4], y2[4], op=1;
		double distance;
		x1 = rand() % Map_Width;
		y1 = rand() % Map_Height;
		int r1 = rand() % 4;
		int r2 = rand() % 4;
		int r3 = rand() % 7;
		int r4 = rand() % 4;
		x2[0] = enemy_Location[0][r1][0]; y2[0] = enemy_Location[0][r1][1];
		x2[1] = enemy_Location[1][r2][0]; y2[1] = enemy_Location[1][r2][1];
		x2[2] = enemy_Location[2][r3][0]; y2[2] = enemy_Location[2][r3][1];
		x2[3] = enemy_Location[3][r4][0]; y2[3] = enemy_Location[3][r4][1];
		for (int i = 0; i < 4; i++) {
			distance= sqrt(pow(x1 - x2[i], 2) + pow(y1 - y2[i], 2));
			if (tempMap[x1][y1] == 'G' && distance >= 6.0) {
				continue;
			}
			op = 0;
		}
		if (op == 0) continue;

		// Player information
		p.x = x1, p.y = y1, p.direction = 2, p.life=5;
		p.score = 0, p.balloon_Power = 1, p.balloon_Count = 1;
		p.current_Balloon_Count = 0;
		
		// Enemy information
		for (int i = 0; i < 4; i++) {
			e[i].x = x2[i], e[i].y = y2[i], e[i].direction = 2, e[i].op = 0, e[i].life = 3;
			e[i].balloon_Power = 1, e[i].score = 0, e[i].balloonX = -1, e[i].balloonY = -1;
		}
		break;
	}
	cout << "Finish the Player and Enemy Locate setting. = >" << endl;
	cout << "Player ( X, Y ) = ( " << p.x << ", " << p.y << " )" << endl;
	for (int i = 0; i < 4; i++) {
		cout << "Enemy" << i+1 << " ( X, Y ) = ( " << e[i].x << ", " << e[i].y << " )" << endl;
	}
}

void ofApp::player_Pop_Balloon() { // Pop the Balloon
	visited_Init(); // Initialize 2D Array visited to false.
	int enemy_Life_Op[4] = { 0,0,0,0 };
	int player_Life_Op = 0;
	if (pb_V.size()) { // Player Balloon has at least one on the Map
		for (int i = 0; i < pb_V.size(); i++) { // Player Balloon in turn:
			int currentX = pb_V[i].x;
			int currentY = pb_V[i].y;
			int power = p.balloon_Power;
			Map[currentX][currentY] = 'G'; // Convert the current Balloon to Ground 'G'.
			for (int j = 0; j < 4; j++) { // Let the left, right, up and down balloons pop.
				// Make Balloon as effective as Player's power.
				int nextX = currentX + dx[j] * power; 
				int nextY = currentY + dy[j] * power;
				// Is there a block that needs to be removed by approaching one space from top to bottom?
				if (j == 0) { // find block to the Right
					for (int x = currentX; x <= nextX; x++) {
						if (x >= Map_Width || visited[x][currentY]) break; // It's important not to invade the spot where the balloon pop!
						if (x == p.x && currentY == p.y) { // Game Over if the current player's location is within the range of balloon popping.
							if (!player_Life_Op) {
								player_Life_Decrease();
								player_Life_Op = 1;
							}
						}
						for (int k = 0; k < 4; k++) {
							if ((x == e[k].x && currentY == e[k].y) || (x == e[k].balloonX && currentY == e[k].balloonY)) {
								// If Enemy's Balloon or Enemy is inside the Player's Balloon Range
								if (!enemy_Life_Op[k]) {
									enemy_Life_Decrease(k);
									enemy_Life_Op[k] = 1;
								}
							}
						}
						if (Map[x][currentY] == 'B' || Map[x][currentY] == 'X' || Map[x][currentY] == 'F') { 
							// // Block, Box, Flow_Box removed by balloon => Each different score.
							if (Map[x][currentY] == 'B') {
								p.score += 10;
								Map[x][currentY] = 'G'; // Block removed by balloon => change the ground 
								visited[x][currentY] = true; // The location is pop, so visit processing is essential. 
														     // (To keep the other balloon from crossing its position)
							}
							else if (Map[x][currentY] == 'X') {
								p.score += 15;
								Map[x][currentY] = 'G'; // Box removed by balloon => change the ground 
								visited[x][currentY] = true; // The location is pop, so visit processing is essential. 
															 // (To keep the other balloon from crossing its position)
							}
							else if (Map[x][currentY] == 'F') {
								int index = currentY * Map_Width + x; // Convert the location to index
								if (++flow_Block[index] == 3) { // Water balloon burst by weight
									p.score += 20;
									Map[x][currentY] = 'G'; // Flow Box removed by balloon => change the ground 
									visited[x][currentY] = true; // The location is pop, so visit processing is essential. 
															     // (To keep the other balloon from crossing its position)
								}
								else {
									visited[x][currentY] = true; // The location is pop, so visit processing is essential.
															     // (To keep the other balloon from crossing its position)
								}
							}
							break;
						}
						else if (Map[x][currentY] == 'H' || Map[x][currentY] == 'T' || Map[x][currentY] == 'W')
						{ 
							break; // Case of House, Tree, Water Balloon touched =>  Finish the balloon effect in direction.
						}
					}
				}
				else if (j == 1) { // If you want information, Check j==0.
					for (int x = currentX; x >= nextX; x--) {
						if (x < 0 || visited[x][currentY]) break;
						if (x == p.x && currentY == p.y) {
							if (!player_Life_Op) {
								player_Life_Decrease();
								player_Life_Op = 1;
							}
						}
						for (int k = 0; k < 4; k++) {
							if ((x == e[k].x && currentY == e[k].y) || (x == e[k].balloonX && currentY == e[k].balloonY)) {
								if (!enemy_Life_Op[k]) {
									enemy_Life_Decrease(k);
									enemy_Life_Op[k] = 1;
								}
							}
						}
						if (Map[x][currentY] == 'B' || Map[x][currentY] == 'X' || Map[x][currentY] == 'F') {
							if (Map[x][currentY] == 'B') {
								p.score += 10;
								Map[x][currentY] = 'G'; 
								visited[x][currentY] = true; 
							}
							else if (Map[x][currentY] == 'X') {
								p.score += 15;
								Map[x][currentY] = 'G'; 
								visited[x][currentY] = true;  
							}
							else if (Map[x][currentY] == 'F') {
								int index = currentY * Map_Width + x; 
								if (++flow_Block[index] == 3) {
									p.score += 20;
									Map[x][currentY] = 'G';
									visited[x][currentY] = true;
								}
								else {
									visited[x][currentY] = true; 
								}
							}
							break;
						}
						else if (Map[x][currentY] == 'H' || Map[x][currentY] == 'T' || Map[x][currentY] == 'W')
						{
							break;
						}
					}
				}
				else if (j == 2) { // If you want information, Check j==0.
					for (int y = currentY; y <= nextY; y++) {
						if (y >= Map_Height || visited[currentX][y]) break;
						if (currentX == p.x && y == p.y) {
							if (!player_Life_Op) {
								player_Life_Decrease();
								player_Life_Op = 1;
							}
						}
						for (int k = 0; k < 4; k++) {
							if ((currentX == e[k].x && y == e[k].y) || (currentX == e[k].balloonX && y == e[k].balloonY)) {
								if (!enemy_Life_Op[k]) {
									enemy_Life_Decrease(k);
									enemy_Life_Op[k] = 1;
								}
							}
						}
						if (Map[currentX][y] == 'B' || Map[currentX][y] == 'X' || Map[currentX][y] == 'F') {
							if (Map[currentX][y] == 'B') {
								p.score += 10;
								Map[currentX][y] = 'G';
								visited[currentX][y] = true;
							}
							else if (Map[currentX][y] == 'X') {
								p.score += 15;
								Map[currentX][y] = 'G';
								visited[currentX][y] = true;
							}
							else if (Map[currentX][y] == 'F') {
								int index = y * Map_Width + currentX;
								if (++flow_Block[index] == 3) {
									p.score += 20;
									Map[currentX][y] = 'G';
									visited[currentX][y] = true;
								}
								else {
									visited[currentX][y] = true;
								}
							}
							break;
						}
						else if (Map[currentX][y] == 'H' || Map[currentX][y] == 'T' || Map[currentX][y] == 'W')
						{
							break;
						}
					}
				}
				else if (j == 3) { // If you want information, Check j==0.
					for (int y = currentY; y >= nextY; y--) {
						if (y < 0 || visited[currentX][y]) break;
						if (currentX == p.x && y == p.y) {
							if (!player_Life_Op) {
								player_Life_Decrease();
								player_Life_Op = 1;
							}
						}
						for (int k = 0; k < 4; k++) {
							if ((currentX == e[k].x && y == e[k].y) || (currentX == e[k].balloonX && y == e[k].balloonY)) {
								if (!enemy_Life_Op[k]) {
									enemy_Life_Decrease(k);
									enemy_Life_Op[k] = 1;
								}
							}
						}
						if (Map[currentX][y] == 'B' || Map[currentX][y] == 'X' || Map[currentX][y] == 'F') {
							if (Map[currentX][y] == 'B') {
								p.score += 10;
								Map[currentX][y] = 'G';
								visited[currentX][y] = true;
							}
							else if (Map[currentX][y] == 'X') {
								p.score += 15;
								Map[currentX][y] = 'G';
								visited[currentX][y] = true;
							}
							else if (Map[currentX][y] == 'F') {
								int index = y * Map_Width + currentX;
								if (++flow_Block[index] == 3) {
									p.score += 20;
									Map[currentX][y] = 'G';
									visited[currentX][y] = true;
								}
								else {
									visited[currentX][y] = true;
								}
							}
							break;
						}
						else if (Map[currentX][y] == 'H' || Map[currentX][y] == 'T' || Map[currentX][y] == 'W')
						{
							break;
						}
					}
				}
			}
		}
		p.current_Balloon_Count -= pb_V.size(); // Count by the number of balloon that pop => player current Balloon count = 0 
		pb_V.clear(); // Free that vector with the location of the player's balloon.
	}
}

void ofApp::memory_Free() { 
	for (int i = 0; i < 4; i++) {
		em_V[i].clear();
		e_V[i].clear();
	}
	pb_V.clear();
	for (int i = 0; i < Map_Width; i++) {
		free(Map[i]);
		free(tempMap[i]);
		free(visited[i]);
	}
	free(Map);
	free(tempMap);
	free(visited);
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
	if (key == 'd' || key == 'D') {
		if (!load_flag) return;
		draw_flag = (draw_flag + 1) % 2;
		if (draw_flag == 0) {
			for (int i = 0; i < Map_Width; i++) { // Initialize the Map with a tempMap
				for (int j = 0; j < Map_Height; j++) {
					Map[i][j] = tempMap[i][j];
				}
			}
			info_Init(); // Initialize the player and enemy Location ...
			mySound.stop();
		}
		else {
			play_Time = 0;
			mySound.load("Crazy Arcade BGM - Village.mp3");
			mySound.play();
		}
	}
	if (key == 'q' || key == 'Q') {
		draw_flag = 0;

		memory_Free();
		
		cout << "Memory for all informations has been freed!" << endl;
		cout << "Good Bye~!!!" << endl;

		_Exit(0);
	}
	if (key == 'l' || key == 'L') {
		// Open the Open File Dialog
		ofFileDialogResult openFileResult = ofSystemLoadDialog("Select a only txt for Map");

		// Check whether the user opened a file
		if (openFileResult.bSuccess) {
			ofLogVerbose("User selected a file");

			// We have a file, so let's check it and process it
			processOpenFileSelection(openFileResult);
			load_flag = 1;
		}
	}
	if (key == OF_KEY_SHIFT) {
		if (play_Time > 0) player_Pop_Balloon(); // Pop the Balloon!
	}
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}

void ofApp::memory_Allocation_Info() { 
	Map = (char**)malloc(sizeof(char*) * Map_Width);
	for (int i = 0; i < Map_Width; i++) {
		Map[i] = (char*)malloc(sizeof(char) * Map_Height);
	}

	tempMap = (char**)malloc(sizeof(char*) * Map_Width);
	for (int i = 0; i < Map_Width; i++) {
		tempMap[i] = (char*)malloc(sizeof(char) * Map_Height);
	}

	visited = (int**)malloc(sizeof(int*) * Map_Width);
	for (int i = 0; i < Map_Width; i++) {
		visited[i] = (int*)malloc(sizeof(int) * Map_Height);
	}
}

void ofApp::processOpenFileSelection(ofFileDialogResult openFileResult) {
	string fileName = openFileResult.getName();
	ofFile file(fileName);

	if (!file.exists()) cout << "Target file does not exists." << endl;
	else cout << "We found the target file." << endl;

	ofBuffer buffer(file);

	// Determine Map Width and Map Height.
	for (ofBuffer::Line it = buffer.getLines().begin(), end = buffer.getLines().end(); it != end; ++it) {
		string line = *it;
		Map_Width = line.size();
		Map_Height++;
	}

	memory_Allocation_Info(); // 2D Array of Map, tempMap, and visited is allocated memory. 

	int index = 0; // index는 height 처리
	for (ofBuffer::Line it = buffer.getLines().begin(), end = buffer.getLines().end(); it != end; ++it) {
		string line = *it;
		for (int i = 0; i < Map_Width; i++) {
			Map[i][index] = line[i];
			tempMap[i][index] = line[i];
		}
		index++;
	} 

	// Show the written Map.
	cout << "Finish the map setting. => " << endl;
	for (int i = 0; i < Map_Width; i++) {
		if (i == 0) cout << " ";
		cout << "--";
		if (i == Map_Width-1) cout << "-" << endl;
	}
	for (int i = 0; i < Map_Height; i++) {
		for (int j = 0; j < Map_Width; j++) {
			if (j == 0) cout << "| ";
			cout << Map[j][i] << " ";
			if (j == Map_Width - 1) cout << "|";
		}
		cout << endl;
	}
	for (int i = 0; i < Map_Width; i++) {
		if (i == 0) cout << " ";
		cout << "--";
		if (i == Map_Width-1) cout << "-" << endl;
	}

	info_Init(); // Initialize the Player and Enemy Location ...
}