#include<iostream>
#include <vector>
#include <cmath>
using namespace std;

#define OPRN_CANT_BE_PERF -1

struct BinomialTreeNode{
    BinomialTreeNode *parent;
    int key;
    int degree;
    //leftmost child
    BinomialTreeNode *child;
    BinomialTreeNode *sibling;
    BinomialTreeNode(int k){
        parent=nullptr; key=k; degree=0; child=nullptr; sibling=nullptr;
    }
};

BinomialTreeNode *parent(BinomialTreeNode *x){
    return x->parent;
}

int key(BinomialTreeNode *x){
    return x->key;
}

int degree(BinomialTreeNode *x){
    return x->degree;
}

BinomialTreeNode *child(BinomialTreeNode *x){
    return x->child;
}

BinomialTreeNode *sibling(BinomialTreeNode *x){
    return x->sibling;
}

BinomialTreeNode *binomialLink(BinomialTreeNode *parent,BinomialTreeNode *newChild){
    if(parent==nullptr || newChild==nullptr || degree(parent)!=degree(newChild)) return parent;
    //make new child as leftmost child of parent and ex-child as sibling of leftmost child
    BinomialTreeNode *temp=parent->child;
    parent->child=newChild;
    newChild->sibling=temp;
    parent->degree+=1;
    //set parent as parent of new child
    newChild->parent=parent;
    return parent;
}

class BinomialHeap{
    private:
    BinomialTreeNode *rootListHead;

    BinomialTreeNode *mergeRootLists(BinomialTreeNode *rootList1,BinomialTreeNode *rootList2){
        //if one list is null
        if(rootList1==nullptr) return rootList2;
        if(rootList2==nullptr) return rootList1;
        BinomialTreeNode *mergedRootList=nullptr;
        BinomialTreeNode *temp=nullptr;
        while(rootList1!=nullptr && rootList2!=nullptr){
            if(degree(rootList1)<=degree(rootList2)){
                if(mergedRootList==nullptr){
                    mergedRootList=rootList1;
                    temp=mergedRootList;
                }else{
                    temp->sibling=rootList1;
                    temp=temp->sibling;
                } 
                rootList1=rootList1->sibling;  
            }else{
                if(mergedRootList==nullptr){
                    mergedRootList=rootList2;
                    temp=mergedRootList;
                }else{
                    temp->sibling=rootList2;
                    temp=temp->sibling;
                } 
                rootList2=rootList2->sibling;
            }
        }
        while(rootList1!=nullptr){
            temp->sibling=rootList1;
            rootList1=rootList1->sibling;
            temp=temp->sibling;
        }
        while(rootList2!=nullptr){
            temp->sibling=rootList2;
            rootList2=rootList2->sibling;
            temp=temp->sibling;
        }
        return mergedRootList;
    }

    BinomialTreeNode *unionBH(BinomialTreeNode *rootList1,BinomialTreeNode *rootList2){
        //if one list is null
        if(rootList1==nullptr) return rootList2;
        if(rootList2==nullptr) return rootList1;
        BinomialTreeNode *mergedList=mergeRootLists(rootList1,rootList2);
        BinomialTreeNode *rootList=nullptr;
        BinomialTreeNode *prev=nullptr;
        BinomialTreeNode *x=mergedList;
        BinomialTreeNode *nx=x->sibling; //next of x
        BinomialTreeNode *snx=nx->sibling; //sibling of next of x
        while(snx!=nullptr){
            if(degree(x)==degree(nx) && degree(x)!=degree(snx)){
                if(key(x)>key(nx)) x=binomialLink(nx,x);
                else x=binomialLink(x,nx);
                x->sibling=snx;
                if(prev==nullptr) rootList=x;
                else prev->sibling=x;
            }else{
                if(rootList==nullptr) rootList=x;
                prev=x;
                x=nx;
            }
            nx=snx;
            snx=snx->sibling;
        }
        if(degree(x)==degree(nx)){ //size two or at end match case
            if(key(x)>key(nx)) x=binomialLink(nx,x);
            else x=binomialLink(x,nx);
            x->sibling=nullptr;
            if(prev==nullptr) rootList=x;
            else prev->sibling=x;
        }
        if(rootList==nullptr) rootList=x; //size two no match case
        return rootList;
    }

    void findNode(BinomialTreeNode *node,int key,BinomialTreeNode *&target){
        if(node==nullptr) return;
        if(node->key==key){
            target=node;
            return;
        }
        findNode(node->sibling,key,target);
        findNode(node->child,key,target);
    }

    void printTree(BinomialTreeNode *root,int depth){
        if(root==nullptr) return;
        for(int i=0;i<depth;i++) cout<<"  ";
        cout<<key(root);
        BinomialTreeNode *temp=root->child;
        while(temp!=nullptr){
            cout<<"\n";
            printTree(temp,depth+1);
            temp=temp->sibling;
        }
    }

    public:
    BinomialHeap(){
        rootListHead=nullptr;
    }

    void insert(int key){
        BinomialTreeNode *newNode=new BinomialTreeNode(key);
        rootListHead=unionBH(rootListHead,newNode);
    }

    int getMin(){
        if(rootListHead==nullptr) return OPRN_CANT_BE_PERF;
        BinomialTreeNode *temp=rootListHead;
        int min=INT_MAX;
        while(temp!=nullptr){
            if(key(temp)<min) min=temp->key;
            temp=temp->sibling;
        }
        return min;
    }

    int extractMin(){
        if(rootListHead==nullptr) return OPRN_CANT_BE_PERF;
        BinomialTreeNode *temp=rootListHead;
        int min=getMin();
        BinomialTreeNode *prevMin=nullptr;
        while(temp!=nullptr){
            if(key(temp)==min) break;
            prevMin=temp;
            temp=temp->sibling;
        }
        BinomialTreeNode *minNode=temp;
        if(prevMin==nullptr) rootListHead=rootListHead->sibling;
        else prevMin->sibling=minNode->sibling;
        //set parent to null of all child of minNode to make another root list
        temp=minNode->child;
        while(temp!=nullptr){
            temp->parent=nullptr;
            temp=temp->sibling;
        }
        rootListHead=unionBH(rootListHead,minNode->child);
        delete minNode;
        return min;
    }

    bool decreaseKey(int key,int newKey){
        if(key<=newKey) return false;
        BinomialTreeNode *target=nullptr;
        findNode(rootListHead,key,target);
        if(target==nullptr) return false;
        while(target->parent!=nullptr && (target->parent->key)>newKey){
            target->key=target->parent->key;
            target=target->parent;
        }
        target->key=newKey;
        return true;
    }

    bool deleteNode(int key){
        if(decreaseKey(key,INT_MIN)){
            extractMin();
            return true;
        }
        return false;
    }

    void display(){
        if(rootListHead==nullptr) cout<<"Binomial Heap is Empty.\n";
        BinomialTreeNode *temp=rootListHead;
        while(temp!=nullptr){
            cout<<"\nB"<<degree(temp)<<" Tree:\n";
            printTree(temp,0);
            temp=temp->sibling;
        }      
    }
};

int main(){
    BinomialHeap bh;
    bh.insert(10);
    bh.insert(5);
    bh.insert(15);
    bh.insert(3);
    bh.deleteNode(15);
    bh.display();
    return 0;
}