#include <stdio.h>
#include <math.h>

#include "MSActivity.h"

MSActivity::MSActivity()
  : CActivity()
{
  Debug("\nconstructor {");
  Debug("} constructor\n");
}

MSActivity::~MSActivity(void)
{
  Debug("\ndestructor {");
  Debug("} destructor\n");
}

//calclate activity
double MSActivity::calcActivity(double* robotic_state)
{
  Debug("\ncalcActivity {");
  static double A=0.0;
  static double B=0.0;
  static double alpha_bar=0.0;
  double gamma=0.9999;

  //alpha$B$N7W;;(B
  double alpha = calcAlpha(robotic_state);

  //current activity
//   double error = alpha - alpha_bar;
//   double curr_a = 100.0*error/(fabs(error)+1.0);

  double curr_a = 100*(alpha - alpha_bar) + 25.0*calcBeta(robotic_state);

  //  Error("alpha = %f\n", alpha - alpha_bar);
  //  Error("beta  = %f\n", calcBeta(robotic_state));

  //  fprintf(stderr, "alpha = %f\n", alpha);
  //  fprintf(stderr, "error = %f\n", error);
  //  fprintf(stderr, "acti = %f\n", curr_a);
  //  fprintf(stderr, "alpha_bar = %f\n", alpha_bar);

  A = gamma*alpha + gamma*A;
  B = gamma + gamma*B;
  alpha_bar = A / B;

  Debug("} calcActivity\n");
  return curr_a;
}

double MSActivity::calcAlpha(double* robotic_state)
{
  Debug("\ncalcAlpha{");
  //inverse of distance between goal and position of hand
  if(!error_value){
    return 100.0;
  }
  return 1.0/error_value;
}

double MSActivity::calcBeta(double* robotic_state)
{
  Debug("\ncalcBeta {");
  //inverse of distance between goal and position of hand
  //  double s2 = 0.64;
  double s2 = 1.0;
  //  return 1.0/sqrt(2*M_PI*s2)*exp(-error_value*error_value*0.5/s2);
  return exp(-error_value*error_value*0.5/s2);
}

//$B&A$r7W;;$9$k(B
// double Activity::calcAlpha(double* value)
// {
//   //$BL\I8CM$H!$0l$DA0$H8=:_$N%;%s%5$N5wN%$N:9$r$H$k(B
//   double sum1 = 0.0;
//   double sum2 = 0.0;
//   for(int i=0; i<dim; i++){
//     sum1 += pow(tar[i]-pre_value[i], 2);
//     sum2 += pow(tar[i]-value[i], 2);
//   }
//   double ret = sqrt(sum1) - sqrt(sum2);
//   //  double ret = 0.1/sum2 + sqrt(sum1) - sqrt(sum2);
//   //  double ret = exp(sqrt(sum1)-sqrt(sum2));

//   //  fprintf(stderr, "alpha = %f\n", ret);

//   //  return sign(ret);
//   return ret;
// }

// double Activity::calcAct(double* value)
// {


//   //gain of alpha
//   alphagain = 200.0;
//   //previous a
//   pre_a = 0.0;


//   //time step
//   delta_t = 0.001;
//   //time contant
//   this->tau = 0.1;


//   //$B8=:_$N(Ba
//   double curr_a = 0;
//   //alpha$B$N7W;;(B
//   double alpha = calcAlpha(value);
//   double f_alpha = 1.5*tanh(alphagain*alpha);
//   //  double f_alpha = sign(alpha);
//   //  double f_alpha = tanh(2.0*(alphagain*alpha-0.5));
//   //  double f_alpha = tanh(2.0*(alpha-0.5));

//   //runge kutta method
//   double k1 = (-pre_a + f_alpha)/tau;
//   double k2 = (-(pre_a+k1*delta_t/2.0) + f_alpha)/tau;
//   double k3 = (-(pre_a+k2*delta_t/2.0) + f_alpha)/tau;
//   double k4 = (-(pre_a+k3*delta_t) + f_alpha)/tau;

//   curr_a = pre_a + delta_t/6.0*(k1+2.0*k2+2.0*k3+k4);

//   if(curr_a>1.0){
//     curr_a = 1.0;
//   }

//   //activity$B$NJ]B8(B
//   pre_a = curr_a;

//   return curr_a;
// }
