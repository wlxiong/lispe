/*
 *  LispE
 *
 * Copyright 2020-present NAVER Corp.
 * The 3-Clause BSD License
 */
//  elements.h
//
//

#ifndef elements_h
#define elements_h

#include "tools.h"
#include "vecte.h"
#include <set>

#ifdef MACDEBUG
extern vector<Element*> __indexes;
#endif

//The status to be able to manage the life cycle of an object
const uint16_t s_destructible =  0;
//The 14th bit is set to 1 in protect mode (see LispE::eval)
const uint16_t s_protect = 0x4000;
//The 14th and the 15th bits are set to 1 in constant mode
const uint16_t s_constant = 0xC000;

class LispE;
class Listincode;
class Numbers;
class Integers;
class Floats;
class Strings;

typedef enum {
    //Default values
    v_null, v_emptylist, v_emptyatom, v_true,
    
    //Default types
    t_emptystring, t_operator, t_atom, t_float, t_short, t_integer, t_number,
    t_string, t_plus_string, t_minus_string, t_minus_plus_string,
    t_set, t_setn, t_seti, t_sets, t_floats, t_shorts, t_integers, t_numbers, t_strings,
    t_list, t_llist, t_matrix, t_tensor, t_matrix_float, t_tensor_float,
    t_dictionary, t_dictionaryi, t_dictionaryn, t_heap, t_data, t_maybe,
    t_pair, t_error, t_function, t_library_function, t_pattern, t_lambda, t_thread, 
    
    //System instructions
    l_void, l_set_max_stack_size, l_addr_, l_trace, l_eval, l_use, l_terminal, l_link, l_debug_function,
    
    //Default Lisp instructions
    l_number, l_float, l_string, l_short, l_integer, l_atom,
        
    //threads
    l_lock, l_waiton, l_trigger, l_threadstore, l_threadretrieve, l_threadclear,
    
    //Recording in the stack or in memory
    l_sleep, l_wait,
    l_lambda, l_defun, l_infix, l_dethread, l_deflib, l_deflibpat, l_defpat, l_defmacro, l_lib, l_self,l_label,
    l_setq, l_setg, l_index, l_at_index, l_set_at, l_extract, l_set_range,
    l_block, l_root, l_elapse,
    l_if, l_ife,  l_ncheck, l_check, l_cond, l_select, l_switch,
    l_catch, l_throw, l_maybe,
    
    //Check values
    l_atomp, l_numberp, l_consp, l_emptyp, l_zerop, l_nullp, l_stringp,
    l_quote, l_quoted, l_emptylist,
    //Numerical operations
    l_sign, l_signp, l_minus_plus,
    l_plus, l_minus, l_multiply, l_divide,
    l_leftshift, l_rightshift,
    l_bitand, l_bitor, l_bitxor, l_bitandnot,
    l_bitnot, l_power, l_mod,
    l_listand, l_listor, l_listxor,
    
    l_plusequal, l_minusequal, l_multiplyequal,  l_powerequal,
    l_leftshiftequal, l_rightshiftequal,
    l_bitandequal, l_bitandnotequal, l_bitorequal, l_bitxorequal,
    l_divideequal,l_modequal,
    l_concatenate, l_sum, l_product, l_stringf, l_size,
    l_and, l_or, l_xor, l_not, l_eq, l_neq,
    l_equal, l_equalonezero, l_different, l_lower, l_greater, l_lowerorequal,l_greaterorequal, l_minmax, l_min, l_max, l_compare,
    
    l_innerproduct, l_matrix, l_tensor, l_matrix_float, l_tensor_float, l_outerproduct, l_factorial, l_iota, l_iota0,
    l_reduce, l_scan, l_backreduce, l_backscan, l_rho, l_rank, l_irank,
    l_member, l_transpose, l_invert, l_determinant, l_solve, l_ludcmp, l_lubksb,
    
    //Comparisons
        
    l_in, l_search, l_revertsearch, l_count, l_replaceall, l_searchall, l_cyclic, l_car, l_cdr, l_cadr, l_last, l_flip,
    l_fread, l_fwrite, l_fappend,
    
    //mutable operations
    l_key, l_keyn, l_keyi, l_keys, l_values, l_pop, l_popfirst, l_poplast,
    l_to_list, l_to_llist, l_list, l_llist, l_heap, l_cons, l_consb, l_flatten, l_nconc, l_nconcn, l_push, l_pushfirst, l_pushlast, l_insert, l_extend,
    l_unique, l_clone, l_rotate,
    l_numbers, l_floats, l_shorts, l_integers, l_strings, l_set, l_setn, l_seti, l_sets,
    l_dictionary, l_dictionaryi, l_dictionaryn,
    
    //Display values
    l_print, l_println, l_printerr, l_printerrln, l_prettify, l_bodies,
    
    l_mark, l_resetmark,
    l_while, l_loop, l_loopcount, l_range, l_rangein, l_irange, l_irangein, l_multiloop, l_polyloop,
    l_atoms, l_atomise, l_join, l_sort,
    l_load, l_input, l_getchar, l_pipe, l_type,  l_return, l_break, l_reverse,
    l_apply, l_maplist, l_filterlist, l_droplist, l_takelist, l_mapping, l_checking, l_folding,
    l_composenot, l_data, l_compose, l_map, l_filter, l_take, l_repeat, l_cycle, l_replicate, l_drop, l_takewhile, l_dropwhile,
    l_for, l_foldl, l_scanl, l_foldr, l_scanr, l_foldl1, l_scanl1, l_foldr1, l_scanr1,
    l_zip, l_zipwith,
    c_opening, c_closing, c_opening_bracket, c_closing_bracket, c_opening_data_brace, c_opening_brace, c_closing_brace, c_colon,
    e_error_brace, e_error_bracket, e_error_parenthesis, e_error_string, e_no_error,
    t_comment, l_final
} lisp_code;

//------------------------------------------------------------------------------------------

const unsigned long P_NONE = 1 << 0;
const unsigned long P_ONE = 1 << 1;
const unsigned long P_TWO = 1 << 2;
const unsigned long P_THREE = 1 << 3;
const unsigned long P_FOUR = 1 << 4;
const unsigned long P_FIVE = 1 << 5;
const unsigned long P_SIX = 1 << 6;
const unsigned long P_SEVEN = 1 << 7;
const unsigned long P_EIGHT = 1 << 8;
const unsigned long P_NINE = 1 << 9;
const unsigned long P_TEN = 1 << 10;
const unsigned long P_ELEVEN = 1 << 11;
const unsigned long P_TWELVE = 1 << 12;
const unsigned long P_THIRTEEN = 1 << 13;
const unsigned long P_FOURTEEN = 1 << 14;
const unsigned long P_FIFTEEN = 1 << 15;
const unsigned long P_FULL = -1;
const unsigned long P_ATLEASTONE = P_FULL^P_NONE;
const unsigned long P_ATLEASTTWO = P_ATLEASTONE^P_ONE;
const unsigned long P_ATLEASTTHREE = P_ATLEASTTWO^P_TWO;
const unsigned long P_ATLEASTFOUR = P_ATLEASTTHREE^P_THREE;
const unsigned long P_ATLEASTFIVE = P_ATLEASTFOUR^P_FOUR;
const unsigned long P_ATLEASTSIX = P_ATLEASTFIVE^P_FIVE;
const unsigned long P_ATLEASTSEVEN = P_ATLEASTSIX^P_SIX;
const unsigned long P_ATLEASTEIGHT = P_ATLEASTSEVEN^P_SEVEN;
const unsigned long P_ATLEASTNINE = P_ATLEASTEIGHT^P_EIGHT;
const unsigned long P_ATLEASTTEN = P_ATLEASTNINE^P_NINE;
const unsigned long P_ATLEASTELEVEN = P_ATLEASTTEN^P_TEN;
const unsigned long P_ATLEASTTWELVE = P_ATLEASTELEVEN^P_ELEVEN;
const unsigned long P_ATLEASTTHIRTEEN = P_ATLEASTTWELVE^P_TWELVE;
const unsigned long P_ATLEASTFOURTEEN = P_ATLEASTTHIRTEEN^P_THIRTEEN;
const unsigned long P_ATLEASTFIFTEEN = P_ATLEASTFOURTEEN^P_FOURTEEN;

//------------------------------------------------------------------------------------------
//false_ is actually a bit misleading as it is an alias to null_
#define false_ lisp->n_null
#define true_ lisp->n_true
#define null_ lisp->n_null

#define quote_ lisp->delegation->_QUOTE

#define emptyatom_ lisp->delegation->_EMPTYATOM
#define emptystring_ lisp->delegation->_EMPTYSTRING
#define emptylist_ lisp->delegation->_EMPTYLIST
#define emptydictionary_ lisp->delegation->_EMPTYDICTIONARY

#define minusone_ lisp->delegation->_MINUSONE
#define zero_ lisp->n_zero
#define one_ lisp->n_one
#define two_ lisp->delegation->_TWO

#define booleans_ lisp->_BOOLEANS
#define numbools_ lisp->delegation->_NUMERICAL_BOOLEANS

#define terminal_ lisp->delegation->_TERMINAL

#define separator_ lisp->delegation->_LISTSEPARATOR

#define error_ lisp->delegation->_ERROR
#define break_ lisp->delegation->_BREAK

#define check_mismatch -2
#define check_ok -1

//------------------------------------------------------------------------------------------
#define _releasing(f) f->release();f=null_
//------------------------------------------------------------------------------------------
class List;

class Element {
public:
    
    short type;
#ifdef MACDEBUG
    long __idx;
    lisp_code __lc;
#endif
    uint16_t status;
    
    Element(short ty) {
        type = ty;
#ifdef MACDEBUG
        __lc = (lisp_code)type;
        __idx = __indexes.size();
        __indexes.push_back(this);
#endif
        status = s_destructible;
    }
    
    Element(short ty, uint16_t s) {
        type = ty;
#ifdef MACDEBUG
        __lc = (lisp_code)type;
        __idx = __indexes.size();
        __indexes.push_back(this);
#endif 
        status = s;
    }
    
    virtual ~Element() {
#ifdef MACDEBUG
        __indexes[__idx] = NULL;
#endif
    }
    
    virtual void clean() {}
    virtual bool garbageable() {
        return true;
    }
    
    void generate_body_from_macro(LispE* lisp, Listincode* code, unordered_map<short,Element*>& dico_variables);
    void replaceVariableNames(LispE* lisp, unordered_map<short, Element*>& names);
    bool replaceVariableNames(LispE* lisp);

    virtual bool isArgumentFunction() {
        return false;
    }
    
    virtual Element* transformargument(LispE* lisp) {
        return this;
    }

    inline bool is_protected() {
        return (status & s_protect);
    }

    inline bool not_protected() {
        return !(status & s_protect);
    }

    virtual void increment() {
        status += not_protected();
    }

    virtual void decrement() {
        status -= not_protected();
        if (!status)
            delete this;
    }

    virtual void incrementstatus(uint16_t nb) {
        status += nb * not_protected();
    }
    
    virtual void decrementstatus(uint16_t nb) {
        status -= nb * not_protected();
        if (!status)
            delete this;
    }
    
    //The status is decremented without destroying the element.
    virtual void decrementkeep() {
        status -= not_protected();
    }

    virtual void garbaging_values(LispE*) {}
    
    virtual char isPureList() {
        return 0;
    }

    virtual char isPureList(long& x, long& y) {
        return 0;
    }
    
    virtual void getShape(vecte<long>& sz) {}

    virtual Element* transposed(LispE* lisp) {
        return this;
    }

    virtual Element* invert_sign(LispE* lisp);
    
    virtual Element* rotate(LispE* lisp, long axis) {
        return reverse(lisp, true);
    }

    virtual Element* asList(LispE* lisp);
    
    virtual void concatenate(LispE* lisp, Element* e) {}

    virtual Element* last_element(LispE* lisp);
    virtual Element* last() {
        return this;
    }
    
    virtual Element* unique(LispE* lisp) {
        return this;
    }

    virtual Element* rotate(bool left) {
        return this;
    }

    virtual void append(LispE* lisp, u_ustring& k) {}
    virtual void append(LispE* lisp, double v) {}
    virtual void append(LispE* lisp, long v) {}
    
    virtual void push_element(LispE* lisp, List* l);
    virtual void push_element_front(LispE* lisp, List* l);
    virtual void push_element_back(LispE* lisp, List* l);
    
    /*
     Duplication is forced
     unless it is for status == s_destructible
     copy is used in operations such as
     plus, minus etc. as a receptacle for calculations
     and return this object as the result of the calculation.
     If the status is s_destructible, this
     means that we can use this object as a receptacle for the calculation
     Thus in the following code:
     (setq r 100)
     (+ r 10 20)
     We want to prevent 'r' from changing its value, knowing that r->status == s_label
     
     We also use copy to stack:
     
     (setq r '(1 2 3))
     (setq l r) # call for copy, r will be duplicated
     (setq r 10) # the value of l is not lost
     
     We also use copy to manage list mutability cases.
     and dictionaries.
     unless it is for status == s_destructible
     Thus in the following code:
     
     (setq l '(1 2 3))
     (push l 10)
     
     We want to prevent the saved list: '(1 2 3) from being modified.
     Without a copy, we risk a side effect, if this code is
     in a function. So we will call 'copy' in the setq...
     
     */

    virtual bool element_container() {
        return false;
    }
    
    Element* duplicate() {
        if (!status)
            return this;
        return copying(true);
    }
    
    virtual Element* fullcopy() {
        return copying(true);
    }

    virtual Element* copyatom(LispE* lisp, uint16_t s) {
        return this;
    }

    virtual Element* newInstance() {
        return this;
    }

    virtual Element* newInstance(Element* v) {
        return this;
    }

    virtual bool checkShape(long depth, vecte<long>& sz) {
        return false;
    }

    virtual Element* copying(bool duplicate = true) {
        return this;
    }
    
    virtual Element* rank(LispE* lisp, vecte<long>&);
    
    //We only duplicate constant containers...
    virtual Element* duplicate_constant(LispE* lisp, bool pair = false) {
        return this;
    }
    
    virtual void flatten(LispE*, List* l);
    virtual void flatten(LispE*, Numbers* l);
    virtual void flatten(LispE*, Floats* l);
    
    virtual void* begin_iter() {
        return NULL;
    }
    
    virtual Element* next_iter(LispE* lisp, void* it);
    virtual Element* next_iter_exchange(LispE* lisp, void* it);
    
    virtual void clean_iter(void* it) {}
    virtual bool check_element(LispE* lisp, Element* element_value);
    virtual Element* search_element(LispE*, Element* element_value, long idx);
    virtual Element* search_all_elements(LispE*, Element* element_value, long idx);
    virtual Element* replace_all_elements(LispE*, Element* element_value, Element* remp);
    virtual Element* count_all_elements(LispE*, Element* element_value, long idx);
    virtual Element* search_reverse(LispE*, Element* element_value, long idx);
    
    virtual Element* list_and(LispE*, Element* value);
    virtual Element* list_xor(LispE*, Element* value);
    virtual Element* list_or(LispE*, Element* value);

    virtual bool compare_string(LispE* lisp, u_ustring& u) {
        return (u == asUString(lisp));
    }
    
    virtual char check_match(LispE* lisp, Element* value) {
        return check_ok*(value == this);
    }
    
    virtual bool check_arity(LispE* lisp, unsigned long);
    virtual void setmark(bool v) {}
    virtual bool mark() {return false;}
    virtual void setusermark(bool v) {}
    virtual bool usermark() {return false;}
    virtual void resetusermark() {}
    
    virtual bool unify_kleene(LispE* lisp, Element* value, Element* current, long& i, long& r, bool record) {
        return (i < value->size() && unify(lisp, value->index(i), record));
    }
    
    virtual bool isequal(LispE* lisp, Element* value);
    virtual bool unify(LispE* lisp, Element* value, bool record);
    virtual Element* check_member(LispE*, Element* s) {
        return this;
    }
    
    virtual void setterminal(char v = 1) {}
    /*
     This copy version has two purposes:
     
     a) In the case of a nulemic value or a string, we want to be able to save it as a constant.
     in the appropriate pool. This happens especially when a value is stored in a container after an arithmetic operation.
     
     b) in the case of containers, copy only duplicates the object if it is a constant.
     This is a case that appears mainly for instructions such as push, key and keys that can locally modify an object (side effect).
     but must not modify a value stored in the garbage collector from the compilation.
     */
        
    virtual Element* quoting() {
        return this;
    }
    
    virtual char unquoting() { return 0;}
    
    virtual void setquoted(char q) {}
    
    void prettyfying(LispE* lisp, string& code);
    string prettify(LispE* lisp);
    
    virtual void reserve(long sz) {}
    
    virtual string toString(LispE* lisp) {
        string s;
        u_ustring w = asUString(lisp);
        s_unicode_to_utf8(s, w);
        return s;
    }

    virtual u_ustring asUString(LispE* lisp) {
        return w_to_u(asString(lisp));
    }

    virtual Element* loop(LispE* lisp, short label,  List* code);
    virtual wstring stringInList(LispE* lisp) {
        return asString(lisp);
    }

    virtual u_ustring stringInUList(LispE* lisp) {
        return asUString(lisp);
    }

    virtual wstring jsonString(LispE* lisp) {
        return stringInList(lisp);
    }
    
    virtual wstring asString(LispE* lisp) {
        return L"";
    }
    
    virtual double asNumber() {
        return 0;
    }

    virtual float asFloat() {
        return 0;
    }

    virtual short asShort() {
        return 0;
    }
    
    virtual long asInteger() {
        return 0;
    }
    
    virtual int asInt() {
        return (int)asInteger();
    }
    
    virtual void setvalue(double v) {}
    virtual void setvalue(long v) {}
    virtual void setvalue(float v) {}
    virtual void setvalue(short v) {}
    virtual void setvalue(u_ustring& v) {}
    
    virtual bool Boolean() {
        return true;
    }
    
    Element* Boolean(LispE* lisp);
    
    virtual void append(Element* e) {}
    virtual Element* insert(LispE*, Element* e, long idx);
    virtual void insertion(Element* e, long idx) {}
    virtual void front(Element* e) {}
    virtual void beforelast(Element* e) {}
    
    virtual void appendraw(Element* e) {}
    virtual Element* replace(LispE* lisp, long i, Element* e);
    virtual Element* replace(LispE* lisp, Element* i, Element* e) {
        return replace(lisp, i->asInteger(), e);
    }
    
    virtual void replacing(long i, Element* e) {}
    
    virtual Element* release_but_last() {
        return this;
    }
    
    virtual void change(long i, Element* e) {}
    virtual void changelast(Element* e) {}
    
    virtual Element* composing(LispE*, bool compose) {
        return this;
    }
    
    virtual Element* eval(LispE*) {
        return this;
    }
    
    virtual bool isContainer() {
        return false;
    }
    
    virtual bool isOperator() {
        return false;
    }
    
    virtual bool isExecutable(LispE*) {
        return false;
    }
    
    virtual bool isInstruction() {
        return false;
    }
    
    virtual bool isFunction() {
        return false;
    }

    virtual bool isPureAtom() {
        return false;
    }

    virtual bool isEmpty() {
        return true;
    }
    
    virtual bool isNonOperatorAtom() {
        return false;
    }
    
    virtual bool isBreak() {
        return false;
    }
    
    virtual bool isAtom() {
        return false;
    }
    
    virtual bool isError() {
        return false;
    }
    
    virtual bool isString() {
        return false;
    }
    
    virtual bool isList() {
        return false;
    }

    virtual bool isSet() {
        return false;
    }

    virtual bool isLambda() {
        return false;
    }
    
    virtual bool isValueList() {
        return false;
    }

    virtual bool isNotEmptyList() {
        return false;
    }
    
    virtual bool isInteger() {
        return false;
    }
    
    virtual bool isNumber() {
        return false;
    }
    
    virtual double checkNumber(LispE* lisp);
    virtual long checkInteger(LispE* lisp);
    virtual short checkShort(LispE* lisp);
    virtual float checkFloat(LispE* lisp);

    virtual bool isDictionary() {
        return false;
    }
    
    virtual Element* extraction(LispE* lisp, List*);
    virtual Element* replace_in(LispE* lisp, List*);
    
    virtual Element* charge(LispE*,string chemin);
    
    virtual Element* join_in_list(LispE* lisp, u_ustring& sep);
    
    virtual Element* dictionary(LispE* lisp) {
        return this;
    }
    
    virtual void reversechoice() {}
    
    virtual bool verify() {
        return true;
    }
    
    virtual void protecting(bool protection, LispE* lisp) {
        if (protection) {
            if (status == s_constant)
                status = s_protect;
        }
        else {
            if (status == s_protect)
                status = s_destructible;
        }
    }
    
    virtual void release() {
        if (!status)
            delete this;
    }
    
    virtual bool equalvalue(long n) {
        return false;
    }
    
    virtual bool equalvalue(double v) {
        return false;
    }
    
    virtual bool equalvalue(u_ustring& v) {
        return false;
    }

    virtual bool egal(Element* e);
    Element* lessegal(LispE* lisp, Element* e);
    
    virtual Element* equal(LispE* lisp, Element* e);
    virtual Element* less(LispE* lisp, Element* e);
    virtual Element* compare(LispE* lisp, Element* e);
    virtual Element* lessorequal(LispE* lisp, Element* e);
    virtual Element* more(LispE* lisp, Element* e);
    virtual Element* moreorequal(LispE* lisp, Element* e);
    
    virtual bool is_cyclic() {
        return false;
    }
    
    virtual Element* cadr(LispE*,Element*);
    virtual Element* car(LispE*);
    virtual Element* cdr(LispE*);
    virtual Element* cadr(LispE*, u_ustring& actions);
    
    virtual long argumentsize(LispE*, long sz) {
        return -1;
    }
    
    virtual long default_insertion() {
        return size();
    }
    
    virtual long size() {
        return 0;
    }
    
    virtual long shapesize() {
        return 0;
    }
    
    virtual short function_label();
    
    virtual short label() {
        return type;
    }
    
    virtual short type_element() {
        return type;
    }
    
    virtual Element* thekeys(LispE* lisp);
    virtual Element* thevalues(LispE* lisp);
    
    virtual Element* minimum(LispE*);
    virtual Element* maximum(LispE*);
    virtual Element* minmax(LispE*);

    virtual Element* value_from_index(LispE*, long i);
    
    virtual Element* value_on_index(LispE*, Element* idx);
    virtual Element* value_on_index(LispE*, long i);
    virtual Element* value_on_index(wstring& k, LispE* l);
    virtual Element* value_on_index(u_ustring& k, LispE* l);
    virtual Element* value_on_index(double k, LispE* l);
    
    virtual Element* index(long i) {
        return this;
    }

    virtual Element* protected_index(LispE*, Element* k);
    virtual Element* protected_index(LispE*, double k);
    virtual Element* protected_index(LispE*, long i);
    virtual Element* protected_index(LispE*, u_ustring&);


    virtual Element* checkkey(LispE*, Element* e);
    
    virtual void recording(string&, Element*) {}
    virtual void recording(u_ustring&, Element*) {}
    virtual void recording(double, Element*) {}
    virtual void recording(long, Element*) {}

    virtual bool remove(LispE*, Element*) {
        return false;
    }
    
    virtual bool remove(double) {
        return false;
    }
    
    virtual bool remove(long) {
        return false;
    }

    virtual bool removefirst() {
        return false;
    }

    virtual bool removelast() {
        return false;
    }

    virtual bool remove(wstring&) {
        return false;
    }
    
    virtual bool remove(u_ustring&) {
        return false;
    }
    
    virtual Element* reverse(LispE*, bool duplique = true);
    
    virtual Element* bit_not(LispE* l);
    virtual Element* bit_and(LispE* l, Element* e);
    virtual Element* bit_and_not(LispE* l, Element* e);
    virtual Element* bit_or(LispE* l, Element* e);
    virtual Element* bit_xor(LispE* l, Element* e);
    virtual Element* plus(LispE* l, Element* e);
    virtual Element* minus(LispE* l, Element* e);
    virtual Element* multiply(LispE* l, Element* e);
    virtual Element* divide(LispE* l, Element* e);
    virtual Element* mod(LispE* l, Element* e);
    virtual Element* power(LispE* l, Element* e);
    virtual Element* leftshift(LispE* l, Element* e);
    virtual Element* rightshift(LispE* l, Element* e);
    
    virtual Element* plus_direct(LispE* lisp, Element* e) {
        return plus(lisp, e);
    }
    virtual Element* minus_direct(LispE* lisp, Element* e) {
        return minus(lisp, e);
    }
    virtual Element* multiply_direct(LispE* lisp, Element* e) {
        return multiply(lisp, e);
    }
    
    virtual Element* divide_direct(LispE* lisp, Element* e) {
        return divide(lisp, e);
    }

    virtual Element* argumentvalue() {
        return NULL;
    }
};

class Error : public Element {
public:
    
    wstring message;
    
    Error(string m) : Element(t_error) {
        s_utf8_to_unicode(message, USTR(m), m.size());
    }
    
    Error(wstring m) : message(m), Element(t_error) {}
    
    Error(wstring m, uint16_t s) : message(m), Element(t_error,s) {}

    Error(u_ustring m) : Element(t_error) {
        message = _u_to_w(m);
    }
    
    Error(u_ustring m, uint16_t s) : Element(t_error,s) {
        message = _u_to_w(m);
    }

    wstring asString(LispE* lisp);
    
    bool Boolean() {
        return false;
    }
    
    bool isError() {
        return true;
    }
};

class Maybe : public Element {
public:
    
    u_ustring message;
    
    Maybe(LispE* lisp, Element* e) : message(e->asUString(lisp)), Element(t_maybe) {}
    
    bool unify(LispE* lisp, Element* value, bool record) {
        return (value->label() == t_maybe);
    }

    bool isequal(LispE* lisp, Element* value) {
        return (value->label() == t_maybe);
    }

    u_ustring asUString(LispE* lisp) {
        return message;
    }
    
    wstring asString(LispE* lisp) {
        return _u_to_w(message);
    }
    
    Element* equal(LispE* lisp, Element* e);
    bool egal(Element* e);
};

class Atome : public Element {
public:
    u_ustring name;
    short atome;
    
    Atome(short a, u_ustring w) : name(w), atome(a), Element(t_atom) {}
    Atome(short a, uint16_t s, u_ustring w) : name(w), atome(a), Element(t_atom, s) {}

    Element* transformargument(LispE* lisp);
    
    bool garbageable() {
        return false;
    }
    
    u_ustring asUString(LispE* lisp) {
        return name;
    }

    short function_label() {
        return atome;
    }
    
    //Atoms cannot be present in the garbage
    void protecting(bool protection, LispE* lisp) {}

    wstring asString(LispE* lisp) {
        return _u_to_w(name);
    }
    
    wstring jsonString(LispE* lisp) {
        return wjsonstring(name);
    }
    
    Element* eval(LispE* lisp);
    
    virtual short label() {
        return atome;
    }
    
    bool isPureAtom() {
        return true;
    }
    
    bool isNonOperatorAtom() {
        return true;
    }
    
    bool isAtom() {
        return true;
    }
    
    Element* equal(LispE* lisp, Element* e);
    bool egal(Element* e);
    
    bool Boolean() {
        return atome;
    }
    
    double asNumber() {
        return Boolean();
    }

    float asFloat() {
        return Boolean();
    }

    short asShort() {
        return Boolean();
    }
    
    long asInteger() {
        return Boolean();
    }
    
    int asInt() {
        return Boolean();
    }
        
    virtual char check_match(LispE* lisp, Element* value) {
        return check_ok*(atome == value->label());
    }
    
    void release() {}
    void decrementstatus(uint16_t nb) {}
    void decrementkeep() {}
    void incrementstatus(uint16_t nb) {}
    void increment() {}
    void decrement() {}

    bool unify(LispE* lisp, Element* value, bool record);
    bool isequal(LispE* lisp, Element* value);
    bool isExecutable(LispE* lisp);
    
};

//all atoms between t_atom to t_error are Atomtype
class Atomtype : public Atome {
public:

    Atomtype(short a, u_ustring w) : Atome(a, w) {}
    Atomtype(short a, uint16_t s, u_ustring w) : Atome(a, s, w) {}

    char check_match(LispE* lisp, Element* value) {
        return check_ok*(atome == value->type_element());
    }

};

class Atomnotlabel : public Atome {
public:
    
    Atomnotlabel(short a, u_ustring w) : Atome(a, w) {}
    Atomnotlabel(short a, uint16_t s, u_ustring w) : Atome(a, s, w) {}
    
    short label() {
        return v_null;
    }
    
};

class Atomekleene : public Atome {
public:
    char action;
    
    Atomekleene(short a, u_ustring w, char act) : Atome(a, w) {
        action = act;
    }
    
    bool unify_kleene(LispE* lisp, Element* value, Element* current, long& i, long& r, bool record);
    
};


class Operator : public Element {
public:
    u_ustring name;
    
    Operator(short c, u_ustring w) : name(w), Element(c) {}

    u_ustring asUString(LispE* lisp) {
        return name;
    }

    bool garbageable() {
        return false;
    }
    
    //Atoms cannot be present in the garbage
    void protecting(bool protection, LispE* lisp) {}

    wstring asString(LispE* lisp) {
        return _u_to_w(name);
    }

    wstring jsonString(LispE* lisp) {
        return wjsonstring(name);
    }

    bool isAtom() {
        return true;
    }
    
    short label() {
        return type;
    }
    
    //Notice that we return t_atom
    //This function is used in Atomtype::check_match
    //so that 'key' for instance is considered as an atom
    short type_element() {
        return t_atom;
    }

    bool isInstruction() {
        return true;
    }

    bool isOperator() {
        return true;
    }
    
    bool isExecutable(LispE*) {
        return true;
    }

    void release() {}
    void decrementstatus(uint16_t nb) {}
    void decrementkeep() {}
    void incrementstatus(uint16_t nb) {}
    void increment() {}
    void decrement() {}

};


class Instruction : public Element {
public:
    u_ustring name;
    
    Instruction(short c, u_ustring w) : name(w), Element(c) {}
    
    u_ustring asUString(LispE* lisp) {
        return name;
    }

    bool garbageable() {
        return false;
    }
    
   //Atoms cannot be present in the garbage
    void protecting(bool protection, LispE* lisp) {}

    wstring asString(LispE* lisp) {
        return _u_to_w(name);
    }

    wstring jsonString(LispE* lisp) {
        return wjsonstring(name);
    }

    bool isInstruction() {
        return true;
    }
    
    bool isNonOperatorAtom() {
        return true;
    }
    
    bool isAtom() {
        return true;
    }
    
    short type_element() {
        return t_atom;
    }

    short label() {
        return type;
    }
    
    bool isExecutable(LispE*) {
        return true;
    }
    
    Element* eval(LispE*);
    
    void release() {}
    void decrementstatus(uint16_t nb) {}
    void decrementkeep() {}
    void incrementstatus(uint16_t nb) {}
    void increment() {}
    void decrement() {}

};

class Returnpool : public Element {
public:
    Element* value;
    LispE* lisp;

    Returnpool(LispE* l);
    Returnpool(LispE* l, Element* e) : lisp(l), Element(l_return) {
        value = e ;
    }

    void decrementstatus(uint16_t nb);
    void decrement();
    
    void release();

    u_ustring asUString(LispE* lisp) {
        return value->asUString(lisp);
    }

    //Atoms cannot be present in the garbage
    void protecting(bool protection, LispE* lisp) {}

    Returnpool* set(Element* e) {
        value = e;
        return this;
    }
    
    wstring asString(LispE* lisp) {
        return value->asString(lisp);
    }
    
    double asNumber() {
        return value->asNumber();
    }
    
    float asFloat() {
        return value->asFloat();
    }

    long asInteger() {
        return value->asInteger();
    }
    
    short asShort() {
        return value->asShort();
    }
    
    int asInt() {
        return value->asInt();
    }
    
    Element* eval(LispE* lisp) {
        return value;
    }
    
    bool isInstruction() {
        return true;
    }
    
    bool isExecutable(LispE* lisp) {
        return true;
    }

};

class Cadr : public Element {
public:
    
    u_ustring action;
    
    Cadr(string c) : Element(l_cadr) {
        c = c.substr(1, c.size()-2);
        for (long i = 0; i < c.size(); i++)
            action += (wchar_t)c[i];
    }
    
    Cadr(u_ustring c) : Element(l_cadr) {
        action = c.substr(1, c.size()-2);
    }

    //Atoms cannot be present in the garbage
    void protecting(bool protection, LispE* lisp) {}

    u_ustring asUString(LispE* lisp) {
        return(U"c" + action + U"r");
    }
    
    wstring asString(LispE* lisp) {
        return(L"c" + _u_to_w(action) + L"r");
    }
    
    Element* cadr(LispE* lisp, Element* e) {
        return e->cadr(lisp, action);
    }
    
    bool isAtom() {
        return true;
    }
    
    short type_element() {
        return t_atom;
    }

    bool isExecutable(LispE* lisp) {
        return true;
    }

    bool isInstruction() {
        return true;
    }
};

class Float : public Element {
public:
  
    float number;
    Float(float d) : Element(t_float) {
        number = d;
    }
    
    Float(float d, uint16_t s) : number(d), Element(t_float, s) {}
    
    bool equalvalue(float v) {
        return (v == number);
    }

    Element* invert_sign(LispE* lisp);
    Element* reverse(LispE*, bool duplique = true);
    
    bool egal(Element* e);
    Element* equal(LispE* lisp, Element* e);
    Element* less(LispE* lisp, Element* e);
    Element* compare(LispE* lisp, Element* e);
    Element* lessorequal(LispE* lisp, Element* e);
    Element* more(LispE* lisp, Element* e);
    Element* moreorequal(LispE* lisp, Element* e);
    
    void setvalue(float v) {number = v;}

    
    char check_match(LispE* lisp, Element* value) {
        return check_ok*(number == value->asFloat());
    }
    
    bool unify(LispE* lisp, Element* value, bool record) {
        return (value == this || value->asFloat() == number);
    }
    
    bool isequal(LispE* lisp, Element* value) {
        return (value == this || value->asFloat() == number);
    }

    bool isNumber() {
        return true;
    }
    
    double checkNumber(LispE* lisp) {
        return number;
    }
    
    long checkInteger(LispE* lisp) {
        return number;
    }
    
    short checkShort(LispE* lisp) {
        return number;
    }
    
    float checkFloat(LispE* lisp) {
        return number;
    }
    
    wstring asString(LispE* lisp) {
        return convertToWString(number);
    }

    string toString(LispE* lisp) {
        return convertToString(number);
    }

    double asNumber() {
        return number;
    }

    float asFloat() {
        return number;
    }

    short asShort() {
        return (short)number;
    }

    long asInteger() {
        return (long)number;
    }

    int asInt() {
        return (int)number;
    }

    bool Boolean() {
        return (number);
    }
    
    virtual Element* fullcopy() {
        return new Float(number);
    }
    
    virtual Element* copyatom(LispE* lisp, uint16_t s);

    virtual Element* copying(bool duplicate = true) {
        if (!status)
            return this;
        
        return new Float(number);
    }
    
    Element* bit_not(LispE* l);
    Element* bit_and(LispE* l, Element* e);
    Element* bit_and_not(LispE* l, Element* e);
    Element* bit_or(LispE* l, Element* e);
    Element* bit_xor(LispE* l, Element* e);
    Element* plus(LispE* l, Element* e);
    Element* minus(LispE* l, Element* e);
    Element* multiply(LispE* l, Element* e);
    Element* divide(LispE* l, Element* e);
    Element* mod(LispE* l, Element* e);
    Element* power(LispE* l, Element* e);
    Element* leftshift(LispE* l, Element* e);
    Element* rightshift(LispE* l, Element* e);
    
    Element* plus_direct(LispE* lisp, Element* e);
    Element* minus_direct(LispE* lisp, Element* e);
    Element* multiply_direct(LispE* lisp, Element* e);
    Element* divide_direct(LispE* lisp, Element* e);
};

class Floatpool : public Float {
public:
    LispE* lisp;
    Floatpool(LispE* l, double d) : lisp(l), Float(d) {}
    Floatpool(float d) : lisp(NULL), Float(d, s_constant) {}
    
    inline Floatpool* set(float d) {
        number = d;
        return this;
    }
    
    virtual void decrementstatus(uint16_t nb);
    virtual void decrement();
    virtual void release();
    
    virtual Element* fullcopy();
    virtual Element* copyatom(LispE* lisp, uint16_t s);
    virtual Element* copying(bool duplicate = true);
};

class Number : public Element {
public:
  
    double number;
    Number(double d) : Element(t_number) {
        number = d;
    }
    
    Number(double d, uint16_t s) : number(d), Element(t_number, s) {}
    
    bool equalvalue(double v) {
        return (v == number);
    }

    Element* invert_sign(LispE* lisp);
    Element* reverse(LispE*, bool duplique = true);
    
    bool egal(Element* e);
    Element* equal(LispE* lisp, Element* e);
    Element* less(LispE* lisp, Element* e);
    Element* compare(LispE* lisp, Element* e);
    Element* lessorequal(LispE* lisp, Element* e);
    Element* more(LispE* lisp, Element* e);
    Element* moreorequal(LispE* lisp, Element* e);
    
    void setvalue(double v) {number = v;}

    char check_match(LispE* lisp, Element* value) {
        return check_ok*(number == value->asNumber());
    }
    
    bool unify(LispE* lisp, Element* value, bool record) {
        return (value == this || value->asNumber() == number);
    }
    
    bool isequal(LispE* lisp, Element* value) {
        return (value == this || value->asNumber() == number);
    }
    
    bool isNumber() {
        return true;
    }
    
    double checkNumber(LispE* lisp) {
        return number;
    }
    
    long checkInteger(LispE* lisp) {
        return number;
    }
    
    short checkShort(LispE* lisp) {
        return number;
    }
    
    wstring asString(LispE* lisp) {
        return convertToWString(number);
    }

    string toString(LispE* lisp) {
        return convertToString(number);
    }

    double asNumber() {
        return number;
    }

    float asFloat() {
        return number;
    }

    short asShort() {
        return (short)number;
    }

    long asInteger() {
        return (long)number;
    }
    
    int asInt() {
        return (int)number;
    }

    bool Boolean() {
        return (number);
    }
    
    virtual Element* fullcopy() {
        return new Number(number);
    }
    
    virtual Element* copyatom(LispE* lisp, uint16_t s);

    virtual Element* copying(bool duplicate = true) {
        if (!status)
            return this;
        
        return new Number(number);
    }
    
    Element* bit_not(LispE* l);
    Element* bit_and(LispE* l, Element* e);
    Element* bit_and_not(LispE* l, Element* e);
    Element* bit_or(LispE* l, Element* e);
    Element* bit_xor(LispE* l, Element* e);
    Element* plus(LispE* l, Element* e);
    Element* minus(LispE* l, Element* e);
    Element* multiply(LispE* l, Element* e);
    Element* divide(LispE* l, Element* e);
    Element* mod(LispE* l, Element* e);
    Element* power(LispE* l, Element* e);
    Element* leftshift(LispE* l, Element* e);
    Element* rightshift(LispE* l, Element* e);
    
    Element* plus_direct(LispE* lisp, Element* e);
    Element* minus_direct(LispE* lisp, Element* e);
    Element* multiply_direct(LispE* lisp, Element* e);
    Element* divide_direct(LispE* lisp, Element* e);
};

class Numberpool : public Number {
public:
    LispE* lisp;
    Numberpool(LispE* l, double d) : lisp(l), Number(d) {}
    Numberpool(double d) : lisp(NULL), Number(d, s_constant) {}
    
    inline Numberpool* set(double d) {
        number = d;
        return this;
    }
    
    virtual void decrementstatus(uint16_t nb);
    virtual void decrement();
    virtual void release();
    
    virtual Element* fullcopy();
    virtual Element* copyatom(LispE* lisp, uint16_t s);
    virtual Element* copying(bool duplicate = true);
};

class Constfloat : public Floatpool {
public:
    bool provide;
    
    Constfloat(double d) : Floatpool(d) {
        status = s_constant;
        provide = false;
    }
    
    Constfloat(LispE* lisp, double d) : Floatpool(lisp, d) {
		status = s_constant;
        provide = false;
	}

    bool garbageable() {
        return false;
    }
    
    void incrementstatus(uint16_t nb) {}
    void decrementstatus(uint16_t nb) {}
    void increment() {}
    void decrement() {}

    void release() {}
    
    Element* fullcopy();
    Element* copyatom(LispE* lisp, uint16_t s);
    Element* copying(bool duplicate = true);
    Element* duplicate_constant(LispE* lisp, bool pair = false);
};

class Constnumber : public Numberpool {
public:
    bool provide;
    
    Constnumber(double d) : Numberpool(d) {
        status = s_constant;
        provide = false;
    }
    
    Constnumber(LispE* lisp, double d) : Numberpool(lisp, d) {
        status = s_constant;
        provide = false;
    }

    bool garbageable() {
        return false;
    }
    
    void incrementstatus(uint16_t nb) {}
    void decrementstatus(uint16_t nb) {}
    void increment() {}
    void decrement() {}

    void release() {}
    
    Element* fullcopy();
    Element* copyatom(LispE* lisp, uint16_t s);
    Element* copying(bool duplicate = true);
    Element* duplicate_constant(LispE* lisp, bool pair = false);
};

class Short : public Element {
public:
 
    short integer;
    Short(short d) : Element(t_short) {
        integer = d;
    }
    
    Short(short d, uint16_t s) : integer(d), Element(t_short, s) {}

    bool equalvalue(short n) {
        return (integer == n);
    }
    
    Element* invert_sign(LispE* lisp);
    bool egal(Element* e);
    Element* equal(LispE* lisp, Element* e);
    Element* less(LispE* lisp, Element* e);
    Element* compare(LispE* lisp, Element* e);
    Element* lessorequal(LispE* lisp, Element* e);
    Element* more(LispE* lisp, Element* e);
    Element* moreorequal(LispE* lisp, Element* e);
    
    void setvalue(short v) { integer = v;}

    
    char check_match(LispE* lisp, Element* value) {
        return check_ok*(integer == value->asInteger());
    }
    
    bool unify(LispE* lisp, Element* value, bool record) {
        return (value == this || value->asInteger() == integer);
    }

    bool isequal(LispE* lisp, Element* value) {
        return (value == this || value->asInteger() == integer);
    }

    Element* reverse(LispE*, bool duplique = true);
    
    u_ustring asUString(LispE* lisp) {
        return convertToUString((long)integer);
    }

    wstring asString(LispE* lisp) {
        return convertToWString((long)integer);
    }

    string toString(LispE* lisp) {
        return convertToString((long)integer);
    }

    bool isInteger() {
        return true;
    }

    bool isNumber() {
        return true;
    }
    
    double checkNumber(LispE* lisp) {
        return integer;
    }
    
    long checkInteger(LispE* lisp) {
        return integer;
    }
    
    short checkShort(LispE* lisp) {
        return integer;
    }

    double asNumber() {
        return integer;
    }
    
    float asFloat() {
        return integer;
    }

    short asShort() {
        return integer;
    }

    long asInteger() {
        return integer;
    }
    
    int asInt() {
        return (int)integer;
    }

    bool Boolean() {
        return (integer);
    }
    
    Element* fullcopy() {
        return new Short(integer);
    }

    Element* copyatom(LispE* lisp, uint16_t s) {
        if (status < s)
            return this;
        return new Short(integer);
    }

    // There is a difference between the two copies
    //The first one makes a final copy
    Element* copying(bool duplicate = true) {
        if (!status)
            return this;
        
        return new Short(integer);
    }
    
    Element* bit_not(LispE* l);
    Element* bit_and(LispE* l, Element* e);
    Element* bit_and_not(LispE* l, Element* e);
    Element* bit_or(LispE* l, Element* e);
    Element* bit_xor(LispE* l, Element* e);
    Element* plus(LispE* l, Element* e);
    Element* minus(LispE* l, Element* e);
    Element* multiply(LispE* l, Element* e);
    Element* divide(LispE* l, Element* e);
    Element* mod(LispE* l, Element* e);
    Element* power(LispE* l, Element* e);
    Element* leftshift(LispE* l, Element* e);
    Element* rightshift(LispE* l, Element* e);

    Element* plus_direct(LispE* lisp, Element* e) {
        if (e->type == t_short) {
            integer += ((Short*)e)->integer;
            return this;
        }
        return plus(lisp, e);
    }
    
    Element* minus_direct(LispE* lisp, Element* e) {
        if (e->type == t_short) {
            integer -= ((Short*)e)->integer;
            return this;
        }
        return minus(lisp, e);
    }
    
    Element* multiply_direct(LispE* lisp, Element* e) {
        if (e->type == t_short) {
            integer *= ((Short*)e)->integer;
            return this;
        }
        return multiply(lisp, e);
    }
    
    Element* divide_direct(LispE* lisp, Element* e);

};

class Integer : public Element {
public:
 
    long integer;
    Integer(long d) : Element(t_integer) {
        integer = d;
    }
    
    Integer(long d, uint16_t s) : integer(d), Element(t_integer, s) {}

    bool equalvalue(long n) {
        return (integer == n);
    }
    
    Element* invert_sign(LispE* lisp);
    bool egal(Element* e);
    Element* equal(LispE* lisp, Element* e);
    Element* less(LispE* lisp, Element* e);
    Element* compare(LispE* lisp, Element* e);
    Element* lessorequal(LispE* lisp, Element* e);
    Element* more(LispE* lisp, Element* e);
    Element* moreorequal(LispE* lisp, Element* e);
    
    
    void setvalue(long v) { integer = v;}

    char check_match(LispE* lisp, Element* value) {
        return check_ok*(integer == value->asInteger());
    }
    
    bool unify(LispE* lisp, Element* value, bool record) {
        return (value == this || value->asInteger() == integer);
    }
    
    bool isequal(LispE* lisp, Element* value) {
        return (value == this || value->asInteger() == integer);
    }
    
    Element* reverse(LispE*, bool duplique = true);
    
    u_ustring asUString(LispE* lisp) {
        return convertToUString(integer);
    }

    wstring asString(LispE* lisp) {
        return convertToWString(integer);
    }

    string toString(LispE* lisp) {
        return convertToString(integer);
    }

    bool isInteger() {
        return true;
    }
    
    bool isNumber() {
        return true;
    }
    
    double checkNumber(LispE* lisp) {
        return integer;
    }
    
    long checkInteger(LispE* lisp) {
        return integer;
    }

    short checkShort(LispE* lisp) {
        return integer;
    }

    double asNumber() {
        return integer;
    }
    
    float asFloat() {
        return integer;
    }

    short asShort() {
        return integer;
    }

    long asInteger() {
        return integer;
    }
    
    int asInt() {
        return (int)integer;
    }

    bool Boolean() {
        return (integer);
    }
    
    virtual Element* fullcopy() {
        return new Integer(integer);
    }

    virtual Element* copyatom(LispE* lisp, uint16_t s);

    // There is a difference between the two copies
    //The first one makes a final copy
    virtual Element* copying(bool duplicate = true) {
        if (!status)
            return this;
        
        return new Integer(integer);
    }
    
    Element* bit_not(LispE* l);
    Element* bit_and(LispE* l, Element* e);
    Element* bit_and_not(LispE* l, Element* e);
    Element* bit_or(LispE* l, Element* e);
    Element* bit_xor(LispE* l, Element* e);
    Element* plus(LispE* l, Element* e);
    Element* minus(LispE* l, Element* e);
    Element* multiply(LispE* l, Element* e);
    Element* divide(LispE* l, Element* e);
    Element* mod(LispE* l, Element* e);
    Element* power(LispE* l, Element* e);
    Element* leftshift(LispE* l, Element* e);
    Element* rightshift(LispE* l, Element* e);

    Element* plus_direct(LispE* lisp, Element* e) {
        if (e->type == t_integer) {
            integer += ((Integer*)e)->integer;
            return this;
        }
        return plus(lisp, e);
    }
    
    Element* minus_direct(LispE* lisp, Element* e) {
        if (e->type == t_integer) {
            integer -= ((Integer*)e)->integer;
            return this;
        }
        return minus(lisp, e);
    }
    
    Element* multiply_direct(LispE* lisp, Element* e) {
        if (e->type == t_integer) {
            integer *= ((Integer*)e)->integer;
            return this;
        }
        return multiply(lisp, e);
    }
    
    Element* divide_direct(LispE* lisp, Element* e);

};

class Integerpool : public Integer {
public:
    LispE* lisp;
    Integerpool(LispE* l, long d) : lisp(l), Integer(d) {}
    Integerpool(long d) : lisp(NULL), Integer(d, s_constant) {}

    inline Integerpool* set(long d) {
        integer = d;
        return this;
    }

    virtual void decrementstatus(uint16_t nb);
    virtual void decrement();
    
    virtual void release();
    virtual Element* fullcopy();
    virtual Element* copyatom(LispE* lisp, uint16_t s);
    virtual Element* copying(bool duplicate = true);

};

class Constinteger : public Integerpool {
public:
    bool provide;
    
    Constinteger(long d) : Integerpool(d) {
        provide = false;
        status = s_constant;
    }
    
    Constinteger(LispE* l, double d) : Integerpool(l, d) {
        status = s_constant;
        provide = false;
    }

    bool garbageable() {
        return false;
    }
    
    void incrementstatus(uint16_t nb) {}
    void decrementstatus(uint16_t nb) {}
    void increment() {}
    void decrement() {}

    
    void release() {}
    Element* fullcopy();
    Element* copyatom(LispE* lisp, uint16_t s);
    Element* copying(bool duplicate = true);
    Element* duplicate_constant(LispE* lisp, bool pair = false);
};

class Constshort : public Short {
public:

    Constshort(short d) : Short(d) {
        status = s_constant;
    }
    
    bool garbageable() {
        return false;
    }
    
    void incrementstatus(uint16_t nb) {}
    void decrementstatus(uint16_t nb) {}
    void increment() {}
    void decrement() {}

    
    void release() {}
    Element* fullcopy();
    Element* copyatom(LispE* lisp, uint16_t s);
    Element* copying(bool duplicate = true);
    Element* duplicate_constant(LispE* lisp, bool pair = false);
};

class String : public Element {
public:
  
    u_ustring content;
    
    String() : Element(t_string) {}
    
    String(wchar_t c) : Element(t_string) {
        content = (u_uchar)c;
    }
    String(string c) : Element(t_string) {
        s_utf8_to_unicode(content, USTR(c), c.size());
    }
    String(wstring c) : Element(t_string) {
        content = _w_to_u(c);
    }
    String(wstring c, uint16_t s) : Element(t_string, s) {
        content = _w_to_u(c);
    }
    String(u_ustring c) : Element(t_string) {
        content = c;
    }
    String(u_ustring c, uint16_t s) : Element(t_string, s) {
        content = c;
    }

    void setvalue(u_ustring& v) {
        content = v;
    }

    bool isString() {
        return true;
    }
    
    void push_element(LispE* lisp, List* l);
    void push_element_front(LispE* lisp, List* l);
    void push_element_back(LispE* lisp, List* l);

    void append(Element* e) {
        content += e->asUString(NULL);
    }
        
    Element* cadr(LispE*, u_ustring& actions);
    Element* rotate(bool left);
    
    bool compare_string(LispE*, u_ustring& u) {
        return (content == u);
    }
        
    char check_match(LispE* lisp, Element* value) {
        return check_ok*value->compare_string(lisp, content);
    }
    
    bool unify(LispE* lisp, Element* value, bool record) {
        return (value == this || value->compare_string(lisp, content));
    }
    
    bool isequal(LispE* lisp, Element* value, bool record) {
        return (value == this || value->compare_string(lisp, content));
    }
    
    Element* loop(LispE* lisp, short label,  List* code);
    
    bool isEmpty() {
        return (content == U"");
    }
        
    void* begin_iter() {
        long* n = new long[1];
        n[0] = 0;
        return n;
    }
    
    Element* next_iter(LispE* lisp, void* it);
    Element* next_iter_exchange(LispE* lisp, void* it);

    void clean_iter(void* it) {
        delete (long*)it;
    }

    bool check_element(LispE* lisp, Element* element_value);
    Element* search_element(LispE*, Element* element_value, long idx);
    Element* search_all_elements(LispE*, Element* element_value, long idx);
    Element* replace_all_elements(LispE*, Element* element_value, Element* remp);
    Element* count_all_elements(LispE*, Element* element_value, long idx);
    Element* search_reverse(LispE*, Element* element_value, long idx);
    
    Element* list_and(LispE*, Element* value);
    Element* list_xor(LispE*, Element* value);
    Element* list_or(LispE*, Element* value);

    Element* insert(LispE* lisp, Element* e, long idx);

    bool equalvalue(u_ustring& v) {
        return (v == content);
    }

    Element* replace(LispE* lisp, long i, Element* e);
    
    bool egal(Element* e);
    Element* equal(LispE* lisp, Element* e);
    Element* less(LispE* lisp, Element* e);
    Element* compare(LispE* lisp, Element* e);
    Element* lessorequal(LispE* lisp, Element* e);
    Element* more(LispE* lisp, Element* e);
    Element* moreorequal(LispE* lisp, Element* e);
    
    Element* protected_index(LispE*,long i);
    
    Element* value_from_index(LispE*, long i);
    
    Element* value_on_index(LispE*, long i);
    Element* value_on_index(LispE*, Element* idx);
    Element* protected_index(LispE*, Element* k);
    Element* reverse(LispE*, bool duplique = true);

    Element* last_element(LispE* lisp);
    
    long size() {
        return content.size();
    }
    
    Element* extraction(LispE* lisp, List*);
    Element* replace_in(LispE* lisp, List*);
    
    Element* car(LispE*);
    Element* cdr(LispE*);
    
    
    virtual wstring stringInList(LispE* lisp) {
        return wjsonstring(content);
    }
    
    virtual u_ustring stringInUList(LispE* lisp) {
        return ujsonstring(content);
    }
    
    virtual Element* copyatom(LispE* lisp, uint16_t s);
    
    virtual Element* fullcopy() {
        return new String(content);
    }

    // There is a difference between the two copies
    //The first one makes a final copy
    virtual Element* copying(bool duplicate = true) {
        if (!status)
            return this;
        
        return new String(content);
    }
    
    virtual wstring asString(LispE* lisp) {
        return _u_to_w(content);
    }

    virtual u_ustring asUString(LispE* lisp) {
        return content;
    }

    Element* charge(LispE* lisp, string chemin) {
        std::ifstream f(chemin.c_str(),std::ios::in|std::ios::binary);
        if (f.fail()) {
            string erreur = "Unknown file: ";
            erreur += chemin;
            throw new Error(erreur);
        }
        
        string ch = "";
        string ln;
        while (!f.eof()) {
            getline(f, ln);
            ch += ln + "\n";
        }
        
        content = U"";
        s_utf8_to_unicode(content, USTR(ch), ch.size());
        return this;
    }
    
    bool Boolean() {
        return (content != U"");
    }
    
    double asNumber() {
        return convertingfloathexa(content);
    }
    
    float asFloat() {
        return convertingfloathexa(content);
    }

    short asShort() {
        return (short)convertingfloathexa(content);
    }
    
    long asInteger() {
        return (long)convertingfloathexa(content);
    }
    
    Element* plus(LispE* l, Element* e);
    Element* thekeys(LispE* lisp);
};

class Stringpool : public String {
public:
    LispE* lisp;

    Stringpool(LispE* l) : lisp(l) {}
    Stringpool(LispE* l, u_ustring& d) : lisp(l), String(d) {}
    Stringpool(u_ustring& d) : lisp(NULL), String(d, s_constant) {}
    
    inline Stringpool* set(u_ustring& d) {
        content = d;
        return this;
    }
    
    void decrementstatus(uint16_t nb);
    void decrement();
    
    void release();
    virtual Element* fullcopy();
    virtual Element* copyatom(LispE* lisp, uint16_t s);
    virtual Element* copying(bool duplicate = true);

};


class Conststring : public Stringpool {
public:
    bool provide;
    
    Conststring(u_ustring w) : Stringpool(w) {
        provide = false;
        status = s_constant;
    }
    
	Conststring(LispE* l, u_ustring w) : Stringpool(l, w) {
		status = s_constant;
        provide = false;
	}

    bool garbageable() {
        return false;
    }
    
    void incrementstatus(uint16_t nb) {}
    void decrementstatus(uint16_t nb) {}
    void increment() {}
    void decrement() {}
    void release() {}

    Element* fullcopy();
    Element* copyatom(LispE* lisp, uint16_t s);
    Element* copying(bool duplicate = true);
    Element* duplicate_constant(LispE* lisp, bool pair = false);
};

class InfiniterangeNumber : public Element {
public:
    double initial_value;
    double increment;
    double bound;
    bool infinite_loop;

    InfiniterangeNumber(double v, double i) : Element(l_list) {
        initial_value = v;
        increment = i;
        infinite_loop = true;
        bound = 0;
    }
    
    InfiniterangeNumber(double v, double i, double b) : Element(l_list) {
        initial_value = v;
        increment = i;
        infinite_loop = false;
        bound = b;
    }
    
    inline bool compare(char check, long value) {
        return (!check || (check == -1 && value > bound) || (check == 1 && value < bound));
    }

    bool isContainer() {
        return true;
    }
    
    bool isList() {
        return true;
    }

    wstring asString(LispE* lisp);
    
    Element* loop(LispE* lisp, short label,  List* code);
    Element* car(LispE*);
    Element* cdr(LispE*);
};

class InfiniterangeInteger : public Element {
public:
    long initial_value;
    long increment;
    long bound;
    bool infinite_loop;
    
    InfiniterangeInteger(long v, long i) : Element(l_list) {
        initial_value = v;
        increment = i;
        infinite_loop = true;
        bound = 0;
    }
    
    InfiniterangeInteger(long v, long i, long b) : Element(l_list) {
        initial_value = v;
        increment = i;
        infinite_loop = false;
        bound = b;
    }
    
    inline bool compare(char check, long value) {
        return (!check || (check == -1 && value > bound) || (check == 1 && value < bound));
    }
    
    bool isContainer() {
        return true;
    }
    
    bool isList() {
        return true;
    }
    
    wstring asString(LispE* lisp);

    
    Element* loop(LispE* lisp, short label,  List* code);
    Element* car(LispE*);
    Element* cdr(LispE*);

};


class Infinitelist : public Element {
public:
    Element* value;
    
    Infinitelist(LispE* lisp);
    
    bool isContainer() {
        return true;
    }
    
    bool isList() {
        return true;
    }
    
    u_ustring asUString(LispE* lisp);
    wstring asString(LispE* lisp);

    void append(Element* e) {
        e->increment();
        value->decrement();
        value = e;
    }
    
    Element* loop(LispE* lisp, short label,  List* code);
};

class Cyclelist : public Element {
public:
    Element* value;
    
    Cyclelist(LispE* lisp);
    
    bool isContainer() {
        return true;
    }
    
    bool isList() {
        return true;
    }
    
    void append(Element* e) {
        e->increment();
        value->decrement();
        value = e;
    }
    
    u_ustring asUString(LispE* lisp);
    wstring asString(LispE* lisp);

    Element* loop(LispE* lisp, short label,  List* code);
};


class Dictionary : public Element {
public:
    
    map<u_ustring, Element*> dictionary;
    Element* object;
    bool marking;
    bool usermarking;
    
    Dictionary() : Element(t_dictionary) {
        object = NULL;
        marking = false;
        usermarking =  false;
    }
    
    Dictionary(uint16_t s) : Element(t_dictionary, s) {
        object = NULL;
        marking = false;
        usermarking =  false;
    }
    
    ~Dictionary() {
        //There might be some left over
        for (auto& a : dictionary)
            a.second->decrement();
    }
    
    bool isEmpty() {
        return dictionary.empty();
    }

    bool element_container() {
        return true;
    }
    
    bool isDictionary() {
        return true;
    }
    
    bool isContainer() {
        return true;
    }
    
    void setmark(bool v) {
        marking = v;
    }
    
    bool mark() {
        return marking;
    }

    virtual Element* newInstance() {
        return new Dictionary;
    }

    void* begin_iter() {
        return new std::map<u_ustring, Element*>::iterator(dictionary.begin());
    }
    
    Element* next_iter(LispE* lisp, void* it);
    Element* next_iter_exchange(LispE* lisp, void* it);

    void clean_iter(void* it) {
        delete (std::map<u_ustring, Element*>::iterator*)it;
    }

    void setusermark(bool v) {
        usermarking = v;
    }
    
    bool usermark() {
        return  usermarking;
    }

    void resetusermark() {
        if (marking)
            return;
        marking = true;
        usermarking = false;
        for (auto& a: dictionary) {
            a.second->resetusermark();
        }
        marking = false;
    }

    void garbaging_values(LispE*);
    
    Element* loop(LispE* lisp, short label,  List* code);
    
    Element* minimum(LispE*);
    Element* maximum(LispE*);
    Element* minmax(LispE*);
    
    void flatten(LispE*, List* l);
    
    bool check_element(LispE* lisp, Element* element_value);
    Element* search_element(LispE*, Element* element_value, long idx);
    Element* search_all_elements(LispE*, Element* element_value, long idx);
    Element* replace_all_elements(LispE*, Element* element_value, Element* remp);
    Element* count_all_elements(LispE*, Element* element_value, long idx);
    Element* search_reverse(LispE*, Element* element_value, long idx);    
    Element* checkkey(LispE* lisp, Element* e);

    virtual Element* fullcopy() {
        if (marking)
            return object;
        
        marking = true;
        Dictionary* d = new Dictionary;
        object = d;
        Element* e;
        for (auto& a: dictionary) {
            e = a.second->fullcopy();
            d->dictionary[a.first] = e;
            e->increment();
        }
        marking = false;
        return d;
    }

    virtual Element* copying(bool duplicate = true) {
        if (!is_protected() && !duplicate)
            return this;
        
        Dictionary* d = new Dictionary;
        Element* e;
        for (auto& a: dictionary) {
            e = a.second->copying(false);
            d->dictionary[a.first] = e;
            e->increment();
        }
        return d;
    }
    
    virtual Element* copyatom(LispE* lisp, uint16_t s);

    //In the case of a container for push, key and keyn
    // We must force the copy when it is a constant
    Element* duplicate_constant(LispE* lisp, bool pair = false);
    
    Element* join_in_list(LispE* lisp, u_ustring& sep);
    
    virtual void release() {
        if (!status && !marking) {
            marking = true;
            marking = false;
            delete this;
        }
    }
    
    virtual void decrement() {
        if (is_protected() || marking)
            return;
        
        marking = true;
        
        status--;
        if (!status)
            delete this;
        else
            marking = false;
    }
    

    virtual void decrementstatus(uint16_t nb) {
        if (is_protected() || marking)
            return;
        
        marking = true;
        
        status-=nb;
        if (!status)
            delete this;
        else
            marking = false;
    }
    
    bool unify(LispE* lisp, Element* e, bool record) {
        if (marking)
            return (e == object);
        
        if (e == this)
            return true;
        
        if (e->type != t_dictionary || e->size() != dictionary.size())
            return false;

        marking = true;
        object = e;

        Dictionary* d = (Dictionary*)e;
        for (auto& a: dictionary) {
            if (!dictionary.count(a.first) ||
                !d->dictionary[a.first]->unify(lisp, a.second, record)) {
                marking = false;
                return false;
            }
        }
        marking = false;
        return true;
    }

    bool isequal(LispE* lisp, Element* e) {
        if (marking)
            return (e == object);
        
        if (e == this)
            return true;
        
        if (e->type != t_dictionary || e->size() != dictionary.size())
            return false;

        marking = true;
        object = e;

        Dictionary* d = (Dictionary*)e;
        for (auto& a: dictionary) {
            if (!dictionary.count(a.first) ||
                !d->dictionary.at(a.first)->isequal(lisp, a.second)) {
                marking = false;
                return false;
            }
        }
        marking = false;
        return true;
    }
     

    bool egal(Element* e);
    Element* equal(LispE* lisp, Element* e);
    
    long size() {
        return dictionary.size();
    }
    
    Element* reverse(LispE*, bool duplique = true);
    
    void protecting(bool protection, LispE* lisp) {
        if (protection) {
            if (status == s_constant)
                status = s_protect;
        }
        else {
            if (status == s_protect)
                status = s_destructible;
        }
        
        for (auto& a: dictionary)
            a.second->protecting(protection, lisp);
    }
    
    wstring jsonString(LispE* lisp) {
        if (!dictionary.size())
            return L"{}";
                
        if (marking)
            return L"#inf";
        
        marking = true;
        wstring tampon(L"{");
        
        bool premier = true;
        for (auto& a: dictionary) {
            if (!premier) {
                tampon += L",";
            }
            else
                premier = false;
            tampon += wjsonstring(a.first);
            tampon += L":";
            tampon += a.second->jsonString(lisp);
        }
        tampon += L"}";
        marking = false;
        return tampon;
    }
    
    wstring asString(LispE* lisp) {
        long taille = dictionary.size();
        if (!taille)
            return L"{}";

        if (marking)
            return L"...";
        
        marking = true;

        wstring tampon(L"{");
        
        bool premier = true;
        for (auto& a: dictionary) {
            if (!premier) {
                tampon += L" ";
            }
            else
                premier = false;
            tampon += wjsonstring(a.first);
            tampon += L":";
            tampon += a.second->stringInList(lisp);
        }
        tampon += L"}";
        marking = false;
        return tampon;
    }

    u_ustring asUString(LispE* lisp) {
        long taille = dictionary.size();
        if (!taille)
            return U"{}";

        if (marking)
            return U"...";
        
        marking = true;

        u_ustring tampon(U"{");
        
        bool premier = true;
        for (auto& a: dictionary) {
            if (!premier) {
                tampon += U" ";
            }
            else
                premier = false;
            tampon += ujsonstring(a.first);
            tampon += U":";
            tampon += a.second->stringInUList(lisp);
        }
        tampon += U"}";
        marking = false;
        return tampon;
    }
    

    bool Boolean() {
        return (dictionary.size());
    }
    
    Element* protected_index(LispE*, u_ustring&);
    
    Element* value_on_index(wstring& k, LispE* l);
    Element* value_on_index(u_ustring& k, LispE* l);
    Element* value_on_index(LispE*, Element* idx);
    Element* protected_index(LispE*, Element* k);
    
    void recording(string& c, Element* e) {
        u_ustring k;
        s_utf8_to_unicode(k, USTR(c), c.size());
        auto it = dictionary.find(k);
        if (it != dictionary.end()) {
            it->second->decrement();
            it->second = e;
        }
        else
            dictionary[k] = e;
        e->increment();
    }
    
    void recording(u_ustring& k, Element* e) {
        auto it = dictionary.find(k);
        if (it != dictionary.end()) {
            it->second->decrement();
            it->second = e;
        }
        else
            dictionary[k] = e;
        e->increment();
    }

    Element* replace(LispE* lisp, Element* i, Element* e) {
        u_ustring k = i->asUString(lisp);
        recording(k, e);
        return this;
    }

    Element* thekeys(LispE* lisp);
    
    Element* thevalues(LispE* lisp);

    bool remove(LispE* lisp, Element* e) {
        wstring d =  e->asString(lisp);
        return remove(d);
    }

    bool remove(wstring& w) {
        u_pstring k = _w_to_u(w);
        auto it = dictionary.find(k);
        if (it == dictionary.end())
            return false;
        
        it->second->decrement();
        dictionary.erase(k);
        return true;
    }

    bool remove(u_ustring& k) {
        auto it = dictionary.find(k);
        if (it == dictionary.end())
            return false;
        
        it->second->decrement();
        dictionary.erase(k);
        return true;
    }

};

class Dictionarypool : public Dictionary {
public:
    u_ustring u_key;
    LispE* lisp;
    bool choice;
    
    Dictionarypool(LispE* l) : choice(true), lisp(l) {}

    void reversechoice() {
        choice = 1 - choice;
    }

    bool verify() {
        return choice;
    }

    void decrementstatus(uint16_t nb);
    void decrement();
    
    void release();
    Element* fullcopy();
    Element* copying(bool duplicate = true);
    Element* newInstance();
    Element* copyatom(LispE* lisp, uint16_t s);
    
    void append(LispE* lisp, u_ustring& k);
    void append(LispE* lisp, double v);
    void append(LispE* lisp, long v);
    
    void append(Element* e) {
        if (choice) {
            if (!e->isNumber() && !e->isString())
                throw new Error("Error: a key should be a string or a number");
            else
                if (u_key != U"") {
                    u_ustring msg = U"Error: missing value for key:'";
                    msg += u_key;
                    msg += U"'";
                    throw new Error(msg);
                }
            u_key = e->asUString(lisp);
            e->release();
        }
        else {
            dictionary[u_key] = e;
            e->increment();
            u_key = U"";
            reversechoice();
        }
    }


};


//This version of the dictionary is indexed on a number
class Dictionary_n : public Element {
public:

    unordered_map<double, Element*> dictionary;
    Element* object;
    bool marking;
    bool usermarking;
    

    Dictionary_n() : Element(t_dictionaryn) {
        object = NULL;
        marking = false;
        usermarking = false;
    }
    
    Dictionary_n(uint16_t s) : Element(t_dictionaryn, s) {
        object = NULL;
        marking = false;
        usermarking = false;
    }
    
    ~Dictionary_n() {
        //There might be some left over
        for (auto& a : dictionary)
            a.second->decrement();
    }

    bool isDictionary() {
        return true;
    }
    
    bool isEmpty() {
        return dictionary.empty();
    }

    virtual Element* newInstance() {
        return new Dictionary_n;
    }

    bool element_container() {
        return true;
    }
    
    bool isContainer() {
        return true;
    }
    
    void setmark(bool v) {
        marking = v;
    }
    
    bool mark() {
        return marking;
    }

    void setusermark(bool v) {
        usermarking = v;
    }
    
    bool usermark() {
        return  usermarking;
    }

    void resetusermark() {
        if (marking)
            return;
        marking = true;
        usermarking = false;
        for (auto& a: dictionary) {
            a.second->resetusermark();
        }
        marking = false;
    }

    void* begin_iter() {
        return new std::unordered_map<double, Element*>::iterator(dictionary.begin());
    }
    
    Element* next_iter(LispE* lisp, void* it);
    Element* next_iter_exchange(LispE* lisp, void* it);

    void clean_iter(void* it) {
        delete (std::unordered_map<double, Element*>::iterator*)it;
    }

    void garbaging_values(LispE*);
    
    Element* minimum(LispE*);
    Element* maximum(LispE*);
    Element* minmax(LispE*);

    void flatten(LispE*, List* l);
    
    bool check_element(LispE* lisp, Element* element_value);
    Element* loop(LispE* lisp, short label,  List* code);
    Element* search_element(LispE*, Element* element_value, long idx);
    Element* search_all_elements(LispE*, Element* element_value, long idx);
    Element* replace_all_elements(LispE*, Element* element_value, Element* remp);
    Element* count_all_elements(LispE*, Element* element_value, long idx);
    Element* search_reverse(LispE*, Element* element_value, long idx);
    Element* checkkey(LispE* lisp, Element* e);
    Element* reverse(LispE*, bool duplique = true);

    virtual Element* fullcopy() {
        if (marking)
            return object;
        
        marking = true;
        Dictionary_n* d = new Dictionary_n;
        object = d;
        Element* e;
        for (auto& a: dictionary) {
            e = a.second->fullcopy();
            d->dictionary[a.first] = e;
            e->increment();
        }
        marking = false;
        return d;
    }
    

    virtual Element* copying(bool duplicate = true) {
        if (!is_protected() && !duplicate)
            return this;
        
        Dictionary_n* d = new Dictionary_n;
        Element* e;
        for (auto& a: dictionary) {
            e = a.second->copying(false);
            d->dictionary[a.first] = e;
            e->increment();
        }
        return d;
    }
    
    virtual Element* copyatom(LispE* lisp, uint16_t s);

    //In the case of a container for push, key and keyn
    // We must force the copy when it is a constant
    Element* duplicate_constant(LispE* lisp, bool pair = false);
    
    Element* join_in_list(LispE* lisp, u_ustring& sep);
    
    void release() {
        if (!status && !marking) {
            marking = true;
            marking = false;
            delete this;
        }
    }
    
    void decrement() {
        if (is_protected() || marking)
            return;
        
        marking = true;
        
        status--;
        if (!status) {
            delete this;
        }
        else
            marking = false;
    }
    

    void decrementstatus(uint16_t nb) {
        if (is_protected() || marking)
            return;
        
        marking = true;
        
        status -= nb;
        if (!status) {
            delete this;
        }
        else
            marking = false;
    }
    
    bool unify(LispE* lisp, Element* e, bool record) {
        if (marking)
            return (object == e);
        
        if (e == this)
            return true;
        
        if (e->type != t_dictionaryn || e->size() != dictionary.size())
            return false;
        
        marking =  true;
        object = e;
        
        Dictionary_n* d = (Dictionary_n*)e;
        for (auto& a: dictionary) {
            if (!dictionary.count(a.first) ||
                !d->dictionary[a.first]->unify(lisp, a.second, record)) {
                marking = false;
                return false;
            }
        }

        marking = false;
        return true;
    }

    bool isequal(LispE* lisp, Element* e) {
        if (marking)
            return (object == e);
        
        if (e == this)
            return true;
        
        if (e->type != t_dictionaryn || e->size() != dictionary.size())
            return false;
        
        marking =  true;
        object = e;
        
        Dictionary_n* d = (Dictionary_n*)e;
        for (auto& a: dictionary) {
            if (!dictionary.count(a.first) ||
                !d->dictionary.at(a.first)->isequal(lisp, a.second)) {
                marking = false;
                return false;
            }
        }

        marking = false;
        return true;
    }
    bool egal(Element* e);
    Element* equal(LispE* lisp, Element* e);
    
    long size() {
        return dictionary.size();
    }
    
    void protecting(bool protection, LispE* lisp) {
        if (protection) {
            if (status == s_constant)
                status = s_protect;
        }
        else {
            if (status == s_protect)
                status = s_destructible;
        }
        
        for (auto& a: dictionary)
            a.second->protecting(protection, lisp);
    }
    
    
    wstring jsonString(LispE* lisp) {
        if (!dictionary.size())
            return L"{}";
                
        if (marking)
            return L"#inf";
        
        marking = true;

        wstring tampon;
        tampon += L"{";
        
        bool premier = true;
        for (auto& a: dictionary) {
            if (!premier) {
                tampon += L",";
            }
            else
                premier = false;
            
            tampon += convertToWString(a.first);
            tampon += L":";
            tampon += a.second->jsonString(lisp);
        }
        tampon += L"}";
        marking = false;
        return tampon;
    }
    
    
    wstring asString(LispE* lisp) {
        long taille = dictionary.size();
        if (!taille)
            return L"{}";

        if (marking)
            return L"...";
        
        marking = true;

        wstring tampon;
        tampon += L"{";
        
        bool premier = true;
        for (auto& a: dictionary) {
            if (!premier) {
                tampon += L" ";
            }
            else
                premier = false;
            tampon += convertToWString(a.first);
            tampon += L":";
            tampon += a.second->stringInList(lisp);
        }
        tampon += L"}";
        marking = false;
        return tampon;
    }

    u_ustring asUString(LispE* lisp) {
        long taille = dictionary.size();
        if (!taille)
            return U"{}";

        if (marking)
            return U"...";
        
        marking = true;

        u_ustring tampon;
        tampon += U"{";
        
        bool premier = true;
        for (auto& a: dictionary) {
            if (!premier) {
                tampon += U" ";
            }
            else
                premier = false;
            tampon += convertToUString(a.first);
            tampon += U":";
            tampon += a.second->stringInUList(lisp);
        }
        tampon += U"}";
        marking = false;
        return tampon;
    }
    

    bool Boolean() {
        return (dictionary.size());
    }
    
    Element* protected_index(LispE*, double k);

    Element* value_on_index(double k, LispE* l);
    Element* value_on_index(LispE*, Element* idx);
    Element* protected_index(LispE*, Element* k);
    
    void recording(double  k, Element* e) {
        auto it = dictionary.find(k);
        if (it != dictionary.end()) {
            it->second->decrement();
            it->second = e;
        }
        else
            dictionary[k] = e;
        e->increment();
    }
    
    Element* replace(LispE* lisp, Element* i, Element* e) {
        recording(i->asNumber(), e);
        return e;
    }
    
    Element* thekeys(LispE* lisp);
    
    Element* thevalues(LispE* lisp);
    
    bool remove(LispE*, Element* e) {
        double d =  e->asNumber();
        return remove(d);
    }

    bool remove(double k) {
        auto it = dictionary.find(k);
        if (it == dictionary.end())
            return false;
        
        it->second->decrement();
        dictionary.erase(k);
        return true;
    }
    
    //bool traverse(LispE*, Dictionary_as_list*);
};

class Dictionary_npool : public Dictionary_n {
public:
    LispE* lisp;
    
    Dictionary_npool(LispE* l) : lisp(l) {}

    void decrementstatus(uint16_t nb);
    void decrement();
    
    void release();
    Element* fullcopy();
    Element* copyatom(LispE* lisp, uint16_t s);
    Element* copying(bool duplicate = true);
    Element* newInstance();
};

//This version of the dictionary is indexed on a number
class Dictionary_i : public Element {
public:

    unordered_map<long, Element*> dictionary;
    Element* object;
    bool marking;
    bool usermarking;
    

    Dictionary_i() : Element(t_dictionaryi) {
        object = NULL;
        marking = false;
        usermarking = false;
    }
    
    Dictionary_i(uint16_t s) : Element(t_dictionaryi, s) {
        object = NULL;
        marking = false;
        usermarking = false;
    }
    
    ~Dictionary_i() {
        //There might be some left over
        for (auto& a : dictionary)
            a.second->decrement();
    }

    bool isDictionary() {
        return true;
    }
    
    bool isEmpty() {
        return dictionary.empty();
    }

    void* begin_iter() {
        return new std::unordered_map<long, Element*>::iterator(dictionary.begin());
    }
    
    Element* next_iter(LispE* lisp, void* it);
    Element* next_iter_exchange(LispE* lisp, void* it);

    void clean_iter(void* it) {
        delete (std::unordered_map<long, Element*>::iterator*)it;
    }

    virtual Element* newInstance() {
        return new Dictionary_i;
    }

    bool element_container() {
        return true;
    }
    
    bool isContainer() {
        return true;
    }
    
    void setmark(bool v) {
        marking = v;
    }
    
    bool mark() {
        return marking;
    }

    void setusermark(bool v) {
        usermarking = v;
    }
    
    bool usermark() {
        return  usermarking;
    }

    void resetusermark() {
        if (marking)
            return;
        marking = true;
        usermarking = false;
        for (auto& a: dictionary) {
            a.second->resetusermark();
        }
        marking = false;
    }

    void garbaging_values(LispE*);
    
    Element* minimum(LispE*);
    Element* maximum(LispE*);
    Element* minmax(LispE*);

    void flatten(LispE*, List* l);
    
    bool check_element(LispE* lisp, Element* element_value);
    Element* loop(LispE* lisp, short label,  List* code);
    Element* search_element(LispE*, Element* element_value, long idx);
    Element* search_all_elements(LispE*, Element* element_value, long idx);
    Element* replace_all_elements(LispE*, Element* element_value, Element* remp);
    Element* count_all_elements(LispE*, Element* element_value, long idx);
    Element* search_reverse(LispE*, Element* element_value, long idx);
    Element* checkkey(LispE* lisp, Element* e);
    Element* reverse(LispE*, bool duplique = true);

    virtual Element* fullcopy() {
        if (marking)
            return object;
        
        marking = true;
        Dictionary_i* d = new Dictionary_i;
        object = d;
        Element* e;
        for (auto& a: dictionary) {
            e = a.second->fullcopy();
            d->dictionary[a.first] = e;
            e->increment();
        }
        marking = false;
        return d;
    }
    

    virtual Element* copying(bool duplicate = true) {
        if (!is_protected() && !duplicate)
            return this;
        
        Dictionary_i* d = new Dictionary_i;
        Element* e;
        for (auto& a: dictionary) {
            e = a.second->copying(false);
            d->dictionary[a.first] = e;
            e->increment();
        }
        return d;
    }
    
    virtual Element* copyatom(LispE* lisp, uint16_t s);

    //In the case of a container for push, key and keyn
    // We must force the copy when it is a constant
    Element* duplicate_constant(LispE* lisp, bool pair = false);
    
    Element* join_in_list(LispE* lisp, u_ustring& sep);
    
    void release() {
        if (!status && !marking) {
            marking = true;
            marking = false;
            delete this;
        }
    }
    
    void decrement() {
        if (is_protected() || marking)
            return;
        
        marking = true;
        
        status--;
        if (!status) {
            delete this;
        }
        else
            marking = false;
    }
    

    void decrementstatus(uint16_t nb) {
        if (is_protected() || marking)
            return;
        
        marking = true;
        
        status-=nb;
        if (!status) {
            delete this;
        }
        else
            marking = false;
    }
    
    bool unify(LispE* lisp, Element* e, bool record) {
        if (marking)
            return (object == e);
        
        if (e == this)
            return true;
        
        if (e->type != t_dictionaryi || e->size() != dictionary.size())
            return false;
        
        marking =  true;
        object = e;
        
        Dictionary_i* d = (Dictionary_i*)e;
        for (auto& a: dictionary) {
            if (!dictionary.count(a.first) ||
                !d->dictionary.at(a.first)->unify(lisp, a.second, record)) {
                marking = false;
                return false;
            }
        }
        marking = false;
        return true;
    }
    
    bool isequal(LispE* lisp, Element* e) {
        if (marking)
            return (object == e);
        
        if (e == this)
            return true;
        
        if (e->type != t_dictionaryi || e->size() != dictionary.size())
            return false;
        
        marking =  true;
        object = e;
        
        Dictionary_i* d = (Dictionary_i*)e;
        for (auto& a: dictionary) {
            if (!dictionary.count(a.first) ||
                !d->dictionary.at(a.first)->isequal(lisp, a.second)) {
                marking = false;
                return false;
            }
        }
        
        marking = false;
        return true;
    }
    
    bool egal(Element* e);
    Element* equal(LispE* lisp, Element* e);
    
    long size() {
        return dictionary.size();
    }
    
    void protecting(bool protection, LispE* lisp) {
        if (protection) {
            if (status == s_constant)
                status = s_protect;
        }
        else {
            if (status == s_protect)
                status = s_destructible;
        }
        
        for (auto& a: dictionary)
            a.second->protecting(protection, lisp);
    }
    
    
    wstring jsonString(LispE* lisp) {
        if (!dictionary.size())
            return L"{}";
                
        if (marking)
            return L"#inf";
        
        marking = true;

        wstring tampon;
        tampon += L"{";
        
        bool premier = true;
        for (auto& a: dictionary) {
            if (!premier) {
                tampon += L",";
            }
            else
                premier = false;
            
            tampon += convertToWString(a.first);
            tampon += L":";
            tampon += a.second->jsonString(lisp);
        }
        tampon += L"}";
        marking = false;
        return tampon;
    }
    
    
    wstring asString(LispE* lisp) {
        long taille = dictionary.size();
        if (!taille)
            return L"{}";

        if (marking)
            return L"...";
        
        marking = true;

        wstring tampon;
        tampon += L"{";
        
        bool premier = true;
        for (auto& a: dictionary) {
            if (!premier) {
                tampon += L" ";
            }
            else
                premier = false;
            tampon += convertToWString(a.first);
            tampon += L":";
            tampon += a.second->stringInList(lisp);
        }
        tampon += L"}";
        marking = false;
        return tampon;
    }

    u_ustring asUString(LispE* lisp) {
        long taille = dictionary.size();
        if (!taille)
            return U"{}";

        if (marking)
            return U"...";
        
        marking = true;

        u_ustring tampon;
        tampon += U"{";
        
        bool premier = true;
        for (auto& a: dictionary) {
            if (!premier) {
                tampon += U" ";
            }
            else
                premier = false;
            tampon += convertToUString(a.first);
            tampon += U":";
            tampon += a.second->stringInUList(lisp);
        }
        tampon += U"}";
        marking = false;
        return tampon;
    }
    

    bool Boolean() {
        return (dictionary.size());
    }
    
    Element* protected_index(LispE*, long k);

    Element* value_on_index(long k, LispE* l);
    Element* value_on_index(LispE*, Element* idx);
    Element* protected_index(LispE*, Element* k);
    
    void recording(long k, Element* e) {
        auto it = dictionary.find(k);
        if (it != dictionary.end()) {
            it->second->decrement();
            it->second = e;
        }
        else
            dictionary[k] = e;
        e->increment();
    }
    
    Element* replace(LispE* lisp, Element* i, Element* e) {
        recording(i->asInteger(), e);
        return e;
    }
    
    Element* thekeys(LispE* lisp);
    
    Element* thevalues(LispE* lisp);
    
    bool remove(LispE*, Element* e) {
        long d =  e->asInteger();
        return remove(d);
    }

    bool remove(long k) {
        auto it = dictionary.find(k);
        if (it == dictionary.end())
            return false;
        
        it->second->decrement();
        dictionary.erase(k);
        return true;
    }
    
    //bool traverse(LispE*, Dictionary_as_list*);
};

class Dictionary_ipool : public Dictionary_i {
public:
    LispE* lisp;
    
    Dictionary_ipool(LispE* l) : lisp(l) {}

    void decrementstatus(uint16_t nb);
    void decrement();
    
    void release();
    Element* fullcopy();
    Element* copyatom(LispE* lisp, uint16_t s);
    Element* copying(bool duplicate = true);
    Element* newInstance();
};

// A temporary structure to read a dictionary
class Dictionary_as_list : public Element {
public:
    vector<Element*> keyvalues;
    vector<Element*> valuevalues;
    uint64_t keyvalue;
    short mxkeyvalue;
    bool purekeys;
    bool choice;
    bool select;
    
    Dictionary_as_list() : Element(t_dictionary) {
        choice = true;
        purekeys = true;
        select = true;
        
        //To the bits are set to zeo
        keyvalue = 0;
        mxkeyvalue = 0;
    }
    
    short label() {
        return t_dictionary;
    }

    u_ustring asUString(LispE* lisp) {
        u_ustring s = U"{";
        for (long i = 0; i < keyvalues.size(); i++) {
            if (i)
                s += U" ";
            if (keyvalues[i]->label() == v_null)
                s += U"_";
            else
                s += keyvalues[i]->asUString(lisp);
            s += U":";
            s += valuevalues[i]->stringInUList(lisp);
        }
        s += U"}";
        return s;
    }

    wstring asString(LispE* lisp) {
        wstring s = L"{";
        for (long i = 0; i < keyvalues.size(); i++) {
            if (i)
                s += L" ";
            if (keyvalues[i]->label() == v_null)
                s += L"_";
            else
                s += keyvalues[i]->asString(lisp);
            s += L":";
            s += valuevalues[i]->stringInList(lisp);
        }
        s += L"}";
        return s;
    }

    void append(LispE* lisp, u_ustring& k);
    void append(LispE* lisp, double v);
    void append(LispE* lisp, long v);

    void append(Element* e) {
        if (choice) {
            keyvalues.push_back(e);
            if (type == t_dictionary) {
                //Initial, type prend le type de 'e'
                if (e->isInteger())
                    type = t_integer;
                else {
                    if (e->isNumber())
                        type = t_number;
                    else
                        type = t_string;
                }
            }
            else {
                //If the elements are mangled, then it is a dictionary indexed on strings
                if (type == t_integer && !e->isInteger() && e->isNumber())
                    type = t_number;
                else {
                    if ( (type == t_integer || type == t_number) && !e->isNumber()) {
                        type = t_string;
                    }
                }
            }
        }
        else {
            valuevalues.push_back(e);
            choice = true;
        }
        select = 1 - select;
    }
    
    void reversechoice() {
        choice = 1 - choice;
    }
    
    void push(Element* e) {
        if (select) {
            if (!e->isString() && !e->isNumber())
                purekeys = false;
            else {
                //We record its position in bit vector
                mxkeyvalue = keyvalues.size();
                keyvalue |= ((uint64_t)1 << mxkeyvalue++);
            }
            keyvalues.push_back(e);
        }
        else
            valuevalues.push_back(e);
        select = 1 - select;
    }
    
    bool verify() {
        if (keyvalue) {
            //Then it should be a sequence of 1 from bit 0
            return ((((uint64_t)1 << mxkeyvalue) - 1) == keyvalue);
        }
        return select;
    }
    
    Element* dictionary(LispE* lisp);
    
    bool isDictionary() {
        return true;
    }
    
    bool unify(LispE* lisp, Element* e, bool record);
    bool traverse(LispE*, Dictionary*, Strings* keys, Integers* consummed, long di, long i, bool record);
    bool traverse(LispE*, Dictionary_n*, Numbers* keys, Integers* consummed, long di, long i, bool record);

    void release() {
        if (!status) {
            for (long i = 0; i < keyvalues.size(); i++) {
                keyvalues[i]->release();
            }
            for (long i = 0; i < valuevalues.size(); i++) {
                valuevalues[i]->release();
            }
        }
    }
};

class Dictionary_as_buffer : public Element {
public:
    Dictionary* dico;
    u_ustring key;
    bool choice;
    
    Dictionary_as_buffer(LispE* lisp);
    
    void reversechoice() {
        choice = 1 - choice;
    }

    bool verify() {
        return choice;
    }
    
    void append(LispE* lisp, u_ustring& k);
    void append(LispE* lisp, double v);
    void append(LispE* lisp, long v);

    void append(Element* e) {
        if (choice) {
            if (!e->isNumber() && !e->isString())
                throw new Error("Error: a key should be a string or a number");
            else
                if (key != U"") {
                    u_ustring msg = U"Error: missing value for key:'";
                    msg += key;
                    msg += U"'";
                    throw new Error(msg);
                }
            key = e->asUString(NULL);
            e->release();
        }
        else {
            dico->dictionary[key] = e;
            e->increment();
            key = U"";
            reversechoice();
        }
    }

    bool isDictionary() {
        return true;
    }
    
    Element* dictionary(LispE*) {
        return dico;
    }
    
    void release() {
        dico->release();
    }
    
};

class Set_s : public Element {
public:
    
    std::set<u_ustring> ensemble;
    Conststring exchange_value;

    Set_s() : exchange_value(U""), Element(t_sets) {}
    
    Set_s(std::set<u_ustring>& e) : exchange_value(U""), ensemble(e), Element(t_sets) {}
    
    Set_s(uint16_t s) : exchange_value(U""), Element(t_sets, s) {}
    
    bool isContainer() {
        return true;
    }
    
    bool isSet() {
        return true;
    }
    
    bool isEmpty() {
        return ensemble.empty();
    }
    
    virtual Element* newInstance() {
        return new Set_s();
    }
    
    Element* asList(LispE* lisp);

    void* begin_iter() {
        return new std::set<u_ustring>::iterator(ensemble.begin());
    }
    
    Element* next_iter(LispE* lisp, void* it);
    Element* next_iter_exchange(LispE* lisp, void* it);

    void clean_iter(void* it) {
        delete (std::set<u_ustring>::iterator*)it;
    }
    
    Element* loop(LispE* lisp, short label,  List* code);
    
    Element* minimum(LispE*);
    Element* maximum(LispE*);
    Element* minmax(LispE*);
    
    void flatten(LispE*, List* l);
    
    bool check_element(LispE* lisp, Element* element_value);
    Element* search_element(LispE*, Element* element_value, long idx);
    Element* search_all_elements(LispE*, Element* element_value, long idx);
    Element* replace_all_elements(LispE*, Element* element_value, Element* remp);
    Element* count_all_elements(LispE*, Element* element_value, long idx);
    Element* search_reverse(LispE*, Element* element_value, long idx);
    Element* checkkey(LispE* lisp, Element* e);

    Element* list_and(LispE*, Element* value);
    Element* list_xor(LispE*, Element* value);
    Element* list_or(LispE*, Element* value);

    virtual Element* fullcopy();
    virtual Element* copying(bool duplicate = true);
    
    //In the case of a container for push, key and keyn
    // We must force the copy when it is a constant
    Element* duplicate_constant(LispE* lisp, bool pair = false);
    
    Element* join_in_list(LispE* lisp, u_ustring& sep);
    
    bool unify(LispE* lisp, Element* e, bool record) {
        if (e == this)
            return true;
        
        if (e->type != t_sets)
            return false;

        return ensemble == ((Set_s*)e)->ensemble;
    }
     
    bool isequal(LispE* lisp, Element* e) {
        if (e == this)
            return true;
        
        if (e->type != t_sets)
            return false;

        return ensemble == ((Set_s*)e)->ensemble;
    }
     
    bool egal(Element* e);
    Element* equal(LispE* lisp, Element* e);
    
    long size() {
        return ensemble.size();
    }
            
    wstring jsonString(LispE* lisp) {
        if (ensemble.empty())
            return L"[]";
                
        wstring tampon(L"[");
        
        bool premier = true;
        for (auto& a: ensemble) {
            if (!premier) {
                tampon += L",";
            }
            else
                premier = false;
            tampon += wjsonstring(a);
        }
        tampon += L"]";
        return tampon;
    }
    
    wstring asString(LispE* lisp) {
        if (ensemble.empty())
            return L"{}";
                
        wstring tampon(L"{");
        
        bool premier = true;
        for (auto& a: ensemble) {
            if (!premier) {
                tampon += L" ";
            }
            else
                premier = false;
            tampon += wjsonstring(a);
        }
        tampon += L"}";
        return tampon;
    }

    u_ustring asUString(LispE* lisp) {
        if (ensemble.empty())
            return U"{}";
                
        u_ustring tampon(U"{");
        
        bool premier = true;
        for (auto& a: ensemble) {
            if (!premier) {
                tampon += U" ";
            }
            else
                premier = false;
            tampon += ujsonstring(a);
        }
        tampon += U"}";
        return tampon;
    }
    

    bool Boolean() {
        return ensemble.empty();
    }
    
    Element* protected_index(LispE*, u_ustring&);
    
    Element* index(long i) {
        for (auto& a: ensemble) {
            if (i <= 0) {
                exchange_value.content = a;
                return &exchange_value;
            }
            i--;
        }
        exchange_value.content = U"";
        return &exchange_value;
    }

    Element* value_from_index(LispE*, long i);
    
    Element* value_on_index(LispE*, long i);
    Element* protected_index(LispE*,long i);

    Element* value_on_index(wstring& k, LispE* l);
    Element* value_on_index(u_ustring& k, LispE* l);
    Element* value_on_index(LispE*, Element* idx);
    Element* protected_index(LispE*, Element* k);

    void push_element(LispE* lisp, List* l);
    void push_element_front(LispE* lisp, List* l);
    void push_element_back(LispE* lisp, List* l);

    void append(LispE* lisp, u_ustring& k) {
        ensemble.insert(k);
    }
    
    void add(u_ustring k) {
        ensemble.insert(k);
    }

    void append(u_ustring& k) {
        ensemble.insert(k);
    }

    void storevalue(LispE*, u_ustring& k) {
        ensemble.insert(k);
    }

    void add(wstring k) {
        ensemble.insert(_w_to_u(k));
    }

    void append(wstring& k) {
        ensemble.insert(_w_to_u(k));
    }
    
    void append(string& k) {
        u_ustring w;
        s_utf8_to_unicode(w, USTR(k), k.size());
        ensemble.insert(w);
    }
    
    void append(LispE* lisp, double v);
    void append(LispE* lisp, long v);

    void append(Element* e) {
        ensemble.insert(e->asUString(NULL));
    }
    
    void appendraw(Element* e) {
        ensemble.insert(e->asUString(NULL));
    }

    Element* insert(LispE* lisp, Element* e, long idx) {
        ensemble.insert(e->asUString(lisp));
        return this;
    }
    
    Element* replace(LispE* lisp, Element* i, Element* e) {
        u_ustring k = i->asUString(lisp);
        ensemble.erase(k);
        ensemble.insert(e->asUString(lisp));
        return this;
    }

    Element* thekeys(LispE* lisp);
    Element* thevalues(LispE* lisp);

    bool remove(LispE* lisp, Element* e) {
        u_ustring k =  e->asUString(lisp);
        return ensemble.erase(k);
    }

    bool remove(wstring& w) {
        u_pstring k = _w_to_u(w);
        return ensemble.erase(k);
    }

    bool remove(u_ustring& k) {
        return ensemble.erase(k);
    }
    
    virtual Element* copyatom(LispE* lisp, uint16_t s);

    Element* plus(LispE* l, Element* e);
};

class Set_spool : public Set_s {
public:
    LispE* lisp;
    
    Set_spool(LispE* l) : lisp(l) {
        exchange_value.lisp = l;
        exchange_value.provide = true;
    }

    Set_spool(LispE* l, Set_s* e) : lisp(l), Set_s(e->ensemble) {
        exchange_value.lisp = l;
        exchange_value.provide = true;
    }

    Set_spool* set(Set_s* s) {
        ensemble = s->ensemble;
        return this;
    }
    
    void decrementstatus(uint16_t nb);
    void decrement();
    
    void release();
    Element* fullcopy();
    Element* copying(bool duplicate = true);
    Element* copyatom(LispE* lisp, uint16_t s);
    Element* newInstance();

};

class Set_n : public Element {
public:
    
    std::set<double> ensemble;
    Constnumber exchange_value;
    
    Set_n() : exchange_value(0), Element(t_setn) {}
    
    Set_n(std::set<double>& e) : exchange_value(0), ensemble(e), Element(t_setn) {}
    
    Set_n(uint16_t s) : exchange_value(0), Element(t_setn, s) {}
    
    bool isContainer() {
        return true;
    }
        
    bool isSet() {
        return true;
    }
    
    bool isEmpty() {
        return ensemble.empty();
    }

    Element* newInstance() {
        return new Set_n();
    }

    Element* asList(LispE* lisp);
    
    void* begin_iter() {
        return new std::set<double>::iterator(ensemble.begin());
    }
    
    virtual Element* copyatom(LispE* lisp, uint16_t s);

    Element* next_iter(LispE* lisp, void* it);
    Element* next_iter_exchange(LispE* lisp, void* it);

    void clean_iter(void* it) {
        delete (std::set<double>::iterator*)it;
    }

    Element* loop(LispE* lisp, short label,  List* code);
    
    Element* minimum(LispE*);
    Element* maximum(LispE*);
    Element* minmax(LispE*);
    
    void flatten(LispE*, List* l);
    
    bool check_element(LispE* lisp, Element* element_value);
    Element* search_element(LispE*, Element* element_value, long idx);
    Element* search_all_elements(LispE*, Element* element_value, long idx);
    Element* replace_all_elements(LispE*, Element* element_value, Element* remp);
    Element* count_all_elements(LispE*, Element* element_value, long idx);
    Element* search_reverse(LispE*, Element* element_value, long idx);
    Element* checkkey(LispE* lisp, Element* e);

    Element* list_and(LispE*, Element* value);
    Element* list_xor(LispE*, Element* value);
    Element* list_or(LispE*, Element* value);

    Element* fullcopy();
    Element* copying(bool duplicate = true);
    
    //In the case of a container for push, key and keyn
    // We must force the copy when it is a constant
    Element* duplicate_constant(LispE* lisp, bool pair = false);
    
    Element* join_in_list(LispE* lisp, u_ustring& sep);
    
    bool unify(LispE* lisp, Element* e, bool record) {
        if (e == this)
            return true;
        
        if (e->type != t_sets)
            return false;

        return ensemble == ((Set_n*)e)->ensemble;
    }
     
    bool isequal(LispE* lisp, Element* e) {
        if (e == this)
            return true;
        
        if (e->type != t_sets)
            return false;

        return ensemble == ((Set_n*)e)->ensemble;
    }
     
    bool egal(Element* e);
    Element* equal(LispE* lisp, Element* e);
    
    long size() {
        return ensemble.size();
    }
            
    wstring jsonString(LispE* lisp) {
        if (ensemble.empty())
            return L"[]";
        
        wstring buffer(L"[");
        bool first = true;
        for (auto& a: ensemble) {
            if (first)
                first = false;
            else
                buffer += L",";
            buffer += convertToWString(a);
        }
        buffer += L"]";
        return buffer;
    }
    
    u_ustring asUString(LispE* lisp) {
        if (ensemble.empty())
            return U"{}";
        
        u_ustring buffer(U"{");
        bool first = true;
        for (auto& a: ensemble) {
            if (first)
                first = false;
            else
                buffer += U" ";
            buffer += convertToUString(a);
        }
        buffer += U"}";
        return buffer;
    }
    
    wstring asString(LispE* lisp) {
        if (ensemble.empty())
            return L"{}";
        
        wstring buffer(L"{");
        bool first = true;
        for (auto& a: ensemble) {
            if (first)
                first = false;
            else
                buffer += L" ";
            buffer += convertToWString(a);
        }
        buffer += L"}";
        return buffer;
    }
    
    bool Boolean() {
        return ensemble.empty();
    }
    
    Element* index(long i) {
        for (auto& a: ensemble) {
            if (i <= 0) {
                exchange_value.number = a;
                return &exchange_value;
            }
            i--;
        }
        exchange_value.number = 0;
        return &exchange_value;
    }

    Element* value_from_index(LispE*, long i);
    
    Element* value_on_index(LispE*, long i);
    Element* protected_index(LispE*,long i);
    
    Element* protected_index(LispE*, double&);
    Element* value_on_index(LispE*, Element* idx);
    Element* protected_index(LispE*, Element* k);
    void storevalue(LispE*, double k) {
        ensemble.insert(k);
    }

    
    void add(double k) {
        ensemble.insert(k);
    }

    void push_element(LispE* lisp, List* l);
    void push_element_front(LispE* lisp, List* l);
    void push_element_back(LispE* lisp, List* l);

    void append(LispE* lisp, double v) {
        ensemble.insert(v);
    }
    void append(LispE* lisp, long v) {
        ensemble.insert((double)v);
    }

    void append(Element* e) {
        ensemble.insert(e->asNumber());
    }
    
    void appendraw(Element* e) {
        ensemble.insert(e->asNumber());
    }

    Element* insert(LispE* lisp, Element* e, long idx) {
        ensemble.insert(e->asNumber());
        return this;
    }
    
    Element* replace(LispE* lisp, Element* i, Element* e) {
        double k = i->asNumber();
        ensemble.erase(k);
        ensemble.insert(e->asNumber());
        return this;
    }

    Element* thekeys(LispE* lisp);
    Element* thevalues(LispE* lisp);

    bool remove(LispE* lisp, Element* e) {
        double k = e->asNumber();
        return ensemble.erase(k);
    }

    bool remove(long k) {
        return ensemble.erase(k);
    }
        
    bool remove(double k) {
        return ensemble.erase(k);
    }
        
    Element* bit_not(LispE* l);
    Element* bit_and(LispE* l, Element* e);
    Element* bit_and_not(LispE* l, Element* e);
    Element* bit_or(LispE* l, Element* e);
    Element* bit_xor(LispE* l, Element* e);
    Element* plus(LispE* l, Element* e);
    Element* minus(LispE* l, Element* e);
    Element* multiply(LispE* l, Element* e);
    Element* divide(LispE* l, Element* e);
    Element* mod(LispE* l, Element* e);
    Element* power(LispE* l, Element* e);
    Element* leftshift(LispE* l, Element* e);
    Element* rightshift(LispE* l, Element* e);

};

class Set_npool : public Set_n {
public:
    LispE* lisp;
    
    Set_npool(LispE* l) : lisp(l) {
        exchange_value.lisp = l;
        exchange_value.provide = true;
    }

    Set_npool(LispE* l, Set_n* e) : lisp(l), Set_n(e->ensemble) {
        exchange_value.lisp = l;
        exchange_value.provide = true;
    }

    Set_npool* set(Set_n* s) {
        ensemble = s->ensemble;
        return this;
    }
    
    void decrementstatus(uint16_t nb);
    void decrement();
    
    void release();
    Element* fullcopy();
    Element* copying(bool duplicate = true);
    Element* copyatom(LispE* lisp, uint16_t s);
    Element* newInstance();

};

class Set_i : public Element {
public:
    
    std::set<long> ensemble;
    Constinteger exchange_value;
    
    Set_i() : exchange_value(0), Element(t_seti) {}
    
    Set_i(std::set<long>& e) : exchange_value(0), ensemble(e), Element(t_seti) {}
    
    Set_i(uint16_t s) : exchange_value(0), Element(t_seti, s) {}
    
    bool isContainer() {
        return true;
    }
        
    bool isSet() {
        return true;
    }
    
    bool isEmpty() {
        return ensemble.empty();
    }

    Element* asList(LispE* lisp);
    
    Element* newInstance() {
        return new Set_i();
    }

    void* begin_iter() {
        return new std::set<long>::iterator(ensemble.begin());
    }
    
    virtual Element* copyatom(LispE* lisp, uint16_t s);

    Element* next_iter(LispE* lisp, void* it);
    Element* next_iter_exchange(LispE* lisp, void* it);

    void clean_iter(void* it) {
        delete (std::set<long>::iterator*)it;
    }

    Element* loop(LispE* lisp, short label,  List* code);
    
    Element* minimum(LispE*);
    Element* maximum(LispE*);
    Element* minmax(LispE*);
    
    void flatten(LispE*, List* l);
    
    bool check_element(LispE* lisp, Element* element_value);
    Element* search_element(LispE*, Element* element_value, long idx);
    Element* search_all_elements(LispE*, Element* element_value, long idx);
    Element* replace_all_elements(LispE*, Element* element_value, Element* remp);
    Element* count_all_elements(LispE*, Element* element_value, long idx);
    Element* search_reverse(LispE*, Element* element_value, long idx);
    Element* checkkey(LispE* lisp, Element* e);

    Element* list_and(LispE*, Element* value);
    Element* list_xor(LispE*, Element* value);
    Element* list_or(LispE*, Element* value);

    Element* fullcopy();
    Element* copying(bool duplicate = true);
    
    //In the case of a container for push, key and keyn
    // We must force the copy when it is a constant
    Element* duplicate_constant(LispE* lisp, bool pair = false);
    
    Element* join_in_list(LispE* lisp, u_ustring& sep);
    
    bool unify(LispE* lisp, Element* e, bool record) {
        if (e == this)
            return true;
        
        if (e->type != t_sets)
            return false;

        return ensemble == ((Set_i*)e)->ensemble;
    }
     
    bool isequal(LispE* lisp, Element* e) {
        if (e == this)
            return true;
        
        if (e->type != t_sets)
            return false;

        return ensemble == ((Set_i*)e)->ensemble;
    }
     
    bool egal(Element* e);
    Element* equal(LispE* lisp, Element* e);
    
    long size() {
        return ensemble.size();
    }
            
    wstring jsonString(LispE* lisp) {
        if (ensemble.empty())
            return L"[]";
        
        wstring buffer(L"[");
        bool first = true;
        for (auto& a: ensemble) {
            if (first)
                first = false;
            else
                buffer += L",";
            buffer += convertToWString(a);
        }
        buffer += L"]";
        return buffer;
    }
    
    u_ustring asUString(LispE* lisp) {
        if (ensemble.empty())
            return U"{}";
        
        u_ustring buffer(U"{");
        bool first = true;
        for (auto& a: ensemble) {
            if (first)
                first = false;
            else
                buffer += U" ";
            buffer += convertToUString(a);
        }
        buffer += U"}";
        return buffer;
    }
    
    wstring asString(LispE* lisp) {
        if (ensemble.empty())
            return L"{}";
        
        wstring buffer(L"{");
        bool first = true;
        for (auto& a: ensemble) {
            if (first)
                first = false;
            else
                buffer += L" ";
            buffer += convertToWString(a);
        }
        buffer += L"}";
        return buffer;
    }
    
    bool Boolean() {
        return ensemble.empty();
    }
    
    Element* index(long i) {
        for (auto& a: ensemble) {
            if (i <= 0) {
                exchange_value.integer = a;
                return &exchange_value;
            }
            i--;
        }
        exchange_value.integer = 0;
        return &exchange_value;
    }

    Element* value_from_index(LispE*, long i);
    
    Element* value_on_index(LispE*, long i);
    Element* protected_index(LispE*,long i);
    
    Element* protected_index(LispE*, double&);
    Element* value_on_index(LispE*, Element* idx);
    Element* protected_index(LispE*, Element* k);
    void storevalue(LispE*, double k) {
        ensemble.insert(k);
    }

    
    void add(double k) {
        ensemble.insert(k);
    }

    void push_element(LispE* lisp, List* l);
    void push_element_front(LispE* lisp, List* l);
    void push_element_back(LispE* lisp, List* l);

    void append(LispE* lisp, double v) {
        ensemble.insert(v);
    }
    void append(LispE* lisp, long v) {
        ensemble.insert((double)v);
    }

    void append(Element* e) {
        ensemble.insert(e->asInteger());
    }
    
    void appendraw(Element* e) {
        ensemble.insert(e->asInteger());
    }

    Element* insert(LispE* lisp, Element* e, long idx) {
        ensemble.insert(e->asInteger());
        return this;
    }
    
    Element* replace(LispE* lisp, Element* i, Element* e) {
        long k = i->asInteger();
        ensemble.erase(k);
        ensemble.insert(e->asInteger());
        return this;
    }

    Element* thekeys(LispE* lisp);
    Element* thevalues(LispE* lisp);

    bool remove(LispE* lisp, Element* e) {
        return ensemble.erase(e->asInteger());
    }

    bool remove(long k) {
        return ensemble.erase(k);
    }

    bool remove(double k) {
        return ensemble.erase((long)k);
    }
        
    Element* bit_not(LispE* l);
    Element* bit_and(LispE* l, Element* e);
    Element* bit_and_not(LispE* l, Element* e);
    Element* bit_or(LispE* l, Element* e);
    Element* bit_xor(LispE* l, Element* e);
    Element* plus(LispE* l, Element* e);
    Element* minus(LispE* l, Element* e);
    Element* multiply(LispE* l, Element* e);
    Element* divide(LispE* l, Element* e);
    Element* mod(LispE* l, Element* e);
    Element* power(LispE* l, Element* e);
    Element* leftshift(LispE* l, Element* e);
    Element* rightshift(LispE* l, Element* e);

};

class Set_ipool : public Set_i {
public:
    LispE* lisp;
    
    Set_ipool(LispE* l) : lisp(l) {
        exchange_value.lisp = l;
        exchange_value.provide = true;
    }

    Set_ipool(LispE* l, Set_i* e) : lisp(l), Set_i(e->ensemble) {
        exchange_value.lisp = l;
        exchange_value.provide = true;
    }

    Set_ipool* set(Set_i* s) {
        ensemble = s->ensemble;
        return this;
    }
    
    void decrementstatus(uint16_t nb);
    void decrement();
    
    void release();
    Element* fullcopy();
    Element* copying(bool duplicate = true);
    Element* copyatom(LispE* lisp, uint16_t s);
    Element* newInstance();

};

class Set : public Element {
public:
    
    std::unordered_map<u_ustring, Element*> dictionary;

    Set() : Element(t_set) {}
    
    Set(std::unordered_map<u_ustring, Element*>& d) : dictionary(d), Element(t_set) {
        for (auto& a : dictionary)
            a.second->increment();
    }

    Set(std::unordered_map<u_ustring, Element*>& d, bool duplicate) : Element(t_set) {
        Element* v;
        for (auto& a : d) {
            v = a.second->fullcopy();
            dictionary[a.first] = v;
            v->increment();
        }
    }

    Set(uint16_t s) : Element(t_sets, s) {}
    
    bool isContainer() {
        return true;
    }
    
    bool isSet() {
        return true;
    }
    
    bool isEmpty() {
        return dictionary.empty();
    }
    
    virtual Element* newInstance() {
        return new Set();
    }
    
    Element* asList(LispE* lisp);

    void* begin_iter() {
        return new std::unordered_map<u_ustring, Element*>::iterator(dictionary.begin());
    }
    
    Element* next_iter(LispE* lisp, void* it);
    Element* next_iter_exchange(LispE* lisp, void* it);

    void clean_iter(void* it) {
        delete (std::unordered_map<u_ustring, Element*>::iterator*)it;
    }

    Element* loop(LispE* lisp, short label,  List* code);
    
    Element* minimum(LispE*);
    Element* maximum(LispE*);
    Element* minmax(LispE*);
    
    void flatten(LispE*, List* l);
    
    bool check_element(LispE* lisp, Element* element_value);
    Element* search_element(LispE*, Element* element_value, long idx);
    Element* search_all_elements(LispE*, Element* element_value, long idx);
    Element* replace_all_elements(LispE*, Element* element_value, Element* remp);
    Element* count_all_elements(LispE*, Element* element_value, long idx);
    Element* search_reverse(LispE*, Element* element_value, long idx);
    Element* checkkey(LispE* lisp, Element* e);

    Element* list_and(LispE*, Element* value);
    Element* list_xor(LispE*, Element* value);
    Element* list_or(LispE*, Element* value);

    virtual Element* fullcopy();
    virtual Element* copying(bool duplicate = true);
    
    //In the case of a container for push, key and keyn
    // We must force the copy when it is a constant
    Element* duplicate_constant(LispE* lisp, bool pair = false);
    
    Element* join_in_list(LispE* lisp, u_ustring& sep);
    
    void add(LispE* lisp, Element* e) {
        u_ustring k = e->asUString(lisp);
        if (dictionary.find(k) != dictionary.end())
            dictionary[k]->decrement();
        
        dictionary[k] = e;
        e->increment();
    }

    bool unify(LispE* lisp, Element* e, bool record) {
        if (e == this)
            return true;
        
        if (e->type != t_sets || dictionary.size() != e->size())
            return false;

        for (auto& a: dictionary) {
            if (((Set*)e)->dictionary.find(a.first) != ((Set*)e)->dictionary.end())
                return false;
        }
        return true;
    }
     
    bool isequal(LispE* lisp, Element* e) {
        if (e == this)
            return true;
        
        if (e->type != t_sets || dictionary.size() != e->size())
            return false;

        for (auto& a: dictionary) {
            if (((Set*)e)->dictionary.find(a.first) != ((Set*)e)->dictionary.end())
                return false;
        }
        return true;
    }
     
    bool egal(Element* e);
    Element* equal(LispE* lisp, Element* e);
    
    long size() {
        return dictionary.size();
    }
            
    wstring jsonString(LispE* lisp) {
        if (dictionary.empty())
            return L"[]";
                
        wstring tampon(L"[");
        
        bool premier = true;
        for (auto& a: dictionary) {
            if (!premier) {
                tampon += L",";
            }
            else
                premier = false;
            tampon += a.second->jsonString(lisp);
        }
        tampon += L"]";
        return tampon;
    }
    
    wstring asString(LispE* lisp) {
        if (dictionary.empty())
            return L"{}";
                
        wstring tampon(L"{");
        
        bool premier = true;
        for (auto& a: dictionary) {
            if (!premier) {
                tampon += L" ";
            }
            else
                premier = false;
            tampon += a.second->asString(lisp);
        }
        tampon += L"}";
        return tampon;
    }

    u_ustring asUString(LispE* lisp) {
        if (dictionary.empty())
            return U"{}";
                
        u_ustring tampon(U"{");
        
        bool premier = true;
        for (auto& a: dictionary) {
            if (!premier) {
                tampon += U" ";
            }
            else
                premier = false;
            tampon += a.second->asUString(lisp);
        }
        tampon += U"}";
        return tampon;
    }
    

    bool Boolean() {
        return dictionary.empty();
    }
    
    Element* protected_index(LispE*, u_ustring&);
    
    Element* index(long i) {
        for (auto& a: dictionary) {
            if (i <= 0) {
                return a.second;
            }
            i--;
        }
        return NULL;
    }

    Element* value_from_index(LispE*, long i);
    
    Element* value_on_index(LispE*, long i);
    Element* protected_index(LispE*,long i);

    Element* value_on_index(wstring& k, LispE* l);
    Element* value_on_index(u_ustring& k, LispE* l);
    Element* value_on_index(LispE*, Element* idx);
    Element* protected_index(LispE*, Element* k);

    void push_element(LispE* lisp, List* l);
    void push_element_front(LispE* lisp, List* l);
    void push_element_back(LispE* lisp, List* l);

    void append(Element* e) {
        u_ustring k = e->asUString(NULL);
        if (dictionary.find(k) != dictionary.end())
            dictionary[k]->decrement();
        dictionary[k] = e;
        e->increment();
    }
    
    void appendraw(Element* e) {
        u_ustring k = e->asUString(NULL);
        if (dictionary.find(k) != dictionary.end())
            dictionary[k]->decrement();
        dictionary[k] = e;
        e->increment();
    }

    Element* insert(LispE* lisp, Element* e, long idx) {
        u_ustring k = e->asUString(NULL);
        if (dictionary.find(k) != dictionary.end())
            dictionary[k]->decrement();
        dictionary[k] = e;
        e->increment();
        return this;
    }
    
    Element* replace(LispE* lisp, Element* i, Element* e) {
        u_ustring k = i->asUString(lisp);
        if (dictionary.find(k) != dictionary.end()) {
            dictionary[k]->decrement();
            dictionary.erase(k);
        }
        k = e->asUString(lisp);
        dictionary[k] = e;
        e->increment();
        return this;
    }

    Element* thekeys(LispE* lisp);
    Element* thevalues(LispE* lisp);

    bool remove(LispE* lisp, Element* e) {
        u_ustring k =  e->asUString(lisp);
        auto it = dictionary.find(k);
        if (it == dictionary.end())
            return false;
        
        it->second->decrement();
        dictionary.erase(k);
        return true;
    }

    bool remove(wstring& w) {
        u_pstring k = _w_to_u(w);
        auto it = dictionary.find(k);
        if (it == dictionary.end())
            return false;
        
        it->second->decrement();
        dictionary.erase(k);
        return true;
    }

    bool remove(u_ustring& k) {
        auto it = dictionary.find(k);
        if (it == dictionary.end())
            return false;
        
        it->second->decrement();
        dictionary.erase(k);
        return true;
    }
    
    virtual Element* copyatom(LispE* lisp, uint16_t s);
    
    virtual void release() {
        if (!status) {
            for (auto& a: dictionary)
                a.second->release();
            delete this;
        }
    }
    
    virtual void decrement() {
        if (is_protected())
            return;
        
        status--;
        if (!status) {
            for (auto& a: dictionary)
                a.second->release();
            delete this;
        }
    }
    

    virtual void decrementstatus(uint16_t nb) {
        if (is_protected())
            return;
        
        
        status-=nb;
        if (!status) {
            for (auto& a: dictionary)
                a.second->release();
            delete this;
        }
    }

};

class Setpool : public Set {
public:
    LispE* lisp;
    
    Setpool(LispE* l) : lisp(l) {}

    Setpool(LispE* l, Set* e) : lisp(l), Set(e->dictionary) {}

    Setpool* set(Set* s) {
        dictionary = s->dictionary;
        for (auto& a : dictionary)
            a.second->increment();
        return this;
    }
    
    void decrementstatus(uint16_t nb);
    void decrement();
    
    void release();
    Element* fullcopy();
    Element* copying(bool duplicate = true);
    Element* copyatom(LispE* lisp, uint16_t s);
    Element* newInstance();
    
};


#endif /* elements_h */

