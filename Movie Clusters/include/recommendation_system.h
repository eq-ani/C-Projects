#ifndef __RECOMMENDATION_SYSTEM
#define __RECOMMENDATION_SYSTEM

typedef struct Cluster Cluster;
typedef struct Cluster_Movies Cluster_Movies;
typedef struct Parameters Parameters;
typedef struct Movie Movie;

struct Parameters {
  int a;
  int b;
};

// Represents a Cluster.
struct Cluster {
  int cluster_id;
  Parameters cluster_parameters;
  Cluster_Movies* list;
};

// Represents a movie in the linked list (Cluster_Movies).
struct Cluster_Movies {
  Movie* movie;
  Cluster_Movies* next;
};

// Represents a Movie with its name, parameter, and cluster id.
struct Movie {
  char* movie_name;
  int cluster_id;
  Parameters movie_parameters;
};

int calculate_distance(Parameters cluster_parameters,
                       Parameters movie_parameters);
int add_movies(Cluster cluster[], int number_of_clusters, Movie movies[],
               int number_of_movies);
Movie* remove_movie(Cluster cluster[], int number_of_clusters, Movie* movie);
int move_movie_to_cluster(int cluster_id, Cluster cluster[],
                          int number_of_clusters, Movie* movie);
Cluster* k_means_clustering(Cluster cluster[], int number_of_clusters,
                            Movie movies[], int number_of_movies);

#endif
