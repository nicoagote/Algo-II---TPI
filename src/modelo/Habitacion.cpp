#include "Habitacion.h"
#include "assert.h"
#include <istream>
#include <sstream>
#include <string>

Habitacion string_to_hab(std::istream& is) {
    int row = 0;
    int col = 0;
    int max_col = 0;
    int width;
    int height;

    set<Pos> occupied;

    char c;
    while (is.get(c)) {
        if (c == '#') {
            occupied.insert(Pos(col, row));
            col++;
        } else if (c == '\n') {
            row++;
            max_col = std::max(col, max_col);
            col = 0;
        } else {
            col++;
        }
    }
    width = max_col;
    height = row;

    assert(height == width);

    return Habitacion(height, occupied);
}

Habitacion string_to_hab(string s) {
    std::istringstream is(s);
    int row = 0;
    int col = 0;
    int max_col = 0;
    int width;
    int height;

    set<Pos> occupied;

    char c;
    while (is.get(c)) {
        if (c == '#') {
            occupied.insert(Pos(col, row));
            col++;
        } else if (c == '\n') {
            row++;
            max_col = std::max(col, max_col);
            col = 0;
        } else {
            col++;
        }
    }
    width = max_col;
    height = row;

    assert(height == width);

    return Habitacion(height, occupied);
}

Habitacion::Habitacion(unsigned int tam, set<Pos> ocupadas) : _habitacion(vector<vector<bool> >(tam, vector<bool>(tam, false))) {
    for (Pos p : ocupadas) {
        _habitacion[p.first][p.second] = true;
    }
}

unsigned int Habitacion::tam() const {
    return _habitacion.size();
}

bool Habitacion::ocupado(Pos p) const {
    return _habitacion[p.first][p.second];
}

bool Habitacion::operator==(const Habitacion& h) const {
    return _habitacion == h._habitacion;
}
