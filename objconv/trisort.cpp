#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include <ctype.h>
#include "trisort.h"
#include <math.h>
#ifdef __APPLE__
#define sqrtf sqrt
#endif
const int RIGHT_HANDED=1;
bool Face::Cross (Plane & result)const {
    double size =0;
    for (unsigned int i=2;(!size)&&i<this->p.size();i++) {
        Vector v1(0,0,0);
        v1.x = this->p[i].V.x-this->p[0].V.x;
        v1.y = this->p[i].V.y-this->p[0].V.y;
        v1.z = this->p[i].V.z-this->p[0].V.z;
        Vector v2(0,0,0);
        v2.x = this->p[1].V.x-this->p[0].V.x;
        v2.y = this->p[1].V.y-this->p[0].V.y;
        v2.z = this->p[1].V.z-this->p[0].V.z;
        result.a = v1.y * v2.z - v1.z * v2.y;
        result.b = v1.z * v2.x - v1.x * v2.z;
        result.c = v1.x * v2.y  - v1.y * v2.x;
        size = result.a*result.a+result.b*result.b+result.c*result.c;
    }
    if (size)
        size = ((double)1)/sqrtf (size);
    else
        return false;
    result.a *=RIGHT_HANDED*(size);
    result.b *=RIGHT_HANDED*(size);
    result.c *=RIGHT_HANDED*(size);
    return true;
}


Plane Face::planeEqu() const{
    if (p.empty())
        return Plane (1,0,0,0);
    Plane rez(1,1,1,0);
    if (!Cross (rez)) 
        rez=Plane(1,1,1,0);
    
    rez.d = ((rez.a*p[0].V.x)+(rez.b*p[0].V.y)+(rez.c*p[0].V.z));
    rez.d = -rez.d;
    return rez;
 }
bool Face::operator < (const Face &b) const{
    return false;
}

void Mesh::sort () {
std::sort (f.begin(),f.end());
}
void Index::write (FILE * fp) const{
    fprintf (fp,"%d/",p);
    if (n!=-1)
        fprintf (fp,"%d",n);
    fprintf (fp,"/");
    if (t!=-1)
        fprintf (fp,"%d",t);
    if (c!=-1) {
        fprintf (fp,"/%d",c);
    }
}
void Mesh::write(const char * filename)const {
    FILE * fp = fopen (filename,"w");
    if (fp) {
    unsigned int i;
    for (i=0;i<p.size();i++) {
        fprintf (fp,"p %lf %lf %lf\n",p[i].x,p[i].y,p[i].z);
    }
    for (i=0;i<n.size();i++) {
        fprintf (fp,"n %lf %lf %lf\n",n[i].x,n[i].y,n[i].z);
    }
    for (i=0;i<t.size();i++) {
        fprintf (fp,"p %lf %lf\n",t[i].x,t[i].y);        
    }
    for (i=0;i<c.size();i++) {
        fprintf (fp,"c %lf %lf %lf\n",c[i].x,c[i].y,c[i].z);        
    }
    for (i=0;i<f.size();i++) {
        fprintf (fp,"f");
        for (unsigned j=0;j<f[i].p.size();j++) {
            fprintf (fp," ");
            f[i].p[j].write (fp);
        }
        fprintf (fp,"\n");
    }
    fclose (fp);
    }
}
char * findspace (char *line) {
    while (iswhitespace(line[0])&&line[0])
        line++;
    if (!line[0])
        return NULL;
    while (!iswhitespace(line[0])&&line[0])
        line++;
    return line;
}
Index Mesh::processfacevertex(char * vertex)const {
    int a,b,c,d;a=0;b=c=d=-1;
    sscanf (vertex,"%d",&a);
    while (vertex[0] && (*vertex)!='/')
        vertex++;
    if (vertex[0]=='/') {
        vertex++;
        if (vertex[0]!='/')
            sscanf (vertex,"%d",&b);
        while (vertex[0] && (*vertex)!='/')
            vertex++;
        if (vertex[0]=='/') {
            vertex++;
            if (vertex[0]!='/')
                sscanf (vertex,"%d",&c);
            while (vertex[0] && (*vertex)!='/')
                vertex++;
            if (vertex[0]=='/') {
                vertex++;
                sscanf (vertex,"%d",&d);
            }
        }
    }
    Vector v(0,0,0);
    if (a<=(int)p.size()&&a>0)
        v=p[a-1];
    return Index (v,a,b,c,d);
}
Face Mesh::processface (char * line) const{
    Face f;
    while (line[0]) {
        char * lastspace = findspace (line);
        if (lastspace) {
            bool done= (lastspace[0]==0);
            (*lastspace)=0;
            f.p.push_back (processfacevertex (line));
            line =lastspace+1;
            if (done)
                break;
        }else {
            break;
        }
    }
    return f;
}
void Mesh::processline (char * line) {
    double a=0,b=0,c=0,d=0;
    switch (line[0]) {
        case 'p':
            sscanf (line,"p %lf %lf %lf",&a,&b,&c);
            p.push_back (Vector (a,b,c));
            break;
        case 'n':
            sscanf (line,"n %lf %lf %lf",&a,&b,&c);
            n.push_back (Vector (a,b,c));            
            break;
        case 't':
            sscanf (line,"t %lf %lf",&a,&b);
            t.push_back (Vector (a,b,0));
            break;
        case 'c':
            sscanf (line,"c %lf %lf %lf %lf",&a,&b,&c,&d);
            this->c.push_back (Vector (a,b,c));
            break;
        case 'f':
            f.push_back (processface(line+1));
            break;
        default:
            break;
    }
}

Mesh::Mesh (const char * filename) {
    FILE * fp = fopen ( filename,"r");
    char line [65536];
    line[65535]=0;
    while (fgets (line,65535,fp)) {
        processline(line);
    }
}

int main (int argc, char ** argv) {
    for (int i=1;i<argc;i++) {
        Mesh m(argv[i]);
        m.sort();
std::string rez (argv[i]);
        rez+=".out";
        m.write (rez.c_str());
    }
};