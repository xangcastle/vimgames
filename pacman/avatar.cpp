/*

Copyright 2015 Jamal Moon

PacVim is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License (LGPL) as 
published by the Free Software Foundation, either version 3 of the 
License, or (at your option) any later version.

PacVim program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

 */
#include "avatar.h"
#include <sstream>

avatar::avatar() {
	x = 1;
	y = 1;
	lives = 3;
	points = 0;
	portrait = "👻";
	isPlayer = false;
	color = COLOR_WHITE;
}

avatar::avatar(int a, int b) {
	x = a;
	y = b;
	lives = 3;
	points = 0;
	portrait = "👻";
	isPlayer = false;
	color = COLOR_WHITE;
	letterUnder = charAt(a, b);
}


avatar::avatar(int a, int b, bool human) {
	x = a;
	y = b;
	lives = 3;
	points = 0;
	isPlayer = human;
	if(human)
		portrait = "😃"; // default for player
	else
		portrait = "👻";
	letterUnder = charAt(a, b);
	moveTo(a, b);
}

avatar::avatar(int a, int b, bool human, int c) {
	avatar(a, b, human);
	color = c;
}

int avatar::getPoints() { return points; }
bool avatar::getPlayer() { return isPlayer; }
int avatar::getX() { return x; }
int avatar::getY() { return y; }
std::string avatar::getPortrait() { return portrait; }

bool avatar::setPos(int theX, int theY) { 
	x = theX;
	y = theY;
	return true;
}

bool avatar::moveTo(int a, int b) {
	if(!isValid(a, b))
		return false;

	// character at destination 
	chtype curChar = charAt(a, b);
	if(isPlayer) {
		if((curChar & COLOR_PAIR(6)) == COLOR_PAIR(6) ) {
			GAME_WON = -1;
			return false;
		}
		// hit a ghost.. red color
		if((curChar & COLOR_PAIR(1)) == COLOR_PAIR(1)) {
			GAME_WON = -1;
			return false;
		}
		
		// points
		if((curChar & A_CHARTEXT) != ' ' && !(curChar & COLOR_PAIR(2))) {
			points++;
		}
		
		// Restore old position with Green Highlight
		writeAt(x, y, letterUnder, COLOR_GREEN);

		// move
		x = a;
		y = b;
		
		// Save new position
		letterUnder = curChar;
		
		// Draw Player Emoji
		writeAt(x, y, portrait); 
		
		// Move system cursor to new position (scaled)
		move(b, a * 2);

		if(points >= TOTAL_POINTS) {
			GAME_WON = 1;
		}
	}
	else { // it is a ghost

		// get player position, see if we stepped on the player
		int playerX, playerY;
		getyx(stdscr, playerY, playerX);
		// Adjust playerX to logical coordinate
		playerX /= 2;
		
		if(playerY == b && playerX == a) {
			GAME_WON = -1; // hit the player, end the game
		}
		// check if we are hitting a ghost-- if so, it's an invalid location
		if( (curChar & COLOR_PAIR(1)) == COLOR_PAIR(1) ) {
			return false;
		}
		writeAt(x, y, letterUnder);
		letterUnder = charAt(a, b);

		writeAt(a, b, portrait,  color); 
		x = a;
		y = b;
	}
	refresh();
	return true;

}
bool avatar::moveRight() {
	if(!isValid(x+1, y)) 
		return false;
	
	moveTo(x+1, y+0);
	return true;
}

bool avatar::moveLeft() {
	if(!isValid(x-1, y))
		return false;
	
	moveTo(x-1, y);
	return true;
}

bool avatar::moveUp() {
	if(!isValid(x,y-1)) 
		return false;
	
	moveTo(x, y-1);
	return true;
}

bool avatar::moveDown() {
	if(!isValid(x,y+1))
		return false;
	moveTo(x, y+1);
	return true;
}

bool avatar::parseWordEnd(bool isWord) {
	// Formula: Get next char, is it alphanumeric? If so, loop & break
	//		on nonalpha-, and viceversa. 
	// 2nd case: if you are not at the end of a word, loop until you
	//		reach a space
	
	// Use chtype to preserve attributes, but mask for checks
	if((charAt(x+1, y) & A_CHARTEXT) == ' ') {
		moveRight();
	}
	// store the current character type
	chtype curCharVal = charAt(x, y);
	char curChar = curCharVal & A_CHARTEXT;
	bool isAlpha = isalnum((unsigned char)curChar);
	
	chtype nextCharVal = charAt(x+1, y);
	char nextChar = nextCharVal & A_CHARTEXT;

	while(nextChar == ' ') {
		if(!moveTo(x+1, y))
			return false;
		nextCharVal = charAt(x+1, y);
		nextChar = nextCharVal & A_CHARTEXT;
		
		curCharVal = charAt(x, y);
		curChar = curCharVal & A_CHARTEXT;
		isAlpha = isalnum((unsigned char)curChar);
	}
	// breakOnSpace = true if the current character isn't the end of a word
	bool breakOnSpace = (nextChar != ' ' && curChar != ' ');
	bool breakOnAlpha = !isalnum((unsigned char)nextChar) && nextChar != ' ';
	while(true) { // no definite loop #; break when we reach conditions
		if((!breakOnAlpha == !isalnum((unsigned char)nextChar))  && isWord) {
			break;
		}
		else if(breakOnSpace && (nextChar == ' ')) {
			break; 
		}
		else if(nextChar == '#') { // not allowed to go on # so break
			break;
		}
		else { // iterate
			if(!moveTo(x+1, y))
				return false;
			if(nextChar != ' ')
				breakOnSpace = true;
			nextCharVal = charAt(x+1, y);
			nextChar = nextCharVal & A_CHARTEXT;
			
			curCharVal = charAt(x, y);
			curChar = curCharVal & A_CHARTEXT;
		}
	}
	return true;
}
				
bool avatar::parseWordBackward(bool isWord) {
	// Formula: Get next char, is it alphanumeric? If so, loop & break
	//		on nonalpha-, and viceversa. 
	// 2nd case: if you are not at the end of a word, loop until you
	//		reach a space

	// store the current character type
	chtype curCharVal = charAt(x, y); 
	char curChar = curCharVal & A_CHARTEXT;
	bool isAlpha = isalnum((unsigned char)curChar);
	
	chtype nextCharVal = charAt(x-1, y); 
	char nextChar = nextCharVal & A_CHARTEXT;

	// breakOnSpace = true if the current character isn't the end of a word
	bool breakOnSpace = (nextChar != ' ' && curChar != ' ');
	bool breakOnAlpha = breakOnSpace && (!isAlpha && !isalnum((unsigned char)nextChar) && isWord); 
	bool breakOnNonAlpha = breakOnSpace && ((isAlpha && isalnum((unsigned char)nextChar))
	 		|| (!isAlpha && isalnum((unsigned char)nextChar)))&& isWord;


	while(true) { // no definite loop #; break when we reach conditions
		if(curChar != ' ' && ((isalnum((unsigned char)nextChar) && breakOnAlpha) || (!isalnum((unsigned char)nextChar) && breakOnNonAlpha))) {
			break;
		}
		else if(breakOnSpace && nextChar == ' ') {
			break; 
		}
		else if(nextChar == '#') { // not allowed to go on # so break
			return false;
		}
		else { // iterate

			if(!moveTo(x-1, y))
				return false;
			if(nextChar != ' ' && !breakOnSpace) {
				if(isWord) {	
					breakOnAlpha = !isalnum((unsigned char)nextChar);
					breakOnNonAlpha = isalnum((unsigned char)nextChar);
				}
				breakOnSpace = true;
			}
			nextCharVal = charAt(x-1, y); 
			nextChar = nextCharVal & A_CHARTEXT;
			curCharVal = charAt(x, y);
			curChar = curCharVal & A_CHARTEXT;
		}
	}
	return true;
}

bool avatar::parseWordForward(bool isWord) {
	chtype curCharVal = charAt(x, y); 
	char curChar = curCharVal & A_CHARTEXT;
	
	bool isAlpha = isalnum((unsigned char)curChar);
	char lastChar= 'X';

	bool breakOnAlpha = !isalnum((unsigned char)curChar);
	
	while(true) {
		if(curChar != ' ' && (!isalnum((unsigned char)curChar) == !breakOnAlpha) && isWord ) { // if they are the same
			break;
		}
		else if(lastChar == ' ' && curChar != ' ') {
			break;
		}
		else if(lastChar == '#' || curChar == '#') {
			moveTo(x-1, y);
			return false;
		}
		else {
			lastChar = curChar;
			if(!moveTo(x+1,y))
				return false;
			curCharVal = charAt(x,y);
			curChar = curCharVal & A_CHARTEXT;
		}
	}
	return true;
}

bool avatar::parseToEnd() {
	// go to end of line, then move back until hash tag (wall)
	// after that, move left once more so you are inside the board

	x = WIDTH;
	while(isValid(x, y)) {
		x--;
		writeError("shiet");
	}
	while(!isValid(x, y)) {
		x--;
		if(x > WIDTH || x <= 1 || y<= 1 || y > HEIGHT)
			break;
	}
	moveTo(x, y);
	return true;
}

bool avatar::parseToBeginning() { 
	x = 0;
	while(isValid(x, y) && x <= WIDTH)  {
		 x++; 
	}
	while(!isValid(x, y) && x <= WIDTH) {
		 x++;
	 }
	moveTo(x, y);
	return true;
}

