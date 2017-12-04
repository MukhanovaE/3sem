/* Рассчитать определённый интеграл какой-нибудь простой, интегрируемой аналитически на некотором промежутке функции f(x) методом Монте-Карло:
расчёт проводить параллельно в n-потоков некоторой программы А, генерируя в каждом из них N/n точек равномерно как по интервалу, так и по области значений.
Собирать и обрабатывать результаты в программе Б, читающей результаты из разделяемой памяти. [В случае использования независимых ячеек разделяемой памяти, 
сохранение результатов можно обеспечить без синхронизации, в случае разделяемой ячейки в разделяемой памяти - синхронизация необходима. Реализация способа хранения
влияет на результаты эксперимента 2 а,б*) (см. ниже).

Задачи, поставленные перед Вами как исследователем:
1) Оценить прирост производительности в связи с использованием n' потоков вместо 1, где n' - число физических ядер Вашей машины. 
Узнать число ядер-например, 'cat /cpu/procinfo'
2) Провести серию измерений, в которой, увеличивая число n' :
  а) пронаблюдать стагнацию роста производительности при увеличении числа потоков
  б*) определить, при каких n' (насколько больших) накладные расходы от использования многопоточности [и синхронизации, если она имела место]
  превосходят преимущества их использования.
*/

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

double intergalMC(long NTry) ;

int main() 
{
    int fd[2], result, proc=0;
    const int N = 4; //number of processes
    pid_t child[N];
    double childRes[N];
    const long Ntry=10000000;  
    size_t size;
    double arr[2];
    double arr1[2]={1.,2.};
    char resstring[14];
    int parentPID = getpid() ; 
    
    
    if(pipe(fd) < 0)
    {
        printf("1: Can not create pipe\n");
        exit(-1);
    }
    
    for (proc = 0; proc < N; proc++)
    {
       if(getpid()==parentPID) 
        result = fork();
        child[proc]=result;
     }
     
     //Calculate integral
     double intResult=intergalMC(Ntry);
     
     //send result to parent
     if(getpid()!=parentPID){
       close(fd[0]);
       size = write(fd[1], &intResult, sizeof(double));
       if (size < 0) 
       {
         printf("Can\'t write\n");
         exit(-1);
       }
       close (fd[1]);
       exit(-1);
     }
     
     //parent
     double total=0.;
     //collect from children 
     if(getpid()==parentPID){
       for (proc = 0; proc < N; proc++){
          waitpid( child[proc], &result,0); 
          close(fd[1]);
          size = read(fd[0], childRes, sizeof(double));
          total+=childRes[0];
          if (size !=sizeof(double))
          {
                        printf("Can\'t read\n");
                        exit(-1);
          }
       }
     }
     total/=N*Ntry;
     printf("Integral=%e \n",total) ;
     
  return 0;
}

double intergalMC(long Ntry)
{
  //calculate Monte-Carlo integral  
  double xMin=0.;
  double xMax=2.;
  double yMin=0.;
  double yMax=2.;
  double res=0.;  
  for(long i=0; i<Ntry; i++){
    double x=xMin+(xMax-xMin)*rand()/RAND_MAX  ;
    double y=yMin+(yMax-yMin)*rand()/RAND_MAX  ;
  
    double f = x*y;
    res+=f ;
  }
  return res ;
}
