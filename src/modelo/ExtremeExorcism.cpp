#include "ExtremeExorcism.h"

// TODO!!! Completar

ExtremeExorcism::ExtremeExorcism(Habitacion h, set<Jugador> jugadores, PosYDir f_init, list<Accion> acciones_fantasma, Contexto *ctx) : _ticks(0), _jugadoresVivos(linear_set<InfoJV>()), _jVJ(linear_set<InfoJ*>()), _jugadores(string_map<InfoJ>()), _nombres(linear_set<Jugador>()), _fantasmasVivos(list<PosYDir>()), _fantasmasVivos_Id(linear_set<unsigned int>()), _fantasmas(vector<Estrategia>()), _habitacion(h), _mapa(vector<vector<bool>>(h.tam(), vector<bool>(h.tam(), false))) {
    // Generamos la estrategia del fantasma y completamos la información relevante a los fantasmas
    _fantasmas.push_back(Estrategia(h, f_init, acciones_fantasma));
    _fantasmasVivos.push_back(f_init);
    _fantasmasVivos_Id.fast_insert(0);

    // Usamos la función localizar_jugadores y completamos la información relevante a nuestros jugadores en el juego
    map<Jugador, PosYDir> ubicacionesIniciales = ctx->localizar_jugadores(jugadores, list<Fantasma>(1, _fantasmas[0]), h);

    for(pair<Jugador, PosYDir> p : ubicacionesIniciales) {
        _nombres.insert(p.first);
        linear_set<InfoJV>::iterator jVIt = _jugadoresVivos.fast_insert(InfoJV(p.first, p.second.pos, p.second.dir));
        _jugadores[p.first] = InfoJ(&jVIt, Historial(p.second.pos, p.second.dir));
        _jVJ.fast_insert(&_jugadores[p.first]);
    }
}

void ExtremeExorcism::pasar() {
    // TODO!!
    _ticks++;
    actuarFantasmas();
}

void ExtremeExorcism::ejecutarAccion(Jugador j, Accion a) {
    // TODO!!!
    _ticks++;
    actuarJugador(j,a);
}

list<pair<Jugador, PosYDir>> ExtremeExorcism::posicionJugadores() const {
    list<pair<Jugador, PosYDir>> jugadoresVivos;
    for(InfoJV jv : _jugadoresVivos) {
        jugadoresVivos.push_back(jv);
    }
    return jugadoresVivos;
}

list<PosYDir> ExtremeExorcism::posicionFantasmas() const {
    return _fantasmasVivos;
}

PosYDir ExtremeExorcism::posicionEspecial() const {
    // TODO!!!
    return PosYDir(make_pair(0,0), ARRIBA);
}


//Consigue la posisicion y direccion de los fantasmas que disparan en el tick acutal

list<PosYDir> ExtremeExorcism::disparosFantasmas() const {
    list<PosYDir> res;

        for (auto it = _fantasmasVivos_Id.begin(); it!= _fantasmasVivos_Id.end(); ++it) {
            int fantasma = *it;
            Evento evento = _fantasmas[fantasma][_ticks];
            if (evento.dispara){
                tuple<PosYDir()> temp = evento.pos_y_dir();
                res.push_back(temp);
            }
    }
    return res;
}


//Realiza lo mismo que la funcion anterior, pero devuelve por referencia

list<PosYDir> &ExtremeExorcism:: disparosFantasmasAux(list<PosYDir>& res) const{
    for (auto it = _fantasmasVivos_Id.begin(); it!= _fantasmasVivos_Id.end(); ++it) {
        int fantasma = *it;
        Evento evento = _fantasmas[fantasma][_ticks];
        if (evento.dispara){
            PosYDir temp = evento.pos_y_dir();
            res.push_back(temp);

        }

    }
    return res;
}

//cambiar linear_set a set
// Problemas con el const de la funcion.
//
 /*
set<Pos> ExtremeExorcism::posicionesDisparadas()  const {
    linear_set<Pos> posicionesAlcanzadas;
    linear_set<Pos> res = posicionesDisparadasAux(posicionesAlcanzadas);
    return res;
}
*/
linear_set<Pos> &ExtremeExorcism::posicionesDisparadasAux(linear_set<Pos> &posicionesAlcanzadas) {
    list<PosYDir> hola; //lista vacia
    list<PosYDir> &posYDirInicial = disparosFantasmasAux(hola);


    for (auto it = posYDirInicial.begin(); it != posYDirInicial.end(); ++it){
        PosYDir posydir = *it;
        Pos casillero = posydir.pos;
        Dir direccion = posydir.dir;
        while (_habitacion.disponible((_habitacion.actualizarD(direccion,posydir)).pos)) {
            casillero = (_habitacion.actualizarD(direccion,posydir).pos);
            if (not (_mapa[get<0>(casillero)][get<1>(casillero)])) {
                _mapa[get<0>(casillero)][get<1>(casillero)] = true;
                posicionesAlcanzadas.fast_insert(casillero);
            }

        }

    }
    for (auto itt = posicionesAlcanzadas.begin() ;  itt != posicionesAlcanzadas.end(); ++itt){
        Pos casillero = *itt;
        _mapa[get<0>(casillero)][get<1>(casillero)] = false;
    }


return posicionesAlcanzadas;

}

bool ExtremeExorcism::jugadorVivo(Jugador j) const {
    return _jugadores.at(j).aInfoJV != NULL;
}

const Habitacion& ExtremeExorcism::habitacion() const {
    return _habitacion;
}

PosYDir ExtremeExorcism::posicionJugador(Jugador j) const {
    return _jugadores.at(j).historial.back().posYDir();
}

const set<Jugador>& ExtremeExorcism::jugadores() const {
    return (set<Jugador>) _nombres;
}

const list<Fantasma>& ExtremeExorcism::fantasmas() const {
    list<Fantasma> fantasmas;
    for(Estrategia e : _fantasmas) {
        fantasmas.push_back((Fantasma) e);
    }
    return fantasmas;
}

/*================================= FUNCIONES AUXILIARES ====================================*/
// Para nuestros tipos auxiliares:
ExtremeExorcism::EventoJugador::EventoJugador(unsigned int t, Pos p, Dir d, bool s) : tick(t), pos(p), dir(d), dispara(s) {}
PosYDir ExtremeExorcism::EventoJugador::posYDir() const {
    return PosYDir(pos, dir);
}

ExtremeExorcism::InfoJ::InfoJ() : aInfoJV(nullptr), historial(Historial()) {}
ExtremeExorcism::InfoJ::InfoJ(linear_set<InfoJV>::iterator* p, Historial h) : aInfoJV(p), historial(h) {}

ExtremeExorcism::InfoJV::InfoJV(Jugador j, Pos p, Dir d) : nombre(j), pos(p), dir(d) {}
PosYDir ExtremeExorcism::InfoJV::posYDir() const {
    return PosYDir(pos, dir);
}
ExtremeExorcism::InfoJV::operator pair<Jugador, PosYDir>() const {
    return make_pair(nombre, posYDir());
}

//Constructor por defecto
ExtremeExorcism::Historial::Historial() : historial(list<EventoJugador>()) {}
//Constructor custom. Dado un tick, posición y dirección actualiza el historial
ExtremeExorcism::Historial::Historial(Pos pos, Dir dir){
    historial.push_back(EventoJugador(0, pos, dir, false));
}
ExtremeExorcism::EventoJugador ExtremeExorcism::Historial::actualizar(Habitacion& h, Accion a, unsigned int tick,
                                                                      ExtremeExorcism::EventoJugador e) {
    EventoJugador nuevoEvento = e;

    if(a == DISPARAR){
        nuevoEvento.tick = tick;
        nuevoEvento.dispara = true;
    }else if(a == ESPERAR){
        nuevoEvento.tick = tick;
        nuevoEvento.dispara = false;
    }else{
        PosYDir nueva = h.actualizar(a, e.posYDir());
        nuevoEvento.pos = nueva.pos;
        nuevoEvento.dir = nueva.dir;
        nuevoEvento.tick = tick;
        nuevoEvento.dispara = false;
    }
    return nuevoEvento;
}
void ExtremeExorcism::Historial::actuar(Habitacion& h, Accion a, unsigned int tick) {
    EventoJugador nuevoEvento = actualizar(h, a, tick, historial.back());
    historial.push_back(nuevoEvento);
}
void ExtremeExorcism::Historial::morir(unsigned int tick) {
    historial.push_back(EventoJugador(tick, Pos(-1, -1), historial.back().dir, false));
}
ExtremeExorcism::EventoJugador ExtremeExorcism::Historial::back() const {
    return historial.back();
}


ExtremeExorcism::Estrategia::Estrategia(vector<Evento> e) : _estrategia(e) {}
ExtremeExorcism::Estrategia::Estrategia(Habitacion& h, PosYDir init, list<Accion> la) : _estrategia(vector<Evento>(1, Evento(init.pos, init.dir, false))) {
    for (list<Accion>::iterator it = la.begin(); it != la.end(); ++it) {
        _estrategia.push_back(actuar(h, *it, _estrategia[_estrategia.size() - 1]));
    }
}
ExtremeExorcism::Estrategia::operator Fantasma() const {
    return list<Evento>(_estrategia.begin(), _estrategia.end());
}
Evento ExtremeExorcism::Estrategia::operator [](unsigned int t) const {
    bool esEnLaIda = (t / (_estrategia.size() + 5)) % 2;
    t = t % (_estrategia.size() + 5);
    if(t < _estrategia.size()) {
        if(esEnLaIda) {
            return _estrategia[t];
        } else {
            return Evento(_estrategia[_estrategia.size() - 1 - t].pos, invertir(_estrategia[_estrategia.size() - 1 - t].dir), _estrategia[_estrategia.size() - 1 - t].dispara);
        }
    } else {
        if(esEnLaIda) {
            return Evento(_estrategia[_estrategia.size()-1].pos, _estrategia[_estrategia.size()-1].dir, false);
        } else {
            return Evento(_estrategia[0].pos, invertir(_estrategia[0].dir), false);
        }
    }
}
Evento ExtremeExorcism::Estrategia::actuar(Habitacion &h, Accion a, Evento e) const {
    PosYDir ubicacionNueva = h.actualizar(a, e.pos_y_dir());
    return Evento(ubicacionNueva.pos, ubicacionNueva.dir, a == DISPARAR);
}
Dir ExtremeExorcism::Estrategia::invertir(Dir d) const {
    switch (d) {
    case ARRIBA:
        d = ABAJO;
        break;
    case IZQUIERDA:
        d = DERECHA;
        break;
    case ABAJO:
        d = ARRIBA;
        break;
    case DERECHA:
        d = IZQUIERDA;
        break;
    }
    return d;
}



void ExtremeExorcism::actuarFantasmas() {
    _fantasmasVivos.clear();


    /* COPIAR CODIGO DE POSICIONES ALCANZADAS ACA!!!
     *
     *  ACA SE PINTA EL MAPA
     *
     *   // TODO!!

     *
     *
     * */

    for (auto it = _jVJ.begin(); it != _jVJ.end();) {
        InfoJ* estadoAcutal = *(it);
        linear_set<InfoJV>::iterator nodo_en_vivos = *(estadoAcutal->aInfoJV);
        Pos p = estadoAcutal->historial.back().pos;

        if (_mapa[p.first][p.second]) {
            // COMPLETAR ESTA FUNCION QUE AGREGA LA MUERTE AL HISTORIAL !!
            // TODO!!
            estadoAcutal->historial.morir(_ticks);
            // COMPLETAR ESTA FUNCION QUE ELIMINA EL NODO AL QUE APUNTA Y LO AVANZA!!!!!!
            // TODO!!
            nodo_en_vivos.erase();
            estadoAcutal->aInfoJV = nullptr;
            it = it.erase();

        } else {
            ++it;

        }

    }

    /* COPIAR CODIGO DE POSICIONES ALCANZADAS ACA!!!
    *
    *  ACA SE DESPINTA EL MAPA
     *
     *  // TODO!!
    *
    */




}

void ExtremeExorcism::actuarJugador(Jugador& j, Accion& a) {
    // Actualizamos info del jugador que actuó.
    InfoJ* estadoActual = &_jugadores[j];
    estadoActual->historial.actuar(_habitacion, a, _ticks);



    Pos pos = estadoActual->historial.back().pos;
    Dir dir = estadoActual->historial.back().dir;
    bool disparo = estadoActual->historial.back().dir;

    //Guardamos las posiciones alcanzadas por disparos para poder saber qué fantasmas murieron.
    linear_set<Pos> alcanzadas;


    while (false /*funcion que hay que usar???*/)
    {

    }

    //Eliminamos a los fantasmas que fueron alcanzados por disparos del conjunto de ID’S.
    //Si el fantasma especial fue alcanzado, cambio un flag cambioDeRonda a true.

    bool cambioRonda = false;


    for (auto it = _fantasmasVivos_Id.begin(); it != _fantasmasVivos_Id.end();) {
        int id = *(it);
        Pos p_fantasma = _fantasmas[id][_ticks-1].pos;


        if (_mapa[p_fantasma.first][p_fantasma.second]) {
            it = it.erase();
            cambioRonda = cambioRonda or id == (_fantasmas.size()-1);

        } else {
            ++it;

        }

    }


}

void ExtremeExorcism::resetearFantasmas() {

}

void ExtremeExorcism::resetearJugadores() {}

bool ExtremeExorcism::cambioRonda() {
    return  _fantasmasVivos_Id.count(_fantasmas.size()-1);

}