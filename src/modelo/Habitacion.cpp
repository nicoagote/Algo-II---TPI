#include "Habitacion.h"
#include "assert.h"
#include <istream>
#include <sstream>
#include <string>



/* test nro 789*/


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

PosYDir Habitacion::actualizar(Accion a, PosYDir pd) const {
    if(a == DISPARAR or a == ESPERAR){
        return pd;
    }else if(a == MARRIBA){
        if(not disponible(make_pair(pd.pos.first, pd.pos.second+1))){
            return PosYDir(pd.pos, ARRIBA);
        }else{
            return PosYDir(make_pair(pd.pos.first, pd.pos.second+1), ARRIBA);
        }
    }else if(a == MABAJO){
        if(not disponible(make_pair(pd.pos.first, pd.pos.second-1))){
            return PosYDir(pd.pos, ABAJO);
        }else{
            return PosYDir(make_pair(pd.pos.first, pd.pos.second-1), ABAJO);
        }
    }else if(a == MDERECHA){
        if(not disponible(make_pair(pd.pos.first+1, pd.pos.second))){
            return PosYDir(pd.pos, DERECHA);
        }else{
            return PosYDir(make_pair(pd.pos.first+1, pd.pos.second), DERECHA);
        }
    }else if(a == MIZQUIERDA){
        if(not disponible(make_pair(pd.pos.first-1, pd.pos.second))){
            return PosYDir(pd.pos, IZQUIERDA);
        }else{
            return PosYDir(make_pair(pd.pos.first-1, pd.pos.second), IZQUIERDA);
        }
    }
}

PosYDir Habitacion::actualizarD(Dir d, PosYDir pd) const{
    if(d == ARRIBA){
        if(not disponible(make_pair(pd.pos.first, pd.pos.second+1))){
            return PosYDir(pd.pos, ARRIBA);
        }else{
            return PosYDir(make_pair(pd.pos.first, pd.pos.second+1), ARRIBA);
        }
    }else if(d == ABAJO){
        if(not disponible(make_pair(pd.pos.first, pd.pos.second-1))){
            return PosYDir(pd.pos, ABAJO);
        }else{
            return PosYDir(make_pair(pd.pos.first, pd.pos.second-1), ABAJO);
        }
    }else if(d == DERECHA){
        if(not disponible(make_pair(pd.pos.first+1, pd.pos.second))){
            return PosYDir(pd.pos, DERECHA);
        }else{
            return PosYDir(make_pair(pd.pos.first+1, pd.pos.second), DERECHA);
        }
    }else if(d == IZQUIERDA){
        if(not disponible(make_pair(pd.pos.first-1, pd.pos.second))){
            return PosYDir(pd.pos, IZQUIERDA);
        }else{
            return PosYDir(make_pair(pd.pos.first-1, pd.pos.second), IZQUIERDA);
        }
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

bool Habitacion::disponible(Pos pos) const {
    return (pos.first >= 0 and pos.first < tam() and pos.second >= 0 and pos.second < tam() and not ocupado(pos));
}
