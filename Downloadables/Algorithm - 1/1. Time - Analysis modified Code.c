#include<stdio.h>
#include<stdlib.h>
#include<time.h>
int max_height=0;

typedef struct{
    int data;
    struct node* left;
    struct node* right;
    int height;
    int place;
    int is_leaf;
}node;

typedef struct
{
    node *root;
}head;

void insert(head *t,int ele)
{
    node *p,*q;
    p=(node*)malloc(sizeof(node));
    p->data=ele;
    p->left=NULL;
    p->right=NULL;
    if(t->root==NULL)
    {
        t->root=p;
        
    }
    else
    {
        q=t->root;
        while(q!=NULL)
        {
            if(ele<=q->data)
            {
                if(q->left==NULL)
                {
                    q->left=p;
                    break;
                }
                q=q->left;
            }
            else if(ele>q->data)
            {
                if(q->right==NULL)
                {
                    q->right=p;
                    break;
                }
                q=q->right;
            }
        }
    }
    
    
    p->height=height(t,ele);
    
    if(p->height >=max_height)
    {
        max_height=p->height;
    }
}


int height(head *t,int val)
{
    
    node *q=t->root;
    int i=0;
    while(q!=NULL)
    {
        if(val==q->data)
            return i;
        else if(val<q->data)
        {
            q=q->left;
            i++;
        }
        else if(val>q->data)
        {
            q=q->right;
            i++;
        }
    }
    return -1;
}
int is_leaf_func(node *y)
{
    if(y->left==NULL && y->right==NULL)
    {
        return 1;
    }
    return 0;
}
void postorder(node *y,int next_pos[],int modifier[])
{
    if(y!=NULL)
    {
        postorder(y->left,next_pos,modifier);
        postorder(y->right,next_pos,modifier);
        y->is_leaf=is_leaf_func(y);
        if(y->is_leaf==1)
        {
            y->place=next_pos[y->height];
        }
        else if(y->right==NULL)
        {node *temp;
            temp=y->left;
            y->place = temp->place+1;
        }
        else if(y->left==NULL)
        {
            node *temp;
            temp=y->right;
            y->place=temp->place-1;
        }
        else
        {
            node *temp;
            temp=y->right;
            node *temp1;
            temp1=y->left;
            y->place =(temp1->place + temp->place)/2;
        }
        
        modifier[y->height]=(modifier[y->height]>next_pos[y->height]-y->place)?modifier[y->height]:next_pos[y->height]-y->place;
        if(y->is_leaf==0)
        {
            y->place =y->place+modifier[y->height];
        }
        next_pos[y->height]=y->place+2;
        //        printf("%d %d %d %d %d\n",y->data,y->height,y->is_leaf,y->place,modifier[y->height]);
    }
}
int main()
{
    int xyz = 0;
    for(;xyz<501000;){
        clock_t start = clock();
        head x;
        
        x.root=NULL;
        srand(time(NULL));
        for(int i = 0; i<xyz;++i)
        {
            int e = rand()%(1000);
            insert(&x, e);
        }
//        printf("The maximum height of tree is %d\n",max_height);
        
        int modifier[max_height+1];
        int next_pos[max_height+1];
        int i;
        for(i=0;i<=max_height;i++)
        {
            modifier[i]=0;
            next_pos[i]=1;
        }
        postorder(x.root,next_pos,modifier);
        clock_t end = clock();
        float seconds = (float)(end-start)/CLOCKS_PER_SEC;
//        printf("time: %f\n",seconds);
        FILE* filepointer;
        filepointer = fopen("./time-1.txt","a");
        fprintf(filepointer,"%f",seconds);
        fputs(" ",filepointer);
        fprintf(filepointer,"%d",xyz);
        fputs("\n",filepointer);
        fclose(filepointer);
        xyz = xyz +1000;
    }
    return 0;
}
