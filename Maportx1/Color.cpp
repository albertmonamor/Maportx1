#include "Color.h"
#include <windows.h>


Color::Color() {

}
Color::~Color() {

}

void Color::setColorOutput(int _color) {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, _color);
}