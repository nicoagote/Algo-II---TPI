#ifndef EXTREME_EXORCISM_H
#define EXTREME_EXORCISM_H

#include "Contexto.h"
#include "Habitacion.h"
#include "TiposJuego.h"
#include "modulos_basicos/string_map.h"
#include "modulos_basicos/linear_set.h"
#include <list>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

using namespace std;

class ExtremeExorcism {
public:
  ExtremeExorcism(Habitacion h, set<Jugador> jugadores, PosYDir f_init,
                  list<Accion> acciones_fantasma, Contexto *ctx);

  void pasar();

  void ejecutarAccion(Jugador j, Accion a);

  list<pair<Jugador, PosYDir>> posicionJugadores() const;

  list<PosYDir> posicionFantasmas() const;

  PosYDir posicionEspecial() const;

  list<PosYDir> disparosFantasmas() const;

  set<Pos> posicionesDisparadas() const;

  bool jugadorVivo(Jugador j) const;

  const Habitacion &habitacion() const;

  PosYDir posicionJugador(Jugador j) const;

  const set<Jugador> &jugadores() const;

  const list<Fantasma> &fantasmas() const;

private:
    // ==================================== Estructuras auxiliares para más legibilidad ========================================= //
    struct EventoJugador;
    struct InfoJ;
    struct InfoJV;

    struct EventoJugador {
        unsigned int tick;
        Pos pos;
        Dir dir;
        bool dispara;

        EventoJugador(unsigned int, Pos, Dir, bool);

        bool operator==(const Evento&) const;
    };

    struct InfoJ {
        linear_set<InfoJV>::iterator* aInfoJV;
        list<EventoJugador> historial;
    };

    struct InfoJV {
        Jugador nombre;
        Pos pos;
        Dir dir;
    };

    unsigned int _ticks;
    linear_set<InfoJV> _jugadoresVivos;
    linear_set<InfoJ*> _jVJ;
    string_map<InfoJ> _jugadores;
    linear_set<Jugador> _nombres;
    list<PosYDir> _fantasmasVivos;
    linear_set<unsigned int> _fantasmasVivos_Id;
    vector<vector<Evento>> _fantasmas;
    Habitacion _habitacion;
    vector<vector<bool>> _mapa;
};

#endif
