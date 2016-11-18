#include "Tree.h"
#include <cmath>


Tree::Tree(int h, int w, int wu, int vu, int xx, int yy, int num){
    height = h;
    width = w;
    weight_unit = wu;
    value_unit = vu;
    number = num;
    x = xx;
    y = yy;
}

Tree::Tree(const Tree &tree){
    height = tree.height;
    width = tree.width;
    weight_unit = tree.weight_unit;
    value_unit = tree.value_unit;
    number = tree.number;
    x = tree.x;
    y = tree.y;
}

int Tree::getValue(){
    return height*width*value_unit;
}

int Tree::getWeight(){
    return height*width*weight_unit;
}

bool Tree::isHeavier(Tree b){
    return getWeight() > b.getWeight();
}

int Tree::getDistance(Tree b){
    return int(std::abs(x-b.x)+std::abs(y-b.y));
}