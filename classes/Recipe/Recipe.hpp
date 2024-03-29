#ifndef __RECIPE__
#define __RECIPE__

#include <vector>
#include <string>
#include <iostream>
#include <sstream>

using namespace std;

class Recipe {
private:
  int row;
  int col;
  vector<string> items;
  string result;
  int resultQty;

public:
  //! Constructor
  Recipe();
  Recipe(vector<string> data);

  //! Getter
  int getRow() const;
  int getCol() const;
  string getResult() const;
  vector<string> getItems() const;
  int getResultQty() const;

  //! For showing recipes
  friend ostream& operator<<(ostream &os, const Recipe &r);
};

class RecipeWrongFormatException: public exception {
private:
  const vector<string> data;
public:
  RecipeWrongFormatException(vector<string> data): data(data) {}
  const char* what() const throw() {
    string out = "Recipe has wrong format\n";
    for (int i = 0; i < this->data.size(); i++) {
      out += this->data[i] + "\n";
    }
    const char* res = out.c_str();
    return res;
  }
};

#endif