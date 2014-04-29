//TODO:
//Change class X<T>
// to 
// template <class T>
// class X {}
//Why do we have a separate iterator implementation?
//To decouple iterating + To decouple removal
//Variadic templates
//Todo check if small small things work
//Composition: one container within the other
//Logical to expect the end user to know where everything exists + fault tolerant
//Make iterator nested - encapsulation
#include<vector>
using namespace std;


class Sheet{};
class  Table: public Sheet{};
class Chart: public Sheet{};

template<class T>
class Iterator{
    public:
        virtual bool hasNext() = 0;
        virtual void remove() = 0;
        virtual T next() = 0;
};


//TODO: take care of pointers
//Covariant types
template<class T>
class Iterable{
    public:
    virtual Iterator<T>* iterator() = 0; 
};

template<class T>
class IterableList: public Iterable<T>{
	//But this object will be hidden or private anyway
	public:

	public:
		virtual void add(T t);
		virtual void remove(void* t);
		//This is the internal iterator we can eventually implement anyway
		
		
		virtual Iterator<T>* iterator() = 0;
};




/*
class ContainerNode{
	protected:
		virtual void remove(void* t) = 0;
};*/

//Is good enough for the leaf too
template <class T>
// is this class T required?
class AbstractContainer: public Iterable<T>{
//T is a pointer like Table*
    //R is the root type
    protected:
    	virtual IterableList<T>* get_iterable_list() = 0; //If this wasn't there, we should be coupling
    // every iterator with the container.  So must this be there or not?
    //Also should be accessible only by the iterator class. Don't make it public.
    //Does this work?** Check this out
         virtual IterableList<T>* create_iterable_list() = 0;
    	 virtual void remove(void* t) = 0; // TODO: Must be accessible only by iterator class
    	friend class ContainerIteratorAttorney;
    public:
	    //virtual void add(T t) = 0;

		 //class ContainerIteratorAttorney;
		//***************ITERATOR IMPLEMENTATION******************//
		    //Alternatively we would have had to create an iterator for all classes
		//This allows for reuse
		//template <class T>
		//iterator doesn't need to be templated
		//class OurIterator: public Iterator<T>{
		class ContainerIterator: public Iterator<T>{
					private:
					 Iterator<T>* localIterator;
		            //T must be a pointer like Table* etc.,
		            protected:
		                AbstractContainer<T>* container;
		                IterableList<T>* iterable_list; 
		               
		               
		             void remove() {
		             //TODO:Make this unoverridable
		                 ContainerIteratorAttorney::call_remove(container, (void*)next());
		                 //container->remove(next()); //Why don't
		                 //we implement it here itself?
		                 //Because we want to decouple this implementation
		             } 
		            public:
		            //Parent constructor is called
		            ContainerIterator(AbstractContainer* container){
		                this->container = container;
		                this->iterable_list = ContainerIteratorAttorney::call_get_iterable_list(container);
		            	this->localIterator = this->iterable_list->iterator();
		            }
		             bool hasNext(){
		             	return this->localIterator->hasNext();
		             } 
		             T next() {
		             	return this->localIterator->next();
		             }
		             
		        };
	    ContainerIterator* iterator() {
	    	return new  ContainerIterator(this);
	    } //factory method implemented
	    
	    //*************ATTORNEY************************//
	    
		class ContainerIteratorAttorney{
    		private:
	        static void call_remove(AbstractContainer* container, void *t){ //is static possible?
	            container->remove(t);
	        }
	        static IterableList<T>* call_get_iterable_list(AbstractContainer* container){
	            return container->get_iterable_list();
	        }
        	friend class ContainerIterator;
		};

};

//http://en.wikibooks.org/wiki/More_C%2B%2B_Idioms/Friendship_and_the_Attorney-Client
//template <class T>
//class ContainerIteratorAttorney{
 


//NOTE: get_iterable_list is just an abstraction.... like a factory mathod maybe?




//TODO: remove must be accessible only from Iterator! Not from Container
//TODO: cna we make this more templatable?
//note the virtual inheritance

//http://stackoverflow.com/questions/23330504/template-template-in-c-t-vt/23330605#23330605
//Remove vector keyword everywhere
//Initially we allowed c++ stl. but cups.

/*
template <class T>
class AbstractNonLeafContainer: public AbstractContainer<T>{
	protected:
		vector<Container*> child_containers;
	public:
	//Disadvantage call goes to everyone
		void remove(void* t){
			for(vector<Container>::iterator it = child_containers.begin(); it != child_container.end(); it++ ){
				(*it)->remove(t);
			}
        }
}
*/

template <class T>
class AbstractLeafContainer: public virtual AbstractContainer<T>{
	protected:
		IterableList<T>* iterable_list;
		//virtual IterableList<T>* create_iterable_list() = 0; //Factory method
		IterableList<T>* get_iterable_list(){
			return this->iterable_list;
		}
	public:
		AbstractLeafContainer(){
			this->iterable_list = this->create_iterable_list();	
		}
		void add(T t) {
			iterable_list->add(t);
		}
		void remove(void* t){
                         //Logically must call iterator and do this
                         //However, in this imlementation iterator ony calls
                         //so that'll become infinite recusion
                         //This is also implementation dependent
                         iterable_list->remove(t);

         }
};



//none of the below containers actually implement get_iterable_list ?
//it's a factory method :)
class AbstractTableContainer: public virtual AbstractLeafContainer<Table*>{
	friend class AbstractSheetContainer;
};

class AbstractChartContainer: public virtual AbstractLeafContainer<Chart*>{
	friend class AbstractSheetContainer;
};


class AbstractSheetContainer : public virtual AbstractContainer<Sheet*> {
    protected: // Why protected? So that child classes that inherit this and implemented
    //this have access to this.
        AbstractTableContainer* tableContainer;
        AbstractChartContainer* chartContainer;
        //Disadvantage? Delegating this work to child class!
        //Who will ensure thatit does work as trusted? =/
		//virtual IterableList<Sheet*>* get_iterable_list() = 0
		 IterableList<Sheet*>* get_iterable_list(){
		    IterableList<Table*>* table_list = tableContainer->get_iterable_list();
		    IterableList<Chart*>* chart_list = chartContainer->get_iterable_list();
			IterableList<Sheet*>* iterable_list = this->create_iterable_list();
			Iterator<Table*>* it_table = table_list->iterator();
			while(it_table->hasNext()){
				iterable_list->add((Sheet*)it_table->next()); //Valid typecasting
			}
			Iterator<Chart*>* it_chart = chart_list->iterator();
			while(it_chart->hasNext()){
				iterable_list->add((Sheet*)it_chart->next()); //Valid typecasting
			}
			return iterable_list;
		}
        
        
        //Protected factory methods because others must not be able to access
        //Child class must be able to define?
        //Can return abstract class pointer ;)

        //warning: pure virtual ‘virtual AbstractTableContainer* AbstractSheetContainer::createTableContainer()’ called from constructor
        //ALERT: FactoryMethod?? t;)
        //Delegating creation of these two in the child
        virtual AbstractTableContainer* createTableContainer() = 0;
        virtual AbstractChartContainer* createChartContainer() = 0;
        //Why the above two functions?
        //In the child class what if the user forgets to create the tablecontainer and chart containers?
    public:
        
        //Make this non overridable? Nope don.t doesn't  make sense
        // Another factory method
        virtual ContainerIterator* iterator() = 0; //Cannot implement function
        //here because OurIterator is abstract and cannot be instantiated.
        //Trust theinheritor to behave properly
        
      
        //void remove(void* sheet){
          //  tableContainer->remove(sheet);
            //chartContainer->remove(sheet);
        //}
        
    /*    
        AbstractSheetContainer(){
        //Constructor makes sure that these are called and initialized
        	child_containers.push_back(createTableContainer());
            child_containers.push_back(createChartContainer());
        }
      */  
        AbstractSheetContainer(){
        //Constructor makes sure that these are called and initialized
        	tableContainer = createTableContainer();
            chartContainer = createChartContainer();
        }
        
        ~AbstractSheetContainer(){
                //Free memory?
                //Call destructors of those containers respetively
        }
      
      
        AbstractTableContainer* get_table_container(){
           // return child_containers.at(1);
            return tableContainer;
        }
        
        AbstractChartContainer* get_chart_container(){
            return chartContainer;
        }
      };


//Alternatively we would have had to create an iterator for all classes
//This allows for reuse
//template <class T>
//iterator doesn't need to be templated
//class OurIterator: public Iterator<T>{

                
 //FACADE PATTERN??
        
/********************** IMPLEMENTATION********************/
//All vector references must be here!
//This allows for reuse
//A speicifc implementation

//Let's set up the basic collection system
template <class T>
class Vector: public IterableList<T>{
	private:
		vector<T>* elements;
	    

	public:
	Vector(){
		elements = new vector<T>;
	}
	
	void add(T t){
		elements->insert(elements->begin(), t);
	}
	void remove(void* t){
		for(typename  std::vector<T>::iterator it = elements->begin(); it < elements->end(); it++){
            if((void*)(*it) == t){
                elements->erase(it);
                break;
            }
        }
	}
	

	
	class VectorIterator: public Iterator<T>{
		private:
			int position;
			vector<T>* parent_list;
			typename std::vector<T>::iterator it;
		public:
			VectorIterator(Vector<T>* caller){
				parent_list = caller->elements;
				it = parent_list->begin();
			}
			bool hasNext() {
				
				return (it != parent_list->end());
			}
	        void remove() {
	        	parent_list->erase(it);
	        }
	        T next(){
	        	return *(it++);
	        }
	};
	VectorIterator* iterator(){
		return new VectorIterator(this);
	}
};

//The following Iterator class is coupled with the above class
//Must have Vector object stored

//Remember this is just the implementation part.
//I can create a container for a leaf that is made up of ArrayList too!
//In that case I should not be writing this template class
//And thus virtual inheritance should not be a problem!
//There was virtual inheritance there jsut so this can be taken care of
template <class T>
class MyImplementation : public virtual AbstractContainer<T>{
     protected:
                Vector<T>* create_iterable_list() {
                	return new Vector<T>;
                } //Factory method

};

//REMEMBER TO DISCUSS FIRST SOLTION WITH MyIterator!
class MyTableContainer : public MyImplementation<Table*>, public AbstractTableContainer{};
class MyChartContainer : public MyImplementation<Chart*>, public AbstractChartContainer{};

//This is forced to implement all factory methods
//That will enforce us to concretize all required classes

//NOTE: AbstractFactory!?
class MySheetContainer : public MyImplementation<Sheet*>, public AbstractSheetContainer{
	  protected:
	  	
      public:
        MyTableContainer* createTableContainer(){//IS THIS CORRECT?
            return new MyTableContainer;
        }
        MyChartContainer*  createChartContainer(){
            return new MyChartContainer;
        }    
};

int main(){
	return 0;
}
