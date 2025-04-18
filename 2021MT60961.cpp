#include <iostream>
// #include <vector>
#include <unordered_map>
// #include <unordered_set>
// #include <deque>
// #include <stack>
// #include <list>
// #include <algorithm>
#include <fstream>

// using namespace std;
// #include <iostream>
using namespace std;

// Define Stack class
class Stack {
private:
    // Index of the top element in the stack
    int top;
    // Array to store stack elements
    int arr[1024];


public:
    // Constructor to initialize an empty stack
    Stack() { top = -1; }

    // Function to add an element x to the top of the stack
    void push(int x)
    {
        // If the stack is full, print "Stack overflow
        if (top >= 1024) {
            cout << "Stack overflow" << endl;
            return;
        }
      
        arr[++top] = x;
        // cout << "Pushed " << x << " to stack\n";
    }

    // Function to remove the top element from the stack
    int pop()
    {
        
        if (top < 0) {
            cout << "Stack underflow" << endl;
            return 0;
        }
        // Remove the top element from the stack and
        // decrement top
        return arr[top--];
    }

    // Function to return the top element of the stack
    int peek()
    {
     
        if (top < 0) {
            cout << "Stack is empty" << endl;
            return 0;
        }
        // Return the top element of the stack
        return arr[top];
    }

    // Function to check if the stack is empty
    bool isEmpty()
    {
        // Return true if the stack is empty (i.e., top is
        // -1)
        return (top < 0);
    }
};

class my_queue {
    int frontIndex; // Index of the front element
    int rearIndex;  // Index where the next element will be inserted
    int *a;         // Array to store queue elements
    int size;       // Maximum size of the queue
    int s=1024;
    int currentSize; // Current number of elements in the queue

public:
    // Constructor
    my_queue() {
        frontIndex = 0;         // Front starts at index 0
        rearIndex = -1;         // Rear starts at -1 (will be incremented during push)
        size = s;               // Set the maximum size of the queue
        currentSize = 0;        // Initially, the queue is empty
        a = new int[size];      // Allocate memory for the queue
    }

    // Destructor to deallocate memory
    ~my_queue() {
        delete[] a;
    }

    // Function to add an item to the queue (push)
    void push(int x) {
        if (currentSize == size) {
            cout << "Queue overflow, cannot push " << x << endl;
            return;
        }
        rearIndex = (rearIndex + 1) % size; // Move rear to the next position (circularly)
        a[rearIndex] = x;                   // Insert the element at the rear
        currentSize++;                      // Increase the size of the queue
    }

    // Function to remove an item from the queue (pop)
    void pop() {
        if (currentSize == 0) {
            cout << "Queue underflow, cannot pop" << endl;
            return;
        }
        frontIndex = (frontIndex + 1) % size; // Move front to the next position (circularly)
        currentSize--;                        // Decrease the size of the queue
    }

    // Function to get the current size of the queue
    int get_size() {
        return currentSize;
    }

    // Function to get the front element of the queue
    int front() {
        if (currentSize == 0) {
            cout << "Queue is empty, no front element" << endl;
            return -1; // Return -1 to indicate the queue is empty
        }
        return a[frontIndex];
    }
};
class Queue {
public:
    int front, rear, size;
    unsigned capacity;
    int* array;
};
 

int getPageNumber(string address, int pageSize) {
    unsigned long addr = stoul(address, 0, 16); // Convert hexadecimal string to integer
    cout<<addr/pageSize<<endl;
    return addr/pageSize;
}
// FIFO Algorithm
int simulateFIFO(int * accesses,int n, int tlbSize) {
    unordered_map<int,int> tlb;    // TLB stores page numbers
    // Queue fifoQueue= Queue createQueue(1024);      
    my_queue fifoQueue;
    int hits = 0;

    for (int i=0;i<n;i++) {
        int pageNum=accesses[i];
        if (tlb.count(pageNum)) {
            hits++; // TLB hit
        } else {
            // If TLB is full, remove the oldest entry
            if (tlb.size() == tlbSize) {
                tlb.erase(fifoQueue.front());
                fifoQueue.pop();
            }
            // Insert the new page number
            tlb[pageNum]=true;
            fifoQueue.push(pageNum);
        }
    }
    return hits;
}

//set implemented fifo function
// int simulateFIFO(const vector<int>& accesses, int tlbSize) {
//     unordered_set<int> tlb;  // TLB stores page numbers
//     Queue* fifoQueue = createQueue(tlbSize);  // FIFO queue to manage order of insertion
//     int hits = 0;

//     for (int pageNum : accesses) {
//         if (tlb.count(pageNum)) {
//             hits++; // TLB hit
//         } else {
//             // If TLB is full, remove the oldest entry
//             if (tlb.size() == tlbSize) {
//                 int oldestPage = dequeue(fifoQueue);
//                 tlb.erase(oldestPage);
//             }
//             // Insert the new page number
//             tlb.insert(pageNum);
//             enqueue(fifoQueue, pageNum);
//         }
//     }
//     return hits;
// }



int simulateLIFO(int * accesses,int n, int tlbSize) {
    unordered_map<int,int> tlb;    // TLB stores page numbers
    Stack lifoStack;           // LIFO stack to manage the most recent insertion
    int hits = 0;

    for (int i=0;i<n;i++) {
        int pageNum=accesses[i];
        if (tlb.count(pageNum)) {
            hits++; // TLB hit
        } else {
            // If TLB is full, remove the most recent entry from the stack
            if (tlb.size() == tlbSize) {
                int pageToRemove = lifoStack.pop(); // Pop the most recent page from the stack
                tlb.erase(pageToRemove);            // Remove it from TLB
            }
            // Insert the new page number into TLB and push it to the stack
            tlb[pageNum]=true;
            lifoStack.push(pageNum);                // Push the new page onto the stack
        }
    }
    return hits;
}

// LRU Algorithm
// int simulateLRU(const vector<int>& accesses, int tlbSize) {
//     unordered_map<int, list<int>::iterator> tlbMap; // Map page number to its position in LRU list
//     list<int> lruList;         // Doubly linked list to manage the least recently used pages
//     int hits = 0;

//     for (int pageNum : accesses) {
//         if (tlbMap.count(pageNum)) {
//             hits++; // TLB hit
//             // Move the accessed page to the front of the list (most recently used)
//             lruList.erase(tlbMap[pageNum]);
//             lruList.push_front(pageNum);
//             tlbMap[pageNum] = lruList.begin();
//         } else {
//             // If TLB is full, remove the least recently used page
//             if (tlbMap.size() == tlbSize) {
//                 tlbMap.erase(lruList.back());
//                 lruList.pop_back();
//             }
//             // Insert the new page number at the front (most recently used)
//             lruList.push_front(pageNum);
//             tlbMap[pageNum] = lruList.begin();
//         }
//     }
//     return hits;
// }
int simulateLRU(int * accesses, int n, int tlbSize) {
    unordered_map<int, int> tlb; // Map to track page number and its last accessed index
    int currentIndex = 0;        // To track the current access index
    int hits = 0;

    for (int i=0;i<n;i++) {
        int pageNum=accesses[i];
        if (tlb.find(pageNum) != tlb.end()) {
            // TLB hit
            hits++;
            // Update the access index of the page
            tlb[pageNum] = currentIndex;
        } else {
            // TLB miss
            if (tlb.size() == tlbSize) {
                // If TLB is full, find the least recently used page
                int lruPage = -1;
                int leastRecentIndex = currentIndex;

                for (const auto& entry : tlb) {
                    if (entry.second < leastRecentIndex) {
                        leastRecentIndex = entry.second;
                        lruPage = entry.first;
                    }
                }

                // Remove the least recently used page
                if (lruPage != -1) {
                    tlb.erase(lruPage);
                }
            }

            // Insert the new page into the TLB
            tlb[pageNum] = currentIndex;
        }

        // Move to the next access index
        currentIndex++;
    }

    return hits;
}

// Optimal Algorithm
int simulateOPT(int * accesses,int n, int tlbSize) {
    unordered_map<int,bool> tlb;  // TLB stores page numbers
    // unordered_map<int, int> futureAccessIndex;  // Maps each access to the next access index
    int hits = 0;
    // int n = accesses.size();

    // Precompute future accesses for all pages
   
    unordered_map<int, my_queue> pageFutureAccess;

    for (int i = 0; i < n; i++) {
        int page = accesses[i];
        pageFutureAccess[page].push(i);  // Record when each page is accessed
        // pageFutureAccess->enqueue(i)
    }

    for (int i = 0; i < n; i++) {
        int page = accesses[i];

        // Remove the current access for this page
        pageFutureAccess[page].pop();

        if (tlb.count(page)) {
            // TLB hit
            hits++;
        } else {
            // TLB miss
            if (tlb.size() == tlbSize) {
                // If TLB is full, find the page to replace
                int farthestIndex = -1;
                int pageToReplace = -1;

                for (auto it : tlb) {
                    int tlbPage=it.first;
                    if (pageFutureAccess[tlbPage].get_size()==0) {
                        // If a page won't be used again, replace it
                        pageToReplace = tlbPage;
                        break;
                    } else {
                        // Find the next access index for the page in the future
                        int nextAccess = pageFutureAccess[tlbPage].front();
                        if (nextAccess > farthestIndex) {
                            farthestIndex = nextAccess;
                            pageToReplace = tlbPage;
                        }
                    }
                }

                // Replace the page with the farthest next use or no use at all
                tlb.erase(pageToReplace);
            }

            // Insert the new page number into TLB
            tlb[page]=true;
        }
    }

    return hits;
}


int main() {
    int T;
    cin >> T;
        while (T--) {
        int addressSpaceSize, pageSize, tlbSize, numAccesses;
        cin >> addressSpaceSize >> pageSize >> tlbSize >> numAccesses;

        // vector<int> accesses(numAccesses);
        int *accesses = new int[numAccesses];
        for (int i = 0; i < numAccesses; i++) {
    
            string address;
       
            cin>> address;
         
             accesses[i] = getPageNumber(address, pageSize *1024);

            
          
        }

        // Simulate the four algorithms
        int fifoHits = simulateFIFO(accesses,numAccesses, tlbSize);
        int lifoHits = simulateLIFO(accesses,numAccesses, tlbSize);
        int lruHits = simulateLRU(accesses,numAccesses, tlbSize);
        int optHits = simulateOPT(accesses, numAccesses ,tlbSize);

        // Output the result for this test case
        
        cout << fifoHits << " " << lifoHits << " " << lruHits << " " << optHits << endl;
    }

    return 0;
}
