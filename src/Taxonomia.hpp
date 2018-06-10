#include <sstream>
#include <stack>

// Métodos de Taxonomia (ya implementados por la cátedra):

template<class T>
int Taxonomia<T>::_espiarProximoCaracter(istream& is) const {
	int c = is.peek();
	while (c == ' ' || c == '\t' || c == '\r' || c == '\n') {
		is.get();
		c = is.peek();
	}
	return is.peek();
}

template<class T>
typename Taxonomia<T>::Nodo* Taxonomia<T>::_leerDe(istream& is) {
	Taxonomia<T>::Nodo* nodo = new Taxonomia<T>::Nodo();
	is >> nodo->valor;
	if (_espiarProximoCaracter(is) == '{') {
		is.get();
		while (_espiarProximoCaracter(is) != '}') {
			nodo->hijos.push_back(_leerDe(is));
		}
		is.get();
	}
	return nodo;
}

template<class T>
Taxonomia<T>::Taxonomia(const string& input) {
	istringstream is(input);
	_raiz = _leerDe(is);
}

template<class T>
void Taxonomia<T>::_borrar(Taxonomia<T>::Nodo* nodo) {
	for (int i = 0; i < nodo->hijos.size(); i++) {
		_borrar(nodo->hijos[i]);
	}
	delete nodo;
}

template<class T>
Taxonomia<T>::~Taxonomia() {
	_borrar(_raiz);
}

template<class T>
void Taxonomia<T>::_identar(ostream& os, int tab) const {
	for (int i = 0; i < tab; i++) {
		os << "  ";
	}
}

template<class T>
void Taxonomia<T>::_mostrarEn(
		ostream& os, Taxonomia<T>::Nodo* nodo, int tab) const {
	_identar(os, tab);
	os << nodo->valor;
	if (nodo->hijos.size() == 0) {
		os << "\n";
	} else {
		os << " {\n";
		for (int i = 0; i < nodo->hijos.size(); i++) {
			_mostrarEn(os, nodo->hijos[i], tab + 1);
		}
		_identar(os, tab);
		os << "}\n";
	}
}

template<class T>
void Taxonomia<T>::mostrar(ostream& os) const {
	_mostrarEn(os, _raiz, 0);
}

////////////////////////////////////////

// Métodos para implementar el iterador de Taxonomia<T> (para completar)

// Devuelve un iterador válido al principio de la taxonomía.
template<class T>
typename Taxonomia<T>::iterator Taxonomia<T>::begin() {
	//Recorro en preorder, entonces esto es la raiz
    return iterator(_raiz);
}

// Devuelve un iterador válido al final de la taxonomía.
template<class T>
typename Taxonomia<T>::iterator Taxonomia<T>::end() {
	//Preorder Creo algo que vaya al principio y busco siempre el ultimo hijo
    //Quiza se pueda hacer mejor...
    Taxonomia<T>::iterator it = iterator(_raiz);

    //Tengo que poner el iterador al final de la taxonomia, recorro todo por el ultimo elem del vector
    while(it.nodoActual->hijos.size() > 0){
        //Si hay hijos agarro el último insertado

        //Guardo el padre y la pos en el array de hijos para poder recorrer con --
        it.padres.emplace(it.nodoActual, it.nodoActual->hijos.size() - 1);

        //Cambio el nodo por el siguiente
        //it.nodoActual = it.nodoActual->hijos[it.nodoActual->hijos.size() - 1];
        it.nodoActual = it.nodoActual->hijos.back(); // .back() es el último insertado, deberia dar lo mismo que arriba
    }

    //Ahora it esta posicionado en el final Y tiene los padres correctos cargados
    return it;
}

// Constructor por defecto del iterador.
// (Nota: puede construir un iterador inválido).
template<class T>
Taxonomia<T>::iterator::iterator() : nodoActual(nullptr), padres(){
    //FIJARSE SI ESTO ESTA BIEEEEEN
}

template<class T>
Taxonomia<T>::iterator::iterator(Taxonomia<T>::Nodo *elem) {
    //Seteo el nodo por donde empieza a recorrer
    nodoActual = elem;
}

// Referencia mutable al nombre de la categoría actual.
// Pre: el iterador está posicionado sobre una categoría.
template<class T>
T& Taxonomia<T>::iterator::operator*() const {
    //Devuelvo una referencia al valor del nodo
    return nodoActual->valor;
}

// Cantidad de subcategorías de la categoría actual.
// Pre: el iterador está posicionado sobre una categoría.
template<class T>
int Taxonomia<T>::iterator::cantSubcategorias() const {
    return (int) nodoActual->hijos.size();
}

// Ubica el iterador sobre la i-ésima subcategoría.
// Pre: el iterador está posicionado sobre una categoría
// y además 0 <= i < cantSubcategorias().
template<class T>
void Taxonomia<T>::iterator::subcategoria(int i) {
    //Apilo al nodoActual como padre
    padres.push(std::make_pair(nodoActual, i));

    //Ahora tengo que mover el nodo actual al hijo correcto
    nodoActual = nodoActual->hijos[i];
}

// Devuelve true sii la categoría actual es la raíz. 
// Pre: el iterador está posicionado sobre una categoría.
template<class T>
bool Taxonomia<T>::iterator::esRaiz() const {
    //Si no hay padres guardados, entonces estoy en la raiz
    return padres.size() == 0;
}

// Ubica el iterador sobre la supercategoría de la categoría
// actual.
// Pre: el iterador está posicionado sobre una categoría
// y además !esRaiz()
template<class T>
void Taxonomia<T>::iterator::supercategoria() {
	//La supercategoria en este caso es el primer padre de la pila de padres (o sea el último agregado)
    //Cambiamos el puntero a ese
    nodoActual = padres.top().first;

    //Sacamos ese padre de padres
    padres.pop();
}

// Compara dos iteradores por igualdad.
// Pre: deben ser dos iteradores de la misma taxonomía.
template<class T>
bool Taxonomia<T>::iterator::operator==(
		const Taxonomia<T>::iterator& otro) const {
    //Tengo que ver que los iteradores sean iguales, me fijo las posiciones de memoria
    return nodoActual == otro.nodoActual;
}

// Ubica el iterador sobre la categoría siguiente a la actual
// en el recorrido *preorder* de la taxonomía.
// Si se trata de la última categoría de la taxonomía,
// el iterador resultante debe ser igual al iterador end()
// de la taxonomía.
template<class T>
void Taxonomia<T>::iterator::operator++() {
    //Hay que avanzar!!!!!
    //Para avanzar hago esto:
    //  Me fijo si el nodo actual tiene hijos
    //  Si tiene:
    //      Si no recorri todos los hijos:
    //          Voy al siguiente hijo
    //      Si recorri todos:
    //          Vuelvo al padre
    //  Si no tiene hijos:
    //      Vuelvo al padre
    //
    //NOTA: Si el nodo actual es igual al end, no hay que avanzar!
    //Con algo asi se puede recorrer todo en preorder

    //Empiezo
    //std::cout << "ENTRE EN ++" << std::endl;

    //Acá me fijo si avanzo o no
    //Si entoy al final entonces todos los padres tienen la posicion cantHijos - 1 en el stack
    std::stack<std::pair<Nodo*, int>> copiaStack(padres);
    bool estoyAlfinal = true;

    if(nodoActual->hijos.size() != 0)
        estoyAlfinal = false;

    while(!copiaStack.empty()){
        if(copiaStack.top().second != copiaStack.top().first->hijos.size() - 1){
            estoyAlfinal = false;
            break;
        }

        copiaStack.pop();
    }

    if(estoyAlfinal)
        return;
    //

    //std::cout << "PASE EL CHECK DE FINAL" << std::endl;

    if(nodoActual->hijos.size() != 0){
        //Si el nodo tiene hijos por recorrer, se pasa al que sigue
        //Para ver por que hijo se recorrió, me fijo en el stack de padres
        //No borro un padre hasta no haber recorrido todo el vector en el stack

        //Si no se entro a este padre
        if(padres.empty() || padres.top().first != nodoActual){
            //El de arriba de todo no es el actual, tengo que agregarlo
            padres.emplace(nodoActual, 0);

            //std::cout << "Pase de la categoria: " << nodoActual->valor << std::endl;

            //Ahora el nodo actual es el primer hijo
            nodoActual = nodoActual->hijos[0];

            //std::cout << "A la categotia: " << nodoActual->valor << std::endl;
        }
    }
    else{
        //El nodo no tiene hijos
        //Tengo que volver al padre

        nodoActual = padres.top().first;

        //Si estoy aca es porque volvi de un hijo
        //Me tengo que fijar en el stack como moverme
        if(padres.top().second + 1 < nodoActual->hijos.size()){
            //Puedo seguir recorriendo los hijos
            //Entonces avanzo al que sigue
            nodoActual = nodoActual->hijos[padres.top().second + 1];

            //Aumento la posicion en el stack
            padres.top().second = padres.top().second + 1;
        }
        else{
            //Ya no puedo recorrer mas hijos, tengo que irme al otro padre

            //Saco el padre de arriba del stack (que ahora deberia ser igual a nodo actual)
            padres.pop();

            //Ahora padres.top() es el padre del nodo actual
            //Entonces ahora vuelvo a ese padre
            //nodoActual = padres.top().first;

            //Tengo que ir subiendo por los padres hasta que llegue a uno donde pueda recorrer los hijos
            while(padres.top().second + 1 >= padres.top().first->hijos.size()){
                padres.pop();
            }

            //Saque a los padres que no tengo que recorrer
            //Ahora cambio el nodo
            padres.top().second = padres.top().second + 1;

            nodoActual = padres.top().first->hijos[padres.top().second];
        }

    }
}

// Ubica el iterador sobre la categoría anterior a la actual
// en el recorrido *preorder* de la taxonomía.
// Si se trata de la raíz de la taxonomía el iterador
// resultante debe ser igual al iterador end() de la taxonomía.
// Pre: el iterador está posicionado sobre una categoría.
template<class T>
void Taxonomia<T>::iterator::operator--() {
	//Hay que recorrer para atraz
    //Es igual a ++ pero lo tengo que hacer al revés

    //Si estoy en el primero (la raíz), no hay padres
    if(padres.empty()){
        //Estoy en el primero, tengo que ir al último

        //Copiado de Taxonomia::end()!
        //Tengo que poner el iterador al final de la taxonomia, recorro todo por el ultimo elem del vector
        while(nodoActual->hijos.size() > 0){
            //Si hay hijos agarro el último insertado

            //Guardo el padre y la pos en el array de hijos para poder recorrer con --
            padres.emplace(nodoActual, nodoActual->hijos.size() - 1);

            //Cambio el nodo por el siguiente
            //it.nodoActual = it.nodoActual->hijos[it.nodoActual->hijos.size() - 1];
            nodoActual = nodoActual->hijos.back(); // .back() es el último insertado, deberia dar lo mismo que arriba
        }

    }
    else{
        //No estoy en la raiz

        if(nodoActual->hijos.size() != 0){
            //Tiene hijos, tengo que ver si puedo recorrer hacia atrás el vector

            if(padres.top().second == 0){
                nodoActual = padres.top().first;
                padres.pop();
            }
            else{
                padres.top().second = padres.top().second - 1;

                nodoActual = padres.top().first->hijos[padres.top().second];

                while(nodoActual->hijos.size() > 0){
                    padres.emplace(nodoActual, nodoActual->hijos.size() - 1);
                    nodoActual = nodoActual->hijos.back();
                }
            }
        }
        else{

            if(padres.top().second == 0){
                nodoActual = padres.top().first;
                padres.pop();
            }
            else{
                padres.top().second = padres.top().second - 1;

                nodoActual = padres.top().first->hijos[padres.top().second];

                while(nodoActual->hijos.size() > 0){
                    padres.emplace(nodoActual, nodoActual->hijos.size() - 1);
                    nodoActual = nodoActual->hijos.back();
                }
            }
        }
    }
}

// Inserta una subcategoría con el nombre indicado
// en el lugar i-ésimo.
// Observación: esta operación modifica la taxonomía y
// puede invalidar otros iteradores.
// Pre: el iterador está posicionado sobre una categoría,
// y además 0 <= i <= cantSubcategorias().
template<class T>
void Taxonomia<T>::iterator::insertarSubcategoria(int i, const T& nombre) {
	//Creo el nuevo nodo
    Nodo* nuevaCategoria = new Taxonomia<T>::Nodo();

    //Le asigno el valor
    nuevaCategoria->valor = nombre;

    nodoActual->hijos.push_back(nuevaCategoria);

    //Ahora la ubico en el lugar correcto
    for(int j = nodoActual->hijos.size() - 1; i < j;j--){
        swap(nodoActual->hijos[j], nodoActual->hijos[j-1]);
    }

}

// Elimina la categoría actual de la taxonomía
// (y todas sus subcategorías).
// Observación: esta operación modifica la taxonomía y
// puede invalidar otros iteradores. Debe encargarse de
// liberar la memoria.
// Pre: el iterador está posicionado sobre una categoría,
// y además !esRaiz().
template<class T>
void Taxonomia<T>::iterator::eliminarCategoria() {
	//Tengo que eliminar la categoria actual
    //Creo un iterador para borrar los hijos
    //Taxonomia<T>::iterator itHijos(nodoActual);

//    std::cout << "BORRANDO" << std::endl;
//
//    std::cout << "ANTES DE BORRADO: " << std::endl;
//    std::cout << "NODO A BORRAR: " << nodoActual->valor << std::endl;
//    std::cout << "HIJOS: {" << std::endl;
//
//    for(int i = 0; i < nodoActual->hijos.size(); i++){
//        std::cout << nodoActual->hijos[i]->valor << ", ";
//    }
//
//    std::cout << "}" << std::endl;

    Taxonomia<T>::iterator itHijos(nodoActual);

    //Acá borro los hijos
    for(int i = 0; itHijos.nodoActual->hijos.size(); i++){
        //
//        std::cout << "CICLO " << i << "DE NODO " << nodoActual->valor << std::endl;
        itHijos.subcategoria(i);
        itHijos.eliminarCategoria();
    }
    //

    //Ahora borro el nodo actual
    T valorBorrado = nodoActual->valor;

    delete nodoActual;

//    std::cout << "EMPEZANDO PARTE DONDE CORRO EL PRIMERO" << std::endl;

    //Tengo que volver al padre
    nodoActual = padres.top().first;

    //Saco al padre de la pila de padres ya que no baje
    padres.pop();

    int indiceHijoBorrado = -1;

    for(int i = 0; i < nodoActual->hijos.size() ;i++){
        if(nodoActual->hijos[i]->valor == valorBorrado){
            indiceHijoBorrado = i;
            break;
        }
    }

    //std::cout << "INDICE DEL BORRADO: " << indiceHijoBorrado << std::endl;

    for(int i = indiceHijoBorrado; i < nodoActual->hijos.size() - 1; i++){
        swap(nodoActual->hijos[i], nodoActual->hijos[i+1]);
    }

    //Ahora el borrado esta al final
    //Reduzco el tamaño del vector
    nodoActual->hijos.resize(nodoActual->hijos.size() - 1);

//    std::cout << "DESPUES DE BORRADO: " << std::endl;
//    std::cout << "PADRE: " << nodoActual->valor << std::endl;
//    std::cout << "HIJOS: {" << std::endl;
//
//    for(int i = 0; i < nodoActual->hijos.size(); i++){
//        std::cout << nodoActual->hijos[i]->valor << ", ";
//    }
//
//    std::cout << "}" << std::endl;
}
