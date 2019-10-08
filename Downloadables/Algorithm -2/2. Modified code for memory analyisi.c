#include <GL/glut.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
typedef struct{
    int data; //value of node
    struct node* llink; //pointer to left subtree
    struct node* rlink; //pointer to right subtree
    int xcoord;  //xcoordinate of the node
    int ycoord;  //ycoordinate of the node
    int offset;  //distance to each son
    int thread;
    int is_leaf; //if node is a leaf
    struct node* ll; //pointer to leftmost node on lowest level of left subtree
    struct node* lr;//pointer to rightmost node on lowest level of left subtree
    struct node* rl;//pointer to leftmost node on lowest level of right subtree
    struct node* rr;//pointer to rightmost node on lowest level of right subtree
}node;

typedef struct
{
    node *root;
}head;

typedef struct{
    node* addr; //address
    int off;  //offset from root of subtree
    int lev; //tree level
}extreme;

//global variables declaration
head x;
extreme lmost,rmost;
int minsep=2;
int cursep,rootsep,loffsum,roffsum;
int max_height=0,xCenter=0,yCenter=0,r=3;
int memory = 4 + sizeof(head) + sizeof(extreme) + sizeof(extreme)+(sizeof(int)*9);
void init(void)
{
    glClearColor(1,1,1,0);
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(0.0,1000.0,0.0,1000.0);
}

void setPixel(float xCoordinate, float yCoordinate)
{
    xCoordinate=(float)xCoordinate/100.0;
    yCoordinate=(float)yCoordinate/100.0;
    glBegin(GL_POINTS);
    glVertex2f(xCoordinate,yCoordinate);
    glEnd();
    glFlush(); //executes all OpenGL functions as quickly as possible
}

void bresenhamAlg (int x0, int y0, int x1, int y1)
{
    int dx = abs (x1 - x0);
    int dy = abs (y1 - y0);
    int x, y;
    if (dx >= dy)
    {
        int d = 2*dy-dx;
        int ds = 2*dy;
        int dt = 2*(dy-dx);
        if (x0 < x1)
        {
            x = x0;
            y = y0;
        }
        else
        {
            x = x1;
            y = y1;
            x1 = x0;
            y1 = y0;
        }
        setPixel (x, y);
        while (x < x1)
        {
            if (d < 0)
                d += ds;
            else
            {
                if (y < y1)
                {
                    y++;
                    d += dt;
                }
                else
                {
                    y--;
                    d += dt;
                }
            }
            x++;
            setPixel (x, y);
        }
    }
    else
    {
        int d = 2*dx-dy;
        int ds = 2*dx;
        int dt = 2*(dx-dy);
        if (y0 < y1)
        {
            x = x0;
            y = y0;
        }
        else
        {
            x = x1;
            y = y1;
            y1 = y0;
            x1 = x0;
        }
        setPixel (x, y);
        while (y < y1)
        {
            if (d < 0)
                d += ds;
            else
            {
                if (x > x1)
                {
                    x--;
                    d += dt;
                }
                else
                {
                    x++;
                    d += dt;
                }
            }
            y++;
            setPixel (x, y);
        }
    }
} // end of bresenham

void PlotCircle(int x, int y)
{
    float xc = xCenter/100.0;
    float yc = yCenter/100.0;
    glColor3f(0.0f,0.0f,0.0f);
    float xf = (float)x/100;
    float yf = (float)y/100;
    
    glVertex2f(xf+xc, yf+yc);
    glVertex2f(-xf+xc, yf+yc);
    glVertex2f(xf+xc, -yf+yc);
    glVertex2f(-xf+xc, (-yf+yc));
    
    glVertex2f(yf+xc, (xf+yc));
    glVertex2f(-yf+xc, (xf+yc));
    glVertex2f(yf+xc, (-xf+yc));
    glVertex2f(-yf+xc, (-xf+yc));
    
    return;
}

void Circle()
{
    int x = 0;
    int y = r;
    float decision = (5.0/4.0) - r;
    glBegin(GL_POINTS);
    PlotCircle(x, y);
    while(y>x)
    {
        if(decision < 0)
        {
            //east
            decision+=2.0*x + 3.0;
        }
        else
        {
            //south-east.
            decision+=2.0*(x-y)+5.0;
            y--;
        }
        x++;
        PlotCircle(x, y);
    }
    PlotCircle(x, y);//here, x==y;
    glEnd();
}
//function setup assigns relative x coordinates to all nodes during a postorder traversal.
void setup( node* t,int level,extreme rmost,extreme lmost)
{
    node* l;
    node* r;
    extreme lr,ll,rr,rl;
    memory += 2*(sizeof(node)) + 4*(sizeof(extreme));
    if(t!=NULL)
    {
        
        t->ycoord=level;
        l=t->llink; //follows contour of left subtree
        r=t->rlink; //follows contour of right subtree
        setup(l,level+1,lr,ll); //position subtrees recursively
        setup(r,level+1,rr,rl);
        ll.addr=t->ll; //assigning of extreme type ll,lr,rr,rl to node's ll,lr,rl,rr
        lr.addr=t->lr;
        rl.addr=t->rl;
        rr.addr=t->rr;
        if(t->llink==NULL && t->rlink==NULL)
        { //a leaf is both the leftmost and rightmost node on the lowest level of subtree consisting of itself
            rmost.addr=t;
            lmost.addr=t;
            rmost.lev=level;
            lmost.lev=level;
            rmost.off=0;
            lmost.off=0;
            t->offset=0;
        }
        else
        { //when node is not a leaf
            cursep=minsep;
            rootsep=minsep;
            loffsum=0;
            roffsum=0;
            //consider each level in turn until one subtree is exhausted,pushing the subtrees apart when neccessary
            while(l!=NULL &&r!=NULL)
            {
                if(cursep<minsep)
                {//push subtrees apart
                    rootsep=rootsep+(minsep-cursep);
                    cursep=minsep;
                } //advance l and r
                
                if(l->rlink!=NULL)
                {
                    loffsum=loffsum+l->offset;
                    cursep=cursep-l->offset;
                    l=l->rlink;
                }
                else
                {
                    loffsum=loffsum-l->offset;
                    cursep=cursep+l->offset;
                    l=l->llink;
                }
                if(r->llink!=NULL)
                {
                    roffsum=roffsum-r->offset;
                    cursep=cursep-r->offset;
                    r=r->llink;
                }
                else
                {
                    roffsum=roffsum+r->offset;
                    cursep=cursep+r->offset;
                    r=r->rlink;
                } /*else ends*/
            } /*while loop end */
            //set the offset in node and include in it accumulated offsets for left and right subtree of it.
            t->offset=(rootsep+1)/2;
            loffsum=loffsum-t->offset;
            roffsum=roffsum+t->offset;
            //updating extreme descendants information
            if(rl.lev>ll.lev || t->llink==NULL)
            {
                lmost=rl;
                lmost.off=lmost.off+t->offset;
            }
            else
            {
                lmost=ll;
                lmost.off=lmost.off-t->offset;
            }
            if(lr.lev>rr.lev || t->rlink==NULL)
            {
                rmost=lr;
                rmost.off=rmost.off-t->offset;
            }
            else
            {
                rmost=rr;
                rmost.off=rmost.off+t->offset;
            }
            //if subtrees of node are of uneven heights then checking if threading is necessary then atmost one thread needs to be inserted.
            if(l!=NULL && l!=t->llink)
            {
                rr.addr->thread=1;//true
                rr.addr->offset=abs((rr.off+t->offset)-loffsum);
                if(loffsum-t->offset <= rr.off)
                {
                    rr.addr->llink=l;
                }
                else
                {
                    rr.addr->rlink=l;
                }
            }
            else if(r!=NULL && r!=t->rlink)
            {
                ll.addr->thread=1;//true
                ll.addr->offset=abs((ll.off-t->offset)-roffsum);
                if(roffsum+t->offset>=ll.off)
                {
                    ll.addr->rlink=r;
                }
                else
                {
                    ll.addr->llink=r;
                }
            }
        }/*end of if t not leaf*/
    }/*end of t!=NULL*/
}/*end of function setup*/

//function petrify converts relative positionings to absolute coordinates
void petrify(node* t,int xpos)
{
    memory+=sizeof(node) + sizeof(int );
    if(t!=NULL)
    {
        t->xcoord=xpos;
        if(t->thread)
        {
            t->thread=0;
            t->rlink=NULL; //threade node must be a leaf
            t->llink=NULL;
        }
        petrify(t->llink,xpos-t->offset);
        petrify(t->rlink,xpos+t->offset);
    } //end of t!=NULL
}//end of petrify function

void insert(head *t,int ele)
{
    memory += sizeof(int) + sizeof(int);
    node *p,*q;
    p=(node*)malloc(sizeof(node));
    memory += (sizeof(int))*2 + sizeof(node);
    p->data=ele;
    p->llink=NULL;
    p->rlink=NULL;
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
                if(q->llink==NULL)
                {
                    q->llink=p;
                    break;
                }
                q=q->llink;
            }
            else if(ele>q->data)
            {
                if(q->rlink==NULL)
                {
                    q->rlink=p;
                    break;
                }
                q=q->rlink;
            }
        }
    }
    p->ycoord=height(t,ele);
    if(p->ycoord >=max_height)
    {
        max_height=p->ycoord;
    }
}

int height(head *t,int val)
{
    memory += 3*(sizeof(int));
    node *q=t->root;
    int i=0;
    while(q!=NULL)
    {
        if(val==q->data)
        {
            q->is_leaf=is_leaf_func(q);
            return i;
        }
        else if(val<q->data)
        {
            q=q->llink;
            i++;
        }
        else if(val>q->data)
        {
            q=q->rlink;
            i++;
        }
    }
    return i;
}

int is_leaf_func(node *y)
{
    memory+=sizeof(int);
    if(y->llink==NULL && y->rlink==NULL)
    {
        return 1;
    }
    return 0;
}

void preorder(node* y)
{
    memory+=(sizeof(int));
    if(y!=NULL)
    {
        //printf("%d %d %d %d %d\n",y->data,y->xcoord,y->ycoord,y->offset,y->thread);
        preorder(y->llink);
        preorder(y->rlink);
    }
}


void preorder2(node* x)
{
    memory += sizeof(int);
    if(x!=NULL)
    {
        xCenter=10*x->xcoord;
        yCenter=10*(max_height-x->ycoord);
        Circle();
        if(x->llink!=NULL)
        {
            node *temp;
            temp=x->llink;
            int xCenter1=10*temp->xcoord;
            int yCenter1=10*(max_height-temp->ycoord);
            memory+=sizeof(int)*3;
            bresenhamAlg(xCenter,yCenter,xCenter1,yCenter1);
        }
        if(x->rlink!=NULL)
        {
            node *temp;
            temp=x->rlink;
            int xCenter1=10*temp->xcoord;
            int yCenter1=10*(max_height-temp->ycoord);
            memory+=sizeof(int)*3;
            bresenhamAlg(xCenter,yCenter,xCenter1,yCenter1);
        }
        preorder2(x->llink);
        preorder2(x->rlink);
    }
}

void display()
{
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    preorder2(x.root);
    glFlush();
}

void resize(int width, int height)
{
    if(height == 0)
    {
        height = 1;
    }
    GLfloat aspect = (float)width / (float)height;
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if(width>=height)
    {
        gluOrtho2D(-1.0f * aspect, 1.0f * aspect, -1.0f, 1.0f);
    }
    else
    {
        gluOrtho2D(-1.0f,1.0f,-1.0f/aspect,1.0f/aspect);
    }
}
//function lrmostCalc to calculate the leftmost nad rightmost node for the root of binary search tree
int c=1;
void lrmostCalc(node *x)
{
    memory+=sizeof(int);
    if(x!=NULL)
    {
        lrmostCalc(x->llink);
        lrmostCalc(x->rlink);
        if(x->ycoord==max_height&&c==1)
        {
            lmost.addr=x;
            c=0;
        }
        if(x->ycoord==max_height)
        {
            rmost.addr=x;
        }
    }
}
//postorder traversal to calculate ll,lr,rr,rl for each node in binary tree.
void postorder(node* y)
{
    memory+=sizeof(int);
    if(y!=NULL)
    {
        postorder(y->llink);
        postorder(y->rlink);
        if(y->llink==NULL && y->rlink==NULL)
        {
            //if node is a leaf then node itself would be ll,lr,rl,rr
            node* temp1=y->ll=y;
            node* temp2=y->lr=y;
            node* temp3=y->rl=y;
            node* temp4=y->rr=y;
            memory+=4*(sizeof(int));
        }
        else if(y->llink==NULL)
        {
            //if node doesn't have left child then ll and lr would be node itself.
            y->ll=y;
            y->lr=y;
        }
        else if(y->rlink==NULL)
        {
            //if node doesn't have right child then rl and rr would be node itself.
            y->rr=y;
            y->rl=y;
        }
        if(y->llink!=NULL)
        {
            
            //if the node has left child then calculation of ll and lr
            node* temp;
            temp=y->llink;
            node *temp1;
            temp1=temp->ll;
            node *temp2;
            temp2=temp->rr;
            memory+=sizeof(int)*3;
            if(temp1->ycoord>temp2->ycoord)
            {
                y->ll=temp->ll;
                y->lr=temp->lr;
            }
            else if(temp1->ycoord<temp2->ycoord)
            {
                y->ll=temp->rl;
                y->lr=temp->rr;
            }
            else if(temp1->ycoord==temp2->ycoord)
            {
                y->ll=temp->ll;
                y->lr=temp->rr;
            }
            temp1=y->ll;
            temp2=y->lr;
        }
        if(y->rlink!=NULL)
        {
            //if node has right child then calculation of rl and rr.
            node *temp=y->rlink;
            node *temp1=temp->ll;
            node *temp2=temp->rr;
            memory+=sizeof(int)*3;
            if(temp1->ycoord>temp2->ycoord)
            {
                y->rl=temp->ll;
                y->rr=temp->lr;
            }
            else if(temp1->ycoord<temp2->ycoord)
            {
                y->rl=temp->rl;
                y->rr=temp->rr;
            }
            else if(temp1->ycoord==temp2->ycoord)
            {
                y->rl=temp->ll;
                y->rr=temp->rr;
            }
            node* temp3=y->rl;
            node* temp4=y->rr;
            memory+=sizeof(int)*2;
        }
        
        node *temp1=y->ll;
        node *temp2=y->lr;
        node *temp3=y->rl;
        node *temp4=y->rr;
        memory+=sizeof(int)*4;
    }
}
int main(int argc,char **argv)
{
    
    
    int i = 0,e=0;
    
    int ele  = 0;
    memory+=sizeof(int)*3;
    for(;i<501000;){
        x.root=NULL;
        srand(time(NULL));
        for(;e<=i;++e){
            ele = rand()*(1000);
            insert(&x,ele);
        }
        lrmostCalc(x.root);
        postorder(x.root);
        setup(x.root,0,rmost,lmost);
        petrify(x.root,0);
        preorder(x.root);
        FILE* filepointer = fopen("./memory-1.txt","a");;
        fprintf(filepointer,"%f",(float)memory/1000);
        
        fputs(" ",filepointer);
        fprintf(filepointer,"%d",i);
        fputs("\n",filepointer);
        fclose(filepointer);
        i+=1000;
    }
    
    return 0;
}
