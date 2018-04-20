#include <iostream>
#include <math.h>
#include <algorithm>
#include <SFML/Graphics.hpp>
//#define _CRT_SECURE_NO_WARNINGS
using namespace std;
using namespace sf;

const int window_w = 1920;
const int window_h = 1080;
float wid = 1920;
int hei = 1080;
float wid_n = 1920;
float hei_n = 1080;
const int l_size = 80;
const int r_size = 30;
const int h_size = 100;
float speed = 50;
float del = 0;
const int scale = 1;
RenderWindow window(VideoMode(window_w, window_h), "AVL Tree");
Font font;
View view;

struct Node {
	int c;
	Node *l, *r;
	int h;
	Node(int cc) :c(cc), h(1), l(NULL), r(NULL) {}
};
typedef Node* Tree;

int get_h(Tree t) {
	if (!t)return 0;
	return t->h;
}

int balancefact(Tree t) {
	if (!t)return 0;
	return get_h(t->r) - get_h(t->l);
}

void update(Tree t) {
	if (!t)return;
	t->h = max(get_h(t->l), get_h(t->r)) + 1;
}

Tree rot_right(Tree t) {
	Tree q = t->l;
	t->l = q->r;
	q->r = t;
	update(t);
	update(q);
	return q;
}

Tree rot_left(Tree t) {
	Tree q = t->r;
	t->r = q->l;
	q->l = t;
	update(t);
	update(q);
	return q;
}

Tree balance(Tree t) {
	if (!t)return t;
	update(t);
	if (balancefact(t) == 2) {
		if (balancefact(t->r) < 0)
			t->r = rot_right(t->r);
		return rot_left(t);
	}
	if (balancefact(t) == -2) {
		if (balancefact(t->l) > 0)
			t->l = rot_left(t->l);
		return rot_right(t);
	}
	return t;
}

Tree add(int c, Tree t) {
	if (!t) {
		t = new Node(c);
		return t;
	}
	if (t->c == c)return t;
	if (t->c > c) {
		t->r = add(c, t->r);
	}
	else {
		t->l = add(c, t->l);
	}
	return balance(t);
}

Tree get_min(Tree t) {
	if (!t->l)return t;
	return get_min(t->l);
}

Tree erase(int c, Tree t) {
	if (!t)return t;
	if (t->c > c) {
		t->r = erase(c, t->r);
	}
	else if (t->c < c) {
		t->l = erase(c, t->l);
	}
	else if (t->l && t->r) {
		t->c = get_min(t->r)->c;
		t->r = erase(t->c, t->r);
	}
	else if (t->l)
		t = t->l;
	else
		t = t->r;
	return balance(t);
}

void draw(int x, int y, int c) {
	CircleShape cir;
	Text text;
	text.setFont(font);
	cir.setOrigin(Vector2f(r_size, r_size));
	cir.setRadius(r_size);
	cir.setOutlineColor(Color::Blue);
	cir.setOutlineThickness(3);
	text.setCharacterSize(144);
	text.setString(to_string(c));
	text.setOrigin(text.getLocalBounds().left + text.getLocalBounds().width / 2,
		text.getLocalBounds().top + text.getLocalBounds().height / 2);
	float min_size = min((r_size*1.5f) / text.getLocalBounds().width, (r_size*1.5f / 2) / text.getLocalBounds().height);
	text.setScale(Vector2f(min_size, min_size));
	cir.setPosition(x, y);
	text.setPosition(Vector2f(x, y));
	text.setColor(Color::Black);
	window.draw(cir);
	window.draw(text);
}

void draw_edg(int x1, int y1, int x2, int y2) {
	if (x1 == 0 && y1 == 0)return;
	RectangleShape line(Vector2f(sqrt((x2 - x1)*(x2 - x1) + (y2 - y1)*(y2 - y1)), 6));
	line.setFillColor(Color::Cyan);
	line.setPosition(Vector2f(x1, y1));
	line.rotate(atan2(y2 - y1, x2 - x1) * 180.0 / atan2(0, -1));
	window.draw(line);
}

int get(Tree t, int l, int h) {
	if (!t)return 0;
	int r = l;
	if (t->l)
		r = get(t->l, l, h - 1);
	else
		r += l_size*scale;
	if (t->r)
		r = get(t->r, r, h - 1);
	else
		r += l_size*scale;
	if (!t->l && !t->r)
		r += l_size;
	int x_cor = l + (r - l) / 2;
	int y_cor = h * h_size;
	draw(x_cor, y_cor, t->c);
	return r;
}

pair<int,pair<int,int>> get_edg(Tree t, int l, int h) {
	if (!t)return make_pair(0, make_pair(0, 0));
	int r = l;
	pair<int, int> left = make_pair(0, 0);
	pair<int, int> right = make_pair(0, 0);
	if (t->l) {
		pair<int, pair<int, int>> res = get_edg(t->l, l, h - 1);
		r = res.first;
		left = res.second;
	}
	else
		r += l_size*scale;
	if (t->r) {
		pair<int, pair<int, int>> res = get_edg(t->r, r, h - 1);
		r = res.first;
		right = res.second;
	}
	else
		r += l_size*scale;
	if (!t->l && !t->r)
		r += l_size;
	int x_cor = l + (r - l) / 2;
	int y_cor = h * h_size;
	draw_edg(left.first, left.second, x_cor, y_cor);
	draw_edg(right.first, right.second, x_cor, y_cor);
	return make_pair(r, make_pair(x_cor, y_cor));
}

int main() {
	view.reset(FloatRect(0, 0, window_w, window_h));
	srand(time(NULL));
	Tree t = 0;
	Clock clock;
	font.loadFromFile("CyrilicOld.TTF");
	bool change = true;
	int tim = 0;
	int kadr = 0;
	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event) || change) {
			float del_tim = clock.getElapsedTime().asMicroseconds();
			del = del_tim / 4e4;
			clock.restart();
			tim += del_tim;
			kadr++;
			if (tim > 1e6) {
				tim -= 1e6;
			//	cout << kadr << endl;
				kadr = 0;
			}
			if (event.type == Event::Closed || Keyboard::isKeyPressed(Keyboard::Escape)) {
				window.close();
			}
			if (Keyboard::isKeyPressed(Keyboard::Dash)) {
				view.zoom(1.03f);
				speed = speed*1.03f;
				wid = wid*1.03f;
				change = true;
			}
			if (Keyboard::isKeyPressed(Keyboard::Equal)) {
				view.zoom(0.97f);
				speed = speed*0.97f;
				wid = wid*0.97f;
				change = true;
			}
			if (Keyboard::isKeyPressed(Keyboard::Left)) {
				view.move(Vector2f(-speed*del, 0));
				wid += speed*del;
				change = true;
			}
			if (Keyboard::isKeyPressed(Keyboard::Down)) {
				view.move(Vector2f(0, speed*del));
				change = true;
			}
			if (Keyboard::isKeyPressed(Keyboard::Up)) {
				view.move(Vector2f(0, -speed*del));
				change = true;
			}
			if (Keyboard::isKeyPressed(Keyboard::Right)) {
				view.move(Vector2f(speed*del, 0));
				wid += speed*del;
				change = true;
			}
			if (Mouse::isButtonPressed(Mouse::Left)) {
				for (int i = 0; i < 10; i++) {
					int key = rand() - rand();
					t = add(key, t);
				}
				change = true;
			}
			if (Keyboard::isKeyPressed(Keyboard::A)) {
				int key;
				cin >> key;
				t = add(key, t);
				change = true;
			}
			if (Keyboard::isKeyPressed(Keyboard::E)) {
				int key;
				cin >> key;
				t = erase(key, t);
				change = true;
			}
			if (Keyboard::isKeyPressed(Keyboard::N)) {
				view.zoom(wid_n / wid);
				speed *= wid_n / wid;
				wid = wid_n;
			}
			if (change) {
				window.setView(view);
				window.clear(Color(128, 106, 89));
				get_edg(t, 0, get_h(t));
				wid_n = get(t, 0, get_h(t));
				hei_n = get_h(t)*h_size;
				//change = false;
				window.display();
			}
		}
	}
	return 0;
}