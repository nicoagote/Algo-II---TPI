#ifndef STRING_MAP_H_
#define STRING_MAP_H_

#include <string>
#include <vector>

using namespace std;

template<typename T>
class string_map {
public:
    /**
    CONSTRUCTOR
    * Construye un diccionario vacio.
     * O(1)
    **/
    string_map();

    /**
    CONSTRUCTOR POR COPIA
    * Construye un diccionario por copia.
    * O(*this=aCopiar)
    **/
    string_map(const string_map<T>& aCopiar);

    /**
    OPERADOR ASIGNACION
    * O(borrarTrie(this->raiz) + Nodo(d.raiz))
     */
    string_map& operator=(const string_map& d);

    /**
    DESTRUCTOR
    * O(borrarTrie(this->raiz)
    **/
    ~string_map();

    /**
    operator[]
    * Acceso o definición de pares clave/valor
    * O(|key|)
    **/

    T &operator[](const string &key);

    /**
    COUNT
    * Devuelve la cantidad de apariciones de la clave (0 o 1).
    * Sirve para identificar si una clave está definida o no.
    * O(|l|)
    **/

    int count(const string &key) const;

    /**
    AT
    * Dada una clave, devuelve su significado.
    * PRE: La clave está definida.
    --PRODUCE ALIASING--
    -- Versión modificable y no modificable
    * O(|l|)
    **/
    const T& at(const string& key) const;
    T& at(const string& key);

    /**
    ERASE
    * Dada una clave, la borra del diccionario junto a su significado.
    * PRE: La clave está definida.
    --PRODUCE ALIASING--
     O(|key|*delete(T))
    **/
    void erase(const string& key);

    /**
     SIZE
     * Devuelve cantidad de claves definidas
     * O(1)
     **/
    int size() const;

    /**
     EMPTY
     * devuelve true si no hay ningún elemento en el diccionario
     * O(1)
     **/
    bool empty() const;

private:


    struct Nodo {
        /* O(1) */
        Nodo();
        /* Recursivo. T(n)=O( 256T(n-1) + copy(T) ) donde n es la cantidad de nodos en el arbol si n>1 */
        Nodo(Nodo*);
        /* O(1) */
        int hijos();
        /* O(1) */
        bool tieneUnHijo();
        /* O(1) */
        bool noTieneHijos();

        vector<Nodo*> siguientes;
        T* definicion;
    };


    Nodo* raiz;
    int _size;

    /* Recursivo. T(n)=O( 256T(n-1) + delete(T) ) donde n es la cantidad de nodos en el arbol si n>1 */
    void borrarTrie(Nodo* n);



};

#include "string_map.hpp"

#endif // STRING_MAP_H_
