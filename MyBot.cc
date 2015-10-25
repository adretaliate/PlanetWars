#include <iostream>
#include <bits/stdc++.h>
#include "PlanetWars.h"

using namespace std;

vector<Planet> myPlanets; 
vector<Planet> enemyPlanets; 
vector<Planet> neutralPlanets; 
vector<Fleet> myFleets;
vector<Fleet> fleets;
int max_distance,max_growth,min_growth;
// ofstream a;

// The DoTurn function is where your code goes. The PlanetWars object contains
// the state of the game, including information about all planets and fleets
// that currently exist. Inside this function, you issue orders using the
// pw.IssueOrder() function. For example, to send 10 ships from planet 3 to
// planet 8, you would say pw.IssueOrder(3, 8, 10).
//
// There is already a basic strategy in place here. You can use it as a
// starting point, or you can throw it out entirely and replace it with your
// own. Check out the tutorials and articles on the contest website at
// http://www.ai-contest.com/resources.

bool compare_turns(const Fleet& p, const Fleet& q) {
  if(p.TurnsRemaining() > q.TurnsRemaining()) return false;
  return true;
}

int Distance(const Planet& source_planet, const Planet& destination_planet) {
  double dx = source_planet.X() - destination_planet.X();
  double dy = source_planet.Y() - destination_planet.Y();
  return (int)ceil(sqrt(dx * dx + dy * dy));
}

struct compare_donors
{
  vector<Planet> enemy;

  compare_donors(Planet p){

      enemy.push_back(p);
    }

  bool operator()(Planet p, Planet q)
  {
      if(Distance(p,enemy[0])<Distance(q,enemy[0]))return true;
      else return false;
  }

};

int finalShips(const Planet& p)
{
  // ofstream f;
  // f.open("finalships.txt");
  // f<<"fleets size "<<fleets.size()<<" planet num_ships "<<p.NumShips()<<endl;
  vector<Fleet> fleet = fleets;
  //  for (int i = 0; i < fleet.size(); ++i)
  // {
  //   f<<"balh " << fleet[i].NumShips() << endl;
  // }
  // sort(fleet.begin(), fleet.end(), compare_turns);
  for(int i=0;i<fleet.size();i++){
    for(int j=i+1;j<fleet.size();j++){
      if(fleet[i].TurnsRemaining()>fleet[j].TurnsRemaining()){
        Fleet temp=fleet[i];
        fleet[i]=fleet[j];
        fleet[j]=temp;
      }
    }
  }
  // for (int i = 0; i < fleet.size(); ++i)
  // {
  //   f<<"fleet num_ships " << fleet[i].NumShips() << endl;
  // }
  int owner = p.Owner();
  int num_ships = p.NumShips();
  if(owner!=1)num_ships=-num_ships;
  int turns=0;
  // f<<"NumShips initial "<<num_ships<<endl;
  for(int i=0;i<fleet.size();i++)
  {
    int temp=fleet[i].TurnsRemaining()-turns;
    // f<<"fleet directed to: "<< fleet[i].DestinationPlanet()<<" enemy: "<<p.PlanetID()<<endl;
    if(fleet[i].DestinationPlanet()==p.PlanetID())
    {
        // f<<"NumShips "<<num_ships<<endl;
        // f<<"fleets "<<fleet[i].NumShips()<<endl;
        if(fleet[i].Owner()==1)
        {
            if(owner==0)
            {
                num_ships += fleet[i].NumShips();
                if(num_ships>0)
                {
                    owner=1; 
                }
            }

            else if(owner==1)
            {
                num_ships+=fleet[i].NumShips() + temp*p.GrowthRate();
            }

            else if(owner==2)
            {
                num_ships=num_ships-temp*p.GrowthRate()+fleet[i].NumShips();
                if(num_ships>0)owner=1;
            }
        }

        else
        {
            if(owner==0)
            {
                num_ships = num_ships + fleet[i].NumShips();
                if(num_ships>0)
                {
                    owner=2;
                    num_ships = -num_ships;
                }
            }

            else if(owner==1)
            {
                num_ships= num_ships - fleet[i].NumShips() + temp*p.GrowthRate();
                if(num_ships<0)owner=2;
            }

            else if(owner==2)
            {
                num_ships = num_ships - temp*p.GrowthRate() - fleet[i].NumShips();

            }
        }

        turns += temp;
    }
  }

  return num_ships;

}

int attack_heuristic(const Planet& p)
{
  int w1 = 10, w2 = -2, w3 = -2; int count_surrounding=0; 
  int heuristic=0; 
  
  for(int i=0;i<enemyPlanets.size();i++)
  {
      if(Distance(p,enemyPlanets[i]) <= 0.25*max_distance && p.PlanetID()!=enemyPlanets[i].PlanetID())
      {
          count_surrounding++;
          heuristic = heuristic - (w1*enemyPlanets[i].GrowthRate() - w2*enemyPlanets[i].NumShips() + w3*Distance(p,enemyPlanets[i])); 
      }
  }


  for(int i=0;i<myPlanets.size();i++)
  {
      if(Distance(p,myPlanets[i]) <= 0.25*max_distance && p.PlanetID()!=myPlanets[i].PlanetID())
      {
          count_surrounding++;
          heuristic = heuristic + (w1*myPlanets[i].GrowthRate() - w2*myPlanets[i].NumShips() + w3*Distance(p,myPlanets[i])); 
      }
  }


  heuristic += (count_surrounding+1)*(w1*p.GrowthRate()+ w2*p.NumShips());
  // a<<p.NumShips()<<endl;
  return heuristic;
}

int defence_heuristic(const Planet& p)
{
  int w1 = 10, w2 = -2, w3 = -2,w4=2; int count_surrounding=0; 
  int heuristic=0; 
  
  for(int i=0;i<enemyPlanets.size();i++)
  {
      if(Distance(p,enemyPlanets[i]) <= 0.25*max_distance && p.PlanetID()!=enemyPlanets[i].PlanetID())
      {
          count_surrounding++;
          heuristic = heuristic - (w1*enemyPlanets[i].GrowthRate() - w2*enemyPlanets[i].NumShips() + w3*Distance(p,enemyPlanets[i])); 
      }
  }


  for(int i=0;i<myPlanets.size();i++)
  {
      if(Distance(p,myPlanets[i]) <= 0.25*max_distance && p.PlanetID()!=myPlanets[i].PlanetID())
      {
          count_surrounding++;
          heuristic = heuristic + (w1*myPlanets[i].GrowthRate() - w2*myPlanets[i].NumShips() + w3*Distance(p,myPlanets[i])); 
      }
  }


  heuristic += (count_surrounding+1)*(w1*p.GrowthRate()+ w4*finalShips(p));
  
  return heuristic;
}

int neutral_heuristic(const Planet& p)
{
  int w1 = 10, w2 = -2, w3 = -2,w4=-2, w5=3; int count_surrounding=0; 
  int heuristic=0; 
  
  for(int i=0;i<enemyPlanets.size();i++)
  {
      if(Distance(p,enemyPlanets[i]) <= 0.25*max_distance && p.PlanetID()!=enemyPlanets[i].PlanetID())
      {
          count_surrounding++;
          heuristic = heuristic - (w1*enemyPlanets[i].GrowthRate() - w2*enemyPlanets[i].NumShips() + w3*Distance(p,enemyPlanets[i])); 
      }
  }


  for(int i=0;i<myPlanets.size();i++)
  {
      if(Distance(p,myPlanets[i]) <= 0.25*max_distance && p.PlanetID()!=myPlanets[i].PlanetID())
      {
          count_surrounding++;
          heuristic = heuristic + (w1*myPlanets[i].GrowthRate() - w2*myPlanets[i].NumShips() + w3*Distance(p,myPlanets[i])); 
      }
  }


  heuristic += (count_surrounding+1)*(w5*p.GrowthRate()+ w4*p.NumShips());
  
  return heuristic;
}

struct compare_enemy{
bool operator()(const Planet& p1, const Planet& p2)
{
    // a<<p1.NumShips()<<" "<<p2.NumShips()<<endl;
    int h1= attack_heuristic(p1);
    int h2= attack_heuristic(p2);

    if(h1<h2){
      // a<<p1.NumShips()<<" "<<p2.NumShips()<<endl;
      return false;
    }
    else {
      // a<<p1.NumShips()<<" "<<p2.NumShips()<<endl;
      return true;
    }
}
};

int maxDistance(const PlanetWars& pw)
{
    int dist=-1;
    vector<Planet> planets = pw.Planets();
    for(int i=0;i<planets.size();i++)
    {
        for(int j=0;j<planets.size();j++)
        {
            if(dist<pw.Distance(planets[i].PlanetID(),planets[j].PlanetID()))
            {
                dist = pw.Distance(planets[i].PlanetID(),planets[j].PlanetID());
            }
        }
    }

    return dist;
}

int maxHelp(const Planet& p)
{
  int x=0;
  Planet temp=p;
  //  ofstream t;
  // t.open("while.txt");
  while(finalShips(temp)>=0 && x<p.NumShips()){
    temp.AddShips(-6);
    x+=6;
    // t<<x<<" ";
  }
  // t<<endl;
   x=max(0,x-6);
  int y = p.NumShips()-x;
  
  return max(0,x-(max_growth-p.GrowthRate()+1)*y/((max_growth-min_growth+1)) );
}

int neededToAttack(const Planet& p, int distance)
{
    int nta=0;
    // ofstream t;
    // t.open("while1.txt");
    for(int i=0;i<enemyPlanets.size();i++)
    {
        if(Distance(p,enemyPlanets[i]) <= 0.2*max_distance && enemyPlanets[i].PlanetID()!=p.PlanetID())
        {
            int maxhelp = maxHelp(enemyPlanets[i]);
            // t<<enemyPlanets[i].PlanetID()<<" "<<enemyPlanets[i].NumShips()<<endl;
            if(maxhelp>0)
            {
                nta+=maxhelp;
            }

            else 
            {
                nta+=enemyPlanets[i].NumShips();
            }
        }
    }

  int x=0;
  vector<Fleet> f = fleets;
  Fleet temp(1,0,1,p.PlanetID(),0,0);
  fleets.push_back(temp);
  // t<<fleets.size()<<endl;
  // t<<"Number of ships "<<p.NumShips()<<endl;
  int z=finalShips(p);
  // t<<"z "<<z<<endl;
  // t<<"nta "<<nta<<endl;
  while(z<=0)
  {
    Fleet temp(1,x+6,1,p.PlanetID(),distance,distance);
    fleets[fleets.size()-1]=temp;
    x+=6;
    // t<<x<<endl;
    // t<<"fleets send "<<fleets[fleets.size()-1].NumShips()<<" "<<temp.NumShips()<<endl;
    // t<<"final "<<finalShips(p)<<" "<<fleets.size()<<endl;
    z=finalShips(p);
  }

  fleets=f;
  nta+=x;

  return nta;
}

int neededToDefend(const Planet& p, int distance)
{
    return neededToAttack(p,distance);
}

int neededToNeutral(const Planet& p, int distance)
{
    return neededToAttack(p,distance);
}

void attack(const PlanetWars& pw)
{
  vector<Planet> donorPlanets;
  vector<Planet> myPlanetsNotTouched;
  vector<Fleet> tempFleet = fleets;
  // ofstream t1;
  // t1.open("finalships1.txt");
  for(int i=0;i<myPlanets.size();i++)
  {
      int final_ships = finalShips(myPlanets[i]);
      // t1<<"final ships "<<final_ships<<endl;

      if(final_ships>0)donorPlanets.push_back(myPlanets[i]);
      else myPlanetsNotTouched.push_back(myPlanets[i]);
  }
  // ofstream t;
  // t.open("attack.txt");
  // t<<fleets.size()<<endl;
  // for(int i=0;i<enemyPlanets.size();i++){
  //   // t<<"Number of ships "<<enemyPlanets[i].NumShips()<<endl;
  // }
  // sort(enemyPlanets.begin(), enemyPlanets.end(), compare_enemy());
//   for(int i=0;i<enemyPlanets.size();i++){
//   // t<<"attack_heuristic "<<attack_heuristic(enemyPlanets[i])<<" "<<enemyPlanets[i].GrowthRate()<<" "<<enemyPlanets[i].NumShips()<<endl;
// }
  for(int i=0;i<enemyPlanets.size();i++){
    
    for(int j=i+1;j<enemyPlanets.size();j++){
      if(attack_heuristic(enemyPlanets[i])<attack_heuristic(enemyPlanets[j])){
        Planet temp=enemyPlanets[i];
        enemyPlanets[i]=enemyPlanets[j];
        enemyPlanets[j]=temp;
      }
    }
  }

//   for(int i=0;i<enemyPlanets.size();i++){
//   t<<"attack_heuristic "<<attack_heuristic(enemyPlanets[i])<<endl;
// }
//   for(int i=0;i<enemyPlanets.size();i++){
//     t<<"Number of ships "<<enemyPlanets[i].NumShips()<<endl;
//   }
  for(int i=0;i<enemyPlanets.size();i++)
  {
      // t<<"fletts "<<fleets.size()<<endl;
      //sort donor according to distance with enemyPlanet
      // t<<"Number of ships "<<enemyPlanets[i].NumShips()<<endl;
      // sort(donorPlanets.begin(), donorPlanets.end(), compare_donors(enemyPlanets[i]));   //experiment area
      for(int k=0;k<donorPlanets.size();k++){
        for(int j=k+1;j<donorPlanets.size();j++){
          if(Distance(donorPlanets[k],enemyPlanets[i])>Distance(donorPlanets[j],enemyPlanets[i])){
            Planet temp=donorPlanets[k];
            donorPlanets[k]=donorPlanets[j];
            donorPlanets[j]=temp;
          }
        }
      }
      
      vector<pair<int,int> > num_attack_ships;
      int neededForAttack;
      // t<<"enemy enemy"<<endl;
      for(int j=0; j<donorPlanets.size();j++)
      {
          // t<<"fletts: "<<fleets.size()<<endl;
          int maxhelp = maxHelp(donorPlanets[j]);
          vector<Fleet> tmp = fleets;
          fleets = tempFleet;
          int distance = Distance(enemyPlanets[i],donorPlanets[j]);
          // t<<"Number of ships "<<enemyPlanets[i].NumShips()<<endl;
          // // t<<"fletts: "<<fleets.size()<<endl;
          // for(int k=0;k<fleets.size();k++){
          //   t<<"size "<<fleets[k].NumShips()<<endl;
          // }
          
          neededForAttack = neededToAttack(enemyPlanets[i],distance);

          // t<<"neededForAttack "<<neededForAttack<<" "<<donorPlanets[j].NumShips()<<" "<<maxhelp<<endl;
          // t<<"EnemyPlanets "<<enemyPlanets.size()<<endl;
          
          // if( donorPlanets.size()==1 && donorPlanets[0].NumShips()==32){
          //   while(true){}
          // }
          if(maxhelp >= neededForAttack)
          {
            if(neededForAttack!=0){
              //t<<"neededForAttack maxhelp "<<neededForAttack<<" "<<donorPlanets[j].NumShips()<<endl;
              num_attack_ships.push_back(make_pair(j,neededForAttack));
              tempFleet.push_back(Fleet(1,neededForAttack,donorPlanets[j].PlanetID(),enemyPlanets[i].PlanetID(),distance,distance));
            }
              // fleets=tempFleet;
              // break;
              for(int k=0;k<num_attack_ships.size();k++)
              {
                  pw.IssueOrder(donorPlanets[num_attack_ships[k].first].PlanetID(),enemyPlanets[i].PlanetID(),num_attack_ships[k].second);
                  donorPlanets[num_attack_ships[k].first].AddShips(-num_attack_ships[k].second);
              }
              fleets = tempFleet;

              
          // if( i==1 &&j==1 &&donorPlanets.size()==2 && donorPlanets[0].NumShips()==1 && donorPlanets[1].NumShips()==46){
          //   while(true){}
          // }
              
              break;
          }

          else 
          {
            if(maxhelp!=0)
            {
               num_attack_ships.push_back(make_pair(j,maxhelp));
              tempFleet.push_back(Fleet(1,maxhelp,donorPlanets[j].PlanetID(),enemyPlanets[i].PlanetID(),distance,distance));
              fleets=tmp;
            }
          }   
      }
  }

  for(int i=0;i<myPlanetsNotTouched.size();i++)
  {
      donorPlanets.push_back(myPlanetsNotTouched[i]);
  }

  myPlanets = donorPlanets;

}


void defence(const PlanetWars& pw)
{
  vector<Planet> donorPlanets;
  vector<Planet> dangerPlanets;
  vector<Fleet> tempFleet = fleets;

  for(int i=0;i<myPlanets.size();i++)
  {
      int final_ships = finalShips(myPlanets[i]);

      if(final_ships>0)donorPlanets.push_back(myPlanets[i]);
      else dangerPlanets.push_back(myPlanets[i]);
  }

  for(int i=0;i<dangerPlanets.size();i++){
    for(int j=i+1;j<dangerPlanets.size();j++){
      if(defence_heuristic(dangerPlanets[i])<defence_heuristic(dangerPlanets[j])){
        Planet temp=dangerPlanets[i];
        dangerPlanets[i]=dangerPlanets[j];
        dangerPlanets[j]=temp;
      }
    }
  }

  for(int i=0; i<dangerPlanets.size();i++)
  {

    for(int k=0;k<donorPlanets.size();k++){
        for(int j=k+1;j<donorPlanets.size();j++){
          if(Distance(donorPlanets[k],dangerPlanets[i])>Distance(donorPlanets[j],dangerPlanets[i])){
            Planet temp=donorPlanets[k];
            donorPlanets[k]=donorPlanets[j];
            donorPlanets[j]=temp;
          }
        }
      }
      
    vector<pair<int,int> > num_defence_ships;
    int neededForDefence;

    for(int j=0;j<donorPlanets.size();j++)
    {
          int maxhelp = maxHelp(donorPlanets[j]);
          vector<Fleet> tmp = fleets;
          fleets = tempFleet;
          int distance = Distance(dangerPlanets[i],donorPlanets[j]);
          
          neededForDefence = neededToDefend(dangerPlanets[i],distance);

          if(maxhelp >= neededForDefence)
          {
            if(neededForDefence!=0){
              //t<<"neededForAttack maxhelp "<<neededForAttack<<" "<<donorPlanets[j].NumShips()<<endl;
              num_defence_ships.push_back(make_pair(j,neededForDefence));
              tempFleet.push_back(Fleet(1,neededForDefence,donorPlanets[j].PlanetID(),dangerPlanets[i].PlanetID(),distance,distance));
            }
              // fleets=tempFleet;
              // break;
              for(int k=0;k<num_defence_ships.size();k++)
              {
                  pw.IssueOrder(donorPlanets[num_defence_ships[k].first].PlanetID(),dangerPlanets[i].PlanetID(),num_defence_ships[k].second);
                  donorPlanets[num_defence_ships[k].first].AddShips(-num_defence_ships[k].second);
              }
              fleets = tempFleet;

              
          // if( i==1 &&j==1 &&donorPlanets.size()==2 && donorPlanets[0].NumShips()==1 && donorPlanets[1].NumShips()==46){
          //   while(true){}
          // }
              
              break;
          }

          else 
          {
              if(maxhelp!=0)
              {
                  num_defence_ships.push_back(make_pair(j,maxhelp));
                  tempFleet.push_back(Fleet(1,maxhelp,donorPlanets[j].PlanetID(),dangerPlanets[i].PlanetID(),distance,distance));
                  fleets=tmp;
              }
          }   
    }
  }

  for(int i=0;i<dangerPlanets.size();i++)
  {
      donorPlanets.push_back(dangerPlanets[i]);
  }

  myPlanets = donorPlanets;

}


void neutralHunting(const PlanetWars& pw)
{ 
  vector<Planet> donorPlanets;
  vector<Planet> notTouchedMyPlanets;
  vector<Fleet> tempFleet = fleets;

  for(int i=0;i<myPlanets.size();i++)
  {
      int final_ships = finalShips(myPlanets[i]);

      if(final_ships>0)donorPlanets.push_back(myPlanets[i]);
      else notTouchedMyPlanets.push_back(myPlanets[i]);
  }

  for(int i=0;i<neutralPlanets.size();i++){
    for(int j=i+1;j<neutralPlanets.size();j++){
      if(neutral_heuristic(neutralPlanets[i])<neutral_heuristic(neutralPlanets[j])){
        Planet temp=neutralPlanets[i];
        neutralPlanets[i]=neutralPlanets[j];
        neutralPlanets[j]=temp;
      }
    }
  }

  for(int i=0; i<neutralPlanets.size();i++)
  {

    for(int k=0;k<donorPlanets.size();k++){
        for(int j=k+1;j<donorPlanets.size();j++){
          if(Distance(donorPlanets[k],neutralPlanets[i])>Distance(donorPlanets[j],neutralPlanets[i])){
            Planet temp=donorPlanets[k];
            donorPlanets[k]=donorPlanets[j];
            donorPlanets[j]=temp;
          }
        }
      }
      
    vector<pair<int,int> > num_neutral_ships;
    int neededForneutral;

    for(int j=0;j<donorPlanets.size();j++)
    {
          int maxhelp = maxHelp(donorPlanets[j]);
          vector<Fleet> tmp = fleets;
          fleets = tempFleet;
          int distance = Distance(neutralPlanets[i],donorPlanets[j]);
          
          neededForneutral = neededToNeutral(neutralPlanets[i],distance);

          if(maxhelp >= neededForneutral)
          {
            if(neededForneutral!=0){
              //t<<"neededForAttack maxhelp "<<neededForAttack<<" "<<donorPlanets[j].NumShips()<<endl;
              num_neutral_ships.push_back(make_pair(j,neededForneutral));
              tempFleet.push_back(Fleet(1,neededForneutral,donorPlanets[j].PlanetID(),neutralPlanets[i].PlanetID(),distance,distance));
            }
              // fleets=tempFleet;
              // break;
              for(int k=0;k<num_neutral_ships.size();k++)
              {
                  pw.IssueOrder(donorPlanets[num_neutral_ships[k].first].PlanetID(),neutralPlanets[i].PlanetID(),num_neutral_ships[k].second);
                  donorPlanets[num_neutral_ships[k].first].AddShips(-num_neutral_ships[k].second);
              }
              fleets = tempFleet;

              
          // if( i==1 &&j==1 &&donorPlanets.size()==2 && donorPlanets[0].NumShips()==1 && donorPlanets[1].NumShips()==46){
          //   while(true){}
          // }
              
              break;
          }

          else 
          {
              if(maxhelp!=0)
              {
                  num_neutral_ships.push_back(make_pair(j,maxhelp));
                  tempFleet.push_back(Fleet(1,maxhelp,donorPlanets[j].PlanetID(),neutralPlanets[i].PlanetID(),distance,distance));
                  fleets=tmp;
              }
          }   
    }
  }

  for(int i=0;i<notTouchedMyPlanets.size();i++)
  {
      donorPlanets.push_back(notTouchedMyPlanets[i]);
  }

  myPlanets = donorPlanets;

}

int startingNeutralHeuristic(const Planet& p)
{
    int w1=6,w2=-3,w3=-5;
    return (w1*p.GrowthRate()+ w2*p.NumShips() + w3*Distance(p,myPlanets[0]));

}

void initialNeutralHunting(const PlanetWars& pw)
 {
	ofstream f;
	// f.open("n.txt");
    for(int i=0;i<neutralPlanets.size();i++){
        for(int j=i+1;j<neutralPlanets.size();j++){
          if(startingNeutralHeuristic(neutralPlanets[i]) < startingNeutralHeuristic(neutralPlanets[j])){
            Planet temp=neutralPlanets[i];
            neutralPlanets[i]=neutralPlanets[j];
            neutralPlanets[j]=temp;
          }
        }
      }

    int maxhelp=0;
     if(Distance(myPlanets[0], enemyPlanets[0])*myPlanets[0].GrowthRate() >= enemyPlanets[0].NumShips()){
     	maxhelp=myPlanets[0].NumShips()-1;
     }
     else{
     	maxhelp=Distance(myPlanets[0], enemyPlanets[0])*myPlanets[0].GrowthRate();
     }
    // f<<maxhelp<<endl;
    for(int i=0;i<neutralPlanets.size();i++)
    {
      if(Distance(neutralPlanets[i],myPlanets[0])<Distance(neutralPlanets[i],enemyPlanets[0])){
        int neededToNeutralAttack = neutralPlanets[i].NumShips()+1;
        // f<<maxhelp<<" "<<neededToNeutralAttack<<endl;
        if(maxhelp >= neededToNeutralAttack)
        {
        	// f<<"Issue "<<neededToNeutralAttack<<" "<<neutralPlanets[i].NumShips()<<endl;
            pw.IssueOrder(myPlanets[0].PlanetID(),neutralPlanets[i].PlanetID(),neededToNeutralAttack);
            myPlanets[0].AddShips(-neededToNeutralAttack);
            maxhelp-=neededToNeutralAttack;
        }
      }
    }
}

void sniping(const PlanetWars& pw)
{
    int snipingDistance=2;
    vector<Planet> donorPlanets;
    vector<Planet> notTouchedMyPlanets;
    vector<Fleet> tempFleet = fleets;

    for(int i=0;i<myPlanets.size();i++)
    {
        int final_ships = finalShips(myPlanets[i]);

        if(final_ships>0)donorPlanets.push_back(myPlanets[i]);
        else notTouchedMyPlanets.push_back(myPlanets[i]);
    }

    for(int i=0;i<neutralPlanets.size();i++){
      for(int j=i+1;j<neutralPlanets.size();j++){
        if(neutral_heuristic(neutralPlanets[i])<neutral_heuristic(neutralPlanets[j])){
          Planet temp=neutralPlanets[i];
          neutralPlanets[i]=neutralPlanets[j];
          neutralPlanets[j]=temp;
        }
      }
    }
    for(int i=0;i<neutralPlanets.size();i++){
      int no_of_fleets=0;
      int turns_remaining=0;
      int no_of_ships;
      for(int j=0;j<fleets.size();j++){
        if(fleets[j].DestinationPlanet()==neutralPlanets[i].PlanetID() && fleets[j].Owner()==2){
          no_of_fleets++;
          turns_remaining=fleets[j].TurnsRemaining();
          no_of_ships=fleets[j].NumShips();
        }
        else if(fleets[j].Owner()==1) no_of_fleets++;
      }
      vector<Planet> snipingPlanets;
      if(no_of_fleets==1 && no_of_ships>neutralPlanets[i].NumShips()){
        for(int j=0;j<donorPlanets.size();j++){
          if(Distance(neutralPlanets[i],donorPlanets[j])>turns_remaining && Distance(neutralPlanets[i],donorPlanets[j])<=turns_remaining+snipingDistance){
            snipingPlanets.push_back(donorPlanets[j]);
          }
        }
        vector<pair<int,int> > num_snipe_ships;
        int neededForSnipe;
        for(int j=0;j<snipingPlanets.size();j++){
          int maxhelp = maxHelp(snipingPlanets[j]);
          vector<Fleet> tmp = fleets;
          fleets = tempFleet;
          int distance = Distance(neutralPlanets[i],snipingPlanets[j]);
          neededForSnipe = -finalShips(neutralPlanets[i])+1+(distance-turns_remaining)*neutralPlanets[i].GrowthRate();
          if(maxhelp >= neededForSnipe)
          {
            if(neededForSnipe!=0){
              //t<<"neededForAttack maxhelp "<<neededForAttack<<" "<<donorPlanets[j].NumShips()<<endl;
              num_snipe_ships.push_back(make_pair(j,neededForSnipe));
              tempFleet.push_back(Fleet(1,neededForSnipe,snipingPlanets[j].PlanetID(),neutralPlanets[i].PlanetID(),distance,distance));
            }
              // fleets=tempFleet;
              // break;
              for(int k=0;k<num_snipe_ships.size();k++)
              {
                  pw.IssueOrder(snipingPlanets[num_snipe_ships[k].first].PlanetID(),neutralPlanets[i].PlanetID(),num_snipe_ships[k].second);
                  snipingPlanets[num_snipe_ships[k].first].AddShips(-num_snipe_ships[k].second);
              }
              fleets = tempFleet;

              
          // if( i==1 &&j==1 &&donorPlanets.size()==2 && donorPlanets[0].NumShips()==1 && donorPlanets[1].NumShips()==46){
          //   while(true){}
          // }
              
              break;
          }

          else 
          {
              if(maxhelp!=0)
              {
                  num_snipe_ships.push_back(make_pair(j,maxhelp));
                  tempFleet.push_back(Fleet(1,maxhelp,snipingPlanets[j].PlanetID(),neutralPlanets[i].PlanetID(),distance,distance));
                  fleets=tmp;
              }
          }
        }
      }
    }
    for(int i=0;i<notTouchedMyPlanets.size();i++)
  {
      donorPlanets.push_back(notTouchedMyPlanets[i]);
  }

  myPlanets = donorPlanets;
}

void DoTurn(const PlanetWars& pw)
{
  // a.open("heuristic.txt");
  myPlanets = pw.MyPlanets();
  enemyPlanets = pw.EnemyPlanets();
  neutralPlanets = pw.NeutralPlanets();
  myFleets = pw.MyFleets();
  fleets = pw.Fleets();

  max_distance = maxDistance(pw);
  max_growth=-1; min_growth=myPlanets[0].GrowthRate();

    for(int i=0;i<pw.Planets().size();i++)
    {
        if(pw.Planets()[i].GrowthRate() > max_growth)max_growth=pw.Planets()[i].GrowthRate();
        if(pw.Planets()[i].GrowthRate() < min_growth)min_growth=pw.Planets()[i].GrowthRate();
    }

bool intial=false;
if(pw.NumShips(1)==pw.NumShips(2) && pw.MyPlanets().size()==1 && pw.EnemyPlanets().size()==1 && pw.Fleets().size()==0)
  {
  	initialNeutralHunting(pw);
  	intial=true;
  }

    if(pw.NumShips(1)>=1.5*pw.NumShips(2))
    {
         attack(pw);
         defence(pw);
    }
    else 
    {
         defence(pw);
         attack(pw);
    }

    //sniping(pw);
    
  if(!intial) neutralHunting(pw);
    sniping(pw);
  // }
}

// This is just the main game loop that takes care of communicating with the
// game engine for you. You don't have to understand or change the code below.
int main(int argc, char *argv[]) {
  std::string current_line;
  std::string map_data;
  while (true) {
    int c = std::cin.get();
    current_line += (char)c;
    if (c == '\n') {
      if (current_line.length() >= 2 && current_line.substr(0, 2) == "go") {
        PlanetWars pw(map_data);
        map_data = "";
        DoTurn(pw);
	pw.FinishTurn();
      } else {
        map_data += current_line;
      }
      current_line = "";
    }
  }
  return 0;
}
