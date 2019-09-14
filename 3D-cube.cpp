#include<iostream>
#include<cmath>
#include <windows.h>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <stdlib.h>

using namespace std;

static int slices = 16;
static int stacks = 16;

float **rotator;
float **coordinates;
float **projector;
float **Dcoordinates;

float Xp=0.0f, Yp=0.0f, Zp=0.0f, Xc=0.0f, Yc=0.0f, Zc=-200.0f, s;
float def=1.0f;

float X=200,Y=200,Z=-200;

float ka=1.0f;
float kd=1.0f;
float ks=1.0f;

float Iamb=0.1f;
float Ip=0.8f;
float Is=1.0f;

float cosTheta=0;
float n=50;

int delay=10;
float angle=1.0f, temp=1,temp2=1;
int radi=5;

void drawPixel(int x, int y,float r, float g, float b)
{
    glBegin(GL_POINTS);
        glColor4f(r,g,b,1);
		glVertex2i(x,y);
	glEnd();
}

void DrawLine(int x1,int y1,int x2,int y2,float r,float g,float b) //x1 < x2
{
    if(x1>x2)
    {
        swap(x1,x2);
        swap(y1,y2);
    }

    int dx=x2-x1;
	int dy=y2-y1;

	int x=x1, y=y1;

    if(dy==0)//horizontal
    {
        while(x<x2)
        {
            drawPixel(x,y,r,g,b);
            x++;
        }
    }
    else if(dx==0)//vertical
    {
        if(y>y2)
        {
            while(y>=y2)
            {
                drawPixel(x,y,r,g,b);
                y--;
            }
        }
        else
        {
            while(y<y2)
            {
                drawPixel(x,y,r,g,b);
                y++;
            }
        }
    }
    else if(dy==dx)//1
    {
        while(x<x2)
        {
            drawPixel(x,y,r,g,b);
            x++;
            y++;
        }
    }
    else if(-dy==dx)//-1
    {
        while(x<x2)
        {
            drawPixel(x,y,r,g,b);
            x++;
            y--;
        }
    }
    else if(dy>=0)
    {
        if(dy>dx)//1 to infinity
        {
            int d=dy-2*dx;
            int increN=2*dx;
            int incrNE=2*(dx-dy);

            drawPixel(x,y,r,g,b);

            while (y < y2)
            {
                if (d <= 0)
                {
                    d+=increN;
                    y++;
                }
                else
                {
                    d+=incrNE;
                    x++;
                    y++;
                }
                drawPixel(x,y,r,g,b);
            }
        }
        else if(dy<dx)//0 to 1
        {
            int d=2*dy-dx;
            int increE=2*dy;
            int incrNE=2*(dy-dx);

            drawPixel(x,y,r,g,b);

            while (x < x2)
            {
                if (d <= 0)
                {
                    d+=increE;
                    x++;
                }
                else
                {
                    d+=incrNE;
                    x++;
                    y++;
                }
                drawPixel(x,y,r,g,b);
            }
        }
    }
    else if(dy<0)
    {
        if(-dy>dx)//-1 to -infinity
        {
            int d=dy+2*dx;
            int increS=2*dx;
            int incrSE=2*(dy+dx);

            drawPixel(x,y,r,g,b);

            while (y > y2)
            {
                if (d <= 0)
                {
                    d+=increS;
                    y--;
                }
                else
                {
                    d+=incrSE;
                    y--;
                    x++;
                }
                drawPixel(x,y,r,g,b);
            }
        }
        else if(-dy<dx) //0 to -1
        {
            int d=2*dy+dx;
            int increE=2*dy;
            int incrSE=2*(dy+dx);

            drawPixel(x,y,r,g,b);

            while (x < x2)
            {
                if (d <= 0)
                {
                    d+=incrSE;
                    x++;
                    y--;
                }
                else
                {
                    d+=increE;
                    x++;
                }
                drawPixel(x,y,r,g,b);
            }
        }
    }
}

void drawLines(int Xp, int Yp,int X, int Y)
{
    DrawLine(Xp+X,Yp+Y,-Xp+X,Yp+Y,1,1,1);
    DrawLine(Xp+X,-Yp+Y,-Xp+X,-Yp+Y,1,1,1);
    DrawLine(Yp+X,Xp+Y,-Yp+X,Xp+Y,1,1,1);
    DrawLine(Yp+X,-Xp+Y,-Yp+X,-Xp+Y,1,1,1);
}

void drawCircle(int r, int X, int Y)
{
    int x=0;
    int y=r;
    int h=1-r;
    DrawLine(-r+X,Y,r+X,Y,1,1,1);
    while(y>=x)
    {
        if(h<=0)
        {
            h=h+2*x+3;
            x++;
        }
        else
        {
            h=h+2*(x-y)+5;
            x++,y--;
        }
        drawLines(x,y,X,Y);
    }
}

bool insidePolygon(float x, float y, float co[])
{
    bool det=true;
    float a,b,c;

    a=-(co[1]-co[7]);//-(y1-y4)
    b=co[0]-co[6];//x1-x4
    c=-(a*co[6]+b*co[7]);//-(ax4+by4)

    if((a*x+b*y+c)<0)
    {
        for(int i=0;i<3;i++)
        {
            a=-(co[2*i+3]-co[2*i+1]);
            b=co[2*i+2]-co[2*i];
            c=-(a*co[2*i]+b*co[2*i+1]);

            if((a*x+b*y+c)>0)
            {
                det=false;
                break;
            }
        }

    }
    else if((a*x+b*y+c)>0)
    {
        for(int i=0;i<3;i++)
        {
            a=-(co[2*i+3]-co[2*i+1]);
            b=co[2*i+2]-co[2*i];
            c=-(a*co[2*i]+b*co[2*i+1]);

            if((a*x+b*y+c)<0)
            {
                det=false;
                break;
            }
        }
    }

    return det;
}

float getInterpolatedZ(float c[], float i, float j, float min_x, float max_x)
{
    bool once=false;
    bool twice=false;
    float z1=0, z2=0, x1=0, x2=0;

    float eq;

    if((c[3]-c[1])!=0)
        eq=((j-c[1])*(c[2]-c[0])/(c[3]-c[1]))+c[0];
    else
        eq=j;

    if(eq>=min_x && eq<=max_x)
    {
        once=true;
        z1=c[8]+(c[9]-c[8])*(c[1]-j)/(c[1]-c[3]);
        x1=eq;
    }

    if((c[5]-c[3])!=0)
        eq=((j-c[3])*(c[4]-c[2])/(c[5]-c[3]))+c[2];
    else
        eq=j;

    if(eq>=min_x && eq<=max_x)
    {
        if(once)
        {
            z2=c[9]+(c[10]-c[9])*(c[3]-j)/(c[3]-c[5]);
            x2=eq;
            twice=true;
        }
        else
        {
            once=true;
            z1=c[9]+(c[10]-c[9])*(c[3]-j)/(c[3]-c[5]);
            x1=eq;
        }
    }

    if((c[7]-c[5])!=0)
        eq=((j-c[5])*(c[6]-c[4])/(c[7]-c[5]))+c[4];
    else
        eq=j;

    if(eq>=min_x && eq<=max_x)
    {
        if(once && !twice)
        {
            twice=true;
            z2=c[10]+(c[11]-c[10])*(c[5]-j)/(c[5]-c[7]);
            x2=eq;
        }
        else if(!once && !twice)
        {
            once=true;
            z1=c[10]+(c[11]-c[10])*(c[5]-j)/(c[5]-c[7]);
            x1=eq;
        }
    }

    if((c[1]-c[7])!=0)
        eq=((j-c[7])*(c[0]-c[6])/(c[1]-c[7]))+c[6];
    else
        eq=j;

    if(eq>=min_x && eq<=max_x)
    {
        if(once && !twice)
        {
            z2=c[11]+(c[8]-c[11])*(c[7]-j)/(c[7]-c[1]);
            x2=eq;
            twice=true;
        }
    }
    if(x1!=x2)
        return (z1+(((z2-z1)*(x1-i))/(x1-x2)));
    else
        return ((z1+z2)/2);
}

void fillSurface(float c[], float r, float g, float b, float I, float J, float K)
{
    int num=4;

    float min_x=c[0];
    float min_y=c[1];

    float max_x=c[0];
    float max_y=c[1];

    for(int i=0;i<num;i++)
    {
        if(c[2*i]<min_x)//x values
            min_x=c[2*i];
        if(c[2*i+1]<min_y)//y values
            min_y=c[2*i+1];

        if(c[2*i]>max_x)//x values
            max_x=c[2*i];
        if(c[2*i+1]>max_y)//y values
            max_y=c[2*i+1];
    }

    for(float i=min_x;i<=max_x;i++)
    {
        for(float j=min_y;j<=max_y;j++)
        {
            if(insidePolygon(i,j,c))
            {
                float k=getInterpolatedZ(c, i,j, min_x, max_x);

                cosTheta=((I*(X-i)+J*(Y-j)+K*(Z-k))/(sqrt(pow(X-i,2)+pow(Y-j,2)+pow(Z-k,2))));

                float temp;
                if(cosTheta>=0)
                {
                    temp=(Iamb*ka+Ip*kd*cosTheta);
                }
                else//surface is on the backside, no diffusion
                {
                    temp=(Iamb*ka);
                }

                float phi=0.5*acos(cosTheta);

                float I=Is*ks*pow(cos(phi),n);

                if(I>=0.95 && I<=1)
                {
                    if(r==0 && g==0 && b==1)
                        drawPixel(i,j,pow(I,n),pow(I,n),I);
                    else if(r==0 && g==1 && b==0)
                        drawPixel(i,j,pow(I,n),I,pow(I,n));
                    else if(r==0 && g==1 && b==1)
                        drawPixel(i,j,pow(I,n),I,I);
                    else if(r==1 && g==0 && b==0)
                        drawPixel(i,j,I,pow(I,n),pow(I,n));
                    else if(r==1 && g==0 && b==1)
                        drawPixel(i,j,I,pow(I,n),I);
                    else if(r==1 && g==1 && b==0)
                        drawPixel(i,j,I,I,pow(I,n));
                }
                else
                    drawPixel(i,j,r*(temp+I),g*(temp+I),b*(temp+I));
            }
        }
    }
}

void homogenize(float **c)
{
    for(int i=0; i<8; i++)
    {
        c[0][i]=c[0][i]/c[3][i];
        c[1][i]=c[1][i]/c[3][i];
        c[2][i]=c[2][i]/c[3][i];
        c[3][i]=1;
    }
}

void matrixMultiply(float **a, float **b)
{
    float result[4][8];
    for(int i=0; i<4; i++)
    {
        for(int j=0; j<8; j++)
        {
            float ans=0;
            for(int l=0; l<4; l++)
            {
                ans+=a[i][l]*b[l][j];
            }
            result[i][j]=ans;
        }
    }
    for(int i=0; i<4; i++)
    {
        for(int j=0; j<8; j++)
            b[i][j]=result[i][j];
    }
    homogenize(b);
}

/* GLUT callback Handlers */

static void resize(int width, int height)
{
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glOrtho(-width/2, width/2, -height/2, height/2, -300, 300);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity() ;
}

void project()
{
    float dx, dy, dz, Q;
    Q=sqrt(pow((Xc-Xp),2)+pow((Yc-Yp),2)+pow((Zc-Zp),2));

    dx=(Xc-Xp)/Q;
    dy=(Yc-Yp)/Q;
    dz=(Zc-Zp)/Q;

    projector[0][0]=1.0f;
    projector[0][1]=0.0f;
    projector[0][2]=-dx/dz;
    projector[0][3]=Zp*(dx/dz);
    projector[1][0]=0.0f;
    projector[1][1]=1.0f;
    projector[1][2]=-dy/dz;
    projector[1][3]=Zp*(dy/dz);
    projector[2][0]=0.0f;
    projector[2][1]=0.0f;
    projector[2][2]=-Zp/(Q*dz);
    projector[2][3]=Zp+(pow(Zp,2)/(Q*dz));
    projector[3][0]=0.0f;
    projector[3][1]=0.0f;
    projector[3][2]=-1/(Q*dz);
    projector[3][3]=1+(Zp/(Q*dz));

    matrixMultiply(projector,Dcoordinates);
}

void setRotate(char axis)
{
    if(axis=='y')
    {
    rotator[0][0]=cos((atan(1)*4)*angle/180);
    rotator[0][1]=0.0f;
    rotator[0][2]=sin((atan(1)*4)*angle/180);
    rotator[0][3]=0.0f;
    rotator[1][0]=0.0f;
    rotator[1][1]=1.0f;
    rotator[1][2]=0.0f;
    rotator[1][3]=0.0f;
    rotator[2][0]=-sin((atan(1)*4)*angle/180);
    rotator[2][1]=0.0f;
    rotator[2][2]=cos((atan(1)*4)*angle/180);
    rotator[2][3]=0.0f;
    rotator[3][0]=0.0f;
    rotator[3][1]=0.0f;
    rotator[3][2]=0.0f;
    rotator[3][3]=1.0f;
    }
    else if(axis=='x')
    {
    rotator[0][0]=1;
    rotator[0][1]=0;
    rotator[0][2]=0;
    rotator[0][3]=0;
    rotator[1][0]=0;
    rotator[1][1]=cos((atan(1)*4)*angle/180);
    rotator[1][2]=-sin((atan(1)*4)*angle/180);
    rotator[1][3]=0;
    rotator[2][0]=0;
    rotator[2][1]=sin((atan(1)*4)*angle/180);
    rotator[2][2]=cos((atan(1)*4)*angle/180);
    rotator[2][3]=0;
    rotator[3][0]=0;
    rotator[3][1]=0;
    rotator[3][2]=0;
    rotator[3][3]=1;
    }
    else if(axis=='z')
    {
    rotator[0][0]=cos((atan(1)*4)*angle/180);
    rotator[0][1]=-sin((atan(1)*4)*angle/180);
    rotator[0][2]=0;
    rotator[0][3]=0;
    rotator[1][0]=sin((atan(1)*4)*angle/180);
    rotator[1][1]=cos((atan(1)*4)*angle/180);
    rotator[1][2]=0;
    rotator[1][3]=0;
    rotator[2][0]=0;
    rotator[2][1]=0;
    rotator[2][2]=1;
    rotator[2][3]=0;
    rotator[3][0]=0;
    rotator[3][1]=0;
    rotator[3][2]=0;
    rotator[3][3]=1;
    }

    matrixMultiply(rotator,coordinates);
}

void drawSurface(float **surfaces)
{
    float r,g,b;

    for(int i=0;i<3;i++)
    {
        float furthest=0;
        float c[12];
        float I,J,K;
        int checkpoint=-1;

        for(int j=0;j<6;j++)
        {
            if(surfaces[j][12]==1)
            {
                if((surfaces[j][2]+surfaces[j][5]+surfaces[j][8]+surfaces[j][11])>furthest || furthest==0)
                {
                    if(checkpoint!=-1)
                        surfaces[checkpoint][12]=1;

                    checkpoint=j;
                    furthest=surfaces[j][2]+surfaces[j][5]+surfaces[j][8]+surfaces[j][11];

                    surfaces[j][12]=0;

                    c[0]=surfaces[j][0];
                    c[1]=surfaces[j][1];
                    c[2]=surfaces[j][3];
                    c[3]=surfaces[j][4];
                    c[4]=surfaces[j][6];
                    c[5]=surfaces[j][7];
                    c[6]=surfaces[j][9];
                    c[7]=surfaces[j][10];

                    c[8]=surfaces[j][2];
                    c[9]=surfaces[j][5];
                    c[10]=surfaces[j][8];
                    c[11]=surfaces[j][11];

                    r=surfaces[j][13];
                    g=surfaces[j][14];
                    b=surfaces[j][15];

                    I=(surfaces[j][0]+surfaces[j][3]+surfaces[j][6]+surfaces[j][9])/4;
                    J=(surfaces[j][1]+surfaces[j][4]+surfaces[j][7]+surfaces[j][10])/4;
                    K=(surfaces[j][2]+surfaces[j][5]+surfaces[j][8]+surfaces[j][11])/4;

                    float mod=sqrt(pow(I,2)+pow(J,2)+pow(K,2));

                    I/=mod;
                    J/=mod;
                    K/=mod;
                }

                fillSurface(c,r,g,b,I,J,K);
            }
        }
    }
}

void detectSurfaces(float **surfaces, float **Dcoordinates)
{
    int t=0;
    /*first surface color=red*///r=13 g=14 b=15
    surfaces[t][13]=def;
    surfaces[t][14]=0.0;
    surfaces[t][15]=0.0;
    surfaces[t][0]=Dcoordinates[0][0];
    surfaces[t][1]=Dcoordinates[1][0];
    surfaces[t][2]=Dcoordinates[2][0];
    surfaces[t][3]=Dcoordinates[0][1];
    surfaces[t][4]=Dcoordinates[1][1];
    surfaces[t][5]=Dcoordinates[2][1];
    surfaces[t][6]=Dcoordinates[0][2];
    surfaces[t][7]=Dcoordinates[1][2];
    surfaces[t][8]=Dcoordinates[2][2];
    surfaces[t][9]=Dcoordinates[0][3];
    surfaces[t][10]=Dcoordinates[1][3];
    surfaces[t][11]=Dcoordinates[2][3];
    t++;
    /*second surface color=blue*/
    surfaces[t][13]=0.0;
    surfaces[t][14]=0.0;
    surfaces[t][15]=def;
    surfaces[t][0]=Dcoordinates[0][0];
    surfaces[t][1]=Dcoordinates[1][0];
    surfaces[t][2]=Dcoordinates[2][0];
    surfaces[t][3]=Dcoordinates[0][1];
    surfaces[t][4]=Dcoordinates[1][1];
    surfaces[t][5]=Dcoordinates[2][1];
    surfaces[t][6]=Dcoordinates[0][5];
    surfaces[t][7]=Dcoordinates[1][5];
    surfaces[t][8]=Dcoordinates[2][5];
    surfaces[t][9]=Dcoordinates[0][4];
    surfaces[t][10]=Dcoordinates[1][4];
    surfaces[t][11]=Dcoordinates[2][4];
    t++;
    /*third surface color=yellow*/
    surfaces[t][13]=def;
    surfaces[t][14]=def;
    surfaces[t][15]=0.0;
    surfaces[t][0]=Dcoordinates[0][0];
    surfaces[t][1]=Dcoordinates[1][0];
    surfaces[t][2]=Dcoordinates[2][0];
    surfaces[t][3]=Dcoordinates[0][3];
    surfaces[t][4]=Dcoordinates[1][3];
    surfaces[t][5]=Dcoordinates[2][3];
    surfaces[t][6]=Dcoordinates[0][7];
    surfaces[t][7]=Dcoordinates[1][7];
    surfaces[t][8]=Dcoordinates[2][7];
    surfaces[t][9]=Dcoordinates[0][4];
    surfaces[t][10]=Dcoordinates[1][4];
    surfaces[t][11]=Dcoordinates[2][4];
    t++;
    /*fourth surface color=cyan*/
    surfaces[t][13]=0.0;
    surfaces[t][14]=def;
    surfaces[t][15]=def;
    surfaces[t][0]=Dcoordinates[0][2];
    surfaces[t][1]=Dcoordinates[1][2];
    surfaces[t][2]=Dcoordinates[2][2];
    surfaces[t][3]=Dcoordinates[0][3];
    surfaces[t][4]=Dcoordinates[1][3];
    surfaces[t][5]=Dcoordinates[2][3];
    surfaces[t][6]=Dcoordinates[0][7];
    surfaces[t][7]=Dcoordinates[1][7];
    surfaces[t][8]=Dcoordinates[2][7];
    surfaces[t][9]=Dcoordinates[0][6];
    surfaces[t][10]=Dcoordinates[1][6];
    surfaces[t][11]=Dcoordinates[2][6];
    t++;
    /*fifth surface color=green*/
    surfaces[t][13]=0.0;
    surfaces[t][14]=def;
    surfaces[t][15]=0.0;
    surfaces[t][0]=Dcoordinates[0][1];
    surfaces[t][1]=Dcoordinates[1][1];
    surfaces[t][2]=Dcoordinates[2][1];
    surfaces[t][3]=Dcoordinates[0][2];
    surfaces[t][4]=Dcoordinates[1][2];
    surfaces[t][5]=Dcoordinates[2][2];
    surfaces[t][6]=Dcoordinates[0][6];
    surfaces[t][7]=Dcoordinates[1][6];
    surfaces[t][8]=Dcoordinates[2][6];
    surfaces[t][9]=Dcoordinates[0][5];
    surfaces[t][10]=Dcoordinates[1][5];
    surfaces[t][11]=Dcoordinates[2][5];
    t++;
    /*sixth surface color=magenta*/
    surfaces[t][13]=def;
    surfaces[t][14]=0.0;
    surfaces[t][15]=def;
    surfaces[t][0]=Dcoordinates[0][4];
    surfaces[t][1]=Dcoordinates[1][4];
    surfaces[t][2]=Dcoordinates[2][4];
    surfaces[t][3]=Dcoordinates[0][5];
    surfaces[t][4]=Dcoordinates[1][5];
    surfaces[t][5]=Dcoordinates[2][5];
    surfaces[t][6]=Dcoordinates[0][6];
    surfaces[t][7]=Dcoordinates[1][6];
    surfaces[t][8]=Dcoordinates[2][6];
    surfaces[t][9]=Dcoordinates[0][7];
    surfaces[t][10]=Dcoordinates[1][7];
    surfaces[t][11]=Dcoordinates[2][7];
}

void detectVisibleSurfaces(float **surfaces)
{
    for(int i=0;i<6;i++)
    {
        int count=0;
        surfaces[i][12]=1;

        for(int j=0;j<6;j++)
        {
            if(i==j)
                continue;

            if((surfaces[i][2]+surfaces[i][5]+surfaces[i][8]+surfaces[i][11])>(surfaces[j][2]+surfaces[j][5]+surfaces[j][8]+surfaces[j][11]))
                count++;
        }

        if(count>=3)//at most 3 surfaces are visible
            surfaces[i][12]=0;
    }
}

void draw3DCube(float **Dcoordinates)
{
    float **surfaces;

    surfaces=new float *[6];//6 possible surfaces
    for(int i=0;i<6;i++)
    {
        surfaces[i]=new float[16];//x1,y1,z1,x2,y2,z2,x3,y3,z3,x4,y4,z4,t/f,r,g,b
        surfaces[i][12]=1;
    }
    detectSurfaces(surfaces,Dcoordinates);
    detectVisibleSurfaces(surfaces);
    drawSurface(surfaces);
}

static void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    setRotate('x');
    angle=temp2*temp;
    setRotate('y');
    angle=temp2+temp;
    setRotate('z');
    angle=temp-temp2;

    for(int i=0; i<4; i++)
    {
        for(int j=0; j<8; j++)
            Dcoordinates[i][j]=coordinates[i][j];
    }

    project();

    for(int i=0;i<8;i++)
    {
        Dcoordinates[2][i]=coordinates[2][i];//used for surface detection
    }

    /*drawing axes*/
    DrawLine(0,-240, 0, 240,1,1,1);
    DrawLine(-360,0, 360,0,1,1,1);
    /*drawing axes*/

    drawCircle(radi,X,Y);//light source

    draw3DCube(Dcoordinates);

    glutSwapBuffers();
}

void update(int value)
{
	glutPostRedisplay();

	glutTimerFunc(delay,update,value);
}

static void key(unsigned char key, int x, int y)
{
    if(key=='s')
        temp=0,temp2=0;
    if(key=='r')
    {
        temp=rand()%3*pow(-1,rand()%2+1);
        temp2=rand()%2*pow(-1,rand()%2+1);
        temp*=pow(-3,rand()%1);
        temp2*=pow(-2,rand()%1);
    }
    if(key=='n')
    {
        if(n<=100)
            n+=5;
    }
    if(key=='m')
    {
        if(n>=40)
            n-=5;
    }
    if(key=='f')
    {
        if(Z>=-200)
            Z-=10;
    }
    if(key=='b')
    {
        if(Z<=0)
            Z+=10;
    }
    glutPostRedisplay();
}

/* Program entry point */

void freeall()
{
    for(int i=0; i<4; i++)
    {
        delete [] rotator[i];
        delete [] projector[i];
        delete [] coordinates[i];
        delete [] Dcoordinates[i];
    }
    delete [] rotator;
    delete [] projector;
    delete [] coordinates;
    delete [] Dcoordinates;
}

void init()
{
    s=100;
    Zp=-2*s;
    Zc=-4*s;

    coordinates=new float *[4];
    Dcoordinates=new float *[4];
    rotator=new float *[4];
    projector=new float *[4];
    for(int i=0; i<4; i++)
    {
        coordinates[i]=new float[8];
        Dcoordinates[i]=new float[8];
        rotator[i]=new float[4];
        projector[i]=new float[4];
    }

    coordinates[0][0]=-s;
    coordinates[1][0]=-s;
    coordinates[2][0]=-s;
    coordinates[3][0]=1;
    coordinates[0][1]=s;
    coordinates[1][1]=-s;
    coordinates[2][1]=-s;
    coordinates[3][1]=1;
    coordinates[0][2]=s;
    coordinates[1][2]=-s;
    coordinates[2][2]=s;
    coordinates[3][2]=1;
    coordinates[0][3]=-s;
    coordinates[1][3]=-s;
    coordinates[2][3]=s;
    coordinates[3][3]=1;
    coordinates[0][4]=-s;
    coordinates[1][4]=s;
    coordinates[2][4]=-s;
    coordinates[3][4]=1;
    coordinates[0][5]=s;
    coordinates[1][5]=s;
    coordinates[2][5]=-s;
    coordinates[3][5]=1;
    coordinates[0][6]=s;
    coordinates[1][6]=s;
    coordinates[2][6]=s;
    coordinates[3][6]=1;
    coordinates[0][7]=-s;
    coordinates[1][7]=s;
    coordinates[2][7]=s;
    coordinates[3][7]=1;
}

void handleMouse(int button, int state, int x, int y)
{
    X=x-320;
    Y=-y+240;
}

int main(int argc, char *argv[])
{
    init();
    cout<<"r: randomize"<<endl;
    cout<<"s: stop"<<endl;
    cout<<"n: increase smoothness of cube"<<endl;
    cout<<"m: decrease smoothness of cube"<<endl;
    cout<<"b: move light source backward"<<endl;
    cout<<"f: move light source forward"<<endl;
    cout<<"Click anywhere to put the light source there."<<endl;
    glutInit(&argc, argv);
    glutInitWindowSize(640,480);
    glutInitWindowPosition(10,10);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

    glutCreateWindow("GLUT Shapes");
    glutMouseFunc(handleMouse);
    glutReshapeFunc(resize);
    glutTimerFunc(delay, update, 0);
    glutDisplayFunc(display);
    glutKeyboardFunc(key);

    atexit(freeall);//free all allocated memory

    glutMainLoop();

    return EXIT_SUCCESS;
}
