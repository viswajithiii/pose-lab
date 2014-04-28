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
#include<vector>
using namespace std;

typedef int Sheet, Table, Chart;

template<class T>
class Iterator{
    public:
        bool hasNext() = 0;
        void remove() = 0;
        T next() = 0;
};

template<class T>
class IterableList{
	public:
		IterableList<T> join(IterableList<T> a, IterableList<T> b);
};

//TODO: take care of pointers
template<class T>
class Iterable{
    public:
    virtual Iterator<T>* iterator() = 0; //Covariant types
};
template <class T, template <class T> class V>
class ContainerIteratorAttorney;


template <class T, template <class T> class V>
//iterator doesn't need to be templated
class OurIterator;

//Is good enough for the leaf too
template <class T, template <class T> class V>
// is this class T required?
class AbstractContainer: public Iterable<T>{
//T is a pointer like Table*
    //R is the root type
    protected:
    virtual V<T> get_iterable_list() = 0; //If this wasn't there, we should be coupling
    // every iterator with the container.  So must this be there or not?
    //Also should be accessible only by the iterator class. Don't make it public.
    //Does this work?** Check this out
    void remove(void* t); // TODO: Must be accessible only by iterator class
    friend class ContainerIteratorAttorney<T, V>;
    public:
    void add(T t);
    OurIterator<T, V>* iterator() = 0;
};

//http://en.wikibooks.org/wiki/More_C%2B%2B_Idioms/Friendship_and_the_Attorney-Client
template <class T, template <class T> class V>
class ContainerIteratorAttorney{
    private:
        static void call_remove(AbstractContainer<T,V>* container, void *t){ //is static possible?
            container->remove(t);
        }
        static V<T> call_get_iterable_list(AbstractContainer<T,V>* container){
            return container->get_iterable_list();
        }
        friend class OurIterator<T, V>;
};

/* Why are the following two optios bad? For very class in document hierarchy I need to add
a cotnainer class.*/



//TODO: Remove, hwo will that be done? Some sort of a recursive call must happen
//To make sure that something is removed at the leaf level

/*Option 2: Why bad? Client side programmmer: PITA!! Has to make nested scoping and call..
Why peace? When I add a new class to the hierarchy I just need to change one class
*/



//TODO: remove must be accessible only from Iterator! Not from Container
//TODO: cna we make this more templatable?
//note the virtual inheritance

//Remove vector keyword everywhere
template < template <class T> class V>
class AbstractSheetContainer : public virtual AbstractContainer<Sheet*, V > {
    protected: // Why protected? So that child classes that inherit this and implemented
    //this have access to this.
        AbstractContainer<Table*, V>* tableContainer;
        AbstractContainer<Chart*, V>* chartContainer;
        IterableList<Sheet*> get_iterable_list() = 0;
        //Disadvantage? Delegating this work to child class!
        //Who will ensure thatit does work as trusted? =/
    public:
        
        //Make this non overridable? Nope don.t doesn't  make sense
        // Another factory method
        OurIterator<Sheet, V>* iterator() = 0; //Cannot iplement function
        //here because OurIterator is abstract and cannot be instantiated.
        //Trust theinheritor to behave properly
        /*{
            return new OurIterator(this);
        };*/
        
        void remove(void* sheet){
            tableContainer.remove(sheet);
            chartContainer.remove(sheet);
        }
      
        AbstractSheetContainer(){
        //Constructor makes sure that these are called and initialized
            tableContainer = createTableContainer();
            chartContainer = createChartContainer();
        }
        
        AbstractContainer<Table*, V>* get_table_container(){
            return tableContainer;
        }
        
        AbstractContainer<Chart*, V>* get_chart_container(){
            return chartContainer;
        }
        
        ~AbstractSheetContainer(){
                //Free memory?
                //Call destructors of those containers respetively
        }
        
        //ALERT: FactoryMethod?? ;)
        //Delegating creation of these two in the child
        virtual AbstractContainer<Table*, V>* createTableContainer() = 0;
        virtual AbstractContainer<Chart*, V>* createChartContainer() = 0;
        //Why the above two functions?
        //In the child class what if the user forgets to create the tablecontainer and chart containers?
};

//Alternatively we would have had to create an iterator for all classes
//This allows for reuse
template <class T, template <class> class V>
//iterator doesn't need to be templated
class OurIterator: public Iterator<T>{
            //T must be a pointer like Table* etc.,
            protected:
                AbstractContainer<T, V>* container;
                V<T> iterable_list;
            public:
            OurIterator(AbstractContainer<T,V>* container){
                this->container = container;
                this->iterable_list =ContainerIteratorAttorney<T, V>::call_get_iterable_list();
            }
             bool hasNext() = 0; //Implemented in child class?
             T next() = 0;
             void remove() {
             //TODO:Make this unoverridable
                 ContainerIteratorAttorney<T, V>::call_remove(container, (void*)next());
                 //container->remove(next()); //Why don't
                 //we implement it here itself?
                 //Because we want to decouple this implementation
             } 
        };
        
        
 //FACADE PATTERN??
        
/********************** IMPLEMENTATION********************/
//All vector references must be here!
//This allows for reuse
//A speicifc implementation
template <class T>
class MyIterator: public OurIterator<T, vector<T> >{
            private:
                private int position;

            public:       
             bool hasNext(){
                 return (position < iterable_list.size());
             };
             
             T next() {
                 return iterable_list[position ++];
             };
             
   
        };
        
template <class T>
class MyLeafContainer : public AbstractContainer<T, vector<T> >{
     private:
                vector<T> T_list; //CAN BE ANYTHING ACTUALLY
            protected:
                vector<T> get_iterable_list(){
                    return T_list;
                }
            public:
                void remove(void* t){
                         //Logically must call iterator and do this
                         //However, in this imlementation iterator ony calls
                         //so that'll become infinite recusion
                         //This is also implementation dependent
                         for(std::vector<T>::iterator it = chart_list.begin(); it < table_list.end(); it++){
                             if((void*)(*it) == t){
                                 T_list.erase(it);
                                 break;
                             }
                         }

                 }
                MyIterator<T>* iterator(){
                    return new MyIterator<T>(this);
                };
};

template <class T>
class MyNonLeafContainer : public virtual AbstractContainer<T>{
    public:
        MyIterator<T>* iterator(){
            return new MyIterator<T>(this);
        }
};

//This is forced to implement all factory methods
//That will enforce us to concretize all required classes
class MySheetContainer : public AbstractSheetContainer<vector>, public MyNonLeafContainer<Sheet*>{
    protected: // Why protected? 
        
     vector<Sheet*> get_iterable_list()   {
            vector<Table*> table_vector = tableContainer.get_iterable_list(); //TODO: WILL THIS WORK? It's protected..
            vector<Chart*> chart_vector = chartContainer.get_iterable_list(); //TODO: DITTO :P
            vector<Sheet*> sheet_vector;
            //PSEUDOCODE: Unite the above two vectors
            return sheet_vector;
        }  
    public:
    
        
        
        MyLeafContainer<Table*>* createTableContainer(){//IS THIS CORRECT?
            return new MyLeafContainer<Table*>;
        }
        MyLeafContainer<Chart*>*  createChartContainer(){
            return new MyLeafContainer<Chart*>;
        }    
};