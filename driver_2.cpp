#include "GLFW/glfw3.h"
#include "bits/stdc++.h"
#include "DrawUtil.h"
using namespace std;

const double INITIAL_LENGTH = 11;
const double MAX_GENERATIONS = 6;
const double ROTATE_ANGLE = 25;
const double INITIAL_ANGLE = 90;

// define the variables in the rule string
#define OPENBRACKET '['
#define CLOSEDBRACKET ']'
#define COLORCHANGE 'C'
#define	TURNLEFT '+'
#define TURNRIGHT '-'
#define DRAW 'F'
#define CIRCLE 'c'
#define WIDTH 'W'

// create an object pointer of the DrawUtil Class
DrawUtil* DU;
struct Point{
	double X;
	double Y;
	Point() : X(0), Y(0) {};
	Point(double _x, double _y) : X(_x), Y(_y) {};
};

// create a map for the rule variable and associated definition <rule character, definition>
map<char, string> Rule;
class Tree{
	string Str;
	Point Pos, tempPos;
	int Generation;
	double Length;
	double Dir;
	int c_index;
	int w_index;
public:
	Tree(string _str, Point _pos, double _len, double _dir, int _gen, int _col, int _width){	//constructor function for the Tree class
		Pos = _pos;
		tempPos=_pos;
		Length = _len;
		Dir = _dir;
		c_index = _col;
		Generation = _gen;
		Str = _str;
		w_index = _width;
		for (Generation = _gen; Generation < MAX_GENERATIONS; Generation++){
			Str.clear();
			for (auto Char : _str){
				if (isalpha(Char)){
					if (Rule.find(Char) != Rule.end()) copy(Rule[Char].begin(), Rule[Char].end(), back_inserter(Str));
					else Str.push_back(Char);
				}
				else{
					Str.push_back(Char);
				}
			}
			cout << Generation << ' ' << Str << endl;
			_str = Str;
		}
	}
	void Process(){		// main function to check the characters and draw the fractal structure recursively
		if (Generation > MAX_GENERATIONS) return;
		int i = 0, n = Str.size();
		while (i < n){	//while i is not the last character
			char Ch = Str[i];	// get the ith character of the string
			switch(Ch){
				case OPENBRACKET : {	//get the longest substring with the starting and the closing brackets
					int OpenBrackets = 1;
					int j = i;
					while (OpenBrackets){
					 	i++;
						if (Str[i] == CLOSEDBRACKET) OpenBrackets--;
						else if (Str[i] == OPENBRACKET) OpenBrackets++;
					}
					Tree child = *(new Tree(Str.substr(j + 1, i-j-1), Pos, Length, Dir, Generation, c_index, w_index));
					child.Process();
					break;
				}
				case TURNLEFT : {	//rotate the branch left by defined angle
					Dir += ROTATE_ANGLE;
					if (Dir >= 360) Dir -= 360;		// if the angle increases beyond 360, subtract 360
					break;
				}
				case TURNRIGHT : {	//rotate the branch right by defined angle
					Dir -= ROTATE_ANGLE;
					if (Dir < 0) Dir += 360;	// if angle decreases beyond 0, add 360
					break;
				}
				case COLORCHANGE : {
					if(Generation <= 3)
					{
						c_index = 2;
						break;
					}
					else
					{
						c_index = Str[++i] - '0';
						break;
					}
				}
				case WIDTH : {
					w_index = Str[++i] - '0';
				}
				case CIRCLE : {
					if(Generation == MAX_GENERATIONS){
						DU->setColor(DU->Colors[c_index].R, DU->Colors[c_index].G, DU->Colors[c_index].B);
						DU->changePointSize(1);
						DU->fillCircle(tempPos.X, tempPos.Y,1);
					}
					break;
				}
				default : {
					Point NewPos = *(new Point(Pos.X + cos((M_PI / 180.0) * Dir) * Length, Pos.Y + sin((M_PI / 180.0) * Dir) * Length));
					DU->setColor(DU->Colors[c_index].R, DU->Colors[c_index].G, DU->Colors[c_index].B);
					DU->changePointSize(w_index);
					DU->drawLine((int)Pos.X, (int)Pos.Y, (int)NewPos.X, (int)NewPos.Y);
					Pos = NewPos;
					break;
				}
			}
			i++;
		}
	}
};

int main(){

	DU = new DrawUtil();
	DU->initializeColors();

	int H = 1080, W = 1920;
	GLFWwindow* window = DU->StartWindow(H, W);

	DU->drawNightSky(W, H);
	DU->drawStars(W, H, 600);
	DU->drawGrass(W);
	DU->setColor(1,1,1);
	DU->fillCircle(0.15*W, 0.85*H, 50);

	freopen("in2", "r", stdin);
	string s;
	cin >> s;
	string _tmp; char _ch;
	while (cin >> _ch >> _tmp){
		Rule[_ch] = _tmp;
	}

	Tree tree2(s, Point(W*0.3,H*0.0), INITIAL_LENGTH, INITIAL_ANGLE, 1, 0, 2);
	tree2.Process();

	DU->Stayawake(window);
}