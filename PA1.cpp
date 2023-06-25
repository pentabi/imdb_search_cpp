/*
 * Course: CS216
 * Project: Project 1
 * Purpose: it reads data from an input file which passes as command line argument
 *          then stores in an object of IMDB class (to represent imdb database)
 *          two movies are from the user-input, then performs case insensitive matching
 *          and match the movie titles from the database
 *          it then allowes user to input an actor/actress name, to search for co-actors in each movie which this actor/actress was in, in a well-formatted display
 * Author: Tabito Satoh
 */
#include <iostream>
#include <limits>
#include <fstream>
#include <string>
#include <cctype>
#include <set>
#include <map>
#include <sstream>
#include "imdb.h"

using namespace std;

void analyzeCoActors(const IMDB& imdb);


// this function asks the user to input two movie titles
// then finds the matched two movies from the database
// and displays all the actors in either of the two movies
// if no matched movie then display "Invalid movie title" and do nothing
// note that using const call by reference is for efficiency purpose:
//           no copy of IMDB object is made and the argument has been protected
void analyzeMovies(const IMDB& imdb)
{
    // Ask the user to input two movie titles to search for
    string movie1, movie2;
    cout << "Please input the first movie title: ";
    getline(cin,movie1);
    cout << "Please input the second movie title: ";
    getline(cin,movie2);
    // Check if both movie1 and movie2 can be matched with movies in the database
    // If either one is not, quit the program
    // Otherwise display all the actors in either of two movies
    //                   which are actually the union of two sets
    string matchedMovie1, matchedMovie2;
    matchedMovie1 = imdb.matchExistingMovie(movie1);
    matchedMovie2 = imdb.matchExistingMovie(movie2);
    // decide which one movie title is not valid
    bool notValid = false;
    if (matchedMovie1.length() == 0)
    {
        cout << "\" " << movie1 << " \"" << " is not a VALID movie title!" << endl;
        notValid = true;
    }
    if (matchedMovie2.length() == 0)
    {
        cout << "\" " << movie2 << " \"" << " is not a VALID movie title!" << endl;
        notValid = true;
    }
    if (notValid)
        return;
    // both movie titles are valid
    if (imdb.isExistingMovie(matchedMovie1) && imdb.isExistingMovie(matchedMovie2))
    {
        cout << "Your input matches the following two movies: " << endl;
        cout << matchedMovie1 << endl;
        cout << matchedMovie2 << endl;
        cout << "Both movies are in the database, please continue..." << endl;

        string menu_option;
        while(true){
            cout << "Please input your menu option (enter Q or q to quit)" << endl;
            cout << "A -- to print all the actors in either of the two movies." << endl;
            cout << "C -- to print all the common actors in both of the movies."  << endl;
            cout << "O -- to print all the actors who are in only one movie, but not in both." << endl;
            //if there is error here it's probably bc you need ignore
            cin >> menu_option;
            //make the menu_option input lower case
            for(int i = 0; i < menu_option.length(); i++) menu_option[i] = tolower(menu_option[i]);

            //make set of union, intersect, and symmetric_difference
            set<string> all;   // to store the union of actors_in_movie1 and actors_in_movie2
            all = imdb.find_actors_in_a_movie(matchedMovie1);
            set<string> actors_in_movie2 = imdb.find_actors_in_a_movie(matchedMovie2);
            set<string> actors_in_movie1 = imdb.find_actors_in_a_movie(matchedMovie1);
            for (auto i = actors_in_movie2.begin(); i != actors_in_movie2.end(); i++)
            {
                if (imdb.isExistingActor(*i))
                    all.insert(*i);
                else
                    cout << "isExistingActor function is not correct!!!" << endl;
            }

            set<string> inter; // to store intersection of actors_in_movie1 and actors_in_movie2
            for(auto it = actors_in_movie1.begin(); it != actors_in_movie1.end(); it++)
            {
                if (actors_in_movie2.find(*it)!= actors_in_movie2.end()){
                    if (imdb.isExistingActor(*it))
                    {
                        inter.insert(*it);
                    }
                }
            }

            set<string> sd; 
            for(auto it = all.begin(); it != all.end(); it++){
                if(inter.find(*it) == inter.end()){
                    sd.insert(*it);
                }
            }


            //The options
            if(menu_option == "a"){
                
                // Display all the actors in either one movie
                cout << "All the actors in either of the two movies:" << endl;
                for (auto i = all.begin(); i != all.end(); i++)
                    cout << *i << endl;

            }else if(menu_option == "c"){
                // Display common actors
                cout << "Common actors in both movies:" << endl;
                for (auto i = inter.begin(); i != inter.end(); i++)
                    cout << *i << endl;
            }else if(menu_option == "o"){
                // Display Actors only in one of two movies:
                cout << "Actors only in one of two movies:" << endl;
                for (auto i = sd.begin(); i != sd.end(); i++)
                    cout << *i << endl;
            }else if(menu_option == "q"){
                // goes back to selecting 1 or 2
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                break;
            }else{
                // Invalid option
                cout << "Invalid Option!" << endl;
            }
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }
    else
    {
        cout << "isExistingMovie function is not correct!!!" << endl;
        return;
    }
}

/*
 * Purpose: this function asks the user to type an actor's name
 * then check if the user-input name is valid(it is valid if it is in the database stored in the passing in object
 * if it is valid, display each movie which this actor is in, the co-actors who appeared in the same movie
 *@param imdb, the object of IMDB class: stores all information to look up (const call by reference)
 *@return void function
 */
void analyzeCoActors(const IMDB& imdb)
{
    string actor_name;
    cout << "Finding the co-actors of the actor by typing his/her name: ";
    getline(cin, actor_name);
    if (!imdb.isExistingActor(actor_name))
    {
        cout << "The actor name you entered is not in the database." << endl;
        return;
    }   

    set<string> movies_of_actor;
    movies_of_actor = imdb.find_movies_for_an_actor(actor_name);
    for (set<string>::iterator im = movies_of_actor.begin(); im != movies_of_actor.end(); im++)
    {
        cout << "The co-actors of " << actor_name << " in the movie \"" << *im << "\" are: " << endl;
        // Display all the co-actors in one movie
        set<string> coactors = imdb.find_actors_in_a_movie(*im);
        for (set<string>::iterator ia = coactors.begin(); ia != coactors.end(); ia++)
        {
            if(*ia != actor_name)
                cout << *ia << endl;
        }    
        cout << "***********************************" << endl;
    }
}    


int main(int argc, char* argv[])
{
    // Check whether the number of command line arguments is exactly one 
    if (argc != 2)
    {
        cout << "Warning: need exactly one command line argument." << endl;
        cout << "Usage: " << argv[0] << " inputfile_name" << endl;
        return 1;
    }
    ifstream in_file;
    in_file.open(argv[1]);

    // Check whether the input file can be open successfully or not
    if (!in_file.good())
    {
        cout << "Warning: cannot open file named " << argv[1] << "!" << endl;
        return 2;
    }

    // Read data from the input file, assume "good data" from the input file
    // each line of the input file: actor name, movie1, movie2, ..., movien
    // read one line at a time then tokenize it:
    // create a pair <actor_name, the set of movieTitles which the actor was in>
    //        and insert to IMDB object
    IMDB cs216_imdb;

    while (!in_file.eof())
    {
        string line;
        getline(in_file, line);
        string actorName, movieTitle;
        istringstream iss(line);
        getline(iss, actorName,';');

        // Extract extra white space
        iss>>ws;

        // Create a new set of movies associated with name
        set<string> movies;

        while (getline(iss, movieTitle, ';'))
        {
            movies.insert(movieTitle);
            // extract white space 
            iss>>ws;
        }
        // insert the pair to IMDB object: cs216_imdb
        cs216_imdb.insert_an_actor(actorName, movies);
    }
    // close the input file
    in_file.close();

    int option;
    while (true)
    {
        bool validInput = true;
        cout << "This application stores information about Actors and their Movies, please choose your option (Enter Q or q to quit):" << endl;
        cout << "1. Actors in Movies" << endl;
        cout << "2. Actors and co-actors" << endl;
        cin >> option;

        //checking if the input is valid
        if (cin.fail())
        {
            cin.clear();
            string input_to_check;
            cin >> input_to_check;
            if (input_to_check == "Q" || input_to_check == "q")
                break;
            else
            {
                cout << input_to_check << endl;
                cout << "Expecting a number as the option!" << endl;
                validInput = false;
            }
        }
        // get rid of anything left in the input stream
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        

        // if there is a valid input option
        if (validInput)
        {
            switch (option)
            {
                case 1: analyzeMovies(cs216_imdb);
                        break;
                case 2: analyzeCoActors(cs216_imdb);
                        break;
                default:
                        cout << "Invalid option number!" << endl;
            }
        }
    }
    cout << "Thank you for using my program, bye..." << endl;
    return 0;
}


