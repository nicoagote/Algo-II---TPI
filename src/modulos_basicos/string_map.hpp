template <typename T>
string_map<T>::Nodo::Nodo() {
    definicion = nullptr;
    vector<Nodo*> n;
    for (int i = 0; i < 255; ++i) {
        n.push_back(nullptr);
    }
    siguientes = n;
}


template <typename T>
int string_map<T>::Nodo::hijos() {
    int hijos = 0;
    for (int i = 0; i < siguientes.size(); ++i) {
        if (siguientes[i] != nullptr) {hijos++;}
    }
    return hijos;
}

template <typename T>
bool string_map<T>::Nodo::tieneUnHijo() {
    return hijos() == 1;
}


template <typename T>
bool string_map<T>::Nodo::noTieneHijos() {
    return hijos() == 0;
}


template <typename T>
string_map<T>::Nodo::Nodo(string_map<T>::Nodo* original) {
    definicion = nullptr;
    vector<Nodo*> n;
    for (int i = 0; i < 255; ++i) {
        n.push_back(nullptr);
    }
    siguientes = n;

    if (original != nullptr) {
        if (original->definicion != nullptr) {
            definicion = new T(*original->definicion);
        }

        for (int j = 0; j < original->siguientes.size(); ++j) {
            if ( original->siguientes[j] != nullptr) {
                siguientes[j] = new Nodo(original->siguientes[j]);
            }
        }
    }
}



template <typename T>
void string_map<T>::borrarTrie(string_map<T>::Nodo *n) {
    if (n != nullptr) {
        for (int i = 0; i < 255; ++i) {
            borrarTrie(n->siguientes[i]);
        }

        if (n->definicion != nullptr) {
            delete n->definicion;
        }

        delete n;
    }
}




template <typename T>
string_map<T>::string_map(){
    // COMPLETAR
    _size = 0;
    raiz = new Nodo();
}

template <typename T>
string_map<T>::string_map(const string_map<T>& aCopiar) : string_map() { *this = aCopiar; } // Provisto por la catedra: utiliza el operador asignacion para realizar la copia.

template <typename T>
string_map<T>& string_map<T>::operator=(const string_map<T>& d) {
    // COMPLETAR
    borrarTrie(raiz);
    raiz = new Nodo(d.raiz);
    _size = d.size();
}

template <typename T>
string_map<T>::~string_map(){
    borrarTrie(raiz);

}

template <typename T>
T& string_map<T>::operator[](const string& clave){
    Nodo* actual = raiz;
    for (int i = 0; i < clave.length(); ++i) {
        int letra = int(clave[i]);
        /*si no hay mas nodos, lo creamos*/
        if (actual->siguientes[letra] == nullptr ) {
            actual->siguientes[letra] = new Nodo();
        }
        actual = actual->siguientes[letra];
    }

    /*si no esta definido lo definimos*/
    if (actual->definicion == nullptr) {
        actual->definicion = new T();
        _size++;
    }

    return (*actual->definicion);
}


template <typename T>
int string_map<T>::count(const string& clave) const{
    // COMPLETAR
    Nodo* actual = raiz;
    int i = 0;
     while (actual != nullptr and i < clave.length()) {
         int letra = int(clave[i]);
         actual = actual->siguientes[letra];
         i++;
     }

     int res = 0;
     if (actual != nullptr and actual->definicion != nullptr) {
         res = 1;
     }
    return res;
}

template <typename T>
const T& string_map<T>::at(const string& clave) const {
    // COMPLETAR
    Nodo* actual = raiz;
    for (int i = 0; i < clave.length(); ++i) {
        int letra = int(clave[i]);
        actual = actual->siguientes[letra];
    }

    return *actual->definicion;

}

template <typename T>
T& string_map<T>::at(const string& clave) {
    // COMPLETAR
    Nodo* actual = raiz;
    for (int i = 0; i < clave.length(); ++i) {
        int letra = int(clave[i]);
        actual = actual->siguientes[letra];
    }

    return *actual->definicion;
}

template <typename T>
void string_map<T>::erase(const string& clave) {
    // COMPLETAR
    Nodo* actual = raiz;
    Nodo** inicioDeBorrado = &raiz;
    for (int i = 0; i < clave.length(); ++i) {
        int letra = int(clave[i]);
        if ( (actual->hijos()>1) or actual->definicion != nullptr){
            inicioDeBorrado = &actual->siguientes[letra];
        }
        actual = actual->siguientes[letra];

    }

    /*si continua a partir del que se borra*/
    if (not actual->noTieneHijos()) {
        delete actual->definicion;
        actual->definicion = nullptr;
    } else { /*se borra la cadena desde inicioDeBorrado*/
        actual = *inicioDeBorrado;
        *inicioDeBorrado = nullptr;
        borrarTrie(actual);
    }

    _size--;
}

template <typename T>
int string_map<T>::size() const{
    // COMPLETAR
    return _size;
}

template <typename T>
bool string_map<T>::empty() const{
    // COMPLETAR

    return raiz->noTieneHijos();
}