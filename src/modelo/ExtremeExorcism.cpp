#include "ExtremeExorcism.h"

// TODO!!! Completar

ExtremeExorcism::ExtremeExorcism(Habitacion h, set<Jugador> jugadores, PosYDir f_init, list<Accion> acciones_fantasma, Contexto *ctx) : _ticks(0), _jugadoresVivos(linear_set<InfoJV>()), _jVJ(linear_set<InfoJ*>()), _jugadores(string_map<InfoJ>()), _nombres(linear_set<Jugador>()), _fantasmasVivos(list<PosYDir>()), _fantasmasVivos_Id(linear_set<unsigned int>()), _fantasmas(vector<Estrategia>()), _habitacion(h), _mapa(vector<vector<bool>>(h.tam(), vector<bool>(h.tam(), false))), _ctx(
        nullptr), aux_1(list<Fantasma>()), aux_2(jugadores) {
    // Generamos la estrategia del fantasma y completamos la información relevante a los fantasmas
    _fantasmas.push_back(Estrategia(h, f_init, acciones_fantasma));
    _fantasmasVivos.push_back(f_init);
    _fantasmasVivos_Id.fast_insert(0);

    aux_1.push_back((Fantasma) _fantasmas[0]);


                // Usamos la función localizar_jugadores y completamos la información relevante a nuestros jugadores en el juego
                map<Jugador, PosYDir> ubicacionesIniciales = ctx->localizar_jugadores(jugadores, list<Fantasma>(1, _fantasmas[0]), h);


    for(pair<Jugador, PosYDir> p : ubicacionesIniciales) {
        _nombres.insert(p.first);
        linear_set<InfoJV>::iterator jVIt = _jugadoresVivos.fast_insert(InfoJV(p.first, p.second.pos, p.second.dir));
        _jugadores[p.first] = InfoJ(&jVIt, Historial(p.second.pos, p.second.dir));
        _jVJ.fast_insert(&_jugadores[p.first]);
    }

    //Guardamos el contexto
    _ctx = ctx;
}

void ExtremeExorcism::pasar() {
    _ticks++;
    actuarFantasmas();

    aux_1.clear();
    for (int i = 0; i < _fantasmas.size(); ++i) {
        aux_1.push_back((Fantasma) _fantasmas[i]);
    }
}

void ExtremeExorcism::ejecutarAccion(Jugador j, Accion a) {
    _ticks++;
    actuarJugador(j,a);
    if (cambioRonda()) {
        resetearFantasmas(j);
        resetearJugadores();
        _ticks = 0;
    } else {
        actuarFantasmas();
    }

    aux_1.clear();
    for (int i = 0; i < _fantasmas.size(); ++i) {
        aux_1.push_back((Fantasma) _fantasmas[i]);
    }
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
    // Devuelve la PosYDir del fantasma especial en este momento.
    Estrategia fantasmaEspecial = _fantasmas.back();
    Evento ultimoEvento = fantasmaEspecial[_ticks];
    return PosYDir(ultimoEvento.pos, ultimoEvento.dir);
}


//Consigue la posisicion y direccion de los fantasmas que disparan en el tick acutal

list<PosYDir> ExtremeExorcism::disparosFantasmas() const {
    list<PosYDir> res;

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
    return _jugadores.at(j).aInfoJV != nullptr;
}

const Habitacion& ExtremeExorcism::habitacion() const {
    return _habitacion;
}

PosYDir ExtremeExorcism::posicionJugador(Jugador j) const {
    auto test = _jugadores.at(j).historial.back().posYDir();
    return test;
}

const set<Jugador>& ExtremeExorcism::jugadores() const {
    return aux_2;
}

const list<Fantasma>& ExtremeExorcism::fantasmas() const {
    return aux_1;
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
const vector<Evento> ExtremeExorcism::Historial::armarEstrategia() const {
    list<EventoJugador>::const_iterator it = historial.begin();
    Evento def = Evento(historial.front().pos, historial.front().dir, false);
    vector<Evento> v = vector<Evento>(historial.back().tick + 1, def);

    for(int i = 0; i < v.size(); i++) {
        if(it->tick != i) {
            v[i] = def;
        } else {
            v[i] = Evento(it->pos, it->dir, it->dispara);
            def = Evento(it->pos, it->dir, false);
            ++it;
        }
    }

    return v;
}


ExtremeExorcism::Estrategia::Estrategia(vector<Evento> e) : _estrategia(e) {}
ExtremeExorcism::Estrategia::Estrategia(Habitacion& h, PosYDir init, list<Accion> la) : _estrategia(vector<Evento>(1, Evento(init.pos, init.dir, false))) {
    for (list<Accion>::iterator it = la.begin(); it != la.end(); ++it) {
        _estrategia.push_back(actuar(h, *it, _estrategia[_estrategia.size() - 1]));
    }
}
ExtremeExorcism::Estrategia::Estrategia(Historial h) : _estrategia(h.armarEstrategia()) {}
ExtremeExorcism::Estrategia::operator Fantasma() const {
    return list<Evento>(_estrategia.begin(), _estrategia.end());
}
Evento ExtremeExorcism::Estrategia::operator [](unsigned int t) const {
    bool esEnLaIda = 0 == ((t / (_estrategia.size() + 5)) % 2);
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
     linear_set<Pos> posicionesAlcanzadas;
     //pintar el mapa
    for (auto it = _fantasmasVivos_Id.begin(); it!= _fantasmasVivos_Id.end(); ++it) {
        int fantasma = *it;
        Evento evento = _fantasmas[fantasma][_ticks];
        if (evento.dispara) {
            PosYDir posYDirFantasmas = evento.pos_y_dir();
            Pos casillero = evento.pos;
            Dir direccion = evento.dir;


            while (_habitacion.disponible((_habitacion.actualizarD(direccion, posYDirFantasmas)).pos)) {
                casillero = (_habitacion.actualizarD(direccion, posYDirFantasmas).pos);
                if (not(_mapa[get<0>(casillero)][get<1>(casillero)])) {
                    _mapa[get<0>(casillero)][get<1>(casillero)] = true;
                    posicionesAlcanzadas.fast_insert(casillero);
                }

            }
        }



    for (auto it = _jVJ.begin(); it != _jVJ.end();) {
        InfoJ* estadoAcutal = *(it);
        linear_set<InfoJV>::iterator nodo_en_vivos = *(estadoAcutal->aInfoJV);
        Pos p = estadoAcutal->historial.back().pos;

        if (_mapa[p.first][p.second]) {
            estadoAcutal->historial.morir(_ticks);
            // PREGUNTAR A MARCH
            // TODO!!
            _jugadoresVivos.erase(*nodo_en_vivos);
            estadoAcutal->aInfoJV = nullptr;
            auto copiaIt = it;
            ++it;
            _jVJ.erase(*copiaIt);

        } else {
            ++it;

        }

    }
//Despintar el mapa

        for (auto itt = posicionesAlcanzadas.begin(); itt != posicionesAlcanzadas.end(); ++itt) {
            Pos casillero = *itt;
            _mapa[get<0>(casillero)][get<1>(casillero)] = false;
        }
    }




}

void ExtremeExorcism::actuarJugador(Jugador& j, Accion& a) {
    // Actualizamos info del jugador que actuó.
    InfoJ* estadoActual = &_jugadores[j];
    estadoActual->historial.actuar(_habitacion, a, _ticks);



    Pos pos = estadoActual->historial.back().pos;
    Dir dir = estadoActual->historial.back().dir;
    bool disparo = estadoActual->historial.back().dispara;

//    TODO!!!
//    linear_set<InfoJV>::iterator datos_en_vivos = *(estadoActual->aInfoJV);
//    (*(datos_en_vivos)).pos = pos;
//    (*(datos_en_vivos)).dir = dir;



    //Guardamos las posiciones alcanzadas por disparos para poder saber qué fantasmas murieron.
    linear_set<Pos> alcanzadas;

    PosYDir actual = PosYDir(pos, dir);


    while (disparo and _habitacion.actualizarD(actual.dir, actual).pos != actual.pos)
    {
        actual =  _habitacion.actualizarD(actual.dir, actual);
        alcanzadas.fast_insert(actual.pos);
    }

    //Eliminamos a los fantasmas que fueron alcanzados por disparos del conjunto de ID’S.
    //Si el fantasma especial fue alcanzado, cambio un flag cambioDeRonda a true.

    bool cambioRonda = false;


    for (auto it = _fantasmasVivos_Id.begin(); it != _fantasmasVivos_Id.end();) {
        int id = *(it);
        Pos p_fantasma = _fantasmas[id][_ticks-1].pos;


        if (alcanzadas.count(p_fantasma)) {
            auto copiaIt = it;
            ++it;
            _fantasmasVivos_Id.erase(*copiaIt);
            cambioRonda = cambioRonda or id == (_fantasmas.size()-1);

        } else {
            ++it;
        }

    }


}

void ExtremeExorcism::resetearFantasmas(Jugador j) {
    //Crea la Estrategia para el nuevo fantasma (Rellena)
    Estrategia nuevaEstrategia = Estrategia(_jugadores[j].historial);
    //Agregamos al vector de fantasmas
    _fantasmas.push_back(nuevaEstrategia);

    //Reseteamos el conjunto de fantmasVivosId
    for(int i = 0; i < _fantasmas.size(); i++){
        _fantasmasVivos_Id.insert(i);
    }

    //Reseteamos el conjunto de fantasmasVivos
    _fantasmasVivos.clear();
    for(int i = 0; i < _fantasmas.size(); i++){
        Evento e = _fantasmas[i][0];
        PosYDir pd = e.pos_y_dir();
        _fantasmasVivos.push_back(pd);

    }

}

void ExtremeExorcism::resetearJugadores() {
    // Usamos la función localizar_jugadores y completamos la información relevante a nuestros jugadores en el juego
    map<Jugador, PosYDir> ubicacionesIniciales = _ctx->localizar_jugadores(_nombres, fantasmas(), _habitacion);
    _jugadoresVivos = linear_set<InfoJV>();
    _jVJ = linear_set<InfoJ*>();

    for(pair<Jugador, PosYDir> p : ubicacionesIniciales) {
        linear_set<InfoJV>::iterator jVIt = _jugadoresVivos.fast_insert(InfoJV(p.first, p.second.pos, p.second.dir));

        _jugadores[p.first] = InfoJ(&jVIt, Historial(p.second.pos, p.second.dir));

        _jVJ.fast_insert(&_jugadores[p.first]);
    }
}

bool ExtremeExorcism::cambioRonda() {
    return  _fantasmasVivos_Id.count(_fantasmas.size()-1);
}

bool ExtremeExorcism::InfoJ::operator==(ExtremeExorcism::InfoJ x) const {
    return x.historial == historial and x.aInfoJV == aInfoJV;
}

bool ExtremeExorcism::InfoJV::operator==(ExtremeExorcism::InfoJV x) const {
    return x.pos==pos and x.dir==dir and x.nombre==nombre;
}

bool ExtremeExorcism::EventoJugador::operator==(ExtremeExorcism::EventoJugador x) const {
    return x.posYDir() == posYDir() and x.dispara ==dispara and x.tick == tick;
}

bool ExtremeExorcism::Historial::operator==(ExtremeExorcism::Historial x) const {
    return x.historial == historial;
}

bool ExtremeExorcism::Estrategia::operator==(ExtremeExorcism::Estrategia x) const {
    return x._estrategia == _estrategia;
}
