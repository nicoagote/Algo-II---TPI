#include "ExtremeExorcism.h"



ExtremeExorcism::ExtremeExorcism(Habitacion h, set<Jugador> jugadores, PosYDir f_init, list<Accion> acciones_fantasma, Contexto *ctx) :


_ticks(0), _jugadoresVivos(linear_set<InfoJV>()), _jVJ(linear_set<InfoJ*>()), _jugadores(string_map<InfoJ>()),
_nombres(linear_set<Jugador>()), _fantasmasVivos(list<PosYDir>()), _fantasmasVivos_Id(linear_set<unsigned int>()),
_fantasmas(vector<Estrategia>()), _habitacion(h), _mapa(vector<vector<bool>>(h.tam(), vector<bool>(h.tam(), false))),
_ctx(nullptr), _fantasmasAux(list<Fantasma>()), _jugadoresAux(jugadores) {


    // Generamos la estrategia del fantasma y completamos la información relevante a los fantasmas
    _fantasmas.push_back(Estrategia(h, f_init, acciones_fantasma));
    _fantasmasVivos.push_back(f_init);
    _fantasmasVivos_Id.fast_insert(0);

    _fantasmasAux.push_back((Fantasma) _fantasmas[0]);


    // Usamos la función localizar_jugadores y completamos la información relevante a nuestros jugadores en el juego
    map<Jugador, PosYDir> ubicacionesIniciales = ctx->localizar_jugadores(jugadores, list<Fantasma>(1, _fantasmas[0]), h);


    for(pair<Jugador, PosYDir> p : ubicacionesIniciales) {
        _nombres.insert(p.first);
        linear_set<InfoJV>::iterator jVIt = _jugadoresVivos.fast_insert(InfoJV(p.first, p.second.pos, p.second.dir));
        _jugadores[p.first] = InfoJ(new linear_set<InfoJV>::iterator(jVIt), Historial(p.second.pos, p.second.dir));
        _jVJ.fast_insert(&_jugadores[p.first]);
    }

    //Guardamos el contexto
    _ctx = ctx;
}


//Todos los jugadores pasan y actuan los fantasmas
void ExtremeExorcism::pasar() {
    _ticks++;
    actuarFantasmas();

    _fantasmasAux.clear();
    for (int i = 0; i < _fantasmas.size(); ++i) {
        _fantasmasAux.push_back((Fantasma) _fantasmas[i]);
    }
}

//ejecuta las acciones del jugador que no sea pasar y las de los fantasmas luego.
void ExtremeExorcism::ejecutarAccion(Jugador j, Accion a) {
    _ticks++;
    actuarJugador(j,a);
    if (not cambioRonda()) {
        resetearFantasmas(j);
        resetearJugadores();
        _ticks = 0;
    } else {
        actuarFantasmas();
    }
}

//Devuelve un conjunto con todas las posiciones de los jugadores vivos.
list<pair<Jugador, PosYDir>> ExtremeExorcism::posicionJugadores() const {
    list<pair<Jugador, PosYDir>> jugadoresVivos;
    for(InfoJV jv : _jugadoresVivos) {
        jugadoresVivos.push_back(jv);
    }
    return jugadoresVivos;
}

//Devuelve un conjunto con todas las posiciones de los fantasmas vivos
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

set<Pos> ExtremeExorcism::posicionesDisparadas() {
    linear_set<Pos> posicionesAlcanzadas;
    linear_set<Pos> res = posicionesDisparadasAux(posicionesAlcanzadas);
    return res;
}

linear_set<Pos> &ExtremeExorcism::posicionesDisparadasAux(linear_set<Pos> &posicionesAlcanzadas) {
    list<PosYDir> lista; //lista vacia
    list<PosYDir> &posYDirInicial = disparosFantasmasAux(lista);

    // Pinta el mapa de casilleros ocupados
    for (auto it = posYDirInicial.begin(); it != posYDirInicial.end(); ++it){
        PosYDir posydir = *it;
        Pos casillero = posydir.pos;
        Dir direccion = posydir.dir;
        while (_habitacion.actualizarD(direccion,posydir).pos != posydir.pos) {
            posydir = _habitacion.actualizarD(direccion, posydir);
            casillero = posydir.pos;
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
    return _jugadoresAux;
}

const list<Fantasma>& ExtremeExorcism::fantasmas() const {
    return _fantasmasAux;
}

/*================================= FUNCIONES AUXILIARES ====================================*/
// Para nuestros tipos auxiliares:

//Contructor y funciones de InfoJ-------------------------
ExtremeExorcism::InfoJ::InfoJ() : aInfoJV(nullptr), historial(Historial()) {}

ExtremeExorcism::InfoJ::InfoJ(linear_set<InfoJV>::iterator* p, Historial h) : aInfoJV(p), historial(h) {}

ExtremeExorcism::InfoJ::~InfoJ() {
    if(aInfoJV != NULL) {
        delete aInfoJV;
        aInfoJV = NULL;
    }
}

void ExtremeExorcism::InfoJ::operator=(ExtremeExorcism::InfoJ other) {
    if(aInfoJV != NULL) {
        delete aInfoJV;
        aInfoJV = NULL;
    }

    if(other.aInfoJV != NULL) {
        aInfoJV = new linear_set<InfoJV>::iterator(*(other.aInfoJV));
    } else {
        aInfoJV = NULL;
    }
    historial = other.historial;
}

ExtremeExorcism::InfoJV::InfoJV(Jugador j, Pos p, Dir d) : nombre(j), pos(p), dir(d) {}

PosYDir ExtremeExorcism::InfoJV::posYDir() const {
    return PosYDir(pos, dir);
}
ExtremeExorcism::InfoJV::operator pair<Jugador, PosYDir>() const {
    return make_pair(nombre, posYDir());
}

bool ExtremeExorcism::InfoJ::operator==(ExtremeExorcism::InfoJ x) const {
    return x.historial == historial and x.aInfoJV == aInfoJV;
}

bool ExtremeExorcism::InfoJV::operator==(ExtremeExorcism::InfoJV x) const {
    return x.pos==pos and x.dir==dir and x.nombre==nombre;
}


//Constructor y funciones de Historial----------------------------------------------------------

//Constructor por defecto (no debe ser usado por el usuario)
ExtremeExorcism::Historial::Historial() : historial(list<EventoJugador>()) {}


//Constructor. Dadas la posicion inicial y la direccion inicial genera el historial que comienza allí.
ExtremeExorcism::Historial::Historial(Pos pos, Dir dir){
    historial.push_back(EventoJugador(0, pos, dir, false));
}
//Actualiza el historial con el resultado de una acción.
void ExtremeExorcism::Historial::actuar(Habitacion& h, Accion a, unsigned int tick) {
    EventoJugador nuevoEvento = actualizar(h, a, tick, historial.back());
    historial.push_back(nuevoEvento);
}
//Actualiza el historial registrando la muerte del jugador en cuestión.
// ACLARACIóN: en el TPD usamos la posición make_pair(tam(h), tam(h)) como posición designada para registrar que el jugador estaba muerto, pero en la implementación por comodidad preferimos usar la posición (-1, -1) en su lugar.
void ExtremeExorcism::Historial::morir(unsigned int tick) {
    historial.push_back(EventoJugador(tick, Pos(-1, -1), historial.back().dir, false));
}
//Devuelve el último evento del historial
ExtremeExorcism::EventoJugador ExtremeExorcism::Historial::back() const {
    return historial.back();
}
//Genera la estrategia de un fantasma a partir del historial de un jugador.
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

bool ExtremeExorcism::Historial::operator==(ExtremeExorcism::Historial x) const {
    return x.historial == historial;
}

//Funciones de Evento y EventoJugador----------------------------------------------------------

bool ExtremeExorcism::EventoJugador::operator==(ExtremeExorcism::EventoJugador x) const {
    return x.posYDir() == posYDir() and x.dispara ==dispara and x.tick == tick;
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

ExtremeExorcism::EventoJugador::EventoJugador(unsigned int t, Pos p, Dir d, bool s) : tick(t), pos(p), dir(d), dispara(s) {}

PosYDir ExtremeExorcism::EventoJugador::posYDir() const {
    return PosYDir(pos, dir);
}

//Contructor y funciones de Estrategia--------------------------------------------

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

bool ExtremeExorcism::Estrategia::operator==(ExtremeExorcism::Estrategia x) const {
    return x._estrategia == _estrategia;
}

Evento ExtremeExorcism::Estrategia::actuar(Habitacion &h, Accion a, Evento e) const {
    PosYDir ubicacionNueva = h.actualizar(a, e.pos_y_dir());
    return Evento(ubicacionNueva.pos, ubicacionNueva.dir, a == DISPARAR);
}


//Funciones de actuar el juego.-----------------------------------------------------


void ExtremeExorcism::actuarFantasmas() {
     _fantasmasVivos.clear();
     linear_set<Pos> posicionesAlcanzadas;
     //Actualiza fantasmasVivos y llena el mapa con las posiciones alcanzadas por disparos de los fantasmas
    for (auto it = _fantasmasVivos_Id.begin(); it!= _fantasmasVivos_Id.end(); ++it) {
        int fantasma = *it;
        Evento evento = _fantasmas[fantasma][_ticks];
        PosYDir posYDirFantasmas = evento.pos_y_dir();
        Pos casillero = evento.pos;
        Dir direccion = evento.dir;

        _fantasmasVivos.push_back(posYDirFantasmas);
        if (evento.dispara) {

            while (_habitacion.actualizarD(direccion, posYDirFantasmas).pos != posYDirFantasmas.pos) {
                posYDirFantasmas = _habitacion.actualizarD(direccion, posYDirFantasmas);
                casillero = posYDirFantasmas.pos;
                if (not(_mapa[get<0>(casillero)][get<1>(casillero)])) {
                    _mapa[get<0>(casillero)][get<1>(casillero)] = true;
                    posicionesAlcanzadas.fast_insert(casillero);
                }
            }
        }
    }

    //Usando el mapa llenado anteriormente, actualiza los jugadores matando aquellos que se hayan muerto.
    for (auto it = _jVJ.begin(); it != _jVJ.end();) {
        InfoJ* estadoAcutal = *(it);
        linear_set<InfoJV>::iterator nodo_en_vivos = *(estadoAcutal->aInfoJV);
        Pos p = estadoAcutal->historial.back().pos;

        if (_mapa[p.first][p.second]) {
            estadoAcutal->historial.morir(_ticks);
            // ESTA ELIMINACIóN NO SE EJECUTA EN O(1) PERO MARCH NOS LO ADMITIó PORQUE NO ESTáN IMPLEMENTADOS LOS ITERADORES PARA ELIMINAR, MODIFICAR o AGREGAR en O(1)
            _jugadoresVivos.erase(*nodo_en_vivos);
            estadoAcutal->aInfoJV = nullptr;
            auto copiaIt = it;
            ++it;
            // ESTA ELIMINACIóN NO SE EJECUTA EN O(1) PERO MARCH NOS LO ADMITIó PORQUE NO ESTáN IMPLEMENTADOS LOS ITERADORES PARA ELIMINAR, MODIFICAR o AGREGAR en O(1)
            _jVJ.erase(*copiaIt);
            delete estadoAcutal->aInfoJV;
            estadoAcutal->aInfoJV = NULL;
        } else {
            ++it;

        }

    }

    //Para mantener el invariante de representación despintamos el mapa
    for (auto itt = posicionesAlcanzadas.begin(); itt != posicionesAlcanzadas.end(); ++itt) {
        Pos casillero = *itt;
        _mapa[get<0>(casillero)][get<1>(casillero)] = false;
    }
}

void ExtremeExorcism::actuarJugador(Jugador& j, Accion& a) {
    // Actualizamos info del jugador que actuó.
    InfoJ* estadoActual = &_jugadores[j];
    estadoActual->historial.actuar(_habitacion, a, _ticks);

    Pos pos = estadoActual->historial.back().pos;
    Dir dir = estadoActual->historial.back().dir;
    bool disparo = estadoActual->historial.back().dispara;

    // ESTA ELIMINACIóN NO SE EJECUTA EN O(1) PERO MARCH NOS LO ADMITIó PORQUE NO ESTáN IMPLEMENTADOS LOS ITERADORES PARA ELIMINAR, MODIFICAR o AGREGAR en O(1)
    // ESTE COMBO erase -> fastInsert SIMULA UNA MODIFICACIóN QUE HUBIESE SIDO EJECUTADA EN O(1)
    _jugadoresVivos.erase(*(*(estadoActual->aInfoJV)));
     *(estadoActual->aInfoJV) = _jugadoresVivos.fast_insert(InfoJV(j, pos, dir));

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
            // ESTA ELIMINACIóN NO SE EJECUTA EN O(1) PERO MARCH NOS LO ADMITIó PORQUE NO ESTáN IMPLEMENTADOS LOS ITERADORES PARA ELIMINAR, MODIFICAR o AGREGAR en O(1)
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

    //Actualizamos el acmpo _fantasmasAux de nuestra estructura que nos permitirá devolver la lista actualizada de fantasmas en la nueva ronda actual.
    _fantasmasAux.clear();
    for (int i = 0; i < _fantasmas.size(); ++i) {
        _fantasmasAux.push_back((Fantasma) _fantasmas[i]);
    }
}


void ExtremeExorcism::resetearJugadores() {
    // Usamos la función localizar_jugadores y completamos la información relevante a nuestros jugadores en el juego
    map<Jugador, PosYDir> ubicacionesIniciales = _ctx->localizar_jugadores(_nombres, fantasmas(), _habitacion);
    _jugadoresVivos = linear_set<InfoJV>();
    _jVJ = linear_set<InfoJ*>();

    for(pair<Jugador, PosYDir> p : ubicacionesIniciales) {
        linear_set<InfoJV>::iterator jVIt = _jugadoresVivos.fast_insert(InfoJV(p.first, p.second.pos, p.second.dir));

        if(_jugadores[p.first].aInfoJV != NULL) {
            delete _jugadores[p.first].aInfoJV;
            _jugadores[p.first].aInfoJV = NULL;
        }
        _jugadores[p.first] = InfoJ(new linear_set<InfoJV>::iterator(jVIt), Historial(p.second.pos, p.second.dir));

        _jVJ.fast_insert(&_jugadores[p.first]);
    }
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


bool ExtremeExorcism::cambioRonda() {
    return  _fantasmasVivos_Id.count(_fantasmas.size()-1);
}