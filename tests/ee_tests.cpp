#include "estado/EstadoJuego.h"
#include "gtest-1.8.1/gtest.h"
#include "modelo/Contexto.h"
#include "modelo/ExtremeExorcism.h"
#include "modelo/localizadores/localizador_aleatorio.h"
#include "modelo/localizadores/localizador_estatico.h"

bool en_rango(Pos p, unsigned int tam) {
  return p.first >= 0 and p.second >= 0 and p.first < tam and p.second < tam;
}

TEST(EE, ContextoAleatorio) {
  Localizador *l = new LocalizadorAleatorio();
  Contexto ctx = Contexto(l);
  set<Jugador> jugadores = set<Jugador>{"P1", "P2", "P3"};
  Habitacion h(3, {Pos(0, 0), Pos(0, 1), Pos(0, 2), Pos(1, 1)});

  for (int i = 0; i < 100; i++) {
    map<Jugador, PosYDir> res =
        ctx.localizar_jugadores(jugadores, list<Fantasma>(), h);

    for (const Jugador &j : jugadores) { // Están todos los jugadores
      EXPECT_TRUE(res.count(j));
    }
    set<Pos> pos;
    for (const pair<Jugador, PosYDir> &j_pd : res) { // Son posiciones válidas
      EXPECT_TRUE(en_rango(j_pd.second.pos, 3));
      EXPECT_TRUE(not h.ocupado(j_pd.second.pos));
      pos.insert(j_pd.second.pos);
    }
    EXPECT_EQ(pos.size(), jugadores.size()); // Las posiciones no se repiten
  }
  delete l;
}

TEST(EE, ContextoEstatico_1J_1R) {
  Localizador *l = new LocalizadorEstatico({{"P1", PosYDir({0, 0}, ARRIBA)}});
  Contexto ctx = Contexto(l);
  set<Jugador> j1 = set<Jugador>{"P1"};
  Habitacion h(3, {});

  map<Jugador, PosYDir> res = ctx.localizar_jugadores(j1, list<Fantasma>(), h);

  EXPECT_TRUE(res.count("P1"));
  EXPECT_FALSE(res.count("P2"));
  EXPECT_EQ(res.at("P1"), PosYDir({0, 0}, ARRIBA));

  delete l;
}

TEST(EE, ContextoEstatico_2J_2R) {
  Localizador *l = new LocalizadorEstatico(
      {{0, {{"P1", PosYDir({0, 0}, ARRIBA)}, {"P2", PosYDir({1, 1}, ABAJO)}}},
       {1,
        {{"P2", PosYDir({0, 0}, ARRIBA)}, {"P1", PosYDir({1, 1}, DERECHA)}}}});
  Contexto ctx = Contexto(l);
  set<Jugador> j2 = set<Jugador>{"P1", "P2"};
  Habitacion h(3, {});

  map<Jugador, PosYDir> res = ctx.localizar_jugadores(j2, list<Fantasma>(), h);

  EXPECT_TRUE(res.count("P1"));
  EXPECT_TRUE(res.count("P2"));
  EXPECT_EQ(res.at("P1"), PosYDir({0, 0}, ARRIBA));
  EXPECT_EQ(res.at("P2"), PosYDir({1, 1}, ABAJO));

  list<Fantasma> r2;
  r2.push_back({});
  res = ctx.localizar_jugadores(j2, r2, h);

  EXPECT_TRUE(res.count("P1"));
  EXPECT_TRUE(res.count("P2"));
  EXPECT_EQ(res.at("P1"), PosYDir({1, 1}, DERECHA));
  EXPECT_EQ(res.at("P2"), PosYDir({0, 0}, ARRIBA));

  list<Fantasma> r3;
  r3.push_back({});
  r3.push_back({});

  EXPECT_TRUE(res.count("P1"));
  EXPECT_TRUE(res.count("P2"));
  EXPECT_EQ(res.at("P1"), PosYDir({1, 1}, DERECHA));
  EXPECT_EQ(res.at("P2"), PosYDir({0, 0}, ARRIBA));

  delete l;
}

TEST(EE, InitRonda_J1) {
  Localizador *l = new LocalizadorEstatico({{"P1", {PosYDir({0, 0}, ARRIBA)}}});
  Habitacion h(3, {{1, 1}});
  Contexto ctx(l);
  set<Jugador> js{"P1"};
  Fantasma f{
    Evento({2, 2}, ARRIBA, false),
    Evento({2, 2}, ARRIBA, false),
    Evento({2, 2}, ARRIBA, false),
    Evento({2, 2}, ARRIBA, false),
    Evento({2, 2}, ARRIBA, false),
    Evento({2, 2}, ARRIBA, false),
    Evento({2, 2}, ABAJO, false),
    Evento({2, 2}, ABAJO, false),
    Evento({2, 2}, ABAJO, false),
    Evento({2, 2}, ABAJO, false),
    Evento({2, 2}, ABAJO, false),
    Evento({2, 2}, ABAJO, false),
  };

  ExtremeExorcism ee(h, js, PosYDir({2, 2}, ARRIBA), {}, &ctx);

  EXPECT_EQ(ee.jugadores(), js);
  EXPECT_EQ(ee.fantasmas(), list<Fantasma>{f});

  // Jugador
  EXPECT_TRUE(ee.jugadorVivo("P1"));
  EXPECT_EQ(ee.habitacion(), h);
  EXPECT_EQ(ee.posicionJugador("P1"), PosYDir({0, 0}, ARRIBA));
  auto pos_j = ee.posicionJugadores();
  EXPECT_EQ(pos_j.size(), 1);
  EXPECT_EQ(pos_j.back().first, "P1");
  EXPECT_EQ(pos_j.back().second, PosYDir({0, 0}, ARRIBA));

  // Fantasma
  auto pos_f = ee.posicionFantasmas();
  EXPECT_EQ(pos_f.size(), 1);
  EXPECT_EQ(pos_f.back(), PosYDir({2, 2}, ARRIBA));
  EXPECT_EQ(ee.posicionEspecial(), PosYDir({2, 2}, ARRIBA));

  delete l;
}

TEST(EE, Estado_InitRonda_J1) {
  Localizador *l = new LocalizadorEstatico({{"0", {PosYDir({0, 2}, ARRIBA)}}});
  Habitacion h(3, {{1, 1}});
  Contexto ctx(l);
  set<Jugador> js{"0"};
  ExtremeExorcism ee(h, js, PosYDir({2, 0}, ARRIBA), {}, &ctx);

  EstadoJuego ej(1, "W..\n"
                    ".#.\n"
                    "..w\n");

  ej.verificar(ee);

  delete l;
}

TEST(EE, InitRonda_J2) {
  Localizador *l = new LocalizadorEstatico(
      {{"0", {PosYDir({0, 2}, ARRIBA)}}, {"1", {PosYDir({2, 2}, ABAJO)}}});
  Habitacion h(3, {{1, 1}});
  Contexto ctx(l);
  set<Jugador> js{"0", "1"};
  ExtremeExorcism ee(h, js, PosYDir({2, 0}, ARRIBA), {}, &ctx);

  EstadoJuego ej(2, "W.D\n"
                    ".#.\n"
                    "..w\n");

  ej.verificar(ee);

  delete l;
}

TEST(EE, MoverJugador) {
  Localizador *l = new LocalizadorEstatico(
      {{"0", {PosYDir({0, 2}, ARRIBA)}}, {"1", {PosYDir({2, 2}, ABAJO)}}});
  Habitacion h(3, {{1, 1}});
  Contexto ctx(l);
  set<Jugador> js{"0", "1"};

  ExtremeExorcism ee(h, js, PosYDir({2, 0}, ARRIBA), {}, &ctx);
  EstadoJuego(2, "W.D\n"
                 ".#.\n"
                 "..w\n")
      .verificar(ee);

  ee.ejecutarAccion("0", MDERECHA);
  EstadoJuego(2, ".RD\n"
                 ".#.\n"
                 "..w\n")
      .verificar(ee);

  ee.ejecutarAccion("1", MABAJO);
  EstadoJuego(2, ".R.\n"
                 ".#D\n"
                 "..w\n")
      .verificar(ee);

  ee.ejecutarAccion("0", DISPARAR);
  EstadoJuego(2, ".R.\n"
                 ".#D\n"
                 "..w\n")
      .verificar(ee);

  delete l;
}

TEST(EE, MoverJugadorObstaculo) {
  Localizador *l = new LocalizadorEstatico(
      {{"0", {PosYDir({0, 2}, ARRIBA)}}, {"1", {PosYDir({2, 2}, ABAJO)}}});
  Habitacion h(3, {{1, 1}});
  Contexto ctx(l);
  set<Jugador> js{"0", "1"};

  ExtremeExorcism ee(h, js, PosYDir({2, 0}, ARRIBA), {}, &ctx);
  EstadoJuego(2, "W.D\n"
                 ".#.\n"
                 "..w\n")
      .verificar(ee);

  ee.ejecutarAccion("0", MDERECHA);
  EstadoJuego(2, ".RD\n"
                 ".#.\n"
                 "..w\n")
      .verificar(ee);

  ee.ejecutarAccion("0", MABAJO);
  EstadoJuego(2, ".ED\n"
                 ".#.\n"
                 "..w\n")
      .verificar(ee);

  ee.ejecutarAccion("0", MIZQUIERDA);
  EstadoJuego(2, "Q.D\n"
                 ".#.\n"
                 "..w\n")
      .verificar(ee);

  ee.ejecutarAccion("0", MABAJO);
  EstadoJuego(2, "..D\n"
                 "E#.\n"
                 "..w\n")
      .verificar(ee);

  ee.ejecutarAccion("0", MDERECHA);
  EstadoJuego(2, "..D\n"
                 "R#.\n"
                 "..w\n")
      .verificar(ee);

  delete l;
}

TEST(EE, MoverFuera) {
  Localizador *l = new LocalizadorEstatico({
      {"0", {PosYDir({0, 2}, ARRIBA)}},
  });
  Habitacion h(3, {{1, 1}});
  Contexto ctx(l);
  set<Jugador> js{"0"};

  ExtremeExorcism ee(h, js, PosYDir({2, 0}, ARRIBA), {}, &ctx);
  EstadoJuego(1, "W..\n"
                 ".#.\n"
                 "..w\n")
      .verificar(ee);

  ee.ejecutarAccion("0", MIZQUIERDA);
  EstadoJuego(1, "Q..\n"
                 ".#.\n"
                 "..w\n")
      .verificar(ee);

  ee.ejecutarAccion("0", MARRIBA);
  EstadoJuego(1, "W..\n"
                 ".#.\n"
                 "..w\n")
      .verificar(ee);

  delete l;
}

// Movimientos fantasma sin repetición
TEST(EE, Pasar) {
  Localizador *l = new LocalizadorEstatico({
      {"0", {PosYDir({0, 0}, ARRIBA)}},
  });
  Habitacion h(3, {{1, 1}});
  Contexto ctx(l);
  set<Jugador> js{"0"};
  Fantasma f{
      Evento({2, 0}, ARRIBA, false),   Evento({2, 1}, ARRIBA, false),
      Evento({2, 2}, ARRIBA, false),   Evento({1, 2}, IZQUIERDA, false),
      Evento({1, 2}, IZQUIERDA, true),
  };

  ExtremeExorcism ee(h, js, PosYDir({2, 0}, ARRIBA),
                     {MARRIBA, MARRIBA, MIZQUIERDA, DISPARAR}, &ctx);

  EstadoJuego(1, "...\n"
                 ".#.\n"
                 "W.w\n")
      .verificar(ee);

  ee.pasar();
  EstadoJuego(1, "...\n"
                 ".#w\n"
                 "W..\n")
      .verificar(ee);

  ee.pasar();
  EstadoJuego(1, "..w\n"
                 ".#.\n"
                 "W..\n")
      .verificar(ee);

  ee.pasar();
  EstadoJuego(1, ".q.\n"
                 ".#.\n"
                 "W..\n")
      .verificar(ee);

  ee.pasar();
  EstadoJuego(1, ".q.\n"
                 ".#.\n"
                 "W..\n")
      .verificar(ee);
}

// Movimientos fantasma con repetición
TEST(EE, PasarConRepeticion) {
  Localizador *l = new LocalizadorEstatico({
      {"0", {PosYDir({0, 0}, ARRIBA)}},
  });
  Habitacion h(3, {{1, 1}});
  Contexto ctx(l);
  set<Jugador> js{"0"};

  ExtremeExorcism ee(h, js, PosYDir({2, 0}, ARRIBA),
                     {MARRIBA, MARRIBA, DISPARAR}, &ctx);

  EstadoJuego(1, "...\n"
                 ".#.\n"
                 "W.w\n")
      .verificar(ee);

  ee.pasar();
  EstadoJuego(1, "...\n"
                 ".#w\n"
                 "W..\n")
      .verificar(ee);

  ee.pasar();
  EstadoJuego(1, "..w\n"
                 ".#.\n"
                 "W..\n")
      .verificar(ee);

  ee.pasar();
  EstadoJuego(1, "..w\n"
                 ".#.\n"
                 "W..\n")
      .verificar(ee);

  for (int i = 0; i < 5; i++) {
    ee.pasar();
    EstadoJuego(1, "..w\n"
                   ".#.\n"
                   "W..\n")
        .verificar(ee);
  }

  ee.pasar();
  EstadoJuego(1, "..e\n"
                 ".#.\n"
                 "W..\n").verificar(ee);
 
  ee.pasar();
  EstadoJuego(1, "..e\n"
                 ".#.\n"
                 "W..\n").verificar(ee);
  
  ee.pasar();
  EstadoJuego(1, "...\n"
                 ".#e\n"
                 "W..\n").verificar(ee);
  
  ee.pasar();
  EstadoJuego(1, "...\n"
                 ".#.\n"
                 "W.e\n").verificar(ee);

  for (int i = 0; i < 5; i++) {
    ee.pasar();
    EstadoJuego(1, "...\n"
                   ".#.\n"
                   "W.e\n")
        .verificar(ee);
  }
 
  ee.pasar();
  EstadoJuego(1, "...\n"
                 ".#.\n"
                 "W.w\n")
      .verificar(ee);
  
  ee.pasar();
  EstadoJuego(1, "...\n"
                 ".#w\n"
                 "W..\n")
      .verificar(ee);
}


// Fantasma mata jugador
TEST(EE, FantasmaMataJugador) {
  Localizador *l = new LocalizadorEstatico({
      {"0", {PosYDir({0, 0}, ARRIBA)}},
  });
  Habitacion h(3, {{1, 1}});
  Contexto ctx(l);
  set<Jugador> js{"0"};

  ExtremeExorcism ee(h, js, PosYDir({2, 0}, ARRIBA),
                     {ESPERAR, MIZQUIERDA, DISPARAR}, &ctx);
  EstadoJuego(1, "...\n"
                 ".#.\n"
                 "W.w\n")
      .verificar(ee);
  
  ee.pasar();
  EstadoJuego(1, "...\n"
                 ".#.\n"
                 "W.w\n")
      .verificar(ee);
  
  ee.pasar();
  EstadoJuego(1, "...\n"
                 ".#.\n"
                 "Wq.\n")
      .verificar(ee);

  ee.pasar();
  EstadoJuego(1, "...\n"
                 ".#.\n"
                 ".q.\n")
      .verificar(ee);
}

// Fantasma mata varios jugadores
// TODO:

// Disparo fantasma bloqueado por mapa
// TODO:

// Disparo jugador bloqueado por mapa
// TODO:

// Cambio de ronda, fantasma nuevo, 1 jugador
// TODO:

// Movimientos fantasma nuevo con repetición
// TODO:

// Cambio de ronda, fantasma nuevo, 2 jugadores
// TODO:

// Disparo jugador mata dos fantasmas
// TODO:

// Ningun fantasma dispara
// TODO:

// Un fantasma dispara
// TODO:

// Dos fantasmas disparan
// TODO:
