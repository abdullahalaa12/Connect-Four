#include<SFML/graphics.hpp>
#include<SFML/audio.hpp>
#include<string>
#include<iostream>
#include<Windows.h>
using namespace std;
using namespace sf;

struct Cell
{
	CircleShape Circle;
	int Num;
	bool win;
};

int col=7,row=6;

bool EventCheck(Event& Click, RenderWindow& window);
bool MenueClickedCheck(RenderWindow& window, Text& Button);
Sprite Animation(float MoveScale, Texture &T, CircleShape C);
void Reset(Cell **Cell, Texture& T);
int CheckWin(int x,int y, Cell **Cell,bool test);
int AI(Cell **cell);
int User(RenderWindow& window, Cell **Cell);

int main()
{
	ShowWindow(GetConsoleWindow(), SW_HIDE);
	Texture TextureCell, TextureYellowCell, TextureRedCell, TextureYellow, TextureRed;
	TextureCell.loadFromFile("graphics/_Cell.png");
	TextureYellowCell.loadFromFile("graphics/_YellowCell.png");
	TextureRedCell.loadFromFile("graphics/_RedCell.png");
	TextureYellow.loadFromFile("graphics/Yellow.png");
	TextureRed.loadFromFile("graphics/Red.png");

	CircleShape WinCell[4];

	SoundBuffer MenuSoundBuffer, ClickSoundBuffer, WrongSoundBuffer;
	ClickSoundBuffer.loadFromFile("Audio/Click.wav");
	MenuSoundBuffer.loadFromFile("Audio/Menu.wav");
	WrongSoundBuffer.loadFromFile("Audio/Wrong2.wav");

	Sound MenuSound(MenuSoundBuffer), ClickSound(ClickSoundBuffer), WrongSound(WrongSoundBuffer);
	MenuSound.setLoop(true);
	MenuSound.setVolume(10);
	MenuSound.play();

	Font font;
	font.loadFromFile("fonts/1.ttf");
	Text Start("Start", font, 70), Options("Options", font, 70), Quit("Exit", font, 70), Restart("Again", font, 32)
		, Quit2("Exit", font, 32), Win("", font, 35);
	Restart.setPosition( 240, /*row * 54 - 120*/5);
	Quit2.setPosition(320, /*row * 54 - 50*/5);
	Start.setPosition(200 - Start.getGlobalBounds().width / 2, 50);
	Options.setPosition(200 - Options.getGlobalBounds().width / 2, 160);
	Quit.setPosition(200 - Quit.getGlobalBounds().width / 2, 270);

	bool isPlay = false, Page = false;
	RenderWindow StartMenu(VideoMode(400, 400), "Start Menu");
	while (StartMenu.isOpen())
	{
		Event Click;
		StartMenu.clear(Color(23, 93, 222));		
		MenueClickedCheck(StartMenu, Start);
		MenueClickedCheck(StartMenu, Quit);
		MenueClickedCheck(StartMenu, Options);
		StartMenu.draw(Start);
		StartMenu.draw(Options);
		StartMenu.draw(Quit);
		if (EventCheck(Click, StartMenu))
			if (MenueClickedCheck(StartMenu, Start))
			{
				ClickSound.play();
				Page = true;
				StartMenu.close();
			}
			else if (MenueClickedCheck(StartMenu, Quit))
				StartMenu.close();
		StartMenu.display();
	}

	Clock Time;
	int Counter = 0, win = 0, x = 0, y = 0;
	float StartTime = 0.0;

	Cell** cell = new Cell *[col];
	for (int i = 0; i < col; i++)
		cell[i] = new Cell[row];
	Reset(cell, TextureCell);
	RenderWindow window(VideoMode(col * 54, 50+(row * 54)), "Connect Four");
	while (window.isOpen() && Page)
	{
		Event Click;
		window.clear(Color(23, 93, 222));
		for (int x = 0; x < col; x++)
			for (int y = 0, i = 0; y < row; y++)
			{
				window.draw(cell[x][y].Circle);
				if (cell[x][y].win)
				{
					WinCell[i].setRadius(10);
					WinCell[i].setFillColor(Color::Green);
					WinCell[i].setPosition(cell[x][y].Circle.getPosition().x + 14, cell[x][y].Circle.getPosition().y + 14);
					window.draw(WinCell[i]);
					i++;
				}
			}
		if (!win)
		{

			//Animation
			if (Counter % 2==0)
				window.draw(Animation(StartTime, TextureRed, cell[x][y].Circle));
			else
				window.draw(Animation(StartTime, TextureYellow, cell[x][y].Circle));


			if (!isPlay)
			{
				//User turn
				if (Counter % 2 == 0)
					x = User(window, cell);
				//computer Turn
				else
					x = AI(cell);
				for (int i = 0; i < row; i++) {
					if (cell[x][i].Num == 0) {
						y = i;
						break;
					}
				}
				if (EventCheck(Click, window) || Counter % 2) {
					if (cell[x][y].Num == 0)
					{
						ClickSound.play();
						Time.restart();
						isPlay = true;
					}
					else
					{
						cout << "Full !!    Try Again\n";
						WrongSound.play();
					}
				}
			}


			else
			{
				StartTime =Time.getElapsedTime().asSeconds() ;
				if (Animation(StartTime, TextureCell, cell[x][y].Circle).getPosition().y > cell[x][y].Circle.getPosition().y)
				{
					StartTime = 0.0;
					cell[x][y].Num = (Counter % 2) + 1;
					isPlay = false;
					if (cell[x][y].Num == 1)
						cell[x][y].Circle.setTexture(&TextureRedCell);
					else
						cell[x][y].Circle.setTexture(&TextureYellowCell);
					win = CheckWin(x, y, cell,true);
					Counter++;
					if (Counter == col * row && win == 0)
						win = 3;
						
				}
			}
		}
		else
		{
			if (win == 2)
			{
				Win.setString("Computer Win");
				Win.setFillColor(Color::Yellow);
			}
			else if (win == 1)
			{
				Win.setString("You Win");
				Win.setFillColor(Color::Red);
			}
			else if (win == 3)
			{
				Win.setString("No One Won");
				Win.setFillColor(Color::Cyan);
			}
			Win.setPosition(3, 3);
			MenueClickedCheck(window, Restart);
			MenueClickedCheck(window, Quit2);
			window.draw(Win);
			window.draw(Restart);
			window.draw(Quit2);
			if (EventCheck(Click, window))
				if (MenueClickedCheck(window, Restart))
				{
					Reset(cell, TextureCell);
					win = 0;
					Counter = 0;
				}
				else if (MenueClickedCheck(window, Quit2))
					window.close();
		}
		window.display();
	}
	for (int i = 0; i < col; i++)
		delete[] cell[i];
	delete[] cell;
	cell = NULL;
	return 0;
}

bool EventCheck(Event& Click, RenderWindow& window)
{
	while (window.pollEvent(Click))
	{
		switch (Click.type)
		{
		case Event::Closed:
			window.close();
			break;
		case Event::MouseButtonPressed:
			if (Mouse::isButtonPressed(Mouse::Left))
				return true;
		}
	}
	return false;
}

bool MenueClickedCheck(RenderWindow& window, Text& Button)
{
	if (Mouse::getPosition(window).x > Button.getGlobalBounds().left &&
		Mouse::getPosition(window).x < (Button.getGlobalBounds().left + Button.getGlobalBounds().width) &&
		Mouse::getPosition(window).y > Button.getGlobalBounds().top &&
		Mouse::getPosition(window).y < (Button.getGlobalBounds().top + Button.getGlobalBounds().height))
	{
		Button.setFillColor(Color(0,51,51));
		return true;
	}
	Button.setFillColor(Color::White);
	return false;
}

Sprite Animation(float MoveScale, Texture &T, CircleShape C)
{
	Sprite A(T);
	A.setPosition(C.getPosition().x,  MoveScale*1000+2);
	return A;
}

void Reset(Cell **Cell, Texture& T)
{
	for (int i = 0; i < col; i++)
		for (int j = 0; j < row; j++)
		{
			Cell[i][j].Circle.setRadius(24);
			Cell[i][j].Circle.setPosition(2+(i * 54), 50  + ((row-j-1) * 54));
			Cell[i][j].Circle.setTexture(&T);
			Cell[i][j].Num = 0;
			Cell[i][j].win = false;
		}
}

int CheckWin(int x, int y, Cell **cell,bool test)
{
	
				//Vertical

	for (int i = y - 3; i <= y && i >= 0; i++)
	{
	//	cout <<i+1 << endl;
		if (cell[x][i].Num != cell[x][y].Num)
			break;
		else if (i == y)
		{
			if (test)
				for (i = 0; i < 4; i++)
					cell[x][y - i].win = true;
			return cell[x][y].Num;
		}
	}


	            //Horizontal

	for (int i = x - 3, k = 0; i <= x + 3 && i < col; i++)
	{
		if (i < 0)
			continue;
		//cout << i +1<< endl;
		if (cell[i][y].Num != cell[x][y].Num)
			k = 0;
		else if (++k == 4)
		{
			if (test)
			for (int m = 0; m <4; m++)
				cell[i - m][y].win = true;
			return cell[x][y].Num;
		}
	}


				//Diagonal

	for (int i = x - 3, j = y - 3, k = 0; i <= x + 3 && j <= y + 3 && i < col && j < row; i++,j++)
	{
		if (i < 0 || j < 0)
			continue;


		if (cell[i][j].Num != cell[x][y].Num)
			k = 0;
		else if (++k == 4)
		{
			if (test)
			for (int m = 0; m < 4; m++)
				cell[i - m][j-m].win = true;
			return cell[x][y].Num;
		}
	}


	for (int i = x + 3, j = y - 3, k = 0; i >= x - 3 && j <= y + 3 && i >=0&&j<row; i--, j++)
	{
		if (i > col - 1 || j < 0)
			continue;
		//cout << i+1 << ends << j+1 << endl;		
		if (cell[i][j].Num != cell[x][y].Num)
			k = 0;
		else if (++k == 4)
		{
			if(test)
			for (int m = 0; m < 4; m++)
				cell[i + m][j - m].win = true;
			return cell[x][y].Num;
		}
	}


	return 0;
}

int AI(Cell **cell)
{
	int Max = 0, Counter = 0, colume = rand() % col;

	cout << endl << endl;
	for (int i = 0; i < col; i++) {
		int y=-1;
		for(int j=0;j<row;j++)
			if (cell[i][j].Num == 0){
				y = j;
				break;
			}
		if (y < 0)
			continue;


		//Center
		if (i == (int)(col/2))
			Counter += 4;


		//Lines
		
		for (int j = i - 3, k = 0; j < i; j++)
			if (j >= 0)
				if (cell[j][y].Num == 1) {
					k--;
					if(k>0)
					Counter -= 2 + k * 2 + k;
					break;
				}
				else if (cell[j][y].Num == 2) {
					Counter += 2 + k;
					k++;
				}
		for (int j = i + 3, k = 0; j > i; j--)
			if (j <col)
				if (cell[j][y].Num == 1) {
					k--;
					if (k > 0)
					Counter -= 2 + k * 2 + k;
					break;
				}
				else if (cell[j][y].Num == 2) {
					Counter += 2 + k;
					k++;
				}

		
		Counter += 2;
		for (int j = i + 3; j > i; j--) {
			if (j < col)
				if (cell[j][y].Num == 1) {
					Counter -= 2;
					break;
				}
		}



		//Win
		cell[i][y].Num = 2;
		if (CheckWin(i, y, cell, false) == 2)
			Counter += 1000;
		cell[i][y].Num = 0;

		//Definse
		cell[i][y].Num = 1;
		if (CheckWin(i, y, cell, false) == 1)
			Counter += 900;
		cell[i][y].Num = 0;

		cell[i][y+1].Num = 1;
		if (CheckWin(i, y+1, cell, false) == 1)
			Counter += -100;
		cell[i][y+1].Num = 0;


		if (Counter > Max) {
			colume = i;
			Max = Counter;
		}
		//cout << i << ends << Counter <<ends<< endl;
		Counter = 0;
	}

	//cout << "AI Choose : " << colume << endl;
	return colume;
}

int User(RenderWindow& window, Cell **Cell)
{
	for (int i = 0; i < col; i++)
		if (Mouse::getPosition(window).x >= (i * 54) &&
			Mouse::getPosition(window).x <= (i + 1) * 54)
			return i;
	if (Mouse::getPosition(window).x > col)
		return col - 1;
	else
		return 0;
}
