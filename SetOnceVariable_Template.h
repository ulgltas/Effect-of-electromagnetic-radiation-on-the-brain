/* This is a template for providing variables that can be set only once */
/* That is, when it has been set once, it can never be changed.         */

// [RB] quel est l'interet de cette classe?
// apres reflexion, ce genre de classe n'apporte rien et empeche 
// les autres membres du groupe (et moi)
// de comprendre facilement le reste du code

#ifndef SETONCEVARIABLE_TEMPLATE_H
#define SETONCEVARIABLE_TEMPLATE_H

#include <iostream>
#include <typeinfo>
#include <stdio.h>
#include <stdlib.h>

template<typename T>
class SetOnceVariable_Template{ // [RB] ne pas mettre "Template" dans le nom
	private:
		T value;
		bool alreadySet = false;
	public:
		// Default constructor:
		SetOnceVariable_Template(){};
		// Constructor:
		SetOnceVariable_Template(T init){
			this->value = init;
			this->alreadySet = true;
		};
		// Destructor:
		~SetOnceVariable_Template(void){}; // [RB] inutile
		// Set the value / overloading of the '=' operator:
		SetOnceVariable_Template<T>& operator=(const T& value){
			if(this->alreadySet == false){
				this->value = value;
				this->alreadySet = true;
			}else{
				fprintf(stderr,"In %s :: %s :: variable has already been set. Aborting.\n",
					typeid(this).name(),
					__FUNCTION__);
				fprintf(stderr,"File %s:%d\n",__FILE__,__LINE__);
				abort();
			}
			return *this;
		}
		// Get the value:
		const T& get(void){return this->value;} // [RB] "void" inutile

		//
		bool get_alreadySet(void){ // [RB] "void" inutile
			return this->alreadySet;
		}
};

#endif