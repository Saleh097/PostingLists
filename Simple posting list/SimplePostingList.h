#include <forward_list>
#include <vector>

#include <iostream>
using namespace std;

class SimplePostingList{
    private:
        string token;
        forward_list<int> idList;
        forward_list<int>::iterator currentPosting;
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
        SimplePostingList(string wrd){
            token = wrd;
            currentPosting = idList.before_begin();
        }

        void addSingleDocId(int id){
            forward_list<int>::iterator nextPosting;
            if (idList.empty()){
                idList.insert_after(idList.before_begin(), id);
                return;
            }
            if(id<*idList.begin()){
                idList.insert_after(idList.before_begin(),id);
                return;
            }
            nextPosting = idList.begin();
            for (currentPosting = idList.begin(), nextPosting++ ; nextPosting != idList.end(); //find locate
                currentPosting++, nextPosting++){
                if (id < *nextPosting){
                    if (*currentPosting != id)
                        idList.insert_after(currentPosting, id);
                    return;
                }
            }
          //if code reaches here it means iterator has come till end and no insertion happened
            if (id > *currentPosting)
                idList.insert_after(currentPosting, id);
        }

        void addSortedDocIds(vector<int> ids){
            if(idList.empty()){
                setNewIds(ids);
                return;
            }
            int maxId;
            for(currentPosting=idList.begin() ; currentPosting!=idList.end() ; currentPosting++) //find maximum posting (front of list)
                maxId = *currentPosting;
            currentPosting = idList.before_begin();
            forward_list<int>::iterator nextPosting = idList.begin();
            for (int id : ids){
                maxId = id>maxId ? id : maxId;
                while(*nextPosting<maxId){
                    currentPosting++;
                    nextPosting++;
                }
                if(*nextPosting!=maxId)
                    idList.insert_after(currentPosting, id);
            }
        }

        void setPointerToBegining(){
            if (idList.empty())
                return;
            currentPosting = idList.before_begin();
        }

        void clear(){
            idList.clear();
        }

        void addDocId(vector<int> ids){
            forward_list<int>::iterator nextPosting;
            ids = quicksort(ids);
            currentPosting = idList.before_begin(), nextPosting = idList.begin();
            for (int id : ids){
                if (*nextPosting < id){
                    if (*currentPosting != id)
                        idList.insert_after(currentPosting, id);
                    break;
                }
                currentPosting++;
                nextPosting++;
            }
        }

        void addByOrder(int id){    
            //IMPORTANT!!!!  :    it should used continuesly with no curreption, also set to begin should be called before it
            idList.insert_after(currentPosting++,id);
        }

        void setNewIds(vector<int> ids){ //usually used for creating posting list
            ids = quicksort(ids);
            idList.clear();
            currentPosting = idList.before_begin();
            for (int id : ids){
                idList.insert_after(currentPosting, id);
                currentPosting++;
            }
        }

        forward_list<int> intersectWith(SimplePostingList spl){
            forward_list<int>::iterator shortList,longList,answer;
            forward_list<int> intersection;
            forward_list<int> argu = spl.getPostings();
            answer = intersection.before_begin();

            forward_list<int> *shorter;
            if(postingCount(idList) <= postingCount(argu)){  //set shorter and longer lists to its corresponding iterator
                shorter = &idList;
                longList = argu.begin();
            }
            else{
                longList = idList.begin();
                shorter = &argu;
            }

            shortList = shorter->begin();
            while(shortList != shorter->end()){
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

        forward_list<int> unionWith(SimplePostingList spl){
            forward_list<int>::iterator shortList,longList,answer;
            forward_list<int> Union;
            forward_list<int> argu = spl.getPostings();
            answer = Union.before_begin();

            forward_list<int> *shorter;
            forward_list<int> *longer;
            if(postingCount(idList) <= postingCount(argu)){  //set shorter and longer lists to its corresponding iterator
                shorter = &idList;
                longer = &argu;
            }
            else{
                longer = &idList;
                shorter = &argu;
            }

            longList = longer->begin();
            shortList = shorter->begin();
            while(shortList != shorter->end() && longList != longer->end()){
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
                    Union.insert_after(answer,*shortList);
                    answer++;
                    longList++;
                }
                Union.insert_after(answer,*shortList);
                answer++;
            }
            return Union;
        }

        forward_list<int> getPostings(){
            return idList;
        }

        void removePosting(int id){
            idList.remove(id);
        }

        int postingCount(forward_list<int> postings){
            int numberOfPostings;
            currentPosting = postings.begin();
            for(numberOfPostings=0;currentPosting!=postings.end();numberOfPostings++,currentPosting++);
            return numberOfPostings;
        }
        void printAll(){
            forward_list<int>::iterator it;
            for (it = idList.begin(); it != idList.end(); it++)
                cout << *it << endl;
        }
};