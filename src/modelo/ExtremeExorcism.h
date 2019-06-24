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
/* // Problemas con el const de la funcion.
  set<Pos> posicionesDisparadas() const;
*/
  bool jugadorVivo(Jugador j) const;

  const Habitacion &habitacion() const;

  PosYDir posicionJugador(Jugador j) const;

  const set<Jugador> &jugadores() const;

  const list<Fantasma> &fantasmas() const;

private:
    linear_set<Pos> &posicionesDisparadasAux(linear_set<Pos> &posicionesAlcanzadas);


    list<PosYDir>& disparosFantasmasAux(list<PosYDir>& res) const;

    // ==================================== Estructuras auxiliares para más legibilidad ========================================= //
    struct EventoJugador;
    struct InfoJ;
    struct InfoJV;
    struct Estrategia;
    struct Historial;

    struct EventoJugador {
        unsigned int tick;
        Pos pos;
        Dir dir;
        bool dispara;

        EventoJugador(unsigned int tick, Pos, Dir, bool);
        PosYDir posYDir() const;
    };

    struct Historial {
    public:
        Historial();
        Historial(Pos pos, Dir dir);
        void actuar(Habitacion& h, Accion a , unsigned int tick);
        void morir(unsigned int tick);
        EventoJugador back() const;
        const vector<Evento>& armarEstrategia() const;

    private:
        EventoJugador actualizar(Habitacion& h, Accion a, unsigned int tick, EventoJugador e);
        list<EventoJugador> historial;
    };


    struct InfoJ {
        linear_set<InfoJV>::iterator* aInfoJV;
        Historial historial;

        InfoJ(); // TODO!!! : NO USAR!!! - EL HISTORIAL ROMPE EL INVARIANTE
        InfoJ(linear_set<InfoJV>::iterator*, Historial);
    };

    struct InfoJV {
        Jugador nombre;
        Pos pos;
        Dir dir;

        InfoJV(Jugador, Pos, Dir);
        PosYDir posYDir() const;
        operator pair<Jugador, PosYDir>() const;
    };

    struct Estrategia {
        public:
            Estrategia(vector<Evento>);
            Estrategia(Habitacion& h, PosYDir, list<Accion>);
            Estrategia(Historial& h);
            operator Fantasma() const;
            Evento operator[](unsigned int tick) const;

        private :
            Evento actuar(Habitacion& h, Accion a, Evento e) const;
            Dir invertir(Dir d) const;

            vector<Evento> _estrategia;
    };

    void actuarFantasmas();
    void actuarJugador(Jugador& j, Accion& a);
    bool cambioRonda();

    void resetearJugadores();
    void resetearFantasmas(Jugador);


    unsigned int _ticks;
    linear_set<InfoJV> _jugadoresVivos;
    linear_set<InfoJ*> _jVJ;
    string_map<InfoJ> _jugadores;
    linear_set<Jugador> _nombres;
    list<PosYDir> _fantasmasVivos;
    linear_set<unsigned int> _fantasmasVivos_Id;
    vector<Estrategia> _fantasmas;
    Habitacion _habitacion;
    vector<vector<bool>> _mapa;
    Contexto *_ctx;
};

#endif
