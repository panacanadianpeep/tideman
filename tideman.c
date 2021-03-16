//including libaries or c header files
#include <cs50.h>
#include <stdio.h>
#include <string.h>

// Max number of candidates
#define MAX 9

// preferences[i][j] is number of voters who prefer i over j
int preferences[MAX][MAX];

// locked[i][j] means i is locked in over j
bool locked[MAX][MAX];

// Each pair has a winner, loser
typedef struct
{
    int winner;
    int loser;
}
pair;

// Array of candidates
string candidates[MAX];
pair pairs[MAX * (MAX - 1) / 2];

int pair_count;
int candidate_count;

// Function prototypes
bool vote(int rank, string name, int ranks[]);
void record_preferences(int ranks[]);
void add_pairs(void);
void sort_pairs(void);
void lock_pairs(void);
bool check_pairs_bool(int i, pair current_item, int nTimesRan);
bool check_pairs_recursion(int i, int h, pair current_item, int nTimesRan);
void print_winner(void);
int print_winner_loop();

int main(int argc, string argv[])
{
    // Check for invalid usage
    if (argc < 2)
    {
        printf("Usage: tideman [candidate ...]\n");
        return 1;
    }

    // Populate array of candidates
    candidate_count = argc - 1;
    if (candidate_count > MAX)
    {
        printf("Maximum number of candidates is %i\n", MAX);
        return 2;
    }
    for (int i = 0; i < candidate_count; i++)
    {
        candidates[i] = argv[i + 1];
    }

    // Clear graph of locked in pairs
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            locked[i][j] = false;
        }
    }

    pair_count = 0;
    int voter_count = get_int("Number of voters: ");

    // Query for votes
    for (int i = 0; i < voter_count; i++)
    {
        // ranks[i] is voter's ith preference
        int ranks[candidate_count];

        // Query for each rank
        for (int j = 0; j < candidate_count; j++)
        {
            string name = get_string("Rank %i: ", j + 1);

            if (!vote(j, name, ranks))
            {
                printf("Invalid vote.\n");
                return 3;
            }
        }

        record_preferences(ranks);

        printf("\n");
    }

    add_pairs();
    sort_pairs();
    lock_pairs();
    print_winner();
    return 0;
}

// Update ranks given a new vote
bool vote(int rank, string name, int ranks[])
{
    //Check for number of candidates if candidates is equal to name
    for (int i = 0; i < candidate_count; i++)
    {
        if (strcmp(candidates[i], name) == 0)
        {
            //if check is equal to true, update ranks and return true
            ranks[rank] = i;
            return true;
        }
    }
    //else return false
    return false;
}

// Update preferences given one voter's ranks
void record_preferences(int ranks[])
{
    //initalising variables
    int ranki;
    int rankj;
    int counter = 1;
    int ccount = candidate_count;
    
    //running number of candidates squared, if i is equal to j, update preferences
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            if (i < j)
            {
                preferences[ranks[i]][ranks[j]]++;
            }
        }
    }
    return;
}

// Record pairs of candidates where one is preferred over the other
void add_pairs(void)
{
    //initalisng variables
    int counter = 1;
    int h = 0;
    
    // add two counters, i and j, to represent number of candidates - 1 and number of candidates
    
    for (int i = 0; i < (candidate_count - 1) ; i++)
    {
        for (int j = counter; j < candidate_count; j++)
        {
            // if preferences ith row and jth column is less than the jth row and column, update winner to be j and loser to be i
            if (preferences[i][j] < preferences[j][i])
            {
                pairs[h].winner = j;
                pairs[h].loser = i;
                h++;
            }
            // if preferences ith row and jth column is more than the jth row and column, update winner to be i and loser to be j
            else if (preferences[i][j] > preferences[j][i])
            {
                pairs[h].winner = i;
                pairs[h].loser = j;
                h++;
            }
        }
        counter++;
    }

    pair_count = h;
    
    return;
}

// Sort pairs in decreasing order by strength of victory
void sort_pairs(void)
{
    //intisalise variables
    bool sorted = false;

    pair tempPair;
    
    //bubble sort algorithm
    
    while (sorted == false)
    {
        sorted = true;
        for (int i = 0; i < pair_count; i++)
        {
            if (preferences[pairs[i].winner][pairs[i].loser] < preferences[pairs[i + 1].winner][pairs[i + 1].loser])
            {
                tempPair = pairs[i + 1];
                pairs[i + 1] = pairs[i];
                pairs[i] = tempPair;
                sorted = false;
            }
        }
    }
}

// Lock pairs into the candidate graph in order, without creating cycles
void lock_pairs(void)
{
    
    //initalising variables
    int nTimesRan;

    //for every pair, do this

    for (int i = 0; i < pair_count; i++)
    {
        //initalising more variables
        pair current = pairs[i];

        nTimesRan = 0;
        
        //if return value for check pairs bool is equal to true, update locked to winner
        
        if (check_pairs_bool(i, current, nTimesRan) == true)
        {
            locked[pairs[i].winner][pairs[i].loser] = true;
        }
    }
    return;
}

bool check_pairs_bool(int i, pair current_item, int nTimesRan)
{
    //intisalise variables
    bool IsCycle = true;

    int h = current_item.loser;

    //do recursion for algorithm

    IsCycle = check_pairs_recursion(i, h, current_item, nTimesRan);

    if (IsCycle == true)
    {
        return false;
    }

    return true;
}

bool check_pairs_recursion(int i, int h, pair current_item, int nTimesRan)
{
    
    //initalise variables
    bool IsCycle = false;

    //do this candidate count times and set counter k
    
    for (int k = 0; k < candidate_count; k++)
    {
        //if k is equal current pair loser and h(k from the previous loop or the current pair's loser))
        if (k == current_item.loser && h == current_item.winner)
        {
            //set IsCycle = true and return back
            IsCycle = true;
            return true;
        }
        
        //else
        else
        {
            //if locked h row k column is equal true, set counter nTimesRna + 1 and do this function again(aka recursion) and as soon as recursion finish, exit
            if (locked[h][k] == true)
            {
                nTimesRan++;
                IsCycle = check_pairs_recursion(i, k, current_item, nTimesRan);
                nTimesRan--;
            }
        }

        //If IsCycle == true, return true

        if (IsCycle == true)
        {
            return true;
        }
    }
    return false;
}

// Print the winner of the election
void print_winner(void)
{
    //initalise variables
    
    int count = 0;
    bool IsItTrue = true;
    int candidate_index = 0;
    
    //do for number of candidates times, and have counter i
    
    for (int i = 0; i < candidate_count; i++)
    {
        //do for candidate count times and have counter j and if locked jth column ith row is equal true, set IsItTrue to false
        
        for (int j = 0; j < candidate_count; j++)
        {
            if (locked[j][i] == true)
            {
                IsItTrue = false;
            }
        }
        
        //IfIsTrue is equal to true, print candidates
        
        if (IsItTrue == true)
        {
            printf("%s", candidates[i]);
        }
        
        //reset is it true
        
        IsItTrue = true;
    }
    //print new line
    printf("\n");
    return;
}