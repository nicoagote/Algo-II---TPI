#include "ExtremeExorcism.h"

// TODO!!! Completar

ExtremeExorcism::ExtremeExorcism(Habitacion h, set<Jugador> jugadores, PosYDir f_init,
list<Accion> acciones_fantasma, Contexto *ctx) {}

void ExtremeExorcism::pasar() {}

void ExtremeExorcism::ejecutarAccion(Jugador j, Accion a) {}

list<pair<Jugador, PosYDir>> ExtremeExorcism::posicionJugadores() const {
    return list<pair<Jugador, PosYDir>>();
}

list<PosYDir> ExtremeExorcism::posicionFantasmas() const {
    return list<PosYDir>();
}

PosYDir ExtremeExorcism::posicionEspecial() const {
    return PosYDir();
}

list<PosYDir> ExtremeExorcism::disparosFantasmas() const {}

set<Pos> ExtremeExorcism::posicionesDisparadas() const {}

bool ExtremeExorcism::jugadorVivo(Jugador j) const {}

const Habitacion& ExtremeExorcism::habitacion() const {}

PosYDir ExtremeExorcism::posicionJugador(Jugador j) const {}

const set<Jugador>& ExtremeExorcism::jugadores() const {}

const list<Fantasma>& ExtremeExorcism::fantasmas() const {}