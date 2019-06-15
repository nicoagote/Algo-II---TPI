#include "ExtremeExorcism.h"

// TODO!!! Completar

ExtremeExorcism::ExtremeExorcism(Habitacion h, set<Jugador> jugadores, PosYDir f_init,
list<Accion> acciones_fantasma, Contexto *ctx) : _habitacion(h) {}

void ExtremeExorcism::pasar() {}

void ExtremeExorcism::ejecutarAccion(Jugador j, Accion a) {}

list<pair<Jugador, PosYDir>> ExtremeExorcism::posicionJugadores() const {
    return list<pair<Jugador, PosYDir>>();
}

list<PosYDir> ExtremeExorcism::posicionFantasmas() const {
    return list<PosYDir>();
}

PosYDir ExtremeExorcism::posicionEspecial() const {
    return PosYDir(make_pair(0,0), ARRIBA);
}

list<PosYDir> ExtremeExorcism::disparosFantasmas() const {
    return list<PosYDir>();
}

set<Pos> ExtremeExorcism::posicionesDisparadas() const {
    return set<Pos>();
}

bool ExtremeExorcism::jugadorVivo(Jugador j) const {
    return false;
}

const Habitacion& ExtremeExorcism::habitacion() const {
    return Habitacion(0, set<Pos>());
}

PosYDir ExtremeExorcism::posicionJugador(Jugador j) const {
    return PosYDir(make_pair(0,0), ARRIBA);
}

const set<Jugador>& ExtremeExorcism::jugadores() const {
    return set<Jugador>();
}

const list<Fantasma>& ExtremeExorcism::fantasmas() const {
    return list<Fantasma>();
}