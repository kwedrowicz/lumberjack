#pragma once

class Constants {
	static const int TRUE_MAX_HEIGHT;
	static const int TRUE_MAX_WIDTH;
	static const int ABSOLUTE_MAX_NO_TREES;
public:
	static int MAX_TIME;
	static int MAX_SIZE;
	static int MAX_HEIGHT;
	static int MAX_WIDTH;
	static int MAX_VALUE;
	static int MAX_WEIGHT;
	static int MAX_NO_TREES;

	static void limitNoTrees(int noTrees);
	static void limitBySize(int size);
	static void restore();
};