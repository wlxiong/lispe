/*
 *  LispE
 *
 * Copyright 2020-present NAVER Corp.
 * The 3-Clause BSD License
 */
//
//  lists.cxx
//
//


#include "lispe.h"
#include "tools.h"
#include <math.h>
#include <algorithm>

//--------------------------------------------------------------------------------
//Pools methods
//--------------------------------------------------------------------------------
void Listpool::decrement() {
    if (is_protected())
        return;
    
    status--;
    if (!status) {
        liste.decrement();
        liste.clear();
        lisp->list_pool.push_back(this);
    }
}

void Listpool::decrementstatus(uint16_t nb) {
    if (is_protected())
        return;
    
    status -= nb;
    if (!status) {
        liste.decrement();
        liste.clear();
        lisp->list_pool.push_back(this);
    }
}

void Listpool::release() {
    if (!status) {
        liste.decrement();
        liste.clear();
        lisp->list_pool.push_back(this);
    }
}

void Listpool::release(Element* e) {
    if (!status) {
        liste.decrement(e);
        liste.clear();
        lisp->list_pool.push_back(this);
    }
}

void Listpool::rawrelease() {
    if (!status) {
        liste.clear();
        liste.decrement();
        lisp->list_pool.push_back(this);
    }
}

Element* Listpool::newInstance() {
    return lisp->provideList();
}

Element* Listpool::fullcopy() {
    if (liste.marking)
        return liste.object;
    liste.marking = true;
    if (lisp->preparingthread)
        liste.object = new List;
    else
        liste.object = lisp->provideList();
    for (long i = 0; i < liste.size(); i++) {
        liste.object->append(liste[i]->fullcopy());
    }
    liste.marking = false;
    return liste.object;
}

Element* Listpool::copyatom(uint16_t s) {
    if (liste.shared(status) < s)
        return this;
    
    List* l = lisp->provideList();
    for (long i = 0; i < liste.size(); i++) {
        l->append(liste[i]->copyatom(s));
    }
    release();
    return l;
}

Element* Listpool::copying(bool duplicate) {
    //If we are in a thread preparation, then we
    //copy it as non pool objects
    //to avoid pool objects to access a lisp thread environment
    //through the wrong lisp pointer
    List* l;
    if (lisp->preparingthread)
        l = new List;
    else {
        if (!is_protected() && liste.nocdr() && !duplicate)
            return this;
        
        l = lisp->provideList();
    }
    for (long i = 0; i < liste.size(); i++) {
        l->append(liste[i]->copying(false));
    }
    return l;
}

void Numberspool::decrement() {
    if (is_protected())
        return;
    
    status--;
    if (!status) {
        liste.clear();
        lisp->numbers_pool.push_back(this);
    }
}

void Floatspool::decrement() {
    if (is_protected())
        return;
    
    status--;
    if (!status) {
        liste.clear();
        lisp->floats_pool.push_back(this);
    }
}

void Floatspool::decrementstatus(uint16_t nb) {
    if (is_protected())
        return;
    
    status -= nb;
    if (!status) {
        liste.clear();
        lisp->floats_pool.push_back(this);
    }
}

void Floatspool::release() {
    if (!status) {
        liste.clear();
        lisp->floats_pool.push_back(this);
    }
}

Element* Floatspool::newInstance() {
    return lisp->provideFloats();
}

Element* Floatspool::newInstance(Element* v) {
    return lisp->provideFloats(liste.size(), v->asFloat());
}

Element* Floatspool::fullcopy() {
    if (lisp->preparingthread)
        return new Floats(this);
    
    return lisp->provideFloats(this);
}

Element* Floatspool::copying(bool duplicate) {
    //If we are in a thread preparation, then we
    //copy it as non pool objects
    //to avoid pool objects to access a lisp thread environment
    //through the wrong lisp pointer
    if (lisp->preparingthread)
        return new Floats(this);
    
    if (!is_protected() && !duplicate)
        return this;
    
    return lisp->provideFloats(this);
}

Element* Floatspool::copyatom(uint16_t s) {
    if (liste.shared(status) < s)
        return this;
    
    Floats* f = lisp->provideFloats(this);
    release();
    return f;
}

void Numberspool::decrementstatus(uint16_t nb) {
    if (is_protected())
        return;
    
    status -= nb;
    if (!status) {
        liste.clear();
        lisp->numbers_pool.push_back(this);
    }
}

void Numberspool::release() {
    if (!status) {
        liste.clear();
        lisp->numbers_pool.push_back(this);
    }
}

Element* Numberspool::newInstance() {
    return lisp->provideNumbers();
}

Element* Numberspool::newInstance(Element* v) {
    return lisp->provideNumbers(liste.size(), v->asNumber());
}

Element* Numberspool::fullcopy() {
    if (lisp->preparingthread)
        return new Numbers(this);
    
    return lisp->provideNumbers(this);
}

Element* Numberspool::copying(bool duplicate) {
    //If we are in a thread preparation, then we
    //copy it as non pool objects
    //to avoid pool objects to access a lisp thread environment
    //through the wrong lisp pointer
    if (lisp->preparingthread)
        return new Numbers(this);
    
    if (!is_protected() && !duplicate)
        return this;
    
    return lisp->provideNumbers(this);
}

Element* Numberspool::copyatom(uint16_t s) {
    if (liste.shared(status) < s)
        return this;
    
    Numbers* n = lisp->provideNumbers(this);
    release();
    return n;
}

void Integerspool::decrement() {
    if (is_protected())
        return;
    
    status--;
    if (!status) {
        lisp->integers_pool.push_back(this);
        liste.clear();
    }
}

void Integerspool::decrementstatus(uint16_t nb) {
    if (is_protected())
        return;
    
    status -= nb;
    if (!status) {
        lisp->integers_pool.push_back(this);
        liste.clear();
    }
}

void Integerspool::release() {
    if (!status) {
        lisp->integers_pool.push_back(this);
        liste.clear();
    }
}

Element* Integerspool::newInstance() {
    return lisp->provideIntegers();
}

Element* Integerspool::newInstance(Element* v) {
    return lisp->provideIntegers(liste.size(), v->asInteger());
}

Element* Integerspool::fullcopy() {
    if (lisp->preparingthread)
        return new Integers(this);
    
    return lisp->provideIntegers(this);
}

Element* Integerspool::copying(bool duplicate) {
    //If we are in a thread preparation, then we
    //copy it as non pool objects
    //to avoid pool objects to access a lisp thread environment
    //through the wrong lisp pointer
    if (lisp->preparingthread)
        return new Integers(this);
    
    //If it is a CDR, we need to copy it...
    if (!is_protected() && !duplicate)
        return this;
    
    return lisp->provideIntegers(this);
}

Element* Integerspool::copyatom(uint16_t s) {
    if (liste.shared(status) < s)
        return this;
    
    Integers* i = lisp->provideIntegers(this);
    release();
    return i;
}

void Stringspool::decrement() {
    if (is_protected())
        return;
    
    status--;
    if (!status) {
        lisp->strings_pool.push_back(this);
        liste.clear();
    }
}

void Stringspool::decrementstatus(uint16_t nb) {
    if (is_protected())
        return;
    
    status -= nb;
    if (!status) {
        lisp->strings_pool.push_back(this);
        liste.clear();
    }
}

void Stringspool::release() {
    if (!status) {
        lisp->strings_pool.push_back(this);
        liste.clear();
    }
}

Element* Stringspool::newInstance() {
    return lisp->provideStrings();
}

Element* Stringspool::fullcopy() {
    if (lisp->preparingthread)
        return new Strings(this);
    
    return lisp->provideStrings(this);
}

Element* Stringspool::copying(bool duplicate) {
    //If we are in a thread preparation, then we
    //copy it as non pool objects
    //to avoid pool objects to access a lisp thread environment
    //through the wrong lisp pointer
    if (lisp->preparingthread)
        return new Strings(this);
    
    //If it is a CDR, we need to copy it...
    if (!is_protected() && !duplicate)
        return this;
    
    return lisp->provideStrings(this);
}

Element* Stringspool::copyatom(uint16_t s) {
    if (liste.shared(status) < s)
        return this;
    
    return lisp->provideStrings(this);
}

//--------------------------------------------------------------------------------
//List methods
//--------------------------------------------------------------------------------

Element* Pair::cdr(LispE* lisp) {
    long sz = liste.size();
    if (!sz)
        return null_;
    
    if (sz == 2)
        return liste.back();
    
    return new Pair(this, 1);
}

//------------------------------------------------------------------------------------------
inline bool LIST::compare(LispE* lisp, List* comparison, short instruction, long i, long j) {
    comparison->liste[1] = item->buffer[i];
    comparison->liste[2] = item->buffer[j];
    return comparison->eval_Boolean(lisp, instruction);
}

void LIST::sorting(LispE* lisp, List* comparison, short instruction, long rmin, long rmax) {
    //(setq s (sort '< (shuffle (cons 5 (range 1 99999 1)))))
    //(sort '< '(28 60 10 38 80 34 8 22 78 68 85 48 13 39 100 56 89 82 11 52 99 50 20 96 97 59 23 81 53 15 3 67 77 7 57 74 49 32 86 66 43 26 75 62 29 71 2 91 51 1 18 12 24 21 36 72 90 40 70 14 61 93 6 4 79 94 47 58 30 83 84 44 88 63 95 45 33 65 37 92 27 64 55 9 31 73 54 16 98 5 46 25 76 42 17 69 19 35 5 41 87))
    //(sort '< '(20 12 15 13 19 17 14))
    //(sort '< (shuffle (range 1 16 1)))
    //(sort '< '(4 3 7 1 5))
    //(sort '< '(10 4 8 5 12 2 6 11 3 9 7 9))
    
    //check sorting stability
    //(loop i (range 1 9999 1) (select (<= (at s i) (at s (+ i 1))) (println 'erreur i)))
    
    long j = rmax-rmin+1;
    long pivot;
    
    if (j < 7) {
        if (j < 2)
            return;
        
        if (j == 2) {
            if (compare(lisp, comparison, instruction, rmax, rmin))
                item->swap(rmax, rmin);
            return;
        }
        
        if (j == 3) {
            if (compare(lisp, comparison, instruction, rmin, rmin + 1)) {
                if (compare(lisp, comparison, instruction, rmin + 1, rmax))
                    return;
            }
            else {
                item->swap(rmin, rmin + 1);
                if (compare(lisp, comparison, instruction, rmin + 1, rmax))
                    return;
            }
            item->swap(rmax, rmin + 1);
            if (compare(lisp, comparison, instruction, rmin, rmin + 1))
                return;
            item->swap(rmin, rmin + 1);
            return;
        }
        
        long sz;
        while (rmax > rmin) {
            sz = rmin;
            for (j = rmin; j < rmax; j++) {
                if (compare(lisp, comparison, instruction, j + 1, j)) {
                    item->swap(j, j + 1);
                    sz = j;
                    pivot = j;
                    while (pivot > rmin && compare(lisp, comparison, instruction, pivot, pivot - 1))
                        item->swap(pivot, pivot - 1);
                }
            }
            rmax = sz;
        }
        return;
    }
    
    pivot = rmin - 1;
    comparison->liste[2] = item->buffer[rmax];
    for (j = rmin; j < rmax; j++) {
        comparison->liste[1] = item->buffer[j];
        if (comparison->eval_Boolean(lisp, instruction)) {
            pivot++;
            item->swap(pivot,j);
        }
    }
    pivot++;
    item->swap(pivot, rmax);
    
    sorting(lisp, comparison, instruction, rmin, pivot-1);
    sorting(lisp, comparison, instruction, pivot+1, rmax);
}

void LIST::sorting(LispE* lisp, List* comparison) {
    //We sort between home and last...
    long sz = item->last - home;
    if (sz <= 1)
        return;
    
    sorting(lisp, comparison, comparison->liste[0]->type, home, item->last - 1);
}

//------------------------------------------------------------------------------------------

Element* List::check_member(LispE* lisp, Element* the_set) {
    List* r = lisp->provideList();
    Element* e;
    for (long i = 0; i < size(); i++) {
        e = liste[i]->check_member(lisp,the_set);
        r->append(e);
    }
    return r;
}

Element* LList::check_member(LispE* lisp, Element* the_set) {
    LList* r = new LList(liste.mark);
    Element* e;
    for (u_link* a = liste.begin(); a != NULL; a = a->next()) {
        e = a->value->check_member(lisp,the_set);
        r->append(e);
    }
    return r;
}

Element* List::minimum(LispE* lisp) {
    if (!liste.size())
        return null_;
    Element* v = index(0);
    for (long i = 1; i < liste.size(); i++) {
        if (v->more(lisp, liste[i]) == true_)
            v = liste[i];
    }
    return v->copying(false);
}

Element* LList::minimum(LispE* lisp) {
    if (liste.empty())
        return null_;
    Element* v = liste.front();
    u_link* it = liste.begin();
    it = it->next();
    for (;it != NULL; it = it->next()) {
        if (v->more(lisp, it->value) == true_)
            v = it->value;
    }
    return v->copying(false);
}

Element* List::maximum(LispE* lisp) {
    if (!liste.size())
        return null_;
    Element* v = index(0);
    for (long i = 1; i < liste.size(); i++) {
        if (v->less(lisp, liste[i]) == true_)
            v = liste[i];
    }
    return v->copying(false);
}

Element* LList::maximum(LispE* lisp) {
    if (liste.empty())
        return null_;
    Element* v = liste.front();
    u_link* it = liste.begin();
    it = it->next();
    for (;it != NULL; it = it->next()) {
        if (v->less(lisp, it->value) == true_)
            v = it->value;
    }
    return v->copying(false);
}

Element* List::minmax(LispE* lisp) {
    if (!liste.size())
        return null_;
    Element* v_min = index(0);
    Element* v_max = v_min;
    for (long i = 1; i < liste.size(); i++) {
        if (v_min->more(lisp, liste[i]) == true_)
            v_min = liste[i];
        else {
            if (v_max->less(lisp, liste[i]) == true_)
                v_max = liste[i];
        }
    }
    List* l = lisp->provideList();
    l->append(v_min);
    l->append(v_max);
    return l;
}

Element* LList::minmax(LispE* lisp) {
    if (liste.empty())
        return null_;
    Element* v_min = liste.front();
    Element* v_max = v_min;
    u_link* it = liste.begin();
    it = it->next();
    for (; it != NULL; it = it->next()) {
        if (v_min->more(lisp, it->value) == true_)
            v_min = it->value;
        else {
            if (v_max->less(lisp, it->value) == true_)
                v_max = it->value;
        }
    }
    List* l = lisp->provideList();
    l->append(v_min);
    l->append(v_max);
    return l;
}

void List::flatten(LispE* lisp, List* l) {
    for (long i = 0; i < size(); i++) {
        liste[i]->flatten(lisp, l);
    }
}

void List::flatten(LispE* lisp, Numbers* l) {
    for (long i = 0; i < size(); i++) {
        liste[i]->flatten(lisp, l);
    }
}

void List::flatten(LispE* lisp, Floats* l) {
    for (long i = 0; i < size(); i++) {
        liste[i]->flatten(lisp, l);
    }
}

void LList::flatten(LispE* lisp, List* l) {
    for (u_link* a = liste.begin(); a != NULL; a = a->next()) {
        a->value->flatten(lisp, l);
    }
}

void LList::flatten(LispE* lisp, Numbers* l) {
    for (u_link* a = liste.begin(); a != NULL; a = a->next()) {
        a->value->flatten(lisp, l);
    }
}

void LList::flatten(LispE* lisp, Floats* l) {
    for (u_link* a = liste.begin(); a != NULL; a = a->next()) {
        a->value->flatten(lisp, l);
    }
}

Element* List::transposed(LispE* lisp) {
    vecte<long> sz;
    getShape(sz);
    if (sz.size() <= 1)
        return this;
    long i = sz[0];
    sz.vecteur[0] = sz[1];
    sz.vecteur[1] = i;
    Element* tenseur;
    if (sz.size() == 2)
        tenseur = new Matrice(lisp, sz[0], sz[1], 0.0);
    else
        tenseur = new Tenseur(lisp, sz, zero_);
    
    Element* e;
    for (i = 0; i < sz[1]; i++) {
        e = liste[i];
        for (long j = 0; j < sz[0]; j++) {
            tenseur->index(j)->replacing(i, e->index(j)->copying(false));
        }
    }
    
    return tenseur;
}

void List::storevalue(LispE* lisp, double v) {
    append(lisp->provideNumber(v));
}

void List::storevalue(LispE* lisp,long v) {
    append(lisp->provideInteger(v));
}

void List::storevalue(LispE* lisp, u_ustring& v) {
    append(lisp->provideString(v));
}

void List::garbaging_values(LispE* lisp) {
    if (liste.marking)
        return;
    liste.marking = true;
    for (long i = 0; i < liste.size(); i++) {
        lisp->control_garbaging(liste[i]);
        liste[i]->garbaging_values(lisp);
    }
    liste.marking = false;
}

void LList::garbaging_values(LispE* lisp) {
    for (u_link* a = liste.begin(); a != NULL; a = a->next()) {
        lisp->control_garbaging(a->value);
        a->value->garbaging_values(lisp);
    }
}

void List::append(LispE* lisp, u_ustring& k) {
    append(lisp->provideString(k));
}

void LList::append(LispE* lisp, u_ustring& k) {
    append(lisp->provideString(k));
}

void List::append(LispE* lisp, double v) {
    append(lisp->provideNumber(v));
}

void LList::append(LispE* lisp, double v) {
    append(lisp->provideNumber(v));
}

void List::append(LispE* lisp, long v) {
    append(lisp->provideInteger(v));
}

void LList::append(LispE* lisp, long v) {
    append(lisp->provideInteger(v));
}

Element* List::storeRank(LispE* lisp, Element* current, vecte<long>& shape, vecte<long>& positions, long idx) {
    long nb = shape.size() - idx;
    long p_idx = -1;
    if (idx < positions.size())
        p_idx = positions[idx];
    
    long i;
    if (p_idx == -1) {
        //If nb == 2, then it is a matrix
        //If nb == 1, then it is a vector
        if (nb == 1)
            return current;
        
        List* m = lisp->provideList();

        if (nb == 2) {
            Element* result;
            Element* ref = current->index(0);
            if (ref->isValueList()) {
                for (long j = 0; j < shape[idx+1]; j++) {
                    result = ref->newInstance();
                    for (i = 0; i < shape[idx]; i++) {
                        result->append(current->index(i)->index(j));
                    }
                    m->append(result);
                }
            }
            else {
                for (long j = 0; j < shape[idx+1]; j++) {
                    result = ref->newInstance();
                    for (i = 0; i < shape[idx]; i++) {
                        result->append(current->index(i)->index(j)->copying(false));
                    }
                    m->append(result);
                }
            }
        }
        else {
            for (i = 0; i < shape[idx]; i++) {
                m->append(storeRank(lisp, current->index(i), shape, positions, idx+1));
            }
        }
        return m;
    }
    
    if (nb == 1)
        return current->index(p_idx)->copying(false);
    
    return storeRank(lisp, current->index(p_idx), shape, positions, idx+1);
}

Element* List::rank(LispE* lisp, vecte<long>& positions) {
    //We get rid of the final negative values (useless)
    while (positions.size() > 1 && positions.back() < 0)
        positions.pop_back();
    
    vecte<long> shape;
    getShape(shape);
    if (!checkShape(0, shape))
        throw new Error("Error: unregular matrix: some sub-lists have different sizes");
    
    short sz = positions.size();
    if (!sz || sz > shape.size())
        throw new Error("Error: index mismatch");

    while (positions.size() > 1 && positions.back() < 0)
        positions.pop_back();

    return storeRank(lisp, this, shape, positions, 0);
}

Element* LList::loop(LispE* lisp, short label, List* code) {
    long i_loop;
    Element* e = null_;
    lisp->recording(null_, label);
    Element* element;
    long sz = code->liste.size();
    for (u_link* a = liste.begin(); a != NULL; a = a->next()) {
        element = a->value->copying(false);
        lisp->replacingvalue(element, label);
        _releasing(e);
        //We then execute our instructions
        for (i_loop = 3; i_loop < sz && e->type != l_return; i_loop++) {
            e->release();
            e = code->liste[i_loop]->eval(lisp);
        }
        if (e->type == l_return) {
            if (e->isBreak())
                return null_;
            return e;
        }
    }
    return e;
}

Element* List::loop(LispE* lisp, short label, List* code) {
    long i_loop;
    Element* e = null_;
    lisp->recording(null_, label);
    Element* element;
    long sz = code->liste.size();
    for (long i = 0; i < liste.size(); i++) {
        element = liste[i]->copying(false);
        lisp->replacingvalue(element, label);
        _releasing(e);
        //We then execute our instructions
        for (i_loop = 3; i_loop < sz && e->type != l_return; i_loop++) {
            e->release();
            e = code->liste[i_loop]->eval(lisp);
        }
        if (e->type == l_return) {
            if (e->isBreak())
                return null_;
            return e;
        }
    }
    return e;
}

Element* List::multiloop(LispE* lisp) {
    List* values = lisp->provideList();
    List* indexes = lisp->provideList();
    Element* e = null_;
    Element* ix;
    long sz = size();
    long var;
    long indexe = 0;
    long nbvars = liste[1]->size();
    
    short label;
    
    try {
        for (var = 0; var < nbvars; var++) {
            label = liste[1]->index(var)->label();
            lisp->recording(null_, label);
            e = liste[var+2]->eval(lisp);
            values->append(e);
            indexes->append(e->thekeys(lisp));
        }
        
        long nb = values->liste[0]->size();
        e = null_;
        while (indexe < nb) {
            _releasing(e);
            for (var = 0; var < nbvars; var++) {
                ix = indexes->liste[var]->index(indexe);
                e = values->liste[var]->value_on_index(lisp, ix);
                label = liste[1]->index(var)->label();
                lisp->replacingvalue(e, label);
            }
            //We then execute our instructions
            e = null_;
            for (var = nbvars + 2; var < sz && e->type != l_return; var++) {
                e->release();
                e = liste[var]->eval(lisp);
            }
            if (e->type == l_return) {
                indexes->release();
                if (e->isBreak()) {
                    values->release();
                    return null_;
                }
                values->release(e->eval(lisp));
                return e;
            }
            indexe++;
        }
    }
    catch(Error* err) {
        values->release();
        indexes->release();
        throw err;
    }
    
    values->release(e);
    indexes->release();
    return e;
}

Element* List::polyloop(LispE* lisp) {
    List* values = lisp->provideList();
    Element* e = null_;
    long sz = size();
    long var;
    long indexe = 0;
    long nbvars = liste[1]->size();
    
    short label;
    
    try {
        for (var = 0; var < nbvars; var++) {
            label = liste[1]->index(var)->label();
            lisp->recording(null_, label);
            values->append(liste[var + 2]->eval(lisp));
        }
        
        long nb = values->liste[0]->size();
        
        while (indexe < nb) {
            _releasing(e);
            for (var = 0; var < nbvars; var++) {
                e = values->liste[var]->value_from_index(lisp, indexe);
                label = liste[1]->index(var)->label();
                lisp->replacingvalue(e, label);
            }
            e = null_;
            //We then execute our instructions
            for (var = nbvars + 2; var < sz && e->type != l_return; var++) {
                e->release();
                e = liste[var]->eval(lisp);
            }
            if (e->type == l_return) {
                if (e->isBreak()) {
                    values->release();
                    return null_;
                }
                values->release(e->eval(lisp));
                return e;
            }
            indexe++;
        }
    }
    catch(Error* err) {
        values->release();
        throw err;
    }
    
    values->release(e);
    return e;
}

Element* List::insert(LispE* lisp, Element* e, long ix) {
    if (ix < 0)
        throw new Error("Error: Wrong index in 'insert'");
    
    e = e->copying(false);
    List* l = (List*)duplicate_constant();
    l->liste.insert(ix, e);
    return l;
}

Element* LList::insert(LispE* lisp, Element* e, long ix) {
    if (ix < 0)
        throw new Error("Error: Wrong index in 'insert'");
    
    e = e->copying(false);
    LList* l = (LList*)duplicate_constant();
    l->insertion(e, ix);
    return l;
}

Element* List::rotate(bool left) {
    if (liste.size() <= 1)
        return this;
    
    List* l = (List*)newInstance();
    if (left) {
        for (long i = 1; i < liste.size(); i++)
            l->append(liste[i]->copying(false));
        l->append(liste[0]->copying(false));
        return l;
    }
    
    l->append(liste.back()->copying(false));
    for (long i = 0; i < liste.size() - 1; i ++)
        l->append(liste[i]->copying(false));
    return l;
}

Element* LList::rotate(bool left) {
    if (!liste.atleast2())
        return this;
    
    LList* l = new LList(liste.mark);
    u_link* it = liste.last();
    bool cyclic = (it->_next != NULL);
    u_link* tail = NULL;
    
    if (left) {
        l->append(liste.first->value->copying(false));
        if (cyclic)
            tail = l->liste.first;

        while (it != liste.first) {
            l->push_front(it->value->copying(false));
            it = it->previous();
        }
        
        if (cyclic) {
            //there is a cycle
            //we need to reproduce it...
            l->liste.first->_previous = tail;
            tail->_next = l->liste.first;
        }
        return l;
    }
    Element* last = it->value->copying(false);
    it = it->previous();
    while (it != NULL) {
        l->push_front(it->value->copying(false));
        if (cyclic) {
            tail = l->liste.first;
            cyclic = false;
        }
        it = it->previous();
    }
    l->push_front(last);
    if (tail != NULL) {
        //there is a cycle
        //we need to reproduce it...
        l->liste.first->_previous = tail;
        tail->_next = l->liste.first;
    }
    return l;
}

Element* List::unique(LispE* lisp) {
    if (liste.size() == 0)
        return this;
    
    List* list = lisp->provideList();
    long i, j;
    bool found;
    list->append(liste[0]->copying(false));
    for (i = 1; i < liste.size(); i++) {
        found = false;
        for (j = 0; j < list->liste.size(); j++) {
            if (liste[i]->unify(lisp, list->liste[j], false)) {
                found = true;
                break;
            }
        }
        if (!found)
            list->append(liste[i]->copying(false));
    }
    return list;
}

Element* LList::unique(LispE* lisp) {
    if (liste.empty())
        return this;
    
    List* list = lisp->provideList();
    bool found;
    list->append(liste.front()->copying(false));
    u_link* it = liste.begin();
    it = it->next();
    for (;it != NULL; it = it->next()) {
        found = false;
        for (long j = 0; j < list->liste.size(); j++) {
            if (it->value->unify(lisp, list->liste[j], false)) {
                found = true;
                break;
            }
        }
        if (!found)
            list->append(it->value->copying(false));
    }
    return list;
}

Element* List::thekeys(LispE* lisp) {
    Integers* keys = lisp->provideIntegers();
    for (long i = 0; i< size(); i++) {
        keys->liste.push_back(i);
    }
    return keys;
}

Element* LList::thekeys(LispE* lisp) {
    Integers* keys = lisp->provideIntegers();
    long sz = size();
    for (long i = 0; i < sz; i++) {
        keys->liste.push_back(i);
    }
    return keys;
}

long List::find_element(LispE* lisp, Element* valeur, long ix) {
    for (long i = ix; i < liste.size(); i++) {
        if (liste[i]->equal(lisp, valeur) == true_)
            return i;
    }
    return -1;
}

Element* List::search_element(LispE* lisp, Element* valeur, long ix) {
    for (long i = ix; i < liste.size(); i++) {
        if (liste[i]->equal(lisp, valeur) == true_)
            return lisp->provideInteger(i);
    }
    return null_;
}

Element* LList::search_element(LispE* lisp, Element* valeur, long ix) {
    u_link*  it = at(ix);
    long i = 0;
    for (;it != NULL; it = it->next()) {
        if (it->value->equal(lisp, valeur) == true_)
            return lisp->provideInteger(i);
        i++;
    }
    return null_;
}

bool List::check_element(LispE* lisp, Element* valeur) {
    for (long i = 0; i < liste.size(); i++) {
        if (liste[i]->equal(lisp, valeur) == true_)
            return true;
    }
    return false;
}

bool LList::check_element(LispE* lisp, Element* valeur) {
    u_link*  it = liste.begin();
    long i = 0;
    for (;it != NULL; it = it->next()) {
        if (it->value->equal(lisp, valeur) == true_)
            return true;
        i++;
    }
    return false;
}

Element* List::replace_all_elements(LispE* lisp, Element* valeur, Element* remp) {
    long nb = 0;
    for (long i = 0; i < liste.size(); i++) {
        if (liste[i]->equal(lisp, valeur) == true_) {
            replacing(i, remp->copying(false));
            nb++;
        }
    }
    return lisp->provideInteger(nb);
}

Element* LList::replace_all_elements(LispE* lisp, Element* valeur, Element* remp) {
    u_link* a = liste.begin();
    long nb = 0;
    for (; a != NULL; a = a->next()) {
        if (a->value->equal(lisp, valeur) == true_) {
            if (a->value != remp) {
                a->value->decrement();
                a->value = remp->copying(false);
                a->value->increment();
                nb++;
            }
        }
    }
    return lisp->provideInteger(nb);
}

Element* List::search_all_elements(LispE* lisp, Element* valeur, long ix) {
    Integers* l = lisp->provideIntegers();
    long sz = liste.size();
    for (long i = ix; i < sz; i++) {
        if (liste[i]->equal(lisp, valeur) == true_) {
            l->liste.push_back(i);
        }
    }
    if (l->liste.size() == 0) {
        l->release();
        return emptylist_;
    }
    return l;
}

Element* LList::search_all_elements(LispE* lisp, Element* valeur, long ix) {
    Integers* l = lisp->provideIntegers();
    u_link* a = at(ix);
    for (; a != NULL; a = a->next()) {
        if (a->value->equal(lisp, valeur) == true_) {
            l->liste.push_back(ix);
        }
        ix++;
    }
    if (l->liste.empty()) {
        l->release();
        return emptylist_;
    }
    return l;
}

Element* List::count_all_elements(LispE* lisp, Element* valeur, long ix) {
    long nb = 0;
    long sz = liste.size();
    for (long i = ix; i < sz; i++) {
        if (liste[i]->equal(lisp, valeur) == true_) {
            nb++;
        }
    }
    return lisp->provideInteger(nb);
}

Element* LList::count_all_elements(LispE* lisp, Element* valeur, long ix) {
    long nb = 0;
    u_link* a = at(ix);
    for (; a != NULL; a = a->next()) {
        if (a->value->equal(lisp, valeur) == true_) {
            nb++;
        }
        ix++;
    }
    return lisp->provideInteger(nb);
}

Element* List::list_and(LispE* lisp, Element* value) {
    if (!value->isList() && !value->isSet())
        throw new Error("Error: Can only apply '&&&' to strings, lists or sets");
    
    List* result = lisp->provideList();
    long sz = liste.size();
    for (long i = 0; i < sz; i++) {
        if (value->check_element(lisp, liste[i]) && !result->check_element(lisp, liste[i]))
            result->append(liste[i]->copying(false));
    }
    if (result->isEmpty()) {
        result->release();
        return emptylist_;
    }
    return result;
}

Element* LList::list_and(LispE* lisp, Element* value) {
    if (!value->isList() && !value->isSet())
        throw new Error("Error: Can only apply '&&&' to strings, lists or sets");
    
    LList* result = new LList(liste.mark);
    u_link* a = liste.begin();
    for (; a != NULL; a = a->next()) {
        if (value->check_element(lisp, a->value) && !result->check_element(lisp, a->value))
            result->append(a->value->copying(false));
    }
    if (result->isEmpty()) {
        result->release();
        return emptylist_;
    }
    return result;
}

Element* List::list_or(LispE* lisp, Element* value) {
    if (!value->isList() && !value->isSet())
        throw new Error("Error: Can only apply '|||' to strings, lists or sets");
    
    List* result = lisp->provideList();
    long sz = liste.size();
    long i;
    for (i = 0; i < sz; i++) {
        if (result->check_element(lisp, liste[i]) == false)
            result->append(liste[i]->copying(false));
    }
    
    if (value->type == t_llist) {
        u_link* a = ((LList*)value)->liste.begin();
        for (; a != NULL; a = a->next()) {
            if (result->check_element(lisp, a->value) == false)
                result->append(a->value->copying(false));
        }
    }
    else {
        if (value->isList()) {
            sz = value->size();
            for (i = 0; i < sz; i++) {
                if (result->check_element(lisp, value->index(i)) == false)
                    result->append(value->index(i)->copying(false));
            }
        }
        else {
            if (value->isSet()) {
                void* iter = value->begin_iter();
                Element* next_value = value->next_iter(lisp, iter);
                while (next_value != emptyatom_) {
                    if (result->check_element(lisp, next_value) == false)
                        result->append(next_value);
                    next_value = value->next_iter(lisp, iter);
                }
                value->clean_iter(iter);
            }
        }
    }
    
    if (result->isEmpty()) {
        result->release();
        return emptylist_;
    }
    return result;
}

Element* LList::list_or(LispE* lisp, Element* value) {
    if (!value->isList() && !value->isSet())
        throw new Error("Error: Can only apply '|||' to strings, lists or sets");
    
    LList* result = new LList(liste.mark);
    u_link* a = liste.begin();
    for (; a != NULL; a = a->next()) {
        if (result->check_element(lisp, a->value) == false)
            result->append(a->value->copying(false));
    }
    
    if (value->type == t_llist) {
        u_link* a = ((LList*)value)->liste.begin();
        for (; a != NULL; a = a->next()) {
            if (result->check_element(lisp, a->value) == false)
                result->append(a->value->copying(false));
        }
    }
    else {
        if (value->isList()) {
            long sz = value->size();
            for (long i = 0; i < sz; i++) {
                if (result->check_element(lisp, value->index(i)) == false)
                    result->append(value->index(i)->copying(false));
            }
        }
        else {
            if (value->isSet()) {
                void* iter = value->begin_iter();
                Element* next_value = value->next_iter(lisp, iter);
                while (next_value != emptyatom_) {
                    if (result->check_element(lisp, next_value) == false)
                        result->append(next_value);
                    next_value = value->next_iter(lisp, iter);
                }
                value->clean_iter(iter);
            }
        }
    }
    
    if (result->isEmpty()) {
        result->release();
        return emptylist_;
    }
    return result;
}

Element* List::list_xor(LispE* lisp, Element* value) {
    if (!value->isList() && !value->isSet())
        throw new Error("Error: Can only apply '^^^' to strings, lists or sets");
    
    List* intersection = (List*)list_and(lisp, value);
    
    List* result = lisp->provideList();
    long sz = liste.size();
    long i;
    for (i = 0; i < sz; i++) {
        if (!intersection->check_element(lisp, liste[i]) && !result->check_element(lisp, liste[i]))
            result->append(liste[i]->copying(false));
    }
    
    Element* v;
    if (value->type == t_llist) {
        u_link* a = ((LList*)value)->liste.begin();
        for (; a != NULL; a = a->next()) {
            v = a->value;
            if (!intersection->check_element(lisp, v) && !result->check_element(lisp, v))
                result->append(v->copying(false));
        }
    }
    else {
        if (value->isList()) {
            sz = value->size();
            for (i = 0; i < sz; i++) {
                v = value->index(i);
                if (!intersection->check_element(lisp, v) && !result->check_element(lisp, v))
                    result->append(v->copying(false));
            }
        }
        else {
            if (value->isSet()) {
                void* iter = value->begin_iter();
                v = value->next_iter(lisp, iter);
                while (v != emptyatom_) {
                    if (!intersection->check_element(lisp, v) && !result->check_element(lisp, v))
                        result->append(v);
                    v = value->next_iter(lisp, iter);
                }
                value->clean_iter(iter);
            }
        }
    }

    intersection->release();
    if (result->isEmpty()) {
        result->release();
        return emptylist_;
    }
    return result;
}

Element* LList::list_xor(LispE* lisp, Element* value) {
    if (!value->isList() && !value->isSet())
        throw new Error("Error: Can only apply '^^^' to strings, lists or sets");
    
    LList* intersection = (LList*)list_and(lisp, value);
    
    LList* result = new LList(liste.mark);
    u_link* a = liste.begin();
    for (; a != NULL; a = a->next()) {
        if (!intersection->check_element(lisp, a->value) && !result->check_element(lisp, a->value))
            result->append(a->value->copying(false));
    }
    
    Element* v;
    if (value->type == t_llist) {
        u_link* a = ((LList*)value)->liste.begin();
        for (; a != NULL; a = a->next()) {
            v = a->value;
            if (!intersection->check_element(lisp, v) && !result->check_element(lisp, v))
                result->append(v->copying(false));
        }
    }
    else {
        if (value->isList()) {
            long sz = value->size();
            for (long i = 0; i < sz; i++) {
                v = value->index(i);
                if (!intersection->check_element(lisp, v) && !result->check_element(lisp, v))
                    result->append(v->copying(false));
            }
        }
        else {
            if (value->isSet()) {
                void* iter = value->begin_iter();
                v = value->next_iter(lisp, iter);
                while (v != emptyatom_) {
                    if (!intersection->check_element(lisp, v) && !result->check_element(lisp, v))
                        result->append(v);
                   v = value->next_iter(lisp, iter);
                }
                value->clean_iter(iter);
            }
        }
    }

    intersection->release();
    if (result->isEmpty()) {
        result->release();
        return emptylist_;
    }
    return result;
}

Element* List::search_reverse(LispE* lisp, Element* valeur, long ix) {
    for (long i = liste.size() - 1; i >= ix; i--) {
        if (liste[i]->equal(lisp, valeur) == true_)
            return lisp->provideInteger(i);
    }
    return minusone_;
}

Element* LList::search_reverse(LispE* lisp, Element* valeur, long ix) {
    u_link* it = liste.b_at(ix);
    for (; it != NULL; it = it->previous()) {
        if (it->value->equal(lisp, valeur) == true_)
            return lisp->provideInteger(ix);
        if (!ix)
            break;
        ix--;
    }
    
    return minusone_;
}

Element* List::reverse(LispE* lisp, bool duplicate) {
    if (liste.size() <= 1)
        return this;
    
    if (duplicate) {
        List* l = lisp->provideList();
        for (long i = liste.size()-1; i >= 0; i--)
            l->append(liste[i]->copying(false));
        return l;
    }
    
    liste.reverse();
    return this;
}

Element* LList::reverse(LispE* lisp, bool duplicate) {
    if (duplicate) {
        LList* l = new LList(liste.mark);
        u_link*  it = liste.begin();
        if (it == NULL)
            return l;
        
        u_link* p = it;
        u_link* tail = NULL;
        for (; it != NULL; it = it->next()) {
            p = it;
            l->push_front(it->value->copying(false));
            if (tail == NULL)
                tail = l->liste.first;
        }
        if (p->_next != NULL) {
            //there is a cycle
            //we need to reproduce it...
            l->liste.first->_previous = tail;
            tail->_next = l->liste.first;
        }
        return l;
    }
    
    liste.reverse();
    return this;
}

Element* List::rotate(LispE* lisp, long axis) {
    return reverse(lisp, true);
}

Element* List::protected_index(LispE* lisp,long i) {
    if (i >= 0 && i < liste.size())
        return liste[i];
    return null_;
}

Element* LList::protected_index(LispE* lisp,long i) {
    Element* e = null_;
    if (i >= 0) {
        e = at_e(i)->copying(false);
        if (e == NULL)
            return null_;
    }
    return e;
}

Element* List::last_element(LispE* lisp) {
    if (!liste.size())
        return null_;
    return liste.back();
}

Element* LList::last_element(LispE* lisp) {
    if (liste.empty())
        return null_;
    return liste.back();
}

Element* List::value_on_index(LispE* lisp, long i) {
    if (i >= 0 && i < liste.size())
        return liste[i]->copying(false);
    return null_;
}

Element* LList::value_on_index(LispE* lisp, long i) {
    Element* e = null_;
    if (i >= 0) {
        e = at_e(i)->copying(false);
        if (e == NULL)
            return null_;
    }
    return e;
}

Element* List::value_from_index(LispE* lisp, long i) {
    return liste[i]->copying(false);
}

Element* LList::value_from_index(LispE* lisp, long i) {
    return at_e(i)->copying(false);
}

Element* List::value_on_index(LispE* lisp, Element* ix) {
    long i = ix->checkInteger(lisp);
    if (i < 0)
        i = liste.size() + i;
    
    if (i >= 0 && i < liste.size())
        return liste[i]->copying(false);
    
    return null_;
}

Element* LList::value_on_index(LispE* lisp, Element* ix) {
    long i = ix->checkInteger(lisp);
    if (i < 0)
        i = liste.size() + i;
    
    ix = null_;
    if (i >= 0) {
        ix = at_e(i)->copying(false);
        if (ix == NULL)
            return null_;
    }
    return ix;
}

Element* List::protected_index(LispE* lisp, Element* ix) {
    long i = ix->checkInteger(lisp);
    if (i < 0)
        i = liste.size() + i;
    
    if (i >= 0 && i < liste.size())
        return liste[i];
    
    throw new Error("Error: index out of bounds");
}

Element* LList::protected_index(LispE* lisp, Element* ix) {
    long i = ix->checkInteger(lisp);
    if (i < 0)
        i = liste.size() + i;
    
    if (i >= 0) {
        ix = at_e(i)->copying(false);
        if (ix == NULL)
            throw new Error("Error: index out of bounds");
    }
    else
        throw new Error("Error: index out of bounds");
    return ix;
}

Element* List::join_in_list(LispE* lisp, u_ustring& sep) {
    u_ustring str;
    u_ustring beg;
    for (long i = 0; i < liste.size(); i++) {
        str += beg;
        beg = sep;
        str += liste[i]->asUString(lisp);
    }
    return lisp->provideString(str);
}

Element* LList::join_in_list(LispE* lisp, u_ustring& sep) {
    u_ustring str;
    u_ustring beg;
    for (u_link* a = liste.begin(); a != NULL; a = a->next()) {
        str += beg;
        beg = sep;
        str += a->value->asUString(lisp);
    }
    return lisp->provideString(str);
}

Element* List::newInstance(Element* e) {
    long i;
    long sz = size();
    for (i = 0; i < sz; i++) {
        if (liste[i]->isList())
            break;
    }
    
    if (i == sz) {
        switch(e->type) {
            case t_float:
                return new Floats(sz, e->asNumber());
            case t_number:
                return new Numbers(sz, e->asNumber());
            case t_short:
                return new Shorts(sz, e->asShort());
            case t_integer:
                return new Integers(sz, e->asInteger());
            case t_string:
                return new Strings(sz, e->asString(NULL));
        }
    }
    
    List* lst = (List*)newInstance();
    for (i = 0; i < sz; i++) {
        if (liste[i]->isList())
            lst->append(liste[i]->newInstance(e));
        else
            lst->append(e);
    }
    return lst;
}

Element* List::equal(LispE* lisp, Element* e) {
    return booleans_[e->isList() && liste.equal(((List*)e)->liste)];
}

Element* LList::equal(LispE* lisp, Element* e) {
    return booleans_[((e->type == t_llist && e->size() == 0 && liste.empty()) || e == this)];
}

bool List::egal(Element* e) {
    return e->isList() && liste.equal(((List*)e)->liste);
}

bool LList::egal(Element* e) {
    return ((e->type == t_llist && e->size() == 0 && liste.empty()) || e == this);
}

Element* List::extraction(LispE* lisp, List* l) {
    Element* e_from = l->liste[2];
    Element* e;
    
    long from = 0;
    long firstisString = -1;
    short nxt = 3;
    short ty;
    switch (e_from->label()) {
        case l_minus:
            e_from = l->liste[3]->eval(lisp);
            nxt = 4;
            ty = e_from->type;
            if (ty == t_string)
                ty = t_minus_string;
            else
                throw new Error("Error: Wrong value after first operator: '-'");
            break;
        case l_plus:
            e_from = l->liste[3]->eval(lisp);
            nxt = 4;
            ty = e_from->type;
            if (ty == t_string)
                ty = t_plus_string;
            else
                throw new Error("Error: Wrong value after first operator: '+'");
            break;
        case l_minus_plus:
            e_from = l->liste[3]->eval(lisp);
            nxt = 4;
            ty = e_from->type;
            if (ty == t_string)
                ty = t_minus_plus_string;
            else
                throw new Error("Error: Wrong value after first operator: '-+'");
            break;
        default:
            e_from = e_from->eval(lisp);
            ty = e_from->type;
    }
    
    e = null_;
    switch (ty) {
        case t_string: {
            e = search_element(lisp, e_from, 0);
            if (e == null_)
                return emptylist_;
            from = e->asInteger();
            firstisString = 0;
            break;
        }
        case t_plus_string: {
            e = search_element(lisp, e_from, 0);
            if (e == null_)
                return emptylist_;
            firstisString = e->asInteger();
            break;
        }
        case t_minus_string: {
            e = search_reverse(lisp, e_from, 0);
            if (e == minusone_)
                return emptylist_;
            //We skip the first characters
            from = e->asInteger() + 1;
            firstisString = 0;
            break;
        }
        case t_minus_plus_string: {
            e = search_reverse(lisp, e_from, 0);
            if (e == minusone_)
                return emptylist_;
            firstisString = e->asInteger();
            break;
        }
        case t_float:
        case t_short:
        case t_integer:
        case t_number:
            from = e_from->asInteger();
            if (from < 0)
                from = size() + from;
            break;
        default:
            e->release();
            e_from->release();
            throw new Error("Error: cannot use the first position in 'extract'");
    }
    
    e->release();
    e_from->release();
    
    if (from < 0 || from >= size())
        return emptylist_;
    
    if (nxt == l->size()) {
        //Only one element is returned
        return liste[from]->copying(false);
    }
    
    Element* e_upto = l->liste[nxt];
    switch (e_upto->label()) {
        case l_minus:
            e_upto = l->liste[nxt+1]->eval(lisp);
            ty = e_upto->type;
            if (ty == t_string)
                ty = t_minus_string;
            else
                throw new Error("Error: Wrong value after second operator: '-'");
            break;
        case l_plus:
            e_upto = l->liste[nxt+1]->eval(lisp);
            ty = e_upto->type;
            if (ty == t_string)
                ty = t_plus_string;
            else
                throw new Error("Error: Wrong value after second operator: '+'");
            break;
        case l_minus_plus:
            e_upto = l->liste[nxt+1]->eval(lisp);
            ty = e_from->type;
            if (ty == t_string)
                ty = t_minus_plus_string;
            else
                throw new Error("Error: Wrong value after second operator: '-+'");
            break;
        default:
            e_upto = e_upto->eval(lisp);
            ty = e_upto->type;
    }
    
    long upto;
    e = null_;
    switch (ty) {
        case t_string: {
            if (firstisString == -1) firstisString = 0;
            e = search_element(lisp, e_upto, from + firstisString);
            if (e == null_)
                return emptylist_;
            upto = e->asInteger();
            break;
        }
        case t_plus_string: {
            if (firstisString == -1) firstisString = 0;
            e = search_element(lisp, e_upto, from + firstisString);
            if (e == null_)
                return emptylist_;
            //All characters are integrated
            upto = e->asInteger() + 1;
            break;
        }
        case t_minus_string: {
            e = search_reverse(lisp, e_upto, 0);
            if (e == minusone_)
                return emptylist_;
            upto = e->asInteger();
            break;
        }
        case t_minus_plus_string: {
            e = search_reverse(lisp, e_upto, 0);
            if (e == minusone_)
                return emptylist_;
            upto = e->asInteger() - 1;
            break;
        }
        case t_float:
        case t_short:
        case t_integer:
        case t_number:
            upto = e_upto->asInteger();
            if (firstisString != -1 && upto > 0) {
                //in this case upto is a number of characters, not a position
                upto += from + firstisString;
            }
            else {
                if (upto <= 0) {
                    //We start from the end...
                    upto = size() + upto;
                }
            }
            break;
        default:
            e->release();
            e_upto->release();
            throw new Error("Error: cannot use the second position in 'extract'");
    }
    
    e->release();
    e_upto->release();
    if (upto <= from)
        return emptylist_;
    
    if (upto > size())
        upto = size();
    l = lisp->provideList();
    for (;from < upto; from++)
        l->append(liste[from]->copying(false));
    return l;
}

Element* LList::extraction(LispE* lisp, List* l) {
    Element* e_from = l->liste[2];
    Element* e;
    
    long from = 0;
    long firstisString = -1;
    short nxt = 3;
    short ty;
    switch (e_from->label()) {
        case l_minus:
            e_from = l->liste[3]->eval(lisp);
            nxt = 4;
            ty = e_from->type;
            if (ty == t_string)
                ty = t_minus_string;
            else
                throw new Error("Error: Wrong value after first operator: '-'");
            break;
        case l_plus:
            e_from = l->liste[3]->eval(lisp);
            nxt = 4;
            ty = e_from->type;
            if (ty == t_string)
                ty = t_plus_string;
            else
                throw new Error("Error: Wrong value after first operator: '+'");
            break;
        case l_minus_plus:
            e_from = l->liste[3]->eval(lisp);
            nxt = 4;
            ty = e_from->type;
            if (ty == t_string)
                ty = t_minus_plus_string;
            else
                throw new Error("Error: Wrong value after first operator: '-+'");
            break;
        default:
            e_from = e_from->eval(lisp);
            ty = e_from->type;
    }
    
    e = null_;
    switch (ty) {
        case t_string: {
            e = search_element(lisp, e_from, 0);
            if (e == null_)
                return emptylist_;
            from = e->asInteger();
            firstisString = 0;
            break;
        }
        case t_plus_string: {
            e = search_element(lisp, e_from, 0);
            if (e == null_)
                return emptylist_;
            firstisString = e->asInteger();
            break;
        }
        case t_minus_string: {
            e = search_reverse(lisp, e_from, 0);
            if (e == minusone_)
                return emptylist_;
            //We skip the first characters
            from = e->asInteger() + 1;
            firstisString = 0;
            break;
        }
        case t_minus_plus_string: {
            e = search_reverse(lisp, e_from, 0);
            if (e == minusone_)
                return emptylist_;
            firstisString = e->asInteger();
            break;
        }
        case t_float:
        case t_short:
        case t_integer:
        case t_number:
            from = e_from->asInteger();
            break;
        default:
            e->release();
            e_from->release();
            throw new Error("Error: cannot use the first position in 'extract'");
    }
    
    e->release();
    e_from->release();
    
    if (nxt == l->size()) {
        //Only one element is returned
        if (from < 0)
            e = liste.b_at_e(from*-1);
        else
            e = at_e(from);
        
        if (e == NULL)
            return emptylist_;
        
        return e->copying(false);
    }
    
    Element* e_upto = l->liste[nxt];
    switch (e_upto->label()) {
        case l_minus:
            e_upto = l->liste[nxt+1]->eval(lisp);
            ty = e_upto->type;
            if (ty == t_string)
                ty = t_minus_string;
            else
                throw new Error("Error: Wrong value after second operator: '-'");
            break;
        case l_plus:
            e_upto = l->liste[nxt+1]->eval(lisp);
            ty = e_upto->type;
            if (ty == t_string)
                ty = t_plus_string;
            else
                throw new Error("Error: Wrong value after second operator: '+'");
            break;
        case l_minus_plus:
            e_upto = l->liste[nxt+1]->eval(lisp);
            ty = e_from->type;
            if (ty == t_string)
                ty = t_minus_plus_string;
            else
                throw new Error("Error: Wrong value after second operator: '-+'");
            break;
        default:
            e_upto = e_upto->eval(lisp);
            ty = e_upto->type;
    }
    
    long upto;
    e = null_;
    long sz = size();
    switch (ty) {
        case t_string: {
            if (firstisString == -1) firstisString = 0;
            e = search_element(lisp, e_upto, from + firstisString);
            if (e == null_)
                return emptylist_;
            upto = e->asInteger();
            break;
        }
        case t_plus_string: {
            if (firstisString == -1) firstisString = 0;
            e = search_element(lisp, e_upto, from + firstisString);
            if (e == null_)
                return emptylist_;
            //All characters are integrated
            upto = e->asInteger() + 1;
            break;
        }
        case t_minus_string: {
            e = search_reverse(lisp, e_upto, 0);
            if (e == minusone_)
                return emptylist_;
            upto = e->asInteger();
            break;
        }
        case t_minus_plus_string: {
            e = search_reverse(lisp, e_upto, 0);
            if (e == minusone_)
                return emptylist_;
            upto = e->asInteger() - 1;
            break;
        }
        case t_float:
        case t_short:
        case t_integer:
        case t_number:
            upto = e_upto->asInteger();
            if (firstisString != -1 && upto > 0) {
                //in this case upto is a number of characters, not a position
                upto += from + firstisString;
            }
            else {
                if (upto <= 0) {
                    //We start from the end...
                    upto = sz + upto;
                }
            }
            break;
        default:
            e->release();
            e_upto->release();
            throw new Error("Error: cannot use the second position in 'extract'");
    }
    
    e->release();
    e_upto->release();

    if (upto <= from)
        return emptylist_;
    
    if (upto > sz)
        upto = sz;
    
    LList* ll = new LList(liste.mark);
    u_link*  it = liste.at(upto - 1);
    for (;it != NULL && upto != from; it = it->previous(), upto--) {
        ll->push_front(it->value->copying(false));
    }
    return ll;
}

Element* List::replace_in(LispE* lisp, List* l) {
    Element* e_from = l->liste[2];
    Element* e;
    Element* last = l->liste.back();
    last = last->eval(lisp);
    Element* e_upto;
    
    long from = 0;
    long upto;

    long firstisString = -1;
    short nxt = 3;
    short ty;
    try {
        switch (e_from->label()) {
            case l_minus:
                e_from = l->liste[3]->eval(lisp);
                nxt = 4;
                ty = e_from->type;
                if (ty == t_string)
                    ty = t_minus_string;
                else
                    throw new Error("Error: Wrong value after first operator: '-'");
                break;
            case l_plus:
                e_from = l->liste[3]->eval(lisp);
                nxt = 4;
                ty = e_from->type;
                if (ty == t_string)
                    ty = t_plus_string;
                else
                    throw new Error("Error: Wrong value after first operator: '+'");
                break;
            case l_minus_plus:
                e_from = l->liste[3]->eval(lisp);
                nxt = 4;
                ty = e_from->type;
                if (ty == t_string)
                    ty = t_minus_plus_string;
                else
                    throw new Error("Error: Wrong value after first operator: '-+'");
                break;
            default:
                e_from = e_from->eval(lisp);
                ty = e_from->type;
        }
        
        e = null_;
        switch (ty) {
            case t_string: {
                e = search_element(lisp, e_from, 0);
                if (e == null_) {
                    last->release();
                    return emptylist_;
                }
                from = e->asInteger();
                firstisString = 0;
                break;
            }
            case t_plus_string: {
                e = search_element(lisp, e_from, 0);
                if (e == null_) {
                    last->release();
                    return emptylist_;
                }
                firstisString = e->asInteger();
                break;
            }
            case t_minus_string: {
                e = search_reverse(lisp, e_from, 0);
                if (e == minusone_) {
                    last->release();
                    return emptylist_;
                }
                //We skip the first characters
                from = e->asInteger() + 1;
                firstisString = 0;
                break;
            }
            case t_minus_plus_string: {
                e = search_reverse(lisp, e_from, 0);
                if (e == minusone_) {
                    last->release();
                    return emptylist_;
                }
                firstisString = e->asInteger();
                break;
            }
            case t_float:
            case t_short:
            case t_integer:
            case t_number:
                from = e_from->asInteger();
                if (from < 0)
                    from = size() + from;
                break;
            default:
                e->release();
                e_from->release();
                throw new Error("Error: cannot use the first position in 'setrange'");
        }
        
        e->release();
        e_from->release();
        
        if (from < 0 || from >= size()) {
            last->release();
            return this;
        }
        
        if (nxt == l->size() - 1) {
            //We replace our element in place at e_from
            List* l = (List*)fullcopy();
            return l->replace(lisp, from, last->copying(false));
        }
        
        e_upto = l->liste[nxt];
        switch (e_upto->label()) {
            case l_minus:
                e_upto = l->liste[nxt+1]->eval(lisp);
                ty = e_upto->type;
                if (ty == t_string)
                    ty = t_minus_string;
                else
                    throw new Error("Error: Wrong value after second operator: '-'");
                break;
            case l_plus:
                e_upto = l->liste[nxt+1]->eval(lisp);
                ty = e_upto->type;
                if (ty == t_string)
                    ty = t_plus_string;
                else
                    throw new Error("Error: Wrong value after second operator: '+'");
                break;
            case l_minus_plus:
                e_upto = l->liste[nxt+1]->eval(lisp);
                ty = e_from->type;
                if (ty == t_string)
                    ty = t_minus_plus_string;
                else
                    throw new Error("Error: Wrong value after second operator: '-+'");
                break;
            default:
                e_upto = e_upto->eval(lisp);
                ty = e_upto->type;
        }
        
        e = null_;
        switch (ty) {
            case t_string: {
                if (firstisString == -1) firstisString = 0;
                e = search_element(lisp, e_upto, from + firstisString);
                if (e == null_) {
                    last->release();
                    return emptylist_;
                }
                upto = e->asInteger();
                break;
            }
            case t_plus_string: {
                if (firstisString == -1) firstisString = 0;
                e = search_element(lisp, e_upto, from + firstisString);
                if (e == null_) {
                    last->release();
                    return emptylist_;
                }
                //All characters are integrated
                upto = e->asInteger() + 1;
                break;
            }
            case t_minus_string: {
                e = search_reverse(lisp, e_upto, 0);
                if (e == minusone_) {
                    last->release();
                    return emptylist_;
                }
                upto = e->asInteger();
                break;
            }
            case t_minus_plus_string: {
                e = search_reverse(lisp, e_upto, 0);
                if (e == minusone_) {
                    last->release();
                    return emptylist_;
                }
                upto = e->asInteger() - 1;
                break;
            }
            case t_float:
            case t_short:
            case t_integer:
            case t_number:
                upto = e_upto->asInteger();
                if (firstisString != -1 && upto > 0) {
                    //in this case upto is a number of characters, not a position
                    upto += from + firstisString;
                }
                else {
                    if (upto <= 0) {
                        //We start from the end...
                        upto = size() + upto;
                    }
                }
                break;
            default:
                e->release();
                e_upto->release();
                throw new Error("Error: cannot use the second position in 'setrange'");
        }
    }
    catch (Error* err) {
        last->release();
        throw err;
    }
    
    e->release();
    e_upto->release();
    
    if (upto <= from) {
        last->release();
        return this;
    }
    
    if (upto > size())
        upto = size();
    l = lisp->provideList();
    long i = 0;
    for (i = 0; i < from; i++)
        l->append(liste[i]->copying(false));
    l->append(last->copying(false));
    for (i = upto; i < size(); i++)
        l->append(liste[i]->copying(false));
    return l;
}

Element* LList::replace_in(LispE* lisp, List* l) {
    Element* e_from = l->liste[2];
    Element* e;
    Element* last = l->liste.back();
    last = last->eval(lisp);
    Element* e_upto;
    
    long from = 0;
    long upto;

    long firstisString = -1;
    short nxt = 3;
    short ty;
    long sz = size();
    try {
        switch (e_from->label()) {
            case l_minus:
                e_from = l->liste[3]->eval(lisp);
                nxt = 4;
                ty = e_from->type;
                if (ty == t_string)
                    ty = t_minus_string;
                else
                    throw new Error("Error: Wrong value after first operator: '-'");
                break;
            case l_plus:
                e_from = l->liste[3]->eval(lisp);
                nxt = 4;
                ty = e_from->type;
                if (ty == t_string)
                    ty = t_plus_string;
                else
                    throw new Error("Error: Wrong value after first operator: '+'");
                break;
            case l_minus_plus:
                e_from = l->liste[3]->eval(lisp);
                nxt = 4;
                ty = e_from->type;
                if (ty == t_string)
                    ty = t_minus_plus_string;
                else
                    throw new Error("Error: Wrong value after first operator: '-+'");
                break;
            default:
                e_from = e_from->eval(lisp);
                ty = e_from->type;
        }
        
        e = null_;
        switch (ty) {
            case t_string: {
                e = search_element(lisp, e_from, 0);
                if (e == null_) {
                    last->release();
                    return emptylist_;
                }
                from = e->asInteger();
                firstisString = 0;
                break;
            }
            case t_plus_string: {
                e = search_element(lisp, e_from, 0);
                if (e == null_) {
                    last->release();
                    return emptylist_;
                }
                firstisString = e->asInteger();
                break;
            }
            case t_minus_string: {
                e = search_reverse(lisp, e_from, 0);
                if (e == minusone_) {
                    last->release();
                    return emptylist_;
                }
                //We skip the first characters
                from = e->asInteger() + 1;
                firstisString = 0;
                break;
            }
            case t_minus_plus_string: {
                e = search_reverse(lisp, e_from, 0);
                if (e == minusone_) {
                    last->release();
                    return emptylist_;
                }
                firstisString = e->asInteger();
                break;
            }
            case t_float:
            case t_short:
            case t_integer:
            case t_number:
                from = e_from->asInteger();
                if (from < 0)
                    from = sz + from;
                break;
            default:
                e->release();
                e_from->release();
                throw new Error("Error: cannot use the first position in 'setrange'");
        }
        
        e->release();
        e_from->release();
        
        if (from < 0 || from >= sz) {
            last->release();
            return this;
        }
        
        if (nxt == l->size() - 1) {
            //We replace our element in place at e_from
            LList* l = (LList*)fullcopy();
            return l->replace(lisp, from, last->copying(false));
        }
        
        e_upto = l->liste[nxt];
        switch (e_upto->label()) {
            case l_minus:
                e_upto = l->liste[nxt+1]->eval(lisp);
                ty = e_upto->type;
                if (ty == t_string)
                    ty = t_minus_string;
                else
                    throw new Error("Error: Wrong value after second operator: '-'");
                break;
            case l_plus:
                e_upto = l->liste[nxt+1]->eval(lisp);
                ty = e_upto->type;
                if (ty == t_string)
                    ty = t_plus_string;
                else
                    throw new Error("Error: Wrong value after second operator: '+'");
                break;
            case l_minus_plus:
                e_upto = l->liste[nxt+1]->eval(lisp);
                ty = e_from->type;
                if (ty == t_string)
                    ty = t_minus_plus_string;
                else
                    throw new Error("Error: Wrong value after second operator: '-+'");
                break;
            default:
                e_upto = e_upto->eval(lisp);
                ty = e_upto->type;
        }
        
        e = null_;
        switch (ty) {
            case t_string: {
                if (firstisString == -1) firstisString = 0;
                e = search_element(lisp, e_upto, from + firstisString);
                if (e == null_) {
                    last->release();
                    return emptylist_;
                }
                upto = e->asInteger();
                break;
            }
            case t_plus_string: {
                if (firstisString == -1) firstisString = 0;
                e = search_element(lisp, e_upto, from + firstisString);
                if (e == null_) {
                    last->release();
                    return emptylist_;
                }
                //All characters are integrated
                upto = e->asInteger() + 1;
                break;
            }
            case t_minus_string: {
                e = search_reverse(lisp, e_upto, 0);
                if (e == minusone_) {
                    last->release();
                    return emptylist_;
                }
                upto = e->asInteger();
                break;
            }
            case t_minus_plus_string: {
                e = search_reverse(lisp, e_upto, 0);
                if (e == minusone_) {
                    last->release();
                    return emptylist_;
                }
                upto = e->asInteger() - 1;
                break;
            }
            case t_float:
            case t_short:
            case t_integer:
            case t_number:
                upto = e_upto->asInteger();
                if (firstisString != -1 && upto > 0) {
                    //in this case upto is a number of characters, not a position
                    upto += from + firstisString;
                }
                else {
                    if (upto <= 0) {
                        //We start from the end...
                        upto = sz + upto;
                    }
                }
                break;
            default:
                e->release();
                e_upto->release();
                throw new Error("Error: cannot use the second position in 'setrange'");
        }
    }
    catch (Error* err) {
        last->release();
        throw err;
    }
    
    e->release();
    e_upto->release();
    
    if (upto <= from) {
        last->release();
        return this;
    }
    
    LList* ll = new LList(liste.mark);
    u_link*  it;
    it = liste.last();
    for (; sz != upto && it != NULL; it = it->previous(), sz--) {
        ll->push_front(it->value->copying(false));
    }
    ll->push_front(last->copying(false));

    while (it != NULL && sz != from) {
        sz--;
        it = it->previous();
    }
    
    for (; it != NULL; it = it->previous())
        ll->push_front(it->value->copying(false));
    return ll;
}

Element* List::duplicate_constant(bool pair) {
    if (status == s_constant) {
        List* l;
        if (pair)
            l =  new Pair();
        else
            l = (List*)newInstance();
        for (long i = 0; i < liste.size(); i++) {
            l->append(liste[i]->copying(false));
        }
        return l;
    }
    return this;
}

Element* LList::duplicate_constant(bool pair) {
    if (status == s_constant)
        return back_duplicate();
    return this;
}

Element* LList::asList(LispE* lisp) {
    List* l =  lisp->provideList();
    for (u_link* a = liste.begin(); a != NULL; a = a->next())
        l->append(a->value);
    return l;
}

Element* List::cadr(LispE* lisp, u_ustring& action) {
    long pos = 0;
    long sz = size();
    Element* e = this;
    bool pair = (e->type == t_pair);
    
    for (long i = action.size() - 1; i>= 0; i--) {
        if (action[i] == 'a') {
            e = e->protected_index(lisp, pos);
            if (e == null_)
                throw new Error("Error: You reached the list size limit");
            
            pair = (e->type == t_pair);
            sz = e->size();
            pos = 0;
        }
        else {
            if (pos == sz)
                throw new Error("Error: You reached the list size limit");
            pos++;
        }
    }
    
    if (pos) {
        if (pos == sz)
            return null_;
        if (pair) {
            //The last one...
            if (pos == sz - 1)
                return e->index(pos);
            return new Pair((Pair*)e, pos);
        }
        else {
            return new Listpool(lisp, (List*)e, pos);
        }
    }
    
    return e;
}

Element* LList::cadr(LispE* lisp, u_ustring& action) {
    u_link* it = liste.first;
    Element* e = this;
    
    for (long i = action.size() - 1; i>= 0; i--) {
        if (action[i] == 'a') {
            if (it == NULL)
                throw new Error("Error: You reached the list size limit");

            e = it->value;
            if (i == 0)
                return e;
            u_ustring act = action.substr(0, i);
            return e->cadr(lisp, act);
        }
        else {
            if (it == NULL)
                throw new Error("Error: You reached the list size limit");
            //We do not try to detect a cycle here...
            it = it->_next;
        }
    }
    
    if (it != NULL)
        return new LList(this, it);
    
    return e;
}

Element* List::car(LispE* lisp) {
    if (liste.size() == 0)
        return null_;
    return liste[0];
}

Element* List::cdr(LispE* lisp) {
    if (liste.size() <= 1)
        return null_;
    return new Listpool(lisp, this, 1);
}

Element* LList::car(LispE* lisp) {
    if (liste.empty())
        return null_;
    return liste.front();
}

Element* LList::cdr(LispE* lisp) {
    u_link* it = liste.first;
    if (it == NULL || it->_next == NULL)
        return null_;
    it = it->_next;
    return new LList(this, it);
}

//--------------------------------------------------------------------------------
//Numbers methods
//--------------------------------------------------------------------------------

Element* Numbers::check_member(LispE* lisp, Element* the_set) {
    Numbers* n = lisp->provideNumbers();
    double v;
    long i, j;
    long sz = the_set->size();
    for (j = 0; j < size(); j++) {
        for (i = 0; i < sz; i++) {
            v = the_set->index(i)->asNumber();
            if (liste[j] == v) {
                n->liste.push_back(1);
                break;
            }
        }
        if (i == sz)
            n->liste.push_back(0);
    }
    return n;
}

bool Numbers::compare(LispE* lisp, List* comparison, short instruction, long i, long j) {
    ((Number*)comparison->liste[1])->number = liste[i];
    ((Number*)comparison->liste[2])->number = liste[j];
    return comparison->eval_Boolean(lisp, instruction);
}

void Numbers::sorting(LispE* lisp, List* comparison, short instruction, long rmin, long rmax) {
    //(setq s (sort '< (shuffle (cons 5 (range 1 99999 1)))))
    //(sort '< '(28 60 10 38 80 34 8 22 78 68 85 48 13 39 100 56 89 82 11 52 99 50 20 96 97 59 23 81 53 15 3 67 77 7 57 74 49 32 86 66 43 26 75 62 29 71 2 91 51 1 18 12 24 21 36 72 90 40 70 14 61 93 6 4 79 94 47 58 30 83 84 44 88 63 95 45 33 65 37 92 27 64 55 9 31 73 54 16 98 5 46 25 76 42 17 69 19 35 5 41 87))
    //(sort '< '(20 12 15 13 19 17 14))
    //(sort '< (shuffle (range 1 16 1)))
    //(sort '< '(4 3 7 1 5))
    //(sort '< '(10 4 8 5 12 2 6 11 3 9 7 9))
    
    //check sorting stability
    //(loop i (range 1 9999 1) (select (<= (at s i) (at s (+ i 1))) (println 'erreur i)))
    
    long j = rmax-rmin+1;
    long pivot;
    
    if (j < 7) {
        if (j < 2)
            return;
        
        if (j == 2) {
            if (compare(lisp, comparison, instruction, rmax, rmin))
                swap(rmax, rmin);
            return;
        }
        
        if (j == 3) {
            if (compare(lisp, comparison, instruction, rmin, rmin + 1)) {
                if (compare(lisp, comparison, instruction, rmin + 1, rmax))
                    return;
            }
            else {
                swap(rmin, rmin + 1);
                if (compare(lisp, comparison, instruction, rmin + 1, rmax))
                    return;
            }
            swap(rmax, rmin + 1);
            if (compare(lisp, comparison, instruction, rmin, rmin + 1))
                return;
            swap(rmin, rmin + 1);
            return;
        }
        
        long sz;
        while (rmax > rmin) {
            sz = rmin;
            for (j = rmin; j < rmax; j++) {
                if (compare(lisp, comparison, instruction, j + 1, j)) {
                    swap(j, j + 1);
                    sz = j;
                    pivot = j;
                    while (pivot > rmin && compare(lisp, comparison, instruction, pivot, pivot - 1))
                        swap(pivot, pivot - 1);
                }
            }
            rmax = sz;
        }
        return;
    }
    
    pivot = rmin - 1;
    ((Number*)comparison->liste[2])->number = liste[rmax];
    for (j = rmin; j < rmax; j++) {
        ((Number*)comparison->liste[1])->number = liste[j];
        if (comparison->eval_Boolean(lisp, instruction)) {
            pivot++;
            swap(pivot,j);
        }
    }
    pivot++;
    swap(pivot, rmax);
    
    sorting(lisp, comparison, instruction, rmin, pivot-1);
    sorting(lisp, comparison, instruction, pivot+1, rmax);
}

void Numbers::sorting(LispE* lisp, List* comparison) {
    //We sort between home and last...
    long sz = size();
    if (sz <= 1)
        return;
    
    Constnumber n1(0);
    Constnumber n2(0);
    comparison->liste[1] = &n1;
    comparison->liste[2] = &n2;
    
    n1.number = liste[0];
    n2.number = liste[0];
    if (comparison->eval(lisp)->Boolean())
        throw new Error(L"Error: The comparison must be strict for a 'sort': (comp a a) must return 'nil'.");
    
    sorting(lisp, comparison, comparison->liste[0]->type, 0, sz-1);
}

Element* Numbers::minimum(LispE* lisp) {
    if (!liste.size())
        return null_;
    double v = liste[0];
    for (long i = 1; i < liste.size(); i++) {
        if (v > liste[i])
            v = liste[i];
    }
    return lisp->provideNumber(v);
}

Element* Numbers::maximum(LispE* lisp) {
    if (!liste.size())
        return null_;
    double v = liste[0];
    for (long i = 1; i < liste.size(); i++) {
        if (v < liste[i])
            v = liste[i];
    }
    return lisp->provideNumber(v);
}

Element* Numbers::minmax(LispE* lisp) {
    if (!liste.size())
        return null_;
    double v_min = liste[0];
    double v_max = liste[0];
    for (long i = 1; i < liste.size(); i++) {
        if (v_min > liste[i])
            v_min = liste[i];
        else {
            if (v_max < liste[i])
                v_max = liste[i];
        }
    }
    Numbers* f = lisp->provideNumbers();
    f->liste.push_back(v_min);
    f->liste.push_back(v_max);
    return f;
}

void Numbers::flatten(LispE* lisp, List* l) {
    for (long i = 0; i < size(); i++) {
        l->append(lisp->provideNumber(liste[i]));
    }
}

void Numbers::flatten(LispE* lisp, Numbers* l) {
    for (long i = 0; i < size(); i++) {
        l->liste.push_back(liste[i]);
    }
}

void Numbers::flatten(LispE* lisp, Floats* l) {
    for (long i = 0; i < size(); i++) {
        l->liste.push_back(liste[i]);
    }
}

void Numbers::storevalue(LispE* lisp, double v) {
    liste.push_back(v);
}

void Numbers::storevalue(LispE* lisp,long v) {
    liste.push_back(v);
}

void Numbers::storevalue(LispE* lisp, u_ustring& s) {
    long l;
    double v = convertingfloathexa((u_uchar*)s.c_str(), l);
    liste.push_back(v);
}

Element* Numbers::invert_sign(LispE* lisp) {
    Numbers* n = this;
    if (status)
        n = lisp->provideNumbers(this);
    
    for (long i = 0; i < n->liste.size(); i++)
        n->liste[i] *= -1;
    return n;
}

void Numbers::append(LispE* lisp, u_ustring& k) {
    long l;
    double d = convertingfloathexa((u_uchar*)k.c_str(), l);
    liste.push_back(d);
}

void Numbers::append(LispE* lisp, double v) {
    liste.push_back(v);
}

void Numbers::append(LispE* lisp, long v) {
    liste.push_back(v);
}

Element* Numbers::loop(LispE* lisp, short label, List* code) {
    long i_loop;
    Element* e = null_;
    Number* element;
    lisp->recording(null_, label);
    long sz = code->liste.size();
    for (long i = 0; i < liste.size(); i++) {
        element = lisp->provideNumber(liste[i]);
        lisp->replacingvalue(element, label);
        _releasing(e);
        //We then execute our instructions
        for (i_loop = 3; i_loop < sz && e->type != l_return; i_loop++) {
            e->release();
            e = code->liste[i_loop]->eval(lisp);
        }
        if (e->type == l_return) {
            if (e->isBreak())
                return null_;
            return e;
        }
    }
    return e;
}

Element* Numbers::insert(LispE* lisp, Element* e, long ix) {
    if (ix < 0)
        throw new Error("Error: Wrong index in 'insert'");
    
    Numbers* l = (Numbers*)duplicate_constant();
    l->liste.insert(ix, e->asNumber());
    return l;
}

Element* Numbers::rotate(bool left) {
    if (liste.size() <= 1)
        return this;
    
    Numbers* l = (Numbers*)newInstance();
    if (left) {
        for (long i = 1; i < liste.size(); i++)
            l->liste.push_back(liste[i]);
        l->liste.push_back(liste[0]);
        return l;
    }
    
    l->liste.push_back(liste.back());
    for (long i = 0; i < liste.size() - 1; i ++)
        l->liste.push_back(liste[i]);
    return l;
}

Element* Numbers::unique(LispE* lisp) {
    if (liste.size() == 0)
        return this;
    
    Numbers* nb = lisp->provideNumbers();
    long i, j;
    bool found;
    nb->liste.push_back(liste[0]);
    for (i = 1; i < liste.size(); i++) {
        found = false;
        for (j = 0; j < nb->liste.size(); j++) {
            if (liste[i] == nb->liste[j]) {
                found = true;
                break;
            }
        }
        if (!found)
            nb->liste.push_back(liste[i]);
    }
    return nb;
}

Element* Numbers::thekeys(LispE* lisp) {
    Integers* keys = lisp->provideIntegers();
    for (long i = 0; i< size(); i++) {
        keys->liste.push_back(i);
    }
    return keys;
}

Element* Numbers::search_element(LispE* lisp, Element* valeur, long ix) {
    double v = valeur->asNumber();
    for (long i = ix; i < liste.size(); i++) {
        if (liste[i] == v)
            return lisp->provideInteger(i);
    }
    return null_;
}

bool Numbers::check_element(LispE* lisp, Element* valeur) {
    double v = valeur->asNumber();
    for (long i = 0; i < liste.size(); i++) {
        if (liste[i] == v)
            return true;
    }
    return false;
}

Element* Numbers::replace_all_elements(LispE* lisp, Element* valeur, Element* remp) {
    long nb = 0;
    double v = valeur->asNumber();
    double r = remp->asNumber();
    long sz = liste.size();
    for (long i = 0; i < sz; i++) {
        if (liste[i] == v) {
            liste[i] = r;
            nb++;
        }
    }
    return lisp->provideInteger(nb);
}

Element* Numbers::search_all_elements(LispE* lisp, Element* valeur, long ix) {
    Integers* l = lisp->provideIntegers();
    double v = valeur->asNumber();
    long sz = liste.size();
    for (long i = ix; i < sz; i++) {
        if (liste[i] == v)
            l->liste.push_back(i);
    }
    if (l->liste.size() == 0) {
        l->release();
        return emptylist_;
    }
    return l;
}

Element* Numbers::count_all_elements(LispE* lisp, Element* valeur, long ix) {
    long nb = 0;
    double v = valeur->asNumber();
    long sz = liste.size();
    for (long i = ix; i < sz; i++) {
        if (liste[i] == v)
            nb++;
    }
    return lisp->provideInteger(nb);
}

Element* Numbers::list_and(LispE* lisp, Element* value) {
    if (!value->isList() && !value->isSet())
        throw new Error("Error: Can only apply '&&&' to strings, lists or sets");
    
    Numbers* l = lisp->provideNumbers();
    long sz = liste.size();
    for (long i = 0; i < sz; i++) {
        if (!l->liste.check(liste[i]) && value->check_element(lisp, index(i)))
            l->liste.push_back(liste[i]);
    }
    
    if (l->liste.size() == 0) {
        l->release();
        return emptylist_;
    }
    return l;
}

Element* Numbers::list_or(LispE* lisp, Element* value) {
    if (!value->isList() && !value->isSet())
        throw new Error("Error: Can only apply '|||' to strings, lists or sets");
    
    Numbers* l = lisp->provideNumbers();
    long sz = liste.size();
    long i;
    for (i = 0; i < sz; i++) {
        if (!l->liste.check(liste[i]))
            l->liste.push_back(liste[i]);
    }

    if (value->type == t_llist) {
        u_link* a = ((LList*)value)->liste.begin();
        for (; a != NULL; a = a->next()) {
            for (i = 0; i < l->size(); i++) {
                if (l->liste[i] == a->value->asNumber())
                    break;
            }
            if (i == l->size())
                l->liste.push_back(a->value->asNumber());
        }
    }
    else {
        if (value->isList()) {
            long sz = value->size();
            for (long j = 0; j < sz; j++) {
                for (i = 0; i < l->size(); i++) {
                    if (l->liste[i] == value->index(j)->asNumber())
                        break;
                }
                if (i == l->size())
                    l->liste.push_back(value->index(j)->asNumber());
            }
        }
        else {
            if (value->isSet()) {
                void* iter = value->begin_iter();
                Element* next_value = value->next_iter_exchange(lisp, iter);
                while (next_value != emptyatom_) {
                    for (i = 0; i < l->size(); i++) {
                        if (l->liste[i] == next_value->asNumber())
                            break;
                    }
                    if (i == l->size())
                        l->liste.push_back(next_value->asNumber());
                    next_value = value->next_iter_exchange(lisp, iter);
                }
                value->clean_iter(iter);
            }
        }
    }

    if (l->liste.size() == 0) {
        l->release();
        return emptylist_;
    }
    return l;
}

Element* Numbers::list_xor(LispE* lisp, Element* value) {
    if (!value->isList() && !value->isSet())
        throw new Error("Error: Can only apply '^^^' to strings, lists or sets");
    
    Numbers* l = lisp->provideNumbers();
    Numbers* intersection = (Numbers*)list_and(lisp, value);
    long sz = liste.size();
    long i;
    for (i = 0; i < sz; i++) {
        if (!intersection->liste.check(liste[i]) && !l->liste.check(liste[i]))
            l->liste.push_back(liste[i]);
    }

    double v;
    if (value->type == t_llist) {
        u_link* a = ((LList*)value)->liste.begin();
        for (; a != NULL; a = a->next()) {
            v = a->value->asNumber();
            if (!intersection->liste.check(v) && !l->liste.check(v))
                l->liste.push_back(v);
        }
    }
    else {
        if (value->isList()) {
            long sz = value->size();
            for (i = 0; i < sz; i++) {
                v = value->index(i)->asNumber();
                if (!intersection->liste.check(v) && !l->liste.check(v))
                    l->liste.push_back(v);
            }
        }
        else {
            if (value->isSet()) {
                void* iter = value->begin_iter();
                Element* next_value = value->next_iter_exchange(lisp, iter);
                while (next_value != emptyatom_) {
                    v = next_value->asNumber();
                    if (!intersection->liste.check(v) && !l->liste.check(v))
                        l->liste.push_back(v);
                    next_value = value->next_iter_exchange(lisp, iter);
                }
                value->clean_iter(iter);
            }
        }
    }
    intersection->release();
    
    if (l->liste.size() == 0) {
        l->release();
        return emptylist_;
    }
    return l;
}

Element* Numbers::search_reverse(LispE* lisp, Element* valeur, long ix) {
    double v = valeur->asNumber();
    for (long i = liste.size() - 1; i >= ix; i--) {
        if (liste[i] == v)
            return lisp->provideInteger(i);
    }
    return minusone_;
}

Element* Numbers::reverse(LispE* lisp, bool duplicate) {
    if (liste.size() <= 1)
        return this;
    
    if (duplicate) {
        Numbers* l = lisp->provideNumbers();
        for (long i = liste.size()-1; i >= 0; i--) {
            l->liste.push_back(liste[i]);
        }
        return l;
    }
    
    liste.reverse();
    return this;
}

Element* Numbers::protected_index(LispE* lisp,long i) {
    if (i >= 0 && i < liste.size())
        return lisp->provideNumber(liste[i]);
    return null_;
}

Element* Numbers::last_element(LispE* lisp) {
    if (!liste.size())
        return null_;
    return lisp->provideNumber(liste.back());
}

Element* Numbers::value_on_index(LispE* lisp, long i) {
    if (i >= 0 && i < liste.size())
        return lisp->provideNumber(liste[i]);
    return null_;
}

Element* Numbers::value_from_index(LispE* lisp, long i) {
    return lisp->provideNumber(liste[i]);
}

Element* Numbers::value_on_index(LispE* lisp, Element* ix) {
    long i = ix->checkInteger(lisp);
    if (i < 0)
        i = liste.size() + i;
    
    if (i >= 0 && i < liste.size())
        return lisp->provideNumber(liste[i]);
    
    return null_;
}

Element* Numbers::protected_index(LispE* lisp, Element* ix) {
    long i = ix->checkInteger(lisp);
    if (i < 0)
        i = liste.size() + i;
    
    if (i >= 0 && i < liste.size())
        return lisp->provideNumber(liste[i]);
    
    throw new Error("Error: index out of bounds");
}

Element* Numbers::join_in_list(LispE* lisp, u_ustring& sep) {
    u_ustring str;
    u_ustring beg;
    for (long i = 0; i < liste.size(); i++) {
        str += beg;
        beg = sep;
        str += convertToUString(liste[i]);
    }
    return lisp->provideString(str);
}

Element* Numbers::equal(LispE* lisp, Element* e) {
    return booleans_[(e->type == t_numbers && liste == ((Numbers*)e)->liste)];
}

bool Numbers::egal(Element* e) {
    return (e->type == t_numbers && liste == ((Numbers*)e)->liste);
}

Element* Numbers::extraction(LispE* lisp, List* l) {
    long depuis;
    l->evalAsInteger(2, lisp, depuis);
    if (depuis >= 0) {
        if (depuis >= liste.size())
            return emptylist_;
    }
    else {
        //We start from the end...
        depuis = liste.size() + depuis;
        if (depuis < 0)
            return emptylist_;
    }
    if (l->size() == 3) {
        //On returns only one element
        return lisp->provideNumber(liste[depuis]);
    }
    long upto;
    l->evalAsInteger(3, lisp, upto);
    if (upto >= 0) {
        if (upto >= liste.size())
            upto = liste.size();
    }
    else {
        //We start from the end...
        upto = liste.size() + upto;
        if (upto < 0)
            return emptylist_;
    }
    if (upto < depuis) {
        return emptylist_;
    }
    
    Numbers* n = lisp->provideNumbers();
    for (;depuis < upto; depuis++) {
        n->liste.push_back(liste[depuis]);
    }
    return n;
}

Element* Numbers::replace_in(LispE* lisp, List* l) {
    Element* e_last = l->liste.back()->eval(lisp);
    double last = e_last->asNumber();
    e_last->release();

    long depuis;
    l->evalAsInteger(2, lisp, depuis);
    if (depuis >= 0) {
        if (depuis >= liste.size())
            return emptylist_;
    }
    else {
        //We start from the end...
        depuis = liste.size() + depuis;
        if (depuis < 0)
            return emptylist_;
    }
    if (l->size() == 4) {
        //On returns only one element
        Numbers* l = (Numbers*)fullcopy();
        l->liste[depuis] = last;
        return l;
    }
    long upto;
    l->evalAsInteger(3, lisp, upto);
    if (upto >= 0) {
        if (upto >= liste.size())
            upto = liste.size();
    }
    else {
        //We start from the end...
        upto = liste.size() + upto;
        if (upto < 0)
            return emptylist_;
    }
    if (upto < depuis) {
        return emptylist_;
    }
    
    Numbers* n = lisp->provideNumbers();
    long i;
    for (i = 0; i < depuis; i++)
        n->liste.push_back(liste[i]);
    n->liste.push_back(last);
    for (i = upto; i < size(); i++)
        n->liste.push_back(liste[i]);
    return n;
}

Element* Numbers::duplicate_constant(bool pair) {
    if (status == s_constant) {
        Numbers* l = (Numbers*)newInstance();
        l->liste = liste;
        return l;
    }
    return this;
}

Element* Numbers::asList(LispE* lisp) {
    List* l =  lisp->provideList();
    for (long i = 0; i < liste.size(); i++)
        l->append(lisp->provideNumber(liste[i]));
    return l;
}

Element* Numbers::cadr(LispE* lisp, u_ustring& action) {
    long pos = 0;
    long sz = size();
    long i;
    
    for (i = action.size() - 1; i>= 0; i--) {
        if (action[i] == 'a') {
            if (i)
                throw new Error("Error: the elements of a list of values are no lists");
            return lisp->provideNumber(liste[pos]);
        }
        if (pos == sz)
            throw new Error("Error: You reached the list size limit");
        pos++;
    }
    
    if (pos) {
        if (pos == sz)
            return null_;
        return lisp->provideNumbers(this, pos);
    }
    
    return null_;
}

Element* Numbers::car(LispE* lisp) {
    if (liste.size() == 0)
        return null_;
    return lisp->provideNumber(liste[0]);
}

Element* Numbers::cdr(LispE* lisp) {
    if (liste.size() <= 1)
        return null_;
    return lisp->provideNumbers(this, 1);
}

//--------------------------------------------------------------------------------
//Integers methods
//--------------------------------------------------------------------------------

Element* Integers::check_member(LispE* lisp, Element* the_set) {
    Integers* n = lisp->provideIntegers();
    long v;
    long i, j;
    long sz = the_set->size();
    for (j = 0; j < size(); j++) {
        for (i = 0; i < sz; i++) {
            v = the_set->index(i)->asInteger();
            if (liste[j] == v) {
                n->liste.push_back(1);
                break;
            }
        }
        if (i == sz)
            n->liste.push_back(0);
    }
    return n;
}

bool Integers::compare(LispE* lisp, List* comparison, short instruction, long i, long j) {
    ((Integer*)comparison->liste[1])->integer = liste[i];
    ((Integer*)comparison->liste[2])->integer = liste[j];
    return comparison->eval_Boolean(lisp, instruction);
}

void Integers::sorting(LispE* lisp, List* comparison, short instruction, long rmin, long rmax) {
    //(setq s (sort '< (shuffle (cons 5 (range 1 99999 1)))))
    //(sort '< '(28 60 10 38 80 34 8 22 78 68 85 48 13 39 100 56 89 82 11 52 99 50 20 96 97 59 23 81 53 15 3 67 77 7 57 74 49 32 86 66 43 26 75 62 29 71 2 91 51 1 18 12 24 21 36 72 90 40 70 14 61 93 6 4 79 94 47 58 30 83 84 44 88 63 95 45 33 65 37 92 27 64 55 9 31 73 54 16 98 5 46 25 76 42 17 69 19 35 5 41 87))
    //(sort '< '(20 12 15 13 19 17 14))
    //(sort '< (shuffle (range 1 16 1)))
    //(sort '< '(4 3 7 1 5))
    //(sort '< '(10 4 8 5 12 2 6 11 3 9 7 9))
    
    //check sorting stability
    //(loop i (range 1 9999 1) (select (<= (at s i) (at s (+ i 1))) (println 'erreur i)))
    
    long j = rmax-rmin+1;
    long pivot;
    
    if (j < 7) {
        if (j < 2)
            return;
        
        if (j == 2) {
            if (compare(lisp, comparison, instruction, rmax, rmin))
                swap(rmax, rmin);
            return;
        }
        
        if (j == 3) {
            if (compare(lisp, comparison, instruction, rmin, rmin + 1)) {
                if (compare(lisp, comparison, instruction, rmin + 1, rmax))
                    return;
            }
            else {
                swap(rmin, rmin + 1);
                if (compare(lisp, comparison, instruction, rmin + 1, rmax))
                    return;
            }
            swap(rmax, rmin + 1);
            if (compare(lisp, comparison, instruction, rmin, rmin + 1))
                return;
            swap(rmin, rmin + 1);
            return;
        }
        
        long sz;
        while (rmax > rmin) {
            sz = rmin;
            for (j = rmin; j < rmax; j++) {
                if (compare(lisp, comparison, instruction, j + 1, j)) {
                    swap(j, j + 1);
                    sz = j;
                    pivot = j;
                    while (pivot > rmin && compare(lisp, comparison, instruction, pivot, pivot - 1))
                        swap(pivot, pivot - 1);
                }
            }
            rmax = sz;
        }
        return;
    }
    
    pivot = rmin - 1;
    ((Integer*)comparison->liste[2])->integer = liste[rmax];
    for (j = rmin; j < rmax; j++) {
        ((Integer*)comparison->liste[1])->integer = liste[j];
        if (comparison->eval_Boolean(lisp, instruction)) {
            pivot++;
            swap(pivot,j);
        }
    }
    pivot++;
    swap(pivot, rmax);
    
    sorting(lisp, comparison, instruction, rmin, pivot-1);
    sorting(lisp, comparison, instruction, pivot+1, rmax);
}

void Integers::sorting(LispE* lisp, List* comparison) {
    //We sort between home and last...
    long sz = size();
    if (sz <= 1)
        return;
    
    Constinteger n1(0);
    Constinteger n2(0);
    comparison->liste[1] = &n1;
    comparison->liste[2] = &n2;
    n1.integer = liste[0];
    n2.integer = liste[0];
    if (comparison->eval(lisp)->Boolean())
        throw new Error(L"Error: The comparison must be strict for a 'sort': (comp a a) must return 'nil'.");
    
    sorting(lisp, comparison, comparison->liste[0]->type, 0, sz-1);
}

Element* Integers::minimum(LispE* lisp) {
    if (!liste.size())
        return null_;
    long v = liste[0];
    for (long i = 1; i < liste.size(); i++) {
        if (v > liste[i])
            v = liste[i];
    }
    return lisp->provideInteger(v);
}

Element* Integers::maximum(LispE* lisp) {
    if (!liste.size())
        return null_;
    long v = liste[0];
    for (long i = 1; i < liste.size(); i++) {
        if (v < liste[i])
            v = liste[i];
    }
    return lisp->provideInteger(v);
}

Element* Integers::minmax(LispE* lisp) {
    if (!liste.size())
        return null_;
    long v_min = liste[0];
    long v_max = liste[0];
    for (long i = 1; i < liste.size(); i++) {
        if (v_min > liste[i])
            v_min = liste[i];
        else {
            if (v_max < liste[i])
                v_max = liste[i];
        }
    }
    Floats* f = lisp->provideFloats();
    f->liste.push_back(v_min);
    f->liste.push_back(v_max);
    return f;
}

void Integers::flatten(LispE* lisp, List* l) {
    for (long i = 0; i < size(); i++) {
        l->append(lisp->provideInteger(liste[i]));
    }
}

void Integers::flatten(LispE* lisp, Numbers* l) {
    for (long i = 0; i < size(); i++) {
        l->liste.push_back(liste[i]);
    }
}

void Integers::flatten(LispE* lisp, Floats* l) {
    for (long i = 0; i < size(); i++) {
        l->liste.push_back(liste[i]);
    }
}

void Integers::storevalue(LispE* lisp, double v) {
    liste.push_back(v);
}

void Integers::storevalue(LispE* lisp,long v) {
    liste.push_back(v);
}

void Integers::storevalue(LispE* lisp, u_ustring& s) {
    long v = convertinginteger(s);
    liste.push_back(v);
}

Element* Integers::invert_sign(LispE* lisp) {
    Integers* n = this;
    if (status)
        n = lisp->provideIntegers(this);
    
    for (long i = 0; i < n->liste.size(); i++)
        n->liste[i] *= -1;
    return n;
}

void Integers::append(LispE* lisp, u_ustring& k) {
    long d = convertinginteger(k);
    liste.push_back(d);
}

void Integers::append(LispE* lisp, double v) {
    liste.push_back(v);
}

void Integers::append(LispE* lisp, long v) {
    liste.push_back(v);
}

Element* Integers::loop(LispE* lisp, short label, List* code) {
    long i_loop;
    Element* e = null_;
    Integer* element;
    lisp->recording(null_, label);
    long sz = code->liste.size();
    for (long i = 0; i < liste.size(); i++) {
        element = lisp->provideInteger(liste[i]);
        lisp->replacingvalue(element, label);
        _releasing(e);
        //We then execute our instructions
        for (i_loop = 3; i_loop < sz && e->type != l_return; i_loop++) {
            e->release();
            e = code->liste[i_loop]->eval(lisp);
        }
        if (e->type == l_return) {
            if (e->isBreak())
                return null_;
            return e;
        }
    }
    return e;
}

Element* Integers::insert(LispE* lisp, Element* e, long ix) {
    if (ix < 0)
        throw new Error("Error: Wrong index in 'insert'");
    
    Integers* l = (Integers*)duplicate_constant();
    l->liste.insert(ix, e->asInteger());
    return l;
}

Element* Integers::rotate(bool left) {
    if (liste.size() <= 1)
        return this;
    
    Integers* l = (Integers*)newInstance();
    if (left) {
        for (long i = 1; i < liste.size(); i++)
            l->liste.push_back(liste[i]);
        l->liste.push_back(liste[0]);
        return l;
    }
    
    l->liste.push_back(liste.back());
    for (long i = 0; i < liste.size() - 1; i ++)
        l->liste.push_back(liste[i]);
    return l;
}

Element* Integers::unique(LispE* lisp) {
    if (liste.size() == 0)
        return this;
    
    Integers* nb = lisp->provideIntegers();
    long i, j;
    bool found;
    nb->liste.push_back(liste[0]);
    for (i = 1; i < liste.size(); i++) {
        found = false;
        for (j = 0; j < nb->liste.size(); j++) {
            if (liste[i] == nb->liste[j]) {
                found = true;
                break;
            }
        }
        if (!found)
            nb->liste.push_back(liste[i]);
    }
    return nb;
}

Element* Integers::thekeys(LispE* lisp) {
    Integers* keys = lisp->provideIntegers();
    for (long i = 0; i< size(); i++) {
        keys->liste.push_back(i);
    }
    return keys;
}

Element* Integers::search_element(LispE* lisp, Element* valeur, long ix) {
    long v = valeur->asInteger();
    for (long i = ix; i < liste.size(); i++) {
        if (liste[i] == v)
            return lisp->provideInteger(i);
    }
    return null_;
}

bool Integers::check_element(LispE* lisp, Element* valeur) {
    long v = valeur->asInteger();
    for (long i = 0; i < liste.size(); i++) {
        if (liste[i] == v)
            return true;
    }
    return false;
}

Element* Integers::replace_all_elements(LispE* lisp, Element* valeur, Element* remp) {
    long nb = 0;
    long v = valeur->asInteger();
    long r = remp->asInteger();
    long sz = liste.size();
    for (long i = 0; i < sz; i++) {
        if (liste[i] == v) {
            liste[i] = r;
            nb++;
        }
    }
    return lisp->provideInteger(nb);
}

Element* Integers::search_all_elements(LispE* lisp, Element* valeur, long ix) {
    Integers* l = lisp->provideIntegers();
    long v = valeur->asInteger();
    long sz = liste.size();
    for (long i = ix; i < sz; i++) {
        if (liste[i] == v)
            l->liste.push_back(i);
    }
    if (l->liste.size() == 0) {
        l->release();
        return emptylist_;
    }
    return l;
}

Element* Integers::count_all_elements(LispE* lisp, Element* valeur, long ix) {
    long nb = 0;
    long v = valeur->asInteger();
    long sz = liste.size();
    for (long i = ix; i < sz; i++) {
        if (liste[i] == v)
            nb++;
    }
    return lisp->provideInteger(nb);
}

Element* Integers::list_and(LispE* lisp, Element* value) {
    if (!value->isList() && !value->isSet())
        throw new Error("Error: Can only apply '&&&' to strings, lists or sets");
    
    Integers* l = lisp->provideIntegers();
    long sz = liste.size();
    for (long i = 0; i < sz; i++) {
        if (!l->liste.check(liste[i]) && value->check_element(lisp, index(i)))
            l->liste.push_back(liste[i]);
    }
    if (l->liste.size() == 0) {
        l->release();
        return emptylist_;
    }
    return l;
}

Element* Integers::list_or(LispE* lisp, Element* value) {
    if (!value->isList() && !value->isSet())
        throw new Error("Error: Can only apply '|||' to strings, lists or sets");
    
    Integers* l = lisp->provideIntegers();
    long sz = liste.size();
    long i;
    for (i = 0; i < sz; i++) {
        if (!l->liste.check(liste[i]))
            l->liste.push_back(liste[i]);
    }

    if (value->type == t_llist) {
        u_link* a = ((LList*)value)->liste.begin();
        for (; a != NULL; a = a->next()) {
            for (i = 0; i < l->size(); i++) {
                if (l->liste[i] == a->value->asInteger())
                    break;
            }
            if (i == l->size())
                l->liste.push_back(a->value->asInteger());
        }
    }
    else {
        if (value->isList()) {
            long sz = value->size();
            for (long j = 0; j < sz; j++) {
                for (i = 0; i < l->size(); i++) {
                    if (l->liste[i] == value->index(j)->asInteger())
                        break;
                }
                if (i == l->size())
                    l->liste.push_back(value->index(j)->asInteger());
            }
        }
        else {
            if (value->isSet()) {
                void* iter = value->begin_iter();
                Element* next_value = value->next_iter_exchange(lisp, iter);
                while (next_value != emptyatom_) {
                    for (i = 0; i < l->size(); i++) {
                        if (l->liste[i] == next_value->asInteger())
                            break;
                    }
                    if (i == l->size())
                        l->liste.push_back(next_value->asInteger());
                    next_value = value->next_iter_exchange(lisp, iter);
                }
                value->clean_iter(iter);
            }
        }
    }
    if (l->liste.size() == 0) {
        l->release();
        return emptylist_;
    }
    return l;
}

Element* Integers::list_xor(LispE* lisp, Element* value) {
    if (!value->isList() && !value->isSet())
        throw new Error("Error: Can only apply '^^^' to strings, lists or sets");
    
    Integers* l = lisp->provideIntegers();
    Integers* intersection = (Integers*)list_and(lisp, value);
    long sz = liste.size();
    long i;
    for (i = 0; i < sz; i++) {
        if (!intersection->liste.check(liste[i]) && !l->liste.check(liste[i]))
            l->liste.push_back(liste[i]);
    }

    long v;
    if (value->type == t_llist) {
        u_link* a = ((LList*)value)->liste.begin();
        for (; a != NULL; a = a->next()) {
            v = a->value->asInteger();
            if (!intersection->liste.check(v) && !l->liste.check(v))
                l->liste.push_back(v);
        }
    }
    else {
        if (value->isList()) {
            long sz = value->size();
            for (i = 0; i < sz; i++) {
                v = value->index(i)->asInteger();
                if (!intersection->liste.check(v) && !l->liste.check(v))
                    l->liste.push_back(v);
            }
        }
        else {
            if (value->isSet()) {
                void* iter = value->begin_iter();
                Element* next_value = value->next_iter_exchange(lisp, iter);
                while (next_value != emptyatom_) {
                    v = next_value->asInteger();
                    if (!intersection->liste.check(v) && !l->liste.check(v))
                        l->liste.push_back(v);
                    next_value = value->next_iter_exchange(lisp, iter);
                }
                value->clean_iter(iter);
            }
        }
    }
    intersection->release();
    
    if (l->liste.size() == 0) {
        l->release();
        return emptylist_;
    }
    return l;
}

Element* Integers::search_reverse(LispE* lisp, Element* valeur, long ix) {
    long v = valeur->asInteger();
    for (long i = liste.size() - 1; i >= ix; i--) {
        if (liste[i] == v)
            return lisp->provideInteger(i);
    }
    return minusone_;
}

Element* Integers::reverse(LispE* lisp, bool duplicate) {
    if (liste.size() <= 1)
        return this;
    
    if (duplicate) {
        Integers* l = lisp->provideIntegers();
        for (long i = liste.size()-1; i >= 0; i--) {
            l->liste.push_back(liste[i]);
        }
        return l;
    }
    
    liste.reverse();
    return this;
}

Element* Integers::protected_index(LispE* lisp,long i) {
    if (i >= 0 && i < liste.size())
        return lisp->provideInteger(liste[i]);
    return null_;
}

Element* Integers::last_element(LispE* lisp) {
    if (!liste.size())
        return null_;
    return lisp->provideInteger(liste.back());
}

Element* Integers::value_on_index(LispE* lisp, long i) {
    if (i >= 0 && i < liste.size())
        return lisp->provideInteger(liste[i]);
    return null_;
}

Element* Integers::value_from_index(LispE* lisp, long i) {
    return lisp->provideInteger(liste[i]);
}

Element* Integers::value_on_index(LispE* lisp, Element* ix) {
    long i = ix->checkInteger(lisp);
    if (i < 0)
        i = liste.size() + i;
    
    if (i >= 0 && i < liste.size())
        return lisp->provideInteger(liste[i]);
    
    return null_;
}

Element* Integers::protected_index(LispE* lisp, Element* ix) {
    long i = ix->checkInteger(lisp);
    if (i < 0)
        i = liste.size() + i;
    
    if (i >= 0 && i < liste.size())
        return lisp->provideInteger(liste[i]);
    
    throw new Error("Error: index out of bounds");
}

Element* Integers::join_in_list(LispE* lisp, u_ustring& sep) {
    u_ustring str;
    u_ustring beg;
    for (long i = 0; i < liste.size(); i++) {
        str += beg;
        beg = sep;
        str += convertToUString(liste[i]);
    }
    return lisp->provideString(str);
}

Element* Integers::equal(LispE* lisp, Element* e) {
    return booleans_[(e->type == t_integers && liste == ((Integers*)e)->liste)];
}

bool Integers::egal(Element* e) {
    return (e->type == t_integers && liste == ((Integers*)e)->liste);
}

Element* Integers::extraction(LispE* lisp, List* l) {
    long depuis;
    l->evalAsInteger(2, lisp, depuis);
    if (depuis >= 0) {
        if (depuis >= liste.size())
            return emptylist_;
    }
    else {
        //We start from the end...
        depuis = liste.size() + depuis;
        if (depuis < 0)
            return emptylist_;
    }
    if (l->size() == 3) {
        //On returns only one element
        return lisp->provideInteger(liste[depuis]);
    }
    long upto;
    l->evalAsInteger(3, lisp, upto);
    if (upto >= 0) {
        if (upto >= liste.size())
            upto = liste.size();
    }
    else {
        //We start from the end...
        upto = liste.size() + upto;
        if (upto < 0)
            return emptylist_;
    }
    if (upto < depuis) {
        return emptylist_;
    }
    
    Integers* n = lisp->provideIntegers();
    for (;depuis < upto; depuis++) {
        n->liste.push_back(liste[depuis]);
    }
    return n;
}

Element* Integers::replace_in(LispE* lisp, List* l) {
    Element* elast = l->liste.back()->eval(lisp);
    long last = elast->asInteger();
    elast->release();

    long depuis;
    l->evalAsInteger(2, lisp, depuis);
    if (depuis >= 0) {
        if (depuis >= liste.size())
            return emptylist_;
    }
    else {
        //We start from the end...
        depuis = liste.size() + depuis;
        if (depuis < 0)
            return emptylist_;
    }
    if (l->size() == 4) {
        //On returns only one element
        Integers* l = (Integers*)fullcopy();
        l->liste[depuis] = last;
        return l;
    }
    long upto;
    l->evalAsInteger(3, lisp, upto);
    if (upto >= 0) {
        if (upto >= liste.size())
            upto = liste.size();
    }
    else {
        //We start from the end...
        upto = liste.size() + upto;
        if (upto < 0)
            return emptylist_;
    }
    if (upto < depuis) {
        return emptylist_;
    }
    
    Integers* n = lisp->provideIntegers();
    long i;
    for (i = 0; i < depuis; i++)
        n->liste.push_back(liste[i]);
    n->liste.push_back(last);
    for (i = upto; i < size(); i++)
        n->liste.push_back(liste[i]);
    return n;
}

Element* Integers::duplicate_constant(bool pair) {
    if (status == s_constant) {
        Integers* l = (Integers*)newInstance();
        l->liste = liste;
        return l;
    }
    return this;
}

Element* Integers::asList(LispE* lisp) {
    List* l =  lisp->provideList();
    for (long i = 0; i < liste.size(); i++)
        l->append(lisp->provideInteger(liste[i]));
    return l;
}

Element* Integers::cadr(LispE* lisp, u_ustring& action) {
    long pos = 0;
    long sz = size();
    long i;
    
    for (i = action.size() - 1; i>= 0; i--) {
        for (i = action.size() - 1; i>= 0; i--) {
            if (action[i] == 'a') {
                if (i)
                    throw new Error("Error: the elements of a list of values are no lists");
                return lisp->provideInteger(liste[pos]);
            }
            if (pos == sz)
                throw new Error("Error: You reached the list size limit");
            pos++;
        }
    }
    
    if (pos) {
        if (pos == sz)
            return null_;
        return lisp->provideIntegers(this, pos);
    }
    
    return null_;
}

Element* Integers::car(LispE* lisp) {
    if (liste.size() == 0)
        return null_;
    return lisp->provideInteger(liste[0]);
}

Element* Integers::cdr(LispE* lisp) {
    if (liste.size() <= 1)
        return null_;
    return lisp->provideIntegers(this, 1);
}

//--------------------------------------------------------------------------------
//Strings methods
//--------------------------------------------------------------------------------

Element* Strings::check_member(LispE* lisp, Element* the_set) {
    Strings* n = lisp->provideStrings();
    long sz = the_set->size();
    std::vector<u_ustring> v;
    long i, j;
    for (i = 0; i < sz; i++)
        v.push_back(the_set->index(i)->asUString(lisp));
    
    for (j = 0; j < size(); j++) {
        for (i = 0; i < sz; i++) {
            if (liste[j] == v[i]) {
                n->liste.push_back(v[i]);
                break;
            }
        }
        if (i == sz)
            n->liste.push_back(U"");
    }
    return n;
}

bool Strings::compare(LispE* lisp, List* comparison, short instruction, long i, long j) {
    ((String*)comparison->liste[1])->content = liste[i];
    ((String*)comparison->liste[2])->content = liste[j];
    return comparison->eval_Boolean(lisp, instruction);
}

void Strings::sorting(LispE* lisp, List* comparison, short instruction, long rmin, long rmax) {
    //(setq s (sort '< (shuffle (cons 5 (range 1 99999 1)))))
    //(sort '< '(28 60 10 38 80 34 8 22 78 68 85 48 13 39 100 56 89 82 11 52 99 50 20 96 97 59 23 81 53 15 3 67 77 7 57 74 49 32 86 66 43 26 75 62 29 71 2 91 51 1 18 12 24 21 36 72 90 40 70 14 61 93 6 4 79 94 47 58 30 83 84 44 88 63 95 45 33 65 37 92 27 64 55 9 31 73 54 16 98 5 46 25 76 42 17 69 19 35 5 41 87))
    //(sort '< '(20 12 15 13 19 17 14))
    //(sort '< (shuffle (range 1 16 1)))
    //(sort '< '(4 3 7 1 5))
    //(sort '< '(10 4 8 5 12 2 6 11 3 9 7 9))
    
    //check sorting stability
    //(loop i (range 1 9999 1) (select (<= (at s i) (at s (+ i 1))) (println 'erreur i)))
    
    long j = rmax-rmin+1;
    long pivot;
    
    if (j < 7) {
        if (j < 2)
            return;
        
        if (j == 2) {
            if (compare(lisp, comparison, instruction, rmax, rmin))
                swap(rmax, rmin);
            return;
        }
        
        if (j == 3) {
            if (compare(lisp, comparison, instruction, rmin, rmin + 1)) {
                if (compare(lisp, comparison, instruction, rmin + 1, rmax))
                    return;
            }
            else {
                swap(rmin, rmin + 1);
                if (compare(lisp, comparison, instruction, rmin + 1, rmax))
                    return;
            }
            swap(rmax, rmin + 1);
            if (compare(lisp, comparison, instruction, rmin, rmin + 1))
                return;
            swap(rmin, rmin + 1);
            return;
        }
        
        long sz;
        while (rmax > rmin) {
            sz = rmin;
            for (j = rmin; j < rmax; j++) {
                if (compare(lisp, comparison, instruction, j + 1, j)) {
                    swap(j, j + 1);
                    sz = j;
                    pivot = j;
                    while (pivot > rmin && compare(lisp, comparison, instruction, pivot, pivot - 1))
                        swap(pivot, pivot - 1);
                }
            }
            rmax = sz;
        }
        return;
    }
    
    pivot = rmin - 1;
    ((String*)comparison->liste[2])->content = liste[rmax];
    for (j = rmin; j < rmax; j++) {
        ((String*)comparison->liste[1])->content = liste[j];
        if (comparison->eval_Boolean(lisp, instruction)) {
            pivot++;
            swap(pivot,j);
        }
    }
    pivot++;
    swap(pivot, rmax);
    
    sorting(lisp, comparison, instruction, rmin, pivot-1);
    sorting(lisp, comparison, instruction, pivot+1, rmax);
}

void Strings::sorting(LispE* lisp, List* comparison) {
    //We sort between home and last...
    long sz = size();
    if (sz <= 1)
        return;
    
    Conststring n1(U"");
    Conststring n2(U"");
    comparison->liste[1] = &n1;
    comparison->liste[2] = &n2;
    n1.content = liste[0];
    n2.content = liste[0];
    if (comparison->eval(lisp)->Boolean())
        throw new Error(L"Error: The comparison must be strict for a 'sort': (comp a a) must return 'nil'.");
    
    sorting(lisp, comparison, comparison->liste[0]->type, 0, sz-1);
}

Element* Strings::minimum(LispE* lisp) {
    if (!liste.size())
        return null_;
    u_ustring v = liste[0];
    for (long i = 1; i < liste.size(); i++) {
        if (v > liste[i])
            v = liste[i];
    }
    return lisp->provideString(v);
}

Element* Strings::maximum(LispE* lisp) {
    if (!liste.size())
        return null_;
    u_ustring v = liste[0];
    for (long i = 1; i < liste.size(); i++) {
        if (v < liste[i])
            v = liste[i];
    }
    return lisp->provideString(v);
}

Element* Strings::minmax(LispE* lisp) {
    if (!liste.size())
        return null_;
    u_ustring v_min = liste[0];
    u_ustring v_max = liste[0];
    for (long i = 1; i < liste.size(); i++) {
        if (v_min > liste[i])
            v_min = liste[i];
        else {
            if (v_max < liste[i])
                v_max = liste[i];
        }
    }
    Strings* f = lisp->provideStrings();
    f->liste.push_back(v_min);
    f->liste.push_back(v_max);
    return f;
}

void Strings::flatten(LispE* lisp, List* l) {
    for (long i = 0; i < size(); i++) {
        l->append(lisp->provideString(liste[i]));
    }
}

void Strings::flatten(LispE* lisp, Numbers* l) {
    for (long i = 0; i < size(); i++) {
        l->liste.push_back(convertingfloathexa(liste[i].c_str()));
    }
}

void Strings::flatten(LispE* lisp, Floats* l) {
    for (long i = 0; i < size(); i++) {
        l->liste.push_back(convertingfloathexa(liste[i].c_str()));
    }
}

void Strings::storevalue(LispE* lisp, double v) {
    liste.push_back(convertToUString(v));
}

void Strings::storevalue(LispE* lisp,long v) {
    liste.push_back(convertToUString(v));
}

void Strings::storevalue(LispE* lisp, u_ustring& s) {
    liste.push_back(s);
}

void Strings::append(LispE* lisp, u_ustring& k) {
    liste.push_back(k);
}

void Strings::append(LispE* lisp, double v) {
    liste.push_back(convertToUString(v));
}

void Strings::append(LispE* lisp, long v) {
    liste.push_back(convertToUString(v));
}

Element* Strings::loop(LispE* lisp, short label, List* code) {
    long i_loop;
    Element* e = null_;
    String* element;
    lisp->recording(null_, label);
    long sz = code->liste.size();
    for (long i = 0; i < liste.size(); i++) {
        element = lisp->provideString(liste[i]);
        lisp->replacingvalue(element, label);
        _releasing(e);
        //We then execute our instructions
        for (i_loop = 3; i_loop < sz && e->type != l_return; i_loop++) {
            e->release();
            e = code->liste[i_loop]->eval(lisp);
        }
        if (e->type == l_return) {
            if (e->isBreak())
                return null_;
            return e;
        }
    }
    return e;
}

Element* Strings::insert(LispE* lisp, Element* e, long ix) {
    if (ix < 0)
        throw new Error("Error: Wrong index in 'insert'");
    
    Strings* l = (Strings*)duplicate_constant();
    l->liste.insert(ix, e->asUString(lisp));
    return l;
}

Element* Strings::rotate(bool left) {
    if (liste.size() <= 1)
        return this;
    
    Strings* l = (Strings*)newInstance();
    if (left) {
        for (long i = 1; i < liste.size(); i++)
            l->liste.push_back(liste[i]);
        l->liste.push_back(liste[0]);
        return l;
    }
    
    l->liste.push_back(liste.back());
    for (long i = 0; i < liste.size() - 1; i ++)
        l->liste.push_back(liste[i]);
    return l;
}

Element* Strings::unique(LispE* lisp) {
    if (liste.size() == 0)
        return this;
    
    Strings* nb = lisp->provideStrings();
    long i, j;
    bool found;
    nb->liste.push_back(liste[0]);
    for (i = 1; i < liste.size(); i++) {
        found = false;
        for (j = 0; j < nb->liste.size(); j++) {
            if (liste[i] == nb->liste[j]) {
                found = true;
                break;
            }
        }
        if (!found)
            nb->liste.push_back(liste[i]);
    }
    return nb;
}

Element* Strings::thekeys(LispE* lisp) {
    Integers* keys = lisp->provideIntegers();
    for (long i = 0; i< size(); i++) {
        keys->liste.push_back(i);
    }
    return keys;
}

Element* Strings::search_element(LispE* lisp, Element* valeur, long ix) {
    u_ustring v = valeur->asUString(lisp);
    for (long i = ix; i < liste.size(); i++) {
        if (liste[i] == v)
            return lisp->provideInteger(i);
    }
    return null_;
}

bool Strings::check_element(LispE* lisp, Element* valeur) {
    u_ustring v = valeur->asUString(lisp);
    for (long i = 0; i < liste.size(); i++) {
        if (liste[i] == v)
            return true;
    }
    return false;
}

Element* Strings::replace_all_elements(LispE* lisp, Element* valeur, Element* remp) {
    long nb = 0;
    u_ustring v = valeur->asUString(lisp);
    u_ustring r = remp->asUString(lisp);
    long sz = liste.size();
    for (long i = 0; i < sz; i++) {
        if (liste[i] == v) {
            liste[i] = r;
            nb++;
        }
    }
    return lisp->provideInteger(nb);
}

Element* Strings::search_all_elements(LispE* lisp, Element* valeur, long ix) {
    Integers* l = lisp->provideIntegers();
    u_ustring v = valeur->asUString(lisp);
    long sz = liste.size();
    for (long i = ix; i < sz; i++) {
        if (liste[i] == v)
            l->liste.push_back(i);
    }
    if (l->liste.size() == 0) {
        l->release();
        return emptylist_;
    }
    return l;
}

Element* Strings::count_all_elements(LispE* lisp, Element* valeur, long ix) {
    long nb = 0;
    u_ustring v = valeur->asUString(lisp);
    for (long i = ix; i <
         liste.size(); i++) {
        if (liste[i] == v)
            nb++;
    }
    return lisp->provideInteger(nb);
}

Element* Strings::list_and(LispE* lisp, Element* value) {
    if (!value->isList() && !value->isSet())
        throw new Error("Error: Can only apply '&&&' to strings, lists or sets");
    
    Strings* l = lisp->provideStrings();
    long sz = liste.size();
    for (long i = 0; i < sz; i++) {
        if (!l->liste.check(liste[i]) && value->check_element(lisp, index(i)))
            l->liste.push_back(liste[i]);
    }
    if (l->liste.size() == 0) {
        l->release();
        return emptylist_;
    }
    return l;
}

Element* Strings::list_or(LispE* lisp, Element* value) {
    if (!value->isList() && !value->isSet())
        throw new Error("Error: Can only apply '|||' to strings, lists or sets");
    
    Strings* l = lisp->provideStrings();
    long sz = liste.size();
    long i;
    for (i = 0; i < sz; i++) {
        if (!l->liste.check(liste[i]))
            l->liste.push_back(liste[i]);
    }

    if (value->type == t_llist) {
        u_link* a = ((LList*)value)->liste.begin();
        for (; a != NULL; a = a->next()) {
            for (i = 0; i < l->size(); i++) {
                if (l->liste[i] == a->value->asUString(lisp))
                    break;
            }
            if (i == l->size())
                l->liste.push_back(a->value->asUString(lisp));
        }
    }
    else {
        if (value->isList()) {
            long sz = value->size();
            for (long j = 0; j < sz; j++) {
                for (i = 0; i < l->size(); i++) {
                    if (l->liste[i] == value->index(j)->asUString(lisp))
                        break;
                }
                if (i == l->size())
                    l->liste.push_back(value->index(j)->asUString(lisp));
            }
        }
        else {
            if (value->isSet()) {
                void* iter = value->begin_iter();
                Element* next_value = value->next_iter_exchange(lisp, iter);
                while (next_value != emptyatom_) {
                    for (i = 0; i < l->size(); i++) {
                        if (l->liste[i] == next_value->asUString(lisp))
                            break;
                    }
                    if (i == l->size())
                        l->liste.push_back(next_value->asUString(lisp));
                    next_value = value->next_iter_exchange(lisp, iter);
                }
                value->clean_iter(iter);
            }
        }
    }
    if (l->liste.size() == 0) {
        l->release();
        return emptylist_;
    }
    return l;
}

Element* Strings::list_xor(LispE* lisp, Element* value) {
    if (!value->isList() && !value->isSet())
        throw new Error("Error: Can only apply '^^^' to strings, lists or sets");
    
    Strings* l = lisp->provideStrings();
    Strings* intersection = (Strings*)list_and(lisp, value);
    long sz = liste.size();
    long i;
    for (i = 0; i < sz; i++) {
        if (!intersection->liste.check(liste[i]) && !l->liste.check(liste[i]))
            l->liste.push_back(liste[i]);
    }

    u_ustring v;
    if (value->type == t_llist) {
        u_link* a = ((LList*)value)->liste.begin();
        for (; a != NULL; a = a->next()) {
            v = a->value->asUString(lisp);
            if (!intersection->liste.check(v) && !l->liste.check(v))
                l->liste.push_back(v);
        }
    }
    else {
        if (value->isList()) {
            long sz = value->size();
            for (i = 0; i < sz; i++) {
                v = value->index(i)->asUString(lisp);
                if (!intersection->liste.check(v) && !l->liste.check(v))
                    l->liste.push_back(v);
            }
        }
        else {
            if (value->isSet()) {
                void* iter = value->begin_iter();
                Element* next_value = value->next_iter_exchange(lisp, iter);
                while (next_value != emptyatom_) {
                    v = next_value->asUString(lisp);
                    if (!intersection->liste.check(v) && !l->liste.check(v))
                        l->liste.push_back(v);
                    next_value = value->next_iter_exchange(lisp, iter);
                }
                value->clean_iter(iter);
            }
        }
    }
    
    intersection->release();
    
    if (l->liste.size() == 0) {
        l->release();
        return emptylist_;
    }
    return l;
}

Element* Strings::search_reverse(LispE* lisp, Element* valeur, long ix) {
    u_ustring v = valeur->asUString(lisp);
    for (long i = liste.size() - 1; i >= ix; i--) {
        if (liste[i] == v)
            return lisp->provideInteger(i);
    }
    return minusone_;
}

Element* Strings::reverse(LispE* lisp, bool duplicate) {
    if (liste.size() <= 1)
        return this;
    
    if (duplicate) {
        Strings* l = lisp->provideStrings();
        for (long i = liste.size()-1; i >= 0; i--) {
            l->liste.push_back(liste[i]);
        }
        return l;
    }
    
    liste.reverse();
    return this;
}

Element* Strings::protected_index(LispE* lisp,long i) {
    if (i >= 0 && i < liste.size())
        return lisp->provideString(liste[i]);
    return null_;
}

Element* Strings::last_element(LispE* lisp) {
    if (!liste.size())
        return null_;
    return lisp->provideString(liste.back());
}

Element* Strings::value_on_index(LispE* lisp, long i) {
    if (i >= 0 && i < liste.size())
        return lisp->provideString(liste[i]);
    return null_;
}

Element* Strings::value_from_index(LispE* lisp, long i) {
    return lisp->provideString(liste[i]);
}

Element* Strings::value_on_index(LispE* lisp, Element* ix) {
    long i = ix->checkInteger(lisp);
    if (i < 0)
        i = liste.size() + i;
    
    if (i >= 0 && i < liste.size())
        return lisp->provideString(liste[i]);
    
    return null_;
}

Element* Strings::protected_index(LispE* lisp, Element* ix) {
    long i = ix->checkInteger(lisp);
    if (i < 0)
        i = liste.size() + i;
    
    if (i >= 0 && i < liste.size())
        return lisp->provideString(liste[i]);
    
    throw new Error("Error: index out of bounds");
}

Element* Strings::join_in_list(LispE* lisp, u_ustring& sep) {
    u_ustring str;
    u_ustring beg;
    for (long i = 0; i < liste.size(); i++) {
        str += beg;
        beg = sep;
        str += liste[i];
    }
    return lisp->provideString(str);
}

Element* Strings::equal(LispE* lisp, Element* e) {
    return booleans_[(e->type == t_strings && liste == ((Strings*)e)->liste)];
}

bool Strings::egal(Element* e) {
    return (e->type == t_strings && liste == ((Strings*)e)->liste);
}

Element* Strings::plus(LispE* lisp, Element* e) {
    //Two cases either e is a string or it is a list...
    if (e == NULL) {
        u_ustring d = liste[0];
        for (long i = 1; i < size(); i++) {
            d += liste[i];
        }
        return lisp->provideString(d);
    }
    if (e->isList()) {
        for (long i = 0; i < e->size() && i < size(); i++) {
            liste[i] += e->index(i)->asUString(lisp);
        }
        return this;
    }
    for (long i = 0; i < size(); i++) {
        liste[i] += e->asUString(lisp);
    }
    return this;
}

Element* Strings::extraction(LispE* lisp, List* l) {
    
    Element* e_from = l->liste[2];
    Element* e;
    
    long from = 0;
    long firstisString = -1;
    short nxt = 3;
    short ty;
    switch (e_from->label()) {
        case l_minus:
            e_from = l->liste[3]->eval(lisp);
            nxt = 4;
            ty = e_from->type;
            if (ty == t_string)
                ty = t_minus_string;
            else
                throw new Error("Error: Wrong value after first operator: '-'");
            break;
        case l_plus:
            e_from = l->liste[3]->eval(lisp);
            nxt = 4;
            ty = e_from->type;
            if (ty == t_string)
                ty = t_plus_string;
            else
                throw new Error("Error: Wrong value after first operator: '+'");
            break;
        case l_minus_plus:
            e_from = l->liste[3]->eval(lisp);
            nxt = 4;
            ty = e_from->type;
            if (ty == t_string)
                ty = t_minus_plus_string;
            else
                throw new Error("Error: Wrong value after first operator: '-+'");
            break;
        default:
            e_from = e_from->eval(lisp);
            ty = e_from->type;
    }
    
    e = null_;
    switch (ty) {
        case t_string: {
            e = search_element(lisp, e_from, 0);
            if (e == null_)
                return emptylist_;
            from = e->asInteger();
            firstisString = 0;
            break;
        }
        case t_plus_string: {
            e = search_element(lisp, e_from, 0);
            if (e == null_)
                return emptylist_;
            firstisString = e->asInteger();
            break;
        }
        case t_minus_string: {
            e = search_reverse(lisp, e_from, 0);
            if (e == minusone_)
                return emptylist_;
            //We skip the first characters
            from = e->asInteger() + 1;
            firstisString = 0;
            break;
        }
        case t_minus_plus_string: {
            e = search_reverse(lisp, e_from, 0);
            if (e == minusone_)
                return emptylist_;
            firstisString = e->asInteger();
            break;
        }
        case t_float:
        case t_short:
        case t_integer:
        case t_number:
            from = e_from->asInteger();
            if (from < 0)
                from = size() + from;
            break;
        default:
            e->release();
            e_from->release();
            throw new Error("Error: cannot use the first position in 'extract'");
    }
    
    e->release();
    e_from->release();
    
    if (from < 0 || from >= size())
        return emptylist_;
    
    if (nxt == l->size()) {
        //Only one element is returned
        return lisp->provideString(liste[from]);
    }
    
    Element* e_upto = l->liste[nxt];
    switch (e_upto->label()) {
        case l_minus:
            e_upto = l->liste[nxt+1]->eval(lisp);
            ty = e_upto->type;
            if (ty == t_string)
                ty = t_minus_string;
            else
                throw new Error("Error: Wrong value after second operator: '-'");
            break;
        case l_plus:
            e_upto = l->liste[nxt+1]->eval(lisp);
            ty = e_upto->type;
            if (ty == t_string)
                ty = t_plus_string;
            else
                throw new Error("Error: Wrong value after second operator: '+'");
            break;
        case l_minus_plus:
            e_upto = l->liste[nxt+1]->eval(lisp);
            ty = e_from->type;
            if (ty == t_string)
                ty = t_minus_plus_string;
            else
                throw new Error("Error: Wrong value after second operator: '-+'");
            break;
        default:
            e_upto = e_upto->eval(lisp);
            ty = e_upto->type;
    }
    
    long upto;
    e = null_;
    switch (ty) {
        case t_string: {
            if (firstisString == -1) firstisString = 0;
            e = search_element(lisp, e_upto, from + firstisString);
            if (e == null_)
                return emptylist_;
            upto = e->asInteger();
            break;
        }
        case t_plus_string: {
            if (firstisString == -1) firstisString = 0;
            e = search_element(lisp, e_upto, from + firstisString);
            if (e == null_)
                return emptylist_;
            //All characters are integrated
            upto = e->asInteger() + 1;
            break;
        }
        case t_minus_string: {
            e = search_reverse(lisp, e_upto, 0);
            if (e == minusone_)
                return emptylist_;
            upto = e->asInteger();
            break;
        }
        case t_minus_plus_string: {
            e = search_reverse(lisp, e_upto, 0);
            if (e == minusone_)
                return emptylist_;
            upto = e->asInteger() - 1;
            break;
        }
        case t_float:
        case t_short:
        case t_integer:
        case t_number:
            upto = e_upto->asInteger();
            if (firstisString != -1 && upto > 0) {
                //in this case upto is a number of characters, not a position
                upto += from + firstisString;
            }
            else {
                if (upto <= 0) {
                    //We start from the end...
                    upto = size() + upto;
                }
            }
            break;
        default:
            e->release();
            e_upto->release();
            throw new Error("Error: cannot use the second position in 'extract'");
    }
    
    e->release();
    e_upto->release();
    if (upto <= from)
        return emptylist_;
    
    if (upto > size())
        upto = size();
    Strings* n = lisp->provideStrings();
    for (;from < upto; from++) {
        n->liste.push_back(liste[from]);
    }
    return n;
}

Element* Strings::replace_in(LispE* lisp, List* l) {
    Element* e = l->liste.back()->eval(lisp);
    u_ustring last = e->asUString(lisp);
    e->release();

    Element* e_from = l->liste[2];
    
    long from = 0;
    long firstisString = -1;
    short nxt = 3;
    short ty;
    switch (e_from->label()) {
        case l_minus:
            e_from = l->liste[3]->eval(lisp);
            nxt = 4;
            ty = e_from->type;
            if (ty == t_string)
                ty = t_minus_string;
            else
                throw new Error("Error: Wrong value after first operator: '-'");
            break;
        case l_plus:
            e_from = l->liste[3]->eval(lisp);
            nxt = 4;
            ty = e_from->type;
            if (ty == t_string)
                ty = t_plus_string;
            else
                throw new Error("Error: Wrong value after first operator: '+'");
            break;
        case l_minus_plus:
            e_from = l->liste[3]->eval(lisp);
            nxt = 4;
            ty = e_from->type;
            if (ty == t_string)
                ty = t_minus_plus_string;
            else
                throw new Error("Error: Wrong value after first operator: '-+'");
            break;
        default:
            e_from = e_from->eval(lisp);
            ty = e_from->type;
    }
    
    e = null_;
    switch (ty) {
        case t_string: {
            e = search_element(lisp, e_from, 0);
            if (e == null_)
                return emptylist_;
            from = e->asInteger();
            firstisString = 0;
            break;
        }
        case t_plus_string: {
            e = search_element(lisp, e_from, 0);
            if (e == null_)
                return emptylist_;
            firstisString = e->asInteger();
            break;
        }
        case t_minus_string: {
            e = search_reverse(lisp, e_from, 0);
            if (e == minusone_)
                return emptylist_;
            //We skip the first characters
            from = e->asInteger() + 1;
            firstisString = 0;
            break;
        }
        case t_minus_plus_string: {
            e = search_reverse(lisp, e_from, 0);
            if (e == minusone_)
                return emptylist_;
            firstisString = e->asInteger();
            break;
        }
        case t_float:
        case t_short:
        case t_integer:
        case t_number:
            from = e_from->asInteger();
            if (from < 0)
                from = size() + from;
            break;
        default:
            e->release();
            e_from->release();
            throw new Error("Error: cannot use the first position in 'setrange'");
    }
    
    e->release();
    e_from->release();
    
    if (from < 0 || from >= size())
        return this;
    
    if (nxt == l->size() - 1) {
        //Only one element is returned
        //On returns only one element
        Strings* l = (Strings*)fullcopy();
        l->liste[from] = last;
        return l;
    }
    
    Element* e_upto = l->liste[nxt];
    switch (e_upto->label()) {
        case l_minus:
            e_upto = l->liste[nxt+1]->eval(lisp);
            ty = e_upto->type;
            if (ty == t_string)
                ty = t_minus_string;
            else
                throw new Error("Error: Wrong value after second operator: '-'");
            break;
        case l_plus:
            e_upto = l->liste[nxt+1]->eval(lisp);
            ty = e_upto->type;
            if (ty == t_string)
                ty = t_plus_string;
            else
                throw new Error("Error: Wrong value after second operator: '+'");
            break;
        case l_minus_plus:
            e_upto = l->liste[nxt+1]->eval(lisp);
            ty = e_from->type;
            if (ty == t_string)
                ty = t_minus_plus_string;
            else
                throw new Error("Error: Wrong value after second operator: '-+'");
            break;
        default:
            e_upto = e_upto->eval(lisp);
            ty = e_upto->type;
    }
    
    long upto;
    e = null_;
    switch (ty) {
        case t_string: {
            if (firstisString == -1) firstisString = 0;
            e = search_element(lisp, e_upto, from + firstisString);
            if (e == null_)
                return emptylist_;
            upto = e->asInteger();
            break;
        }
        case t_plus_string: {
            if (firstisString == -1) firstisString = 0;
            e = search_element(lisp, e_upto, from + firstisString);
            if (e == null_)
                return emptylist_;
            //All characters are integrated
            upto = e->asInteger() + 1;
            break;
        }
        case t_minus_string: {
            e = search_reverse(lisp, e_upto, 0);
            if (e == minusone_)
                return emptylist_;
            upto = e->asInteger();
            break;
        }
        case t_minus_plus_string: {
            e = search_reverse(lisp, e_upto, 0);
            if (e == minusone_)
                return emptylist_;
            upto = e->asInteger() - 1;
            break;
        }
        case t_float:
        case t_short:
        case t_integer:
        case t_number:
            upto = e_upto->asInteger();
            if (firstisString != -1 && upto > 0) {
                //in this case upto is a number of characters, not a position
                upto += from + firstisString;
            }
            else {
                if (upto <= 0) {
                    //We start from the end...
                    upto = size() + upto;
                }
            }
            break;
        default:
            e->release();
            e_upto->release();
            throw new Error("Error: cannot use the second position in 'setrange'");
    }
    
    e->release();
    e_upto->release();
    if (upto <= from)
        return emptylist_;
    
    if (upto > size())
        upto = size();
    Strings* n = lisp->provideStrings();
    long i;
    for (i = 0; i < from; i++)
        n->liste.push_back(liste[i]);
    n->liste.push_back(last);
    for (i = upto; i < size(); i++)
        n->liste.push_back(liste[i]);
    return n;
}

Element* Strings::duplicate_constant(bool pair) {
    if (status == s_constant) {
        Strings* l = (Strings*)newInstance();
        l->liste = liste;
        return l;
    }
    return this;
}

Element* Strings::asList(LispE* lisp) {
    List* l =  lisp->provideList();
    for (long i = 0; i < liste.size(); i++)
        l->append(lisp->provideString(liste[i]));
    return l;
}

Element* Strings::cadr(LispE* lisp, u_ustring& action) {
    long pos = 0;
    long sz = size();
    long i;
    
    for (i = action.size() - 1; i>= 0; i--) {
        if (action[i] == 'a') {
            if (!i)
                return lisp->provideString(liste[pos]);
            exchange_value.content = liste[pos];
            u_ustring nxt = action.substr(0, i);
            return exchange_value.cadr(lisp, nxt);
        }
        
        if (pos == sz)
            throw new Error("Error: You reached the list size limit");
        pos++;
    }
    
    if (pos) {
        if (pos == sz)
            return null_;
        return new Strings(this, pos);
    }
    
    return null_;
}

Element* Strings::car(LispE* lisp) {
    if (liste.size() == 0)
        return null_;
    return lisp->provideString(liste[0]);
}

Element* Strings::cdr(LispE* lisp) {
    if (liste.size() <= 1)
        return null_;
    return new Strings(this, 1);
}

//--------------------------------------------------------------------------------
//Shorts methods
//--------------------------------------------------------------------------------

Element* Shorts::check_member(LispE* lisp, Element* the_set) {
    Shorts* n = new Shorts();
    long v;
    long i, j;
    long sz = the_set->size();
    for (j = 0; j < size(); j++) {
        for (i = 0; i < sz; i++) {
            v = the_set->index(i)->asShort();
            if (liste[j] == v) {
                n->liste.push_back(1);
                break;
            }
        }
        if (i == sz)
            n->liste.push_back(0);
    }
    return n;
}

bool Shorts::compare(LispE* lisp, List* comparison, short instruction, long i, long j) {
    ((Short*)comparison->liste[1])->integer = liste[i];
    ((Short*)comparison->liste[2])->integer = liste[j];
    return comparison->eval_Boolean(lisp, instruction);
}

void Shorts::sorting(LispE* lisp, List* comparison, short instruction, long rmin, long rmax) {
    //(setq s (sort '< (shuffle (cons 5 (range 1 99999 1)))))
    //(sort '< '(28 60 10 38 80 34 8 22 78 68 85 48 13 39 100 56 89 82 11 52 99 50 20 96 97 59 23 81 53 15 3 67 77 7 57 74 49 32 86 66 43 26 75 62 29 71 2 91 51 1 18 12 24 21 36 72 90 40 70 14 61 93 6 4 79 94 47 58 30 83 84 44 88 63 95 45 33 65 37 92 27 64 55 9 31 73 54 16 98 5 46 25 76 42 17 69 19 35 5 41 87))
    //(sort '< '(20 12 15 13 19 17 14))
    //(sort '< (shuffle (range 1 16 1)))
    //(sort '< '(4 3 7 1 5))
    //(sort '< '(10 4 8 5 12 2 6 11 3 9 7 9))
    
    //check sorting stability
    //(loop i (range 1 9999 1) (select (<= (at s i) (at s (+ i 1))) (println 'erreur i)))
    
    long j = rmax-rmin+1;
    long pivot;
    
    if (j < 7) {
        if (j < 2)
            return;
        
        if (j == 2) {
            if (compare(lisp, comparison, instruction, rmax, rmin))
                swap(rmax, rmin);
            return;
        }
        
        if (j == 3) {
            if (compare(lisp, comparison, instruction, rmin, rmin + 1)) {
                if (compare(lisp, comparison, instruction, rmin + 1, rmax))
                    return;
            }
            else {
                swap(rmin, rmin + 1);
                if (compare(lisp, comparison, instruction, rmin + 1, rmax))
                    return;
            }
            swap(rmax, rmin + 1);
            if (compare(lisp, comparison, instruction, rmin, rmin + 1))
                return;
            swap(rmin, rmin + 1);
            return;
        }
        
        long sz;
        while (rmax > rmin) {
            sz = rmin;
            for (j = rmin; j < rmax; j++) {
                if (compare(lisp, comparison, instruction, j + 1, j)) {
                    swap(j, j + 1);
                    sz = j;
                    pivot = j;
                    while (pivot > rmin && compare(lisp, comparison, instruction, pivot, pivot - 1))
                        swap(pivot, pivot - 1);
                }
            }
            rmax = sz;
        }
        return;
    }
    
    pivot = rmin - 1;
    ((Short*)comparison->liste[2])->integer = liste[rmax];
    for (j = rmin; j < rmax; j++) {
        ((Short*)comparison->liste[1])->integer = liste[j];
        if (comparison->eval_Boolean(lisp, instruction)) {
            pivot++;
            swap(pivot,j);
        }
    }
    pivot++;
    swap(pivot, rmax);
    
    sorting(lisp, comparison, instruction, rmin, pivot-1);
    sorting(lisp, comparison, instruction, pivot+1, rmax);
}

void Shorts::sorting(LispE* lisp, List* comparison) {
    //We sort between home and last...
    long sz = size();
    if (sz <= 1)
        return;
    
    Constshort n1(0);
    Constshort n2(0);
    comparison->liste[1] = &n1;
    comparison->liste[2] = &n2;
    n1.integer = liste[0];
    n2.integer = liste[0];
    if (comparison->eval(lisp)->Boolean())
        throw new Error(L"Error: The comparison must be strict for a 'sort': (comp a a) must return 'nil'.");
    
    sorting(lisp, comparison, comparison->liste[0]->type, 0, sz-1);
}

Element* Shorts::minimum(LispE* lisp) {
    if (!liste.size())
        return null_;
    short v = liste[0];
    for (long i = 1; i < liste.size(); i++) {
        if (v > liste[i])
            v = liste[i];
    }
    return new Short(v);
}

Element* Shorts::maximum(LispE* lisp) {
    if (!liste.size())
        return null_;
    short v = liste[0];
    for (long i = 1; i < liste.size(); i++) {
        if (v < liste[i])
            v = liste[i];
    }
    return new Short(v);
}

Element* Shorts::minmax(LispE* lisp) {
    if (!liste.size())
        return null_;
    short v_min = liste[0];
    short v_max = liste[0];
    for (long i = 1; i < liste.size(); i++) {
        if (v_min > liste[i])
            v_min = liste[i];
        else {
            if (v_max < liste[i])
                v_max = liste[i];
        }
    }
    Shorts* f = new Shorts();
    f->liste.push_back(v_min);
    f->liste.push_back(v_max);
    return f;
}

void Shorts::flatten(LispE* lisp, List* l) {
    for (long i = 0; i < size(); i++) {
        l->append(new Short(liste[i]));
    }
}

void Shorts::flatten(LispE* lisp, Numbers* l) {
    for (long i = 0; i < size(); i++) {
        l->liste.push_back(liste[i]);
    }
}

void Shorts::flatten(LispE* lisp, Floats* l) {
    for (long i = 0; i < size(); i++) {
        l->liste.push_back(liste[i]);
    }
}

void Shorts::storevalue(LispE* lisp, double v) {
    liste.push_back(v);
}

void Shorts::storevalue(LispE* lisp,long v) {
    liste.push_back(v);
}

void Shorts::storevalue(LispE* lisp, u_ustring& s) {
    long v = convertinginteger(s);
    liste.push_back(v);
}

Element* Shorts::invert_sign(LispE* lisp) {
    Shorts* n = this;
    if (status)
        n = new Shorts(this);
    
    for (long i = 0; i < n->liste.size(); i++)
        n->liste[i] *= -1;
    return n;
}

void Shorts::append(LispE* lisp, u_ustring& k) {
    short d = (short)convertinginteger(k);
    liste.push_back(d);
}

void Shorts::append(LispE* lisp, double v) {
    liste.push_back(v);
}

void Shorts::append(LispE* lisp, long v) {
    liste.push_back(v);
}

Element* Shorts::loop(LispE* lisp, short label, List* code) {
    long i_loop;
    Element* e = null_;
    Integer* element;
    lisp->recording(null_, label);
    long sz = code->liste.size();
    for (long i = 0; i < liste.size(); i++) {
        element = lisp->provideInteger(liste[i]);
        lisp->replacingvalue(element, label);
        _releasing(e);
        //We then execute our instructions
        for (i_loop = 3; i_loop < sz && e->type != l_return; i_loop++) {
            e->release();
            e = code->liste[i_loop]->eval(lisp);
        }
        if (e->type == l_return) {
            if (e->isBreak())
                return null_;
            return e;
        }
    }
    return e;
}

Element* Shorts::insert(LispE* lisp, Element* e, long ix) {
    if (ix < 0)
        throw new Error("Error: Wrong index in 'insert'");
    
    Shorts* l = (Shorts*)duplicate_constant();
    l->liste.insert(ix, e->asInteger());
    return l;
}

Element* Shorts::rotate(bool left) {
    if (liste.size() <= 1)
        return this;
    
    Shorts* l = (Shorts*)newInstance();
    if (left) {
        for (long i = 1; i < liste.size(); i++)
            l->liste.push_back(liste[i]);
        l->liste.push_back(liste[0]);
        return l;
    }
    
    l->liste.push_back(liste.back());
    for (long i = 0; i < liste.size() - 1; i ++)
        l->liste.push_back(liste[i]);
    return l;
}

Element* Shorts::unique(LispE* lisp) {
    if (liste.size() == 0)
        return this;
    
    Shorts* nb = new Shorts();
    long i, j;
    bool found;
    nb->liste.push_back(liste[0]);
    for (i = 1; i < liste.size(); i++) {
        found = false;
        for (j = 0; j < nb->liste.size(); j++) {
            if (liste[i] == nb->liste[j]) {
                found = true;
                break;
            }
        }
        if (!found)
            nb->liste.push_back(liste[i]);
    }
    return nb;
}

Element* Shorts::thekeys(LispE* lisp) {
    Shorts* keys = new Shorts();
    for (long i = 0; i< size(); i++) {
        keys->liste.push_back(i);
    }
    return keys;
}

Element* Shorts::search_element(LispE* lisp, Element* valeur, long ix) {
    short v = valeur->asShort();
    for (long i = ix; i < liste.size(); i++) {
        if (liste[i] == v)
            return lisp->provideInteger(i);
    }
    return null_;
}

bool Shorts::check_element(LispE* lisp, Element* valeur) {
    short v = valeur->asShort();
    for (long i = 0; i < liste.size(); i++) {
        if (liste[i] == v)
            return true;
    }
    return false;
}

Element* Shorts::replace_all_elements(LispE* lisp, Element* valeur, Element* remp) {
    long nb = 0;
    short v = valeur->asShort();
    short r = remp->asShort();
    long sz = liste.size();
    for (long i = 0; i < sz; i++) {
        if (liste[i] == v) {
            liste[i] = r;
            nb++;
        }
    }
    return lisp->provideInteger(nb);
}

Element* Shorts::search_all_elements(LispE* lisp, Element* valeur, long ix) {
    Integers* l = lisp->provideIntegers();
    short v = valeur->asShort();
    long sz = liste.size();
    for (long i = ix; i < sz; i++) {
        if (liste[i] == v)
            l->liste.push_back(i);
    }
    if (l->liste.size() == 0) {
        l->release();
        return emptylist_;
    }
    return l;
}

Element* Shorts::count_all_elements(LispE* lisp, Element* valeur, long ix) {
    long nb = 0;
    short v = valeur->asShort();
    long sz = liste.size();
    for (long i = ix; i < sz; i++) {
        if (liste[i] == v)
            nb++;
    }
    return lisp->provideInteger(nb);
}

Element* Shorts::list_and(LispE* lisp, Element* value) {
    if (!value->isList() && !value->isSet())
        throw new Error("Error: Can only apply '&&&' to strings, lists or sets");
    
    Shorts* l = new Shorts();
    long sz = liste.size();
    for (long i = 0; i < sz; i++) {
        if (!l->liste.check(liste[i]) && value->check_element(lisp, index(i)))
            l->liste.push_back(liste[i]);
    }

    if (l->liste.size() == 0) {
        l->release();
        return emptylist_;
    }
    return l;
}

Element* Shorts::list_or(LispE* lisp, Element* value) {
    if (!value->isList() && !value->isSet())
        throw new Error("Error: Can only apply '|||' to strings, lists or sets");
    
    Shorts* l = new Shorts();
    long sz = liste.size();
    long i;
    for (i = 0; i < sz; i++) {
        if (!l->liste.check(liste[i]))
            l->liste.push_back(liste[i]);
    }

    if (value->type == t_llist) {
        u_link* a = ((LList*)value)->liste.begin();
        for (; a != NULL; a = a->next()) {
            for (i = 0; i < l->size(); i++) {
                if (l->liste[i] == a->value->asShort())
                    break;
            }
            if (i == l->size())
                l->liste.push_back(a->value->asShort());
        }
    }
    else {
        if (value->isList()) {
            long sz = value->size();
            for (long j = 0; j < sz; j++) {
                for (i = 0; i < l->size(); i++) {
                    if (l->liste[i] == value->index(j)->asShort())
                        break;
                }
                if (i == l->size())
                    l->liste.push_back(value->index(j)->asShort());
            }
        }
        else {
            if (value->isSet()) {
                void* iter = value->begin_iter();
                Element* next_value = value->next_iter_exchange(lisp, iter);
                while (next_value != emptyatom_) {
                    for (i = 0; i < l->size(); i++) {
                        if (l->liste[i] == next_value->asShort())
                            break;
                    }
                    if (i == l->size())
                        l->liste.push_back(next_value->asShort());
                    next_value = value->next_iter_exchange(lisp, iter);
                }
                value->clean_iter(iter);
            }
        }
    }

    if (l->liste.size() == 0) {
        l->release();
        return emptylist_;
    }
    return l;
}

Element* Shorts::list_xor(LispE* lisp, Element* value) {
    if (!value->isList() && !value->isSet())
        throw new Error("Error: Can only apply '^^^' to strings, lists or sets");
    
    Shorts* l = new Shorts();
    Shorts* intersection = (Shorts*)list_and(lisp, value);
    long sz = liste.size();
    long i;
    for (i = 0; i < sz; i++) {
        if (!intersection->liste.check(liste[i]) && !l->liste.check(liste[i]))
            l->liste.push_back(liste[i]);
    }

    short v;
    if (value->type == t_llist) {
        u_link* a = ((LList*)value)->liste.begin();
        for (; a != NULL; a = a->next()) {
            v = a->value->asShort();
            if (!intersection->liste.check(v) && !l->liste.check(v))
                l->liste.push_back(v);
        }
    }
    else {
        if (value->isList()) {
            long sz = value->size();
            for (i = 0; i < sz; i++) {
                v = value->index(i)->asShort();
                if (!intersection->liste.check(v) && !l->liste.check(v))
                    l->liste.push_back(v);
            }
        }
        else {
            if (value->isSet()) {
                void* iter = value->begin_iter();
                Element* next_value = value->next_iter_exchange(lisp, iter);
                while (next_value != emptyatom_) {
                    v = next_value->asShort();
                    if (!intersection->liste.check(v) && !l->liste.check(v))
                        l->liste.push_back(v);
                    next_value = value->next_iter_exchange(lisp, iter);
                }
                value->clean_iter(iter);
            }
        }
    }
    intersection->release();
    
    if (l->liste.size() == 0) {
        l->release();
        return emptylist_;
    }
    return l;
}

Element* Shorts::search_reverse(LispE* lisp, Element* valeur, long ix) {
    short v = valeur->asShort();
    for (long i = liste.size() - 1; i >= ix; i--) {
        if (liste[i] == v)
            return lisp->provideInteger(i);
    }
    return minusone_;
}

Element* Shorts::reverse(LispE* lisp, bool duplicate) {
    if (liste.size() <= 1)
        return this;
    
    if (duplicate) {
        Shorts* l = new Shorts();
        for (long i = liste.size()-1; i >= 0; i--) {
            l->liste.push_back(liste[i]);
        }
        return l;
    }
    
    liste.reverse();
    return this;
}

Element* Shorts::protected_index(LispE* lisp,long i) {
    if (i >= 0 && i < liste.size())
        return new Short(liste[i]);
    return null_;
}

Element* Shorts::last_element(LispE* lisp) {
    if (!liste.size())
        return null_;
    return new Short(liste.back());
}

Element* Shorts::value_on_index(LispE* lisp, long i) {
    if (i >= 0 && i < liste.size())
        return new Short(liste[i]);
    return null_;
}

Element* Shorts::value_from_index(LispE* lisp, long i) {
    return new Short(liste[i]);
}

Element* Shorts::value_on_index(LispE* lisp, Element* ix) {
    long i = ix->checkInteger(lisp);
    if (i < 0)
        i = liste.size() + i;
    
    if (i >= 0 && i < liste.size())
        return new Short(liste[i]);
    
    return null_;
}

Element* Shorts::protected_index(LispE* lisp, Element* ix) {
    long i = ix->checkInteger(lisp);
    if (i < 0)
        i = liste.size() + i;
    
    if (i >= 0 && i < liste.size())
        return new Short(liste[i]);
    
    throw new Error("Error: index out of bounds");
}

Element* Shorts::join_in_list(LispE* lisp, u_ustring& sep) {
    u_ustring str;
    u_ustring beg;
    for (long i = 0; i < liste.size(); i++) {
        str += beg;
        beg = sep;
        str += convertToUString((long)liste[i]);
    }
    return lisp->provideString(str);
}

Element* Shorts::equal(LispE* lisp, Element* e) {
    return booleans_[(e->type == t_shorts && liste == ((Shorts*)e)->liste)];
}

bool Shorts::egal(Element* e) {
    return (e->type == t_shorts && liste == ((Shorts*)e)->liste);
}

Element* Shorts::extraction(LispE* lisp, List* l) {
    long depuis;
    l->evalAsInteger(2, lisp, depuis);
    if (depuis >= 0) {
        if (depuis >= liste.size())
            return emptylist_;
    }
    else {
        //We start from the end...
        depuis = liste.size() + depuis;
        if (depuis < 0)
            return emptylist_;
    }
    if (l->size() == 3) {
        //On returns only one element
        return new Short(liste[depuis]);
    }
    long upto;
    l->evalAsInteger(3, lisp, upto);
    if (upto >= 0) {
        if (upto >= liste.size())
            upto = liste.size();
    }
    else {
        //We start from the end...
        upto = liste.size() + upto;
        if (upto < 0)
            return emptylist_;
    }
    if (upto < depuis) {
        return emptylist_;
    }
    
    Shorts* n = new Shorts();
    for (;depuis < upto; depuis++) {
        n->liste.push_back(liste[depuis]);
    }
    return n;
}

Element* Shorts::replace_in(LispE* lisp, List* l) {
    Element* elast = l->liste.back()->eval(lisp);
    long last = elast->asInteger();
    elast->release();

    long depuis;
    l->evalAsInteger(2, lisp, depuis);
    if (depuis >= 0) {
        if (depuis >= liste.size())
            return emptylist_;
    }
    else {
        //We start from the end...
        depuis = liste.size() + depuis;
        if (depuis < 0)
            return emptylist_;
    }
    if (l->size() == 4) {
        //On returns only one element
        Shorts* l = (Shorts*)fullcopy();
        l->liste[depuis] = last;
        return l;
    }
    long upto;
    l->evalAsInteger(3, lisp, upto);
    if (upto >= 0) {
        if (upto >= liste.size())
            upto = liste.size();
    }
    else {
        //We start from the end...
        upto = liste.size() + upto;
        if (upto < 0)
            return emptylist_;
    }
    if (upto < depuis) {
        return emptylist_;
    }
    
    Shorts* n = new Shorts();
    long i;
    for (i = 0; i < depuis; i++)
        n->liste.push_back(liste[i]);
    n->liste.push_back(last);
    for (i = upto; i < size(); i++)
        n->liste.push_back(liste[i]);
    return n;
}

Element* Shorts::duplicate_constant(bool pair) {
    if (status == s_constant) {
        Shorts* l = new Shorts;
        l->liste = liste;
        return l;
    }
    return this;
}

Element* Shorts::asList(LispE* lisp) {
    List* l =  lisp->provideList();
    for (long i = 0; i < liste.size(); i++)
        l->append(new Short(liste[i]));
    return l;
}

Element* Shorts::cadr(LispE* lisp, u_ustring& action) {
    long pos = 0;
    long sz = size();
    long i;
    
    for (i = action.size() - 1; i>= 0; i--) {
        for (i = action.size() - 1; i>= 0; i--) {
            if (action[i] == 'a') {
                if (i)
                    throw new Error("Error: the elements of a list of values are no lists");
                return new Short(liste[pos]);
            }
            if (pos == sz)
                throw new Error("Error: You reached the list size limit");
            pos++;
        }
    }
    
    if (pos) {
        if (pos == sz)
            return null_;
        return new Shorts(this, pos);
    }
    
    return null_;
}

Element* Shorts::car(LispE* lisp) {
    if (liste.size() == 0)
        return null_;
    return new Short(liste[0]);
}

Element* Shorts::cdr(LispE* lisp) {
    if (liste.size() <= 1)
        return null_;
    return new Shorts(this, 1);
}

//--------------------------------------------------------------------------------
//Floats methods
//--------------------------------------------------------------------------------

Element* Floats::check_member(LispE* lisp, Element* the_set) {
    Floats* n = lisp->provideFloats();
    float v;
    long i, j;
    long sz = the_set->size();
    for (j = 0; j < size(); j++) {
        for (i = 0; i < sz; i++) {
            v = the_set->index(i)->asFloat();
            if (liste[j] == v) {
                n->liste.push_back(1);
                break;
            }
        }
        if (i == sz)
            n->liste.push_back(0);
    }
    return n;
}

bool Floats::compare(LispE* lisp, List* comparison, short instruction, long i, long j) {
    ((Float*)comparison->liste[1])->number = liste[i];
    ((Float*)comparison->liste[2])->number = liste[j];
    return comparison->eval_Boolean(lisp, instruction);
}

void Floats::sorting(LispE* lisp, List* comparison, short instruction, long rmin, long rmax) {
    //(setq s (sort '< (shuffle (cons 5 (range 1 99999 1)))))
    //(sort '< '(28 60 10 38 80 34 8 22 78 68 85 48 13 39 100 56 89 82 11 52 99 50 20 96 97 59 23 81 53 15 3 67 77 7 57 74 49 32 86 66 43 26 75 62 29 71 2 91 51 1 18 12 24 21 36 72 90 40 70 14 61 93 6 4 79 94 47 58 30 83 84 44 88 63 95 45 33 65 37 92 27 64 55 9 31 73 54 16 98 5 46 25 76 42 17 69 19 35 5 41 87))
    //(sort '< '(20 12 15 13 19 17 14))
    //(sort '< (shuffle (range 1 16 1)))
    //(sort '< '(4 3 7 1 5))
    //(sort '< '(10 4 8 5 12 2 6 11 3 9 7 9))
    
    //check sorting stability
    //(loop i (range 1 9999 1) (select (<= (at s i) (at s (+ i 1))) (println 'erreur i)))
    
    long j = rmax-rmin+1;
    long pivot;
    
    if (j < 7) {
        if (j < 2)
            return;
        
        if (j == 2) {
            if (compare(lisp, comparison, instruction, rmax, rmin))
                swap(rmax, rmin);
            return;
        }
        
        if (j == 3) {
            if (compare(lisp, comparison, instruction, rmin, rmin + 1)) {
                if (compare(lisp, comparison, instruction, rmin + 1, rmax))
                    return;
            }
            else {
                swap(rmin, rmin + 1);
                if (compare(lisp, comparison, instruction, rmin + 1, rmax))
                    return;
            }
            swap(rmax, rmin + 1);
            if (compare(lisp, comparison, instruction, rmin, rmin + 1))
                return;
            swap(rmin, rmin + 1);
            return;
        }
        
        long sz;
        while (rmax > rmin) {
            sz = rmin;
            for (j = rmin; j < rmax; j++) {
                if (compare(lisp, comparison, instruction, j + 1, j)) {
                    swap(j, j + 1);
                    sz = j;
                    pivot = j;
                    while (pivot > rmin && compare(lisp, comparison, instruction, pivot, pivot - 1))
                        swap(pivot, pivot - 1);
                }
            }
            rmax = sz;
        }
        return;
    }
    
    pivot = rmin - 1;
    ((Float*)comparison->liste[2])->number = liste[rmax];
    for (j = rmin; j < rmax; j++) {
        ((Float*)comparison->liste[1])->number = liste[j];
        if (comparison->eval_Boolean(lisp, instruction)) {
            pivot++;
            swap(pivot,j);
        }
    }
    pivot++;
    swap(pivot, rmax);
    
    sorting(lisp, comparison, instruction, rmin, pivot-1);
    sorting(lisp, comparison, instruction, pivot+1, rmax);
}

void Floats::sorting(LispE* lisp, List* comparison) {
    //We sort between home and last...
    long sz = size();
    if (sz <= 1)
        return;
    
    Constfloat n1(0);
    Constfloat n2(0);
    comparison->liste[1] = &n1;
    comparison->liste[2] = &n2;
    
    n1.number = liste[0];
    n2.number = liste[0];
    if (comparison->eval(lisp)->Boolean())
        throw new Error(L"Error: The comparison must be strict for a 'sort': (comp a a) must return 'nil'.");
    
    sorting(lisp, comparison, comparison->liste[0]->type, 0, sz-1);
}

Element* Floats::minimum(LispE* lisp) {
    if (!liste.size())
        return null_;
    float v = liste[0];
    for (long i = 1; i < liste.size(); i++) {
        if (v > liste[i])
            v = liste[i];
    }
    return lisp->provideFloat(v);
}

Element* Floats::maximum(LispE* lisp) {
    if (!liste.size())
        return null_;
    float v = liste[0];
    for (long i = 1; i < liste.size(); i++) {
        if (v < liste[i])
            v = liste[i];
    }
    return lisp->provideFloat(v);
}

Element* Floats::minmax(LispE* lisp) {
    if (!liste.size())
        return null_;
    float v_min = liste[0];
    float v_max = liste[0];
    for (long i = 1; i < liste.size(); i++) {
        if (v_min > liste[i])
            v_min = liste[i];
        else {
            if (v_max < liste[i])
                v_max = liste[i];
        }
    }
    Floats* f = lisp->provideFloats();
    f->liste.push_back(v_min);
    f->liste.push_back(v_max);
    return f;
}

void Element::flatten(LispE* lisp, Floats* l) {
    l->append(this);
}

void Floats::flatten(LispE* lisp, List* l) {
    for (long i = 0; i < size(); i++) {
        l->append(lisp->provideFloat(liste[i]));
    }
}

void Floats::flatten(LispE* lisp, Numbers* l) {
    for (long i = 0; i < size(); i++) {
        l->liste.push_back(liste[i]);
    }
}

void Floats::flatten(LispE* lisp, Floats* l) {
    for (long i = 0; i < size(); i++) {
        l->liste.push_back(liste[i]);
    }
}

void Floats::storevalue(LispE* lisp, double v) {
    liste.push_back(v);
}

void Floats::storevalue(LispE* lisp, float v) {
    liste.push_back(v);
}

void Floats::storevalue(LispE* lisp,long v) {
    liste.push_back(v);
}

void Floats::storevalue(LispE* lisp, u_ustring& s) {
    long l;
    float v = convertingfloathexa((u_uchar*)s.c_str(), l);
    liste.push_back(v);
}

Element* Floats::invert_sign(LispE* lisp) {
    Floats* n = this;
    if (status)
        n = lisp->provideFloats(this);
    
    for (long i = 0; i < n->liste.size(); i++)
        n->liste[i] *= -1;
    return n;
}

void Floats::append(LispE* lisp, u_ustring& k) {
    long l;
    float d = convertingfloathexa((u_uchar*)k.c_str(), l);
    liste.push_back(d);
}

void Floats::append(LispE* lisp, double v) {
    liste.push_back(v);
}

void Floats::append(LispE* lisp, float v) {
    liste.push_back(v);
}

void Floats::append(LispE* lisp, long v) {
    liste.push_back(v);
}

Element* Floats::loop(LispE* lisp, short label, List* code) {
    long i_loop;
    Element* e = null_;
    Float* element;
    lisp->recording(null_, label);
    long sz = code->liste.size();
    for (long i = 0; i < liste.size(); i++) {
        element = lisp->provideFloat(liste[i]);
        lisp->replacingvalue(element, label);
        _releasing(e);
        //We then execute our instructions
        for (i_loop = 3; i_loop < sz && e->type != l_return; i_loop++) {
            e->release();
            e = code->liste[i_loop]->eval(lisp);
        }
        if (e->type == l_return) {
            if (e->isBreak())
                return null_;
            return e;
        }
    }
    return e;
}

Element* Floats::insert(LispE* lisp, Element* e, long ix) {
    if (ix < 0)
        throw new Error("Error: Wrong index in 'insert'");
    
    Floats* l = (Floats*)duplicate_constant();
    l->liste.insert(ix, e->asFloat());
    return l;
}

Element* Floats::rotate(bool left) {
    if (liste.size() <= 1)
        return this;
    
    Floats* l = (Floats*)newInstance();
    if (left) {
        for (long i = 1; i < liste.size(); i++)
            l->liste.push_back(liste[i]);
        l->liste.push_back(liste[0]);
        return l;
    }
    
    l->liste.push_back(liste.back());
    for (long i = 0; i < liste.size() - 1; i ++)
        l->liste.push_back(liste[i]);
    return l;
}

Element* Floats::unique(LispE* lisp) {
    if (liste.size() == 0)
        return this;
    
    Floats* nb = lisp->provideFloats();
    long i, j;
    bool found;
    nb->liste.push_back(liste[0]);
    for (i = 1; i < liste.size(); i++) {
        found = false;
        for (j = 0; j < nb->liste.size(); j++) {
            if (liste[i] == nb->liste[j]) {
                found = true;
                break;
            }
        }
        if (!found)
            nb->liste.push_back(liste[i]);
    }
    return nb;
}

Element* Floats::thekeys(LispE* lisp) {
    Integers* keys = lisp->provideIntegers();
    for (long i = 0; i< size(); i++) {
        keys->liste.push_back(i);
    }
    return keys;
}

Element* Floats::search_element(LispE* lisp, Element* valeur, long ix) {
    float v = valeur->asFloat();
    for (long i = ix; i < liste.size(); i++) {
        if (liste[i] == v)
            return lisp->provideInteger(i);
    }
    return null_;
}

bool Floats::check_element(LispE* lisp, Element* valeur) {
    float v = valeur->asFloat();
    for (long i = 0; i < liste.size(); i++) {
        if (liste[i] == v)
            return true;
    }
    return false;
}

Element* Floats::replace_all_elements(LispE* lisp, Element* valeur, Element* remp) {
    long nb = 0;
    float r = remp->asFloat();
    float v = valeur->asFloat();
    long sz = liste.size();
    for (long i = 0; i < sz; i++) {
        if (liste[i] == v) {
            liste[i] = r;
            nb++;
        }
    }
    return lisp->provideInteger(nb);
}

Element* Floats::search_all_elements(LispE* lisp, Element* valeur, long ix) {
    Integers* l = lisp->provideIntegers();
    float v = valeur->asFloat();
    long sz = liste.size();
    for (long i = ix; i < sz; i++) {
        if (liste[i] == v)
            l->liste.push_back(i);
    }
    if (l->liste.size() == 0) {
        l->release();
        return emptylist_;
    }
    return l;
}

Element* Floats::count_all_elements(LispE* lisp, Element* valeur, long ix) {
    long nb = 0;
    float v = valeur->asFloat();
    long sz = liste.size();
    for (long i = ix; i < sz; i++) {
        if (liste[i] == v)
            nb++;
    }
    return lisp->provideInteger(nb);
}

Element* Floats::list_and(LispE* lisp, Element* value) {
    if (!value->isList() && !value->isSet())
        throw new Error("Error: Can only apply '&&&' to strings, lists or sets");
    
    Floats* l = lisp->provideFloats();
    long sz = liste.size();
    for (long i = 0; i < sz; i++) {
        if (!l->liste.check(liste[i]) && value->check_element(lisp, index(i)))
            l->liste.push_back(liste[i]);
    }

    if (l->liste.size() == 0) {
        l->release();
        return emptylist_;
    }
    return l;
}

Element* Floats::list_or(LispE* lisp, Element* value) {
    if (!value->isList() && !value->isSet())
        throw new Error("Error: Can only apply '|||' to strings, lists or sets");
    
    Floats* l = lisp->provideFloats();
    long sz = liste.size();
    long i;
    for (i = 0; i < sz; i++) {
        if (!l->liste.check(liste[i]))
            l->liste.push_back(liste[i]);
    }

    if (value->type == t_llist) {
        u_link* a = ((LList*)value)->liste.begin();
        for (; a != NULL; a = a->next()) {
            for (i = 0; i < l->size(); i++) {
                if (l->liste[i] == a->value->asFloat())
                    break;
            }
            if (i == l->size())
                l->liste.push_back(a->value->asFloat());
        }
    }
    else {
        if (value->isList()) {
            long sz = value->size();
            for (long j = 0; j < sz; j++) {
                for (i = 0; i < l->size(); i++) {
                    if (l->liste[i] == value->index(j)->asFloat())
                        break;
                }
                if (i == l->size())
                    l->liste.push_back(value->index(j)->asFloat());
            }
        }
        else {
            if (value->isSet()) {
                void* iter = value->begin_iter();
                Element* next_value = value->next_iter_exchange(lisp, iter);
                while (next_value != emptyatom_) {
                    for (i = 0; i < l->size(); i++) {
                        if (l->liste[i] == next_value->asFloat())
                            break;
                    }
                    if (i == l->size())
                        l->liste.push_back(next_value->asFloat());
                    next_value = value->next_iter_exchange(lisp, iter);
                }
                value->clean_iter(iter);
            }
        }
    }
    
    if (l->liste.size() == 0) {
        l->release();
        return emptylist_;
    }
    return l;
}

Element* Floats::list_xor(LispE* lisp, Element* value) {
    if (!value->isList() && !value->isSet())
        throw new Error("Error: Can only apply '^^^' to strings, lists or sets");
    
    Floats* l = lisp->provideFloats();

    Floats* intersection = (Floats*)list_and(lisp, value);
    long sz = liste.size();
    long i;
    for (i = 0; i < sz; i++) {
        if (!intersection->liste.check(liste[i]) && !l->liste.check(liste[i]))
            l->liste.push_back(liste[i]);
    }

    float v;
    if (value->type == t_llist) {
        u_link* a = ((LList*)value)->liste.begin();
        for (; a != NULL; a = a->next()) {
            v = a->value->asFloat();
            if (!intersection->liste.check(v) && !l->liste.check(v))
                l->liste.push_back(v);
        }
    }
    else {
        if (value->isList()) {
            long sz = value->size();
            for (i = 0; i < sz; i++) {
                v = value->index(i)->asFloat();
                if (!intersection->liste.check(v) && !l->liste.check(v))
                    l->liste.push_back(v);
            }
        }
        else {
            if (value->isSet()) {
                void* iter = value->begin_iter();
                Element* next_value = value->next_iter_exchange(lisp, iter);
                while (next_value != emptyatom_) {
                    v = next_value->asFloat();
                    if (!intersection->liste.check(v) && !l->liste.check(v))
                        l->liste.push_back(v);
                    next_value = value->next_iter_exchange(lisp, iter);
                }
                value->clean_iter(iter);
            }
        }
    }
    intersection->release();
    
    if (l->liste.size() == 0) {
        l->release();
        return emptylist_;
    }
    return l;
}

Element* Floats::search_reverse(LispE* lisp, Element* valeur, long ix) {
    float v = valeur->asFloat();
    for (long i = liste.size() - 1; i >= ix; i--) {
        if (liste[i] == v)
            return lisp->provideInteger(i);
    }
    return minusone_;
}

Element* Floats::reverse(LispE* lisp, bool duplicate) {
    if (liste.size() <= 1)
        return this;
    
    if (duplicate) {
        Floats* l = lisp->provideFloats();
        for (long i = liste.size()-1; i >= 0; i--) {
            l->liste.push_back(liste[i]);
        }
        return l;
    }
    
    liste.reverse();
    return this;
}

Element* Floats::protected_index(LispE* lisp,long i) {
    if (i >= 0 && i < liste.size())
        return lisp->provideFloat(liste[i]);
    return null_;
}

Element* Floats::last_element(LispE* lisp) {
    if (!liste.size())
        return null_;
    return lisp->provideFloat(liste.back());
}

Element* Floats::value_on_index(LispE* lisp, long i) {
    if (i >= 0 && i < liste.size())
        return lisp->provideFloat(liste[i]);
    return null_;
}

Element* Floats::value_from_index(LispE* lisp, long i) {
    return lisp->provideFloat(liste[i]);
}

Element* Floats::value_on_index(LispE* lisp, Element* ix) {
    long i = ix->checkInteger(lisp);
    if (i < 0)
        i = liste.size() + i;
    
    if (i >= 0 && i < liste.size())
        return lisp->provideFloat(liste[i]);
    
    return null_;
}

Element* Floats::protected_index(LispE* lisp, Element* ix) {
    long i = ix->checkInteger(lisp);
    if (i < 0)
        i = liste.size() + i;
    
    if (i >= 0 && i < liste.size())
        return lisp->provideFloat(liste[i]);
    
    throw new Error("Error: index out of bounds");
}

Element* Floats::join_in_list(LispE* lisp, u_ustring& sep) {
    u_ustring str;
    u_ustring beg;
    for (long i = 0; i < liste.size(); i++) {
        str += beg;
        beg = sep;
        str += convertToUString(liste[i]);
    }
    return lisp->provideString(str);
}

Element* Floats::equal(LispE* lisp, Element* e) {
    return booleans_[(e->type == t_floats && liste == ((Floats*)e)->liste)];
}

bool Floats::egal(Element* e) {
    return (e->type == t_floats && liste == ((Floats*)e)->liste);
}

Element* Floats::extraction(LispE* lisp, List* l) {
    long depuis;
    l->evalAsInteger(2, lisp, depuis);
    if (depuis >= 0) {
        if (depuis >= liste.size())
            return emptylist_;
    }
    else {
        //We start from the end...
        depuis = liste.size() + depuis;
        if (depuis < 0)
            return emptylist_;
    }
    if (l->size() == 3) {
        //On returns only one element
        return lisp->provideFloat(liste[depuis]);
    }
    long upto;
    l->evalAsInteger(3, lisp, upto);
    if (upto >= 0) {
        if (upto >= liste.size())
            upto = liste.size();
    }
    else {
        //We start from the end...
        upto = liste.size() + upto;
        if (upto < 0)
            return emptylist_;
    }
    if (upto < depuis) {
        return emptylist_;
    }
    
    Floats* n = lisp->provideFloats();
    for (;depuis < upto; depuis++) {
        n->liste.push_back(liste[depuis]);
    }
    return n;
}

Element* Floats::replace_in(LispE* lisp, List* l) {
    Element* e_last = l->liste.back()->eval(lisp);
    float last = e_last->asFloat();
    e_last->release();
    
    long depuis;
    l->evalAsInteger(2, lisp, depuis);
    if (depuis >= 0) {
        if (depuis >= liste.size())
            return emptylist_;
    }
    else {
        //We start from the end...
        depuis = liste.size() + depuis;
        if (depuis < 0)
            return emptylist_;
    }
    if (l->size() == 4) {
        //On returns only one element
        Floats* l = (Floats*)fullcopy();
        l->liste[depuis] = last;
        return l;
    }
    long upto;
    l->evalAsInteger(3, lisp, upto);
    if (upto >= 0) {
        if (upto >= liste.size())
            upto = liste.size();
    }
    else {
        //We start from the end...
        upto = liste.size() + upto;
        if (upto < 0)
            return emptylist_;
    }
    if (upto < depuis) {
        return emptylist_;
    }
    
    Floats* n = lisp->provideFloats();
    long i;
    for (i = 0; i < depuis; i++)
        n->liste.push_back(liste[i]);
    n->liste.push_back(last);
    for (i = upto; i < size(); i++)
        n->liste.push_back(liste[i]);
    return n;
}

Element* Floats::duplicate_constant(bool pair) {
    if (status == s_constant) {
        Floats* l = (Floats*)newInstance();
        l->liste = liste;
        return l;
    }
    return this;
}

Element* Floats::asList(LispE* lisp) {
    List* l =  lisp->provideList();
    for (long i = 0; i < liste.size(); i++)
        l->append(lisp->provideFloat(liste[i]));
    return l;
}

Element* Floats::cadr(LispE* lisp, u_ustring& action) {
    long pos = 0;
    long sz = size();
    long i;
    
    for (i = action.size() - 1; i>= 0; i--) {
        if (action[i] == 'a') {
            if (i)
                throw new Error("Error: the elements of a list of values are no lists");
            return lisp->provideFloat(liste[pos]);
        }
        if (pos == sz)
            throw new Error("Error: You reached the list size limit");
        pos++;
    }

    if (pos) {
        if (pos == sz)
            return null_;
        return lisp->provideFloats(this, pos);
    }
    
    return null_;
}

Element* Floats::car(LispE* lisp) {
    if (liste.size() == 0)
        return null_;
    return lisp->provideFloat(liste[0]);
}

Element* Floats::cdr(LispE* lisp) {
    if (liste.size() <= 1)
        return null_;
    return lisp->provideFloats(this, 1);
}

//--------------------------------------------------------------------------------
//Matrice methods
//--------------------------------------------------------------------------------

Element* Matrice::check_member(LispE* lisp,Element* the_set) {
    Matrice* r = new Matrice;
    r->size_x = size_x;
    r->size_y = size_y;
    Element* e;
    for (long i = 0; i < size(); i++) {
        e = liste[i]->check_member(lisp, the_set);
        r->append(e);
    }
    return r;
}

Element* Matrice::transposed(LispE* lisp) {
    Matrice* transposed_matrix = new Matrice(lisp, size_y, size_x, 0.0);
    long i, j = 0;
    
    Element* e;
    for (i = 0; i < size_x; i++) {
        e = liste[i];
        for (j = 0; j < size_y; j++) {
            transposed_matrix->index(j)->replacing(i, e->index(j));
        }
    }
    return transposed_matrix;
}

Element* Matrice::rank(LispE* lisp, vecte<long>& positions) {
    short sz = positions.size();
    if (!sz || sz > 2)
        throw new Error("Error: index mismatch");
        
    if (positions[0] == -1) {
        //We return all columns
        if (sz == 1 || positions[1] == -1) {
            Matrice* m = new Matrice();
            Numbers* result;
            
            for (long j = 0; j < size_y; j++) {
                result = lisp->provideNumbers();
                for (long i = 0; i < size_x; i++) {
                    result->liste.push_back(val(i,j));
                }
                m->append(result);
            }
            m->size_x = size_y;
            m->size_y = size_x;
            return m;
        }
        else {
            if (positions[1] >= size_y)
                throw new Error("Error: indexes out of bounds");
        }
    }
    else {
        if (sz == 2 && positions[1] != -1) {
            if (positions[1] >= size_y)
                throw new Error("Error: indexes out of bounds");
            return lisp->provideNumber(val(positions[0], positions[1]));
        }
        
        if (positions[0] >= size_x)
            throw new Error("Error: indexes out of bounds");
        
        return lisp->provideNumbers(((Numbers*)liste[positions[0]]));
    }

    Numbers* result = lisp->provideNumbers();
    for (long i = 0; i < size_x; i++) {
        result->liste.push_back(val(i,positions[1]));
    }
    return result;
}

Element* Matrice::loop(LispE* lisp, short label, List* code) {
    long i_loop;
    Element* e = null_;
    lisp->recording(null_, label);
    long sz = code->liste.size();
    for (long i = 0; i < size_x; i++) {
        lisp->replacingvalue(liste[i], label);
        _releasing(e);
        //We then execute our instructions
        for (i_loop = 3; i_loop < sz && e->type != l_return; i_loop++) {
            e->release();
            e = code->liste[i_loop]->eval(lisp);
        }
        if (e->type == l_return) {
            if (e->isBreak())
                return null_;
            return e;
        }
    }
    return e;
}

Element* Matrice::rotate(LispE* lisp, long axis) {
    Matrice* revert_matrix = new Matrice;
    revert_matrix->size_x = size_x;
    revert_matrix->size_y = size_y;
    
    long i;
    
    if (axis == 1) {
        for (i = 0; i < size_x; i++) {
            revert_matrix->append(liste[i]->rotate(lisp,0));
        }
        return revert_matrix;
    }
    
    Element* e;
    for (i = size_x-1; i>= 0;  i--) {
        e = lisp->provideNumbers((Numbers*)liste[i]);
        revert_matrix->append(e);
    }
    return revert_matrix;
}

Element* Matrice_float::check_member(LispE* lisp, Element* the_set) {
    Matrice_float* r = new Matrice_float;
    r->size_x = size_x;
    r->size_y = size_y;
    Element* e;
    for (long i = 0; i < size(); i++) {
        e = liste[i]->check_member(lisp, the_set);
        r->append(e);
    }
    return r;
}

Element* Matrice_float::transposed(LispE* lisp) {
    Matrice_float* transposed_matrix = new Matrice_float(lisp, size_y, size_x, 0.0);
    long i, j = 0;
    
    Element* e;
    for (i = 0; i < size_x; i++) {
        e = liste[i];
        for (j = 0; j < size_y; j++) {
            transposed_matrix->index(j)->replacing(i, e->index(j));
        }
    }
    return transposed_matrix;
}

Element* Matrice_float::rank(LispE* lisp, vecte<long>& positions) {
    short sz = positions.size();
    if (!sz || sz > 2)
        throw new Error("Error: index mismatch");
        
    if (positions[0] == -1) {
        //We return all columns
        if (sz == 1 || positions[1] == -1) {
            Matrice_float* m = new Matrice_float();
            Floats* result;
            
            for (long j = 0; j < size_y; j++) {
                result = lisp->provideFloats();
                for (long i = 0; i < size_x; i++) {
                    result->liste.push_back(val(i,j));
                }
                m->append(result);
            }
            m->size_x = size_y;
            m->size_y = size_x;
            return m;
        }
        else {
            if (positions[1] >= size_y)
                throw new Error("Error: indexes out of bounds");
        }
    }
    else {
        if (sz == 2 && positions[1] != -1) {
            if (positions[1] >= size_y)
                throw new Error("Error: indexes out of bounds");
            return lisp->provideFloat(val(positions[0], positions[1]));
        }
        
        if (positions[0] >= size_x)
            throw new Error("Error: indexes out of bounds");
        
        return lisp->provideFloats(((Floats*)liste[positions[0]]));
    }

    Floats* result = lisp->provideFloats();
    for (long i = 0; i < size_x; i++) {
        result->liste.push_back(val(i,positions[1]));
    }
    return result;
}

Element* Matrice_float::loop(LispE* lisp, short label, List* code) {
    long i_loop;
    Element* e = null_;
    lisp->recording(null_, label);
    long sz = code->liste.size();
    for (long i = 0; i < size_x; i++) {
        lisp->replacingvalue(liste[i], label);
        _releasing(e);
        //We then execute our instructions
        for (i_loop = 3; i_loop < sz && e->type != l_return; i_loop++) {
            e->release();
            e = code->liste[i_loop]->eval(lisp);
        }
        if (e->type == l_return) {
            if (e->isBreak())
                return null_;
            return e;
        }
    }
    return e;
}

Element* Matrice_float::rotate(LispE* lisp, long axis) {
    Matrice_float* revert_matrix = new Matrice_float;
    revert_matrix->size_x = size_x;
    revert_matrix->size_y = size_y;
    
    long i;
    
    if (axis == 1) {
        for (i = 0; i < size_x; i++) {
            revert_matrix->append(liste[i]->rotate(lisp,0));
        }
        return revert_matrix;
    }
    
    Element* e;
    for (i = size_x-1; i>= 0;  i--) {
        e = lisp->provideFloats((Floats*)liste[i]);
        revert_matrix->append(e);
    }
    return revert_matrix;
}

//--------------------------------------------------------------------------------
//Tenseur methods
//--------------------------------------------------------------------------------

Element* Tenseur::check_member(LispE* lisp, Element* the_set) {
    Tenseur* r = new Tenseur;
    r->shape = shape;
    Element* e;
    for (long i = 0; i < size(); i++) {
        e = liste[i]->check_member(lisp, the_set);
        r->append(e);
    }
    return r;
}

Element* Tenseur::transposed(LispE* lisp) {
    vecte<long> sz;
    sz = shape;
    long i = sz[0];
    sz.vecteur[0] = sz[1];
    sz.vecteur[1] = i;
    
    Tenseur* transposed_matrix = new Tenseur(lisp, sz, zero_);
    long j = 0;
    
    Element* e;
    for (i = 0; i < shape[0]; i++) {
        e = liste[i];
        for (j = 0; j < shape[1]; j++) {
            transposed_matrix->index(j)->replacing(i, e->index(j));
        }
    }
    return transposed_matrix;
}

Element* Tenseur::storeRank(LispE* lisp, Element* current, vecte<long>& positions, long idx) {
    long nb = shape.size() - idx;
    long p_idx = -1;
    if (idx < positions.size())
        p_idx = positions[idx];
    
    long i;
    if (p_idx == -1) {
        //If nb == 2, then it is a matrix
        //If nb == 1, then it is a vector
        if (nb == 1)
            return lisp->provideNumbers((Numbers*)current);
        
        List* m = lisp->provideList();

        if (nb == 2) {
            Numbers* result;
            for (long j = 0; j < shape[idx+1]; j++) {
                result = lisp->provideNumbers();
                for (i = 0; i < shape[idx]; i++) {
                    result->liste.push_back(current->index(i)->index(j)->asNumber());
                }
                m->append(result);
            }
        }
        else {
            for (i = 0; i < shape[idx]; i++) {
                m->append(storeRank(lisp, current->index(i), positions, idx+1));
            }
        }
        return m;
    }
    
    if (nb == 1)
        return current->index(p_idx);
    
    return storeRank(lisp, current->index(p_idx), positions, idx+1);
}

Element* Tenseur::rank(LispE* lisp, vecte<long>& positions) {
    //We get rid of the final negative values (useless)
    short sz = positions.size();
    if (!sz || sz > shape.size())
        throw new Error("Error: index mismatch");

    while (positions.size() > 1 && positions.back() < 0)
        positions.pop_back();
    
    //Check positions
    for (long i = 0; i < sz; i++) {
        if (positions[i] != -1 && (positions[i] < 0 || positions[i] >= shape[i]))
            throw new Error("Error: indexes out of bounds");
    }
    
    Element* res = storeRank(lisp, this, positions, 0);
    if (res->type == t_numbers)
        return res;
    
    if (res->type == t_number)
        return lisp->provideFloat(res->asNumber());
    
    //We steal the ITEM structure of res
    //which is a very fast operation
    //Since its internal values are not copied but borrowed
    if (res->index(0)->type == t_floats) {
        Matrice* m = new Matrice((List*)res);
        res->release();
        return m;
    }
    Tenseur* ts = new Tenseur(lisp, (List*)res);
    res->release();
    return ts;
}

Element* Tenseur::loop(LispE* lisp, short label, List* code) {
    long i_loop;
    Element* e = null_;
    lisp->recording(null_, label);
    long sz = code->liste.size();
    for (long i = 0; i < shape[0]; i++) {
        lisp->replacingvalue(liste[i], label);
        _releasing(e);
        //We then execute our instructions
        for (i_loop = 3; i_loop < sz && e->type != l_return; i_loop++) {
            e->release();
            e = code->liste[i_loop]->eval(lisp);
        }
        if (e->type == l_return) {
            if (e->isBreak())
                return null_;
            return e;
        }
    }
    return e;
}

Element* Tenseur::reversion(LispE* lisp, Element* value, long pos, long axis, bool init) {
    if (pos == axis)
        return value->reverse(lisp,true);
    
    if (pos == shape.size() -1)
        return lisp->provideNumbers((Numbers*)value);
    
    Element* r;
    if (init) {
        r = new Tenseur;
        ((Tenseur*)r)->shape = shape;
    }
    else
        r = lisp->provideList();
    
    Element* e;
    for (long i = 0; i < shape[pos]; i++) {
        e = reversion(lisp, value->index(i), pos+1, axis, false);
        r->append(e);
    }
    return r;
}

Element* Tenseur::rotate(LispE* lisp, long axis) {
    return reversion(lisp, this, 0, axis, true);
}

Element* Tenseur_float::check_member(LispE* lisp, Element* the_set) {
    Tenseur_float* r = new Tenseur_float;
    r->shape = shape;
    Element* e;
    for (long i = 0; i < size(); i++) {
        e = liste[i]->check_member(lisp, the_set);
        r->append(e);
    }
    return r;
}

Element* Tenseur_float::transposed(LispE* lisp) {
    vecte<long> sz;
    sz = shape;
    long i = sz[0];
    sz.vecteur[0] = sz[1];
    sz.vecteur[1] = i;
    
    Tenseur_float* transposed_matrix = new Tenseur_float(lisp, sz, zero_);
    long j = 0;
    
    Element* e;
    for (i = 0; i < shape[0]; i++) {
        e = liste[i];
        for (j = 0; j < shape[1]; j++) {
            transposed_matrix->index(j)->replacing(i, e->index(j));
        }
    }
    return transposed_matrix;
}

Element* Tenseur_float::storeRank(LispE* lisp, Element* current, vecte<long>& positions, long idx) {
    long nb = shape.size() - idx;
    long p_idx = -1;
    if (idx < positions.size())
        p_idx = positions[idx];
    
    long i;
    if (p_idx == -1) {
        //If nb == 2, then it is a matrix
        //If nb == 1, then it is a vector
        if (nb == 1)
            return lisp->provideFloats((Floats*)current);
        
        List* m = lisp->provideList();

        if (nb == 2) {
            Floats* result;
            for (long j = 0; j < shape[idx+1]; j++) {
                result = lisp->provideFloats();
                for (i = 0; i < shape[idx]; i++) {
                    result->liste.push_back(current->index(i)->index(j)->asFloat());
                }
                m->append(result);
            }
        }
        else {
            for (i = 0; i < shape[idx]; i++) {
                m->append(storeRank(lisp, current->index(i), positions, idx+1));
            }
        }
        return m;
    }
    
    if (nb == 1)
        return current->index(p_idx);
    
    return storeRank(lisp, current->index(p_idx), positions, idx+1);}



Element* Tenseur_float::rank(LispE* lisp, vecte<long>& positions) {
    //We get rid of the final negative values (useless)
    short sz = positions.size();
    if (!sz || sz > shape.size())
        throw new Error("Error: index mismatch");

    while (positions.size() > 1 && positions.back() < 0)
        positions.pop_back();

    //Check positions
    for (long i = 0; i < sz; i++) {
        if (positions[i] != -1 && (positions[i] < 0 || positions[i] >= shape[i]))
            throw new Error("Error: indexes out of bounds");
    }
    
    Element* res = storeRank(lisp, this, positions, 0);
    if (res->type == t_floats)
        return res;
    
    if (res->type == t_float)
        return lisp->provideFloat(res->asFloat());
    
    //We steal the ITEM structure of res
    //which is a very fast operation
    //Since its internal values are not copied but borrowed
    if (res->index(0)->type == t_floats) {
        Matrice_float* m = new Matrice_float((List*)res);
        res->release();
        return m;
    }
    Tenseur_float* ts = new Tenseur_float(lisp, (List*)res);
    res->release();
    return ts;
}

Element* Tenseur_float::loop(LispE* lisp, short label, List* code) {
    long i_loop;
    Element* e = null_;
    lisp->recording(null_, label);
    long sz = code->liste.size();
    for (long i = 0; i < shape[0]; i++) {
        lisp->replacingvalue(liste[i], label);
        _releasing(e);
        //We then execute our instructions
        for (i_loop = 3; i_loop < sz && e->type != l_return; i_loop++) {
            e->release();
            e = code->liste[i_loop]->eval(lisp);
        }
        if (e->type == l_return) {
            if (e->isBreak())
                return null_;
            return e;
        }
    }
    return e;
}

Element* Tenseur_float::reversion(LispE* lisp, Element* value, long pos, long axis, bool init) {
    if (pos == axis)
        return value->reverse(lisp,true);
    
    if (pos == shape.size() -1)
        return lisp->provideNumbers((Numbers*)value);
    
    Element* r;
    if (init) {
        r = new Tenseur_float;
        ((Tenseur_float*)r)->shape = shape;
    }
    else
        r = lisp->provideList();
    
    Element* e;
    for (long i = 0; i < shape[pos]; i++) {
        e = reversion(lisp, value->index(i), pos+1, axis, false);
        r->append(e);
    }
    return r;
}

Element* Tenseur_float::rotate(LispE* lisp, long axis) {
    return reversion(lisp, this, 0, axis, true);
}
