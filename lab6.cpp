#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <queue>
#include <iomanip>
#include <algorithm>
#include <bits/stdc++.h>


using namespace std;



// Structure to represent a Process
struct Process {
    char name;
    int arrival_time;
    int service_time;
    int service_time_left;
    int priority; // Used only for the Aging algorithm
    int org_priority;
    int start_time= -1;          // Time when the process starts execution
    int finish_time = -1;     // Time when the process finishes execution
    int turnaround_time = -1;     // Turnaround time (completion_time - arrival_time)
    float normturn = -1;
    int time_inQueue = -1;        
    
    vector<string> process_state;         // " " -> didnt arrive yet  . -> ready  * ->running
    

    
};

class CompareFCFS {
public:
    bool operator()(Process a, Process b)
    {
        if (a.arrival_time > b.arrival_time) {
            return true;
        }
        
        return false;
    }
};

class CompareSPN {
private:
    const std::vector<Process>& processes; // Reference to the process collection

public:
    CompareSPN(const std::vector<Process>& processes) : processes(processes) {}
    bool operator()(int a, int b)
    {
        if (processes[a].service_time > processes[b].service_time) {
            return true;
        }
        
        return false;
    }
};

class CompareAging {
private:
    const std::vector<Process>& processes; // Reference to the process collection

public:
    CompareAging(const std::vector<Process>& processes) : processes(processes) {}
    bool operator()(int a, int b)
    {
        if (processes[a].priority < processes[b].priority) {
            return true;
        }
        else if(processes[a].priority == processes[b].priority)
          return processes[a].time_inQueue < processes[b].time_inQueue;
        
        
        return false;
    }
};

bool sort_Priority (const tuple<Process,int>&a,const tuple<Process,int>&b){
    
    return get<0>(a).priority >= get<0>(b).priority;
}
bool sort_arrival (const tuple<Process,int>&a,const tuple<Process,int>&b){
    
    return get<0>(a).arrival_time <= get<0>(b).arrival_time;
}

vector<bool> processor_state;

void display_trace(string policy, int instances, const vector<Process>& processes) {
    if(policy == "Aging" || policy == "FB-2i")
        cout << policy  << " ";
    else
        cout << policy << "  ";
        
    for (int i = 0; i <= instances; i++) {
        cout << i % 10 << " ";
    }
    cout << "\n------------------------------------------------\n";
    for (int i = 0; i < processes.size(); ++i) {
        cout << processes[i].name << "     ";
        for (int j = 0; j < processes[i].process_state.size(); ++j) {
            cout << "|" << processes[i].process_state[j];
        }
        cout << "| \n";
    }
    cout << "------------------------------------------------\n";
    cout << "\n";
    
}

void display_stats(string policy,vector<Process> processes) {
   cout << policy << "\n";

   //PROCESS
   //------------------------------------------------------------------------------
   cout << "Process    |";
   for (int i = 0; i < processes.size(); ++i) {
       cout << "  " << processes[i].name << "  |";
   }
   cout << "\n";
   //------------------------------------------------------------------------------
   //ARRIVAL
   cout << "Arrival    |";
   for (int i = 0; i < processes.size(); ++i) {
       cout << "  " << processes[i].arrival_time << "  |";
   }
   cout << "\n";
   //------------------------------------------------------------------------------
   //SERVICE
   cout << "Service    |";
   for (int i = 0; i < processes.size(); ++i) {
       cout << "  " << processes[i].service_time << "  |";
   }
   cout << " Mean|\n";
   //------------------------------------------------------------------------------
   //FINISH
   cout << "Finish     |";
   double finish_sum = 0;
   for (int i = 0; i < processes.size(); ++i) {
       if(processes[i].finish_time >9)
       cout <<" "  << fixed << setprecision(2)<< processes[i].finish_time << "  |";
       else
       cout <<"  "  << fixed << setprecision(2)<< processes[i].finish_time << "  |";
       //finish_sum += processes[i].finish_time;
   }
      cout<<"-----|\n";
  /* double Mean_finish = finish_sum / processes.size();
   if(Mean_finish >9)
      cout << fixed << setprecision(2) << Mean_finish << "|\n";
  else
      cout<<" " << fixed << setprecision(2) << Mean_finish << "|\n";
   */
   //------------------------------------------------------------------------------
   //TURNAROUND
   cout << "Turnaround |";
   double turnaround_sum = 0;
   for (int i = 0; i < processes.size(); ++i) {
       if(processes[i].turnaround_time > 9)
       cout << " " << fixed << setprecision(2)<< processes[i].turnaround_time << "  |";
       else
       cout << "  " << fixed << setprecision(2)<< processes[i].turnaround_time << "  |";
       turnaround_sum += processes[i].turnaround_time;
   }
   double mean_turn = turnaround_sum / processes.size();
   if(mean_turn > 9)
    cout << fixed << setprecision(2) << mean_turn << "|\n";
   else
     cout <<" " << fixed << setprecision(2) << mean_turn << "|\n";
   //------------------------------------------------------------------------------
   //NORMTURN
   cout << "NormTurn   |";
   double norm_turn_sum = 0;
   for (int i = 0; i < processes.size(); ++i) {
       double norm_turn = static_cast<double>(processes[i].turnaround_time) / processes[i].service_time;
      if(norm_turn>9)
       cout << fixed << setprecision(2) << norm_turn<<"|";
      else
       cout <<" " << fixed << setprecision(2) << norm_turn<<"|";
 
       norm_turn_sum += norm_turn;
   }
   double Mean_Norm = norm_turn_sum / processes.size();
   if(Mean_Norm>9)
   cout << fixed << setprecision(2) << norm_turn_sum / processes.size() << "|\n";

   else
   cout <<" "<< fixed << setprecision(2) << norm_turn_sum / processes.size() << "|\n";
   cout <<"\n";
}

vector<Process> FCFS(vector<Process> processes , int numProcesses,int instances)
{

  priority_queue<Process,vector<Process>,CompareFCFS> ready_queue; 
  for(int v = 0; v < numProcesses ; v++) // copy processes into priority queue
    {
      ready_queue.push(processes[v]);
    }
    
  
  vector<Process> P;
    
    int i; 
    int servicetime_left ;
    while(!ready_queue.empty())
      {
      
        Process current_process = ready_queue.top();
        ready_queue.pop();
        servicetime_left = current_process.service_time_left;
        for (i=current_process.arrival_time;i<instances;i++)
          {
            if(processor_state[i] == true)
              {
                current_process.process_state[i] = ".";
              }
            else
              {
                if( servicetime_left > 0 )
                {current_process.process_state[i] = "*";
                processor_state[i] = true;  
                servicetime_left--;
                if(servicetime_left == 0)
                  current_process.finish_time = i+1;
                }
                else
                {
                  
                  break;
                }
              }
          }
          
          current_process.turnaround_time = current_process.finish_time - current_process.arrival_time ;
          current_process.normturn = (float) current_process.turnaround_time / current_process.service_time;
          
          
            
        
        P.push_back(current_process);
      }
    
  
        return P;
  }
  
vector<Process> RR(vector<Process> processes , int numProcesses,int instances,int quantum)
  {
    priority_queue<Process,vector<Process>,CompareFCFS> initial_queue; 
  for(int v = 0; v < numProcesses ; v++) // copy processes into priority queue
    {
      initial_queue.push(processes[v]);
    }
    vector<Process> P;
    //vector<Process> altered_processes;
    for(int v = 0; v < numProcesses ; v++) // copy processes into priority queue
    {
      P.push_back(initial_queue.top());
      initial_queue.pop();
    }
    
    
    
    queue<int> ready_queue;
    
   
    int i =  0 ;
    // Add processes that have arrived to the ready queue
        for (int p = 0; p < numProcesses; p++) {
            if (P[p].arrival_time == i) {
                ready_queue.push(p);
                P[p].process_state[i] = "."; // Mark as ready
            }
        }
    while (i < instances) {
        
      if (!ready_queue.empty()) {
            int current_process_index = ready_queue.front();
            ready_queue.pop();
            
            for (int q = 0; q < quantum && P[current_process_index].service_time_left > 0 && i < instances; q++) {   
                P[current_process_index].service_time_left--;
                P[current_process_index].process_state[i] = "*"; // Mark as running
                for (int p = 0; p < numProcesses; p++) {
                    if (P[p].name != P[current_process_index].name &&  P[p].service_time_left >0 && i > P[p].arrival_time) {
                        
                        P[p].process_state[i] = "."; // Mark as ready
                    }
                    
                }
                i++;

                // Add any arriving processes to the ready queue
                for (int p = 0; p < numProcesses; p++) {
                    if (P[p].arrival_time == i) {
                        ready_queue.push(p);
                        P[p].process_state[i] = "."; // Mark as ready
                    }
                    
                }
            
    }
    
 
  
  if (P[current_process_index].service_time_left > 0) {
                ready_queue.push(current_process_index);
                P[current_process_index].process_state[i] = "."; // Mark as ready
            }
            else
            {
              P[current_process_index].finish_time = i;
              P[current_process_index].turnaround_time = P[current_process_index].finish_time - P[current_process_index].arrival_time;
               P[current_process_index].normturn = (float) P[current_process_index].turnaround_time / P[current_process_index].service_time;
            }
    }
    else {
            // If no process is running, increment the time
            i++;
            /*// Add any arriving processes to the ready queue
                for (int p = 0; p < numProcesses; p++) {
                    if (P[p].arrival_time == i) {
                        ready_queue.push(p);
                        P[p].process_state[i] = "."; // Mark as ready
                    }
                    
                }*/
        }
        
        
        
        
  }
    
    
        return P;  
    
        
      }

vector<Process> SPN(vector<Process> processes, int numProcesses, int instances){
  priority_queue<Process,vector<Process>,CompareFCFS> initial_queue; 
  for(int v = 0; v < numProcesses ; v++) // copy processes into priority queue
    {
      initial_queue.push(processes[v]);
    }
    vector<Process> P;
    //vector<Process> altered_processes;
    for(int v = 0; v < numProcesses ; v++) 
    {
      P.push_back(initial_queue.top());
      initial_queue.pop();
    }
    
    
     auto compare = CompareSPN(P); // Instantiate the comparator
    priority_queue<int, std::vector<int>, decltype(compare)> ready_queue(compare);
    
    int i =  0 ;
    
    // Add processes that have arrived to the ready queue
        for (int p = 0; p < numProcesses; p++) {
            if (P[p].arrival_time == i) {
                ready_queue.push(p);
                P[p].process_state[i] = "."; // Mark as ready
            }
        }
        
    for(i = 0 ; i < instances ;)
      {   
        if(!ready_queue.empty())
          { 
            int current_process_index = ready_queue.top();
            ready_queue.pop();
            
            
            while(P[current_process_index].service_time_left > 0 &&  i < instances) 
              {
                P[current_process_index].service_time_left--;
                
                P[current_process_index].process_state[i] = "*"; // Mark as running
                
                for (int p = 0; p < numProcesses; p++) {
                    if (P[p].name != P[current_process_index].name &&  P[p].service_time_left >0 && i > P[p].arrival_time) {
                        
                        P[p].process_state[i] = "."; // Mark as ready
                    }
                    
                }
                i++;
                // Add any arriving processes to the ready queue
                for (int p = 0; p < numProcesses; p++) {
                    if (P[p].arrival_time == i) {
                        ready_queue.push(p);
                        P[p].process_state[i] = "."; // Mark as ready
                    }
                    
                }
                
                
              }
              P[current_process_index].finish_time = i;
              P[current_process_index].turnaround_time = P[current_process_index].finish_time - P[current_process_index].arrival_time;
               P[current_process_index].normturn = (float) P[current_process_index].turnaround_time / P[current_process_index].service_time;
          }
      }
      
      return P; 
    
}

vector<Process> SRT(vector<Process> processes, int numProcesses, int instances) {
    priority_queue<Process, vector<Process>, CompareFCFS> initial_queue;
    for (int v = 0; v < numProcesses; v++) {
        initial_queue.push(processes[v]);
    }
    vector<Process> P;
    for (int v = 0; v < numProcesses; v++) {
        P.push_back(initial_queue.top());
        initial_queue.pop();
    }

    auto compare = [&P](int a, int b) {
        return P[a].service_time_left > P[b].service_time_left;
    }; 

    priority_queue<int, vector<int>, decltype(compare)> ready_queue(compare);

    int i = 0;

    // Add processes that have arrived to the ready queue
    for (int p = 0; p < numProcesses; p++) {
        if (P[p].arrival_time == i) {
            ready_queue.push(p);
            P[p].process_state[i] = "."; // Mark as ready
        }
    }//da our inittt el 3ady le el procss @0

    for (i = 0; i < instances;) {
        if (!ready_queue.empty()) {
            int current_process_index = ready_queue.top();
            ready_queue.pop();

            // Mark other processes as ready for this time unit
            for (int p = 0; p < numProcesses; p++) {
                if (P[p].name != P[current_process_index].name && P[p].service_time_left > 0 && i >= P[p].arrival_time) {
                    P[p].process_state[i] = "."; // Mark as ready
                } //el default bta3na b ready
            }

            // Run the current process for one time unit
            P[current_process_index].service_time_left--;
            P[current_process_index].process_state[i] = "*"; // Mark as running
            i++;      //running law geh ma3adha w hena INC lel i 3alatool 3ashan a-compare next

            // Add any newly arrived processes to the ready queue
            for (int p = 0; p < numProcesses; p++) {
                if (P[p].arrival_time == i) {
                    ready_queue.push(p);
                    P[p].process_state[i] = "."; // Mark as ready
                }
            } //new process dkhlt w el default bta3ha b ready
              //ha3ml el comparison bel process di wel current process ely m3aya
              // w ha3ml swap lw true else i++ 3adii

            // Preempt if a new process arrives with a shorter remaining time
            if (!ready_queue.empty() && P[ready_queue.top()].service_time_left < P[current_process_index].service_time_left) {
                ready_queue.push(current_process_index);
                continue; //el continue hena 3ashan m3mlsh process finish lel current process ely lesa mt3mlhash swap
                          //3ashan el continue di 3ala el for loop fa ht-skip ba2y el code w n-increment el i 3adii
            }

            // Finish the process if no remaining time
            if (P[current_process_index].service_time_left == 0) {
                P[current_process_index].finish_time = i;
                P[current_process_index].turnaround_time = P[current_process_index].finish_time - P[current_process_index].arrival_time;
                P[current_process_index].normturn = static_cast<float>(P[current_process_index].turnaround_time) / P[current_process_index].service_time;
            } else {
                ready_queue.push(current_process_index);
            }
        } else {
            // No process is ready; increment time
            i++;
        }
    }

    // Calculate mean turnaround time and mean normalized turnaround time
    double turnaround_sum = 0, normturn_sum = 0;
    for (const auto& process : P) {
        turnaround_sum += process.turnaround_time;
        normturn_sum += process.normturn;
    }
    double mean_turnaround = turnaround_sum / numProcesses;
    double mean_normturn = normturn_sum / numProcesses;

       return P;
}

vector<Process> HRRN(vector<Process> processes, int numProcesses, int instances) {
    priority_queue<Process, vector<Process>, CompareFCFS> initial_queue;
    for (int v = 0; v < numProcesses; v++) {
        initial_queue.push(processes[v]);
    }
    vector<Process> P;
    for (int v = 0; v < numProcesses; v++) {
        P.push_back(initial_queue.top());
        initial_queue.pop();
    }

    int i = 0;

    auto compare = [&](const int& a, const int& b) {
        float waiting_time_a = i - P[a].arrival_time;
        float response_ratio_a = (waiting_time_a + P[a].service_time) / static_cast<float>(P[a].service_time);
        float waiting_time_b = i - P[b].arrival_time;
        float response_ratio_b = (waiting_time_b + P[b].service_time) / static_cast<float>(P[b].service_time);
        return response_ratio_a < response_ratio_b; // Higher response ratio first
    };
    priority_queue<int, vector<int>, decltype(compare)> ready_queue(compare);

    // Add processes that have arrived to the ready queue
    for (int p = 0; p < numProcesses; p++) {
        if (P[p].arrival_time == i) {
            ready_queue.push(p);
            P[p].process_state[i] = "."; // Mark as ready
        }
    }

    for (i = 0; i < instances;) {
        if (!ready_queue.empty()) {
            int current_process_index = ready_queue.top();
            ready_queue.pop();

            while (P[current_process_index].service_time_left > 0 && i < instances) {
                P[current_process_index].service_time_left--;
                P[current_process_index].process_state[i] = "*"; // Mark as running

                for (int p = 0; p < numProcesses; p++) {
                    if (P[p].name != P[current_process_index].name && P[p].service_time_left > 0 && i > P[p].arrival_time) {
                        P[p].process_state[i] = "."; // Mark as ready
                    }
                }

                i++;

                // Add any arriving processes to the ready queue
                for (int p = 0; p < numProcesses; p++) {
                    if (P[p].arrival_time == i) {
                        ready_queue.push(p);
                        P[p].process_state[i] = "."; // Mark as ready
                    }
                }

                // Update the ready queue with the new response ratios
                vector<int> temp;
                while (!ready_queue.empty()) {
                    temp.push_back(ready_queue.top());
                    ready_queue.pop();
                }
                for (int t : temp) {
                    ready_queue.push(t);
                }
            }
            
              P[current_process_index].finish_time = i;
              P[current_process_index].turnaround_time = P[current_process_index].finish_time - P[current_process_index].arrival_time;
               P[current_process_index].normturn = (float) P[current_process_index].turnaround_time / P[current_process_index].service_time;
            
        }
    }

    return P;
}

vector<Process> Aging(vector<Process> processes, int numProcesses, int instances,int quantum)
{ 
  //cout<<"Quantum : "<< quantum <<endl;
  priority_queue<Process,vector<Process>,CompareFCFS> initial_queue; 
  for(int v = 0; v < numProcesses ; v++) // copy processes into priority queue
    {
      initial_queue.push(processes[v]);
      //cout << "Priority of Process "<<processes[v].name << "is  : "<<processes[v].priority << endl;
    }
    vector<Process> P;
    
    for(int v = 0; v < numProcesses ; v++) 
    { Process proc = initial_queue.top();
      P.push_back(proc);
      
      initial_queue.pop();
    }
    
    auto compare = CompareAging(P); // Instantiate the comparator
    priority_queue<int, std::vector<int>, decltype(compare)> ready_queue(compare);
    
    int i =  0 ;
   
    while(i<instances)
    {
        
        for (int p = 0; p < numProcesses; p++) {
                    if (P[p].arrival_time == i) {
                        ready_queue.push(p);
                        P[p].priority++;
                        P[p].process_state[i] = "."; // Mark as readyP[current_process_index].waiting_time=0;
                    }
                    
                }
   //increment all priorities in the ready queue 
   
    vector<int> temp;
    while(!ready_queue.empty())
      {
        int indx = ready_queue.top();
        ready_queue.pop();
                    //cout<< P[indx].name << " " <<  P[indx].priority << " ";
                    P[indx].priority++;
                    
                    temp.push_back(indx);
        
      }
      //cout << "\n";
      for(int t = 0 ; t < temp.size() ; t++)
        {
          ready_queue.push(temp[t]);
          
          
        } 
        
        
        if(!ready_queue.empty())
          {
            
             int current_process_index = ready_queue.top();
            ready_queue.pop();
            
        
            for (int q = 0; q < quantum && i < instances; q++) {   
                
                P[current_process_index].process_state[i] = "*"; // Mark as running
                P[current_process_index].time_inQueue=0; 
                for (int p = 0; p < numProcesses; p++) {
                    if (P[p].name != P[current_process_index].name  && i >= P[p].arrival_time) {
                        
                        P[p].process_state[i] = "."; // Mark as ready
                        P[p].time_inQueue++;
                        
                    }
                    
                }
               
           i++;   
           
    }  P[current_process_index].priority = P[current_process_index].org_priority;
    
    ready_queue.push(current_process_index);
    }     
          
    }
        
  return P;
}

vector<Process> FBi(vector<Process> processes, int numProcesses, int instances)
{ 
    priority_queue<Process,vector<Process>,CompareFCFS> initial_queue; 
  for(int v = 0; v < numProcesses ; v++) // copy processes into priority queue
    {
      initial_queue.push(processes[v]);
    }
    vector<Process> P;
    //vector<Process> altered_processes;
    for(int v = 0; v < numProcesses ; v++) // copy processes into priority queue
    {
      P.push_back(initial_queue.top());
      initial_queue.pop();
    }
    
    
    
    queue<int> ready_queue1;
    queue<int> ready_queue2;
    queue<int> ready_queue3;
    queue<int> ready_queue4;
    
    int i = 0;
    int current_process_index = 0;
    for (int p = 0; p < numProcesses; p++) {
                    if (P[p].arrival_time == i) {
                        ready_queue1.push(p);
                        P[p].process_state[i] = "."; // Mark as ready
                    }
                    
                }
    while(i<instances)
      {
        
          
          if(!ready_queue1.empty())
            {
              current_process_index = ready_queue1.front();
              ready_queue1.pop();
              
              P[current_process_index].process_state[i] = "*";
              P[current_process_index].service_time_left--;
              i++;
              for (int p = 0; p < numProcesses; p++) {
                    if (P[p].arrival_time == i && P[p].name!=P[current_process_index].name && i < instances) {
                        ready_queue1.push(p);
                        P[p].process_state[i] = "."; // Mark as ready
                    }
                    
                }
                //i--;
              if(P[current_process_index].service_time_left>0)
                { if(ready_queue1.empty() && ready_queue2.empty() && ready_queue3.empty() && ready_queue4.empty())
                  ready_queue1.push(current_process_index);
                  else
                  ready_queue2.push(current_process_index);
                }
              
            }
            else if(!ready_queue2.empty())
              {
              current_process_index = ready_queue2.front();
              ready_queue2.pop();
              
              P[current_process_index].process_state[i] = "*";
              P[current_process_index].service_time_left--;
              
              if(P[current_process_index].service_time_left>0)
                {
                  if(ready_queue1.empty() && ready_queue2.empty() && ready_queue3.empty() && ready_queue4.empty())
                  ready_queue2.push(current_process_index);
                  else
                  ready_queue3.push(current_process_index);
                }
                i++;
                // Add any arriving processes to the ready queue
                for (int p = 0; p < numProcesses; p++) {
                    if (P[p].arrival_time == i) {
                        ready_queue1.push(p);
                        P[p].process_state[i] = "."; // Mark as ready
                    }
                    
                }
              }
            else if(!ready_queue3.empty())
              {
              current_process_index = ready_queue3.front();
              ready_queue3.pop();
              
              P[current_process_index].process_state[i] = "*";
              P[current_process_index].service_time_left--;
              if(P[current_process_index].service_time_left>0)
                {
                  if(ready_queue1.empty() && ready_queue2.empty() && ready_queue3.empty() && ready_queue4.empty())
                  ready_queue3.push(current_process_index);
                  else
                  ready_queue4.push(current_process_index);
                }
                i++;
                // Add any arriving processes to the ready queue
                for (int p = 0; p < numProcesses; p++) {
                    if (P[p].arrival_time == i) {
                        ready_queue1.push(p);
                        P[p].process_state[i] = "."; // Mark as ready
                    }
                    
                }
              }
            else
              {
              current_process_index = ready_queue4.front();
              ready_queue4.pop();
              
              P[current_process_index].process_state[i] = "*";
              P[current_process_index].service_time_left--;
              if(P[current_process_index].service_time_left>0)
                {
                  ready_queue4.push(current_process_index);
                }
                i++;
                // Add any arriving processes to the ready queue
                for (int p = 0; p < numProcesses; p++) {
                    if (P[p].arrival_time == i) {
                        ready_queue1.push(p);
                        P[p].process_state[i] = "."; // Mark as ready
                    }
                    
                }
              }
              if(P[current_process_index].service_time_left==0)
              {
                P[current_process_index].finish_time = i;
              P[current_process_index].turnaround_time = P[current_process_index].finish_time - P[current_process_index].arrival_time;
               P[current_process_index].normturn = (float) P[current_process_index].turnaround_time / P[current_process_index].service_time;
              }
              
          for(int e=0 ; e<numProcesses ;e++)
            {
              if(P[e].arrival_time <= i-1 && P[e].service_time_left>0 && P[e].name!=P[current_process_index].name)
                P[e].process_state[i-1] = "."; // Mark as ready
                
            }
              
              
      }
      return P;
}

vector<Process> FB2i(vector<Process> processes, int numProcesses, int instances)
{ 
    priority_queue<Process,vector<Process>,CompareFCFS> initial_queue; 
  for(int v = 0; v < numProcesses ; v++) // copy processes into priority queue
    {
      initial_queue.push(processes[v]);
    }
    vector<Process> P;
    //vector<Process> altered_processes;
    for(int v = 0; v < numProcesses ; v++) // copy processes into priority queue
    {
      P.push_back(initial_queue.top());
      initial_queue.pop();
    }
    
    
    
    queue<int> ready_queue1;
    queue<int> ready_queue2;
    queue<int> ready_queue3;
    queue<int> ready_queue4;
    
    int i = 0;
    int current_process_index = 0;
    for (int p = 0; p < numProcesses; p++) {
                    if (P[p].arrival_time == i) {
                        ready_queue1.push(p);
                        P[p].process_state[i] = "."; // Mark as ready
                    }
                    
                }
    while(i<instances)
      {
        
          
          if(!ready_queue1.empty())
            {
              current_process_index = ready_queue1.front();
              ready_queue1.pop();
              
              P[current_process_index].process_state[i] = "*";
              P[current_process_index].service_time_left--;
              for (int p = 0; p < numProcesses; p++) {
                    if (P[p].name != P[current_process_index].name &&  P[p].service_time_left >0 && i > P[p].arrival_time) {
                        
                        P[p].process_state[i] = "."; // Mark as ready
                    }
                    
                }
              i++;
              for (int p = 0; p < numProcesses; p++) {
                    if (P[p].arrival_time == i && P[p].name!=P[current_process_index].name && i < instances) {
                        ready_queue1.push(p);
                        P[p].process_state[i] = "."; // Mark as ready
                    }
                    
                }
                //i--;
              if(P[current_process_index].service_time_left>0)
                { if(ready_queue1.empty() && ready_queue2.empty() && ready_queue3.empty() && ready_queue4.empty())
                  ready_queue1.push(current_process_index);
                  else
                  ready_queue2.push(current_process_index);
                }
              
            }
            else if(!ready_queue2.empty())
              {
              current_process_index = ready_queue2.front();
              ready_queue2.pop();
              for(int q=0;q<2 && i < instances && P[current_process_index].service_time_left>0; q++)
              {P[current_process_index].process_state[i] = "*";
              P[current_process_index].service_time_left--;
              for (int p = 0; p < numProcesses; p++) {
                    if (P[p].name != P[current_process_index].name &&  P[p].service_time_left >0 && i > P[p].arrival_time) {
                        
                        P[p].process_state[i] = "."; // Mark as ready
                    }
                    
                }
              i++;
              // Add any arriving processes to the ready queue
                for (int p = 0; p < numProcesses; p++) {
                    if (P[p].arrival_time == i) {
                        ready_queue1.push(p);
                        P[p].process_state[i] = "."; // Mark as ready
                    }
                    
                }
              }
              
              if(P[current_process_index].service_time_left>0)
                {
                  if(ready_queue1.empty() && ready_queue2.empty() && ready_queue3.empty() && ready_queue4.empty())
                  ready_queue2.push(current_process_index);
                  else
                  ready_queue3.push(current_process_index);
                }
                
             
              }
            else if(!ready_queue3.empty())
              {
              current_process_index = ready_queue3.front();
              ready_queue3.pop();
              for(int q=0;q<4 && i < instances && P[current_process_index].service_time_left>0; q++){
              P[current_process_index].process_state[i] = "*";
              P[current_process_index].service_time_left--;
              for (int p = 0; p < numProcesses; p++) {
                    if (P[p].name != P[current_process_index].name &&  P[p].service_time_left >0 && i > P[p].arrival_time) {
                        
                        P[p].process_state[i] = "."; // Mark as ready
                    }
                    
                }
              i++;
              // Add any arriving processes to the ready queue
                for (int p = 0; p < numProcesses; p++) {
                    if (P[p].arrival_time == i) {
                        ready_queue1.push(p);
                        P[p].process_state[i] = "."; // Mark as ready
                    }
                    
                }
              }
              if(P[current_process_index].service_time_left>0)
                { if(ready_queue1.empty() && ready_queue2.empty() && ready_queue3.empty() && ready_queue4.empty())
                  ready_queue3.push(current_process_index);
                  else
                  ready_queue4.push(current_process_index);
                }
                
            
              }
            else
              {
              current_process_index = ready_queue4.front();
              ready_queue4.pop();
              for(int q=0;q<8 && i < instances && P[current_process_index].service_time_left>0; q++){
              P[current_process_index].process_state[i] = "*";
              P[current_process_index].service_time_left--;
              for (int p = 0; p < numProcesses; p++) {
                    if (P[p].name != P[current_process_index].name &&  P[p].service_time_left >0 && i > P[p].arrival_time) {
                        
                        P[p].process_state[i] = "."; // Mark as ready
                    }
                    
                }
              i++;
              for (int p = 0; p < numProcesses; p++) {
                    if (P[p].arrival_time == i) {
                        ready_queue1.push(p);
                        P[p].process_state[i] = "."; // Mark as ready
                    }
                    
                }
              }
              if(P[current_process_index].service_time_left>0)
                {
                  ready_queue4.push(current_process_index);
                }
                
             
              }
              if(P[current_process_index].service_time_left==0)
              {
                P[current_process_index].finish_time = i;
              P[current_process_index].turnaround_time = P[current_process_index].finish_time - P[current_process_index].arrival_time;
               P[current_process_index].normturn = (float) P[current_process_index].turnaround_time / P[current_process_index].service_time;
              }
              
          
              
              
      }
      return P;
}

string policy_numtoname(string policy_num, int quantum)
      {
       if(policy_num == "1")
      return "FCFS";
      else if(policy_num == "2")
      return "RR-"+std::to_string(quantum);
      else if(policy_num == "3")
      return "SPN";
      else if(policy_num == "4")
      return "SRT";
      else if(policy_num == "5")
      return "HRRN";
      else if(policy_num == "6")
      return "FB-1";
      else if(policy_num == "7")
      return "FB-2i";
      else if(policy_num == "8")
      return "Aging";
      
      else
        exit(0) ;
      }

int main(){
string tr_st;
std::cin  >> tr_st;
cin.ignore();
//std::cout << tr_st << std::endl;

string policyLine;

    getline(cin, policyLine);
    //std::cout << policyLine << std::endl;

    stringstream policyStream(policyLine);
    vector<string> policies;
    //int quantum  = -1;
    vector<int> quantum ;
    string policy;
    
    while (getline(policyStream, policy, ',')) {
        if(policy.length()==3)
        {
          quantum.push_back(std::stoi(&policy[2]));
          policies.push_back(std::string(1, policy[0]));
         //cout << "Quantum : "<<quantum <<endl;
        }
        else{
        quantum.push_back(-1);
        policies.push_back(policy);
        }
    }
    
    /* for(int i = 0;i<policies.size();i++)
       {
         cout << "policy:" <<policies[i]  << " Quantum : " << quantum[i]<< endl;
        }*/
      
      
    int lastInstant;
    cin >> lastInstant;
    cin.ignore();
    //cout << "last instant: "<< lastInstant <<endl;
    
    int numProcesses;
    cin >> numProcesses;
    cin.ignore();
    //cout << "number of Processes: "<< numProcesses <<endl;
    
    
    vector<Process> processes;
    for (int i = 0; i < numProcesses; i++) {
      
        string Line;
        getline(cin, Line);
        stringstream processStream(Line);
        
        Process process;
      
        
        
        process.name = Line[0];
        
        process.arrival_time=std::stoi(&Line[2]);
        
        
        if(policies[0] == "8")
        {
          
          process.priority = std::stoi(&Line[4]);
          process.org_priority = process.priority;
          
        }
        else
        {
          
          process.service_time=std::stoi(&Line[4]);
          process.service_time_left = process.service_time;
          process.priority = -1 ;
        }
        
        processes.push_back(process);
    }
    
 /*   for(int i = 0;i<processes.size();i++)
      {
        
        if(policies[0] != "8")
        {
        cout << "Process: "<<processes[i].name << " arrival time : "<< processes[i].arrival_time << " service time :" << processes[i].service_time << endl;
        }
        else
        {
        cout << "Process: "<<processes[i].name << " arrival time : "<< processes[i].arrival_time << " priority :" << processes[i].priority << endl;
        }
      }*/
      
//intialize processer state
for(int i = 0 ; i < lastInstant ; i++)
{
  processor_state.push_back(false);
}

//intialize processes state
for(int p =0 ; p < numProcesses ; p++)
for(int i = 0 ; i < lastInstant ; i++)
{
  processes[p].process_state.push_back(" ");
}
//cout<< policies[0] <<"HAMADA\n";
//cout << "Type of policies[0]: " << typeid(policies[0]).name() << endl;



vector<Process> P;
string policy_name;
//cout << "policy size : "<< policies.size() <<endl;
for (int plcy = 0 ; plcy < policies.size();plcy++)
{int policyNUMBER = std::stoi(policies[plcy]);
switch (policyNUMBER)
{
case 1:
 P = FCFS(processes,numProcesses,lastInstant);
policy_name = policy_numtoname(policies[plcy],quantum[plcy]) ;
if(tr_st == "trace")
   display_trace(policy_name,lastInstant,P);
else if(tr_st == "stats")
   display_stats(policy_name,P);
   //cout<<"AHMEDDDDD\n";
  break;
case 2:
 P= RR(processes,numProcesses,lastInstant,quantum[plcy]);
 policy_name = policy_numtoname(policies[plcy],quantum[plcy]) ;
  if(tr_st == "trace")
     display_trace(policy_name,lastInstant,P);
 else if(tr_st == "stats")
     display_stats(policy_name,P);
  break;
case 3:
  P = SPN(processes, numProcesses,lastInstant);
   policy_name = policy_numtoname(policies[plcy],quantum[plcy]) ;
  if(tr_st == "trace")
     display_trace(policy_name + " ",lastInstant,P);
 else if(tr_st == "stats")
     display_stats(policy_name,P);
  break;
case 4:
 P = SRT(processes, numProcesses,lastInstant);
   policy_name = policy_numtoname(policies[plcy],quantum[plcy]) ;
  if(tr_st == "trace")
     display_trace(policy_name + " ",lastInstant,P);
 else if(tr_st == "stats")
     display_stats(policy_name,P);
  break;
case 5:
 P = HRRN(processes, numProcesses,lastInstant);
   policy_name = policy_numtoname(policies[plcy],quantum[plcy]) ;
  if(tr_st == "trace")
     display_trace(policy_name,lastInstant,P);
 else if(tr_st == "stats")
     display_stats(policy_name,P);
  break;  
case 6:
  P = FBi(processes, numProcesses,lastInstant);
   policy_name = policy_numtoname(policies[plcy],quantum[plcy]) ;
  if(tr_st == "trace")
     display_trace(policy_name,lastInstant,P);
 else if(tr_st == "stats")
     display_stats(policy_name,P);
  
  break;
case 7:
  P = FB2i(processes, numProcesses,lastInstant);
   policy_name = policy_numtoname(policies[plcy],quantum[plcy]) ;
  if(tr_st == "trace")
     display_trace(policy_name,lastInstant,P);
 else if(tr_st == "stats")
     display_stats(policy_name,P);
  break;      
case 8:
P= Aging(processes,numProcesses,lastInstant,quantum[plcy]);
 policy_name = policy_numtoname(policies[plcy],quantum[plcy]) ;
  if(tr_st == "trace")
     display_trace(policy_name,lastInstant,P);
 else if(tr_st == "stats")
     display_stats(policy_name,P);
  break;
default:
  break;
}
}





return 0;
}
