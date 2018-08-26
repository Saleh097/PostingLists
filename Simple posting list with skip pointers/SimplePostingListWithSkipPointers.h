#include <forward_list>
#include <vector>

#include <iostream>
using namespace std; //remove posting method

class SimplePostingListWithSkipPointers{
    private:
        string token;
        forward_list<int> idsList;
        forward_list<int>::iterator currentPosting;
        const int skipPointerFreq = 5;
        forward_list<forward_list<int>::iterator> skipPointersList;
        forward_list<forward_list<int>::iterator>::iterator skipListPointer;
        /*--------------------QUICK_SORTER__begin------------------*/
        vector<int> quicksort(vector<int> unsorted_vector){
            //Get a comparator and remove from vector:
            int compare_int = unsorted_vector.at(unsorted_vector.size() - 1); //Last value in vector
            unsorted_vector.pop_back();

            vector<int> unsorted_left;  //will contain all values less than (or equal to) random
            vector<int> unsorted_right; //will contain all values greater than random
            vector<int> sorted_left;
            vector<int> sorted_right;
            vector<int> sorted_vector;

            //Divides out the elements in the vector into vector either greater than or less than compare_int
            for (unsigned i = 0; i < unsorted_vector.size(); i++){
                if (unsorted_vector.at(i) <= compare_int){
                    unsorted_left.push_back(unsorted_vector.at(i));
                }
                else{
                    unsorted_right.push_back(unsorted_vector.at(i));
                }
            }

            if (unsorted_left.size() != 0){ //sort left side of random
                sorted_left = quicksort(unsorted_left);
            }
            if (unsorted_right.size() != 0){ //sort right side of random
                sorted_right = quicksort(unsorted_right);
            }

            //return combination of sorted left + compare_int + sorted right			//TODO?? Reserve memory when created sorted vector
            sorted_vector.insert(sorted_vector.end(), sorted_left.begin(), sorted_left.end());
            sorted_vector.push_back(compare_int);
            sorted_vector.insert(sorted_vector.end(), sorted_right.begin(), sorted_right.end());

            return sorted_vector;
        }
        /*--------------------QUICK_SORTER__end------------------*/

    public:
        SimplePostingListWithSkipPointers(string wrd){
            token = wrd;
            currentPosting = idsList.before_begin();
        }

        void addSingleDocId(int id){ //puts document id to correct position, mostly used for editting posting list.
            forward_list<int>::iterator nextPosting;
            if (idsList.empty()){
                idsList.insert_after(idsList.before_begin(), id);
                return;
            }
            if(id<*idsList.begin()){
                idsList.insert_after(idsList.before_begin(),id);
                return;
            }
            nextPosting = idsList.begin();
            for (currentPosting = idsList.begin(), nextPosting++ ; nextPosting != idsList.end(); //find locate
                currentPosting++, nextPosting++){
                if (id < *nextPosting){
                    if (*currentPosting != id)
                        idsList.insert_after(currentPosting, id);
                    return;
                }
            }
          //if code reaches here it means iterator has come till end and no insertion happened
            if (id > *currentPosting)
                idsList.insert_after(currentPosting, id);
        }

        void addSortedDocIds(vector<int> ids){
            if(idsList.empty()){
                setNewIds(ids);
                return;
            }
            int maxId;
            for(currentPosting=idsList.begin() ; currentPosting!=idsList.end() ; currentPosting++) //find maximum posting (front of list)
                maxId = *currentPosting;
            currentPosting = idsList.before_begin();
            forward_list<int>::iterator nextPosting = idsList.begin();
            for (int id : ids){
                maxId = id>maxId ? id : maxId;
                while(*nextPosting<maxId){
                    currentPosting++;
                    nextPosting++;
                }
                if(*nextPosting!=maxId)
                    idsList.insert_after(currentPosting, id);
            }
        }

        void setPointerToBegining(){
            if (idsList.empty())
                return;
            currentPosting = idsList.before_begin();
        }

        void clear(){
            idsList.clear();
        }

        void addDocId(vector<int> ids){ //addes unsorted id list to current posting list
            forward_list<int>::iterator nextPosting;
             ids = quicksort(ids);
            currentPosting = idsList.before_begin(), nextPosting = idsList.begin();
            for (int id : ids){
                if (*nextPosting < id){
                    if (*currentPosting != id)
                        idsList.insert_after(currentPosting, id);
                    break;
                }
                currentPosting++;
                nextPosting++;
            }
        }

        void setNewIds(vector<int> ids){ //usually used for creating posting list
            ids = quicksort(ids); //maybe better to use forward_list::push_front() method
            idsList.clear();
            currentPosting = idsList.before_begin();
            for (int id : ids){
                idsList.insert_after(currentPosting, id);
                currentPosting++;
            }
        }

        //TODO search for more than two intersections algorithms
        forward_list<int> intersectWith(SimplePostingListWithSkipPointers spl){//find intersection of two sorted lists
            forward_list<int>::iterator shortList,longList,answer;
            forward_list<int> intersection;
            forward_list<int> argu = spl.getPostings();
            answer = intersection.before_begin();
            int loopCounter=0;
            forward_list<forward_list<int>::iterator> longListSkips;

            forward_list<int> *shorter;
            if(postingCount(idsList) <= postingCount(argu)){  //set shorter and longer lists to its corresponding iterator
                shorter = &idsList;
                longList = argu.begin();
                longListSkips = spl.getSkipPointers();
            }
            else{
                longList = idsList.begin();
                longListSkips = skipPointersList;
                shorter = &argu;
            }

            skipListPointer = longListSkips.begin();
            skipListPointer++;
            shortList = shorter->begin();
            while(shortList != shorter->end()){ //find better way at time(if exists)
                if(loopCounter%skipPointerFreq==0 && skipListPointer!=longListSkips.end()){
                    if(*(*skipListPointer)<=*shortList)
                        longList = *skipListPointer;
                    skipListPointer++;
                }
                if(*longList<*shortList)
                    longList++;
                else if(*longList>*shortList)
                    shortList++;
                else{
                    intersection.insert_after(answer,*shortList);
                    answer++;
                    shortList++;
                    longList++;
                }
            }
            return intersection;
        }

        forward_list<int> unionWith(SimplePostingListWithSkipPointers spl){
            forward_list<int>::iterator shortList,longList,answer;
            forward_list<int> Union;
            forward_list<int> argu = spl.getPostings();
            answer = Union.before_begin();

            forward_list<int> *shorter;
            forward_list<int> *longer;
            if(postingCount(idsList) <= postingCount(argu)){  //set shorter and longer lists to its corresponding iterator
                shorter = &idsList;
                longer = &argu;
            }
            else{
                longer = &idsList;
                shorter = &argu;
            }

            longList = longer->begin();
            shortList = shorter->begin();
            while(shortList != shorter->end() && longList != longer->end()){ //find better way at time(if exists)
                if(*longList<*shortList){
                    Union.insert_after(answer,*longList);
                    answer++;
                    longList++;
                }
                else if(*longList>*shortList){
                    Union.insert_after(answer,*shortList);
                    answer++;
                    shortList++;
                }
                else{
                    Union.insert_after(answer,*shortList);
                    answer++;
                    shortList++;
                    longList++;
                }
            }
            if(shortList != shorter->end()){
                while(shortList != shorter->end()){
                    Union.insert_after(answer,*shortList);
                    answer++;
                    shortList++;
                }
                Union.insert_after(answer,*shortList);
                answer++;
            }
            else if(longList != longer->end()){
                while(longList != shorter->end()){
                    Union.insert_after(answer,*longList);
                    answer++;
                    longList++;
                }
                Union.insert_after(answer,*longList);
                answer++;
            }
            return Union;
        }

        forward_list<int> getPostings(){
            return idsList; //check returned object is a clone(not pointer or reference)
        }

        void removePosting(int id){
            idsList.remove(id);
        }

        int postingCount(forward_list<int> postings){
            int numberOfPostings;
            currentPosting = postings.begin();
            for(numberOfPostings=0;currentPosting!=postings.end();numberOfPostings++,currentPosting++);
            return numberOfPostings;
        }

        void setSkipPointers(){ //after several insertions (or one for single add) this method should be called
            int i;
            skipPointersList.clear();
            currentPosting = idsList.begin();
            forward_list<forward_list<int>::iterator>::iterator curSkip = skipPointersList.before_begin();
            for(i=0 ; currentPosting!=idsList.end(); i++,currentPosting++){
                if(i%skipPointerFreq==0){
                    skipPointersList.insert_after(curSkip,currentPosting);
                    curSkip++;
                }
            }
        }

        forward_list<forward_list<int>::iterator> getSkipPointers(){
            return skipPointersList;
        }

        void printAll(){
            forward_list<int>::iterator it;
            for (it = idsList.begin(); it != idsList.end(); it++)
                cout << *it << endl;
        } //for later: use skip pointers in add and removes;
};