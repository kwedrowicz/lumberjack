#pragma once
struct Position {
	int x, y;
	Position(int x, int y) : x(x), y(y) {}
	
	Position operator+(const Position b) const {
		return Position(x+b.x, y+b.y);
	}
};
