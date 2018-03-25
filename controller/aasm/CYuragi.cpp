#include <stdio.h>
#include <stdlib.h>
#include <string.h>//memcpy
#include <unistd.h>
#include <math.h>
#include <time.h>
#include <assert.h>

#include "CYuragi.h"

//vector utility
int zero(int dim, double* vector)
{
  for(int i=0; i<dim; i++){
    vector[i] = 0.0;
  }
  return 0;
}
double norm(int dim, double* vector)
{
  return sqrt(inner(dim, vector, vector));
}
double inner(int dim, double* vector1, double* vector2)
{
  double tmp=0.0;
  for(int i=0; i<dim; i++){
    tmp += vector1[i]*vector2[i];
  }
  return tmp;
}

//constructor
CYuragi::CYuragi(int dim_of_ASM_state, int number_of_attractors)
{
  Debug("constructor\n");

  //dimention of state
  this->dim_of_ASM_state = dim_of_ASM_state;
  //number of attractor
  this->number_of_attractors = number_of_attractors;

  //random number
  setRandSeed(0);

  //attractor
  gm = new GMixture(dim_of_ASM_state, number_of_attractors);
  attractor = gm->getCentroid();

  char* state_file = "./log/state.dat";
  if((state_fp = fopen(state_file, "w"))==NULL){
    Error("file open error (%s)...", state_file);
  }

  char* attractor_file = "./log/attractor.dat";
  if((attractor_fp = fopen(attractor_file, "w"))==NULL){
    Error("file open error (%s)...", attractor_file);
  }

  //variance of noise
  //  variance = 1.0;
  variance = 0.36;
  //  variance = 0.04;
  //time step
  delta_t = 0.01;
  //time constant
  tau = 0.1;

  //adaptive attractor
  adaptive = true;

  //previous state
  pre_ASM_state = new double[dim_of_ASM_state]; assert(pre_ASM_state);
  for(int i=0; i<dim_of_ASM_state; i++){
    //    pre_ASM_state[i] = (double)random()/(double)RAND_MAX*2.0-1.0;
    pre_ASM_state[i] = 0.0;
  }
  Debug("constructor\n");
}

//destructor
CYuragi::~CYuragi()
{
  Debug("destructor\n");
  delete [] pre_ASM_state;

  fclose(state_fp);
  fclose(attractor_fp);

  Debug("destructor\n");
}

double* CYuragi::nextASMState(double activity)
{
  Debug("nextASMState\n");
  double* xdot = xDot(activity);
  //eular method
  for(int i=0; i<dim_of_ASM_state; i++){
    pre_ASM_state[i] = pre_ASM_state[i] + delta_t*xdot[i];
    //    Error("xdot[%d] = %f\n", i, xdot[i]);
    if(fabs(pre_ASM_state[i]) > 1.0){
      pre_ASM_state[i] = pre_ASM_state[i]/fabs(pre_ASM_state[i]);
    }
    //    Print("state[%d] = %f\n", i, pre_ASM_state[i]);
  }

  if(adaptive){
    gm->update(activity, pre_ASM_state);
  }

  Debug("nextASMState\n");
  return pre_ASM_state;
}

double* CYuragi::nextASMStateByAattractor(int index)
{
  return attractor[index];
}

//set attractor
int CYuragi::setAttractors(double** attractors)
{
  Debug("setAttractors\n");
  gm->setCentroid(attractors);
  Debug("setAttractors\n");
  return 0;
}

//get attractor
double** CYuragi::getAttractors(void)
{
  return attractor;
}

//calculate Euclidean distance between attractor and state
int CYuragi::whichAttractor(double* Euclid_distance)
{
  double min = 0.0;
  int ind = 0;
  for(int i=0; i<number_of_attractors; i++){
    double sum = 0.0;
    for(int j=0; j<dim_of_ASM_state; j++){
      sum += pow(attractor[i][j]-pre_ASM_state[j], 2.0);
    }
    Euclid_distance[i] = sqrt(sum);
  }
  min = Euclid_distance[ind];
  for(int i=1; i<number_of_attractors; i++){
    if(min > Euclid_distance[i]){
      min = Euclid_distance[i];
      ind = i;
    }
  }
  return ind;
}

int CYuragi::adaptiveAttractor(bool on)
{
  adaptive = on;
  return 0;
}

bool CYuragi::isAdaptive(void)
{
  return adaptive;
}

int CYuragi::resetASMState(double uniform_state)
{
  for(int i=0; i<dim_of_ASM_state; i++){
    pre_ASM_state[i] = uniform_state;
  }
  return 0;
}

int CYuragi::resetASMState(double* ASM_state)
{
  if(ASM_state){
    memcpy(pre_ASM_state, ASM_state, sizeof(double)*dim_of_ASM_state);
  }else{
    for(int i=0; i<dim_of_ASM_state; i++){
      pre_ASM_state[i] = (double)random()/(double)RAND_MAX*2.0-1.0;
    }
  }
  return 0;
}

int CYuragi::saveASMState(double time)
{
  fprintf(state_fp, "%f ", time);
  for(int i=0; i<dim_of_ASM_state; i++){
    fprintf(state_fp, "%f ", pre_ASM_state[i]);
  }
  fprintf(state_fp, "\n");
  return 0;
}

int CYuragi::saveAtractors(double time)
{
  if(!gm->centroidUpdated()) return 0;

  fprintf(attractor_fp, "%f ", time);
  for(int i=0; i<number_of_attractors; i++){
    for(int j=0; j<dim_of_ASM_state; j++){
      fprintf(attractor_fp, "%f ", attractor[i][j]);
    }
  }
  fprintf(attractor_fp, "\n");
  return 0;
}

//calculate dynamics -> this must be overriden
double* CYuragi::updateDynamics(void)
{
  Error("This method must be overriden (updateDynamics)\n");
  return pre_ASM_state;
}
double* CYuragi::updateDynamics(double activity)
{
  Error("This method must be overriden (updateDynamics)\n");
  return pre_ASM_state;
}

double* CYuragi::xDot(double activity)
{
  Debug("\nxDot {");
  static double* xdot = new double[dim_of_ASM_state];

  Debug("activity = %f\n", activity);

  //dynamics (f(x))
  //  double* f_x = updateDynamics();
  double* f_x = updateDynamics(activity);

  double noise;
  for(int i=0; i<dim_of_ASM_state; i++){
    //x$B$N;~4VHyJ,$N7W;;(B
    noise = getNoise(0.0,variance);
    //    xdot[i] = (f_x[i]*activity + noise)/tau;
    xdot[i] = (f_x[i] + noise)/tau;
    //    Debug("f_x[%d] = %f\n", i, f_x[i]);
    //    Error("f_x[%d] = %f\n", i, f_x[i]);
    Debug("xdot[%d] = %f\n", i, xdot[i]);
  }
  Debug("} xDot\n");
  return xdot;
}

//normal distribution
double CYuragi::getNoise(double mean, double s2)
{
  //$B@55,J,I[$N$"$kCM(B
  double nom_dist;
  //$BJV$jCM(B
  double ret;
  while(1){
    //-1~1$B$^$G$N0lMMMp?t$rF@$k(B
    //$B$3$NCM$,%N%$%:$H$J$j$&$k(B
    ret = (double)rand()/(double)RAND_MAX*2.0 - 1.0;
    //$B0lMMMp?t$+$i@55,J,I[$NCM$rF@$k(B
    //$BJ?6Q$NCM$K6a$1$l$P6a$$$[$IBg$-$$CM(B
    //$BJ?6Q$NCM$+$i1s$1$l$P1s$$$[$I>.$5$$CM(B
    nom_dist = exp(-ret*ret/2.0)/sqrt(2.0*M_PI);
    //$B@55,J,I[$NCM$,$"$kMp?t$h$jBg$-$1$l$P:NMQ(B
    //$B$9$J$o$A@55,J,I[$NCM$,Bg$-$$$[$I:NMQ$5$l$k3NN($O9b$/$J$k(B
    if(nom_dist >= (double)rand()/(double)RAND_MAX){
      break;
    }
  }
  return ret*sqrt(s2)+mean;
}

int CYuragi::setRandSeed(unsigned int seed)
{
  //seed of random number
  char* file = "log/yuragi.log";
  FILE* fp;
  if((fp=fopen(file, "a"))==NULL){
    Error("file open error (%s)...\n", file);
    exit(EXIT_FAILURE);
  }
  unsigned t;
  if(!seed){
    t = (unsigned)time(NULL);
  }else{
    t = seed;
  }
  fprintf(fp, "seed of random number is %d\n", t);
  fclose(fp);
  srandom(t);
  return 0;
}
