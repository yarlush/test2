#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include<random>
#include<algorithm>
#include<raylib.h>
#define RAYGUI_IMPLEMENTATION
#include<raygui.h>

using namespace std;
#define JACK 11 
#define QUEEN 12 
#define KING 13
#define ACE 14

#define HEART 3
#define DIAMOND 4
#define CLUBS 5 
#define SPADE 6

#define FLUSHROYAL 0
#define STREETFLUSH 1
#define KARE 2
#define FULLHOUSE 3
#define FLUSH 4
#define STREET 5
#define SET 6
#define TWOPAIRS 7
#define PAIR 8
#define HIGHCARD 9



random_device rd;
auto rng{ default_random_engine{ rd() } };
vector<string> Combs{ "FLUSHROYAL","STREETFLUSH","KARE","FULLHOUSE","FLUSH","STREET", "SET","TWOPAIRS","PAIR","HIGHCARD"};

struct Card
{
	Rectangle rect{ 0,0,40,80 };
	int Value;
	char Suit;
	Card(int v, char s) : Value{ v }, Suit{s} {}
	void printValueSym()
	{
		switch (Value)
		{
		case JACK:
			cout << 'J';
			return;
		case QUEEN:
			cout << 'Q';
			return;
		case KING:
			cout << 'K';
			return;
		case ACE:
			cout << 'A';
			return;
		default:
			cout << Value;
			return;
		}
	}
};
ostream& operator <<(ostream& os, Card c) 
{
	c.printValueSym();
	os << c.Suit;
	return os;
}

struct Combination
{
	int Number;
	Card HighCard;
	Combination(int n, Card c) : Number{ n }, HighCard{c}{}
};

ostream& operator <<(ostream& os, Combination c)
{
	os << Combs[c.Number] << " " << c.HighCard;
	return os;
}





struct Deck
{
	vector<Card> Cards;
	Deck(initializer_list<Card> l) :Cards{l} {}
	Deck() {}
	void sortDec()
	{
		sort(Cards.begin(), Cards.end(), [](Card l, Card r)
			{
				return l.Value > r.Value;
			}
		);
	}
	void addCard(Card c)
	{
		Cards.push_back(c);
	}
	Card takeCard()
	{
		Card c = Cards.back();
		Cards.pop_back();
		return c;
	}
	void Shuffle()
	{
		shuffle(Cards.begin(),Cards.end(),rng);
	}
	void fillDeck()
	{
		for (int i = 3; i <= 6; i++)
		{
			for (int j = 2; j <= 14; j++)
			{
				addCard(Card(j,i));
			}
		}

	}
	Combination checkCombination()
	{
		sortDec();

		bool flush = false;
		bool street = false;
		bool kare = false;
		bool pair = false;
		bool triple = false;
		Card hc(2,3);


		for (auto card : Cards)
		{

			if (count_if(Cards.begin(), Cards.end(), [&](Card c) {return c.Value == card.Value; }) == 2)
			{
				pair = true;
				hc = card;
				break;
			};
		}



		for (auto card : Cards)
		{

			if (count_if(Cards.begin(), Cards.end(), [&](Card c) {return c.Value == card.Value; }) == 3)
			{
				triple = true;
				hc = card;
				break;
			};
		}

		for (auto card : Cards)
		{

			if (count_if(Cards.begin(), Cards.end(), [&](Card c) {return c.Value == card.Value; }) == 4)
			{
				kare = true;
				hc = card;
				break;
			};
		}
		int c1 = count_if(Cards.begin(), Cards.end(), [&](Card c) {return c.Suit == Cards[0].Suit; }) ;
		
		if (c1 == 5)
		{

			flush = true;
		};


		int c2 = count_if(Cards.begin(), Cards.end(), [](Card c)
			{
				static int v = c.Value;
				return c.Value == v--;
			}

		) ;

		

		if (c2 == 5)
		{
			
			street = true;
		};



		if (Cards[0].Value == ACE && street && flush)
		{
			return Combination(FLUSHROYAL,Cards[0]);
		}
		if (street && flush)
		{
			return Combination(STREETFLUSH, Cards[0]);
		}
		if (kare)
		{
			return Combination(KARE, hc);
		}

		if (triple && pair)
		{
			return Combination(FULLHOUSE, hc);
		}
		if (flush)
		{
			return Combination(FLUSH, Cards[0]);
		}
		if (street)
		{
			return Combination(STREET, Cards[0]);
		}
		if (triple)
		{
			return Combination(SET, hc);
		}
		if (pair)
		{
			for (auto card : Cards)
			{

				if (count_if(Cards.begin(), Cards.end(), [&](Card c) {return c.Value == card.Value && card.Value != hc.Value; }) == 2)
				{
					return Combination(TWOPAIRS, hc.Value > card.Value? hc : card);
				};
			}
			return Combination(PAIR, hc);
		}

		return Combination(HIGHCARD, Cards[0]);
	}
};

Deck& operator+(Deck& d1,Deck& d2)
{
	Deck d3;
	d3.Cards.insert(d3.Cards.end(), d1.Cards.begin(), d1.Cards.end());
	d3.Cards.insert(d3.Cards.end(), d2.Cards.begin(), d2.Cards.end());
	return d3;
}
ostream& operator<<(ostream& os, Deck& d)
{
	for (auto card : d.Cards)
		os << card << " ";
	return os;
}


class Player
{



};

int main()
{
	const int screenWidth = 800;
	const int screenHeight = 600;

	SetConfigFlags(FLAG_WINDOW_UNDECORATED);
	InitWindow(screenWidth, screenHeight, "raygui - portable window");

	// General variables
	Vector2 mousePosition = { 0 };
	Vector2 windowPosition = { 500, 200 };
	Vector2 panOffset = mousePosition;
	Vector2 panOffsetCard = mousePosition;
	bool dragWindow = false;

	SetWindowPosition(windowPosition.x, windowPosition.y);

	bool exitWindow = false;

	SetTargetFPS(60);
	Deck player;
	Deck table;
	table.fillDeck();
	table.Shuffle();

	bool dragCard = false;
	int chosen = 0;
	player.addCard(table.takeCard());
	player.Cards.back().rect.x = 60;
	player.Cards.back().rect.y = 60;
	// Main game loop
	while (!exitWindow && !WindowShouldClose())    // Detect window close button or ESC key
	{
		// Update
		//----------------------------------------------------------------------------------
		mousePosition = GetMousePosition();
		if(!dragCard)
			for (int i = 0; i < player.Cards.size(); i++)
			{
				if (CheckCollisionPointRec(mousePosition, player.Cards[i].rect))
				{
					chosen = i;
				}
			}



		if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !dragWindow)
		{
			if (CheckCollisionPointRec(mousePosition, Rectangle{ 0, 0, screenWidth, 20 }))
			{
				dragWindow = true;
				panOffset = mousePosition;
			}
		}

		if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !dragCard)
		{
			if (CheckCollisionPointRec(mousePosition, player.Cards[chosen].rect))
			{
				dragCard = true;
				panOffsetCard.x = mousePosition.x - player.Cards[chosen].rect.x;
				panOffsetCard.y = mousePosition.y - player.Cards[chosen].rect.y;
			}
		}

		if (dragWindow)
		{
			windowPosition.x += (mousePosition.x - panOffset.x);
			windowPosition.y += (mousePosition.y - panOffset.y);

			SetWindowPosition((int)windowPosition.x, (int)windowPosition.y);

			if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) dragWindow = false;
		}

		if (dragCard)
		{
			player.Cards[chosen].rect.x = mousePosition.x-panOffsetCard.x ;
			player.Cards[chosen].rect.y = mousePosition.y - panOffsetCard.y;

			if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) dragCard = false;
		}
		//----------------------------------------------------------------------------------

		// Draw
		//----------------------------------------------------------------------------------
		BeginDrawing();

		ClearBackground(RAYWHITE);

		exitWindow = GuiWindowBox( Rectangle { 0, 0, screenWidth, screenHeight }, "#198# PORTABLE WINDOW");

		if (GuiButton({ 50,50,40,80 }, "Deck"))
		{
			player.addCard(table.takeCard());
			player.Cards.back().rect.x = 100;
			player.Cards.back().rect.y = 100;
		}

		for (int i = 0; i < player.Cards.size(); i++)
		{
			if (GuiButton(player.Cards[i].rect, "Card"))
			{
			}
		}


		EndDrawing();
		//----------------------------------------------------------------------------------
	}

	// De-Initialization
	//--------------------------------------------------------------------------------------
	CloseWindow();        // Close window and OpenGL context
	//--------------------------------------------------------------------------------------

}