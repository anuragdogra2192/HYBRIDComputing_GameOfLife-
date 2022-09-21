#include <iostream>
#include <cmath>
#include <stdlib.h>
#include <ctime>
#include <cstdlib>
#include <iomanip>
#include <string.h>
#include <mpi.h>
#include "omp.h"
using namespace std;

class gameoflife
{
        private:

        int nx,ny,t,start,end;
        // nx and ny is the total no. of cells along x and y axis respectively
        int rank,size,err,n;
        int **create,**cell,**tempcell;
        int i,j,flag;

        public:

        void creator(int,int,int,int);
        void divider();
        void survivalofthedead();
        void update();

};

void gameoflife::creator(int r,int s,int nrow,int ncol)
{//randomly creating the initial model
            rank = r;
            size = s;
            nx = nrow;
            ny = ncol;
                srand(time(NULL));
                //dynamic memory intialisation
                create = new int*[nx];
                for (i=0;i<nx;i++)
                create[i] = new int[ny];

                for(i=0;i<nx;i++)
                {
                   for(j=0;j<ny;j++)
                   {
                           create[i][j]=rand() % 2;
                   }
                }
}
void gameoflife::divider()
{
   t=0;
   t=nx/size;
   start = 0;
   end = 0;
   cell = new int*[t+2];
   for (i=0;i<t+2;i++)
   cell[i] = new int[ny];

   tempcell = new int*[t+2];
   for (i=0;i<t+2;i++)
   tempcell[i] = new int[ny];

   for(i=0;i<t+2;i++)
   {
     for(j=0;j<ny;j++)
      {
        cell[i][j]= 0;
      }
   }
   for(i=0;i<t+2;i++)
   {
     for(j=0;j<ny;j++)
      {
        tempcell[i][j]= 0;
      }
   }

    start=rank*t;
    end=((rank+1)*t)-1;
    for(i=start;i<=end;i++)
     {
        for(j=0;j<ny;j++)
        {
          cell[i-(rank*t)+1][j]=create[i][j];
        }
     }

}

void gameoflife::survivalofthedead()
{
   //cout<<"GAME of Life Begins "<<endl;
   MPI_Status status;
   MPI_Request req;
   int check;
   int ny1;
   int l,m;
   int **cell1;
     ny1=ny;
     cell1 = new int*[t+2];
     for (i=0;i<t+2;i++)
     cell1[i] = new int[ny];

     for(i=0;i<t+2;i++)
     {
        for(j=0;j<ny;j++)
        {
         cell[i][j]= 0;
}
     }

    if(((rank%2)==0))
      {
         if(rank!=0)
         {
           MPI_Send(&cell[1][0],ny,MPI_INT,rank-1,200+rank,MPI_COMM_WORLD);
           MPI_Recv(&cell[0][0],ny,MPI_INT,rank-1,100+rank,MPI_COMM_WORLD,&status);
         }
         if(rank!=size-1)
         {
           MPI_Send(&cell[t][0],ny,MPI_INT,rank+1,150+rank,MPI_COMM_WORLD);
           MPI_Recv(&cell[t+1][0],ny,MPI_INT,rank+1,175+rank,MPI_COMM_WORLD,&status);
         }
     }
     else if((rank%2)!=0)
      {
         if(rank!=(size-1))
         {
           MPI_Send(&cell[t][0],ny,MPI_INT,rank+1,100+(rank+1),MPI_COMM_WORLD);
           MPI_Recv(&cell[t+1][0],ny,MPI_INT,rank+1,200+(rank+1),MPI_COMM_WORLD,&status);
         }
           MPI_Send(&cell[1][0],ny,MPI_INT,rank-1,175+(rank-1),MPI_COMM_WORLD);
           MPI_Recv(&cell[0][0],ny,MPI_INT,rank-1,150+(rank-1),MPI_COMM_WORLD,&status);

      }
     else if(rank==0)
     {
        MPI_Send(&cell[1][0],ny,MPI_INT,size-1,5+(size-1),MPI_COMM_WORLD);
        MPI_Recv(&cell[0][0],ny,MPI_INT,size-1,7+(size-1),MPI_COMM_WORLD,&status);
     }
     else if(rank==size-1)
     {
       MPI_Send(&cell[t][0],ny,MPI_INT,0,7+(size-1),MPI_COMM_WORLD);
       MPI_Recv(&cell[t+1][0],ny,MPI_INT,0,5+(size-1),MPI_COMM_WORLD,&status);
     }
  MPI_Barrier(MPI_COMM_WORLD);

  for(i=0;i<t+2;i++)
   {
     for(j=0;j<ny;j++)
      {
        cell1[i][j]= cell[i][j];
      }
   }

 // now neighbours calculation
  #pragma omp parallel for private(l,m) shared(ny1,cell1) 
   for(l=1;l<t+1;l++)
    {
     for(m=0;m<ny1;m++)
     {
       if(m==0)
       {  //left column
         flag=0;
          flag=flag+cell1[l][m+1]+cell1[l-1][m]+cell1[l-1][m+1]+cell1[l+1][m]+cell1[l+1][m+1];
          flag=flag+cell1[l-1][ny1-1]+cell1[l][ny1-1]+cell1[l+1][ny1-1];
       }
       else if(j=ny-1)
       { //right column
         flag=0;
         flag=flag+cell1[l][m-1]+cell1[l-1][m-1]+cell1[l-1][m]+cell1[l+1][m-1]+cell1[l+1][m];
         flag=flag+cell1[l-1][0]+cell1[l][0]+cell1[l+1][0];
       }
       else
       {
         flag = 0;
         // FIRST neighbours addition
         flag = flag + cell1[l-1][m]+cell1[l+1][m]+cell1[l][m-1]+cell1[l][m+1];
         //SECOND neighbours addition
         flag = flag + cell1[l-1][m-1]+cell1[l-1][m+1]+cell1[l+1][m-1]+cell1[l+1][m+1];
       }
       // now cells survival of the dead begins
       // New cell born  
       if(flag == 3)
       {
         tempcell[l][m] = 1;
       }
       //cell survive
       if(flag == 2)
       {
         tempcell[l][m] = cell1[l][m];
       }
       //cell dead
       if(flag <= 1 || flag > 3)
       {
         tempcell[l][m] = 0;
       }
     }
   }



}
void gameoflife::update()
{
   for(i=0;i<t+2;i++)
   {
     for(j=0;j<ny;j++)
      {
        cell[i][j]= tempcell[i][j];
      }
   }
   for(i=0;i<t+2;i++)
   {
     for(j=0;j<ny;j++)
      {
        tempcell[i][j]= 0;
      }
   }
}

int main(int argc,char **argv)
{
        int required=1,provided;
        MPI_Init_thread(&argc,&argv,required,&provided);
        int a,n,nx,ny;
        int rank,size;
         n=1000;
         nx=240;
         ny=240;
        gameoflife g;
        MPI_Comm_size(MPI_COMM_WORLD,&size);
        MPI_Comm_rank(MPI_COMM_WORLD,&rank);

        g.creator(rank,size,nx,ny);
        g.divider();

        for(a=0;a<n;a++)
        {
                g.survivalofthedead();
                g.update();
        }
        MPI_Finalize();
        return 0;
}

                            


