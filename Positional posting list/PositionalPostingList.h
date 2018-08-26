#include <forward_list>
#include <vector>

#include <iostream>
using namespace std; //remove posting method

class PositionalPostingList{ //+with skip pointer(sp)
    private:
        string token;
        struct docIP{ //documents ID and Position
            int docId;
            int position;
        };
        forward_list<docIP> ipList;
        forward_list<docIP>::iterator currentPosting;

    public:
        PositionalPostingList(string wrd){
            token = wrd;
            currentPosting = ipList.before_begin();
        }

        void addSingleDocIp(int id , int position){ //puts document id to correct position, mostly used for editting posting list.
            forward_list<docIP>::iterator nextPosting;
            docIP newIp;
            newIp.docId = id;
            newIp.position = position;
            if (ipList.empty()){
                ipList.insert_after(ipList.before_begin(), newIp);
                return;
            }
            if(id<(*ipList.begin()).docId){
                ipList.insert_after(ipList.before_begin(),newIp);
                return;
            }
            nextPosting = ipList.begin();
            for (currentPosting = ipList.begin(), nextPosting++ ; nextPosting != ipList.end(); //find locate
                currentPosting++, nextPosting++){
                if (id < (*nextPosting).docId){
                    if ((*currentPosting).docId != id || (*currentPosting).position != newIp.position)
                        ipList.insert_after(currentPosting, newIp);
                    return;
                }
            }
          //if code reaches here it means iterator has come till end and no insertion happened
            if (id > (*currentPosting).docId)
                ipList.insert_after(currentPosting, newIp);
        }

        void addSortedDocIds(vector<docIP> ips){
            if(ipList.empty()){
                setNewIds(ips);
                return;
            }
            int maxId;
            for(currentPosting=ipList.begin() ; currentPosting!=ipList.end() ; currentPosting++) //find maximum posting (front of list)
                maxId = (*currentPosting).docId;
            currentPosting = ipList.before_begin();
            forward_list<docIP>::iterator nextPosting = ipList.begin();
            for (docIP ip : ips){
                maxId = ip.docId>maxId ? ip.docId : maxId;
                while((*nextPosting).docId<maxId){
                    currentPosting++;
                    nextPosting++;
                }
                if(*(nextPosting).docId!=maxId)
                    ipList.insert_after(currentPosting, ip);
            }
        }

        void setPointerToBegining(){
            if (ipList.empty())
                return;
            currentPosting = idList.before_begin();
        }

        void clear(){
            ipList.clear();
        }

        void addDocId(vector<docIP> ips){ //addes unsorted id list to current posting list
            forward_list<docIP>::iterator nextPosting;
             ips = quicksort(ips);
            currentPosting = ipList.before_begin(), nextPosting = ipList.begin();
            for (docIP ip : ips){
                if ((*nextPosting).docId < ip.docId){
                    if (*(currentPosting).docId != ip.docId)
                        idList.insert_after(currentPosting, ip);
                    break;
                }
                currentPosting++;
                nextPosting++;
            }
        }

        void addByOrder(docIP ip){    //inserts id to end of list(*current), used for load postings
            //IMPORTANT!!!!  :    it should used continuesly with no curreption, also set to begin should be called before it
            ipList.insert_after(currentPosting++,ip);
        }

        void setNewIds(vector<docIP> ips){ //usually used for creating posting list
            ips = quicksort(ips); //maybe better to use forward_list::push_front() meyhod
            ipList.clear();
            currentPosting = ipList.before_begin();
            for (docIP ip : ips){
                ipList.insert_after(currentPosting, ip);
                currentPosting++;
            }
        }

        forward_list<int> simpleIntersectionWith(PositionalPostingList pList){ //order is:   "this that"
            string secondTerm = pList.token;
            forward_list<docIP> secondList = pList.getIpList();
            forward_list<docIP>::iterator secIt = secondList.begin();
            forward_list<int> intersection;
            forward_list<int>::iterator answer = intersection.before_begin();
            
            while(currentPosting != ipList.end() && secIt!=secondList.end()){ //find better way at time(if exists)
                if((*secIt).docId<(*currentPosting).docId)
                    secIt++;
                else if((*secIt).docId>(*currentPosting).docId)
                    currentPosting++;
                else{
                    intersection.insert_after(answer,(*currentPosting).docId);
                    answer++;
                    currentPosting++;
                    secIt++;
                }
            }
            return intersection;
        }

        forward_list<int> biWordIntersectionWith(PositionalPostingList pList){ //order is:   "this that"
            string secondTerm = pList.token;
            forward_list<docIP> secondList = pList.getIpList();
            forward_list<docIP>::iterator secIt = secondList.begin();
            forward_list<int> intersection;
            forward_list<int>::iterator answer = intersection.before_begin();
            
            while(currentPosting != ipList.end() && secIt!=secondList.end()){ //find better way at time(if exists)
                if((*secIt).docId<(*currentPosting).docId)
                    secIt++;
                else if((*secIt).docId>(*currentPosting).docId)
                    currentPosting++;
                else{
                    if((*currentPosting).position = (*secIt).position+1){
                        intersection.insert_after(answer,(*currentPosting).docId);
                        answer++;
                    }
                    currentPosting++;
                    secIt++;
                }
            }
            return intersection;
        }

        void removePosting(int id){
            forward_list<docIP>::iterator nextPosting;
            currentPosting = ipList.before_begin();
            bool exist = 1;
            for(nextPosting = ipList.begin() ; (*nextPosting).docId != id ; currentPosting++ , nextPosting++){
                if(nextPosting == ipList.end()){
                    exist = 0;
                    break;
                }
            }
            if(exist)
                ipList.erase_after(currentPosting);
        }

        forward_list<docIP> getIpList(){
            return ipList;
        }

        int postingCount(forward_list<docIP> postings){
            int numberOfPostings;
            currentPosting = postings.begin();
            for(numberOfPostings=0;currentPosting!=postings.end();numberOfPostings++,currentPosting++);
            return numberOfPostings;
        }

        void printAll(){
            forward_list<docIP>::iterator it;
            for (it = ipList.begin(); it != ipList.end(); it++)
                cout << (*it).position << (*it).docId << endl;
        }

};